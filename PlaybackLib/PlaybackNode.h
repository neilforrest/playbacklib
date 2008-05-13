#pragma once

#ifdef PB_IMPORT
#define PB_API __declspec(dllimport)
#else
#define PB_API __declspec(dllexport)
#endif

namespace PlaybackLib
{

	class PB_API CPlaybackNode
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

		// Use the force parameter when loading/saving node data etc
		void SetUseForceData ( bool useForceData );
		bool IsUseForceData ();

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

		// Force at this node
		double m_force[3];

	protected:
		// Use the force parameter when loading/saving node data etc
		bool m_useForce;
	};
}