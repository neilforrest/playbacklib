#pragma once
#include "operation.h"
#include "PlaybackNode.h"

#ifdef PB_IMPORT
#define PB_API __declspec(dllimport)
#else
#define PB_API __declspec(dllexport)
#endif

// For debugging, record the actual pathway at max rate/resolution available
//#define RECORD_OP_RECORD_PATH		"RecordOpPath.csv"

namespace PlaybackLib
{

	// Operation to record a gesture to file
	class PB_API CRecordOp : public COperation
	{
	public:

		// Create a new record operation
		CRecordOp( const char* filename,				// Output filename
				   double sampleRate= 250,		// Samples per second (range > 0 and <= 1000)
				   double resolution= 0.0,		// Minimum change in position to record (range >= 0)
				   bool recordForce= false		// Record force data
				 );

		// Destroy the record operation
		virtual ~CRecordOp(void);

		// Record force data
		void SetUseForceData ( bool useForce );
		bool IsUseForceData ( );

		// For continuity it's sometimes needed to know the last set point / bead position
		void GetLastSetPoint ( double* point );

		// Deep copy operator
		void Copy ( COperation* op );

		// Create a new object of this type
		COperation* Clone ( );

		// Get playback controller force
		void GetForce ( double* force, double* position, 
						ctrl_state* control_x, ctrl_state* control_y, ctrl_state* control_z );

		// Delete this operation after it's finnished
		void Delete ( );

		// Debugging: Return string representation of operation
		std::string ToString ( );

	protected:

		// Output file
		FILE* m_outFile;

		// Resolution of recordin
		double m_resolution;

		// Output filename
		CString m_outFilename;

		// Sample rate Hz
		double m_sampleRate;

		// Start time
		LARGE_INTEGER m_startCount;

		// Performance counter frequency
		LARGE_INTEGER m_freq;

		// Do first time initialisation?
		bool m_firstTime;

		// Last recorded node
		CPlaybackNode m_lastNode;

		// Currently in a sub resolution gap in recording
		bool m_gap;

		// Record force data
		bool m_useForce;

	#ifdef RECORD_OP_RECORD_PATH
		// For debugging, record the actual pathway at max rate/resolution available
		FILE* m_highResRecording;
	#endif
	};
}