// PlaybackLibClientDlg.h : header file
//

#pragma once

#include <Playback.h>

#include <HD/hd.h>
#include <HDU/hduError.h>

#include "afxwin.h"

// CPlaybackLibClientDlg dialog
class CPlaybackLibClientDlg : public CDialog
{
// Construction
public:
	CPlaybackLibClientDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PLAYBACKLIBCLIENT_DIALOG };

	CPlaybackControl* playbackControl;
	CPlaybackDevice* playbackDevice;

	CRecordOp* recordOperation;
	CPlaybackOp* playbackOperation;
	CMoveToPointOp* moveToPoint;

	// Syncronise control and device every... (ms)
	float m_syncEveryMs;

	// Index of operation last selected from list
	int m_selectedOperation;

	// OnTimer id for client update loop
	UINT_PTR m_timer;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedToPoint();
public:
	afx_msg void OnClose();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	afx_msg void OnBnClickedStartRecord();
public:
	afx_msg void OnBnClickedStartPlayback();
public:
	CEdit m_fps;
public:
	CEdit m_syncEvery;
public:
	afx_msg void OnBnClickedUpdateSyncEvery();
public:
	CListBox m_operationsList;
public:
	afx_msg void OnBnClickedCancelOp();
public:
	afx_msg void OnLbnSelchangeOperationsList();
public:
	CComboBox m_splineOrder;
public:
	CEdit m_playbackFilename;
public:
	CEdit m_recordFilename;
public:
	CEdit m_sampleRate;
public:
	CEdit m_resolution;
};
