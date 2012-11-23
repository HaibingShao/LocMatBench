//=================================================================================================
/*!
//  \file blaze/math/expressions/SVecTransposer.h
//  \brief Header file for the sparse vector transposer
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

#ifndef _BLAZE_MATH_EXPRESSIONS_SVECTRANSPOSER_H_
#define _BLAZE_MATH_EXPRESSIONS_SVECTRANSPOSER_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <blaze/math/constraints/Expression.h>
#include <blaze/math/constraints/SparseVector.h>
#include <blaze/math/constraints/TransposeFlag.h>
#include <blaze/math/expressions/SparseVector.h>
#include <blaze/math/Functions.h>
#include <blaze/math/shims/IsDefault.h>
#include <blaze/util/Assert.h>
#include <blaze/util/Types.h>


namespace blaze {

//=================================================================================================
//
//  CLASS SVECTRANSPOSER
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Expression object for the transposition of a sparse vector.
// \ingroup sparse_vector_expression
//
// The SVecTransposer class is a wrapper object for the temporary transposition of a sparse vector.
*/
template< typename VT  // Type of the sparse vector
        , bool TF >    // Transpose flag
class SVecTransposer : public SparseVector< SVecTransposer<VT,TF>, TF >
{
 public:
   //**Type definitions****************************************************************************
   typedef SVecTransposer<VT,TF>       This;           //!< Type of this SVecTransposer instance.
   typedef typename VT::TransposeType  ResultType;     //!< Result type for expression template evaluations.
   typedef typename VT::ResultType     TransposeType;  //!< Transpose type for expression template evaluations.
   typedef typename VT::ElementType    ElementType;    //!< Resulting element type.
   typedef const This&                 CompositeType;  //!< Data type for composite expression templates.
   typedef typename VT::Iterator       Iterator;       //!< Iterator over non-constant elements.
   typedef typename VT::ConstIterator  ConstIterator;  //!< Iterator over constant elements.
   //**********************************************************************************************

   //**Constructor*********************************************************************************
   /*!\brief Constructor for the SVecTransposer class.
   //
   // \param sv The sparse vector operand.
   */
   explicit inline SVecTransposer( VT& sv )
      : sv_( sv )  // The sparse vector operand
   {}
   //**********************************************************************************************

   //**Subscript operator**************************************************************************
   /*!\brief Subscript operator for the direct access to the vector elements.
   //
   // \param index Access index. The index has to be in the range \f$[0..N-1]\f$.
   // \return Reference to the accessed value.
   */
   inline ElementType& operator[]( size_t index ) {
      BLAZE_USER_ASSERT( index < sv_.size(), "Invalid vector access index" );
      return sv_[index];
   }
   //**********************************************************************************************

   //**Begin function******************************************************************************
   /*!\brief Returns an iterator to the first non-zero element of the sparse vector.
   //
   // \return Iterator to the first non-zero element of the sparse vector.
   */
   inline ConstIterator begin() const {
      return sv_.begin();
   }
   //**********************************************************************************************

   //**End function********************************************************************************
   /*!\brief Returns an iterator just past the last non-zero element of the sparse vector.
   //
   // \return Iterator just past the last non-zero element of the sparse vector.
   */
   inline ConstIterator end() const {
      return sv_.end();
   }
   //**********************************************************************************************

   //**Size function*******************************************************************************
   /*!\brief Returns the current size/dimension of the vector.
   //
   // \return The size of the vector.
   */
   inline size_t size() const {
      return sv_.size();
   }
   //**********************************************************************************************

   //**Reset function******************************************************************************
   /*!\brief Resets the vector elements.
   //
   // \return void
   */
   inline void reset() {
      return sv_.reset();
   }
   //**********************************************************************************************

   //**Insert function*****************************************************************************
   /*!\brief Inserting an element into the sparse vector.
   //
   // \param index The index of the new element. The index has to be in the range \f$[0..N-1]\f$.
   // \param value The value of the element to be inserted.
   // \return Iterator to the inserted element.
   // \exception std::invalid_argument Invalid sparse vector access index.
   //
   // This function inserts a new element into the sparse vector. However, duplicate elements are
   // not allowed. In case the sparse matrix already contains an element with row index \a index,
   // a \a std::invalid_argument exception is thrown.
   */
   inline ElementType& insert( size_t index, const ElementType& value ) {
      return sv_.insert( index, value );
   }
   //**********************************************************************************************

   //**Find function*******************************************************************************
   /*!\brief Inserting an element into the sparse vector.
   //
   // \param index The index of the search element. The index has to be in the range \f$[0..N-1]\f$.
   // \return Iterator to the element in case the index is found, end() iterator otherwise.
   //
   // This function can be used to check whether a specific element is contained in the sparse
   // vector. It specifically searches for the element with index \a index. In case the element
   // is found, the function returns an iterator to the element. Otherwise an iterator just past
   // the last non-zero element of the sparse vector (the end() iterator) is returned. Note that
   // the returned sparse vector iterator is subject to invalidation due to inserting operations
   // via the subscript operator or the insert() function!
   */
   inline Iterator find( size_t index ) {
      return sv_.find( index );
   }
   //**********************************************************************************************

   //**Reserve function****************************************************************************
   /*!\brief Setting the minimum capacity of the sparse vector.
   //
   // \param nonzeros The new minimum capacity of the sparse vector.
   // \return void
   //
   // This function increases the capacity of the sparse vector to at least \a nonzeros elements.
   // The current values of the vector elements are preserved.
   */
   inline void reserve( size_t nonzeros ) {
      sv_.reserve( nonzeros );
   }
   //**********************************************************************************************

   //**Append function*****************************************************************************
   /*!\brief Appending an element to the sparse vector.
   //
   // \param index The index of the new element. The index has to be in the range \f$[0..N-1]\f$.
   // \param value The value of the element to be appended.
   // \return void
   //
   // This function provides a very efficient way to fill a sparse vector with elements. It
   // appends a new element to the end of the sparse vector without any additional check or
   // memory allocation. Therefore it is strictly necessary to keep the following preconditions
   // in mind:
   //
   //  - the index of the new element must be strictly larger than the largest index of non-zero
   //    elements in the sparse vector
   //  - the current number of non-zero elements must be smaller than the capacity of the vector
   //
   // Ignoring these preconditions might result in undefined behavior!
   //
   // \b Note: Although append() does not allocate new memory, it still invalidates all iterators
   // returned by the end() functions!
   */
   inline void append( size_t index, const ElementType& value ) {
      sv_.append( index, value );
   }
   //**********************************************************************************************

   //**Transpose assignment of dense vectors*******************************************************
   /*!\brief Implementation of the transpose assignment of a dense vector.
   //
   // \param rhs The right-hand side dense vector to be assigned.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename VT2 >  // Type of the right-hand side dense vector
   inline void assign( const DenseVector<VT2,TF>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_VECTOR_WITH_TRANSPOSE_FLAG( VT2, TF );

      BLAZE_INTERNAL_ASSERT( sv_.size() == (~rhs).size(), "Invalid vector sizes" );

      size_t nonzeros( 0UL );

      for( size_t i=0UL; i<sv_.size(); ++i ) {
         if( !isDefault( (~rhs)[i] ) ) {
            if( nonzeros++ == sv_.capacity() )
               sv_.reserve( extendCapacity() );
            sv_.append( i, (~rhs)[i] );
         }
      }
   }
   //**********************************************************************************************

   //**Transpose assignment of sparse vectors******************************************************
   /*!\brief Implementation of the transpose assignment of a sparse vector.
   //
   // \param rhs The right-hand side sparse vector to be assigned.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename VT2 >  // Type of the right-hand side sparse vector
   inline void assign( const SparseVector<VT2,TF>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_VECTOR_WITH_TRANSPOSE_FLAG( VT2, TF );

      BLAZE_INTERNAL_ASSERT( sv_.size() == (~rhs).size(), "Invalid vector sizes" );

      // Using the following formulation instead of a std::copy function call of the form
      //
      //          end_ = std::copy( (~rhs).begin(), (~rhs).end(), begin_ );
      //
      // results in much less requirements on the ConstIterator type provided from the right-hand
      // sparse vector type
      for( typename VT2::ConstIterator element=(~rhs).begin(); element!=(~rhs).end(); ++element )
         sv_.append( element->index(), element->value() );
   }
   //**********************************************************************************************

 private:
   //**********************************************************************************************
   /*!\brief Calculating a new vector capacity.
   //
   // \return The new compressed vector capacity.
   //
   // This function calculates a new vector capacity based on the current capacity of the sparse
   // vector. Note that the new capacity is restricted to the interval \f$[7..size]\f$.
   */
   inline size_t extendCapacity() const
   {
      using blaze::max;
      using blaze::min;

      size_t nonzeros( 2UL*sv_.capacity()+1UL );
      nonzeros = max( nonzeros, 7UL );
      nonzeros = min( nonzeros, sv_.size() );

      BLAZE_INTERNAL_ASSERT( nonzeros > sv_.capacity(), "Invalid capacity value" );

      return nonzeros;
   }
   //**********************************************************************************************

   //**Member variables****************************************************************************
   VT& sv_;  //!< The sparse vector operand.
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_VECTOR_TYPE( VT );
   BLAZE_CONSTRAINT_MUST_BE_VECTOR_WITH_TRANSPOSE_FLAG( VT, !TF );
   BLAZE_CONSTRAINT_MUST_NOT_BE_EXPRESSION_TYPE( VT );
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************




//=================================================================================================
//
//  GLOBAL OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Resetting the sparse vector contained in a SVecTransposer.
// \ingroup sparse_vector_expression
//
// \param v The sparse vector to be resetted.
// \return void
*/
template< typename VT  // Type of the sparse vector
        , bool TF >    // Transpose flag
inline void reset( SVecTransposer<VT,TF>& v )
{
   v.reset();
}
/*! \endcond */
//*************************************************************************************************

} // namespace blaze

#endif
