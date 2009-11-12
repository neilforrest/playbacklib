#include "StdAfx.h"
#include "PlaybackNode.h"
#include "Parse.h"
#include <sstream>
#include <iostream>

using namespace std;
using namespace PlaybackLib;

CPlaybackNode::CPlaybackNode(void)
{
	m_space[0]= 0.0;
	m_space[1]= 0.0;
	m_space[2]= 0.0;

	m_time= 0.0;

	m_force[0]= 0.0;
	m_force[1]= 0.0;
	m_force[2]= 0.0;

	m_useForce= false;
}

CPlaybackNode::~CPlaybackNode(void)
{
}

// Use the force parameter when loading/saving node data etc
void CPlaybackNode::SetUseForceData ( bool useForceData )
{
	m_useForce= useForceData;
}

bool CPlaybackNode::IsUseForceData ()
{
	return m_useForce;
}

// Get string representation
string CPlaybackNode::ToString ( )
{
	stringstream ss;

	if ( m_useForce )
	{
		ss << m_time << ", " << 
			  m_space[0] << ", " << 
			  m_space[1] << ", " << 
			  m_space[2] << ", " << 
			  m_force[0] << ", " << 
			  m_force[1] << ", " << 
			  m_force[2] << endl;
	}
	else
	{
		ss << m_time << ", " << 
			  m_space[0] << ", " << 
			  m_space[1] << ", " << 
			  m_space[2] << endl;
	}

	return ss.str();
}

// Populate from string representation
bool CPlaybackNode::FromString ( string& str )
{
	vector<string> tokens= tokenize ( str, ", " );

	if ( tokens.size() >= 4 )
	{
		istringstream time(tokens[0]);
		time >> m_time;

		istringstream spacex(tokens[1]);
		spacex >> m_space[0];

		istringstream spacey(tokens[2]);
		spacey >> m_space[1];

		istringstream spacez(tokens[3]);
		spacez >> m_space[2];
	}

	if ( m_useForce && tokens.size() >= 7 )
	{
		istringstream forcex(tokens[4]);
		forcex >> m_force[0];

		istringstream forcey(tokens[5]);
		forcey >> m_force[1];

		istringstream forcez(tokens[6]);
		forcez >> m_force[2];
	}

	return true;
}

// Get next playback node from file
void CPlaybackNode::GetNode ( FILE* f, bool* eof, bool* error )
{
	static const int maxLineLength= 1000;

	char str[maxLineLength];

	// Get next line from file
	fgets ( str, maxLineLength, f );

	if ( feof ( f ) )
	{
		*eof= true;
		return;
	}

	if ( ferror ( f ) )
	{
		*error= true;
		return;
	}

	// Remove new line
	int last= strlen(str)-1;
	if ( str[last] =='\n' ) str[last]= '\0';

	// Parse and create node
	*error= !FromString ( string ( str ) );
}

// Assignment Operator
CPlaybackNode CPlaybackNode::operator= ( CPlaybackNode a )
{
	if ( &a == this ) return *this;

	m_time= a.m_time;
	m_space[0]= a.m_space[0];
	m_space[1]= a.m_space[1];
	m_space[2]= a.m_space[2];

	m_force[0]= a.m_force[0];
	m_force[1]= a.m_force[1];
	m_force[2]= a.m_force[2];

	return *this;
}

CPlaybackNode CPlaybackNode::operator= ( double a ) // Used to zero the node to (0, 0, 0 )
{
	m_time= a;
	m_space[0]= a;
	m_space[1]= a;
	m_space[2]= a;

	m_force[0]= a;
	m_force[1]= a;
	m_force[2]= a;

	return *this;
}

// Equals Operator
bool CPlaybackNode::operator== ( CPlaybackNode a )
{
	return 
		m_time == a.m_time				&&
		m_space[0] == a.m_space[0]		&&
		m_space[1] == a.m_space[1]		&&
		m_space[2] == a.m_space[2]		&&
		(m_useForce ? ( 	m_force[0] == a.m_force[0]		&&
							m_force[1] == a.m_force[1]		&&
							m_force[2] == a.m_force[2]	) : true );
}

// Addition operator
CPlaybackNode CPlaybackNode::operator + ( CPlaybackNode a )
{
	//OutputDebugString ( "CPlaybackNode::operator + ( CPlaybackNode a )\n" );

	CPlaybackNode ans;

	ans.m_time= m_time;

	ans.m_space[0]= m_space[0] + a.m_space[0];
	ans.m_space[1]= m_space[1] + a.m_space[1];
	ans.m_space[2]= m_space[2] + a.m_space[2];

	ans.m_force[0]= m_force[0];
	ans.m_force[1]= m_force[1];
	ans.m_force[2]= m_force[2];

	return ans;
}

CPlaybackNode CPlaybackNode::operator - ( CPlaybackNode a )
{
	CPlaybackNode ans;

	ans.m_time= m_time;

	ans.m_space[0]= m_space[0] - a.m_space[0];
	ans.m_space[1]= m_space[1] - a.m_space[1];
	ans.m_space[2]= m_space[2] - a.m_space[2];

	ans.m_force[0]= m_force[0];
	ans.m_force[1]= m_force[1];
	ans.m_force[2]= m_force[2];

	return ans;
}

// Scalar multiply operator
CPlaybackNode CPlaybackNode::operator * ( double a )
{
	//OutputDebugString ( "CPlaybackNode::operator * ( double a )\n" );

	CPlaybackNode ans;

	ans.m_time= m_time;

	ans.m_space[0]= m_space[0] * a;
	ans.m_space[1]= m_space[1] * a;
	ans.m_space[2]= m_space[2] * a;

	ans.m_force[0]= m_force[0];
	ans.m_force[1]= m_force[1];
	ans.m_force[2]= m_force[2];

	return ans;
}

CPlaybackNode CPlaybackNode::operator / ( double a )
{
	CPlaybackNode ans;

	ans.m_time= m_time;

	ans.m_space[0]= m_space[0] / a;
	ans.m_space[1]= m_space[1] / a;
	ans.m_space[2]= m_space[2] / a;

	ans.m_force[0]= m_force[0];
	ans.m_force[1]= m_force[1];
	ans.m_force[2]= m_force[2];

	return ans;
}