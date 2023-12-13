//
// This file is part of the Terathon Common Library, by Eric Lengyel.
// Copyright 1999-2023, Terathon Software LLC
//
// This software is distributed under the MIT License.
// Separate proprietary licenses are available from Terathon Software.
//


#ifndef TSCompression_h
#define TSCompression_h


#include "TSPlatform.h"


#define TERATHON_COMPRESSION 1


namespace Terathon
{
	namespace Compression
	{
		TERATHON_API uint32 CompressData(const void *input, uint32 dataSize, uint8 *restrict code);
		TERATHON_API void DecompressData(const uint8 *restrict code, uint32 codeSize, void *output);
	}
}


#endif
