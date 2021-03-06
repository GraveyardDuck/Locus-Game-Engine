###########################################################################################################
#                                                                                                         #
#    This file is part of the Locus Game Engine                                                           #
#                                                                                                         #
#    Copyright (c) 2014 Shachar Avni. All rights reserved.                                                #
#                                                                                                         #
#    Use of this file is governed by a BSD-style license. See the accompanying LICENSE.txt for details    #
#                                                                                                         #
###########################################################################################################

cmake_minimum_required(VERSION 2.8)

include(${PROJECT_SOURCE_DIR}/cmake/GlobalProjectOptions.cmake)

if(BUILD_SHARED_LIBS)
	add_definitions(-DLOCUS_GEOMETRY_SHARED)
	add_definitions(-DLOCUS_SHARED)
endif()

include(${PROJECT_SOURCE_DIR}/cmake/UnixOptions.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/MSVCOptions.cmake)

SetUnixOptions(TRUE TRUE)
SetMSVCRuntimeLibrarySettings(TRUE)
SetMSVCWarningLevel4()

set(LOCUS_INCLUDE ${PROJECT_SOURCE_DIR}/include)
set(LOCUS_GEOMETRY_INCLUDE ${LOCUS_INCLUDE}/Locus/Geometry)

include_directories(${LOCUS_INCLUDE})

add_library(Locus_Geometry
            AxisAlignedBox.cpp
            BoundingVolumeHierarchy.cpp
            Collidable.cpp
            CollisionManager.cpp
            DualTransformation.cpp
            EarClipper.cpp
            Frustum.cpp
            Geometry.cpp
            Line.cpp
            LineSegment.cpp
            ModelUtility.cpp
            Moveable.cpp
            Octree.cpp
            OrientedBox.cpp
            Plane.cpp
            PointCloud.cpp
            Polygon.cpp
            PolygonHierarchy.cpp
            PolygonWinding.cpp
            Quaternion.cpp
            Sphere.cpp
            Transformation.cpp
            Triangle.cpp
            Triangulation.cpp
            Vector2Geometry.cpp
            Vector3Geometry.cpp
            ${LOCUS_GEOMETRY_INCLUDE}/AxisAlignedBox.h
            ${LOCUS_GEOMETRY_INCLUDE}/BoundingVolumeHierarchy.h
            ${LOCUS_GEOMETRY_INCLUDE}/Collidable.h
            ${LOCUS_GEOMETRY_INCLUDE}/CollisionManager.h
            ${LOCUS_GEOMETRY_INCLUDE}/DualTransformation.h
            EarClipper.h
            ${LOCUS_GEOMETRY_INCLUDE}/Frustum.h
            ${LOCUS_GEOMETRY_INCLUDE}/Geometry.h
            ${LOCUS_GEOMETRY_INCLUDE}/IntersectionTypes.h
            ${LOCUS_GEOMETRY_INCLUDE}/Line.h
            ${LOCUS_GEOMETRY_INCLUDE}/LineFwd.h
            ${LOCUS_GEOMETRY_INCLUDE}/LineSegment.h
            ${LOCUS_GEOMETRY_INCLUDE}/LineSegmentFwd.h
            ${LOCUS_GEOMETRY_INCLUDE}/LocusGeometryAPI.h
            ${LOCUS_GEOMETRY_INCLUDE}/Model.h
            ${LOCUS_GEOMETRY_INCLUDE}/ModelUtility.h
            ${LOCUS_GEOMETRY_INCLUDE}/MotionProperties.h
            ${LOCUS_GEOMETRY_INCLUDE}/Moveable.h
            ${LOCUS_GEOMETRY_INCLUDE}/Octree.h
            ${LOCUS_GEOMETRY_INCLUDE}/OrientedBox.h
            ${LOCUS_GEOMETRY_INCLUDE}/Plane.h
            ${LOCUS_GEOMETRY_INCLUDE}/PointCloud.h
            ${LOCUS_GEOMETRY_INCLUDE}/Polygon.h
            ${LOCUS_GEOMETRY_INCLUDE}/PolygonFwd.h
            ${LOCUS_GEOMETRY_INCLUDE}/PolygonHierarchy.h
            ${LOCUS_GEOMETRY_INCLUDE}/PolygonWinding.h
            ${LOCUS_GEOMETRY_INCLUDE}/Quaternion.h
            ${LOCUS_GEOMETRY_INCLUDE}/Sphere.h
            ${LOCUS_GEOMETRY_INCLUDE}/Transformation.h
            ${LOCUS_GEOMETRY_INCLUDE}/Triangle.h
            ${LOCUS_GEOMETRY_INCLUDE}/TriangleFwd.h
            ${LOCUS_GEOMETRY_INCLUDE}/Triangulation.h
            ${LOCUS_GEOMETRY_INCLUDE}/Vector2Geometry.h
            ${LOCUS_GEOMETRY_INCLUDE}/Vector3Geometry.h)

target_link_libraries(Locus_Geometry Locus_Common)
target_link_libraries(Locus_Geometry Locus_Math)