#include "StdAfx.h"
#include "PlaybackDevice.h"
#include "PlaybackControl.h"
#include "GarbageCollector.h"
#include <mmsystem.h>

using namespace PlaybackLib;

CPlaybackDevice::CPlaybackDevice( CPlaybackControl* subject )
{
	playbackControl= subject;

	// Initialise controllers
	InitPIDControllers ( );

	// Controller state is initially in reset state
	m_resetControllerState= true;

	// Initialise fps
	m_fps= 0;
	
	// Counter at time of last frame
	m_lastFrameCounter.QuadPart= 0;

	// Number of frames since last check
	m_frameCount= 0;

	QueryPerformanceFrequency((LARGE_INTEGER*)&m_freq);
	
// Output debugging information to the specified file
#ifdef PLAYBACK_DEVICE_DEBUG
	m_debugFile= fopen ( PLAYBACK_DEVICE_DEBUG, "w" );
#endif
}

CPlaybackDevice::~CPlaybackDevice(void)
{
#ifdef PLAYBACK_DEVICE_DEBUG
	fclose ( m_debugFile );
#endif
}

// Initialise PID controllers
void CPlaybackDevice::InitPIDControllers ( )
{
	control_x= init_state(m_pidParams.m_p, m_pidParams.m_i, m_pidParams.m_d, m_pidParams.m_f, 
		                  m_pidParams.m_out_filter, m_pidParams.m_gain, m_pidParams.m_dead_zone, m_pidParams.m_sat_low,
						  m_pidParams.m_sat_high);
	control_y= init_state(m_pidParams.m_p, m_pidParams.m_i, m_pidParams.m_d, m_pidParams.m_f, 
		                  m_pidParams.m_out_filter, m_pidParams.m_gain, m_pidParams.m_dead_zone, m_pidParams.m_sat_low,
						  m_pidParams.m_sat_high);
	control_z= init_state(m_pidParams.m_p, m_pidParams.m_i, m_pidParams.m_d, m_pidParams.m_f, 
		                  m_pidParams.m_out_filter, m_pidParams.m_gain, m_pidParams.m_dead_zone, m_pidParams.m_sat_low,
						  m_pidParams.m_sat_high);
}

// Get average frame rate
double CPlaybackDevice::GetFramesPerSecond ( )
{
	return m_fps;
}

// Advance to next operation
void CPlaybackDevice::AdvanceOperation ( )
{
	// Add operation to completed list
	COperation* finnishedOp= m_operations.at ( 0 ); // Save pointer for later
	m_finishedOperations.push_back ( m_operations.at ( 0 ) );

	// Shift others up
	m_operations.erase ( m_operations.begin ( ) );

	// If an operation remains in the queue
	if ( m_operations.size () > 0 )
	{
		// If it's a move to point operation
		if ( m_operations.at ( 0 )->GetType () == COperation::MoveToPoint )
		{
			// Get the last operations last set point
			double point[3];
			((CMoveToPointOp*)finnishedOp)->GetLastSetPoint ( point );

			// Hand the last set point to the move to operation to use as source point
			((CMoveToPointOp*)m_operations.at ( 0 ))->SetLastSetPoint ( point );				
		}
	}
}

// Update fps calculation
void CPlaybackDevice::UpdateFrameRate ( )
{
	// Calculate frame rate
	LARGE_INTEGER current;
	QueryPerformanceCounter((LARGE_INTEGER*)&current);

	if ( (double)(current.QuadPart - m_lastFrameCounter.QuadPart) / (double)m_freq.QuadPart > 1.0 )
	{
		m_fps= m_frameCount;
		m_frameCount= 0;

		QueryPerformanceCounter((LARGE_INTEGER*)&m_lastFrameCounter);
	}

	m_frameCount++;
}

// Get playback controller force
void CPlaybackDevice::GetForce ( double* force, double* position )
{
	// Update fps calculation
	UpdateFrameRate ( );

	// Zero forces
	//force[0]= 0.0; force[1]= 0.0; force[2]= 0.0;

	// While top most operation is completed, advance queue
	while ( m_operations.size () != 0 )
	{
		// Current operation
		COperation* operation= m_operations.at ( 0 );

		// Check that operation is not finished
		if ( operation->GetState () == operation->Cancelled ||		// Cancelled
			 operation->GetState () == operation->Error ||			// Error
			 operation->GetState () == operation->Completed )		// Finished
		{
			AdvanceOperation ( );
		}
		else
		{
			break;
		}
	}

	// If an operation remains, process it
	if ( m_operations.size () != 0 )
	{
		COperation* operation= m_operations.at ( 0 );

		// Branch on type of operation
		operation->GetForce ( force, position, control_x, control_y, control_z );

		// Need to reset controller state if we become idle
		m_resetControllerState= false;

#ifdef PLAYBACK_DEVICE_DEBUG
	fprintf ( m_debugFile, "%f, %f, %f, %s\n", force[0], force[1], force[2], operation->ToString ().c_str() );
#endif
	}
	else
	{
		// No operation, no continuous control, so reset controller state
		if ( !m_resetControllerState )
		{
			// Reset controllers
			reset_state ( control_x );
			reset_state ( control_y );
			reset_state ( control_z );

			m_resetControllerState= true;
		}

#ifdef PLAYBACK_DEVICE_DEBUG
		fprintf ( m_debugFile, "%f, %f, %f, No Operation\n", force[0], force[1], force[2] );
#endif
	}
}

// Syncronise with subject (CPlaybackControl)
void CPlaybackDevice::Syncronise ( )
{
	// If need be, re-create playback controllers with new settings
	if ( playbackControl->IsPIDParamsChanged () )
	{
		m_pidParams= *( playbackControl->GetPIDParameters () );

		PIDClean ( control_x );
		PIDClean ( control_y );
		PIDClean ( control_z );

		InitPIDControllers ();
		
		playbackControl->SetPIDParamsChanged ( false );
	}

	// Operations added since last sync
	std::vector<COperation*>* addedOperations= playbackControl->GetAddedOperations ( );

	// Controllers copy of operations queue
	std::vector<COperation*>* controlQueue= playbackControl->GetOperations ( );

	// Update status of all operations in the client queue

	// Update finished operations
	for ( unsigned int i= 0; i < m_finishedOperations.size ( ); i++ )
	{
		// Deep copy device version to control version to update status
		//*(controlQueue->at ( 0 ))= *(m_finishedOperations.at ( i ));
		controlQueue->at(0)->Copy ( m_finishedOperations.at ( i ) );

		// Delete device version
		delete m_finishedOperations.at ( i );

		// Remove our reference to client operation
		controlQueue->at ( 0 )->RemoveReference  ();

		// Remove from client queue
		controlQueue->erase ( controlQueue->begin () );
	}

	// Clear finished operations (all deleted)
	m_finishedOperations.clear ();

	// Update queued operations
	for ( unsigned int i= 0; i < m_operations.size (); i++ )
	{
		if ( (controlQueue->at ( i ))->IsCancelled ( ) )
		{
			m_operations.at ( i )->SetState ( COperation::Cancelled );
		}

		// Deep copy device version to control version
		//*(controlQueue->at ( i ))= *(m_operations.at ( i )); // FIX THIS
		controlQueue->at(i)->Copy ( m_operations.at ( i ) );
	}

	// Add new operations to queues
	for ( unsigned int i= 0; i < addedOperations->size (); i++ )
	{
		controlQueue->push_back ( addedOperations->at ( i ) );	// control copy

		// Add our refernce to the client operation
		addedOperations->at(i)->AddReference ();

		// Create and add device copy
		COperation* op= addedOperations->at ( i )->Clone ( );

		m_operations.push_back ( op );	// device copy
	}

	// Clear added operations
	addedOperations->clear ();

	// Collect up old client side operations
	CGarbageCollector::GetInstance ()->Collect ( );
}