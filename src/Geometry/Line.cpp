 /********************************************************************************************************************************************************\
 *                                                                                                                                                        *
 *   This file is part of the Locus Game Engine                                                                                                           *
 *                                                                                                                                                        *
 *   Copyright (c) 2014 Shachar Avni. All rights reserved.                                                                                                *
 *                                                                                                                                                        *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),   *
 *   to deal in the Software without restriction, including without limitation the rights to use, modify, distribute, and to permit persons to whom the   *
 *   Software is furnished to do so, subject to the following conditions:                                                                                 *
 *                                                                                                                                                        *
 *   a) Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.                      *
 *   b) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the             *
 *      documentation and/or other materials provided with the distribution.                                                                              *
 *                                                                                                                                                        *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,    *
 *   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR              *
 *   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,            *
 *   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF            *
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,    *
 *   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                                                                                                   *
 *                                                                                                                                                        *
 \********************************************************************************************************************************************************/

#include "Locus/Geometry/Line.h"
#include "Locus/Geometry/LineSegment.h"

#include "Locus/Common/Float.h"

#include <assert.h>

namespace Locus
{

template <>
Line<Vector3>::Line()
   : V(1, 0, 0), isRay(false) //avoiding degenerate case by setting the line initially to the X axis.
{
}

template <>
Line<Vector2>::Line()
   : V(1, 0), isRay(false)  //avoiding degenerate case by setting the line initially to the X axis.
{
}

template <class PointType>
Line<PointType>::Line(const PointType& pointOnLine, const PointType& vectorInDirectionOfLine, bool isRay)
   : P(pointOnLine), V(vectorInDirectionOfLine), isRay(isRay)
{
}

template <>
bool Line<Vector2>::IsDegenerate() const
{
   return ( (V.x == 0.0f) && (V.y == 0.0f) );
}

template <>
bool Line<Vector3>::IsDegenerate() const
{
   return ( (V.x == 0.0f) && (V.y == 0.0f) && (V.z == 0.0f) );
}

template <class PointType>
PointType Line<PointType>::GetPointOnLine(float s) const
{
   if (isRay)
   {
      if (Float::FLess<float>(s, 0.0f))
      {
         return P;
      }
   }

   return P + V * s;
}

template <class PointType>
bool Line<PointType>::IsPointOnLine(const PointType& checkPoint, float toleranceFactor) const
{
   bool isOnLine = V.cross(checkPoint - P).ApproximatelyEqualTo(Vector3::ZeroVector(), toleranceFactor);

   if (isRay)
   {
      isOnLine = isOnLine && Float::FGreaterOrEqual<float>( (checkPoint - P).dot(V), 0.0f, toleranceFactor );
   }

   return isOnLine;
}

template <>
IntersectionType Line<Vector3>::GetLineIntersection(const Line<Vector3>& otherLine, Vector3& intersectionPoint1, Vector3& intersectionPoint2, float toleranceFactor) const
{
   if (IsDegenerate() || otherLine.IsDegenerate())
   {
      return IntersectionType::None;
   }
   else
   {
      //from http://geomalgorithms.com/a07-_distance.html

      Vector3 wZero = P - otherLine.P;

      float a = V.dot(V);
      float b = V.dot(otherLine.V);
      float c = otherLine.V.dot(otherLine.V);
      float d = V.dot(wZero);
      float e = otherLine.V.dot(wZero);

      float ACMinusBSquared = a * c - b * b;

      if (ACMinusBSquared == 0.0f)
      {
         return GetParallelLineIntersection(otherLine, intersectionPoint1, intersectionPoint2, toleranceFactor);
      }
      else
      {
         Vector3 closestPointOnThisLine = P + ( (b * e - c * d) / ACMinusBSquared ) * V;
         Vector3 closestPointOnOtherLine = otherLine.P + ( (a * e - b * d) / ACMinusBSquared ) * otherLine.V;

         if (Float::FEqual<float>(closestPointOnThisLine.distanceTo(closestPointOnOtherLine), 0.0f, toleranceFactor))
         {
            if (isRay || otherLine.isRay)
            {
               if (isRay && otherLine.isRay)
               {
                  if (!IsPointOnLine(closestPointOnThisLine, toleranceFactor) || !otherLine.IsPointOnLine(closestPointOnThisLine, toleranceFactor))
                  {
                     return IntersectionType::None;
                  }
               }
               else if (isRay)
               {
                  if (!IsPointOnLine(closestPointOnThisLine, toleranceFactor))
                  {
                     return IntersectionType::None;
                  }
               }
               else
               {
                  if (!otherLine.IsPointOnLine(closestPointOnThisLine, toleranceFactor))
                  {
                     return IntersectionType::None;
                  }
               }
            }

            intersectionPoint1 = closestPointOnThisLine;
            return IntersectionType::Point;
         }
         else
         {
            return IntersectionType::None;
         }
      }
   }
}

template <>
IntersectionType Line<Vector2>::GetLineIntersection(const Line<Vector2>& otherLine, Vector2& intersectionPoint1, Vector2& intersectionPoint2, float toleranceFactor) const
{
   //re-use 3D solution

   Line<Vector3> thisLine3D(P, V, isRay);
   Line<Vector3> otherLine3D(otherLine.P, otherLine.V, otherLine.isRay);

   Vector3 intersectionPoint3D1, intersectionPoint3D2;
   IntersectionType lineIntersection = thisLine3D.GetLineIntersection(otherLine3D, intersectionPoint3D1, intersectionPoint3D2, toleranceFactor);

   if (lineIntersection != IntersectionType::None)
   {
      intersectionPoint1.x = intersectionPoint3D1.x;
      intersectionPoint1.y = intersectionPoint3D1.y;

      if (lineIntersection == IntersectionType::LineSegment)
      {
         intersectionPoint2.x = intersectionPoint3D2.x;
         intersectionPoint2.y = intersectionPoint3D2.y;
      }
   }
      
   return lineIntersection;
}

template <class PointType>
IntersectionType Line<PointType>::GetParallelLineIntersection(const Line<PointType>& otherLine, PointType& intersectionPoint1, PointType& intersectionPoint2, float toleranceFactor) const
{
   if (isRay)
   {
      if (otherLine.isRay)
      {
         bool thisPointIsOnOther = otherLine.IsPointOnLine(P, toleranceFactor);
         bool otherPointIsOnThis = IsPointOnLine(otherLine.P, toleranceFactor);

         if (thisPointIsOnOther || otherPointIsOnThis)
         {
            if (P.ApproximatelyEqualTo(otherLine.P, toleranceFactor))
            {
               if (V.goesTheSameWayAs(otherLine.V))
               {
                  return IntersectionType::Ray;
               }
               else
               {
                  intersectionPoint1 = P;

                  return IntersectionType::Point;
               }
            }
            else
            {
               if (thisPointIsOnOther && otherPointIsOnThis)
               {
                  intersectionPoint1 = P;
                  intersectionPoint2 = otherLine.P;

                  return IntersectionType::LineSegment;
               }
               else if (thisPointIsOnOther)
               {
                  //TODO: which ray?
                  //this ray
                  return IntersectionType::Ray;
               }
               else
               {
                  //TODO: which ray?
                  //other ray
                  return IntersectionType::Ray;
               }
            }
         }
         else
         {
            return IntersectionType::None;
         }
      }
      else
      {
         if (otherLine.IsPointOnLine(P, toleranceFactor))
         {
            //TODO: which ray?
            //this ray
            return IntersectionType::Ray;
         }
         else
         {
            return IntersectionType::None;
         }
      }
   }
   else if (otherLine.isRay)
   {
      if (IsPointOnLine(otherLine.P, toleranceFactor))
      {
         //TODO: which ray?
         //other ray
         return IntersectionType::Ray;
      }
      else
      {
         return IntersectionType::None;
      }
   }
   else
   {
      if (otherLine.IsPointOnLine(P, toleranceFactor))
      {
         //lines are coincident
         return IntersectionType::Line;
      }
      else
      {
         return IntersectionType::None;
      }
   }
}

template <class PointType>
IntersectionType Line<PointType>::LineSegmentIntersection(const LineSegment<PointType>& lineSegment, PointType& intersectionPoint1, PointType& intersectionPoint2, float toleranceFactor) const
{
   Line<PointType> lineSegmentAsCompleteLine = lineSegment.MakeLine(false);

   PointType lineLineIntersectionPoint1, lineLineIntersectionPoint2;
   IntersectionType lineLineIntersectionType = GetLineIntersection(lineSegmentAsCompleteLine, lineLineIntersectionPoint1, lineLineIntersectionPoint2, toleranceFactor);

   if (lineLineIntersectionType == IntersectionType::Point)
   {
      if (lineSegment.PointIsOnLineSegment(lineLineIntersectionPoint1, toleranceFactor))
      {
         intersectionPoint1 = lineLineIntersectionPoint1;
         return IntersectionType::Point;
      }
      else
      {
         return IntersectionType::None;
      }
   }
   else if ( (lineLineIntersectionType == IntersectionType::Line) || (lineLineIntersectionType == IntersectionType::Ray) )
   {
      return GetParallelLineSegmentIntersection(lineSegment, intersectionPoint1, intersectionPoint2, toleranceFactor);
   }
   else if (lineLineIntersectionType == IntersectionType::LineSegment)
   {
      intersectionPoint1 = lineLineIntersectionPoint1;
      intersectionPoint2 = lineLineIntersectionPoint2;

      return IntersectionType::LineSegment;
   }
   else
   {
      return IntersectionType::None;
   }
}

template <class PointType>
IntersectionType Line<PointType>::GetParallelLineSegmentIntersection(const LineSegment<PointType>& lineSegment, PointType& intersectionPoint1, PointType& intersectionPoint2, float toleranceFactor) const
{
   if (isRay)
   {
      bool firstPointIsOn = IsPointOnLine(lineSegment.P1, toleranceFactor);
      bool secondPointIsOn = IsPointOnLine(lineSegment.P2, toleranceFactor);

      if (firstPointIsOn || secondPointIsOn)
      {
         if (firstPointIsOn && secondPointIsOn)
         {
            intersectionPoint1 = lineSegment.P1;
            intersectionPoint2 = lineSegment.P2;
         }
         else if (firstPointIsOn)
         {
            intersectionPoint1 = lineSegment.P1;
            intersectionPoint2 = P;
         }
         else
         {
            intersectionPoint1 = lineSegment.P2;
            intersectionPoint2 = P;
         }

         return IntersectionType::LineSegment;
      }
      else
      {
         return IntersectionType::None;
      }
   }
   else
   {
      intersectionPoint1 = lineSegment.P1;
      intersectionPoint2 = lineSegment.P2;

      return IntersectionType::LineSegment;
   } 
}

template class LOCUS_GEOMETRY_API_AT_DEFINITION Line<Vector3>;
template class LOCUS_GEOMETRY_API_AT_DEFINITION Line<Vector2>;

}
