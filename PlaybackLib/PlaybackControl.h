#pragma once
#include "Operation.h"
#include "MoveToPointOp.h"
#include "RecordOp.h"
#include "PlaybackOp.h"
#include "PlaybackDevice.h"
#include <vector>

#ifdef PB_IMPORT
#define PB_API __declspec(dllimport)
#else
#define PB_API __declspec(dllexport)
#endif

class PB_API CPlaybackControl
{
public:
	CPlaybackControl(void);
	virtual ~CPlaybackControl(void);

	// Add a new operation to the queue
	void AddOperation ( COperation* op );

	// Create and add a move to point operation to the queue
	CMoveToPointOp* AddMoveToPointOp ( double x, double y, double z, double speed );

	// Create and add a move to point operation to the queue
	CMoveToPointOp* AddMoveToPointOp ( char* filename, double speed );

	// Create and add a record operation to the queue
	CRecordOp* AddRecordOp ( char* filename, double sampleRate, double resolution );

	// Create and add a playback operation to the queue
	CPlaybackOp* AddPlaybackOp ( char* filename, int bSplineOrder= 3 );

	// Create a new device object that is controlled by this control object
	CPlaybackDevice* GetPlaybackDevice ( );

	// Get queue of operations
	std::vector<COperation*>* GetOperations ( );

	// Get newley added operations
	std::vector<COperation*>* GetAddedOperations ( );

protected:

	// Singleton device observer for this controller
	CPlaybackDevice* m_playbackDevice;

	// Queue of operations
	std::vector<COperation*> m_operationQueue;

	// Newley added operations
	std::vector<COperation*> m_addedOperations;
};
