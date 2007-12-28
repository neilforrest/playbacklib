#pragma once
//#include "PlaybackControl.h"
#include "Operation.h"
#include "PID/pid.h"
#include <vector>

#ifdef PB_IMPORT
#define PB_API __declspec(dllimport)
#else
#define PB_API __declspec(dllexport)
#endif

// Output debugging information to the specified file
//#define PLAYBACK_DEVICE_DEBUG	"PlaybackDeviceDebug.csv"

// Forward declare
class CPlaybackControl;

class PB_API CPlaybackDevice
{
public:
	CPlaybackDevice( CPlaybackControl* subject );
	virtual ~CPlaybackDevice(void);

	// Get playback controller force
	void GetForce ( double* force, double* position );

	// Syncronise with subject (CPlaybackControl)
	void Syncronise ( );

	// Get average frame rate
	double GetFramesPerSecond ( );

protected:

	// Advance to next operation
	void AdvanceOperation ( );

	// Update fps calculation
	void UpdateFrameRate ( );

	// Average frame rate
	double m_fps;

	// Counter at time of last frame check
	LARGE_INTEGER m_lastFrameCounter;
	LARGE_INTEGER m_freq;

	// Number of frames since last check
	int m_frameCount;

	// Operations that have been processed since last sync
	std::vector<COperation*> m_finishedOperations;

	// Ptr to Subject (CPlaybackControl)
	CPlaybackControl* playbackControl;

	// Queue of operations
	std::vector<COperation*> m_operations;

	// PID Controllers
	control_state *control_x, *control_y, *control_z;

	// PID Controller Parameters

	// Record that controller state has been reset
	bool m_resetControllerState;

// Output debugging information to the specified file
#ifdef PLAYBACK_DEVICE_DEBUG
	FILE* m_debugFile;
#endif
};
