#include "StdAfx.h"
#include "PlaybackOp.h"
#include <math.h>

using namespace PlaybackLib;

CPlaybackOp::CPlaybackOp( char* filename, int bSplineOrder )
{
	m_inFile= NULL;
	m_inFilename= CString ( filename );

	// Do first time initialisation?
	m_firstTime= true;

	// Advance to next bead tolerance
	m_tolerance= 15.0;

	// Speed of playback, 1 == record speed
	m_speed= 1.0;

	// Order of b-spline interpolation
	m_bSplineOrder= bSplineOrder;

	// Number of nodes to use in b-spline at any one time
	m_bSplineSize= m_bSplineOrder * 2 + 1;	// b-spline order * 2 + 1

	m_bSpline= NULL;

	m_progress= 0;

	// The last waypoint has been added
	m_endOfWaypoints= false;

	// The playback is currently paused
	m_paused= false;

	// Total duration that playback has been paused for
	m_totalPauseDuration= 0.0;

	// Advance to next bead tolerance
	m_tolerance= 15.0;
	m_useTolerance= true;

	// Should hold user after playback
	m_holdAtEnd= false;

	// Is currently holding after playback
	m_holdingAtEnd= false;

#ifdef PLAYBACK_OP_RECORD_PATH
	// Close debug file
	m_outFile= NULL;
#endif

	// Get freq of timing counter
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_freq);
}

CPlaybackOp::~CPlaybackOp(void)
{
	if ( m_inFile != NULL )
	{
		fclose ( m_inFile );
	}

#ifdef PLAYBACK_OP_RECORD_PATH
	// Close debug file
	if ( m_outFile != NULL )
	{
		fclose ( m_outFile );
	}
#endif

	if ( m_bSpline != NULL )
	{
		delete m_bSpline;
	}
}

// Playback Settings (Adjust _before_ operation begins)
// .................

// B-Spline Order
void CPlaybackOp::SetBSplineOrder ( int order )
{
	m_bSplineOrder= order;
}

int CPlaybackOp::GetBSplineOrder ( )
{
	return m_bSplineOrder;
}

// Playback Speed, (1 == Recorded Speed, 2 == 2 * Recorded Speed, etc)
void CPlaybackOp::SetPlaybackSpeed ( double speed )
{
	m_speed= speed;
}

double CPlaybackOp::GetPlaybackSpeed ( )
{
	return m_speed;
}

// Turn on/off pause of playback if user falls behind playback by more than tolerance
void CPlaybackOp::SetPauseIfResisting ( bool pause )
{
	m_useTolerance= pause;
}

bool CPlaybackOp::IsPauseIfResisting ( )
{
	return m_useTolerance;
}

// Set tolerance used to pause playback if user resists motion
void CPlaybackOp::SetPauseIfResistingTolerance ( double tol )
{
	m_tolerance= tol;
}

double CPlaybackOp::GetPauseIfResistingTolerance ( )
{
	return m_tolerance;
}

// Hold the user at the end of the playback trajectory and wait for cancel
void CPlaybackOp::SetHoldAtEnd ( bool hold )
{
	m_holdAtEnd= hold;
}

bool CPlaybackOp::IsHoldAtEnd ( )
{
	return m_holdAtEnd;
}

// .................

// Is currently holding user at end of playback, waiting for cancel
bool CPlaybackOp::IsHoldingAtEnd ( )
{
	return m_holdingAtEnd;
}

void CPlaybackOp::Copy ( COperation* op )
{
	// Call super class operator
	COperation::Copy(op);

	CPlaybackOp* subOp= (CPlaybackOp*)op;

	m_inFile= subOp->m_inFile;
	m_firstTime= subOp->m_firstTime;

	// Start time
	m_startCount= subOp->m_startCount;
	m_freq= subOp->m_freq;

	// Current bead position
	m_beadPos[0]= subOp->m_beadPos[0];
	m_beadPos[1]= subOp->m_beadPos[1];
	m_beadPos[2]= subOp->m_beadPos[2];

	m_tolerance= subOp->m_tolerance;
	m_useTolerance= subOp->m_useTolerance;
	m_speed= subOp->m_speed;

	m_bSplineOrder= subOp->m_bSplineOrder;
	m_bSplineSize= subOp->m_bSplineSize;

	m_holdAtEnd= subOp->m_holdAtEnd;
	m_holdingAtEnd= subOp->m_holdingAtEnd;
}


// Create a new object of this type
COperation* CPlaybackOp::Clone ( )
{
	CPlaybackOp* clone= new CPlaybackOp ( m_inFilename.GetBuffer () );

	*(clone)= *(this);

	return clone;
}

// Get playback controller force
void CPlaybackOp::GetForce ( double* force, double* position, 
				             ctrl_state* control_x, ctrl_state* control_y, ctrl_state* control_z )
{
	bool eof, error;

	// Do first time initialisation
	if ( m_firstTime )
	{

#ifdef PLAYBACK_OP_RECORD_PATH
		// For debugging, output actual interpolated playback path to the specified filename
		m_outFile= fopen ( PLAYBACK_OP_RECORD_PATH, "w" );
#endif

		// Create b-spline of playback nodes
		m_bSpline= new CBSpline<CPlaybackNode> ( );
		m_bSpline->SetOrder ( m_bSplineOrder );

		// Open input file
		m_inFile= fopen ( m_inFilename.GetString (), "r" );

		if ( m_inFile == NULL )
		{
			// Error opening file
			m_state= Error;
			return;
		}

		// Set progress through curve
		m_progress= 0.0;

		// Save starting time
		QueryPerformanceCounter((LARGE_INTEGER*)&m_startCount);
		m_lastFrameCount= m_startCount;

		// Add first way point positions
		for ( int i= 0; i < m_bSplineSize; i++ )
		{
			eof= false; error= false;
			CPlaybackNode *n= new CPlaybackNode ();
			n->GetNode ( m_inFile, &eof, &error );	// Read from file

			// Check for errors
			if ( eof ) // Reached end of file
			{
				m_endOfWaypoints= true;
				delete n;
				break;
			}

			if ( error )
			{
				m_state= Error;
				delete n;
				return;
			}

			m_bSpline->AddControlPoint ( *n );

			// Delete temp storage
			delete n;
		}

		if ( m_bSpline->GetControlPoints ()->size () < m_bSplineOrder + 1 ||
			 m_bSpline->GetControlPoints ()->size () < 2 )
		{
				m_state= Error;
				return;
		}

		// Create a knot vector and get maximum values of blending functions
		m_bSpline->CreateUniversalKnotVector ( );
		m_bSpline->PreProcessBlendingMaxPoints ( );

		// Index of the next control point to pass
		m_nextControlPoint= 1;

		// Don't exec again
		m_firstTime= false;
	}

	// Work out our progress through the current b-spline control polygon
	LARGE_INTEGER endCount;
	QueryPerformanceCounter((LARGE_INTEGER*)&endCount);

	// Total parameter range
	double range= m_bSpline->GetParameterMax ( );

	if ( m_useTolerance )
	{
		// if close enough to bead position, then advance the bead position
		double beadDistance= sqrt ( pow ( position[0]-m_beadNode.m_space[0], 2 ) +
									pow ( position[1]-m_beadNode.m_space[1], 2 ) +
									pow ( position[2]-m_beadNode.m_space[2], 2 ) );

		// Pause and Un-Pause as required
		if ( !m_paused && beadDistance > m_tolerance ) StartPause ();
		if ( m_paused && beadDistance <= m_tolerance ) EndPause ();
	}

	// Find the time since start of point passed
	double elapsed = (double)(endCount.QuadPart - m_startCount.QuadPart) / (double)m_freq.QuadPart;
	double frameGap= (double)(endCount.QuadPart - m_lastFrameCount.QuadPart) / (double)m_freq.QuadPart;

	m_lastFrameCount.QuadPart= endCount.QuadPart;

	// If not paused and not holding at end of gesture, then update current bead point
	if ( !m_paused && !m_holdingAtEnd )
	{
		// List of control points in current spline
		std::vector<CPlaybackNode>* controlPoints= m_bSpline->GetControlPoints ();

		//                          *Initially 1*
		while ( m_nextControlPoint < controlPoints->size () &&
			    controlPoints->at ( m_nextControlPoint ).m_time / m_speed <= elapsed - m_totalPauseDuration )
		{
			m_nextControlPoint++;
		}

		// Clamp to end of b-spline, new ctrl points are loaded when we reach the
		// halfway point, so if we get the the end of the spline, it's because we're at end of playback
		if ( m_nextControlPoint >= controlPoints->size () )
		{
			m_nextControlPoint= controlPoints->size ()-1;
		}

		// Time since start or passing last control point
		double timeSinceStart= (elapsed - m_totalPauseDuration) - 
								controlPoints->at ( m_nextControlPoint - 1 ).m_time / m_speed;

		// Total time gap between control points
		double totalTimeGap= controlPoints->at ( m_nextControlPoint ).m_time / m_speed - 
							 controlPoints->at ( m_nextControlPoint - 1 ).m_time / m_speed;

		// Desired fraction complete
		double complete= timeSinceStart / totalTimeGap;

		if ( complete < 0 )
		{
			complete= 0.0;
		}
		else if ( complete > 1 )
		{
			complete= 1.0;
		}

		// Desired progress through spline
		m_progress= m_bSpline->GetBlendingFunctionMaximum ( m_nextControlPoint-1 ) +
					complete * ( m_bSpline->GetBlendingFunctionMaximum ( m_nextControlPoint ) -
								 m_bSpline->GetBlendingFunctionMaximum ( m_nextControlPoint-1 ) );

		// If we're over half way through current b-spline
		if ( !m_endOfWaypoints && m_progress > range / 2.0 )
		{
			// Get next waypoint from file
			eof= false; error= false;
			CPlaybackNode *n= new CPlaybackNode ();
			n->GetNode ( m_inFile, &eof, &error );	// Read from file

			// Check for errors
			if ( eof )
			{
				// End of file, last waypoint has been added
				m_endOfWaypoints= true;
			}

			if ( error )
			{
				m_state= Error;
				delete n;
				return;
			}

			// Add to waypoint to b-spline
			if ( !eof )
			{
				// At next waypoint at end and remove first waypoint from start
				m_bSpline->RemoveFirstControlPoint ( );	// Remove first

				m_bSpline->AddControlPoint ( *n );

				// Curve has moved forwards, decrement progress to maintain position
				m_progress-= 1.0;
				m_nextControlPoint-= 1;
			}

			// Delete temp storage
			delete n;
		}

		// Check if we've reached the end of the b-spline
		if ( m_progress >= range )
		{
			if ( m_holdAtEnd )
			{
				// Clamp range and continue
				m_holdingAtEnd= true;
				m_progress= range;
				//break; // while loop
			}
			else
			{
				// Reached end of b-spline, playback completed
				m_state= Completed;
				return;
			}
		}
	}

	// Get point at m_progress along b-spline
	m_bSpline->GetPoint ( m_progress, &m_beadNode );
	
	// PID Control to move to bead position
	force[0]= control(control_x, position[0], m_beadNode.m_space[0]); 
	force[1]= control(control_y, position[1], m_beadNode.m_space[1]);
	force[2]= control(control_z, position[2], m_beadNode.m_space[2]);

#ifdef PLAYBACK_OP_RECORD_PATH
	// For debugging, output actual interpolated playback path to the specified filename
	fprintf ( m_outFile, "%f, %f, %f, %f\n", elapsed - m_totalPauseDuration, m_beadNode.m_space[0], m_beadNode.m_space[1], m_beadNode.m_space[2] );
#endif
}

// For continuity it's sometimes needed to know the last set point / bead position
void CPlaybackOp::GetLastSetPoint ( double* point )
{
	point[0]= m_beadNode.m_space[0];
	point[1]= m_beadNode.m_space[1];
	point[2]= m_beadNode.m_space[2];
}

std::string CPlaybackOp::ToString ( )
{
	return "Playback Operation: " + COperation::ToString ( );
}

void CPlaybackOp::StartPause ()
{
	if ( !m_paused )
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&m_pauseStartCount);
		m_paused= true;
	}
}

void CPlaybackOp::EndPause ()
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