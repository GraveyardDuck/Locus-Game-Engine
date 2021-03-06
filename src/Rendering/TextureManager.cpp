/********************************************************************************************************\
*                                                                                                        *
*   This file is part of the Locus Game Engine                                                           *
*                                                                                                        *
*   Copyright (c) 2014 Shachar Avni. All rights reserved.                                                *
*                                                                                                        *
*   Use of this file is governed by a BSD-style license. See the accompanying LICENSE.txt for details    *
*                                                                                                        *
\********************************************************************************************************/

#include "Locus/Rendering/TextureManager.h"
#include "Locus/Rendering/GLInfo.h"
#include "Locus/Rendering/Image.h"

namespace Locus
{

TextureManager::TextureManager(const GLInfo& glInfo)
   : glInfo(glInfo)
{
}

TextureManager::~TextureManager()
{
   UnLoad();
}

template <class FilePathType>
void TextureManager::Load(const std::string& textureName, const FilePathType& textureFilePath, Texture::MipmapGeneration mipmapGeneration, TextureFiltering filtering, bool clamp)
{
   if (textures.find(textureName) == textures.end())
   {
      textures[textureName] = std::make_unique<Texture>(Image(textureFilePath), mipmapGeneration, filtering, clamp, glInfo);
   }
}

void TextureManager::Load(const std::string& textureName, const std::string& textureLocation, Texture::MipmapGeneration mipmapGeneration, TextureFiltering filtering, bool clamp)
{
   Load<std::string>(textureName, textureLocation, mipmapGeneration, filtering, clamp);
}

void TextureManager::Load(const std::string& textureName, const MountedFilePath& textureLocation, Texture::MipmapGeneration mipmapGeneration, TextureFiltering filtering, bool clamp)
{
   Load<MountedFilePath>(textureName, textureLocation, mipmapGeneration, filtering, clamp);
}

Texture* TextureManager::GetTexture(const std::string& textureName) const
{
   Texture* texture = nullptr;

   std::unordered_map<std::string, std::unique_ptr<Texture>>::const_iterator textureMapIterator = textures.find(textureName);
   if (textureMapIterator != textures.end())
   {
      texture = textureMapIterator->second.get();
   }

   return texture;
}

void TextureManager::UnLoad()
{
   textures.clear();
}

}