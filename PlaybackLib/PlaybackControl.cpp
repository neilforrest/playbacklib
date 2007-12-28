#include "StdAfx.h"
#include "PlaybackControl.h"
#include "PlaybackDevice.h"

CPlaybackControl::CPlaybackControl(void)
{
	// Singleton device observer for this controller
	m_playbackDevice= NULL;
	m_pidParamsChanged= false;
}

CPlaybackControl::~CPlaybackControl(void)
{
	// Delete per this object singleton creation of playback device
	if ( m_playbackDevice != NULL )
	{
		delete m_playbackDevice;
	}
}

// Has pid parameters changed since last sync
void CPlaybackControl::SetPIDParamsChanged ( bool changed )
{
	m_pidParamsChanged= changed;
}

bool CPlaybackControl::IsPIDParamsChanged ( )
{
	return m_pidParamsChanged;
}

// Create a new device object that is controlled by this control object
CPlaybackDevice* CPlaybackControl::GetPlaybackDevice ( )
{
	// Use per this object singleton creation
	if ( m_playbackDevice == NULL )
	{
		m_playbackDevice= new CPlaybackDevice ( this );
	}

	return m_playbackDevice;
}

// Set PID parameters
void CPlaybackControl::SetPIDParameters ( CPIDControlParameters* param )
{
	m_pidParams= *param;
	SetPIDParamsChanged ( true );
}

// Get PID parameters
CPIDControlParameters* CPlaybackControl::GetPIDParameters ( )
{
	return &m_pidParams;
}

// Add a new operation to the queue
void CPlaybackControl::AddOperation ( COperation* op )
{
	m_addedOperations.push_back ( op );
}

// Create and add a move to point operation to the queue
CMoveToPointOp* CPlaybackControl::AddMoveToPointOp ( double x, double y, double z, double speed )
{
	CMoveToPointOp* op= new CMoveToPointOp ( x, y, z, speed );

	AddOperation ( op );

	return op;
}

// Create and add a move to point operation to the queue
CMoveToPointOp* CPlaybackControl::AddMoveToPointOp ( char* filename, double speed )
{
	CMoveToPointOp* op= new CMoveToPointOp ( filename, speed );

	AddOperation ( op );

	return op;
}

// Create and add a record operation to the queue
CRecordOp* CPlaybackControl::AddRecordOp ( char* filename, double sampleRate, double resolution )
{
	CRecordOp* op= new CRecordOp  ( filename, sampleRate, resolution );

	AddOperation ( op );

	return op;
}

// Create and add a playback operation to the queue
CPlaybackOp* CPlaybackControl::AddPlaybackOp ( char* filename, int bSplineOrder )
{
	CPlaybackOp* op= new CPlaybackOp  ( filename, bSplineOrder );
	op->SetHoldAtEnd ( true );
	AddOperation ( op );

	return op;
}

// Get queue of operations
std::vector<COperation*>* CPlaybackControl::GetOperations ( )
{
	return &m_operationQueue;
}

// Get newley added operations
std::vector<COperation*>* CPlaybackControl::GetAddedOperations ( )
{
	return &m_addedOperations;
}