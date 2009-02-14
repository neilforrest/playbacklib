// PlaybackLib.h : main header file for the PlaybackLib DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CPlaybackLibApp
// See PlaybackLib.cpp for the implementation of this class
//

class CPlaybackLibApp : public CWinApp
{
public:
	CPlaybackLibApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
