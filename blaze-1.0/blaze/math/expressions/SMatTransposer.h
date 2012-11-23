//=================================================================================================
/*!
//  \file blaze/math/expressions/SMatTransposer.h
//  \brief Header file for the sparse matrix transposer
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

#ifndef _BLAZE_MATH_EXPRESSIONS_SMATTRANSPOSER_H_
#define _BLAZE_MATH_EXPRESSIONS_SMATTRANSPOSER_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <vector>
#include <blaze/math/constraints/SparseMatrix.h>
#include <blaze/math/constraints/StorageOrder.h>
#include <blaze/math/expressions/SparseMatrix.h>
#include <blaze/util/Assert.h>
#include <blaze/util/Types.h>


namespace blaze {

//=================================================================================================
//
//  CLASS SMATTRANSPOSER
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Expression object for the transposition of a sparse matrix.
// \ingroup sparse_matrix_expression
//
// The SMatTransposer class is a wrapper object for the temporary transposition of a sparse matrix.
*/
template< typename MT  // Type of the sparse matrix
        , bool SO >    // Storage order
class SMatTransposer : public SparseMatrix< SMatTransposer<MT,SO>, SO >
{
 public:
   //**Type definitions****************************************************************************
   typedef SMatTransposer<MT,SO>       This;           //!< Type of this SMatTransposer instance.
   typedef typename MT::TransposeType  ResultType;     //!< Result type for expression template evaluations.
   typedef MT                          OppositeType;   //!< Result type with opposite storage order for expression template evaluations.
   typedef typename MT::ResultType     TransposeType;  //!< Transpose type for expression template evaluations.
   typedef typename MT::ElementType    ElementType;    //!< Resulting element type.
   typedef const This&                 CompositeType;  //!< Data type for composite expression templates.
   typedef typename MT::Iterator       Iterator;       //!< Iterator over non-constant elements.
   typedef typename MT::ConstIterator  ConstIterator;  //!< Iterator over constant elements.
   //**********************************************************************************************

   //**Constructor*********************************************************************************
   /*!\brief Constructor for the SMatTransposer class.
   //
   // \param sm The sparse matrix operand.
   */
   explicit inline SMatTransposer( MT& sm )
      : sm_( sm )  // The sparse matrix operand
   {}
   //**********************************************************************************************

   //**Access operator*****************************************************************************
   /*!\brief 2D-access to the matrix elements.
   //
   // \param i Access index for the row. The index has to be in the range \f$[0..M-1]\f$.
   // \param j Access index for the column. The index has to be in the range \f$[0..N-1]\f$.
   // \return Reference to the accessed value.
   */
   inline const ElementType operator()( size_t i, size_t j ) const {
      BLAZE_INTERNAL_ASSERT( i < sm_.columns(), "Invalid row access index"    );
      BLAZE_INTERNAL_ASSERT( j < sm_.row()    , "Invalid column access index" );
      return sm_(j,i);
   }
   //**********************************************************************************************

   //**Begin function******************************************************************************
   /*!\brief Returns an iterator to the first non-zero element of row/column \a i.
   //
   // \param i The row/column index.
   // \return Iterator to the first non-zero element of row/column \a i.
   //
   // This function returns a row/column iterator to the first non-zero element of row/column \a i.
   // In case the storage order is set to \a rowMajor the function returns an iterator to the first
   // non-zero element of row \a i, in case the storage flag is set to \a columnMajor the function
   // returns an iterator to the first non-zero element of column \a i.
   */
   inline ConstIterator begin( size_t i ) const {
      return ConstIterator( sm_.begin(i) );
   }
   //**********************************************************************************************

   //**End function********************************************************************************
   /*!\brief Returns an iterator just past the last non-zero element of row/column \a i.
   //
   // \param i The row/column index.
   // \return Iterator just past the last non-zero element of row/column \a i.
   //
   // This function returns an row/column iterator just past the last non-zero element of row/column
   // \a i. In case the storage order is set to \a rowMajor the function returns an iterator just
   // past the last non-zero element of row \a i, in case the storage flag is set to \a columnMajor
   // the function returns an iterator just past the last non-zero element of column \a i.
   */
   inline ConstIterator end( size_t i ) const {
      return ConstIterator( sm_.end(i) );
   }
   //**********************************************************************************************

   //**Rows function*******************************************************************************
   /*!\brief Returns the current number of rows of the matrix.
   //
   // \return The number of rows of the matrix.
   */
   inline size_t rows() const {
      return sm_.columns();
   }
   //**********************************************************************************************

   //**Columns function****************************************************************************
   /*!\brief Returns the current number of columns of the matrix.
   //
   // \return The number of columns of the matrix.
   */
   inline size_t columns() const {
      return sm_.rows();
   }
   //**********************************************************************************************

   //**Reset function******************************************************************************
   /*!\brief Resets the matrix elements.
   //
   // \return void
   */
   inline void reset() {
      return sm_.reset();
   }
   //**********************************************************************************************

   //**Insert function*****************************************************************************
   /*!\brief Inserting an element into the sparse matrix.
   //
   // \param i The row index of the new element. The index has to be in the range \f$[0..M-1]\f$.
   // \param j The column index of the new element. The index has to be in the range \f$[0..N-1]\f$.
   // \param value The value of the element to be inserted.
   // \return Iterator to the newly inserted element.
   // \exception std::invalid_argument Invalid sparse matrix access index.
   //
   // This function insert a new element into the sparse matrix. However, duplicate elements are
   // not allowed. In case the sparse matrix already contains an element with row index \a i and
   // column index \a j, a \a std::invalid_argument exception is thrown.
   */
   inline Iterator insert( size_t i, size_t j, const ElementType& value ) {
      return sm_.insert( j, i, value );
   }
   //**********************************************************************************************

   //**Reserve function****************************************************************************
   /*!\brief Setting the minimum capacity of the sparse matrix.
   //
   // \param nonzeros The new minimum capacity of the sparse matrix.
   // \return void
   //
   // This function increases the capacity of the sparse matrix to at least \a nonzeros elements.
   // The current values of the matrix elements and the individual capacities of the matrix rows
   // are preserved.
   */
   inline void reserve( size_t nonzeros ) {
      sm_.reserve( nonzeros );
   }
   //**********************************************************************************************

   //**Reserve function****************************************************************************
   /*!\brief Setting the minimum capacity of a specific row/column of the sparse matrix.
   //
   // \param i The row/column index of the new element \f$[0..M-1]\f$ or \f$[0..N-1]\f$.
   // \param nonzeros The new minimum capacity of the specified row.
   // \return void
   //
   // This function increases the capacity of row/column \a i of the sparse matrix to at least
   // \a nonzeros elements. The current values of the sparse matrix and all other individual
   // row/column capacities are preserved. In case the storage order is set to \a rowMajor, the
   // function reserves capacity for row \a i and the index has to be in the range \f$[0..M-1]\f$.
   // In case the storage order is set to \a columnMajor, the function reserves capacity for column
   // \a i and the index has to be in the range \f$[0..N-1]\f$.
   */
   inline void reserve( size_t i, size_t nonzeros ) {
      sm_.reserve( i, nonzeros );
   }
   //**********************************************************************************************

   //**Append function*****************************************************************************
   /*!\brief Appending an element to the specified row/column of the sparse matrix.
   //
   // \param i The row index of the new element. The index has to be in the range \f$[0..M-1]\f$.
   // \param j The column index of the new element. The index has to be in the range \f$[0..N-1]\f$.
   // \param value The value of the element to be appended.
   // \return void
   //
   // This function provides a very efficient way to fill a sparse matrix with elements. It
   // appends a new element to the end of the specified row/column without any additional
   // parameter verification or memory allocation. Therefore it is strictly necessary to
   // keep the following preconditions in mind:
   //
   //  - the index of the new element must be strictly larger than the largest index of
   //    non-zero elements in the specified row/column of the sparse matrix
   //  - the current number of non-zero elements in row/column \a i must be smaller than
   //    the capacity of row/column \a i.
   //
   // Ignoring these preconditions might result in undefined behavior!
   //
   // \b Note: Although append() does not allocate new memory, it still invalidates all iterators
   // returned by the end() functions!
   */
   inline void append( size_t i, size_t j, const ElementType& value ) {
      sm_.append( j, i, value );
   }
   //**********************************************************************************************

   //**Finalize function***************************************************************************
   /*!\brief Finalizing the element insertion of a row/column.
   //
   // \param i The index of the row/column to be finalized \f$[0..M-1]\f$.
   // \return void
   //
   // This function is part of the low-level interface to efficiently fill the matrix with elements.
   // After completion of row/column \a i via the append() function, this function can be called to
   // finalize row/column \a i and prepare the next row/column for insertion process via append().
   //
   // \b Note: Although finalize() does not allocate new memory, it still invalidates all iterators
   // returned by the end() functions!
   */
   inline void finalize( size_t i ) {
      sm_.finalize( i );
   }
   //**********************************************************************************************

   //**Transpose assignment of row-major sparse matrices*******************************************
   /*!\brief Implementation of the transpose assignment of a row-major sparse matrix.
   //
   // \param rhs The right-hand side sparse matrix to be assigned.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename MT2 >  // Type of the right-hand side sparse matrix
   inline void assign( const SparseMatrix<MT2,false>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( sm_.columns() == (~rhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( sm_.rows() == (~rhs).columns()     , "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( sm_.capacity() >= (~rhs).nonZeros(), "Capacity not sufficient"   );

      typedef typename MT2::ConstIterator  RhsIterator;

      const size_t m( rows() );

      for( size_t i=0UL; i<m; ++i ) {
         for( RhsIterator element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
            sm_.append( element->index(), i, element->value() );
         finalize( i );
      }
   }
   //**********************************************************************************************

   //**Transpose assignment of column-major sparse matrices****************************************
   /*!\brief Implementation of the transpose assignment of a column-major sparse matrix.
   //
   // \param rhs The right-hand side sparse matrix to be assigned.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename MT2 >  // Type of the right-hand side sparse matrix
   inline void assign( const SparseMatrix<MT2,true>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( sm_.columns() == (~rhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( sm_.rows() == (~rhs).columns()     , "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( sm_.capacity() >= (~rhs).nonZeros(), "Capacity not sufficient"   );

      typedef typename MT2::ConstIterator  RhsIterator;

      const size_t m( rows() );
      const size_t n( columns() );

      // Counting the number of elements per row
      std::vector<size_t> rowLengths( m, 0UL );
      for( size_t j=0UL; j<n; ++j ) {
         for( RhsIterator element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
            ++rowLengths[element->index()];
      }

      // Resizing the sparse matrix
      for( size_t i=0UL; i<m; ++i ) {
         sm_.reserve( i, rowLengths[i] );
      }

      // Appending the elements to the rows of the sparse matrix
      for( size_t j=0UL; j<n; ++j ) {
         for( RhsIterator element=(~rhs).begin(j); element!=(~rhs).end(j); ++element ) {
            sm_.append( j, element->index(), element->value() );
         }
      }
   }
   //**********************************************************************************************

 private:
   //**Member variables****************************************************************************
   MT& sm_;  //!< The sparse matrix operand.
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE( MT );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( MT );
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************




//=================================================================================================
//
//  CLASS TEMPLATE SPECIALIZATION FOR ROW-MAJOR MATRICES
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Specialization of SMatTransposer for row-major matrices.
// \ingroup sparse_matrix_expression
//
// This specialization of SMatTransposer adapts the class template to the requirements of
// row-major matrices.
*/
template< typename MT >  // Type of the sparse matrix
class SMatTransposer<MT,true> : public SparseMatrix< SMatTransposer<MT,true>, true >
{
 public:
   //**Type definitions****************************************************************************
   typedef SMatTransposer<MT,true>     This;           //!< Type of this SMatTransposer instance.
   typedef typename MT::TransposeType  ResultType;     //!< Result type for expression template evaluations.
   typedef MT                          OppositeType;   //!< Result type with opposite storage order for expression template evaluations.
   typedef typename MT::ResultType     TransposeType;  //!< Transpose type for expression template evaluations.
   typedef typename MT::ElementType    ElementType;    //!< Resulting element type.
   typedef const This&                 CompositeType;  //!< Data type for composite expression templates.
   typedef typename MT::Iterator       Iterator;       //!< Iterator over non-constant elements.
   typedef typename MT::ConstIterator  ConstIterator;  //!< Iterator over constant elements.
   //**********************************************************************************************

   //**Constructor*********************************************************************************
   /*!\brief Constructor for the SMatTransposer class.
   //
   // \param sm The dense sparse operand.
   */
   explicit inline SMatTransposer( MT& sm )
      : sm_( sm )  // The sparse matrix operand
   {}
   //**********************************************************************************************

   //**Access operator*****************************************************************************
   /*!\brief 2D-access to the matrix elements.
   //
   // \param i Access index for the row. The index has to be in the range \f$[0..M-1]\f$.
   // \param j Access index for the column. The index has to be in the range \f$[0..N-1]\f$.
   // \return Reference to the accessed value.
   */
   inline const ElementType operator()( size_t i, size_t j ) const {
      BLAZE_INTERNAL_ASSERT( i < sm_.columns(), "Invalid row access index"    );
      BLAZE_INTERNAL_ASSERT( j < sm_.row()    , "Invalid column access index" );
      return sm_(j,i);
   }
   //**********************************************************************************************

   //**Begin function******************************************************************************
   /*!\brief Returns an iterator to the first non-zero element of column \a j.
   //
   // \param j The column index.
   // \return Iterator to the first non-zero element of column \a j.
   */
   inline ConstIterator begin( size_t j ) const {
      return ConstIterator( sm_.begin(j) );
   }
   //**********************************************************************************************

   //**End function********************************************************************************
   /*!\brief Returns an iterator just past the last non-zero element of column \a j.
   //
   // \param j The column index.
   // \return Iterator just past the last non-zero element of column \a j.
   */
   inline ConstIterator end( size_t j ) const {
      return ConstIterator( sm_.end(j) );
   }
   //**********************************************************************************************

   //**Rows function*******************************************************************************
   /*!\brief Returns the current number of rows of the matrix.
   //
   // \return The number of rows of the matrix.
   */
   inline size_t rows() const {
      return sm_.columns();
   }
   //**********************************************************************************************

   //**Columns function****************************************************************************
   /*!\brief Returns the current number of columns of the matrix.
   //
   // \return The number of columns of the matrix.
   */
   inline size_t columns() const {
      return sm_.rows();
   }
   //**********************************************************************************************

   //**Reset function******************************************************************************
   /*!\brief Resets the matrix elements.
   //
   // \return void
   */
   inline void reset() {
      return sm_.reset();
   }
   //**********************************************************************************************

   //**Insert function*****************************************************************************
   /*!\brief Inserting an element into the sparse matrix.
   //
   // \param i The row index of the new element. The index has to be in the range \f$[0..M-1]\f$.
   // \param j The column index of the new element. The index has to be in the range \f$[0..N-1]\f$.
   // \param value The value of the element to be inserted.
   // \return Iterator to the newly inserted element.
   // \exception std::invalid_argument Invalid sparse matrix access index.
   //
   // This function insert a new element into the sparse matrix. However, duplicate elements are
   // not allowed. In case the sparse matrix already contains an element with row index \a i and
   // column index \a j, a \a std::invalid_argument exception is thrown.
   */
   inline Iterator insert( size_t i, size_t j, const ElementType& value ) {
      return sm_.insert( j, i, value );
   }
   //**********************************************************************************************

   //**Reserve function****************************************************************************
   /*!\brief Setting the minimum capacity of the sparse matrix.
   //
   // \param nonzeros The new minimum capacity of the sparse matrix.
   // \return void
   //
   // This function increases the capacity of the sparse matrix to at least \a nonzeros elements.
   // The current values of the matrix elements and the individual capacities of the matrix rows
   // are preserved.
   */
   inline void reserve( size_t nonzeros ) {
      sm_.reserve( nonzeros );
   }
   //**********************************************************************************************

   //**Reserve function****************************************************************************
   /*!\brief Setting the minimum capacity of a specific column of the sparse matrix.
   //
   // \param j The column index of the new element \f$[0..N-1]\f$.
   // \param nonzeros The new minimum capacity of the specified row.
   // \return void
   //
   // This function increases the capacity of column \a j of the sparse matrix to at least
   // \a nonzeros elements. The current values of the sparse matrix and all other individual
   // column capacities are preserved.
   */
   inline void reserve( size_t i, size_t nonzeros ) {
      sm_.reserve( i, nonzeros );
   }
   //**********************************************************************************************

   //**Append function*****************************************************************************
   /*!\brief Appending an element to the specified column of the sparse matrix.
   //
   // \param i The row index of the new element. The index has to be in the range \f$[0..M-1]\f$.
   // \param j The column index of the new element. The index has to be in the range \f$[0..N-1]\f$.
   // \param value The value of the element to be appended.
   // \return void
   //
   // This function provides a very efficient way to fill a sparse matrix with elements. It
   // appends a new element to the end of the specified column without any additional parameter
   // verification or memory allocation. Therefore it is strictly necessary to keep the following
   // preconditions in mind:
   //
   //  - the index of the new element must be strictly larger than the largest index of non-zero
   //    elements in the specified column of the sparse matrix
   //  - the current number of non-zero elements in column \a j must be smaller than the capacity
   //    of column \a j.
   //
   // Ignoring these preconditions might result in undefined behavior!
   //
   // \b Note: Although append() does not allocate new memory, it still invalidates all iterators
   // returned by the end() functions!
   */
   void append( size_t i, size_t j, const ElementType& value ) {
      sm_.append( j, i, value );
   }
   //**********************************************************************************************

   //**Finalize function***************************************************************************
   /*!\brief Finalizing the element insertion of a column.
   //
   // \param i The index of the column to be finalized \f$[0..M-1]\f$.
   // \return void
   //
   // This function is part of the low-level interface to efficiently fill the matrix with elements.
   // After completion of column \a i via the append() function, this function can be called to
   // finalize column \a i and prepare the next row/column for insertion process via append().
   //
   // \b Note: Although finalize() does not allocate new memory, it still invalidates all iterators
   // returned by the end() functions!
   */
   inline void finalize( size_t j ) {
      sm_.finalize( j );
   }
   //**********************************************************************************************

   //**Transpose assignment of row-major sparse matrices*******************************************
   /*!\brief Implementation of the transpose assignment of a row-major sparse matrix.
   //
   // \param rhs The right-hand side sparse matrix to be assigned.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename MT2 >  // Type of the right-hand side sparse matrix
   inline void assign( const SparseMatrix<MT2,false>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( sm_.columns() == (~rhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( sm_.rows() == (~rhs).columns()     , "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( sm_.capacity() >= (~rhs).nonZeros(), "Capacity not sufficient"   );

      typedef typename MT2::ConstIterator  RhsIterator;

      const size_t m( rows() );
      const size_t n( columns() );

      // Counting the number of elements per row
      std::vector<size_t> columnLengths( n, 0UL );
      for( size_t i=0UL; i<m; ++i ) {
         for( RhsIterator element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
            ++columnLengths[element->index()];
      }

      // Resizing the sparse matrix
      for( size_t j=0UL; j<n; ++j ) {
         sm_.reserve( j, columnLengths[j] );
      }

      // Appending the elements to the columns of the sparse matrix
      for( size_t i=0UL; i<m; ++i ) {
         for( RhsIterator element=(~rhs).begin(i); element!=(~rhs).end(i); ++element ) {
            sm_.append( element->index(), i, element->value() );
         }
      }
   }
   //**********************************************************************************************

   //**Transpose assignment of column-major sparse matrices****************************************
   /*!\brief Implementation of the transpose assignment of a column-major sparse matrix.
   //
   // \param rhs The right-hand side sparse matrix to be assigned.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename MT2 >  // Type of the right-hand side sparse matrix
   inline void assign( const SparseMatrix<MT2,true>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( sm_.columns() == (~rhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( sm_.rows() == (~rhs).columns()     , "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( sm_.capacity() >= (~rhs).nonZeros(), "Capacity not sufficient"   );

      typedef typename MT2::ConstIterator  RhsIterator;

      const size_t n( columns() );

      for( size_t j=0UL; j<n; ++j ) {
         for( RhsIterator element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
            sm_.append( j, element->index(), element->value() );
         finalize( j );
      }
   }
   //**********************************************************************************************

 private:
   //**Member variables****************************************************************************
   MT& sm_;  //!< The sparse matrix operand.
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE( MT );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT );
   /*! \endcond */
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************

} // namespace blaze

#endif
