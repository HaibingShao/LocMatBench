//=================================================================================================
/*!
//  \file blaze/util/MemoryPool.h
//  \brief Header file for the memory pool class
//
//  Copyright (C) 2011 Klaus Iglberger - All Rights Reserved
//
//  This file is part of the Blaze library. This library is free software; you can redistribute
//  it and/or modify it under the terms of the GNU General Public License as published by the
//  Free Software Foundation; either version 3, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with a special
//  exception for linking and compiling against the Blaze library, the so-called "runtime
//  exception"; see the file COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================

#ifndef _BLAZE_UTIL_MEMORYPOOL_H_
#define _BLAZE_UTIL_MEMORYPOOL_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <vector>
#include <blaze/util/Assert.h>
#include <blaze/util/Byte.h>
#include <blaze/util/NonCopyable.h>
#include <blaze/util/Types.h>


namespace blaze {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Memory pool for small objects.
// \ingroup util
//
// The memory pool efficiently improves the performance of dynamic memory allocations for small
// objects. By allocating a large block of memory that can be dynamically assigned to small
// objects, the memory allocation is reduced from a few hundred cycles to only a few cycles.\n
// The memory pool is build from memory blocks of type Block, which hold the memory for a
// specified number of objects. The memory of these blocks is managed as a single free list.
*/
template< typename Type, size_t Blocksize >
class MemoryPool : private NonCopyable
{
 private:
   //**union FreeObject****************************************************************************
   /*!\brief A single element of the free list of the memory pool.
   */
   union FreeObject {
      FreeObject* next_;            //!< Pointer to the next free object.
      byte dummy_[ sizeof(Type) ];  //!< Dummy array to create an object of the appropriate size.
   };
   //**********************************************************************************************

   //**struct Block********************************************************************************
   /*!\brief Memory block within the memory bool.
   //
   // One memory block holds the memory for exactly \a Blocksize objects of type \a Type.
   */
   struct Block
   {
    public:
      //**Memory management functions**************************************************************
      /*!\name Memory management functions */
      //@{
      void init();
      void free();
      //@}
      //*******************************************************************************************

      //**Member variables*************************************************************************
      /*!\name Member variables */
      //@{
      FreeObject* rawMemory_;  //!< Allocated memory pool of the block.
      //@}
      //*******************************************************************************************
   };
   //**********************************************************************************************

   //**Type definitions****************************************************************************
   typedef std::vector<Block> Blocks;  //!< Vector of memory blocks.
   //**********************************************************************************************

 public:
   //**Constructor*********************************************************************************
   /*!\name Constructor */
   //@{
   inline MemoryPool();
   //@}
   //**********************************************************************************************

   //**Destructor**********************************************************************************
   /*!\name Destructor */
   //@{
   inline ~MemoryPool();
   //@}
   //**********************************************************************************************

   //**Memory management functions*****************************************************************
   /*!\name Memory management functions */
   //@{
   inline void* malloc();
   inline void  free( void* rawMemory );
   //@}
   //**********************************************************************************************

 private:
   //**Memory management functions*****************************************************************
   /*!\name Memory management functions */
   //@{
   inline bool checkMemory( FreeObject* rawMemory ) const;
   //@}
   //**********************************************************************************************

   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   FreeObject* freeList_;  //!< Head of the free list.
   Blocks blocks_;         //!< Vector of available memory blocks.
   //@}
   //**********************************************************************************************
};




//=================================================================================================
//
//  CLASS MEMORYPOOL::BLOCK
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Initialization of a memory block.
//
// \return void
//
// The \a init function allocates a single memory block for \a Blocksize objects of type \a Type.
// This memory is already prepared for the inclusion in the free list of the memory pool.
*/
template< typename Type, size_t Blocksize >
inline void MemoryPool<Type,Blocksize>::Block::init()
{
   rawMemory_ = new FreeObject[ Blocksize ];
   for( size_t i=0; i<Blocksize-1; ++i ) {
      rawMemory_[i].next_ = &rawMemory_[i+1];
   }
   rawMemory_[Blocksize-1].next_ = 0;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Release of the entire memory block.
//
// \return void
*/
template< typename Type, size_t Blocksize >
inline void MemoryPool<Type,Blocksize>::Block::free()
{
   delete [] rawMemory_;
}
//*************************************************************************************************




//=================================================================================================
//
//  CLASS MEMORYPOOL
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Constructor of the memory pool.
*/
template< typename Type, size_t Blocksize >
inline MemoryPool<Type,Blocksize>::MemoryPool()
{
   blocks_.push_back( Block() );
   Block& block = blocks_.back();
   block.init();
   freeList_ = block.rawMemory_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Destructor of the memory pool.
*/
template< typename Type, size_t Blocksize >
inline MemoryPool<Type,Blocksize>::~MemoryPool()
{
   for( typename Blocks::iterator it=blocks_.begin(); it!=blocks_.end(); ++it )
      it->free();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Allocation of raw memory for an object of type \a Type.
//
// \return Pointer to the raw memory.
*/
template< typename Type, size_t Blocksize >
inline void* MemoryPool<Type,Blocksize>::malloc()
{
   if( !freeList_ ) {
      blocks_.push_back( Block() );
      Block& block = blocks_.back();
      block.init();
      freeList_ = block.rawMemory_;
   }

   void* ptr = freeList_;
   freeList_ = freeList_->next_;
   return ptr;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Deallocation of raw memory for an object of type \a Type.
//
// \param rawMemory Pointer to the raw memory.
// \return void
*/
template< typename Type, size_t Blocksize >
inline void MemoryPool<Type,Blocksize>::free( void* rawMemory )
{
   FreeObject* ptr = reinterpret_cast<FreeObject*>( rawMemory );
   BLAZE_INTERNAL_ASSERT( checkMemory( ptr ), "Memory pool check failed" );
   ptr->next_ = freeList_;
   freeList_ = ptr;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Performing a number of checks on the memory to be released.
//
// \param toRelease Pointer to the memory to be released.
// \return \a true if the memory check succeeds, \a false if an error is encountered.
*/
template< typename Type, size_t Blocksize >
inline bool MemoryPool<Type,Blocksize>::checkMemory( FreeObject* toRelease ) const
{
   for( typename Blocks::const_iterator it=blocks_.begin(); it!=blocks_.end(); ++it )
   {
      // Range check
      if( toRelease >= it->rawMemory_ && toRelease < it->rawMemory_+Blocksize )
      {
         // Alignment check
         const byte* const ptr1( reinterpret_cast<const byte*>(toRelease) );
         const byte* const ptr2( reinterpret_cast<const byte*>(it->rawMemory_) );

         if( ( ptr1 - ptr2 ) % sizeof(FreeObject) != 0 ) return false;

         // Duplicate free check
         FreeObject* ptr( freeList_ );
         while( ptr ) {
            if( ptr == toRelease ) return false;
            ptr = ptr->next_;
         }

         return true;
      }
   }
   return false;
}
//*************************************************************************************************

} // namespace blaze

#endif
