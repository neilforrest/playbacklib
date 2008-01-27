#include "StdAfx.h"
#include "Operation.h"

using namespace PlaybackLib;

// Root constructor
COperation::COperation(void)
{
	m_state= Processing;
	m_userCancel= false;
}

COperation::~COperation(void)
{
}

// Cancel this operation
void COperation::Cancel ( )
{
	m_userCancel= true;
}

bool COperation::IsCancelled ( )
{
	return m_userCancel;
}

// Get type of operation
COperation::OperationType COperation::GetType ( )
{
	return m_type;
}

// Get current state of operation
COperation::OperationState COperation::GetState ( )
{
	return m_state;
}

void COperation::SetState ( OperationState state )
{
	m_state= state;
}

// For continuity it's sometimes needed to know the last set point / bead position
void COperation::GetLastSetPoint ( double* point )
{

}

void COperation::Copy ( COperation* op )
{
	OutputDebugString ( "COperation::CopyIn\n" );

	m_type= op->m_type;		// Type of this operation
	m_state= op->m_state;	// Current state of this operation
}

// Create a new object of this type
COperation* COperation::Clone ( )
{
	COperation* clone= new COperation ( );

	*(clone)= *(this);

	return clone;
}

// Get playback controller force
void COperation::GetForce ( double* force, double* position, 
							ctrl_state* control_x, ctrl_state* control_y, ctrl_state* control_z )
{

}

std::string COperation::ToString ( )
{
	if ( m_userCancel ) return "CANCELED";
	else return "Processing";
}