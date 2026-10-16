//--------------------------------------------------------------------------//
//                                                                          //
//                      Teletype.cpp                                        //
//                                                                          //
//               COPYRIGHT (C) 1999 BY DIGITAL ANVIL, INC.                  //
//																			//
//--------------------------------------------------------------------------//
/*
   $Author: Jasony $

	Control that scrolls informative text to the user			

*/
//--------------------------------------------------------------------------//
 
#if defined(_WIN32)
#include "pch.h"
#include <globals.h>
#else
#include "Include/NativeAppGlobals.h"
#include "Include/NativeAppFont.h"
#include "resource.h"
#include <VFX.H>
#include <EventSys.h>
#include <IConnection.h>
#include <TComponent.h>
#include <TSmartPointer.h>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <cstdint>
#include <limits>
#include <initializer_list>
#include <vector>
#endif

#include "ITeletype.h"
#if defined(_WIN32)
#include "BaseHotRect.h"
#endif
#include "EventPriority.h"
#include "DrawAgent.h"
#include "CQTrace.h"
#include "Startup.h"

#if defined(_WIN32)
#include "frame.h"
#endif
#include "Hotkeys.h"
#include "SFX.h"

#if defined(_WIN32)
#include <da_vector>
using namespace da_std;
#else
using std::vector;
static void OffsetRect(RECT *rect, int x, int y)
{ rect->left += x; rect->right += x; rect->top += y; rect->bottom += y; }
#endif

#define MAX_CHARS 256

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
struct TeletypeLine
{
	TeletypeLine (wchar_t *szString, RECT rc, U32 _fontID, COLORREF _color, U32 _speed = 20, bool _bMute = false, bool _bCenter = false) 
		:  fontID(_fontID), color(_color), speed(_speed), bMute(_bMute), bCenter(_bCenter)
	{
		reloadFonts(true);
		wcsncpy(string, szString, sizeof(string)/sizeof(wchar_t));
#if !defined(_WIN32)
        string[MAX_CHARS-1] = 0;
#endif
		bDone = false;
		bLast = false;
		blink = 0;
		counter = 0;
		timer = 0;

		pane.window = NULL;
		pane.x0 = rc.left;
		pane.x1 = rc.right;
		pane.y0 = rc.top;
		pane.y1 = rc.bottom;

		paneCaret = pane;
		rcFull = rc;

		// temp - put someplace else
		Setup();
	}

	TeletypeLine (const TeletypeLine& rhs) : fontID(rhs.fontID), color(rhs.color), speed(rhs.speed), bMute(rhs.bMute), bCenter(rhs.bCenter)
	{
		*this = rhs;
	}

	~TeletypeLine (void)
	{
	}

	TeletypeLine& operator = ( const TeletypeLine& rhs )
	{
		wcsncpy(string, rhs.string, wcslen(rhs.string));
		
		pane = rhs.pane;
		paneCaret = rhs.paneCaret;

		counter = rhs.counter;
		timer = rhs.timer;
		blink = rhs.blink;
		bDone = rhs.bDone;
		bLast = rhs.bLast;

		nChars = rhs.nChars;
		height = rhs.height;
		width  = rhs.width;

		fontAgent = rhs.fontAgent;
		backAgent = rhs.backAgent;
		rcFull	  = rhs.rcFull;	
		
		U32 i;
		for (i = 0; i < nChars+1; i++)
		{
			pos[i] = rhs.pos[i];
		}

		return *this;
	}

	COMPTR<IFontDrawAgent> fontAgent;
	COMPTR<IFontDrawAgent> backAgent;
	PANE pane;
	PANE paneCaret;
	
	static HSOUND HsndTeletype;
	static bool bPlaying;
	const bool bMute;
	const bool bCenter;

	wchar_t string[MAX_CHARS];
	S32     pos[MAX_CHARS
#if !defined(_WIN32)
        + 1
#endif
        ];

	RECT rcFull;
	const U32  fontID;
	const COLORREF color;
	const int speed;  // in chars per second

	bool bDone;
	bool bLast;
	U32  counter;
	U32  oldcounter
#if !defined(_WIN32)
        = 0
#endif
        ;
	U32  timer;
	U32  blink;
	U32  nChars;
	U32  height;
	U32  width;
	U32  xoffset;

	void Setup (void)
	{
		nChars = wcslen(string);
		CQASSERT(nChars <= MAX_CHARS);

		// find all of our positions
		U32 i;
		wchar_t buffer[MAX_CHARS];

		pos[0] = 0;
		for (i = 1; i < nChars+1; i++)
		{
			memset(buffer, 0, sizeof(buffer));
			wcsncpy(buffer, string, i);
			pos[i] = fontAgent->GetStringWidth(buffer);
		}

		height = fontAgent->GetFontHeight();
		width  = fontAgent->GetStringWidth(L"W");

		if (bCenter)
		{
			xoffset = (rcFull.right - rcFull.left) - fontAgent->GetStringWidth(string);
			xoffset /= 2;
		}
		else
		{
			xoffset = 0;
		}

		Update(0);
	}

	void Draw (void)
	{
		backAgent->StringDraw(&pane, 1 + xoffset, 1, string);
		backAgent->StringDraw(&pane, -1 + xoffset , -1, string);
		backAgent->StringDraw(&pane, 1 + xoffset, -1, string);
		backAgent->StringDraw(&pane, -1 + xoffset, 1, string);
		fontAgent->StringDraw(&pane,  xoffset, 0, string);

		if (blink < 500)
		{
			DA::RectangleFill(&paneCaret, pos[counter]+1 + xoffset, 1, pos[counter] + width + xoffset, height, color);
			
			DA::LineDraw(&paneCaret, pos[counter] + xoffset,				0,		pos[counter] + width + xoffset,	0,		0);
			DA::LineDraw(&paneCaret, pos[counter] + width + xoffset,		0,		pos[counter] + width + xoffset,	height,	0);
			DA::LineDraw(&paneCaret, pos[counter] + width + xoffset,		height,	pos[counter] + xoffset,			height, 0);
			DA::LineDraw(&paneCaret, pos[counter] + xoffset,				height, pos[counter] + xoffset,			0,		0);
		}
	}

	void Release (void)
	{
		reloadFonts(false);
	}

	void Reset (void)
	{
		timer = 0;
		blink = 0;
		bDone = false;
	}

	void BumpUp (void)
	{
		OffsetRect(&rcFull, 0, -(pane.y1 - pane.y0));
		pane.y0 = rcFull.top;
		pane.y1 = rcFull.bottom;

		paneCaret.y0 = pane.y0;
		paneCaret.y1 = pane.y1;
	}

	void Update (U32 dt)
	{
		// change the size of the pane to reveal more characters over time
		if (bDone == false)
		{
			timer += dt;
			counter = (timer)*(speed)/1000;

			if (counter > oldcounter && bMute == false)
			{
				SFXMANAGER->Play(HsndTeletype, 0, 0);
			}

			if (counter >= nChars)
			{
				bDone = true;
				counter = nChars;
			}

			pane.x1 = pane.x0 + pos[counter] + xoffset;
			paneCaret.x1 = paneCaret.x0 + pos[counter] + width*2 + xoffset;
			oldcounter = counter;
		}
		else
		{

			// also, blink the caret (below 500 is on, above is off)
			blink = (blink + dt)%1000;

			if (bLast == false)
			{
				// no blinking caret at end of text if it is not the last Teletype line
				blink+=500;
			}
		}
	}

	void reloadFonts(bool bReload)
	{
		if (bReload)
		{
#if defined(_WIN32)
			HFONT hFont;
			COLORREF pen, background;

			pen			= color | 0xFF000000;		
			background	= RGB(0,0,0);
			hFont = CQCreateFont(fontID);

			CreateFontDrawAgent(hFont, 1, pen, background, fontAgent);
			CreateFontDrawAgent(hFont, 0, background | 0xFF000000, background, backAgent);
#else
            *static_cast<IFontDrawAgent **>(fontAgent) = FrontierX::CreateNativeFontDrawAgent(fontID, color);
            *static_cast<IFontDrawAgent **>(backAgent) = FrontierX::CreateNativeFontDrawAgent(fontID, RGB(0,0,0));
            if (!fontAgent || !backAgent) { std::fputs("Native teletype font creation failed\n", stderr); std::abort(); }
#endif
		}
		else
		{
			fontAgent.free();
			backAgent.free();
		}
	}
};

HSOUND TeletypeLine::HsndTeletype;

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
struct TeletypeObj
{
	typedef vector<TeletypeLine*> TTLINE_VECTOR;

	TeletypeObj (const wchar_t *string, U32 _fontID, COLORREF _color, RECT rc, U32 _lifetime = 10000, U32 _speed = 20, 
				 bool _bMute = false, bool _bIgnorePause = false, bool _bCenter = false) 
		: fontID(_fontID), color(_color), bMute(_bMute), bIgnorePause(_bIgnorePause), bCenter(_bCenter)
	{
		lifetime = _lifetime;
		speed = _speed;
		bDead = false;
		nLines = 0;
		timer = 0;

		nChars = (U16)(string[0]-1);
#if defined(_WIN32)
        string = string++;
#else
        nChars = static_cast<U16>(string[0]); // RT_STRING count excludes the terminator.
        ++string; // Skip the source counted-string prefix without unsequenced access.
#endif
		text = string;

		rect.left = IDEAL2REALX(rc.left);
		rect.top = IDEAL2REALY(rc.top);
		rect.right = IDEAL2REALX(rc.right);
		rect.bottom = IDEAL2REALY(rc.bottom);

		processString();
	}

	~TeletypeObj()
	{
		U32 i = 0;
		while (lines.size())
		{
			TeletypeLine * ptr = lines[i];
			lines.erase(lines.begin());
			delete ptr;
		}
	}

	TeletypeObj (const TeletypeObj& rhs) 
		: fontID(rhs.fontID), color(rhs.color), bMute(rhs.bMute), bIgnorePause(rhs.bIgnorePause), bCenter(rhs.bCenter)
	{
		*this = rhs;
	}

	TeletypeObj& operator = ( const TeletypeObj& rhs )
	{
		lines = rhs.lines;

		bDead		= rhs.bDead;
		timer		= rhs.timer;
		lifetime	= rhs.lifetime;
		speed		= rhs.speed;
		nLines		= rhs.nLines;
		lineHeight	= rhs.lineHeight;

		rect	= rhs.rect;
		text	= rhs.text;
		nChars	= rhs.nChars;

		return *this;
	}

	COMPTR<IFontDrawAgent> testAgent;

	bool bDead;
	U32 timer;
	U32 lifetime;
	U32 speed;			// in letters per sec
	U32 nLines; 
	U32 lineHeight;		// the height of one line of text
	U32 teletypeID;

	const U32 fontID;
	const COLORREF color;
	const bool bMute;
	const bool bIgnorePause;
	const bool bCenter;

	RECT rect;			// the all-encompassing rectangle

	const wchar_t *text;		// the counted wide string
	U16 nChars;			// number of characters in the whole string
	TTLINE_VECTOR lines;

	void Draw (void)
	{
		if (!CQFLAGS.bGamePaused || bIgnorePause)
		{
			U32 i;
			for (i = 0; i < nLines; i++)
			{
				if (lines[i]->rcFull.top >= rect.top)
					lines[i]->Draw();
				if (lines[i]->bDone == false)
					break;
			}
		}
	}

	void Update (U32 dt)
	{
		if (!CQFLAGS.bGamePaused || bIgnorePause)
		{
			timer += dt;
			if (lifetime && (timer > lifetime))
			{
				bDead = true;
			}

			U32 i;
			for (i = 0; i < nLines; i++)
			{
				lines[i]->Update(dt);

				// do we have to bump up the text?
				if (lines[i]->rcFull.bottom > rect.bottom)
				{
					bumpUpLines();
				}

				if (lines[i]->bDone == false)
				{
					break;
				}
			}
		}
	}

	void bumpUpLines (void)
	{
		U32 i;
		for (i = 0; i < lines.size(); i++)
		{
			lines[i]->BumpUp();
		}
	}

	void createTestAgent (void)
	{
		if (testAgent == 0)
		{
#if defined(_WIN32)
			HFONT hFont;
			COLORREF pen, background;

			pen			= color | 0xFF000000;		
			background	= RGB(0,0,0);
			hFont = CQCreateFont(fontID);

			CreateFontDrawAgent(hFont, 1, pen, background, testAgent);
#else
            *static_cast<IFontDrawAgent **>(testAgent) = FrontierX::CreateNativeFontDrawAgent(fontID, color);
            if (!testAgent) { std::fputs("Native teletype layout font failed\n", stderr); std::abort(); }
#endif
		}
	}

	void reloadFonts (bool bReload)
	{
		if (bReload)
		{
			createTestAgent();
		}
		else
		{
			if (testAgent)
			{
				testAgent.free();
			}
		}

		U32 i;
		for (i = 0; i < lines.size(); i++)
		{
			lines[i]->reloadFonts(bReload);
		}
	}

	void processString (void)
	{
		wchar_t buffer[MAX_CHARS];

		createTestAgent();
		lineHeight = testAgent->GetFontHeight();

		// divy the string up into seperate lines and add each line to the vector
		RECT rc;
		rc.left   = rect.left;
		rc.top    = rect.top;
		rc.right  = rect.right;
		rc.bottom = rect.top + lineHeight;

		int idx = 0;
		int next  = 0;
		int cnt   = 0;
		
		do 
		{
#if defined(_WIN32)
			// take away any newline characters
			if (text[idx] == 13 || text[idx] == 10)
			{
				idx++;
			}
#endif

			next = getNextLineLength(&text[idx], nChars-idx);
#if !defined(_WIN32)
            if (next < 0 || next >= MAX_CHARS || next > nChars - idx)
            { std::fputs("Native teletype line exceeds source buffer\n", stderr); std::abort(); }
            if (!next && idx < nChars && text[idx] != L'\r' && text[idx] != L'\n')
            { std::fputs("Native teletype line made no progress\n", stderr); std::abort(); }
#endif

			memset(buffer, 0, sizeof(buffer));
			wcsncpy(buffer, &text[idx], next);
			
			if (cnt)
			{
				OffsetRect(&rc, 0, lineHeight);
			}
			TeletypeLine *tl = new TeletypeLine(buffer, rc, fontID, color, speed, bMute, bCenter);

			lines.push_back(tl);

#if defined(_WIN32)
			if (next >= nChars - idx)// wcslen(&text[idx]))
			{
				// this is the last one we're adding
				break;
			}
			else
			{
				idx += next;
				cnt++;
			}
#else
            idx += next;
            if (idx == nChars)
                break;
            // A CRLF pair is one authored line break. Empty and final rows are
            // valid; advance by the separator rather than the row's length.
            if (text[idx] == L'\r')
            {
                ++idx;
                if (idx < nChars && text[idx] == L'\n')
                    ++idx;
            }
            else if (text[idx] == L'\n')
                ++idx;
            ++cnt;
#endif
		} while (
#if defined(_WIN32)
            next > 0
#else
            true
#endif
        );

		nLines = lines.size();
		lines[nLines-1]->bLast = true;
	}

	
	// return number of characters to print in the next line.
	// look for newlines, spaces as clues to where to break
	int getNextLineLength (const wchar_t *string, int lastchar)
	{
		int width = 0;
		int maxWidth = rect.right - rect.left;
		int lastSpace = -1;		// index of last space character found
		int index = 0;
		
#if !defined(_WIN32)
        if (lastchar <= 0)
            return 0;
#endif
		wchar_t letter = string[index];

		while (letter && width <= maxWidth &&
#if defined(_WIN32)
            index <= lastchar
#else
            index < lastchar
#endif
        )
		{
			if (letter == 13 || letter==10)		// newline character?
			{
				break;
			}
			else
			if (letter == ' ')
				lastSpace = index;
			
			wchar_t let[2];
			let[0] = letter;
			let[1] = 0;

			width += testAgent->GetStringWidth(let);
#if defined(_WIN32)
			letter = string[++index];
#else
            ++index;
            letter = index < lastchar ? string[index] : 0;
#endif
		}

		// back up until
		if (width > maxWidth)
		{
			if (lastSpace>=0 && index - lastSpace < 16)		// did we find a break in last 15 characters?
				index = lastSpace + 1;
			else
			if (index>1)
				index--;
		}
		
		return index;
	}
};


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------//
//
struct DACOM_NO_VTABLE Teletype : public IEventCallback, ITeletype
{
	BEGIN_DACOM_MAP_INBOUND(Teletype)
	DACOM_INTERFACE_ENTRY(ITeletype)
	DACOM_INTERFACE_ENTRY(IEventCallback)
	END_DACOM_MAP()

	typedef vector<TeletypeObj*> TTOBJ_VECTOR;

	U32 handle;			// connection handle
	static U32 teletypeIDCounter;
	
	HSOUND hsndTeletype;

	TTOBJ_VECTOR teletypes;


	Teletype (void);

	~Teletype (void);

    void * operator new (size_t size)
	{
		return calloc(size, 1);
	}

	void   operator delete (void *ptr)
	{
		::free(ptr);
	}

	/* ITeletype methods */

	virtual void Flush (void);

	virtual U32 CreateTeletypeDisplay (const wchar_t *_string, RECT rc, U32 fontID, COLORREF color, U32 _lifetime = 10000, U32 _textTime = 5000, bool bMute = false, bool bCenter = false);

	virtual U32 CreateTeletypeDisplayEx (const wchar_t *_string, RECT rc, U32 fontID, COLORREF color, U32 _lifetime = 10000, U32 _textTime = 5000, bool bMute = false, bool bIgnorePause = false);

	virtual BOOL32 IsPlaying (U32 teletypeID);

	virtual U32 GetLastTeletypeID (void)
	{
		return teletypeIDCounter;
	}
	virtual void SetLastTeletypeID (U32 id)
	{
		teletypeIDCounter = id;
	}

	/* IEventCallback methods */

	DEFMETHOD(Notify) (U32 message, void *param = 0);

	/* Teletype methods */

	void reloadFonts (bool bLoad);

	void draw (void);

	IDAComponent * GetBase (void)
	{
		return (ITeletype *) this;
	}

	void onUpdate (U32 dt)			// dt is milliseconds
	{
		U32 i;			
		for (i = 0; i < teletypes.size(); i++)
		{
			if (teletypes[i]->bDead)
			{
				TeletypeObj * ptr = teletypes[i];
				teletypes.erase(teletypes.begin()+i);
				delete ptr;
				break;
			}
		}

		for (i = 0; i < teletypes.size(); i++)
		{
			teletypes[i]->Update(dt);
		}
	}
};
U32	Teletype::teletypeIDCounter = 0;

//--------------------------------------------------------------------------//
//
Teletype::Teletype (void)
{
}
//--------------------------------------------------------------------------//
//
Teletype::~Teletype (void)
{
#if defined(_WIN32)
    if (TELETYPE) TELETYPE->Flush();
#else
    Flush();
#endif

#if defined(_WIN32)
	if (FULLSCREEN)
	{
		COMPTR<IDAConnectionPoint> connection;
		
		if (FULLSCREEN->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
			connection->Unadvise(handle);
	}

#else
    if (EVENTSYS && handle)
    {
        COMPTR<IDAConnectionPoint> connection;
        if (EVENTSYS->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
            connection->Unadvise(handle);
    }
#endif
	// close the  sound handles
	if (SFXMANAGER && hsndTeletype)
	{
		SFXMANAGER->CloseHandle(hsndTeletype);
		hsndTeletype = NULL;
	}
}
//--------------------------------------------------------------------------//
//
U32 Teletype::CreateTeletypeDisplay (const wchar_t *_string, RECT rc, U32 _fontID, COLORREF color, U32 _lifetime, U32 _textTime, bool bMute, bool bCenter)
{
	U32 result;
#if !defined(_WIN32)
    if (!_string || _string[0] < 1 || ! _textTime || rc.left < 0 || rc.top < 0 ||
        rc.right <= rc.left || rc.bottom <= rc.top ||
        wcslen(_string + 1) != static_cast<unsigned>(_string[0]))
    { std::fputs("Native source teletype parameters invalid\n", stderr); std::abort(); }
#endif

	if (hsndTeletype == NULL)
	{
		// load the sounds
		hsndTeletype = SFXMANAGER->Open(SFX::TELETYPE);
		TeletypeLine::HsndTeletype = hsndTeletype;
	}

	U32 speed = U32(_string[0])*1000/_textTime;

	TeletypeObj *to = new TeletypeObj(_string, _fontID, color, rc, _lifetime, speed, bMute, false, bCenter);
	result = to->teletypeID = ++teletypeIDCounter;
	teletypes.push_back(to);

	return result;
}
//--------------------------------------------------------------------------//
//
U32 Teletype::CreateTeletypeDisplayEx (const wchar_t *_string, RECT rc, U32 _fontID, COLORREF color, U32 _lifetime, U32 _textTime, bool bMute, bool bIgnorePause)
{
	U32 result;
#if !defined(_WIN32)
    if (!_string || _string[0] < 1 || ! _textTime || rc.left < 0 || rc.top < 0 ||
        rc.right <= rc.left || rc.bottom <= rc.top ||
        wcslen(_string + 1) != static_cast<unsigned>(_string[0]))
    { std::fputs("Native source teletype parameters invalid\n", stderr); std::abort(); }
#endif

	if (hsndTeletype == NULL)
	{
		// load the sounds
		hsndTeletype = SFXMANAGER->Open(SFX::TELETYPE);
		TeletypeLine::HsndTeletype = hsndTeletype;
	}

	U32 speed = U32(_string[0])*1000/_textTime;

	TeletypeObj *to = new TeletypeObj(_string, _fontID, color, rc, _lifetime, speed, bMute, bIgnorePause);
	result = to->teletypeID = ++teletypeIDCounter;
	teletypes.push_back(to);

	return result;
}
//--------------------------------------------------------------------------//
//
BOOL32 Teletype::IsPlaying (U32 teletypeID)
{
	// go through all the teletypes, see if there is a matching ID
	U32 i;
	for (i = 0; i < teletypes.size(); i++)
	{
		if (teletypes[i]->teletypeID == teletypeID)
		{
			return TRUE;
		}
	}

	return FALSE;
}
//--------------------------------------------------------------------------//
//
void Teletype::draw (void)
{
	bool bFrameLocked = (CQFLAGS.bFrameLockEnabled != 0);

	#if defined(_WIN32)
	if (bFrameLocked)
	{
		if (SURFACE->Lock() == false)
		{
			return;
		}
	}

#else
    if (bFrameLocked) { std::fputs("Native teletype framebuffer lock unsupported\n", stderr); std::abort(); }
#endif
	U32 i;
	for (i = 0; i < teletypes.size(); i++)
	{
		teletypes[i]->Draw();
	}
		
	#if defined(_WIN32)
	if (bFrameLocked)
	{
		SURFACE->Unlock();
	}
#endif
}
//--------------------------------------------------------------------------//
//
void Teletype::Flush (void)
{
	// goddamn STL bit me in the ass again!  Have to delete the pointers stored ourselves....
	while (teletypes.size())
	{
		TeletypeObj * ptr = teletypes[0];
		teletypes.erase(teletypes.begin());
		delete ptr;
	}
	
	// Dump any Teletype sound playing by closing the sound handles
	if (SFXMANAGER && hsndTeletype)
	{
		SFXMANAGER->CloseHandle(hsndTeletype);
		hsndTeletype = NULL;
	}
}
//--------------------------------------------------------------------------//
// receive notifications from event system
//
GENRESULT Teletype::Notify (U32 message, void *param)
{
//	wchar_t string[] = L" Here is the test teletype string, what the hell is the problem here anyway, I don't see anything.";
//	string[0] = wchar_t(wcslen(string)-1);
//	RECT rc;
//	::SetRect(&rc, 20, 20, 200, 200);

	switch (message)
	{
	case CQE_MISSION_CLOSE:
		Flush();
		break;

	case CQE_START3DMODE:
		reloadFonts(true);
		break;
	case CQE_END3DMODE:
		reloadFonts(false);
		break;

	case CQE_ENDFRAME:
		draw();
		break;

//	case SOMETHING_OR_OTHER:
//		Flush();
//		CreateTeletypeDisplay (string, rc, IDS_STATUSBAR_FONT, RGB(255,0,0)); 
//		break;

	case CQE_UPDATE:
//		if (!CQFLAGS.bGamePaused)
			#if defined(_WIN32)
            onUpdate(S32(param) >> 10);
#else
            if (reinterpret_cast<std::uintptr_t>(param) > std::numeric_limits<U32>::max()) return GR_INVALID_PARMS;
            onUpdate(static_cast<U32>(reinterpret_cast<std::uintptr_t>(param)) >> 10);
#endif
		break;
	}

	return GR_OK;
}
//--------------------------------------------------------------------------//
//
void Teletype::reloadFonts (bool bLoad)
{
	U32 i;
	for (i = 0; i < teletypes.size(); i++)
	{
		teletypes[i]->reloadFonts(bLoad);
	}
}

//--------------------------------------------------------------------------//
//
#if defined(_WIN32)
struct _teletype : GlobalComponent
{
	Teletype * TTYPE;
	
	virtual void Startup (void)
	{
		TELETYPE = TTYPE = new DAComponent<Teletype>;
		AddToGlobalCleanupList((IDAComponent **) &TELETYPE);
	}

	virtual void Initialize (void)
	{
		COMPTR<IDAConnectionPoint> connection;
	
		if (FULLSCREEN->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
		{
			connection->Advise(TTYPE->GetBase(), &TTYPE->handle);
			FULLSCREEN->SetCallbackPriority(TTYPE, EVENT_PRIORITY_TEXTCHAT);
		}
	}
};

static _teletype startup;
#else
namespace FrontierX
{
ITeletype *CreateNativeSourceTeletype()
{
    if (!EVENTSYS || !SFXMANAGER) return nullptr;
    auto *owner = new DAComponent<Teletype>;
    COMPTR<IDAConnectionPoint> connection;
    if (EVENTSYS->QueryOutgoingInterface("IEventCallback", connection) != GR_OK ||
        connection->Advise(owner->GetBase(), &owner->handle) != GR_OK)
    { owner->Release(); return nullptr; }
    return owner;
}
// The existing shell process check exercises this source owner without a GPU.
// Counted RT_STRING handling, pause and delayed expiry were unreachable while
// the shell substituted timer-only handles. Passing enables the mission owner.
bool CheckNativeSourceTeletype()
{
    ITeletype *interface = CreateNativeSourceTeletype();
    if (!interface) return false;
    auto *owner = static_cast<Teletype *>(interface);
    const unsigned savedFonts = NativeFontLiveCount();
    const auto savedPause = CQFLAGS.bGamePaused;
    const U32 savedID = owner->GetLastTeletypeID();
    const wchar_t text[] = {2, L'A', L'B', 0};
    const RECT bounds{10,10,200,100};
    const U32 normal = owner->CreateTeletypeDisplay(text, bounds, IDS_TELETYPE_FONT,
        RGB(255,255,0), 10, 100, true, true);
    const U32 ignoring = owner->CreateTeletypeDisplayEx(text, bounds, IDS_TELETYPE_FONT,
        RGB(255,255,0), 10, 100, true, true);
    const U32 persistent = owner->CreateTeletypeDisplay(text, bounds, IDS_TELETYPE_FONT,
        RGB(255,255,0), 0, 100, true, false);
    bool valid = owner->teletypes.size() == 3;
    for (auto *object : owner->teletypes)
        valid = valid && object->nChars == 2 && object->lines.size() == 1 &&
            std::wcscmp(object->lines[0]->string, L"AB") == 0 &&
            object->lines[0]->pos[2] == object->testAgent->GetStringWidth(L"AB");
    CQFLAGS.bGamePaused = 1;
    owner->onUpdate(11); owner->onUpdate(0);
    valid = valid && owner->IsPlaying(normal) && !owner->IsPlaying(ignoring) &&
        owner->IsPlaying(persistent);
    CQFLAGS.bGamePaused = 0;
    owner->onUpdate(11);
    valid = valid && owner->IsPlaying(normal); // Original removal is on the next update.
    owner->onUpdate(0);
    valid = valid && !owner->IsPlaying(normal) && owner->IsPlaying(persistent);
    owner->Flush();
    valid = valid && !owner->IsPlaying(persistent) && NativeFontLiveCount() == savedFonts;
    const auto checkRows = [&](const wchar_t *characters,
                               std::initializer_list<const wchar_t *> expected) {
        const auto count = std::wcslen(characters);
        std::vector<wchar_t> counted(count + 2);
        counted[0] = static_cast<wchar_t>(count);
        std::wmemcpy(counted.data() + 1, characters, count);
        const U32 id = owner->CreateTeletypeDisplay(counted.data(), bounds,
            IDS_TELETYPE_FONT, RGB(255,255,0), 0, 100, true, false);
        auto *object = owner->teletypes.back();
        bool matched = object->nChars == count && object->lines.size() == expected.size();
        std::size_t row = 0;
        for (const auto *line : expected)
        {
            if (row >= object->lines.size()) { matched = false; break; }
            const auto *actual = object->lines[row];
            matched = matched && std::wcscmp(actual->string, line) == 0 &&
                actual->rcFull.top == object->rect.top + row*object->lineHeight &&
                actual->bLast == (row + 1 == expected.size());
            ++row;
        }
        owner->onUpdate(1000);
        owner->onUpdate(0);
        matched = matched && owner->IsPlaying(id); // Zero lifetime survives completed text.
        owner->Flush();
        return matched && !owner->IsPlaying(id) && NativeFontLiveCount() == savedFonts;
    };
    // TM5's actual objective resource ends in LF. Also protect later content
    // after empty rows; the former next>0 loop silently dropped that content.
    valid = checkRows(L"A\n", {L"A", L""}) && valid;
    valid = checkRows(L"\nA\n\nB\nC", {L"", L"A", L"", L"B", L"C"}) && valid;
    valid = checkRows(L"A\r\n\r\nB\rC\nD\r\n", {L"A", L"", L"B", L"C", L"D", L""}) && valid;
    valid = checkRows(L"\r\n", {L"", L""}) && valid;
    // getNextLineLength consumes a counted span, including an empty final
    // span; it must not inspect a presumed terminator beyond that span.
    // Exact heap allocations make an accidental sentinel read visible to ASan.
    {
        auto *object = new TeletypeObj(text, IDS_TELETYPE_FONT, RGB(255,255,0),
            bounds, 0, 20, true, false, false);
        const wchar_t *empty = nullptr;
        valid = object->getNextLineLength(empty, 0) == 0 && valid;
        auto *exact = new wchar_t[2]{L'A', L'B'};
        valid = object->getNextLineLength(exact, 2) == 2 && valid;
        delete [] exact;
        delete object;
        valid = NativeFontLiveCount() == savedFonts && valid;
    }
    owner->SetLastTeletypeID(savedID);
    CQFLAGS.bGamePaused = savedPause;
    interface->Release();
    return valid;
}

}
#endif

//--------------------------------------------------------------------------//
//--------------------------End Teletype.cpp--------------------------------//
//--------------------------------------------------------------------------//