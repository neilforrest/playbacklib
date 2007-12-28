#include "StdAfx.h"
#include "PlaybackNode.h"
#include "Parse.h"

CPlaybackNode::CPlaybackNode(void)
{
	m_space[0]= 0.0;
	m_space[1]= 0.0;
	m_space[2]= 0.0;

	m_time= 0.0;
}

CPlaybackNode::~CPlaybackNode(void)
{
}

// Get string representation
CString CPlaybackNode::ToString ( )
{
	CString str;
	str.Format ( "%f, %f, %f, %f\n", m_time, m_space[0], m_space[1], m_space[2] );

	return str;
}

// Populate from string representation
bool CPlaybackNode::FromString ( CString str )
{
	// Tokenise on , 
	CString token;
	int curPos= 0;

	float data[4];

	// Get first token
	token= str.Tokenize(", ",curPos);
	
	for ( int i= 0; i < 4; i++ )
	{
		if ( !StringToFloat ( token.GetBuffer (), &data[i] ) )
		{
			return false;
		}

		// Get next token
		token= str.Tokenize(", ",curPos);
	}

	m_time= data[0];
	m_space[0]= data[1];
	m_space[1]= data[2];
	m_space[2]= data[3];

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
	*error= !FromString ( CString ( str ) );
}

// Assignment Operator
CPlaybackNode CPlaybackNode::operator= ( CPlaybackNode a )
{
	if ( &a == this ) return *this;

	m_time= a.m_time;
	m_space[0]= a.m_space[0];
	m_space[1]= a.m_space[1];
	m_space[2]= a.m_space[2];

	return *this;
}

CPlaybackNode CPlaybackNode::operator= ( double a ) // Used to zero the node to (0, 0, 0 )
{
	m_time= a;
	m_space[0]= a;
	m_space[1]= a;
	m_space[2]= a;

	return *this;
}

// Equals Operator
bool CPlaybackNode::operator== ( CPlaybackNode a )
{
	return 
		m_time == a.m_time				&&
		m_space[0] == a.m_space[0]		&&
		m_space[1] == a.m_space[1]		&&
		m_space[2] == a.m_space[2];
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

	return ans;
}

CPlaybackNode CPlaybackNode::operator - ( CPlaybackNode a )
{
	CPlaybackNode ans;

	ans.m_time= m_time;

	ans.m_space[0]= m_space[0] - a.m_space[0];
	ans.m_space[1]= m_space[1] - a.m_space[1];
	ans.m_space[2]= m_space[2] - a.m_space[2];

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

	return ans;
}

CPlaybackNode CPlaybackNode::operator / ( double a )
{
	CPlaybackNode ans;

	ans.m_time= m_time;

	ans.m_space[0]= m_space[0] / a;
	ans.m_space[1]= m_space[1] / a;
	ans.m_space[2]= m_space[2] / a;

	return ans;
}