#pragma once
#include "pid/pid.h"
#include "Garbage.h"
#include <string>

#ifdef PB_IMPORT
#define PB_API __declspec(dllimport)
#else
#define PB_API __declspec(dllexport)
#endif

namespace PlaybackLib
{

	class PB_API COperation : public CGarbage
	{
	public:

		// States of operation
		enum OperationState
		{
			Processing,		// The operation has yet to be completed
			Completed,		// The operation has completed
			Cancelled,		// The operation was canceled either by the user or the system
			Error			// The was an error and the operation cannot complete
		};

		// Types of subclass of operation
		enum OperationType
		{
			SetIdle,
			StartPlayback,
			StartRecord,
			MoveToPoint,
			SetHoldAtEnd
		};

		COperation(void);
		virtual ~COperation(void);

		// Cancel this operation
		void Cancel ( );
		bool IsCancelled ( );

		// Get type of operation
		OperationType GetType ( );

		// Get current state of operation
		OperationState GetState ( );
		void SetState ( OperationState state );

		// Deep copy operator
		virtual void Copy ( COperation* op );

		// Create a new object of this type
		virtual COperation* Clone ( );

		// Get playback controller force
		virtual void GetForce ( double* force, double* position, 
								ctrl_state* control_x, ctrl_state* control_y, ctrl_state* control_z );

		// For continuity it's sometimes needed to know the last set point / bead position
		virtual void GetLastSetPoint ( double* point );

		virtual std::string ToString ( );

	protected:
		OperationType m_type;	// Type of this operation
		OperationState m_state;	// Current state of this operation

		// User cancelled operation
		bool m_userCancel;
	};
}