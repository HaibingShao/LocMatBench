//=================================================================================================
/*!
//  \file blaze/math/expressions/DMatTransposer.h
//  \brief Header file for the dense matrix transposer
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

#ifndef _BLAZE_MATH_EXPRESSIONS_DMATTRANSPOSER_H_
#define _BLAZE_MATH_EXPRESSIONS_DMATTRANSPOSER_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <blaze/math/constraints/DenseMatrix.h>
#include <blaze/math/constraints/Expression.h>
#include <blaze/math/constraints/StorageOrder.h>
#include <blaze/math/expressions/DenseMatrix.h>
#include <blaze/util/Assert.h>
#include <blaze/util/Types.h>


namespace blaze {

//=================================================================================================
//
//  CLASS DMATTRANSPOSER
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Expression object for the transposition of a dense matrix.
// \ingroup dense_matrix_expression
//
// The DMatTransposer class is a wrapper object for the temporary transposition of a dense matrix.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
class DMatTransposer : public DenseMatrix< DMatTransposer<MT,SO>, SO >
{
 public:
   //**Type definitions****************************************************************************
   typedef DMatTransposer<MT,SO>       This;           //!< Type of this DMatTransposer instance.
   typedef typename MT::TransposeType  ResultType;     //!< Result type for expression template evaluations.
   typedef typename MT::OppositeType   OppositeType;   //!< Result type with opposite storage order for expression template evaluations.
   typedef typename MT::ResultType     TransposeType;  //!< Transpose type for expression template evaluations.
   typedef typename MT::ElementType    ElementType;    //!< Resulting element type.
   typedef const This&                 CompositeType;  //!< Data type for composite expression templates.
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation flag for intrinsic optimization.
   /*! The \a vectorizable compilation flag indicates whether expressions the matrix is involved
       in can be optimized via intrinsics. In case the dense matrix operand is vectorizable, the
       \a vectorizable compilation flag is set to \a true, otherwise it is set to \a false. */
   enum { vectorizable = MT::vectorizable };
   //**********************************************************************************************

   //**Constructor*********************************************************************************
   /*!\brief Constructor for the DMatTransposer class.
   //
   // \param dm The dense matrix operand.
   */
   explicit inline DMatTransposer( MT& dm )
      : dm_( dm )  // The dense matrix operand
   {}
   //**********************************************************************************************

   //**Access operator*****************************************************************************
   /*!\brief 2D-access to the matrix elements.
   //
   // \param i Access index for the row. The index has to be in the range \f$[0..M-1]\f$.
   // \param j Access index for the column. The index has to be in the range \f$[0..N-1]\f$.
   // \return Reference to the accessed value.
   */
   inline ElementType& operator()( size_t i, size_t j ) {
      BLAZE_INTERNAL_ASSERT( i < dm_.columns(), "Invalid row access index"    );
      BLAZE_INTERNAL_ASSERT( j < dm_.rows()   , "Invalid column access index" );
      return dm_(j,i);
   }
   //**********************************************************************************************

   //**Low-level data access***********************************************************************
   /*!\brief Low-level data access to the matrix elements.
   //
   // \return Pointer to the internal element storage.
   */
   inline ElementType* data() {
      return dm_.data();
   }
   //**********************************************************************************************

   //**Rows function*******************************************************************************
   /*!\brief Returns the current number of rows of the matrix.
   //
   // \return The number of rows of the matrix.
   */
   inline size_t rows() const {
      return dm_.columns();
   }
   //**********************************************************************************************

   //**Columns function****************************************************************************
   /*!\brief Returns the current number of columns of the matrix.
   //
   // \return The number of columns of the matrix.
   */
   inline size_t columns() const {
      return dm_.rows();
   }
   //**********************************************************************************************

   //**Spacing function****************************************************************************
   /*!\brief Returns the spacing between the beginning of two rows.
   //
   // \return The spacing between the beginning of two rows.
   */
   inline size_t spacing() const {
      return dm_.spacing();
   }
   //**********************************************************************************************

   //**Reset function******************************************************************************
   /*!\brief Resets the matrix elements.
   //
   // \return void
   */
   inline void reset() {
      return dm_.reset();
   }
   //**********************************************************************************************

   //**Transpose assignment of row-major dense matrices********************************************
   /*!\brief Implementation of the transpose assignment of a row-major dense matrix.
   //
   // \param rhs The right-hand side dense matrix to be assigned.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename MT2 >  // Type of the right-hand side dense matrix
   inline void assign( const DenseMatrix<MT2,SO>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( dm_.columns() == (~rhs).rows(), "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( dm_.rows() == (~rhs).columns(), "Invalid number of columns" );

      const size_t m( rows() );
      const size_t n( columns() );

      BLAZE_INTERNAL_ASSERT( ( n - ( n % 2UL ) ) == ( n & size_t(-2) ), "Invalid end calculation" );
      const size_t end( n & size_t(-2) );

      for( size_t i=0UL; i<m; ++i ) {
         for( size_t j=0UL; j<end; j+=2UL ) {
            dm_(j    ,i) = (~rhs)(i,j    );
            dm_(j+1UL,i) = (~rhs)(i,j+1UL);
         }
         if( end < n ) {
            dm_(end,i) = (~rhs)(i,end);
         }
      }
   }
   //**********************************************************************************************

   //**Transpose assignment of column-major dense matrices*****************************************
   /*!\brief Implementation of the transpose assignment of a column-major dense matrix.
   //
   // \param rhs The right-hand side dense matrix to be assigned.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename MT2 >  // Type of the right-hand side dense matrix
   inline void assign( const DenseMatrix<MT2,!SO>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( dm_.columns() == (~rhs).rows(), "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( dm_.rows() == (~rhs).columns(), "Invalid number of columns" );

      const size_t m( rows() );
      const size_t n( columns() );
      const size_t block( 16UL );

      for( size_t ii=0UL; ii<m; ii+=block ) {
         const size_t iend( ( m < ii+block )?( m ):( ii+block ) );
         for( size_t jj=0UL; jj<n; jj+=block ) {
            const size_t jend( ( n < jj+block )?( n ):( jj+block ) );
            for( size_t i=ii; i<iend; ++i ) {
               for( size_t j=jj; j<jend; ++j ) {
                  dm_(j,i) = (~rhs)(i,j);
               }
            }
         }
      }
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
   inline void assign( const SparseMatrix<MT2,SO>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( dm_.columns() == (~rhs).rows(), "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( dm_.rows() == (~rhs).columns(), "Invalid number of columns" );

      typedef typename MT2::ConstIterator  ConstIterator;

      for( size_t i=0UL; i<(~rhs).rows(); ++i )
         for( ConstIterator element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
            dm_(element->index(),i) = element->value();
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
   inline void assign( const SparseMatrix<MT2,!SO>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( dm_.columns() == (~rhs).rows(), "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( dm_.rows() == (~rhs).columns(), "Invalid number of columns" );

      typedef typename MT2::ConstIterator  ConstIterator;

      for( size_t j=0UL; j<(~rhs).columns(); ++j )
         for( ConstIterator element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
            dm_(j,element->index()) = element->value();
   }
   //**********************************************************************************************

   //**Transpose addition assignment of row-major dense matrices***********************************
   /*!\brief Implementation of the transpose addition assignment of a row-major dense matrix.
   //
   // \param rhs The right-hand side dense matrix to be added.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename MT2 >  // Type of the right-hand side dense matrix
   inline void addAssign( const DenseMatrix<MT2,SO>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( dm_.columns() == (~rhs).rows(), "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( dm_.rows() == (~rhs).columns(), "Invalid number of columns" );

      const size_t m( rows() );
      const size_t n( columns() );

      BLAZE_INTERNAL_ASSERT( ( n - ( n % 2UL ) ) == ( n & size_t(-2) ), "Invalid end calculation" );
      const size_t end( n & size_t(-2) );

      for( size_t i=0UL; i<m; ++i ) {
         for( size_t j=0UL; j<end; j+=2UL ) {
            dm_(j    ,i) += (~rhs)(i,j    );
            dm_(j+1UL,i) += (~rhs)(i,j+1UL);

         }
         if( end < n ) {
            dm_(end,i) += (~rhs)(i,end);
         }
      }
   }
   //**********************************************************************************************

   //**Transpose addition assignment of column-major dense matrices********************************
   /*!\brief Implementation of the transpose addition assignment of a column-major dense matrix.
   //
   // \param rhs The right-hand side dense matrix to be added.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename MT2 >  // Type of the right-hand side dense matrix
   inline void addAssign( const DenseMatrix<MT2,!SO>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( dm_.columns() == (~rhs).rows(), "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( dm_.rows() == (~rhs).columns(), "Invalid number of columns" );

      const size_t m( rows() );
      const size_t n( columns() );
      const size_t block( 16UL );

      for( size_t ii=0UL; ii<m; ii+=block ) {
         const size_t iend( ( m < ii+block )?( m ):( ii+block ) );
         for( size_t jj=0UL; jj<n; jj+=block ) {
            const size_t jend( ( n < jj+block )?( n ):( jj+block ) );
            for( size_t i=ii; i<iend; ++i ) {
               for( size_t j=jj; j<jend; ++j ) {
                  dm_(j,i) += (~rhs)(i,j);
               }
            }
         }
      }
   }
   //**********************************************************************************************

   //**Transpose addition assignment of row-major sparse matrices**********************************
   /*!\brief Implementation of the transpose addition assignment of a row-major sparse matrix.
   //
   // \param rhs The right-hand side sparse matrix to be added.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename MT2 >  // Type of the right-hand side sparse matrix
   inline void addAssign( const SparseMatrix<MT2,SO>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( dm_.columns() == (~rhs).rows(), "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( dm_.rows() == (~rhs).columns(), "Invalid number of columns" );

      typedef typename MT2::ConstIterator  ConstIterator;

      for( size_t i=0UL; i<(~rhs).rows(); ++i )
         for( ConstIterator element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
            dm_(element->index(),i) += element->value();
   }
   //**********************************************************************************************

   //**Transpose addition assignment of column-major sparse matrices*******************************
   /*!\brief Implementation of the transpose addition assignment of a column-major sparse matrix.
   //
   // \param rhs The right-hand side sparse matrix to be added.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename MT2 >  // Type of the right-hand side sparse matrix
   inline void addAssign( const SparseMatrix<MT2,!SO>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( dm_.columns() == (~rhs).rows(), "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( dm_.rows() == (~rhs).columns(), "Invalid number of columns" );

      typedef typename MT2::ConstIterator  ConstIterator;

      for( size_t j=0UL; j<(~rhs).columns(); ++j )
         for( ConstIterator element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
            dm_(j,element->index()) += element->value();
   }
   //**********************************************************************************************

   //**Transpose subtraction assignment of row-major dense matrices********************************
   /*!\brief Implementation of the transpose subtraction assignment of a row-major dense matrix.
   //
   // \param rhs The right-hand side dense matrix to be subtracted.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename MT2 >  // Type of the right-hand side dense matrix
   inline void subAssign( const DenseMatrix<MT2,SO>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( dm_.columns() == (~rhs).rows(), "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( dm_.rows() == (~rhs).columns(), "Invalid number of columns" );

      const size_t m( rows() );
      const size_t n( columns() );

      BLAZE_INTERNAL_ASSERT( ( n - ( n % 2UL ) ) == ( n & size_t(-2) ), "Invalid end calculation" );
      const size_t end( n & size_t(-2) );

      for( size_t i=0UL; i<m; ++i ) {
         for( size_t j=0UL; j<end; j+=2UL ) {
            dm_(j    ,i) -= (~rhs)(i,j    );
            dm_(j+1UL,i) -= (~rhs)(i,j+1UL);

         }
         if( end < n ) {
            dm_(end,i) -= (~rhs)(i,end);
         }
      }
   }
   //**********************************************************************************************

   //**Transpose subtraction assignment of column-major dense matrices*****************************
   /*!\brief Implementation of the transpose subtraction assignment of a column-major dense matrix.
   //
   // \param rhs The right-hand side dense matrix to be subtracted.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename MT2 >  // Type of the right-hand side dense matrix
   inline void subAssign( const DenseMatrix<MT2,!SO>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( dm_.columns() == (~rhs).rows(), "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( dm_.rows() == (~rhs).columns(), "Invalid number of columns" );

      const size_t m( rows() );
      const size_t n( columns() );
      const size_t block( 16UL );

      for( size_t ii=0UL; ii<m; ii+=block ) {
         const size_t iend( ( m < ii+block )?( m ):( ii+block ) );
         for( size_t jj=0UL; jj<n; jj+=block ) {
            const size_t jend( ( n < jj+block )?( n ):( jj+block ) );
            for( size_t i=ii; i<iend; ++i ) {
               for( size_t j=jj; j<jend; ++j ) {
                  dm_(j,i) -= (~rhs)(i,j);
               }
            }
         }
      }
   }
   //**********************************************************************************************

   //**Transpose subtraction assignment of row-major sparse matrices*******************************
   /*!\brief Implementation of the transpose subtraction assignment of a row-major sparse matrix.
   //
   // \param rhs The right-hand side sparse matrix to be subtracted.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename MT2 >  // Type of the right-hand side sparse matrix
   inline void subAssign( const SparseMatrix<MT2,SO>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( dm_.columns() == (~rhs).rows(), "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( dm_.rows() == (~rhs).columns(), "Invalid number of columns" );

      typedef typename MT2::ConstIterator  ConstIterator;

      for( size_t i=0UL; i<(~rhs).rows(); ++i )
         for( ConstIterator element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
            dm_(element->index(),i) -= element->value();
   }
   //**********************************************************************************************

   //**Transpose subtraction assignment of column-major dense matrices*****************************
   /*!\brief Implementation of the transpose subtraction assignment of a column-major sparse matrix.
   //
   // \param rhs The right-hand side sparse matrix to be subtracted.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename MT2 >  // Type of the right-hand side sparse matrix
   inline void subAssign( const SparseMatrix<MT2,!SO>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( dm_.columns() == (~rhs).rows(), "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( dm_.rows() == (~rhs).columns(), "Invalid number of columns" );

      typedef typename MT2::ConstIterator  ConstIterator;

      for( size_t j=0UL; j<(~rhs).columns(); ++j )
         for( ConstIterator element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
            dm_(j,element->index()) -= element->value();
   }
   //**********************************************************************************************

 private:
   //**Member variables****************************************************************************
   MT& dm_;  //!< The dense matrix operand.
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( MT );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_EXPRESSION_TYPE( MT );
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
/*!\brief Specialization of DMatTransposer for row-major matrices.
// \ingroup dense_matrix_expression
//
// This specialization of DMatTransposer adapts the class template to the requirements of
// row-major matrices.
*/
template< typename MT >  // Type of the dense matrix
class DMatTransposer<MT,true> : public DenseMatrix< DMatTransposer<MT,true>, true >
{
 public:
   //**Type definitions****************************************************************************
   typedef DMatTransposer<MT,true>     This;           //!< Type of this DMatTransposer instance.
   typedef typename MT::TransposeType  ResultType;     //!< Result type for expression template evaluations.
   typedef typename MT::OppositeType   OppositeType;   //!< Result type with opposite storage order for expression template evaluations.
   typedef typename MT::ResultType     TransposeType;  //!< Transpose type for expression template evaluations.
   typedef typename MT::ElementType    ElementType;    //!< Resulting element type.
   typedef const This&                 CompositeType;  //!< Data type for composite expression templates.
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation flag for intrinsic optimization.
   /*! The \a vectorizable compilation flag indicates whether expressions the matrix is involved
       in can be optimized via intrinsics. In case the dense matrix operand is vectorizable, the
       \a vectorizable compilation flag is set to \a true, otherwise it is set to \a false. */
   enum { vectorizable = MT::vectorizable };
   //**********************************************************************************************

   //**Constructor*********************************************************************************
   /*!\brief Constructor for the DMatTransposer class.
   //
   // \param dm The dense matrix operand.
   */
   explicit inline DMatTransposer( MT& dm )
      : dm_( dm )  // The dense matrix operand
   {}
   //**********************************************************************************************

   //**Access operator*****************************************************************************
   /*!\brief 2D-access to the matrix elements.
   //
   // \param i Access index for the row. The index has to be in the range \f$[0..M-1]\f$.
   // \param j Access index for the column. The index has to be in the range \f$[0..N-1]\f$.
   // \return Reference to the accessed value.
   */
   inline ElementType& operator()( size_t i, size_t j ) {
      BLAZE_INTERNAL_ASSERT( i < dm_.columns(), "Invalid row access index"    );
      BLAZE_INTERNAL_ASSERT( j < dm_.rows()   , "Invalid column access index" );
      return dm_(j,i);
   }
   //**********************************************************************************************

   //**Low-level data access***********************************************************************
   /*!\brief Low-level data access to the matrix elements.
   //
   // \return Pointer to the internal element storage.
   */
   inline ElementType* data() {
      return dm_.data();
   }
   //**********************************************************************************************

   //**Rows function*******************************************************************************
   /*!\brief Returns the current number of rows of the matrix.
   //
   // \return The number of rows of the matrix.
   */
   inline size_t rows() const {
      return dm_.columns();
   }
   //**********************************************************************************************

   //**Columns function****************************************************************************
   /*!\brief Returns the current number of columns of the matrix.
   //
   // \return The number of columns of the matrix.
   */
   inline size_t columns() const {
      return dm_.rows();
   }
   //**********************************************************************************************

   //**Spacing function****************************************************************************
   /*!\brief Returns the spacing between the beginning of two columns.
   //
   // \return The spacing between the beginning of two columns.
   */
   inline size_t spacing() const {
      return dm_.spacing();
   }
   //**********************************************************************************************

   //**Reset function******************************************************************************
   /*!\brief Resets the matrix elements.
   //
   // \return void
   */
   inline void reset() {
      return dm_.reset();
   }
   //**********************************************************************************************

   //**Transpose assignment of column-major dense matrices*****************************************
   /*!\brief Implementation of the transpose assignment of a column-major dense matrix.
   //
   // \param rhs The right-hand side dense matrix to be assigned.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename MT2 >  // Type of the right-hand side dense matrix
   inline void assign( const DenseMatrix<MT2,true>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( dm_.columns() == (~rhs).rows(), "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( dm_.rows() == (~rhs).columns(), "Invalid number of columns" );

      const size_t m( rows() );
      const size_t n( columns() );

      BLAZE_INTERNAL_ASSERT( ( m - ( m % 2UL ) ) == ( m & size_t(-2) ), "Invalid end calculation" );
      const size_t end( m & size_t(-2) );

      for( size_t j=0UL; j<n; ++j ) {
         for( size_t i=0UL; i<end; i+=2UL ) {
            dm_(j,i    ) = (~rhs)(i    ,j);
            dm_(j,i+1UL) = (~rhs)(i+1UL,j);
         }
         if( end < m ) {
            dm_(j,end) = (~rhs)(end,j);
         }
      }
   }
   //**********************************************************************************************

   //**Transpose assignment of row-major dense matrices********************************************
   /*!\brief Implementation of the transpose assignment of a row-major dense matrix.
   //
   // \param rhs The right-hand side dense matrix to be assigned.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename MT2 >  // Type of the right-hand side dense matrix
   inline void assign( const DenseMatrix<MT2,false>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( dm_.columns() == (~rhs).rows(), "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( dm_.rows() == (~rhs).columns(), "Invalid number of columns" );

      const size_t m( rows() );
      const size_t n( columns() );
      const size_t block( 16UL );

      for( size_t jj=0UL; jj<n; jj+=block ) {
         const size_t jend( ( n < jj+block )?( n ):( jj+block ) );
         for( size_t ii=0UL; ii<m; ii+=block ) {
            const size_t iend( ( m < ii+block )?( m ):( ii+block ) );
            for( size_t j=jj; j<jend; ++j ) {
               for( size_t i=ii; i<iend; ++i ) {
                  dm_(j,i) = (~rhs)(i,j);
               }
            }
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

      BLAZE_INTERNAL_ASSERT( dm_.columns() == (~rhs).rows(), "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( dm_.rows() == (~rhs).columns(), "Invalid number of columns" );

      typedef typename MT2::ConstIterator  ConstIterator;

      for( size_t j=0UL; j<(~rhs).columns(); ++j )
         for( ConstIterator element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
            dm_(j,element->index()) = element->value();
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

      BLAZE_INTERNAL_ASSERT( dm_.columns() == (~rhs).rows(), "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( dm_.rows() == (~rhs).columns(), "Invalid number of columns" );

      typedef typename MT2::ConstIterator  ConstIterator;

      for( size_t i=0UL; i<(~rhs).rows(); ++i )
         for( ConstIterator element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
            dm_(element->index(),i) = element->value();
   }
   //**********************************************************************************************

   //**Transpose addition assignment of column-major dense matrices********************************
   /*!\brief Implementation of the transpose addition assignment of a column-major dense matrix.
   //
   // \param rhs The right-hand side dense matrix to be added.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename MT2 >  // Type of the right-hand side dense matrix
   inline void addAssign( const DenseMatrix<MT2,true>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( dm_.columns() == (~rhs).rows(), "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( dm_.rows() == (~rhs).columns(), "Invalid number of columns" );

      const size_t m( rows() );
      const size_t n( columns() );

      BLAZE_INTERNAL_ASSERT( ( m - ( m % 2UL ) ) == ( m & size_t(-2) ), "Invalid end calculation" );
      const size_t end( m & size_t(-2) );

      for( size_t j=0UL; j<n; ++j ) {
         for( size_t i=0UL; i<end; i+=2UL ) {
            dm_(j,i    ) += (~rhs)(i    ,j);
            dm_(j,i+1UL) += (~rhs)(i+1UL,j);
         }
         if( end < m ) {
            dm_(j,end) += (~rhs)(end,j);
         }
      }
   }
   //**********************************************************************************************

   //**Transpose addition assignment of row-major dense matrices***********************************
   /*!\brief Implementation of the transpose addition assignment of a row-major dense matrix.
   //
   // \param rhs The right-hand side dense matrix to be added.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename MT2 >  // Type of the right-hand side dense matrix
   inline void addAssign( const DenseMatrix<MT2,false>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( dm_.columns() == (~rhs).rows(), "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( dm_.rows() == (~rhs).columns(), "Invalid number of columns" );

      const size_t m( rows() );
      const size_t n( columns() );
      const size_t block( 16UL );

      for( size_t jj=0UL; jj<n; jj+=block ) {
         const size_t jend( ( n < jj+block )?( n ):( jj+block ) );
         for( size_t ii=0UL; ii<m; ii+=block ) {
            const size_t iend( ( m < ii+block )?( m ):( ii+block ) );
            for( size_t j=jj; j<jend; ++j ) {
               for( size_t i=ii; i<iend; ++i ) {
                  dm_(j,i) += (~rhs)(i,j);
               }
            }
         }
      }
   }
   //**********************************************************************************************

   //**Transpose addition assignment of column-major sparse matrices*******************************
   /*!\brief Implementation of the transpose addition assignment of a column-major sparse matrix.
   //
   // \param rhs The right-hand side sparse matrix to be added.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename MT2 >  // Type of the right-hand side sparse matrix
   inline void addAssign( const SparseMatrix<MT2,true>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( dm_.columns() == (~rhs).rows(), "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( dm_.rows() == (~rhs).columns(), "Invalid number of columns" );

      typedef typename MT2::ConstIterator  ConstIterator;

      for( size_t j=0UL; j<(~rhs).columns(); ++j )
         for( ConstIterator element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
            dm_(j,element->index()) += element->value();
   }
   //**********************************************************************************************

   //**Transpose addition assignment of row-major sparse matrices**********************************
   /*!\brief Implementation of the transpose addition assignment of a row-major sparse matrix.
   //
   // \param rhs The right-hand side sparse matrix to be added.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename MT2 >  // Type of the right-hand side sparse matrix
   inline void addAssign( const SparseMatrix<MT2,false>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( dm_.columns() == (~rhs).rows(), "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( dm_.rows() == (~rhs).columns(), "Invalid number of columns" );

      typedef typename MT2::ConstIterator  ConstIterator;

      for( size_t i=0UL; i<(~rhs).rows(); ++i )
         for( ConstIterator element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
            dm_(element->index(),i) += element->value();
   }
   //**********************************************************************************************

   //**Transpose subtraction assignment of column-major dense matrices*****************************
   /*!\brief Implementation of the transpose subtraction assignment of a column-major dense matrix.
   //
   // \param rhs The right-hand side dense matrix to be subtracted.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename MT2 >  // Type of the right-hand side dense matrix
   inline void subAssign( const DenseMatrix<MT2,true>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( dm_.columns() == (~rhs).rows(), "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( dm_.rows() == (~rhs).columns(), "Invalid number of columns" );

      const size_t m( rows() );
      const size_t n( columns() );

      BLAZE_INTERNAL_ASSERT( ( m - ( m % 2UL ) ) == ( m & size_t(-2) ), "Invalid end calculation" );
      const size_t end( m & size_t(-2) );

      for( size_t j=0UL; j<n; ++j ) {
         for( size_t i=0UL; i<end; i+=2UL ) {
            dm_(j,i    ) -= (~rhs)(i    ,j);
            dm_(j,i+1UL) -= (~rhs)(i+1UL,j);
         }
         if( end < m ) {
            dm_(j,end) -= (~rhs)(end,j);
         }
      }
   }
   //**********************************************************************************************

   //**Transpose subtraction assignment of row-major dense matrices********************************
   /*!\brief Implementation of the transpose subtraction assignment of a row-major dense matrix.
   //
   // \param rhs The right-hand side dense matrix to be subtracted.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename MT2 >  // Type of the right-hand side dense matrix
   inline void subAssign( const DenseMatrix<MT2,false>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( dm_.columns() == (~rhs).rows(), "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( dm_.rows() == (~rhs).columns(), "Invalid number of columns" );

      const size_t m( rows() );
      const size_t n( columns() );
      const size_t block( 16UL );

      for( size_t jj=0UL; jj<n; jj+=block ) {
         const size_t jend( ( n < jj+block )?( n ):( jj+block ) );
         for( size_t ii=0UL; ii<m; ii+=block ) {
            const size_t iend( ( m < ii+block )?( m ):( ii+block ) );
            for( size_t j=jj; j<jend; ++j ) {
               for( size_t i=ii; i<iend; ++i ) {
                  dm_(j,i) -= (~rhs)(i,j);
               }
            }
         }
      }
   }
   //**********************************************************************************************

   //**Transpose subtraction assignment of column-major sparse matrices****************************
   /*!\brief Implementation of the transpose subtraction assignment of a column-major sparse matrix.
   //
   // \param rhs The right-hand side sparse matrix to be subtracted.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename MT2 >  // Type of the right-hand side sparse matrix
   inline void subAssign( const SparseMatrix<MT2,true>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( dm_.columns() == (~rhs).rows(), "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( dm_.rows() == (~rhs).columns(), "Invalid number of columns" );

      typedef typename MT2::ConstIterator  ConstIterator;

      for( size_t j=0UL; j<(~rhs).columns(); ++j )
         for( ConstIterator element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
            dm_(j,element->index()) -= element->value();
   }
   //**********************************************************************************************

   //**Transpose subtraction assignment of row-major dense matrices********************************
   /*!\brief Implementation of the transpose subtraction assignment of a row-major sparse matrix.
   //
   // \param rhs The right-hand side sparse matrix to be subtracted.
   // \return void
   //
   // This function must \b NOT be called explicitly! It is used internally for the performance
   // optimized evaluation of expression templates. Calling this function explicitly might result
   // in erroneous results and/or in compilation errors. Instead of using this function use the
   // assignment operator.
   */
   template< typename MT2 >  // Type of the right-hand side sparse matrix
   inline void subAssign( const SparseMatrix<MT2,false>& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT2 );

      BLAZE_INTERNAL_ASSERT( dm_.columns() == (~rhs).rows(), "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( dm_.rows() == (~rhs).columns(), "Invalid number of columns" );

      typedef typename MT2::ConstIterator  ConstIterator;

      for( size_t i=0UL; i<(~rhs).rows(); ++i )
         for( ConstIterator element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
            dm_(element->index(),i) -= element->value();
   }
   //**********************************************************************************************

   //**Transpose multiplication assignment of dense matrices***************************************
   // No special implementation for the transpose multiplication assignment of dense matrices.
   //**********************************************************************************************

   //**Transpose multiplication assignment of sparse matrices**************************************
   // No special implementation for the transpose multiplication assignment of sparse matrices.
   //**********************************************************************************************

 private:
   //**Member variables****************************************************************************
   MT& dm_;  //!< The dense matrix operand.
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( MT );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_EXPRESSION_TYPE( MT );
   /*! \endcond */
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  GLOBAL OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Resetting the dense matrix contained in a DMatTransposer.
// \ingroup dense_matrix_expression
//
// \param m The dense matrix to be resetted.
// \return void
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline void reset( DMatTransposer<MT,SO>& m )
{
   m.reset();
}
/*! \endcond */
//*************************************************************************************************

} // namespace blaze

#endif
