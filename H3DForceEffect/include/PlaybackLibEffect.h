#ifndef __PLAYBACK_LIB_EFFECT_H__
#define __PLAYBACK_LIB_EFFECT_H__

#include <H3D/H3DForceEffect.h>
#include <H3D/SFVec3f.h>
#include <H3D/SFString.h>
#include <H3D/SFInt32.h>
#include <H3D/SFBool.h>
#include "PlaybackEffect.h"
#include "HAPIPlaybackLibEffect.h"

namespace H3D
{

	/// A H3DForceEffect providing control of a haptic device using PlaybackLib 
	class PLAYBACKEFFECT_API PlaybackLibEffect : public H3DForceEffect
	{
	public:

		/// \brief The MoveToPointField class handles starting a move-to-point operation
		/// when a new value is recieved
		class MoveToPointField: public OnNewValueSField< AutoUpdate< SFVec3f > > {
			virtual void onNewValue( const Vec3f &v );
		};

		/// \brief The RecordToField class handles starting a record operation
		/// when a new value is recieved, the value is the recording filename
		class RecordToField: public OnNewValueSField< AutoUpdate< SFString > > {
			virtual void onNewValue( const string &s );
		};

		/// \brief The PlayFromField class handles starting a record operation
		/// when a new value is recieved, the value is the playback filename
		class PlayFromField: public OnNewValueSField< AutoUpdate< SFString > > {
			virtual void onNewValue( const string &s );
		};

		/// \brief The CancelField class handles stoping all playback operations.
		/// When a value of true is recieved, all operations in queue will be cancelled
		class CancelField: public OnNewValueSField< AutoUpdate< SFBool > > {
			virtual void onNewValue( const bool &b );
		};

		/// Constructor
		PlaybackLibEffect(		Inst< SFNode >  _metadata	= 0,
								Inst < MoveToPointField > _moveToPoint	= 0,
								Inst < RecordToField > _recordTo		= 0,
								Inst < PlayFromField > _playFrom		= 0,
                Inst < SFBool > _holdAtEnd = 0,
								Inst < CancelField > _cancel		= 0,
								Inst < SFInt32 > _hapticDevice	= 0,
                Inst < SFBool > _complete = 0 );

    /// Initialize the node
    virtual void initialize ();

		/// Override traverseSG to add the HAPI force effect
		virtual void traverseSG( TraverseInfo &ti );

		/// \brief Specifies a point to move the haptic device to.
		/// When a new value is recieved a move-to-point operation is added to the queue.
		auto_ptr < MoveToPointField > moveToPoint;

		/// \brief Specifies a file to record movement of the haptic device. 
		/// When a new value is recieved a record operation is added to the queue.
		auto_ptr < RecordToField > recordTo;

		/// \brief Specifies a file to play back movement of the haptic device. 
		/// When a new value is recieved a record operation is added to the queue.
		auto_ptr < PlayFromField > playFrom;

    /// If true then the device will be held at the last position of the previous playback
    /// or move to operation until cancel is set to true
    auto_ptr < SFBool > holdAtEnd;

		/// The index of the haptic device which is used with this effect
		auto_ptr < SFInt32 > hapticDevice;

		/// Sending a value of true requests that all playback operations in queue are cancelled
		auto_ptr < CancelField > cancel;

    /// A value of true is sent each time an operation completes
    /// I.e. when the devices finishes the requested playback operation
    auto_ptr < SFBool > complete;

		/// Add this node to the H3DNodeDatabase system.
		static H3DNodeDatabase database;

	protected:

		/// Cancel all operations
		void cancelAll ();

    void syncronise();

		/// HAPI force effect
		AutoRef < HAPI::HAPIPlaybackLibEffect > hapiPlaybackLibEffect;

    /// The current operation
    COperation* operation;

    /// Reference to haptics device using force effect
    /// Work around for problems with synchronousHapticCB()
    H3DHapticsDevice* device;
	};

}

#endif // Include guard