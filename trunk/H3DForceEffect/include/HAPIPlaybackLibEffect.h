#ifndef __HAPI_PLAYBACK_LIB_EFFECT_H__
#define __HAPI_PLAYBACK_LIB_EFFECT_H__

#include <HAPI/HAPIForceEffect.h>
#include <Playback.h>
#include "PlaybackEffect.h"

namespace HAPI
{

	/// A HAPIForceEffect which enables control of a haptic device using PlaybackLib
	class PLAYBACKEFFECT_API HAPIPlaybackLibEffect : public HAPIForceEffect
	{
	public:

		/// Constructor
		HAPIPlaybackLibEffect ();

		/// Calculate the forces to send to the device
		virtual EffectOutput calculateForces ( const EffectInput& input );

		/// Syncronise graphic/control thread and haptic thread
		/// Call from graphic/control thread, blocks until syncronised
		void syncronise ();

		/// Return the PlaybaclLib CPlaybackControl object to control the playback and recording
		CPlaybackControl* getPlaybackControl ();

	protected:

		/// Callback used to syncronise graphic/control thread and haptic thread
		static H3DUtil::PeriodicThreadBase::CallbackCode syncroniseCB ( void* data );

		/// Playback control
		auto_ptr < PlaybackLib::CPlaybackControl > playbackControl;

	};

}

#endif // Include guard