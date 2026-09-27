#include "FrontierX/NativeWindowManager.h"

#include "FrontierX/LegacyInput.h"
#include "FrontierX/NativeStreamer.h"
#include "FrontierX/SdlWindow.h"
#include "SYSTEM.H"
#include "TComponent.h"
#include "TConnContainer.h"
#include "TConnPoint.h"
#include "WindowManager.h"

#include <cstring>
#include <limits>
#include <string>

namespace FrontierX
{
namespace
{
char interface_name[] = "IWindowManager";

class NativeWindowManager : public ISystemComponent,
                            public IWindowManager,
                            public ConnectionPointContainer<NativeWindowManager>
{
public:
    BEGIN_DACOM_MAP_INBOUND(NativeWindowManager)
    DACOM_INTERFACE_ENTRY(ISystemComponent)
    DACOM_INTERFACE_ENTRY(IAggregateComponent)
    DACOM_INTERFACE_ENTRY(IWindowManager)
    DACOM_INTERFACE_ENTRY(IDAConnectionPointContainer)
    DACOM_INTERFACE_ENTRY2(IID_ISystemComponent, ISystemComponent)
    DACOM_INTERFACE_ENTRY2(IID_IAggregateComponent, IAggregateComponent)
    DACOM_INTERFACE_ENTRY2(IID_IWindowManager, IWindowManager)
    DACOM_INTERFACE_ENTRY2(IID_IDAConnectionPointContainer, IDAConnectionPointContainer)
    END_DACOM_MAP()

    BEGIN_DACOM_MAP_OUTBOUND(NativeWindowManager)
    DACOM_INTERFACE_ENTRY_AGGREGATE("ISystemEventCallback", close_callbacks_)
    END_DACOM_MAP()

    NativeWindowManager() : close_callbacks_(0) {}
    ~NativeWindowManager()
    {
        window_.Close();
        if (live_window_ == this) live_window_ = nullptr;
    }

    GENRESULT init(AGGDESC * descriptor)
    {
        if (!descriptor || (descriptor->description &&
            std::strcmp(descriptor->description, "WindowManager") != 0))
            return GR_INTERFACE_UNSUPPORTED;
        return GR_OK;
    }

    GENRESULT Initialize() override { return GR_OK; }
    void Update() override { ServeMessageQueue(0); }

    S32 Startup(HINSTANCE instance, const C8 * app_name, WMEXITCB exit_handler,
                U32 flags, const tagWNDCLASSEXA * window_class) override
    {
        (void)instance;
        (void)window_class;
        if (window_.IsOpen()) return 1;
        if (!app_name || !app_name[0] || !exit_handler) return 0;
        if (live_window_ && live_window_ != this)
            return (flags & WMF_PREVENT_MULTIPLE) ? -1 : 0;

        std::string error;
        if (!window_.Open(app_name, 800, 600, true, error) ||
            !window_.StartTextInput(error))
        {
            window_.Close();
            return 0;
        }
        exit_callback_ = exit_handler;
        live_window_ = this;
        active_ = true;
        return 1;
    }

    BOOL32 SetWindowPos(U32 width, U32 height, U32 flags) override
    {
        if (!width || !height || width > static_cast<U32>(std::numeric_limits<int>::max()) ||
            height > static_cast<U32>(std::numeric_limits<int>::max())) return FALSE;
        std::string error;
        const bool configured = window_.Configure(
            static_cast<int>(width), static_cast<int>(height),
            (flags & WMF_FULL_SCREEN) != 0,
            (flags & WMF_ALLOW_WINDOW_RESIZE) != 0,
            (flags & WMF_ALWAYS_ON_TOP) != 0,
            (flags & WMF_SHOW) != 0,
            (flags & WMF_CENTER) != 0,
            error);
        return configured ? TRUE : FALSE;
    }

    void SetCallback(WNDPROC callback) override { window_callback_ = callback; }

    HWND GetWindowHandle() const override
    {
        return static_cast<HWND>(window_.WindowId());
    }

    void ServeMessageQueue(U32 flags) override
    {
        if (!window_.IsOpen()) return;

        UpdateNativeStreamers();
        const bool background = (flags & WMF_BACKGROUND) != 0;
        bool wait = (flags & WMF_WAIT) != 0 || (!background && !active_);
        std::string error;
        do
        {
            WindowEvent event;
            if (wait)
            {
                if (!window_.WaitEvent(event, error)) return;
                HandleEvent(event);
                wait = !background && !active_;
            }
            while (window_.PollEvent(event)) HandleEvent(event);
        } while (wait);
    }

    void GetWindowArea(WM_WINAREA & area) override { FillArea(area, false); }
    void GetClientArea(WM_WINAREA & area) override { FillArea(area, true); }
    BOOL32 AppIsActive() const override { return active_ ? TRUE : FALSE; }

    void SetCursorPos(S32 x, S32 y) override
    {
        cursor_x_ = x;
        cursor_y_ = y;
        window_.SetCursorPosition(x, y);
    }

    void GetCursorPos(S32 & x, S32 & y) override
    {
        x = cursor_x_;
        y = cursor_y_;
    }

    static bool HasMessageTarget(LegacyWindowToken window)
    {
        return live_window_ && live_window_->GetWindowHandle() == window &&
            live_window_->window_callback_;
    }
    static bool Dispatch(LegacyWindowToken window, LegacyMessageId message,
                         LegacyWParam wparam, LegacyLParam lparam)
    {
        if (!HasMessageTarget(window)) return false;
        live_window_->window_callback_(window, message, wparam, lparam);
        return true;
    }

private:
    bool CloseVetoed(CONNECTION_NODE<ISystemEventCallback> * node)
    {
        if (!node) return false;
        if (CloseVetoed(node->pNext)) return true;
        return node->client->OnAppClose() != FALSE;
    }

    void HandleEvent(const WindowEvent & event)
    {
        if (event.type == WindowEventType::FocusGained) active_ = true;
        if (event.type == WindowEventType::FocusLost) active_ = false;
        if (event.type == WindowEventType::MouseMotion)
        {
            const auto & motion = std::get<MouseMotionEvent>(event.payload);
            cursor_x_ = static_cast<S32>(motion.x);
            cursor_y_ = static_cast<S32>(motion.y);
        }
        if (event.type == WindowEventType::QuitRequested)
        {
            if (exit_callback_) exit_callback_(0);
            return;
        }
        if (event.type == WindowEventType::CloseRequested &&
            CloseVetoed(close_callbacks_.pClientList)) return;

        const auto translated = input_adapter_.Translate(event);
        for (const auto & item : translated)
        {
            if (item.payload != LegacyInputPayload::WindowMessage) continue;
            const auto & message = item.window_message;
            if (window_callback_)
                window_callback_(static_cast<HWND>(message.window), message.message,
                                 message.wparam, message.lparam);
            else if (message.message == LegacyMessage::Close && exit_callback_)
                exit_callback_(0);
        }
    }

    void FillArea(WM_WINAREA & area, bool client) const
    {
        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;
        const bool available = client
            ? window_.ClientArea(x, y, width, height)
            : window_.WindowArea(x, y, width, height);
        area.x = available ? static_cast<S32>(x) : 0;
        area.y = available ? static_cast<S32>(y) : 0;
        area.w = available ? static_cast<S32>(width) : 0;
        area.h = available ? static_cast<S32>(height) : 0;
    }

    static NativeWindowManager * live_window_;
    SdlWindow window_;
    SdlLegacyInputAdapter input_adapter_;
    ConnectionPoint<NativeWindowManager, ISystemEventCallback> close_callbacks_;
    WMEXITCB exit_callback_ = nullptr;
    WNDPROC window_callback_ = nullptr;
    S32 cursor_x_ = 0;
    S32 cursor_y_ = 0;
    bool active_ = true;
};

NativeWindowManager * NativeWindowManager::live_window_ = nullptr;
}

bool HasNativeWindowMessageTarget(LegacyWindowToken window)
{
    return NativeWindowManager::HasMessageTarget(window);
}

bool DispatchNativeWindowMessage(LegacyWindowToken window, LegacyMessageId message,
                                 LegacyWParam wparam, LegacyLParam lparam)
{
    return NativeWindowManager::Dispatch(window, message, wparam, lparam);
}

void RegisterNativeWindowManager(ICOManager * manager)
{
    if (!manager) return;
    IComponentFactory * factory =
        new DAComponentFactory2<DAComponentAggregate<NativeWindowManager>, AGGDESC>(interface_name);
    if (!factory) return;
    manager->RegisterComponent(factory, interface_name);
    factory->Release();
}
}
