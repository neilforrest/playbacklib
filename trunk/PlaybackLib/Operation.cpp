#include "StdAfx.h"
#include "Operation.h"

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

// For continuity it's sometimes needed to know the last set point / bead position
void COperation::GetLastSetPoint ( double* point )
{

}

// Deep copy operator
COperation COperation::operator = ( COperation op )
{
	if ( &op == this ) return *this;

	m_type= op.m_type;		// Type of this operation
	m_state= op.m_state;	// Current state of this operation
	//m_userCancel= op.m_userCancel;

	return *this;
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
							control_state* control_x, control_state* control_y, control_state* control_z )
{

}

std::string COperation::ToString ( )
{
	if ( m_userCancel ) return "CANCELED";
	else return "Processing";
}