#pragma once

namespace PlaybackLib
{

	class CPlaybackNode
	{
	public:
		CPlaybackNode(void);
		virtual ~CPlaybackNode(void);

		// Assignment Operator
		CPlaybackNode operator= ( CPlaybackNode a );

		CPlaybackNode operator= ( double a ); // Used to zero the node to (0, 0, 0 )

		// Equals Operator
		bool operator== ( CPlaybackNode a );

		// Addition operator
		CPlaybackNode operator + ( CPlaybackNode a );

		CPlaybackNode operator - ( CPlaybackNode a );

		// Scalar multiply operator
		CPlaybackNode operator * ( double a );

		CPlaybackNode operator / ( double a );

		// Get string representation
		CString ToString ( );

		// Populate from string representation
		bool FromString ( CString str );

		// Get next playback node from file
		void GetNode ( FILE* f, bool* eof, bool* error );

		// Position space
		double m_space[3];

		// Position time
		double m_time;
	};
}