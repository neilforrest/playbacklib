#include "StdAfx.h"
#include "GarbageCollector.h"

using namespace PlaybackLib;

// Protected constructor
CGarbageCollector::CGarbageCollector(void)
{

}

CGarbageCollector::~CGarbageCollector(void)
{

}

// Use GetInstance () to access singleton instance
CGarbageCollector* CGarbageCollector::GetInstance ( )
{
	// The one and only instance
	static CGarbageCollector* m_instance= NULL;

	// Do lazy creation
	if ( m_instance == NULL )
	{
		m_instance= new CGarbageCollector ( );
	}

	// Return one and only instance
	return m_instance;
}

// Add an object to be deleted
void CGarbageCollector::AddToDeleteList ( CGarbage* garbage )
{
	m_garbage.push_back ( garbage );
}

// Delete any objects in delete list
void CGarbageCollector::Collect ( )
{
	if ( m_garbage.size () > 0 )
	{
		for ( int i= 0; i < m_garbage.size (); i++ )
		{
			delete m_garbage.at ( i );
		}

		m_garbage.clear ();
	}
}