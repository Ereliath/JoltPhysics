// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT

#pragma once

#include <Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Physics/Collision/ObjectLayer.h>
#include <Physics/Collision/CollisionCollector.h>
#include <Physics/Body/BodyID.h>
#include <Core/NonCopyable.h>

namespace JPH {

struct RayCast;
class BroadPhaseCastResult;
class AABox;
class OrientedBox;
struct AABoxCast;

// Various collector configurations
using RayCastBodyCollector = CollisionCollector<BroadPhaseCastResult, CollisionCollectorTraitsCastRay>;
using CastShapeBodyCollector = CollisionCollector<BroadPhaseCastResult, CollisionCollectorTraitsCastShape>;
using CollideShapeBodyCollector = CollisionCollector<BodyID, CollisionCollectorTraitsCollideShape>;

/// Interface to the broadphase that can perform collision queries. These queries will only test the bounding box of the body to quickly determine a potential set of colliding bodies
class BroadPhaseQuery : public NonCopyable
{
public:
	/// Virtual destructor
	virtual				~BroadPhaseQuery() { }

	/// Cast a ray and add any hits to ioCollector
	virtual void		CastRay(const RayCast &inRay, RayCastBodyCollector &ioCollector, const BroadPhaseLayerFilter &inBroadPhaseLayerFilter = { }, const ObjectLayerFilter &inObjectLayerFilter = { }) const = 0;

	/// Get bodies intersecting with inBox and any hits to ioCollector
	virtual void		CollideAABox(const AABox &inBox, CollideShapeBodyCollector &ioCollector, const BroadPhaseLayerFilter &inBroadPhaseLayerFilter = { }, const ObjectLayerFilter &inObjectLayerFilter = { }) const = 0;

	/// Get bodies intersecting with a sphere and any hits to ioCollector
	virtual void		CollideSphere(Vec3Arg inCenter, float inRadius, CollideShapeBodyCollector &ioCollector, const BroadPhaseLayerFilter &inBroadPhaseLayerFilter = { }, const ObjectLayerFilter &inObjectLayerFilter = { }) const = 0;

	/// Get bodies intersecting with a point and any hits to ioCollector
	virtual void		CollidePoint(Vec3Arg inPoint, CollideShapeBodyCollector &ioCollector, const BroadPhaseLayerFilter &inBroadPhaseLayerFilter = { }, const ObjectLayerFilter &inObjectLayerFilter = { }) const = 0;

	/// Get bodies intersecting with an oriented box and any hits to ioCollector
	virtual void		CollideOrientedBox(const OrientedBox &inBox, CollideShapeBodyCollector &ioCollector, const BroadPhaseLayerFilter &inBroadPhaseLayerFilter = { }, const ObjectLayerFilter &inObjectLayerFilter = { }) const = 0;

	/// Cast a box and add any hits to ioCollector
	virtual void		CastAABox(const AABoxCast &inBox, CastShapeBodyCollector &ioCollector, const BroadPhaseLayerFilter &inBroadPhaseLayerFilter = { }, const ObjectLayerFilter &inObjectLayerFilter = { }) const = 0;
};

} // JPH