#pragma once

#ifdef PB_IMPORT
#define PB_API __declspec(dllimport)
#else
#define PB_API __declspec(dllexport)
#endif

namespace PlaybackLib
{

	// An object under the control of a CGarbageCollector
	class PB_API CGarbage
	{
	public:
		CGarbage(void);
		virtual ~CGarbage(void);

		// Add a reference to this object
		void AddReference ( );

		// Remove reference to this object
		void RemoveReference ( );

	protected:

		// Number of references to this object
		int m_referenceCount;
	};
}