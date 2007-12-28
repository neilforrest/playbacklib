// PlaybackLibClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PlaybackLibClient.h"
#include "PlaybackLibClientDlg.h"
#include "Parse.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


HDCallbackCode HDCALLBACK MainForceCallback(void *pUserData)
{
	CPlaybackLibClientDlg* dlg= (CPlaybackLibClientDlg*)pUserData;

    hdBeginFrame(hdGetCurrentDevice());

	double position[3];
    hdGetDoublev(HD_CURRENT_POSITION, position);

	double force[3];

	dlg->playbackDevice->GetForce ( force, position );

	HDdouble f[3]= { force[0], force[1], force[2] };
	hdSetDoublev(HD_CURRENT_FORCE, f);

    hdEndFrame(hdGetCurrentDevice());

    return HD_CALLBACK_CONTINUE;
}

HDCallbackCode HDCALLBACK syncPlayback (void *pUserData)
{
	CPlaybackLibClientDlg* dlg= (CPlaybackLibClientDlg*)pUserData;

	dlg->playbackDevice->Syncronise ( );

	return HD_CALLBACK_DONE;
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CPlaybackLibClientDlg dialog




CPlaybackLibClientDlg::CPlaybackLibClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPlaybackLibClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// Create object to control playback (from client thread)
	playbackControl= new CPlaybackControl ( );

	// Create object to impliment playback (from device thread)
	playbackDevice= playbackControl->GetPlaybackDevice ( );

	// Set PID parameters
	CPIDControlParameters params;
	params.SetDevicePhantomOmni ();
	playbackControl->SetPIDParameters ( &params );

	// Syncronise control and device every... (ms)
	m_syncEveryMs= 250;

	// Index of operation last selected from list
	m_selectedOperation= -1;
}

void CPlaybackLibClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT10, m_fps);
	DDX_Control(pDX, IDC_EDIT1, m_syncEvery);
	DDX_Control(pDX, IDC_LIST1, m_operationsList);
	DDX_Control(pDX, IDC_COMBO1, m_splineOrder);
	DDX_Control(pDX, IDC_EDIT2, m_playbackFilename);
	DDX_Control(pDX, IDC_EDIT3, m_recordFilename);
	DDX_Control(pDX, IDC_EDIT8, m_sampleRate);
	DDX_Control(pDX, IDC_EDIT9, m_resolution);
}

BEGIN_MESSAGE_MAP(CPlaybackLibClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON2, &CPlaybackLibClientDlg::OnBnClickedToPoint)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON3, &CPlaybackLibClientDlg::OnBnClickedStartRecord)
	ON_BN_CLICKED(IDC_BUTTON5, &CPlaybackLibClientDlg::OnBnClickedStartPlayback)
	ON_BN_CLICKED(IDC_BUTTON9, &CPlaybackLibClientDlg::OnBnClickedUpdateSyncEvery)
	ON_BN_CLICKED(IDC_BUTTON7, &CPlaybackLibClientDlg::OnBnClickedCancelOp)
	ON_LBN_SELCHANGE(IDC_LIST1, &CPlaybackLibClientDlg::OnLbnSelchangeOperationsList)
	ON_BN_CLICKED(IDC_BUTTON1, &CPlaybackLibClientDlg::OnBnClickedStop)
END_MESSAGE_MAP()


// CPlaybackLibClientDlg message handlers

BOOL CPlaybackLibClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
    HDErrorInfo error;

    HHD hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        MessageBox ( "Failed to initialize haptic device" );
        return FALSE;
    }

    /* Schedule the haptic callback function for continuously monitoring the
       button state and rendering the anchored spring force. */
    int callbackHandle = hdScheduleAsynchronous (
		MainForceCallback, this, HD_MIN_SCHEDULER_PRIORITY );// HD_MAX_SCHEDULER_PRIORITY);

    hdEnable(HD_FORCE_OUTPUT);

    /* Start the haptic rendering loop. */
    hdStartScheduler();
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to start scheduler");
        return FALSE;
    }

	m_timer= SetTimer ( 0, m_syncEveryMs, 0 );

	char s[256];
	sprintf ( s, "%.0f", m_syncEveryMs );
	m_syncEvery.SetWindowText ( s );

	m_playbackFilename.SetWindowText ( "Square.csv" );
	m_recordFilename.SetWindowText ( "Test.csv" );
	m_sampleRate.SetWindowText ( "25" );
	m_resolution.SetWindowText ( "0.0" );

	recordOperation= NULL;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPlaybackLibClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPlaybackLibClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPlaybackLibClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPlaybackLibClientDlg::OnBnClickedToPoint()
{
	// TODO: Add your control notification handler code here
	moveToPoint= playbackControl->AddMoveToPointOp ( 0, 0, 0, 200 );
}

void CPlaybackLibClientDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	hdStopScheduler ( );

	CDialog::OnClose();
}

void CPlaybackLibClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	char s[256];

	// Syncronise playback device and controller
	hdScheduleSynchronous(syncPlayback,(void*)this, HD_DEFAULT_SCHEDULER_PRIORITY);

	// Update frame per second display
	sprintf ( s, "%.2f fps", playbackDevice->GetFramesPerSecond () );
	m_fps.SetWindowText ( s );

	// Update operation queue display
	m_operationsList.ResetContent ();

	// Get operations queue
	std::vector<COperation*>* ops= playbackControl->GetOperations ( );

	int i;
	for ( i= 0; i < ops->size (); i++ )
	{
		m_operationsList.AddString ( ops->at ( i )->ToString ().c_str () );
	}

	CDialog::OnTimer(nIDEvent);
}

void CPlaybackLibClientDlg::OnBnClickedStartRecord()
{
	// TODO: Add your control notification handler code here
	char filename[256];
	m_recordFilename.GetWindowText ( filename, 256 );

	char sampleRate[256];
	float sampleRateFloat;
	m_sampleRate.GetWindowText ( sampleRate, 256 );
	StringToFloat ( sampleRate, &sampleRateFloat );

	char resolution[256];

	float resolutionFloat;
	m_resolution.GetWindowText ( resolution, 256 );
	StringToFloat ( resolution, &resolutionFloat );

	recordOperation= playbackControl->AddRecordOp ( filename, (double)sampleRateFloat, (double)resolutionFloat );
}

void CPlaybackLibClientDlg::OnBnClickedStartPlayback()
{
	// TODO: Add your control notification handler code here
	int order= m_splineOrder.GetCurSel ( ) + 1;
	if ( order == 0 ) { order= 3; }

	char filename[256];
	m_playbackFilename.GetWindowText ( filename, 256 );

	moveToPoint= playbackControl->AddMoveToPointOp ( filename, 100 );
	playbackOperation= playbackControl->AddPlaybackOp ( filename, order );
}

void CPlaybackLibClientDlg::OnBnClickedUpdateSyncEvery()
{
	// TODO: Add your control notification handler code here
	char buffer[256];

	m_syncEvery.GetWindowText ( buffer, 256 );
	StringToFloat ( buffer, &m_syncEveryMs );
	
	// Update timer rate
	m_timer= SetTimer ( 0, m_syncEveryMs, 0 );
}

void CPlaybackLibClientDlg::OnBnClickedCancelOp()
{
	// TODO: Add your control notification handler code here
	std::vector<COperation*>* ops= playbackControl->GetOperations ( );

	if ( m_selectedOperation < 0 || m_selectedOperation >= ops->size () ) return;

	ops->at ( m_selectedOperation )->Cancel ( );
}

void CPlaybackLibClientDlg::OnLbnSelchangeOperationsList()
{
	// TODO: Add your control notification handler code here
	int curSel= m_operationsList.GetCurSel ();

	if ( curSel == -1 ) return;

	// Index of operation last selected from list
	m_selectedOperation= curSel;
}

void CPlaybackLibClientDlg::OnBnClickedStop()
{
	// TODO: Add your control notification handler code here
	if ( recordOperation != NULL )
	{
		recordOperation->Cancel ();
		//delete recordOperation;
	}
}
