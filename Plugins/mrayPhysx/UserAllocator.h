// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//							       USER ALLOCATOR
//
//						    Written by Bob Schade, 5-1-06
// ===============================================================================

#include <NxUserAllocator.h>

#ifndef USERALLOCATOR_H
#define USERALLOCATOR_H

class UserAllocator : public NxUserAllocator
{
public:
						UserAllocator();
	virtual				~UserAllocator();

			void		reset();

			void*		malloc(NxU32 size);
			void*		malloc(NxU32 size, NxMemoryType type);
			void*		mallocDEBUG(NxU32 size, const char* file, int line);
			void*		mallocDEBUG(NxU32 size, const char* file, int line, const char* className, NxMemoryType type);
			void*		realloc(void* memory, NxU32 size);
			void		free(void* memory);

			NxU32*		mMemBlockList;
			NxU32		mMemBlockListSize;
			NxU32		mMemBlockFirstFree;
			NxU32		mMemBlockUsed;

			NxI32		mNbAllocatedBytes;
			NxI32		mHighWaterMark;
			NxI32		mTotalNbAllocs;
			NxI32		mNbAllocs;
			NxI32		mNbReallocs;
};

#endif  // USERALLOCATOR_H
