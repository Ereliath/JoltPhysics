// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT

#pragma once

#include <Physics/Vehicle/VehicleConstraint.h>
#include <Physics/Vehicle/VehicleController.h>
#include <Physics/Vehicle/VehicleEngine.h>
#include <Physics/Vehicle/VehicleTransmission.h>
#include <Physics/Vehicle/VehicleTrack.h>

namespace JPH {

class PhysicsSystem;

/// WheelSettings object specifically for TrackedVehicleController
class WheelSettingsTV : public WheelSettings
{
public:
	JPH_DECLARE_SERIALIZABLE_VIRTUAL(WheelSettingsTV)

	// See: WheelSettings
	virtual void				SaveBinaryState(StreamOut &inStream) const override;
	virtual void				RestoreBinaryState(StreamIn &inStream) override;

	float						mLongitudinalFriction = 4.0f;				///< Friction in forward direction of tire
	float						mLateralFriction = 2.0f;					///< Friction in sideway direction of tire
};

/// Wheel object specifically for TrackedVehicleController
class WheelTV : public Wheel
{
public:
	/// Constructor
								WheelTV(const WheelSettingsTV &inWheel);

	/// Override GetSettings and cast to the correct class
	const WheelSettingsTV *		GetSettings() const							{ return static_cast<const WheelSettingsTV *>(mSettings.GetPtr()); }

	/// Update the angular velocity of the wheel based on the angular velocity of the track
	void						CalculateAngularVelocity(const VehicleConstraint &inConstraint);

	/// Update the wheel rotation based on the current angular velocity
	void						Update(float inDeltaTime, const VehicleConstraint &inConstraint);

	int							mTrackIndex = -1;							///< Index in mTracks to which this wheel is attached (calculated on initialization)
	float						mCombinedLongitudinalFriction = 0.0f;		///< Combined friction coefficient in longitudinal direction (combines terrain and track)
	float						mCombinedLateralFriction = 0.0f;			///< Combined friction coefficient in lateral direction (combines terrain and track)
	float						mBrakeImpulse = 0.0f;						///< Amount of impulse that the brakes can apply to the floor (excluding friction), spread out from brake impulse applied on track
};

/// Settings of a vehicle with tank tracks
///
/// Default settings are based around what I could find about the M1 Abrams tank.
/// Note to avoid issues with very heavy objects vs very light objects the mass of the tank should be a lot lower (say 10x) than that of a real tank. That means that the engine/brake torque is also 10x less.
class TrackedVehicleControllerSettings : public VehicleControllerSettings
{
public:
	JPH_DECLARE_SERIALIZABLE_VIRTUAL(TrackedVehicleControllerSettings)

	// Constructor
								TrackedVehicleControllerSettings();

	// See: VehicleControllerSettings
	virtual VehicleController *	ConstructController(VehicleConstraint &inConstraint) const override;
	virtual void				SaveBinaryState(StreamOut &inStream) const override;
	virtual void				RestoreBinaryState(StreamIn &inStream) override;

	VehicleEngineSettings		mEngine;									///< The properties of the engine
	VehicleTransmissionSettings	mTransmission;								///< The properties of the transmission (aka gear box)
	VehicleTrackSettings		mTracks[(int)ETrackSide::Num];				///< List of tracks and their properties
};

/// Runtime controller class for vehicle with tank tracks
class TrackedVehicleController : public VehicleController
{
public:
	/// Constructor
								TrackedVehicleController(const TrackedVehicleControllerSettings &inSettings, VehicleConstraint &inConstraint);

	/// Set input from driver
	/// @param inForward Value between -1 and 1 for auto transmission and value between 0 and 1 indicating desired driving direction and amount the gas pedal is pressed
	/// @param inLeftRatio Value between -1 and 1 indicating an extra multiplier to the rotation rate of the left track (used for steering)
	/// @param inRightRatio Value between -1 and 1 indicating an extra multiplier to the rotation rate of the right track (used for steering)
	/// @param inBrake Value between 0 and 1 indicating how strong the brake pedal is pressed
	void						SetDriverInput(float inForward, float inLeftRatio, float inRightRatio, float inBrake) { mForwardInput = inForward; mLeftRatio = inLeftRatio; mRightRatio = inRightRatio; mBrakeInput = inBrake; }

	/// Get current engine state
	const VehicleEngine &		GetEngine() const							{ return mEngine; }

	/// Get current engine state (writable interface, allows you to make changes to the configuration which will take effect the next time step)
	VehicleEngine &				GetEngine()									{ return mEngine; }

	/// Get current transmission state
	const VehicleTransmission &	GetTransmission() const						{ return mTransmission; }

	/// Get current transmission state (writable interface, allows you to make changes to the configuration which will take effect the next time step)
	VehicleTransmission &		GetTransmission()							{ return mTransmission; }

	/// Get the tracks this vehicle has
	const VehicleTracks	&		GetTracks() const							{ return mTracks; }

	/// Get the tracks this vehicle has (writable interface, allows you to make changes to the configuration which will take effect the next time step)
	VehicleTracks &				GetTracks()									{ return mTracks; }

	/// Multiply an angular velocity (rad/s) with this value to get rounds per minute (RPM)
	static constexpr float		cAngularVelocityToRPM = 60.0f / (2.0f * JPH_PI);

protected:
	/// Synchronize angular velocities of left and right tracks according to their ratios
	void						SyncLeftRightTracks();

	// See: VehicleController
	virtual Wheel *				ConstructWheel(const WheelSettings &inWheel) const override { JPH_ASSERT(IsKindOf(&inWheel, JPH_RTTI(WheelSettingsTV))); return new WheelTV(static_cast<const WheelSettingsTV &>(inWheel)); }
	virtual void				PreCollide(float inDeltaTime, PhysicsSystem &inPhysicsSystem) override;
	virtual void				PostCollide(float inDeltaTime, PhysicsSystem &inPhysicsSystem) override;
	virtual bool				SolveLongitudinalAndLateralConstraints(float inDeltaTime) override;
	virtual void				SaveState(StateRecorder &inStream) const override;
	virtual void				RestoreState(StateRecorder &inStream) override;
#ifdef JPH_DEBUG_RENDERER
	virtual void				Draw(DebugRenderer *inRenderer) const override;
#endif // JPH_DEBUG_RENDERER

	// Control information
	float						mForwardInput = 0.0f;						///< Value between -1 and 1 for auto transmission and value between 0 and 1 indicating desired driving direction and amount the gas pedal is pressed
	float						mLeftRatio = 1.0f;							///< Value between -1 and 1 indicating an extra multiplier to the rotation rate of the left track (used for steering)
	float						mRightRatio = 1.0f;							///< Value between -1 and 1 indicating an extra multiplier to the rotation rate of the right track (used for steering)
	float						mBrakeInput = 0.0f;							///< Value between 0 and 1 indicating how strong the brake pedal is pressed

	// Simluation information
	VehicleEngine				mEngine;									///< Engine state of the vehicle
	VehicleTransmission			mTransmission;								///< Transmission state of the vehicle
	VehicleTracks				mTracks;									///< Tracks of the vehicle
};

} // JPH