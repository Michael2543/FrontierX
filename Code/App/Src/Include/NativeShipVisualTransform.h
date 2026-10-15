#ifndef FRONTIERX_NATIVE_SHIP_VISUAL_TRANSFORM_H
#define FRONTIERX_NATIVE_SHIP_VISUAL_TRANSFORM_H

#if !defined(_WIN32)
#include <3dmath.h>

// SpaceShip::renderSpaceShip applies these original model-space rotations
// only on its direct Engine hull path. Keep attached presentation in that
// same space without changing the simulation transform or authored hardpoints.
inline Transform NativeShipModelAxisCorrection()
{
    Transform correction;
    correction.set_identity();
    correction.rotate_about_i(90 * MUL_DEG_TO_RAD);
    correction.rotate_about_j(180 * MUL_DEG_TO_RAD);
    return correction;
}

inline Transform NativeShipVisualTransform(const Transform &world,
                                         bool directEngineHull)
{
    return directEngineHull ? world * NativeShipModelAxisCorrection() : world;
}

inline Transform NativeShipHardpointTransform(const Transform &shipWorld,
                                             const Transform &nodeWorld,
                                             const Transform &hardpoint,
                                             bool correctedModelAxes)
{
    if (!correctedModelAxes) return nodeWorld * hardpoint;
    // Ship simulation roots are rigid. Recover the complete child frame,
    // including its authored scale, then insert the hull's model correction
    // once at the root rather than rotating an already-world-space muzzle.
    // This is the authored model frame, independent of graphics preferences.
    const Transform nodeLocal = shipWorld.get_inverse() * nodeWorld;
    return NativeShipVisualTransform(shipWorld, true) * nodeLocal * hardpoint;
}

inline Transform NativeShipSpeakerMarkerTransform(const Transform &world,
                                                  const Vector &center,
                                                  float angle)
{
    // Spin in the current ship XY plane about the marker's own center.
    // A J-axis rotation tilts this quad and swings its below-hull offset.
    Transform marker;
    marker.set_identity();
    marker.rotate_about_k(angle);
    marker.translation = center;
    return world * marker;
}

inline void NativeShipSelectionBounds(const float modelBox[6], bool directEngineHull,
                                      float selectionBox[6])
{
    if (!directEngineHull)
    {
        for (unsigned axis = 0; axis < 6; ++axis) selectionBox[axis] = modelBox[axis];
        return;
    }
    const Transform correction = NativeShipModelAxisCorrection();
    for (unsigned corner = 0; corner < 8; ++corner)
    {
        const Vector point = correction * Vector(modelBox[(corner & 1U) ? 0 : 1],
            modelBox[(corner & 2U) ? 2 : 3], modelBox[(corner & 4U) ? 4 : 5]);
        const float coordinates[3] = {point.x, point.y, point.z};
        for (unsigned axis = 0; axis < 3; ++axis)
        {
            if (!corner || coordinates[axis] > selectionBox[axis * 2])
                selectionBox[axis * 2] = coordinates[axis];
            if (!corner || coordinates[axis] < selectionBox[axis * 2 + 1])
                selectionBox[axis * 2 + 1] = coordinates[axis];
        }
    }
}
#endif

#endif
