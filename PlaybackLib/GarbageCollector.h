#pragma once

#include "Garbage.h"
#include <vector>

// The one and only CGarbageCollector to manage CGarbage objects
// Use GetInstance () to access singleton instance
class CGarbageCollector
{
public:

	// Use GetInstance () to access singleton instance
	static CGarbageCollector* GetInstance ( );

	// Delete instance
	virtual ~CGarbageCollector(void);

	// Add an object to be deleted
	void AddToDeleteList ( CGarbage* garbage );

	// Delete any objects in delete list
	void Collect ( );

protected:

	// Protected constructor
	CGarbageCollector(void);

	// List of objects to be deleted
	std::vector<CGarbage*> m_garbage;

};
