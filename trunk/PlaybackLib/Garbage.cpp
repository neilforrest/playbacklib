#include "StdAfx.h"
#include "Garbage.h"
#include "GarbageCollector.h"

using namespace PlaybackLib;

CGarbage::CGarbage(void)
{
	// Number of references to this object
	m_referenceCount= 0;
}

CGarbage::~CGarbage(void)
{
}

	// Add a reference to this object
void CGarbage::AddReference ( )
{
	m_referenceCount++;
}

// Remove reference to this object
void CGarbage::RemoveReference ( )
{
	m_referenceCount--;

	if ( m_referenceCount == 0 )
	{
		CGarbageCollector::GetInstance ()->AddToDeleteList ( this );
	}
}
