#pragma once
#include "operation.h"
#include "PlaybackNode.h"
#include "BSpline.h"

#ifdef PB_IMPORT
#define PB_API __declspec(dllimport)
#else
#define PB_API __declspec(dllexport)
#endif

// For debugging, output actual interpolated playback path to the specified filename
//#define PLAYBACK_OP_RECORD_PATH		"PlaybackOpPath.csv"

namespace PlaybackLib
{

	// Max line length of input file
	static const int maxLineLength= 256;

	// Operation to playback a previously recorded gesture
	class PB_API CPlaybackOp : public COperation
	{
	public:

		// Create a new playback operation
		CPlaybackOp( char* filename,
					 int bSplineOrder= 3 
				   );

		// Destroy playback operation
		virtual ~CPlaybackOp(void);

		// For continuity it's sometimes needed to know the last set point / bead position
		void GetLastSetPoint ( double* point );

		// Playback Settings (Adjust _before_ operation begins)
		// .................

		// B-Spline Order
		void SetBSplineOrder ( int order );
		int GetBSplineOrder ( );

		// Playback Speed, (1 == Recorded Speed, 2 == 2 * Recorded Speed, etc)
		void SetPlaybackSpeed ( double speed );
		double GetPlaybackSpeed ( );

		// Turn on/off pause of playback if user falls behind playback by more than tolerance
		void SetPauseIfResisting ( bool pause );
		bool IsPauseIfResisting ( );

		// Set tolerance used to pause playback if user resists motion
		void SetPauseIfResistingTolerance ( double tol );
		double GetPauseIfResistingTolerance ( );

		// Is the operation currently paused due to user resistance
		bool IsPaused ();

		// Hold the user at the end of the playback trajectory and wait for cancel
		void SetHoldAtEnd ( bool hold );
		bool IsHoldAtEnd ( );

		// .................

		// Is currently holding user at end of playback, waiting for cancel
		bool IsHoldingAtEnd ( );

		// Deep copy operator
		void Copy ( COperation* op );

		// Create a new object of this type
		COperation* Clone ( );

		// Get playback controller force
		void GetForce ( double* force, double* position, 
						ctrl_state* control_x, ctrl_state* control_y, ctrl_state* control_z );

		// Debugging: Return string representation of operation
		std::string ToString ( );

		// Get last known playback time (sec)
		// Time from start of recording that last played back point was recorded (excluding pauses due to user resisting)
		double GetPlaybackTime ();

		// Get total duration of the recording (excluding pauses due to user resisting)
		double GetTotalPlaybackTime ();

		// Get last playback node
		CPlaybackNode GetLastPlaybackNode ();

	protected:

		// Begin a pause in playback
		void StartPause ();

		// End the current pause in playback
		void EndPause ();

		// Read the value of the last time-stamp in the playback file
		CPlaybackNode ReadLastNode ( FILE* file );

		// Current interpolated playback position
		CPlaybackNode m_beadNode;

		// Order of b-spline interpolation
		int m_bSplineOrder;

		// Time amount of time it should take to get from m_beadPos to next waypoint
		double m_dt;

		// Advance to next waypoint?
		bool m_advance;

		// Current point to interpolate from
		double m_fromPoint[3];

		// Playback file handle
		FILE* m_inFile;

		// Input filename
		CString m_inFilename;

		// Start time
		LARGE_INTEGER m_startCount;

		// Last frame time
		LARGE_INTEGER m_lastFrameCount;

		// Performance counter frequency
		LARGE_INTEGER m_freq;

		// Last known playback time (sec)
		double m_playbackTime;

		// Total duration of the recording (excluding pauses due to user resisting) (sec)
		double m_playbackTimeTotal;

		// Do first time initialisation?
		bool m_firstTime;

		// Current bead position
		double m_beadPos[3];

		// Advance to next bead tolerance
		double m_tolerance;
		bool m_useTolerance;

		// Speed of playback, 1 == record speed
		double m_speed;

		// Index of the next control point to pass
		int m_nextControlPoint;

		// Time of passing the last control point
		LARGE_INTEGER m_lastCtrlPointPassed;

		// BSpline of playback nodes
		CBSpline<CPlaybackNode>* m_bSpline;

		// Number of nodes to use in b-spline at one time
		int m_bSplineSize;

		double m_progress;

		double m_time;

		// The last waypoint has been added
		bool m_endOfWaypoints;

		// The playback is currently paused
		bool m_paused;

		// Counter at last pause start time
		LARGE_INTEGER m_pauseStartCount;

		// Total duration that playback has been paused for
		double m_totalPauseDuration;

		// Should hold at end of playback and wait for user to cancel
		bool m_holdAtEnd;

		// Currently olding at end of playback
		bool m_holdingAtEnd;

	#ifdef PLAYBACK_OP_RECORD_PATH
		// For debugging, output actual interpolated playback path to the specified filename
		FILE* m_outFile;
	#endif

	};
}