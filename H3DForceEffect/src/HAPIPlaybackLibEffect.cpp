#include <HAPI/HAPIHapticsDevice.h>
#include "HAPIPlaybackLibEffect.h"

using namespace HAPI;
using namespace PlaybackLib;

// Constructor
HAPIPlaybackLibEffect::HAPIPlaybackLibEffect ( CPIDControlParameters& pidParams )
{
	playbackControl.reset ( new PlaybackLib::CPlaybackControl() );

	// Set PID parameters appropriate for the haptic device
	// TODO: Set this automatically based on H3D device type
	playbackControl->SetPIDParameters ( pidParams );
}

// Calculate the forces to send to the device
HAPIForceEffect::EffectOutput HAPIPlaybackLibEffect::calculateForces ( const HAPIForceEffect::EffectInput& input )
{
	Vec3 hdPos= input.hd->getPosition();

	double playbackForce[3];

	// Convert device position to mm, in order to keep the default tolerance settings etc.
	double devicePosition[3]= { hdPos.x*1000.0, hdPos.y*1000.0, hdPos.z*1000.0 };
	playbackControl->GetPlaybackDevice()->GetForce ( playbackForce, devicePosition );

	return EffectOutput ( Vec3 ( playbackForce[0], playbackForce[1], playbackForce[2] ) );
}

// Syncronise graphic/control thread and haptic thread
// Call from graphic/control thread, blocks until syncronised
void HAPIPlaybackLibEffect::syncronise ( HAPIHapticsDevice& device )
{
  // Disabled: Causing random crashes (H3D2.1.1/PDD 4.2.49)
	//H3DUtil::HapticThread::synchronousHapticCB ( HAPIPlaybackLibEffect::syncroniseCB, this );
  if ( H3DUtil::PeriodicThreadBase* t= device.getThread() ) {
    t->synchronousCallback ( HAPIPlaybackLibEffect::syncroniseCB, this );
  }
}

// Return the PlaybaclLib CPlaybackControl object to control the playback and recording
CPlaybackControl* HAPIPlaybackLibEffect::getPlaybackControl ()
{
	return playbackControl.get();
}

// Callback used to syncronise graphic/control thread and haptic thread
H3DUtil::PeriodicThreadBase::CallbackCode HAPIPlaybackLibEffect::syncroniseCB ( void* data )
{
	HAPIPlaybackLibEffect* pThis= static_cast<HAPIPlaybackLibEffect*>(data);
	pThis->playbackControl->GetPlaybackDevice()->Syncronise ( );

	return H3DUtil::PeriodicThreadBase::CALLBACK_DONE;
}