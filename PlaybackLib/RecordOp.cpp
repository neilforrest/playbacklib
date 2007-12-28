#include "StdAfx.h"
#include "RecordOp.h"
#include <math.h>

CRecordOp::CRecordOp( char* filename, double sampleRate, double resolution )
{
	// Open output file later
	m_outFile= NULL;

	m_outFilename= CString ( filename );

	// Save sample rate
	m_sampleRate= sampleRate;

	m_resolution= resolution;

	// Do first time initialisation?
	m_firstTime= true;

	// Currently in a sub resolution gap in recording
	m_gap= false;
}

CRecordOp::~CRecordOp(void)
{
	if ( m_outFile != NULL )
	{
		// Remember to close gap in sampling
		if ( m_gap )
		{
			// Save remebered node before continuing
			fprintf ( m_outFile, m_lastNode.ToString ( ) );

			m_gap= false;
		}

		fclose ( m_outFile );
	}
}

// For continuity it's sometimes needed to know the last set point / bead position
void CRecordOp::GetLastSetPoint ( double* point )
{
	// Irrelevant for record op
}

// Deep copy operator
CRecordOp CRecordOp::operator = ( CRecordOp op )
{
	if ( &op == this ) return *this;

	// Call super class operator
	COperation::operator=(op);

	m_outFile= op.m_outFile;
	m_sampleRate= op.m_sampleRate;
	m_firstTime= op.m_firstTime;

	// Start time
	m_startCount= op.m_startCount;
	m_freq= op.m_freq;

	m_resolution= op.m_resolution;

	return *this;
}

// Create a new object of this type
COperation* CRecordOp::Clone ( )
{
	CRecordOp* clone= new CRecordOp ( m_outFilename.GetBuffer (), m_sampleRate, m_resolution );

	*(clone)= *(this);

	return clone;
}

// Get playback controller force
void CRecordOp::GetForce ( double* force, double* position, 
				           control_state* control_x, control_state* control_y, control_state* control_z )
{
	// Do first time initialisation
	if ( m_firstTime )
	{
		// Open output file
		m_outFile= fopen ( m_outFilename, "w" );

#ifdef RECORD_OP_RECORD_PATH
	// For debugging, record the actual pathway at max rate/resolution available
		m_highResRecording= fopen ( "HighResRecord.csv", "w" );
#endif

		if ( m_outFile == NULL )
		{
			// Error opening file
			m_state= Error;
			return;
		}

		// Save starting time
		QueryPerformanceFrequency((LARGE_INTEGER*)&m_freq);
		QueryPerformanceCounter((LARGE_INTEGER*)&m_startCount);

		// Output first line to file
		m_lastNode.m_space[0]= position[0];
		m_lastNode.m_space[1]= position[1];
		m_lastNode.m_space[2]= position[2];
		m_lastNode.m_time= 0;

		fprintf ( m_outFile, m_lastNode.ToString ( ) );

		// Don't exec again
		m_firstTime= false;
	}

	LARGE_INTEGER endCount;
	QueryPerformanceCounter((LARGE_INTEGER*)&endCount);

	//find the time
	double elapsed = (double)(endCount.QuadPart - m_startCount.QuadPart) / (double)m_freq.QuadPart;

#ifdef RECORD_OP_RECORD_PATH
	// For debugging, record the actual pathway at max rate/resolution available
	CPlaybackNode n;
	n.m_space[0]= position[0];
	n.m_space[1]= position[1];
	n.m_space[2]= position[2];
	n.m_time= elapsed;
	fprintf ( m_highResRecording, n.ToString () );
#endif

	// If time since last sample > gap between samples
	if ( elapsed - m_lastNode.m_time > ( 1.0 / m_sampleRate ) )
	{
		if ( sqrt ( pow ( position[0]-m_lastNode.m_space[0], 2 ) +
					pow ( position[1]-m_lastNode.m_space[1], 2 ) +
					pow ( position[2]-m_lastNode.m_space[2], 2 ) ) >= m_resolution )
		{

			// If recording after a gap, record end to gap
			if ( m_gap )
			{
				// Save remebered node before continuing
				fprintf ( m_outFile, m_lastNode.ToString ( ) );

				m_gap= false;
			}

			// Output to file
			m_lastNode.m_space[0]= position[0];
			m_lastNode.m_space[1]= position[1];
			m_lastNode.m_space[2]= position[2];
			m_lastNode.m_time= elapsed;

			fprintf ( m_outFile, m_lastNode.ToString ( ) );
		}
		else
		{
			// Remember node
			m_lastNode.m_space[0]= position[0];
			m_lastNode.m_space[1]= position[1];
			m_lastNode.m_space[2]= position[2];
			m_lastNode.m_time= elapsed;

			m_gap= true;
		}
	}
}

std::string CRecordOp::ToString ( )
{
	return "Record Operation: " + COperation::ToString ( );
}