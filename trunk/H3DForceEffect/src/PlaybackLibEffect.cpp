#include "PlaybackLibEffect.h"

using namespace H3D;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase PlaybackLibEffect::database( "PlaybackLibEffect", 
										   &(newInstance<PlaybackLibEffect>), 
										   typeid( PlaybackLibEffect ),
										   &H3DForceEffect::database );

// Add the x3d field interface.
namespace PlaybackLibEffectInternals 
{
	FIELDDB_ELEMENT( PlaybackLibEffect, moveToPoint, INPUT_OUTPUT );
	FIELDDB_ELEMENT( PlaybackLibEffect, recordTo, INPUT_OUTPUT );
	FIELDDB_ELEMENT( PlaybackLibEffect, playFrom, INPUT_OUTPUT );
	FIELDDB_ELEMENT( PlaybackLibEffect, cancel, INPUT_OUTPUT );
	FIELDDB_ELEMENT( PlaybackLibEffect, hapticDevice, INITIALIZE_ONLY );
}

namespace
{
	// Constants
	// TODO: Make these field values of the node if required

	// Move to point speed
	const double moveToSpeed= 100.0;

	// Recording sample rate
	const double recordRate= 250.0;

	// Recording resolution
	const double recordResolution= 0.0;
}

// The MoveToPointField class handles starting a move-to-point operation
// when a new value is recieved
void PlaybackLibEffect::MoveToPointField::onNewValue( const Vec3f &v )
{
	PlaybackLibEffect* owner= static_cast < PlaybackLibEffect* > ( getOwner() );

	// Cancel previous operations
	owner->cancelAll();

	// Add a move to point operation to the queue and sync
	// Parameters: Position, speed
	CMoveToPointOp* op= owner->hapiPlaybackLibEffect->getPlaybackControl()->AddMoveToPointOp ( v.x, v.y, v.z, moveToSpeed);
	op->SetHoldAtEnd ( true );
	owner->hapiPlaybackLibEffect->syncronise();
}

// The RecordToField class handles starting a record operation
// when a new value is recieved, the value is the recording filename
void PlaybackLibEffect::RecordToField::onNewValue( const string &s )
{
	PlaybackLibEffect* owner= static_cast < PlaybackLibEffect* > ( getOwner() );

	// Cancel previous operations
	owner->cancelAll();

	// Add a record operation to the queue and sync
	// Parameters: Filename, sample rate, resolution
	owner->hapiPlaybackLibEffect->getPlaybackControl()->AddRecordOp ( s.c_str(), recordRate, recordResolution );
	owner->hapiPlaybackLibEffect->syncronise();
}

// The PlayFromField class handles starting a record operation
// when a new value is recieved, the value is the playback filename
void PlaybackLibEffect::PlayFromField::onNewValue( const string &s )
{
	PlaybackLibEffect* owner= static_cast < PlaybackLibEffect* > ( getOwner() );

	// Cancel previous operations
	owner->cancelAll();

	// Add a playback operation to the queue and sync
	// Parameters: Filename, b-spline order (optional)
	owner->hapiPlaybackLibEffect->getPlaybackControl()->AddMoveToPointOp ( s.c_str(), moveToSpeed );
	owner->hapiPlaybackLibEffect->getPlaybackControl()->AddPlaybackOp ( s.c_str() );
	owner->hapiPlaybackLibEffect->syncronise();
}

// The CancelField class handles stoping all playback operations.
// When a value of true is recieved, all operations in queue will be cancelled
void PlaybackLibEffect::CancelField::onNewValue( const bool &b )
{
	if ( b )
	{
		PlaybackLibEffect* owner= static_cast < PlaybackLibEffect* > ( getOwner() );
		owner->cancelAll();
		owner->hapiPlaybackLibEffect->syncronise();
	}
}


// Constructor
PlaybackLibEffect::PlaybackLibEffect(		Inst< SFNode			>  _metadata,
											Inst < MoveToPointField > _moveToPoint,
											Inst < RecordToField	> _recordTo,
											Inst < PlayFromField	> _playFrom,
											Inst < CancelField			> _cancel,
											Inst < SFInt32			> _hapticDevice )
: H3DForceEffect ( _metadata ), moveToPoint ( _moveToPoint ), recordTo ( _recordTo ), 
  playFrom ( _playFrom ), cancel ( _cancel ), hapticDevice ( _hapticDevice )
{
	type_name = "PlaybackLibEffect";
	database.initFields( this );

	// Create HAPI force effect
	hapiPlaybackLibEffect.reset ( new HAPI::HAPIPlaybackLibEffect () );
	
	// Default to haptic device 0
	hapticDevice->setValue ( 0 );
}

// Override traverseSG to add the HAPI force effect
void PlaybackLibEffect::traverseSG( TraverseInfo &ti )
{
	// Add the HAPI for effect to be rendered in this frame
	ti.addForceEffect( hapticDevice->getValue(), hapiPlaybackLibEffect.get() );
}

// Cancel all operations
void PlaybackLibEffect::cancelAll ()
{
	// Sycnronise
	hapiPlaybackLibEffect->syncronise();

	// Cancel all operations in the queue
	vector<COperation*>& ops= *hapiPlaybackLibEffect->getPlaybackControl()->GetOperations();
	for ( vector<COperation*>::iterator i= ops.begin(); i != ops.end(); ++i )
		(*i)->Cancel();
}