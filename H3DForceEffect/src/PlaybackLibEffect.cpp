#include "PlaybackLibEffect.h"
#include <H3D/DeviceInfo.h>

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
  FIELDDB_ELEMENT( PlaybackLibEffect, holdAtEnd, INPUT_OUTPUT );
	FIELDDB_ELEMENT( PlaybackLibEffect, cancel, INPUT_OUTPUT );
	FIELDDB_ELEMENT( PlaybackLibEffect, hapticDevice, INITIALIZE_ONLY );
  FIELDDB_ELEMENT( PlaybackLibEffect, complete, OUTPUT_ONLY );
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

  if ( owner->operation ) {
    owner->operation->RemoveReference();
  }

	// Add a move to point operation to the queue and sync
	// Parameters: Position, speed
  CMoveToPointOp* op= owner->hapiPlaybackLibEffect->getPlaybackControl()->AddMoveToPointOp ( v.x, v.y, v.z, moveToSpeed);
  owner->operation= op;
  owner->operation->AddReference();
  op->SetHoldAtEnd ( owner->holdAtEnd->getValue() );
  owner->syncronise();

  owner->complete->setValue ( false, owner->id );
}

// The RecordToField class handles starting a record operation
// when a new value is recieved, the value is the recording filename
void PlaybackLibEffect::RecordToField::onNewValue( const string &s )
{
	PlaybackLibEffect* owner= static_cast < PlaybackLibEffect* > ( getOwner() );

	// Cancel previous operations
	owner->cancelAll();

  if ( owner->operation ) {
    owner->operation->RemoveReference();
  }

	// Add a record operation to the queue and sync
	// Parameters: Filename, sample rate, resolution
	owner->operation= owner->hapiPlaybackLibEffect->getPlaybackControl()->AddRecordOp ( s.c_str(), recordRate, recordResolution );
  owner->operation->AddReference();

  owner->syncronise();

  owner->complete->setValue ( false, owner->id );
}

// The PlayFromField class handles starting a record operation
// when a new value is recieved, the value is the playback filename
void PlaybackLibEffect::PlayFromField::onNewValue( const string &s )
{
	PlaybackLibEffect* owner= static_cast < PlaybackLibEffect* > ( getOwner() );

	// Cancel previous operations
	owner->cancelAll();

  if ( owner->operation ) {
    owner->operation->RemoveReference();
  }

	// Add a playback operation to the queue and sync
	// Parameters: Filename, b-spline order (optional)
	owner->hapiPlaybackLibEffect->getPlaybackControl()->AddMoveToPointOp ( s.c_str(), moveToSpeed );
  CPlaybackOp* op= owner->hapiPlaybackLibEffect->getPlaybackControl()->AddPlaybackOp ( s.c_str() );
  op->SetHoldAtEnd ( owner->holdAtEnd->getValue() );
  owner->operation= op;
  owner->operation->AddReference();

  owner->syncronise();

  owner->complete->setValue ( false, owner->id );
}

// The CancelField class handles stoping all playback operations.
// When a value of true is recieved, all operations in queue will be cancelled
void PlaybackLibEffect::CancelField::onNewValue( const bool &b )
{
	if ( b )
	{
		PlaybackLibEffect* owner= static_cast < PlaybackLibEffect* > ( getOwner() );

    if ( owner->operation ) {
      owner->operation->RemoveReference();
      owner->operation= NULL;
    }

		owner->cancelAll();
    owner->syncronise();
	}
}


// Constructor
PlaybackLibEffect::PlaybackLibEffect(		Inst< SFNode			>  _metadata,
											Inst < MoveToPointField > _moveToPoint,
											Inst < RecordToField	> _recordTo,
											Inst < PlayFromField	> _playFrom,
                      Inst < SFBool > _holdAtEnd,
											Inst < CancelField			> _cancel,
											Inst < SFInt32			> _hapticDevice,
                      Inst < SFBool > _complete )
: H3DForceEffect ( _metadata ), moveToPoint ( _moveToPoint ), recordTo ( _recordTo ), 
  playFrom ( _playFrom ), holdAtEnd ( _holdAtEnd ), cancel ( _cancel ), 
  hapticDevice ( _hapticDevice ), complete ( _complete ),
  operation ( NULL ),
  device ( NULL )
{
	type_name = "PlaybackLibEffect";
	database.initFields( this );
	
	// Default to haptic device 0
	hapticDevice->setValue ( 0 );
  complete->setValue ( false, id );
  holdAtEnd->setValue ( true );
}

// Initialize the node
void PlaybackLibEffect::initialize () {
  H3DForceEffect::initialize ();

  HAPI::HAPIPlaybackLibEffect* playbackEffect= NULL;

  if ( DeviceInfo* di= DeviceInfo::getActive() ) {
    const H3DInt32& i= hapticDevice->getValue();
    if ( i >= 0 && i < (H3DInt32)di->device->size() ) {
      H3DHapticsDevice* d= static_cast<H3DHapticsDevice*>((*di->device)[i]);
      
      // Save device ptr for workaround for synchronousHapticCB() problems 
      device= d;

      string deviceType= d->getTypeName();
      if ( deviceType == "FalconDevice" ) {
        playbackEffect= new HAPI::HAPIPlaybackLibEffect ( CPIDControlFalcon() );
      } else if ( deviceType == "PhantomDevice" ) {
        playbackEffect= new HAPI::HAPIPlaybackLibEffect ( CPIDControlPhantomPremium() );
      } else {
        Console (4) << "Warning: " << getName() << ": Unknown device type " << deviceType << ". Using default PID parameters." << endl;
      }
    } else {
      Console (4) << "Warning: " << getName() << ": Invalid device index " << i << ". Using default PID parameters." << endl;
    }
  } else {
    Console (4) << "Warning: " << getName() << ": No active devices. Using default PID parameters." << endl;
  }

  if ( !playbackEffect ) {
    playbackEffect= new HAPI::HAPIPlaybackLibEffect ();
  }

	// Create HAPI force effect
	hapiPlaybackLibEffect.reset ( playbackEffect );
}

// Override traverseSG to add the HAPI force effect
void PlaybackLibEffect::traverseSG( TraverseInfo &ti )
{
  syncronise();

	// Add the HAPI for effect to be rendered in this frame
	ti.addForceEffect( hapticDevice->getValue(), hapiPlaybackLibEffect.get() );

  if ( operation ) {
    // TODO: Implement common interface to IsHoldingAtEnd() state
    // TODO: Automatic handling of reference counting
    CMoveToPointOp* moveOp= dynamic_cast<CMoveToPointOp*>(operation);
    CPlaybackOp* playOp= dynamic_cast<CPlaybackOp*>(operation);
    if ( (moveOp && moveOp->IsHoldingAtEnd()) ||
         (playOp && playOp->IsHoldingAtEnd()) ||
         operation->GetState() == COperation::Completed ) {
      complete->setValue ( true, id );
      operation->RemoveReference();
      operation= NULL;
    }
  }
}

// Cancel all operations
void PlaybackLibEffect::cancelAll ()
{
	// Sycnronise
  syncronise();

	// Cancel all operations in the queue
	vector<COperation*>& ops= *hapiPlaybackLibEffect->getPlaybackControl()->GetOperations();
	for ( vector<COperation*>::iterator i= ops.begin(); i != ops.end(); ++i )
		(*i)->Cancel();
}

void PlaybackLibEffect::syncronise() {
  if ( device && device->getHAPIDevice() ) {
    hapiPlaybackLibEffect->syncronise ( *device->getHAPIDevice() );
  }
}