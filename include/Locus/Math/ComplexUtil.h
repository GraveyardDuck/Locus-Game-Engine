/********************************************************************************************************\
*                                                                                                        *
*   This file is part of the Locus Game Engine                                                           *
*                                                                                                        *
*   Copyright (c) 2014 Shachar Avni. All rights reserved.                                                *
*                                                                                                        *
*   Use of this file is governed by a BSD-style license. See the accompanying LICENSE.txt for details    *
*                                                                                                        *
\********************************************************************************************************/

#pragma once

#include "Locus/Common/Float.h"

#include <complex>

namespace Locus
{

template <typename T>
inline T& RealPart(std::complex<T>& x)
{
   return reinterpret_cast<T(&)[2]>(x)[0];
}

template <typename T>
inline T& ImaginaryPart(std::complex<T>& x)
{
   return reinterpret_cast<T(&)[2]>(x)[1];
}

template <typename T>
inline bool IsReal(const std::complex<T>& x, T toleranceFactor = Float::DEFAULT_TOLERANCE)
{
   return Float::IsZero<T>(x.imag(), toleranceFactor);
}

template <typename T>
inline bool Equal(const std::complex<T>& x, const std::complex<T>& y, T toleranceFactor = Float::DEFAULT_TOLERANCE)
{
   return ( Float::Equal<T>(x.real(), y.real(), toleranceFactor) && Float::Equal<T>(x.imag(), y.imag(), toleranceFactor) );
}

template <typename T>
inline bool NotEqual(const std::complex<T>& x, const std::complex<T>& y, T toleranceFactor = Float::DEFAULT_TOLERANCE)
{
   return ( Float::NotEqual<T>(x.real(), y.real(), toleranceFactor) || Float::NotEqual<T>(x.imag(), y.imag(), toleranceFactor) );
}

template <typename T>
inline bool IsZero(const std::complex<T>& x, T toleranceFactor = Float::DEFAULT_TOLERANCE)
{
   return ( Float::IsZero<T>(x.real(), toleranceFactor) && Float::IsZero<T>(x.imag(), toleranceFactor) );
}

template <typename T>
inline bool NotZero(const std::complex<T>& x, T toleranceFactor = Float::DEFAULT_TOLERANCE)
{
   return ( Float::NotZero<T>(x.real(), toleranceFactor) || Float::NotZero<T>(x.imag(), toleranceFactor) );
}

}