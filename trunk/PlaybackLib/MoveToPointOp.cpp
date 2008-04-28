#include "StdAfx.h"
#include "MoveToPointOp.h"
#include "PlaybackNode.h"
#include <mmsystem.h>
#include <math.h>

using namespace PlaybackLib;

// Avoid div by zero
const double smallDouble= 0.00001;

CMoveToPointOp::CMoveToPointOp( double x, double y, double z, double speed )
{
	// Set type of operation
	m_type= MoveToPoint;

	// Default parameters
	m_toPoint[0]= x;
	m_toPoint[1]= y;
	m_toPoint[2]= z;

	m_speed= speed;

	// Tolerance defaults
	m_tolerance= 15.0;
	m_useTolerance= true;

	// The playback is currently paused
	m_paused= false;

	// Total duration that playback has been paused for
	m_totalPauseDuration= 0.0;

	// Do first time initialisation?
	m_firstTime= true;

	// Holding at end of playback
	m_holdingAtEnd= false;

	// Should hold at end of playback and wait for user to cancel
	m_holdAtEnd= false;

	// Use a previous set point as the start of our trajectory instead of
	// current device position. Use when continuous control is required.
	m_usePreviousSetPoint= false;
}

// Create a new move to point operation to move to start/end of gesture file
CMoveToPointOp::CMoveToPointOp( char* filename, double speed, bool start )
{
	// Set type of operation
	m_type= MoveToPoint;

	m_speed= speed;

	m_tolerance= 15.0;

	// Do first time initialisation?
	m_firstTime= true;

	// Holding at end of playback
	m_holdingAtEnd= false;

	// Should hold at end of playback and wait for user to cancel
	m_holdAtEnd= false;

	// Get destination point from file
	FILE* f= fopen ( filename, "r" );

	if ( f == NULL )
	{
		m_state= Error;
		return;
	}

	CPlaybackNode firstNode;

	bool eof= false; bool error= false;
	firstNode.GetNode ( f, &eof, &error );

	fclose ( f );

	if ( eof || error )
	{
		m_state= Error;
		return;
	}

	// Default parameters
	m_toPoint[0]= firstNode.m_space[0];
	m_toPoint[1]= firstNode.m_space[1];
	m_toPoint[2]= firstNode.m_space[2];
}

CMoveToPointOp::~CMoveToPointOp(void)
{
}

// Deep copy operator
CMoveToPointOp CMoveToPointOp::operator = ( CMoveToPointOp op )
{
	OutputDebugString ( "CMoveToPointOp\n" );

	if ( &op == this ) return *this;

	// Call super class operator
	COperation::operator=(op);

	// Move to point (x,y,z)
	m_toPoint[0]= op.m_toPoint[0];
	m_toPoint[1]= op.m_toPoint[1];
	m_toPoint[2]= op.m_toPoint[2];

	// Speed of move (units per ms)
	m_speed= op.m_speed;

	// Start position
	m_sourcePoint[0]= op.m_sourcePoint[0];
	m_sourcePoint[1]= op.m_sourcePoint[1];
	m_sourcePoint[2]= op.m_sourcePoint[2];

	// Current bead position
	m_beadPos[0]= op.m_beadPos[0];
	m_beadPos[1]= op.m_beadPos[1];
	m_beadPos[2]= op.m_beadPos[2];

	// Start time
	m_startCount= op.m_startCount;
	m_freq= op.m_freq;

	// Cache distance from start to end of movement
	m_moveToDist= op.m_moveToDist;

	// Advance to next bead tolerance
	m_tolerance= op.m_tolerance;

	// Do first time initialisation?
	m_firstTime= op.m_firstTime;

	// Holding at end of playback
	m_holdAtEnd= op.m_holdAtEnd;
	m_holdingAtEnd= op.m_holdingAtEnd;

	char s[256];
	sprintf ( s, "Copy from %d to %d. Hold at end %s.\n", &op, this, m_holdingAtEnd ? "true" : "false" );
	OutputDebugString ( s );

	return *this;
}

void CMoveToPointOp::Copy ( COperation* op )
{
	CMoveToPointOp* opSub= (CMoveToPointOp*)op;
	OutputDebugString ( "CMoveToPointOp::CopyIn ( )\n" );

	// Call super
	COperation::Copy(op);

	// Move to point (x,y,z)
	m_toPoint[0]= opSub->m_toPoint[0];
	m_toPoint[1]= opSub->m_toPoint[1];
	m_toPoint[2]= opSub->m_toPoint[2];

	// Speed of move (units per ms)
	m_speed= opSub->m_speed;

	// Start position
	m_sourcePoint[0]= opSub->m_sourcePoint[0];
	m_sourcePoint[1]= opSub->m_sourcePoint[1];
	m_sourcePoint[2]= opSub->m_sourcePoint[2];

	// Current bead position
	m_beadPos[0]= opSub->m_beadPos[0];
	m_beadPos[1]= opSub->m_beadPos[1];
	m_beadPos[2]= opSub->m_beadPos[2];

	// Start time
	m_startCount= opSub->m_startCount;
	m_freq= opSub->m_freq;

	// Cache distance from start to end of movement
	m_moveToDist= opSub->m_moveToDist;

	// Advance to next bead tolerance
	m_tolerance= opSub->m_tolerance;

	// Do first time initialisation?
	m_firstTime= opSub->m_firstTime;

	// Holding at end of playback
	m_holdAtEnd= opSub->m_holdAtEnd;
	m_holdingAtEnd= opSub->m_holdingAtEnd;

	m_paused= opSub->m_paused;
}

// Create a new object of this type
COperation* CMoveToPointOp::Clone ( )
{
	CMoveToPointOp* clone= new CMoveToPointOp ( m_toPoint[0], m_toPoint[1], m_toPoint[2], m_speed );

	*(clone)= *(this);

	return clone;
}

// Linearly interpolate between two points, r= 0 to 1
void CMoveToPointOp::InterpolateLinear ( double* a, double* b, double r,
						 double* result )
{
	result[0]= a[0] + (b[0] - a[0]) * r;
	result[1]= a[1] + (b[1] - a[1]) * r;
	result[2]= a[2] + (b[2] - a[2]) * r;
}

// Get playback controller force
void CMoveToPointOp::GetForce ( double* force, double* position, 
							    ctrl_state* control_x, ctrl_state* control_y, ctrl_state* control_z )
{
	// Do first time initialisation
	if ( m_firstTime )
	{
		// Save starting position
		if ( m_usePreviousSetPoint )
		{
			m_sourcePoint[0]= m_previousSetPoint[0];
			m_sourcePoint[1]= m_previousSetPoint[1];
			m_sourcePoint[2]= m_previousSetPoint[2];
		}
		else
		{
			m_sourcePoint[0]= position[0];
			m_sourcePoint[1]= position[1];
			m_sourcePoint[2]= position[2];
		}

		// Use starting pos as first bead pos
		m_beadPos[0]= m_sourcePoint[0];
		m_beadPos[1]= m_sourcePoint[1];
		m_beadPos[2]= m_sourcePoint[2];

		// Save starting time
		QueryPerformanceFrequency((LARGE_INTEGER*)&m_freq);
		QueryPerformanceCounter((LARGE_INTEGER*)&m_startCount);

		// Calc dist btween start and end pts
		m_moveToDist= sqrt ( pow ( m_sourcePoint[0]-m_toPoint[0], 2 ) +
							 pow ( m_sourcePoint[1]-m_toPoint[1], 2 ) +
							 pow ( m_sourcePoint[2]-m_toPoint[2], 2 ) );

		// The desired time, calculated from the speed to get to the toPoint from sourcePoint
		if ( m_speed == 0 ) m_speed= smallDouble; // prevent div by 0

		// t= d / s
		m_totalTime= m_moveToDist / m_speed;

		if ( m_totalTime == 0 ) m_totalTime= smallDouble;

		// Don't exec again
		m_firstTime= false;
	}

	if ( m_useTolerance )
	{
		// if close enough to bead position, then advance the bead position
		double beadDistance= sqrt ( pow ( position[0]-m_beadPos[0], 2 ) +
									pow ( position[1]-m_beadPos[1], 2 ) +
									pow ( position[2]-m_beadPos[2], 2 ) );

		// Pause and Un-Pause as required
		if ( !m_paused && beadDistance > m_tolerance ) StartPause ();
		if ( m_paused && beadDistance <= m_tolerance ) EndPause ();
	}

	if ( !m_paused )
	{
		LARGE_INTEGER endCount;
		QueryPerformanceCounter((LARGE_INTEGER*)&endCount);

		//find the time
		double elapsed = (double)(endCount.QuadPart - m_startCount.QuadPart) / (double)m_freq.QuadPart;

		// Progress to end point as a fraction 0..1
		double progress= (elapsed-m_totalPauseDuration) / m_totalTime;

		// If reached end point
		if ( progress >= 1.0 ) 
		{
			// Clamp progress to 1.0;
			progress= 1.0;

			if ( m_holdAtEnd )
			{
				// Holding at end of playback
				m_holdingAtEnd= true;
				//MessageBox ( 0, "Hold", "", MB_OK );
			}
			else
			{
				// Operation completed state
				m_state= Completed;
				//MessageBox ( 0, "Done", "", MB_OK );
			}
		}

		// Current bead point
		InterpolateLinear ( m_sourcePoint, m_toPoint, progress, m_beadPos );
	}

	// PID Control to move to bead position
	force[0]= control(control_x, position[0], m_beadPos[0]); 
	force[1]= control(control_y, position[1], m_beadPos[1]);
	force[2]= control(control_z, position[2], m_beadPos[2]);

	//char s[256];
	//sprintf ( s, "Progress: %f; Pos: %f, %f, %f\n" , progress, m_beadPos[0], m_beadPos[1], m_beadPos[2] );
	//OutputDebugString ( s );
}

// For continuity it's sometimes needed to know the last set point / bead position
void CMoveToPointOp::GetLastSetPoint ( double* point )
{
	point[0]= m_beadPos[0];
	point[1]= m_beadPos[1];
	point[2]= m_beadPos[2];
}

// Set a previous operations set point as a starting point for our trajectory, rather than use the device position
// which would result in an initial zeroing of output forces
void CMoveToPointOp::SetLastSetPoint ( double* point )
{
	m_usePreviousSetPoint= true;
	m_previousSetPoint[0]= point[0];
	m_previousSetPoint[1]= point[1];
	m_previousSetPoint[2]= point[2];
}

// Is currently holding user at end of playback, waiting for cancel
bool CMoveToPointOp::IsHoldingAtEnd ( )
{
	char s[256];
	sprintf ( s, "Holding at end? %d ??? %s\n", this, m_holdingAtEnd ? "true" : "false" );
	OutputDebugString ( s );
	return m_holdingAtEnd;
}

// Playback Speed, (1 == Recorded Speed, 2 == 2 * Recorded Speed, etc)
void CMoveToPointOp::SetPlaybackSpeed ( double speed )
{
	m_speed= speed;
}

double CMoveToPointOp::GetPlaybackSpeed ( )
{
	return m_speed;
}

// Turn on/off pause of playback if user falls behind playback by more than tolerance
void CMoveToPointOp::SetPauseIfResisting ( bool pause )
{
	m_useTolerance= pause;
}

bool CMoveToPointOp::IsPauseIfResisting ( )
{
	return m_useTolerance;
}

// Set tolerance used to pause playback if user resists motion
void CMoveToPointOp::SetPauseIfResistingTolerance ( double tol )
{
	m_tolerance= tol;
}

double CMoveToPointOp::GetPauseIfResistingTolerance ( )
{
	return m_tolerance;
}

// Is the operation currently paused due to user resistance
bool CMoveToPointOp::IsPaused ()
{
	return m_paused;
}

// Hold the user at the end of the playback trajectory and wait for cancel
void CMoveToPointOp::SetHoldAtEnd ( bool hold )
{
	m_holdAtEnd= hold;
}

bool CMoveToPointOp::IsHoldAtEnd ( )
{
	return m_holdAtEnd;
}


std::string CMoveToPointOp::ToString ( )
{
	return "Move To Point Operation: " + COperation::ToString ( );
}

void CMoveToPointOp::StartPause ()
{
	if ( !m_paused )
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&m_pauseStartCount);
		m_paused= true;
		OutputDebugString ( "Pause\n" );
	}
}

void CMoveToPointOp::EndPause ()
{
	if ( m_paused )
	{
		// Work out duration of pause
		LARGE_INTEGER endCount;
		QueryPerformanceCounter((LARGE_INTEGER*)&endCount);

		double elapsed = (double)(endCount.QuadPart - m_pauseStartCount.QuadPart) / (double)m_freq.QuadPart;

		// Add to total pause duration
		m_totalPauseDuration+= elapsed;

		m_paused= false;
	}
}