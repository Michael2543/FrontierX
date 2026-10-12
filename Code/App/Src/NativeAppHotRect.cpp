// COPYRIGHT (C) 1997 BY DIGITAL ANVIL, INC.
// Original System.cpp hot-rectangle lifecycle for the native App composition.
#include "BaseHotRect.h"
#include "Frame.h"
#include "Include/NativeAppTextResources.h"
#include "Include/NativeAppGlobals.h"

#if !defined(_WIN32)
void BaseHotRect::sendPostedMessages (MESSAGE * pNode)
{
	MESSAGE node = *pNode;

	delete pNode;
	if (node.pNext)
		sendPostedMessages(node.pNext);

	Notify(node.message, node.param);
}
//--------------------------------------------------------------------------
//
BaseHotRect::~BaseHotRect (void)
{
	releaseResources();
	if (parentEventConnection)
		parentEventConnection->Unadvise(eventHandle);
	if (parentKeyboardFocusConnection)
		parentKeyboardFocusConnection->Unadvise(keyboardFocusHandle);

	if (pMessageList)
	{
		MESSAGE *pNode;

		while ((pNode = pMessageList) != 0)
		{
			pMessageList = pMessageList->pNext;
			if (pNode->message == CQE_DELETE_HOTRECT)
				removeClient((BaseHotRect *)pNode->param);
			delete pNode;
		}
	}
}
//--------------------------------------------------------------------------
//
void BaseHotRect::PostMessage (U32 message, void * param)
{
	MESSAGE *pNode;

	if ((pNode = new MESSAGE) != 0)
	{
		pNode->pNext = pMessageList;
		pMessageList = pNode;
		pNode->message = message;
		pNode->param = param;
	}
}
//--------------------------------------------------------------------------
//
namespace FrontierX
{
bool CheckNativeHotRect()
{
    struct ObservedRect : BaseHotRect
    {
        ObservedRect() : BaseHotRect(static_cast<IDAComponent *>(nullptr)) {}
        U32 sequence = 0;
        U32 deletionRequests = 0;
        GENRESULT COMAPI Notify(U32 message, void *parameter) override
        {
            if (message == CQE_DELETE_HOTRECT)
            {
                ++deletionRequests;
                return GR_GENERIC;
            }
            if (message == 0x7001 || message == 0x7002)
                sequence = sequence * 10 + message - 0x7000;
            return BaseHotRect::Notify(message, parameter);
        }
    };
    auto *owner = new ObservedRect;
    owner->screenRect = {10, 20, 30, 40};
    owner->onSetFocus(true);
    LegacyWindowMessage motion{};
    motion.message = LegacyMessage::MouseMove;
    motion.lparam = 10 | (20 << 16);
    bool valid = owner->Notify(motion.message, nullptr) == GR_INVALID_PARMS;
    motion.message = LegacyMessage::LeftButtonDown;
    valid = valid && owner->Notify(LegacyMessage::MouseMove, &motion) == GR_INVALID_PARMS;
    motion.message = LegacyMessage::MouseMove;
    valid = valid && owner->Notify(motion.message, &motion) == GR_OK && owner->bAlert;
    motion.lparam = 31 | (40 << 16);
    owner->Notify(motion.message, &motion);
    valid = valid && !owner->bAlert;
    owner->onSetFocus(false);
    motion.lparam = 10 | (20 << 16);
    owner->Notify(motion.message, &motion);
    valid = valid && !owner->bAlert && !owner->bHasFocus && !owner->actualOwnedFlags;
    owner->PostMessage(0x7001, nullptr);
    owner->PostMessage(0x7002, nullptr);
    valid = valid && owner->sequence == 0;
    owner->Notify(CQE_FLUSHMESSAGES, nullptr);
    valid = valid && owner->sequence == 12;
    owner->Notify(CQE_FLUSHMESSAGES, nullptr);
    valid = valid && owner->sequence == 12;
    owner->PostMessage(0x7001, nullptr); // Destructor releases an unflushed message.
    owner->Release();
    struct ObservedModalFrame : DAComponent<Frame>
    {
        void *operator new(size_t size) { return std::calloc(size, 1); }
        void operator delete(void *pointer) { std::free(pointer); }
    };
    auto *closeOwner = new ObservedRect;
    closeOwner->AddRef();
    BaseHotRect *previousFullscreen = FULLSCREEN;
    FULLSCREEN = nullptr;
    auto *dialog = new ObservedModalFrame;
    FULLSCREEN = previousFullscreen;
    dialog->parent = closeOwner;
    Frame *previousFocus = Frame::g_pFocusFrame;
    Frame::g_pFocusFrame = dialog;
    LegacyWindowMessage close{};
    close.message = LegacyMessage::Close;
    dialog->Notify(close.message, &close);
    closeOwner->Notify(CQE_FLUSHMESSAGES, nullptr);
    valid = valid && closeOwner->deletionRequests == 0;
    Frame::g_pFocusFrame = previousFocus;
    dialog->Notify(close.message, &close);
    closeOwner->Notify(CQE_FLUSHMESSAGES, nullptr);
    valid = valid && closeOwner->deletionRequests == 1;
    delete dialog; // CQDoModal's caller retains the modal frame's ownership.
    closeOwner->Release();
    // A real victory/restart retained movie-hidden HUD flags at native entry.
    const auto previousResolution = GetCurrentInterfaceRes();
    const auto previousWidth = SCREEN_WIDTH;
    const auto previousHeight = SCREEN_HEIGHT;
    const auto previousNoToolbar = CQFLAGS.bNoToolbar;
    const auto previousMovieMode = CQFLAGS.bMovieMode;
    ChangeInterfaceRes(IR_FRONT_END_RESOLUTION);
    CQFLAGS.bNoToolbar = CQFLAGS.bMovieMode = 1;
    ChangeInterfaceRes(IR_IN_GAME_RESOLUTION);
    valid = valid && !CQFLAGS.bNoToolbar && !CQFLAGS.bMovieMode &&
        SCREEN_WIDTH == 640 && SCREEN_HEIGHT == 480;
    // Repeating the current mode must retain a user's hidden-toolbar state.
    CQFLAGS.bNoToolbar = CQFLAGS.bMovieMode = 1;
    ChangeInterfaceRes(IR_IN_GAME_RESOLUTION);
    valid = valid && CQFLAGS.bNoToolbar && CQFLAGS.bMovieMode;
    ChangeInterfaceRes(previousResolution);
    SCREEN_WIDTH = previousWidth;
    SCREEN_HEIGHT = previousHeight;
    CQFLAGS.bNoToolbar = previousNoToolbar;
    CQFLAGS.bMovieMode = previousMovieMode;
    return valid && CheckNativeModalClose();
}
}
#endif
