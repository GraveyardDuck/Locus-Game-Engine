/********************************************************************************************************\
*                                                                                                        *
*   This file is part of the Locus Game Engine                                                           *
*                                                                                                        *
*   Copyright (c) 2014 Shachar Avni. All rights reserved.                                                *
*                                                                                                        *
*   Use of this file is governed by a BSD-style license. See the accompanying LICENSE.txt for details    *
*                                                                                                        *
\********************************************************************************************************/

#include "Locus/Geometry/Geometry.h"
#include "Locus/Geometry/MotionProperties.h"
#include "Locus/Geometry/Sphere.h"
#include "Locus/Geometry/Vector3Geometry.h"

#include "Locus/Common/Float.h"

#include "Locus/Math/Matrix.h"

#include <algorithm>

namespace Locus
{

const float PI = 3.141592653589793238f;
const float TWO_PI = 2.0f * PI;
const float HALF_PI = 0.5f * PI;
const float TO_RADIANS = 0.017453292519943295766666666666667f;
const float TO_DEGREES = 57.295779513082320885235758349777f;

void ResolveCollision(float coefficientOfRestitution,
                      const Sphere& sphere1, const Sphere& sphere2, const FVector3& collisionPoint,
                      const FVector3& impulseDirection, MotionProperties& motionProperties1, MotionProperties& motionProperties2)
{
#define CLAMP_RESOLVED_SPEEDS

   Sphere boundingSpheres[2] = {sphere1, sphere2};

   //static const float_t coefficientOfRestitution = -0.3f; // completely elastic

   //a) compute velocities at the point of contact

   bool hasAngularComponent[2] = { ( FNotZero<float>(motionProperties1.speed) && !ApproximatelyEqual(motionProperties1.rotation, Vec3D::ZeroVector()) ),
                                   ( FNotZero<float>(motionProperties2.speed) && !ApproximatelyEqual(motionProperties2.rotation, Vec3D::ZeroVector()) ) };

   FVector3 contactVectors[2] = { collisionPoint - boundingSpheres[0].center, collisionPoint - boundingSpheres[1].center };

   FVector3 velocities[2] = { motionProperties1.direction * motionProperties1.speed, motionProperties2.direction * motionProperties2.speed };

   FVector3 angularVelocities[2] = { motionProperties1.rotation * motionProperties1.angularSpeed, motionProperties2.rotation * motionProperties2.angularSpeed};

   FVector3 contactVelocities[2] = { velocities[0] + (hasAngularComponent[0] ? Cross(angularVelocities[0], contactVectors[0]) : Vec3D::ZeroVector()),
                                     velocities[1] + (hasAngularComponent[1] ? Cross(angularVelocities[1], contactVectors[1]) : Vec3D::ZeroVector()) };

   //b) compute inverses of inertia tensors

   //For now, use spheres as approximations. The inertia tensor for a sphere is:
   //
   //[ (2/5)mr^2       0          0     ]
   //[      0     (2/5)mr^2       0     ]
   //[      0          0     (2/5)mr^2  ]
   //
   //where the inverse is:
   //
   //[ 5 / (2 * mr^2)        0               0         ]
   //[       0         5 / (2 * mr^2)        0         ]
   //[       0               0         5 / (2 * mr^2)  ]

   float masses[2] = { boundingSpheres[0].Volume(), boundingSpheres[1].Volume() }; //assume density of asteroid is 1 Kg per cubic metre

   Matrix<float> inertiaTensorInverses[2] = {Matrix<float>(3, 3), Matrix<float>(3, 3)};

   for (int asteroidIndex = 0; asteroidIndex < 2; ++asteroidIndex)
   {
      float inertiaTensorInverseVal = 5.0f / (2.0f * masses[asteroidIndex] * boundingSpheres[asteroidIndex].radius * boundingSpheres[asteroidIndex].radius);

      inertiaTensorInverses[asteroidIndex](0, 0) = inertiaTensorInverseVal;
      inertiaTensorInverses[asteroidIndex](1, 1) = inertiaTensorInverseVal;
      inertiaTensorInverses[asteroidIndex](2, 2) = inertiaTensorInverseVal;
   }

   //c) determine direction of impulse

   //TODO: improve this

   //Vector3 impulseDirection = -contactVectors[1];
   
   //Vector3 impulseDirection = boundingSpheres[1].center - boundingSpheres[0].center;

   //impulseDirection.normalize();

   //NOTE: For the subsequent steps to work, the impulse direction must be from asteroid1 towards asteroid2

   //d) determine the impulse magnitude

   FVector3 crossA = Cross(contactVectors[0], impulseDirection);
   FVector3 multA = inertiaTensorInverses[0] * std::vector<float>{crossA.x, crossA.y, crossA.z};

   FVector3 crossB = Cross(contactVectors[1], impulseDirection);
   FVector3 multB = inertiaTensorInverses[1] * std::vector<float>{crossB.x, crossB.y, crossB.z};

   float impulseMagnitudeDenominator = (1.0f / masses[0]) + (1.0f / masses[1]) + Dot(Cross(multA, contactVectors[0]) + Cross(multB, contactVectors[1]), impulseDirection);

   float impulseMagnitude =  ( Dot(-(1 + coefficientOfRestitution) * (contactVelocities[1] - contactVelocities[0]), impulseDirection) ) / impulseMagnitudeDenominator;

   //e) compute reaction impulse vector

   FVector3 impulseVector = impulseMagnitude * impulseDirection;

   //f) compute new linear velocities

#ifdef CLAMP_RESOLVED_SPEEDS
   float minSpeed = std::min(motionProperties1.speed, motionProperties2.speed);
   float maxSpeed = std::max(motionProperties1.speed, motionProperties2.speed);
   //float_t maxSpeed = maxResolvedSpeed;
   float minRotationSpeed = std::min(motionProperties1.angularSpeed, motionProperties2.angularSpeed);
   float maxRotationSpeed = std::max(motionProperties1.angularSpeed, motionProperties2.angularSpeed);
#endif

   motionProperties1.direction = velocities[0] - (impulseVector / masses[0]);
   motionProperties1.speed = Norm(motionProperties1.direction);

#ifdef CLAMP_RESOLVED_SPEEDS
   if (FGreater<float>(motionProperties1.speed, maxSpeed))
   {
      motionProperties1.speed = maxSpeed;
   }
   else if (FLess<float>(motionProperties1.speed, minSpeed))
   {
      motionProperties1.speed = minSpeed;
   }
#endif

   Normalize(motionProperties1.direction);

   motionProperties2.direction = velocities[1] + (impulseVector / masses[1]);
   motionProperties2.speed = Norm(motionProperties2.direction);

#ifdef CLAMP_RESOLVED_SPEEDS
   if (FGreater<float>(motionProperties2.speed, maxSpeed))
   {
      motionProperties2.speed = maxSpeed;
   }
   else if (FLess<float>(motionProperties2.speed, minSpeed))
   {
      motionProperties2.speed = minSpeed;
   }
#endif
      
   Normalize(motionProperties2.direction);

   //g) compute new angular velocities

   motionProperties1.rotation = angularVelocities[0] - impulseMagnitude * ( multA );
   motionProperties1.angularSpeed = Norm(motionProperties1.rotation);
 
#ifdef CLAMP_RESOLVED_SPEEDS
   if (FGreater<float>(motionProperties1.angularSpeed, maxRotationSpeed))
   {
      motionProperties1.angularSpeed = maxRotationSpeed;
   }
   else if (FLess<float>(motionProperties1.angularSpeed, minRotationSpeed))
   {
      motionProperties1.angularSpeed = minRotationSpeed;
   }
#endif

   Normalize(motionProperties1.rotation);

   motionProperties2.rotation = angularVelocities[1] + impulseMagnitude * ( multB );
   motionProperties2.angularSpeed = Norm(motionProperties2.rotation);

#ifdef CLAMP_RESOLVED_SPEEDS
   if (FGreater<float>(motionProperties2.angularSpeed, maxRotationSpeed))
   {
      motionProperties2.angularSpeed = maxRotationSpeed;
   }
   else if (FLess<float>(motionProperties2.angularSpeed, minRotationSpeed))
   {
      motionProperties2.angularSpeed = minRotationSpeed;
   }
#endif

   Normalize(motionProperties2.rotation);
}

}