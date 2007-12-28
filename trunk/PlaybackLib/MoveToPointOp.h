#pragma once
#include "operation.h"

#ifdef PB_IMPORT
#define PB_API __declspec(dllimport)
#else
#define PB_API __declspec(dllexport)
#endif

class PB_API CMoveToPointOp : public COperation
{
public:
	// Create a new move to point operation
	CMoveToPointOp( double x, double y, double z, double speed );

	// Create a new move to point operation to move to start/end of gesture file
	CMoveToPointOp( char* filename, double speed, bool start= true );

	virtual ~CMoveToPointOp(void);

	// Deep copy operator
	CMoveToPointOp operator = ( CMoveToPointOp op );

	// Create a new object of this type
	virtual COperation* Clone ( );

	// Get playback controller force
	void GetForce ( double* force, double* position, 
							control_state* control_x, control_state* control_y, control_state* control_z );

	// For continuity it's sometimes needed to know the last set point / bead position
	void GetLastSetPoint ( double* point );

	// Set a previous operations set point as a starting point for our trajectory, rather than use the device position
	// which would result in an initial zeroing of output forces
	void SetLastSetPoint ( double* point );

	// Is currently holding user at end of playback, waiting for cancel
	bool IsHoldingAtEnd ( );

	// Playback Speed, (1 == Recorded Speed, 2 == 2 * Recorded Speed, etc)
	void SetPlaybackSpeed ( double speed );
	double GetPlaybackSpeed ( );

	// Hold the user at the end of the playback trajectory and wait for cancel
	void SetHoldAtEnd ( bool hold );
	bool IsHoldAtEnd ( );

	// Turn on/off pause of playback if user falls behind playback by more than tolerance
	void SetPauseIfResisting ( bool pause );
	bool IsPauseIfResisting ( );

	// Set tolerance used to pause playback if user resists motion
	void SetPauseIfResistingTolerance ( double tol );
	double GetPauseIfResistingTolerance ( );

	std::string ToString ( );

protected:

	// Begin a pause in playback
	void StartPause ();

	// End the current pause in playback
	void EndPause ();

	// Linearly interpolate between two points, r= 0 to 1
	void InterpolateLinear ( double* a, double* b, double r,
										  double* result );

	// Move to point (x,y,z)
	double m_toPoint[3];

	// Speed of move (units per ms)
	double m_speed;

	// Start position
	double m_sourcePoint[3];

	// Current bead position
	double m_beadPos[3];

	// Use a previous set point as the start of our trajectory instead of
	// current device position. Use when continuous control is required.
	bool m_usePreviousSetPoint;
	double m_previousSetPoint[3];

	// The desired time, calculated from the speed to get to the toPoint from sourcePoint
	double m_totalTime;

	// Start time
	LARGE_INTEGER m_startCount;

	// Performance counter frequency
	LARGE_INTEGER m_freq;

	// Cache distance from start to end of movement
	double m_moveToDist;

	// Advance to next bead tolerance
	double m_tolerance;
	bool m_useTolerance;

	// The playback is currently paused
	bool m_paused;

	// Total duration that playback has been paused for
	double m_totalPauseDuration;

	// Counter at last pause start time
	LARGE_INTEGER m_pauseStartCount;

	// Do first time initialisation?
	bool m_firstTime;

	// Should hold at end of playback and wait for user to cancel
	bool m_holdAtEnd;

	// Currently olding at end of playback
	bool m_holdingAtEnd;
};
