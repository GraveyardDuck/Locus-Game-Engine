/********************************************************************************************************\
*                                                                                                        *
*   This file is part of the Locus Game Engine                                                           *
*                                                                                                        *
*   Copyright (c) 2014 Shachar Avni. All rights reserved.                                                *
*                                                                                                        *
*   Use of this file is governed by a BSD-style license. See the accompanying LICENSE.txt for details    *
*                                                                                                        *
\********************************************************************************************************/

#include "Locus/Rendering/Mesh.h"
#include "Locus/Rendering/GPUVertexData.h"
#include "Locus/Rendering/RenderingState.h"

#include "Locus/Common/Util.h"

#include <Locus/Rendering/Locus_glew.h>

namespace Locus
{

Mesh::Mesh()
   : Model<MeshVertexIndexer, MeshVertex>(), colors(1, Color::White())
{
}

Mesh::Mesh(const std::vector<std::vector<MeshVertex>>& faceTriangles)
   : Model<MeshVertexIndexer, MeshVertex>(), colors(1, Color::White())
{
   std::vector<std::vector<MeshVertex>> faceTrianglesActual;
   Model::GetTriangles(faceTriangles, faceTrianglesActual);

   Construct(faceTrianglesActual);
}

void Mesh::SetColor(const Color& color)
{
   //only allow one color for now

   colors.clear();
   colors.resize(1, color);
   colors.shrink_to_fit();
}

void Mesh::AddTextureCoordinate(const TextureCoordinate& tc)
{
   textureCoordinates.push_back(tc);
}

void Mesh::Construct(const std::vector<std::vector<MeshVertex>>& faceTriangles, std::vector<std::size_t>* degenerateFaceIndices)
{
   std::vector<std::size_t> degenerateFaceIndicesFromModel;
   Model::Construct(faceTriangles, &degenerateFaceIndicesFromModel);

   textureCoordinates.clear();

   std::size_t numFaces = faceTriangles.size();

   std::size_t numDegenerateFaces = degenerateFaceIndicesFromModel.size();
   bool hasAnyMoreDegenerateFaces = numDegenerateFaces > 0;
   std::size_t currentDegenerateFaceIndexIndex = 0;

   std::vector<TextureCoordinate> vertTexCoords;
   vertTexCoords.reserve(numFaces * Triangle3D_t::NumPointsOnATriangle);

   for (std::size_t faceIndex = 0; faceIndex < numFaces; ++faceIndex)
   {
      bool shouldAddFace = !hasAnyMoreDegenerateFaces;

      if (hasAnyMoreDegenerateFaces)
      {
         //degenerateFaceIndicesFromModel is guaranteed to come in sorted order
         shouldAddFace = degenerateFaceIndicesFromModel[currentDegenerateFaceIndexIndex] != faceIndex;

         if (!shouldAddFace)
         {
            ++currentDegenerateFaceIndexIndex;

            if (currentDegenerateFaceIndexIndex == numDegenerateFaces)
            {
               hasAnyMoreDegenerateFaces = false;
            }
         }
      }

      if (shouldAddFace)
      {
         for (std::size_t i = 0; i < Triangle3D_t::NumPointsOnATriangle; ++i)
         {
            vertTexCoords.push_back(faceTriangles[faceIndex][i].textureCoordinate);
         }
      }
   }

   //get unique texture coordinates
   std::vector<std::size_t> sortedTexCoordIndices;
   Util::GetUniqueItems<TextureCoordinate>(vertTexCoords, textureCoordinates, sortedTexCoordIndices);

   //fill faces

   std::size_t numVertices = 0;
   for (std::size_t faceIndex = 0, numFaces = faces.size(); faceIndex < numFaces; ++faceIndex)
   {
      for (std::size_t vertexIndex = 0; vertexIndex < Triangle3D_t::NumPointsOnATriangle; ++vertexIndex)
      {
         faces[faceIndex][vertexIndex].textureCoordinateID = sortedTexCoordIndices[numVertices];
         ++numVertices;
      }
   }

   if (degenerateFaceIndices != nullptr)
   {
      *degenerateFaceIndices = degenerateFaceIndicesFromModel;
   }
}

MeshVertex Mesh::GetVertex(std::size_t faceIndex, int vertexIndex) const
{
   MeshVertex vertex;

   vertex.textureCoordinate = textureCoordinates[faces[faceIndex][vertexIndex].textureCoordinateID];

   return vertex;
}

void Mesh::AssignNormals()
{
   std::size_t numPositions = positions.size();

   normals.clear();
   normals.resize(numPositions);

   std::vector<std::size_t> normalHitCounts(numPositions);

   Vector3 faceNormal;

   for (face_t& face : faces)
   {
      faceNormal = (positions[face[1].positionID] - positions[face[0].positionID]).cross(positions[face[2].positionID] - positions[face[1].positionID]);

      for (std::size_t vertexIndex = 0; vertexIndex < Triangle3D_t::NumPointsOnATriangle; ++vertexIndex)
      {
         face[vertexIndex].normalID = face[vertexIndex].positionID;

         normals[face[vertexIndex].positionID] += faceNormal;
         ++normalHitCounts[face[vertexIndex].positionID];
      }
   }

   for (std::size_t normalIndex = 0; normalIndex < numPositions; ++normalIndex)
   {
      normals[normalIndex] /= static_cast<float>(normalHitCounts[normalIndex]);
   }
}

void Mesh::Clear()
{
   Model::Clear();

   Util::ClearAndShrink(normals);
   Util::ClearAndShrink(textureCoordinates);
}

void Mesh::SetGPUVertexAttributes(ShaderController& ShaderController) const
{
   gpuVertexData->SetAttributes(ShaderController);
}

void Mesh::BindGPUVertexData() const
{
   gpuVertexData->Bind();
}

void Mesh::UpdateGPUVertexData()
{
   if (numTotalVertices > 0)
   {
      if (gpuVertexData != nullptr)
      {
         bool hasNormals = normals.size() > 0;

         bool sendNormals = gpuVertexDataTransferInfo.sendNormals && hasNormals;

         gpuVertexData->Bind();
         gpuVertexData->Buffer(numTotalVertices, GL_STATIC_DRAW);

         std::vector<GPUVertexDataStorage> vertData(numTotalVertices);

         const Color& meshColor = colors[0];

         std::size_t vertDataIndex = 0;
         for (const face_t& face : faces)
         {
            for (const MeshVertexIndexer& vertex : face)
            {
               GPUVertexDataStorage& currentVertData = vertData[vertDataIndex];

               if (gpuVertexDataTransferInfo.sendPositions)
               {
                  currentVertData.position[0] = static_cast<float>(positions[vertex.positionID].x);
                  currentVertData.position[1] = static_cast<float>(positions[vertex.positionID].y);
                  currentVertData.position[2] = static_cast<float>(positions[vertex.positionID].z);
               }

               if (sendNormals)
               {
                  currentVertData.normal[0] = static_cast<float>(normals[vertex.normalID].x);
                  currentVertData.normal[1] = static_cast<float>(normals[vertex.normalID].y);
                  currentVertData.normal[2] = static_cast<float>(normals[vertex.normalID].z);
               }

               if (gpuVertexDataTransferInfo.sendColors)
               {
                  currentVertData.color[0] = meshColor.r;
                  currentVertData.color[1] = meshColor.g;
                  currentVertData.color[2] = meshColor.b;
                  currentVertData.color[3] = meshColor.a;
               }

               if (gpuVertexDataTransferInfo.sendTexCoords)
               {
                  currentVertData.texCoord[0] = static_cast<float>(textureCoordinates[vertex.textureCoordinateID].x);
                  currentVertData.texCoord[1] = static_cast<float>(textureCoordinates[vertex.textureCoordinateID].y);
               }

               ++vertDataIndex;
            }
         }

         gpuVertexData->BufferSub(0, numTotalVertices, vertData.data());

         gpuVertexData->transferInfo = gpuVertexDataTransferInfo;
         gpuVertexData->transferInfo.sendNormals = sendNormals;

         gpuVertexData->drawMode = GL_TRIANGLES;
      }
   }
}

}