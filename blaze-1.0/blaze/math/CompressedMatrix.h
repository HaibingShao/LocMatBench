//=================================================================================================
/*!
//  \file blaze/math/CompressedMatrix.h
//  \brief Implementation of a compressed MxN matrix
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

#ifndef _BLAZE_MATH_COMPRESSEDMATRIX_H_
#define _BLAZE_MATH_COMPRESSEDMATRIX_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <vector>
#include <blaze/math/CompressedVector.h>
#include <blaze/math/DynamicMatrix.h>
#include <blaze/math/DynamicVector.h>
#include <blaze/math/MathTrait.h>
#include <blaze/math/shims/Equal.h>
#include <blaze/math/shims/IsDefault.h>
#include <blaze/math/shims/IsNaN.h>
#include <blaze/math/sparse/MatrixAccessProxy.h>
#include <blaze/math/sparse/SparseElement.h>
#include <blaze/math/SparseMatrix.h>
#include <blaze/math/Types.h>
#include <blaze/math/typetraits/IsResizable.h>
#include <blaze/math/typetraits/IsSparseMatrix.h>
#include <blaze/system/Precision.h>
#include <blaze/system/StorageOrder.h>
#include <blaze/util/Assert.h>
#include <blaze/util/constraints/Const.h>
#include <blaze/util/constraints/Numeric.h>
#include <blaze/util/constraints/Pointer.h>
#include <blaze/util/constraints/Reference.h>
#include <blaze/util/constraints/SameSize.h>
#include <blaze/util/constraints/Volatile.h>
#include <blaze/util/EnableIf.h>
#include <blaze/util/mpl/If.h>
#include <blaze/util/Null.h>
#include <blaze/util/Types.h>
#include <blaze/util/typetraits/IsFloatingPoint.h>
#include <blaze/util/typetraits/IsNumeric.h>


namespace blaze {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*!\defgroup compressed_matrix CompressedMatrix
// \ingroup sparse_matrix
*/
/*!\brief Efficient implementation of a \f$ M \times N \f$ compressed matrix.
// \ingroup compressed_matrix
//
// The CompressedMatrix class template is the representation of an arbitrary sized sparse
// matrix with \f$ M \cdot N \f$ dynamically allocated elements of arbitrary type. The type
// of the elements and the storage order of the matrix can be specified via the two template
// parameters:

   \code
   template< typename Type, bool SO >
   class CompressedMatrix;
   \endcode

//  - Type: specifies the type of the matrix elements. CompressedMatrix can be used with
//          any non-cv-qualified element type. The arithmetic operators for matrix/matrix,
//          matrix/vector and matrix/element operations with the same element type work
//          for any element type as long as the element type supports the arithmetic
//          operation. Arithmetic operations between matrices, vectors and elements of
//          different element types are only supported for all data types supported by
//          the MathTrait class template (for details see the MathTrait class description).
//  - SO  : specifies the storage order (blaze::rowMajor, blaze::columnMajor) of the matrix.
//          The default value is blaze::rowMajor.
//
// Inserting/accessing elements in a compressed matrix can be done by several alternative
// functions. The following example demonstrates all options:

   \code
   using blaze::rowMajor;
   using blaze::columnMajor;

   // Creating a row-major 4x3 compressed matrix with 4 rows and 3 columns
   CompressedMatrix<double,rowMajor> A( 4, 3 );

   // The function call operator provides access to all possible elements of the compressed matrix,
   // including the zero elements. In case the function call operator is used to access an element
   // that is currently not stored in the sparse matrix, the element is inserted into the matrix.
   A(1,2) = 2.0;

   // An alternative for inserting elements into the matrix is the insert() function. However,
   // it inserts the element only in case the element is not already contained in the matrix.
   A.insert( 2, 1, 3.7 );

   // A very efficient way to add new elements to a sparse matrix is the append() function.
   // Note that append() requires that the appended element's index is strictly larger than
   // the currently largest non-zero index of the specified row and that the rows's capacity
   // is large enough to hold the new element.
   A.reserve( 3, 2 );       // Reserving space for 2 non-zero elements in row 3
   A.append( 3, 1, -2.1 );  // Appending the value -2.1 at column index 1 in row 3
   A.append( 3, 2,  1.4 );  // Appending the value 1.4 at column index 2 in row 3

   // In order to traverse all non-zero elements currently stored in the matrix, the begin()
   // and end() functions can be used. In the example, all non-zero elements of the 2nd row
   // are traversed.
   for( CompressedMatrix<double,rowMajor>::Iterator i=A.begin(1); i!=A.end(1); ++i ) {
      ... = i->value();  // Access to the value of the non-zero element
      ... = i->index();  // Access to the index of the non-zero element
   }
   \endcode

// The use of CompressedMatrix is very natural and intuitive. All operations (addition, subtraction,
// multiplication, scaling, ...) can be performed on all possible combination of row-major and
// column-major dense and sparse matrices with fitting element types. The following example gives
// an impression of the use of CompressedMatrix:

   \code
   using blaze::CompressedMatrix;
   using blaze::DynamicMatrix;
   using blaze::rowMajor;
   using blaze::columnMajor;

   CompressedMatrix<double,rowMajor> A( 2, 3 );  // Default constructed, non-initialized, row-major 2x3 matrix
   A(0,0) = 1.0; A(0,2) = 3.0; A(1,1) = 5.0;     // Element initialization

   CompressedMatrix<float,columnMajor> B( 2, 3 );  // Default constructed column-major single precision 2x3 matrix
   B(0,1) = 3.0; B(1,0) = 2.0; B(1,2) = 6.0;       // Element initialization

   DynamicMatrixMatrix<float> C( 2, 3, 4.0F );  // Directly, homogeneously initialized single precision dense 2x3 matrix
   CompressedMatrix<float>    D( 3, 2 );        // Empty row-major sparse single precision matrix

   CompressedMatrix<double,rowMajor>    E( A );  // Creation of a new row-major matrix as a copy of A
   CompressedMatrix<double,columnMajor> F;       // Creation of a default column-major matrix

   E = A + B;     // Matrix addition and assignment to a row-major matrix
   E = A - C;     // Matrix subtraction and assignment to a column-major matrix
   F = A * D;     // Matrix multiplication between two matrices of different element types

   A *= 2.0;      // In-place scaling of matrix A
   E  = 2.0 * B;  // Scaling of matrix B
   F  = D * 2.0;  // Scaling of matrix D

   E += A - B;    // Addition assignment
   E -= A + C;    // Subtraction assignment
   F *= A * D;    // Multiplication assignment
   \endcode
*/
template< typename Type                    // Data type of the sparse matrix
        , bool SO = defaultStorageOrder >  // Storage order
class CompressedMatrix : public SparseMatrix< CompressedMatrix<Type,SO>, SO >
{
 private:
   //**Type definitions****************************************************************************
   typedef SparseElement<Type>  ElementBase;  //!< Base class for the sparse matrix element.
   //**********************************************************************************************

   //**Private class Element***********************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Index-value-pair for the CompressedMatrix class.
   */
   struct Element : public ElementBase
   {
      using ElementBase::operator=;
      friend class CompressedMatrix;
   };
   /*! \endcond */
   //**********************************************************************************************

   //**Private class FindIndex*********************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Helper class for the lower_bound() function.
   */
   struct FindIndex : public std::binary_function<Element,size_t,bool>
   {
      inline bool operator()( const Element& element, size_t index ) const {
         return element.index() < index;
      }
      inline bool operator()( size_t index, const Element& element ) const {
         return index < element.index();
      }
      inline bool operator()( const Element& element1, const Element& element2 ) const {
         return element1.index() < element2.index();
      }
   };
   /*! \endcond */
   //**********************************************************************************************

 public:
   //**Type definitions****************************************************************************
   typedef CompressedMatrix<Type,SO>   This;            //!< Type of this CompressedMatrix instance.
   typedef This                        ResultType;      //!< Result type for expression template evaluations.
   typedef CompressedMatrix<Type,!SO>  OppositeType;    //!< Result type with opposite storage order for expression template evaluations.
   typedef CompressedMatrix<Type,!SO>  TransposeType;   //!< Transpose type for expression template evaluations.
   typedef Type                        ElementType;     //!< Type of the sparse matrix elements.
   typedef const This&                 CompositeType;   //!< Data type for composite expression templates.
   typedef MatrixAccessProxy<This>     Reference;       //!< Reference to a sparse matrix value.
   typedef const Type&                 ConstReference;  //!< Reference to a constant sparse matrix value.
   typedef Element*                    Iterator;        //!< Iterator over non-constant elements.
   typedef const Element*              ConstIterator;   //!< Iterator over constant elements.
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation flag for the detection of aliasing effects.
   /*! This compilation switch indicates whether this type potentially causes compuation errors
       due to aliasing effects. In case the type can cause aliasing effects, the \a canAlias
       switch is set to \a true, otherwise it is set to \a false. */
   enum { canAlias = 0 };
   //**********************************************************************************************

   //**Constructors********************************************************************************
   /*!\name Constructors */
   //@{
                            explicit inline CompressedMatrix();
                            explicit inline CompressedMatrix( size_t m, size_t n );
                            explicit inline CompressedMatrix( size_t m, size_t n, size_t nonzeros );
                            explicit        CompressedMatrix( size_t m, size_t n, const std::vector<size_t>& nonzeros );
                                     inline CompressedMatrix( const CompressedMatrix& sm );
   template< typename MT, bool SO2 > inline CompressedMatrix( const DenseMatrix<MT,SO2>&  dm );
   template< typename MT, bool SO2 > inline CompressedMatrix( const SparseMatrix<MT,SO2>& sm );
   //@}
   //**********************************************************************************************

   //**Destructor**********************************************************************************
   /*!\name Destructor */
   //@{
   inline ~CompressedMatrix();
   //@}
   //**********************************************************************************************

   //**Data access functions***********************************************************************
   /*!\name Data access functions */
   //@{
   inline Reference      operator()( size_t i, size_t j );
   inline ConstReference operator()( size_t i, size_t j ) const;
   inline Iterator       begin( size_t i );
   inline ConstIterator  begin( size_t i ) const;
   inline Iterator       end  ( size_t i );
   inline ConstIterator  end  ( size_t i ) const;
   //@}
   //**********************************************************************************************

   //**Assignment operators************************************************************************
   /*!\name Assignment operators */
   //@{
                                     inline CompressedMatrix& operator= ( const CompressedMatrix& rhs );
   template< typename MT, bool SO2 > inline CompressedMatrix& operator= ( const DenseMatrix<MT,SO2>&  rhs );
   template< typename MT, bool SO2 > inline CompressedMatrix& operator= ( const SparseMatrix<MT,SO2>& rhs );
   template< typename MT, bool SO2 > inline CompressedMatrix& operator+=( const Matrix<MT,SO2>& rhs );
   template< typename MT, bool SO2 > inline CompressedMatrix& operator-=( const Matrix<MT,SO2>& rhs );
   template< typename MT, bool SO2 > inline CompressedMatrix& operator*=( const Matrix<MT,SO2>& rhs );

   template< typename Other >
   inline typename EnableIf< IsNumeric<Other>, CompressedMatrix >::Type&
      operator*=( Other rhs );

   template< typename Other >
   inline typename EnableIf< IsNumeric<Other>, CompressedMatrix >::Type&
      operator/=( Other rhs );
   //@}
   //**********************************************************************************************

   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
                              inline size_t            rows() const;
                              inline size_t            columns() const;
                              inline size_t            capacity() const;
                              inline size_t            capacity( size_t i ) const;
                              inline size_t            nonZeros() const;
                              inline size_t            nonZeros( size_t i ) const;
                              inline void              reset();
                              inline void              clear();
                                     Iterator          insert ( size_t i, size_t j, const Type& value );
                              inline Iterator          find   ( size_t i, size_t j );
                              inline ConstIterator     find   ( size_t i, size_t j ) const;
                                     void              resize ( size_t m, size_t n, bool preserve=true );
                              inline void              reserve( size_t nonzeros );
                                     void              reserve( size_t i, size_t nonzeros );
                              inline CompressedMatrix& transpose();
                                     bool              isDiagonal() const;
                                     bool              isSymmetric() const;
   template< typename Other > inline CompressedMatrix& scale( Other scalar );
   template< typename Other > inline CompressedMatrix& scaleDiagonal( Other scalar );
                              inline void              swap( CompressedMatrix& sm ) /* throw() */;
   //@}
   //**********************************************************************************************

   //**Low-level utility functions*****************************************************************
   /*!\name Low-level utility functions */
   //@{
   inline void append  ( size_t i, size_t j, const Type& value );
   inline void finalize( size_t i );
   //@}
   //**********************************************************************************************

   //**Expression template evaluation functions****************************************************
   /*!\name Expression template evaluation functions */
   //@{
   template< typename Other > inline bool isAliased ( const Other* alias ) const;
   template< typename MT, bool SO2 > inline void assign   ( const DenseMatrix<MT,SO2>&  rhs );
   template< typename MT >           inline void assign   ( const SparseMatrix<MT,SO>&  rhs );
   template< typename MT >           inline void assign   ( const SparseMatrix<MT,!SO>& rhs );
   template< typename MT, bool SO2 > inline void addAssign( const DenseMatrix<MT,SO2>&  rhs );
   template< typename MT, bool SO2 > inline void addAssign( const SparseMatrix<MT,SO2>& rhs );
   template< typename MT, bool SO2 > inline void subAssign( const DenseMatrix<MT,SO2>&  rhs );
   template< typename MT, bool SO2 > inline void subAssign( const SparseMatrix<MT,SO2>& rhs );
   //@}
   //**********************************************************************************************

 private:
   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
   inline size_t extendCapacity() const;
   inline void   reserveElements( size_t nonzeros );
   //@}
   //**********************************************************************************************

   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   size_t m_;         //!< The current number of rows of the sparse matrix.
   size_t n_;         //!< The current number of columns of the sparse matrix.
   size_t capacity_;  //!< The current capacity of the pointer array.
   Iterator* begin_;  //!< Pointers to the first non-zero element of each row.
   Iterator* end_;    //!< Pointers one past the last non-zero element of each row.

   static const Type zero_;  //!< Neutral element for accesses to zero elements.
   //@}
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_NOT_BE_POINTER_TYPE  ( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_REFERENCE_TYPE( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_CONST         ( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_VOLATILE      ( Type );
   BLAZE_CONSTRAINT_MUST_HAVE_SAME_SIZE       ( ElementBase, Element );
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************




//=================================================================================================
//
//  DEFINITION AND INITIALIZATION OF THE STATIC MEMBER VARIABLES
//
//=================================================================================================

template< typename Type, bool SO >
const Type CompressedMatrix<Type,SO>::zero_ = Type();




//=================================================================================================
//
//  CONSTRUCTORS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief The default constructor for CompressedMatrix.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline CompressedMatrix<Type,SO>::CompressedMatrix()
   : m_       ( 0UL )           // The current number of rows of the sparse matrix
   , n_       ( 0UL )           // The current number of columns of the sparse matrix
   , capacity_( 0UL )           // The current capacity of the pointer array
   , begin_( new Iterator[2] )  // Pointers to the first non-zero element of each row
   , end_  ( begin_+1 )         // Pointers one past the last non-zero element of each row
{
   begin_[0] = end_[0] = NULL;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constructor for a matrix of size \f$ M \times N \f$.
//
// \param m The number of rows of the matrix.
// \param n The number of columns of the matrix.
//
// The matrix is initialized to the zero matrix and has no free capacity.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline CompressedMatrix<Type,SO>::CompressedMatrix( size_t m, size_t n )
   : m_       ( m )                     // The current number of rows of the sparse matrix
   , n_       ( n )                     // The current number of columns of the sparse matrix
   , capacity_( m )                     // The current capacity of the pointer array
   , begin_( new Iterator[2UL*m+2UL] )  // Pointers to the first non-zero element of each row
   , end_  ( begin_+(m+1UL) )           // Pointers one past the last non-zero element of each row
{
   for( size_t i=0UL; i<2UL*m_+2UL; ++i )
      begin_[i] = NULL;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constructor for a matrix of size \f$ M \times N \f$.
//
// \param m The number of rows of the matrix.
// \param n The number of columns of the matrix.
// \param nonzeros The number of expected non-zero elements.
//
// The matrix is initialized to the zero matrix.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline CompressedMatrix<Type,SO>::CompressedMatrix( size_t m, size_t n, size_t nonzeros )
   : m_       ( m )                     // The current number of rows of the sparse matrix
   , n_       ( n )                     // The current number of columns of the sparse matrix
   , capacity_( m )                     // The current capacity of the pointer array
   , begin_( new Iterator[2UL*m+2UL] )  // Pointers to the first non-zero element of each row
   , end_  ( begin_+(m+1UL) )           // Pointers one past the last non-zero element of each row
{
   begin_[0UL] = new Element[nonzeros];
   for( size_t i=1UL; i<(2UL*m_+1UL); ++i )
      begin_[i] = begin_[0UL];
   end_[m_] = begin_[0UL]+nonzeros;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constructor for a matrix of size \f$ M \times N \f$.
//
// \param m The number of rows of the matrix.
// \param n The number of columns of the matrix.
// \param nonzeros The expected number of non-zero elements in each row.
//
// The matrix is initialized to the zero matrix.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
CompressedMatrix<Type,SO>::CompressedMatrix( size_t m, size_t n, const std::vector<size_t>& nonzeros )
   : m_       ( m )                      // The current number of rows of the sparse matrix
   , n_       ( n )                      // The current number of columns of the sparse matrix
   , capacity_( m )                      // The current capacity of the pointer array
   , begin_( new Iterator[2UL*m_+2UL] )  // Pointers to the first non-zero element of each row
   , end_  ( begin_+(m_+1UL) )           // Pointers one past the last non-zero element of each row
{
   BLAZE_USER_ASSERT( nonzeros.size() == m, "Size of capacity vector and number of rows don't match" );

   size_t newCapacity( 0UL );
   for( std::vector<size_t>::const_iterator it=nonzeros.begin(); it!=nonzeros.end(); ++it )
      newCapacity += *it;

   begin_[0UL] = end_[0UL] = new Element[newCapacity];
   for( size_t i=0UL; i<m_; ++i ) {
      begin_[i+1UL] = end_[i+1UL] = begin_[i] + nonzeros[i];
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief The copy constructor for CompressedMatrix.
//
// \param sm Sparse matrix to be copied.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline CompressedMatrix<Type,SO>::CompressedMatrix( const CompressedMatrix& sm )
   : m_       ( sm.m_ )                     // The current number of rows of the sparse matrix
   , n_       ( sm.n_ )                     // The current number of columns of the sparse matrix
   , capacity_( sm.m_ )                     // The current capacity of the pointer array
   , begin_   ( new Iterator[2UL*m_+2UL] )  // Pointers to the first non-zero element of each row
   , end_     ( begin_+(m_+1UL) )           // Pointers one past the last non-zero element of each row
{
   const size_t nonzeros( sm.nonZeros() );

   begin_[0UL] = new Element[nonzeros];
   for( size_t i=0UL; i<m_; ++i )
      begin_[i+1UL] = end_[i] = std::copy( sm.begin(i), sm.end(i), begin_[i] );
   end_[m_] = begin_[0UL]+nonzeros;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Conversion constructor from dense matrices.
//
// \param dm Dense matrix to be copied.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
template< typename MT    // Type of the foreign dense matrix
        , bool SO2 >     // Storage order of the foreign dense matrix
inline CompressedMatrix<Type,SO>::CompressedMatrix( const DenseMatrix<MT,SO2>& dm )
   : m_       ( (~dm).rows() )              // The current number of rows of the sparse matrix
   , n_       ( (~dm).columns() )           // The current number of columns of the sparse matrix
   , capacity_( m_ )                        // The current capacity of the pointer array
   , begin_   ( new Iterator[2UL*m_+2UL] )  // Pointers to the first non-zero element of each row
   , end_     ( begin_+(m_+1UL) )           // Pointers one past the last non-zero element of each row
{
   using blaze::assign;

   for( size_t i=0UL; i<2UL*m_+2UL; ++i )
      begin_[i] = NULL;

   assign( *this, ~dm );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Conversion constructor from different sparse matrices.
//
// \param sm Sparse matrix to be copied.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
template< typename MT    // Type of the foreign sparse matrix
        , bool SO2 >     // Storage order of the foreign sparse matrix
inline CompressedMatrix<Type,SO>::CompressedMatrix( const SparseMatrix<MT,SO2>& sm )
   : m_       ( (~sm).rows() )              // The current number of rows of the sparse matrix
   , n_       ( (~sm).columns() )           // The current number of columns of the sparse matrix
   , capacity_( m_ )                        // The current capacity of the pointer array
   , begin_   ( new Iterator[2UL*m_+2UL] )  // Pointers to the first non-zero element of each row
   , end_     ( begin_+(m_+1UL) )           // Pointers one past the last non-zero element of each row
{
   using blaze::assign;

   const size_t nonzeros( (~sm).nonZeros() );

   begin_[0UL] = new Element[nonzeros];
   for( size_t i=0UL; i<m_; ++i )
      begin_[i+1UL] = end_[i] = begin_[0UL];
   end_[m_] = begin_[0UL]+nonzeros;

   assign( *this, ~sm );
}
//*************************************************************************************************




//=================================================================================================
//
//  DESTRUCTOR
//
//=================================================================================================

//*************************************************************************************************
/*!\brief The destructor for CompressedMatrix.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline CompressedMatrix<Type,SO>::~CompressedMatrix()
{
   delete [] begin_[0UL];
   delete [] begin_;
}
//*************************************************************************************************




//=================================================================================================
//
//  DATA ACCESS FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief 2D-access to the sparse matrix elements.
//
// \param i Access index for the row. The index has to be in the range \f$[0..M-1]\f$.
// \param j Access index for the column. The index has to be in the range \f$[0..N-1]\f$.
// \return Reference to the accessed value.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline typename CompressedMatrix<Type,SO>::Reference
   CompressedMatrix<Type,SO>::operator()( size_t i, size_t j )
{
   BLAZE_USER_ASSERT( i < rows()   , "Invalid row access index"    );
   BLAZE_USER_ASSERT( j < columns(), "Invalid column access index" );

   return Reference( *this, i, j );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief 2D-access to the sparse matrix elements.
//
// \param i Access index for the row. The index has to be in the range \f$[0..M-1]\f$.
// \param j Access index for the column. The index has to be in the range \f$[0..N-1]\f$.
// \return Reference to the accessed value.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline typename CompressedMatrix<Type,SO>::ConstReference
   CompressedMatrix<Type,SO>::operator()( size_t i, size_t j ) const
{
   BLAZE_USER_ASSERT( i < rows()   , "Invalid row access index"    );
   BLAZE_USER_ASSERT( j < columns(), "Invalid column access index" );

   const ConstIterator pos( std::lower_bound( begin_[i], end_[i], j, FindIndex() ) );

   if( pos == end_[i] || pos->index_ != j )
      return zero_;
   else
      return pos->value_;
}
//*************************************************************************************************


//*************************************************************************************************
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
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline typename CompressedMatrix<Type,SO>::Iterator
   CompressedMatrix<Type,SO>::begin( size_t i )
{
   BLAZE_USER_ASSERT( i < m_, "Invalid sparse matrix row access index" );
   return begin_[i];
}
//*************************************************************************************************


//*************************************************************************************************
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
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline typename CompressedMatrix<Type,SO>::ConstIterator
   CompressedMatrix<Type,SO>::begin( size_t i ) const
{
   BLAZE_USER_ASSERT( i < m_, "Invalid sparse matrix row access index" );
   return begin_[i];
}
//*************************************************************************************************


//*************************************************************************************************
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
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline typename CompressedMatrix<Type,SO>::Iterator
   CompressedMatrix<Type,SO>::end( size_t i )
{
   BLAZE_USER_ASSERT( i < m_, "Invalid sparse matrix row access index" );
   return end_[i];
}
//*************************************************************************************************


//*************************************************************************************************
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
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline typename CompressedMatrix<Type,SO>::ConstIterator
   CompressedMatrix<Type,SO>::end( size_t i ) const
{
   BLAZE_USER_ASSERT( i < m_, "Invalid sparse matrix row access index" );
   return end_[i];
}
//*************************************************************************************************




//=================================================================================================
//
//  ASSIGNMENT OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Copy assignment operator for CompressedMatrix.
//
// \param rhs Sparse matrix to be copied.
// \return Reference to the assigned sparse matrix.
//
// The sparse matrix is resized according to the given sparse matrix and initialized as a
// copy of this matrix.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline CompressedMatrix<Type,SO>&
   CompressedMatrix<Type,SO>::operator=( const CompressedMatrix& rhs )
{
   if( &rhs == this ) return *this;

   const size_t nonzeros( rhs.nonZeros() );

   if( rhs.m_ > capacity_ || nonzeros > capacity() )
   {
      Iterator* newBegin( new Iterator[2UL*rhs.m_+2UL] );
      Iterator* newEnd  ( newBegin+(rhs.m_+1UL) );

      newBegin[0UL] = new Element[nonzeros];
      for( size_t i=0UL; i<rhs.m_; ++i ) {
         newBegin[i+1UL] = newEnd[i] = std::copy( rhs.begin_[i], rhs.end_[i], newBegin[i] );
      }
      newEnd[rhs.m_] = newBegin[0UL]+nonzeros;

      std::swap( begin_, newBegin );
      end_ = newEnd;
      delete [] newBegin[0UL];
      delete [] newBegin;
      capacity_ = rhs.m_;
   }
   else {
     for( size_t i=0UL; i<rhs.m_; ++i ) {
         begin_[i+1UL] = end_[i] = std::copy( rhs.begin_[i], rhs.end_[i], begin_[i] );
      }
   }

   m_ = rhs.m_;
   n_ = rhs.n_;

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Assignment operator for dense matrices.
//
// \param rhs Dense matrix to be copied.
// \return Reference to the assigned matrix.
//
// The matrix is resized according to the given \f$ M \times N \f$ matrix and initialized as a
// copy of this matrix.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO2 >     // Storage order of the right-hand side dense matrix
inline CompressedMatrix<Type,SO>&
   CompressedMatrix<Type,SO>::operator=( const DenseMatrix<MT,SO2>& rhs )
{
   using blaze::assign;

   if( CanAlias<MT>::value && (~rhs).isAliased( this ) ) {
      CompressedMatrix tmp( rhs );
      swap( tmp );
   }
   else {
      resize( (~rhs).rows(), (~rhs).columns(), false );
      assign( *this, ~rhs );
   }

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Assignment operator for different sparse matrices.
//
// \param rhs Sparse matrix to be copied.
// \return Reference to the assigned matrix.
//
// The matrix is resized according to the given \f$ M \times N \f$ matrix and initialized as a
// copy of this matrix.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side sparse matrix
        , bool SO2 >     // Storage order of the right-hand side sparse matrix
inline CompressedMatrix<Type,SO>&
   CompressedMatrix<Type,SO>::operator=( const SparseMatrix<MT,SO2>& rhs )
{
   using blaze::assign;

   if( ( CanAlias<MT>::value && (~rhs).isAliased( this ) ) ||
       (~rhs).rows()     > capacity_ ||
       (~rhs).nonZeros() > capacity() ) {
      CompressedMatrix tmp( rhs );
      swap( tmp );
   }
   else {
      resize( (~rhs).rows(), (~rhs).columns(), false );
      reset();
      assign( *this, ~rhs );
   }

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Addition assignment operator for the addition of a matrix (\f$ A+=B \f$).
//
// \param rhs The right-hand side matrix to be added to the matrix.
// \return Reference to the matrix.
// \exception std::invalid_argument Matrix sizes do not match.
//
// In case the current sizes of the two matrices don't match, a \a std::invalid_argument exception
// is thrown.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side matrix
        , bool SO2 >     // Storage order of the right-hand side matrix
inline CompressedMatrix<Type,SO>&
   CompressedMatrix<Type,SO>::operator+=( const Matrix<MT,SO2>& rhs )
{
   using blaze::addAssign;

   if( (~rhs).rows() != m_ || (~rhs).columns() != n_ )
      throw std::invalid_argument( "Matrix sizes do not match" );

   addAssign( *this, ~rhs );
   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Subtraction assignment operator for the subtraction of a matrix (\f$ A-=B \f$).
//
// \param rhs The right-hand side matrix to be subtracted from the matrix.
// \return Reference to the matrix.
// \exception std::invalid_argument Matrix sizes do not match.
//
// In case the current sizes of the two matrices don't match, a \a std::invalid_argument exception
// is thrown.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side matrix
        , bool SO2 >     // Storage order of the right-hand side matrix
inline CompressedMatrix<Type,SO>& CompressedMatrix<Type,SO>::operator-=( const Matrix<MT,SO2>& rhs )
{
   using blaze::subAssign;

   if( (~rhs).rows() != m_ || (~rhs).columns() != n_ )
      throw std::invalid_argument( "Matrix sizes do not match" );

   subAssign( *this, ~rhs );
   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Multiplication assignment operator for the multiplication of a matrix (\f$ A*=B \f$).
//
// \param rhs The right-hand side matrix for the multiplication.
// \return Reference to the matrix.
// \exception std::invalid_argument Matrix sizes do not match.
//
// In case the current sizes of the two given matrices don't match, a \a std::invalid_argument
// is thrown.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side matrix
        , bool SO2 >     // Storage order of the right-hand side matrix
inline CompressedMatrix<Type,SO>&
   CompressedMatrix<Type,SO>::operator*=( const Matrix<MT,SO2>& rhs )
{
   if( (~rhs).rows() != n_ )
      throw std::invalid_argument( "Matrix sizes do not match" );

   CompressedMatrix tmp( *this * (~rhs) );
   swap( tmp );

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Multiplication assignment operator for the multiplication between a sparse matrix and
// \brief a scalar value (\f$ A*=s \f$).
//
// \param rhs The right-hand side scalar value for the multiplication.
// \return Reference to the sparse matrix.
*/
template< typename Type     // Data type of the sparse matrix
        , bool SO >         // Storage order
template< typename Other >  // Data type of the right-hand side scalar
inline typename EnableIf< IsNumeric<Other>, CompressedMatrix<Type,SO> >::Type&
   CompressedMatrix<Type,SO>::operator*=( Other rhs )
{
   for( size_t i=0UL; i<m_; ++i ) {
      const Iterator endElem( end(i) );
      for( Iterator elem=begin(i); elem<endElem; ++elem )
         elem->value_ *= rhs;
   }

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Division assignment operator for the division of a sparse matrix by a scalar value
// \brief (\f$ A/=s \f$).
//
// \param rhs The right-hand side scalar value for the division.
// \return Reference to the matrix.
*/
template< typename Type     // Data type of the sparse matrix
        , bool SO >         // Storage order
template< typename Other >  // Data type of the right-hand side scalar
inline typename EnableIf< IsNumeric<Other>, CompressedMatrix<Type,SO> >::Type&
   CompressedMatrix<Type,SO>::operator/=( Other rhs )
{
   BLAZE_USER_ASSERT( rhs != Other(0), "Division by zero detected" );

   typedef typename MathTrait<Type,Other>::DivType  DT;
   typedef typename If< IsNumeric<DT>, DT, Other >::Type  Tmp;

   // Depending on the two involved data types, an integer division is applied or a
   // floating point division is selected.
   if( IsNumeric<DT>::value && IsFloatingPoint<DT>::value ) {
      const Tmp tmp( Tmp(1)/static_cast<Tmp>( rhs ) );
      for( size_t i=0UL; i<m_; ++i ) {
         const Iterator endElem( end(i) );
         for( Iterator elem=begin(i); elem<endElem; ++elem )
            elem->value_ *= tmp;
      }
   }
   else {
      for( size_t i=0UL; i<m_; ++i ) {
         const Iterator endElem( end(i) );
         for( Iterator elem=begin(i); elem<endElem; ++elem )
            elem->value_ /= rhs;
      }
   }

   return *this;
}
//*************************************************************************************************




//=================================================================================================
//
//  UTILITY FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Returns the current number of rows of the sparse matrix.
//
// \return The number of rows of the sparse matrix.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline size_t CompressedMatrix<Type,SO>::rows() const
{
   return m_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the current number of columns of the sparse matrix.
//
// \return The number of columns of the sparse matrix.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline size_t CompressedMatrix<Type,SO>::columns() const
{
   return n_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the maximum capacity of the sparse matrix.
//
// \return The capacity of the sparse matrix.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline size_t CompressedMatrix<Type,SO>::capacity() const
{
   return end_[m_] - begin_[0UL];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the current capacity of the specified row/column.
//
// \param i The index of the row/column.
// \return The current capacity of row/column \a i.
//
// This function returns the current capacity of the specified row/column. In case the
// storage order is set to \a rowMajor the function returns the capacity of row \a i,
// in case the storage flag is set to \a columnMajor the function returns the capacity
// of column \a i.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline size_t CompressedMatrix<Type,SO>::capacity( size_t i ) const
{
   BLAZE_USER_ASSERT( i < rows(), "Invalid row access index" );
   return begin_[i+1UL] - begin_[i];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the number of non-zero elements in the sparse matrix
//
// \return The number of non-zero elements in the sparse matrix.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline size_t CompressedMatrix<Type,SO>::nonZeros() const
{
   size_t nonzeros( 0UL );

   for( size_t i=0UL; i<m_; ++i )
      nonzeros += nonZeros( i );

   return nonzeros;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the number of non-zero elements in the specified row/column.
//
// \param i The index of the row/column.
// \return The number of non-zero elements of row/column \a i.
//
// This function returns the current number of non-zero elements in the specified row/column.
// In case the storage order is set to \a rowMajor the function returns the number of non-zero
// elements in row \a i, in case the storage flag is set to \a columnMajor the function returns
// the number of non-zero elements in column \a i.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline size_t CompressedMatrix<Type,SO>::nonZeros( size_t i ) const
{
   BLAZE_USER_ASSERT( i < rows(), "Invalid row access index" );
   return end_[i] - begin_[i];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Reset to the default initial values.
//
// \return void
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline void CompressedMatrix<Type,SO>::reset()
{
   for( size_t i=0UL; i<m_; ++i )
      end_[i] = begin_[i];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Clearing the sparse matrix.
//
// \return void
//
// After the clear() function, the size of the sparse matrix is 0.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline void CompressedMatrix<Type,SO>::clear()
{
   end_[0UL] = end_[m_];
   m_ = 0UL;
   n_ = 0UL;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Inserting an element into the sparse matrix.
//
// \param i The row index of the new element. The index has to be in the range \f$[0..M-1]\f$.
// \param j The column index of the new element. The index has to be in the range \f$[0..N-1]\f$.
// \param value The value of the element to be inserted.
// \return Iterator to the newly inserted element.
// \exception std::invalid_argument Invalid sparse matrix access index.
//
// This function inserts a new element into the sparse matrix. However, duplicate elements are
// not allowed. In case the sparse matrix already contains an element with row index \a i and
// column index \a j, a \a std::invalid_argument exception is thrown.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline typename CompressedMatrix<Type,SO>::Iterator
   CompressedMatrix<Type,SO>::insert( size_t i, size_t j, const Type& value )
{
   BLAZE_USER_ASSERT( i < rows()   , "Invalid row access index"    );
   BLAZE_USER_ASSERT( j < columns(), "Invalid column access index" );

   const Iterator pos( std::lower_bound( begin_[i], end_[i], j, FindIndex() ) );

   if( pos != end_[i] && pos->index_ == j )
      throw std::invalid_argument( "Bad access index" );

   if( begin_[i+1UL] - end_[i] != 0 ) {
      std::copy_backward( pos, end_[i], end_[i]+1 );
      pos->value_ = value;
      pos->index_ = j;
      ++end_[i];

      return pos;
   }
   else if( end_[m_] - begin_[m_] != 0 ) {
      std::copy_backward( pos, end_[m_-1UL], end_[m_-1UL]+1 );

      pos->value_ = value;
      pos->index_ = j;

      for( size_t k=i+1UL; k<m_+1UL; ++k ) {
         ++begin_[k];
         ++end_[k-1UL];
      }

      return pos;
   }
   else {
      size_t newCapacity( extendCapacity() );

      Iterator* newBegin = new Iterator[2UL*capacity_+2UL];
      Iterator* newEnd   = newBegin+capacity_+1UL;

      newBegin[0UL] = new Element[newCapacity];

      for( size_t k=0UL; k<i; ++k ) {
         const size_t nonzeros( end_[k] - begin_[k] );
         const size_t total( begin_[k+1UL] - begin_[k] );
         newEnd  [k]     = newBegin[k] + nonzeros;
         newBegin[k+1UL] = newBegin[k] + total;
      }
      newEnd  [i]     = newBegin[i] + ( end_[i] - begin_[i] ) + 1;
      newBegin[i+1UL] = newBegin[i] + ( begin_[i+1] - begin_[i] ) + 1;
      for( size_t k=i+1UL; k<m_; ++k ) {
         const size_t nonzeros( end_[k] - begin_[k] );
         const size_t total( begin_[k+1UL] - begin_[k] );
         newEnd  [k]     = newBegin[k] + nonzeros;
         newBegin[k+1UL] = newBegin[k] + total;
      }

      newEnd[m_] = newEnd[capacity_] = newBegin[0UL]+newCapacity;

      Iterator tmp = std::copy( begin_[0UL], pos, newBegin[0UL] );
      tmp->value_ = value;
      tmp->index_ = j;
      std::copy( pos, end_[m_-1UL], tmp+1UL );

      std::swap( newBegin, begin_ );
      end_ = newEnd;
      delete [] newBegin[0UL];
      delete [] newBegin;

      return tmp;
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Searches for a specific matrix element.
//
// \param i The row index of the search element. The index has to be in the range \f$[0..M-1]\f$.
// \param j The column index of the search element. The index has to be in the range \f$[0..N-1]\f$.
// \return Iterator to the element in case the index is found, end() iterator otherwise.
//
// This function can be used to check whether a specific element is contained in the sparse
// matrix. It specifically searches for the element with row index \a i and column index \a j.
// In case the element is found, the function returns an row/column iterator to the element.
// Otherwise an iterator just past the last non-zero element of row \a i or column \a j (the
// end() iterator) is returned. Note that the returned sparse matrix iterator is subject to
// invalidation due to inserting operations via the subscript operator or the insert() function!
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline typename CompressedMatrix<Type,SO>::Iterator
   CompressedMatrix<Type,SO>::find( size_t i, size_t j )
{
   return const_cast<Iterator>( const_cast<const This&>( *this ).find( i, j ) );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Searches for a specific matrix element.
//
// \param i The row index of the search element. The index has to be in the range \f$[0..M-1]\f$.
// \param j The column index of the search element. The index has to be in the range \f$[0..N-1]\f$.
// \return Iterator to the element in case the index is found, end() iterator otherwise.
//
// This function can be used to check whether a specific element is contained in the sparse
// matrix. It specifically searches for the element with row index \a i and column index \a j.
// In case the element is found, the function returns an row/column iterator to the element.
// Otherwise an iterator just past the last non-zero element of row \a i or column \a j (the
// end() iterator) is returned. Note that the returned sparse matrix iterator is subject to
// invalidation due to inserting operations via the subscript operator or the insert() function!
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline typename CompressedMatrix<Type,SO>::ConstIterator
   CompressedMatrix<Type,SO>::find( size_t i, size_t j ) const
{
   const Iterator pos( std::lower_bound( begin_[i], end_[i], j, FindIndex() ) );
   if( pos != end_[i] && pos->index_ == j )
      return pos;
   else return end_[i];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Changing the size of the sparse matrix.
//
// \param m The new number of rows of the sparse matrix.
// \param n The new number of columns of the sparse matrix.
// \param preserve \a true if the old values of the matrix should be preserved, \a false if not.
// \return void
//
// This function resizes the matrix using the given size to \f$ m \times n \f$. During this
// operation, new dynamic memory may be allocated in case the capacity of the matrix is too
// small. Therefore this function potentially changes all matrix elements. In order to preserve
// the old matrix values, the \a preserve flag can be set to \a true.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
void CompressedMatrix<Type,SO>::resize( size_t m, size_t n, bool preserve )
{
   if( m == m_ && n == n_ ) return;

   if( m > capacity_ )
   {
      Iterator* newBegin( new Iterator[2*m+2] );
      Iterator* newEnd  ( newBegin+m+1 );

      newBegin[0UL] = begin_[0UL];

      if( preserve ) {
         for( size_t i=0UL; i<m_; ++i ) {
            newEnd  [i]     = end_  [i];
            newBegin[i+1UL] = begin_[i+1UL];
         }
         for( size_t i=m_; i<m; ++i ) {
            newBegin[i+1UL] = newEnd[i] = begin_[m_];
         }
      }
      else {
         for( size_t i=0UL; i<m; ++i ) {
            newBegin[i+1UL] = newEnd[i] = begin_[0UL];
         }
      }

      newEnd[m] = end_[m_];

      std::swap( newBegin, begin_ );
      delete [] newBegin;

      end_ = newEnd;
      capacity_ = m;
   }
   else if( m > m_ )
   {
      end_[m] = end_[m_];

      if( !preserve ) {
         for( size_t i=0UL; i<m_; ++i )
            end_[i] = begin_[i];
      }

      for( size_t i=m_; i<m; ++i )
         begin_[i+1UL] = end_[i] = begin_[m_];
   }
   else
   {
      if( preserve ) {
         for( size_t i=0UL; i<m; ++i )
            end_[i] = std::lower_bound( begin_[i], end_[i], n, FindIndex() );
      }
      else {
         for( size_t i=0UL; i<m; ++i )
            end_[i] = begin_[i];
      }

      end_[m] = end_[m_];
   }

   m_ = m;
   n_ = n;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Setting the minimum capacity of the sparse matrix.
//
// \param nonzeros The new minimum capacity of the sparse matrix.
// \return void
//
// This function increases the capacity of the sparse matrix to at least \a nonzeros elements.
// The current values of the matrix elements and the individual capacities of the matrix rows
// are preserved.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
void CompressedMatrix<Type,SO>::reserve( size_t nonzeros )
{
   if( nonzeros > capacity() )
      reserveElements( nonzeros );
}
//*************************************************************************************************


//*************************************************************************************************
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
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
void CompressedMatrix<Type,SO>::reserve( size_t i, size_t nonzeros )
{
   BLAZE_USER_ASSERT( i < rows(), "Invalid row access index" );

   const size_t current( capacity(i) );

   if( current >= nonzeros ) return;

   const ptrdiff_t additional( nonzeros - current );

   if( end_[m_] - begin_[m_] < additional )
   {
      const size_t newCapacity( begin_[m_] - begin_[0UL] + additional );
      BLAZE_INTERNAL_ASSERT( newCapacity > capacity(), "Invalid capacity value" );

      Iterator* newBegin( new Iterator[2UL*m_+2UL] );
      Iterator* newEnd  ( newBegin+m_+1UL );

      newBegin[0UL] = new Element[newCapacity];
      newEnd  [m_ ] = newBegin[0UL]+newCapacity;

      for( size_t k=0UL; k<i; ++k ) {
         newEnd  [k    ] = std::copy( begin_[k], end_[k], newBegin[k] );
         newBegin[k+1UL] = newBegin[k] + capacity(k);
      }
      newEnd  [i    ] = std::copy( begin_[i], end_[i], newBegin[i] );
      newBegin[i+1UL] = newBegin[i] + nonzeros;
      for( size_t k=i+1UL; k<m_; ++k ) {
         newEnd  [k    ] = std::copy( begin_[k], end_[k], newBegin[k] );
         newBegin[k+1UL] = newBegin[k] + capacity(k);
      }

      BLAZE_INTERNAL_ASSERT( newBegin[m_] == newEnd[m_], "Invalid pointer calculations" );

      std::swap( newBegin, begin_ );
      delete [] newBegin[0UL];
      delete [] newBegin;
      end_ = newEnd;
   }
   else
   {
      begin_[m_] += additional;
      for( size_t j=m_-1UL; j>i; --j ) {
         begin_[j]  = std::copy_backward( begin_[j], end_[j], end_[j]+additional );
         end_  [j] += additional;
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Transposing the matrix.
//
// \return Reference to the transposed matrix.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
CompressedMatrix<Type,SO>& CompressedMatrix<Type,SO>::transpose()
{
   CompressedMatrix tmp( trans( *this ) );
   swap( tmp );
   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checks if the matrix is diagonal.
//
// \return \a true if the matrix is diagonal, \a false if not.
//
// This function tests whether the matrix is diagonal, i.e. if the non-diagonal elements are
// default elements. In case of integral or floating point data types, a diagonal matrix has
// the form

                        \f[\left(\begin{array}{*{5}{c}}
                        aa     & 0      & 0      & \cdots & 0  \\
                        0      & bb     & 0      & \cdots & 0  \\
                        0      & 0      & cc     & \cdots & 0  \\
                        \vdots & \vdots & \vdots & \ddots & 0  \\
                        0      & 0      & 0      & 0      & mn \\
                        \end{array}\right)\f]
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
bool CompressedMatrix<Type,SO>::isDiagonal() const
{
   for( size_t i=0UL; i<rows(); ++i ) {
      for( ConstIterator element=begin_[i]; element!=end_[i]; ++element )
         if( element->index_ != i && !isDefault( element->value_ ) )
            return false;
   }

   return true;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checks if the matrix is symmetric.
//
// \return \a true if the matrix is symmetric, \a false if not.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline bool CompressedMatrix<Type,SO>::isSymmetric() const
{
   if( m_ != n_ ) return false;

   for( size_t i=0UL; i<rows(); ++i ) {
      for( ConstIterator element=begin_[i]; element!=end_[i]; ++element )
      {
         const size_t index( element->index_ );

         if( isDefault( element->value_ ) )
            continue;

         const Iterator pos( std::lower_bound( begin_[index], end_[index], i, FindIndex() ) );
         if( pos == end_[index] || pos->index_ != i || !equal( pos->value_, element->value_ ) )
            return false;
      }
   }

   return true;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Scaling of the sparse matrix by the scalar value \a scalar (\f$ A=B*s \f$).
//
// \param scalar The scalar value for the matrix scaling.
// \return Reference to the sparse matrix.
*/
template< typename Type     // Data type of the sparse matrix
        , bool SO >         // Storage order
template< typename Other >  // Data type of the scalar value
inline CompressedMatrix<Type,SO>& CompressedMatrix<Type,SO>::scale( Other scalar )
{
   for( size_t i=0UL; i<m_; ++i )
      for( Iterator element=begin_[i]; element!=end_[i]; ++element )
         element->value_ *= scalar;

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Scaling the diagonal of the sparse matrix by the scalar value \a scalar.
//
// \param scalar The scalar value for the diagonal scaling.
// \return Reference to the sparse matrix.
*/
template< typename Type     // Data type of the sparse matrix
        , bool SO >         // Storage order
template< typename Other >  // Data type of the scalar value
inline CompressedMatrix<Type,SO>& CompressedMatrix<Type,SO>::scaleDiagonal( Other scalar )
{
   const size_t size( blaze::min( m_, n_ ) );

   for( size_t i=0UL; i<size; ++i ) {
      Iterator pos = std::lower_bound( begin_[i], end_[i], i, FindIndex() );
      if( pos != end_[i] && pos->index_ == i )
         pos->value_ *= scalar;
   }

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Swapping the contents of two sparse matrices.
//
// \param sm The sparse matrix to be swapped.
// \return void
// \exception no-throw guarantee.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline void CompressedMatrix<Type,SO>::swap( CompressedMatrix& sm ) /* throw() */
{
   std::swap( m_, sm.m_ );
   std::swap( n_, sm.n_ );
   std::swap( capacity_, sm.capacity_ );
   std::swap( begin_, sm.begin_ );
   std::swap( end_  , sm.end_   );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Calculating a new matrix capacity.
//
// \return The new sparse matrix capacity.
//
// This function calculates a new matrix capacity based on the current capacity of the sparse
// matrix. Note that the new capacity is restricted to the interval \f$[7..M \cdot N]\f$.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline size_t CompressedMatrix<Type,SO>::extendCapacity() const
{
   size_t nonzeros( 2UL*capacity()+1UL );
   nonzeros = blaze::max( nonzeros, 7UL   );
   nonzeros = blaze::min( nonzeros, m_*n_ );

   BLAZE_INTERNAL_ASSERT( nonzeros > capacity(), "Invalid capacity value" );

   return nonzeros;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Reserving the specified number of sparse matrix elements.
//
// \param nonzeros The number of matrix elements to be reserved.
// \return void
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
void CompressedMatrix<Type,SO>::reserveElements( size_t nonzeros )
{
   Iterator* newBegin = new Iterator[2UL*m_+2UL];
   Iterator* newEnd   = newBegin+m_+1UL;

   newBegin[0UL] = new Element[nonzeros];

   for( size_t k=0UL; k<m_; ++k ) {
      BLAZE_INTERNAL_ASSERT( begin_[k] <= end_[k], "Invalid row pointers" );
      newEnd  [k]     = std::copy( begin_[k], end_[k], newBegin[k] );
      newBegin[k+1UL] = newBegin[k] + ( begin_[k+1UL] - begin_[k] );
   }

   newEnd[m_] = newBegin[0UL]+nonzeros;

   std::swap( newBegin, begin_ );
   delete [] newBegin[0UL];
   delete [] newBegin;
   end_ = newEnd;
}
//*************************************************************************************************




//=================================================================================================
//
//  LOW-LEVEL UTILITY FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Appending an element to the specified row/column of the sparse matrix.
//
// \param i The row index of the new element. The index has to be in the range \f$[0..M-1]\f$.
// \param j The column index of the new element. The index has to be in the range \f$[0..N-1]\f$.
// \param value The value of the element to be appended.
// \return void
//
// This function provides a very efficient way to fill a sparse matrix with elements. It appends
// a new element to the end of the specified row/column without any additional parameter verification
// or memory allocation. Therefore it is strictly necessary to keep the following preconditions
// in mind:
//
//  - the index of the new element must be strictly larger than the largest index of non-zero
//    elements in the specified row/column of the sparse matrix
//  - the current number of non-zero elements in row/column \a i must be smaller than the capacity
//    of row/column \a i.
//
// Ignoring these preconditions might result in undefined behavior!
//
// \b Note: Although append() does not allocate new memory, it still invalidates all iterators
// returned by the end() functions!
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline void CompressedMatrix<Type,SO>::append( size_t i, size_t j, const Type& value )
{
   BLAZE_USER_ASSERT( i < m_, "Invalid row access index"    );
   BLAZE_USER_ASSERT( j < n_, "Invalid column access index" );
   BLAZE_USER_ASSERT( end_[i] < end_[m_], "Not enough reserved space left" );
   BLAZE_USER_ASSERT( begin_[i] == end_[i] || j > ( end_[i]-1UL )->index_, "Index is not strictly increasing" );

   end_[i]->value_ = value;
   end_[i]->index_ = j;
   ++end_[i];
}
//*************************************************************************************************


//*************************************************************************************************
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
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline void CompressedMatrix<Type,SO>::finalize( size_t i )
{
   BLAZE_USER_ASSERT( i < m_, "Invalid row access index" );

   begin_[i+1UL] = end_[i];
   if( i != m_-1UL )
      end_[i+1UL] = end_[i];
}
//*************************************************************************************************




//=================================================================================================
//
//  EXPRESSION TEMPLATE EVALUATION FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Returns whether the matrix is aliased with the given address \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this matrix, \a false if not.
*/
template< typename Type     // Data type of the sparse matrix
        , bool SO >         // Storage order
template< typename Other >  // Data type of the foreign expression
inline bool CompressedMatrix<Type,SO>::isAliased( const Other* alias ) const
{
   return static_cast<const void*>( this ) == static_cast<const void*>( alias );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the assignment of a row-major dense matrix.
//
// \param rhs The right-hand side dense matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO2 >     // Storage order of the right-hand side dense matrix
inline void CompressedMatrix<Type,SO>::assign( const DenseMatrix<MT,SO2>& rhs )
{
   size_t nonzeros( 0UL );

   for( size_t i=1UL; i<=m_; ++i )
      begin_[i] = end_[i] = end_[m_];

   for( size_t i=0UL; i<m_; ++i )
   {
      begin_[i] = end_[i] = begin_[0UL]+nonzeros;

      for( size_t j=0UL; j<n_; ++j ) {
         if( !isDefault( (~rhs)(i,j) ) ) {
            if( nonzeros++ == capacity() ) {
               reserveElements( extendCapacity() );
               for( size_t k=i+1UL; k<=m_; ++k )
                  begin_[k] = end_[k] = end_[m_];
            }
            append( i, j, (~rhs)(i,j) );
         }
      }
   }

   begin_[m_] = begin_[0UL]+nonzeros;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the assignment of a row-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
template< typename MT >  // Type of the right-hand side sparse matrix
inline void CompressedMatrix<Type,SO>::assign( const SparseMatrix<MT,SO>& rhs )
{
   for( size_t i=0UL; i<(~rhs).rows(); ++i ) {
      begin_[i+1UL] = end_[i] = std::copy( (~rhs).begin(i), (~rhs).end(i), begin_[i] );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the assignment of a column-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
template< typename MT >  // Type of the right-hand side sparse matrix
inline void CompressedMatrix<Type,SO>::assign( const SparseMatrix<MT,!SO>& rhs )
{
   typedef typename MT::ConstIterator  RhsIterator;

   // Counting the number of elements per row
   std::vector<size_t> rowLengths( m_, 0UL );
   for( size_t j=0UL; j<n_; ++j ) {
      for( RhsIterator element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
         ++rowLengths[element->index()];
   }

   // Resizing the sparse matrix
   for( size_t i=0UL; i<m_; ++i ) {
      begin_[i+1UL] = end_[i+1UL] = begin_[i] + rowLengths[i];
   }

   // Appending the elements to the rows of the sparse matrix
   for( size_t j=0UL; j<n_; ++j ) {
      for( RhsIterator element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
         append( element->index(), j, element->value() );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the addition assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO2 >     // Storage order of the right-hand side dense matrix
inline void CompressedMatrix<Type,SO>::addAssign( const DenseMatrix<MT,SO2>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   CompressedMatrix tmp( *this + (~rhs) );
   swap( tmp );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the addition assignment of a sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side sparse matrix
        , bool SO2 >     // Storage order of the right-hand side sparse matrix
inline void CompressedMatrix<Type,SO>::addAssign( const SparseMatrix<MT,SO2>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   CompressedMatrix tmp( *this + (~rhs) );
   swap( tmp );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the subtraction assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO2 >     // Storage order of the right-hand side dense matrix
inline void CompressedMatrix<Type,SO>::subAssign( const DenseMatrix<MT,SO2>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   CompressedMatrix tmp( *this - (~rhs) );
   swap( tmp );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the subtraction assignment of a sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side sparse matrix
        , bool SO2 >     // Storage order of the right-hand sparse matrix
inline void CompressedMatrix<Type,SO>::subAssign( const SparseMatrix<MT,SO2>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   CompressedMatrix tmp( *this - (~rhs) );
   swap( tmp );
}
//*************************************************************************************************








//=================================================================================================
//
//  CLASS TEMPLATE SPECIALIZATION FOR COLUMN-MAJOR MATRICES
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Specialization of CompressedMatrix for column-major matrices.
// \ingroup compressed_matrix
//
// This specialization of CompressedMatrix adapts the class template to the requirements of
// column-major matrices.
*/
template< typename Type >  // Data type of the sparse matrix
class CompressedMatrix<Type,true> : public SparseMatrix< CompressedMatrix<Type,true>, true >
{
 private:
   //**Type definitions****************************************************************************
   typedef SparseElement<Type>  ElementBase;  //!< Base class for the sparse matrix element.
   //**********************************************************************************************

   //**Private class Element***********************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Index-value-pair for the CompressedMatrix class.
   */
   struct Element : public ElementBase
   {
      using ElementBase::operator=;
      friend class CompressedMatrix;
   };
   /*! \endcond */
   //**********************************************************************************************

   //**Private class FindIndex*********************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Helper class for the lower_bound() function.
   */
   struct FindIndex : public std::binary_function<Element,size_t,bool>
   {
      inline bool operator()( const Element& element, size_t index ) const {
         return element.index() < index;
      }
      inline bool operator()( size_t index, const Element& element ) const {
         return index < element.index();
      }
      inline bool operator()( const Element& element1, const Element& element2 ) const {
         return element1.index() < element2.index();
      }
   };
   /*! \endcond */
   //**********************************************************************************************

 public:
   //**Type definitions****************************************************************************
   typedef CompressedMatrix<Type,true>   This;            //!< Type of this CompressedMatrix instance.
   typedef This                          ResultType;      //!< Result type for expression template evaluations.
   typedef CompressedMatrix<Type,false>  OppositeType;    //!< Result type with opposite storage order for expression template evaluations.
   typedef CompressedMatrix<Type,false>  TransposeType;   //!< Transpose type for expression template evaluations.
   typedef Type                          ElementType;     //!< Type of the sparse matrix elements.
   typedef const This&                   CompositeType;   //!< Data type for composite expression templates.
   typedef MatrixAccessProxy<This>       Reference;       //!< Reference to a sparse matrix value.
   typedef const Type&                   ConstReference;  //!< Reference to a constant sparse matrix value.
   typedef Element*                      Iterator;        //!< Iterator over non-constant elements.
   typedef const Element*                ConstIterator;   //!< Iterator over constant elements.
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation flag for the detection of aliasing effects.
   /*! This compilation switch indicates whether this type potentially causes compuation errors
       due to aliasing effects. In case the type can cause aliasing effects, the \a canAlias
       switch is set to \a true, otherwise it is set to \a false. */
   enum { canAlias = 0 };
   //**********************************************************************************************

   //**Constructors********************************************************************************
   /*!\name Constructors */
   //@{
                           explicit inline CompressedMatrix();
                           explicit inline CompressedMatrix( size_t m, size_t n );
                           explicit inline CompressedMatrix( size_t m, size_t n, size_t nonzeros );
                           explicit        CompressedMatrix( size_t m, size_t n, const std::vector<size_t>& nonzeros );
                                    inline CompressedMatrix( const CompressedMatrix& sm );
   template< typename MT, bool SO > inline CompressedMatrix( const DenseMatrix<MT,SO>&  dm );
   template< typename MT, bool SO > inline CompressedMatrix( const SparseMatrix<MT,SO>& sm );
   //@}
   //**********************************************************************************************

   //**Destructor**********************************************************************************
   /*!\name Destructor */
   //@{
   inline ~CompressedMatrix();
   //@}
   //**********************************************************************************************

   //**Data access functions***********************************************************************
   /*!\name Data access functions */
   //@{
   inline Reference      operator()( size_t i, size_t j );
   inline ConstReference operator()( size_t i, size_t j ) const;
   inline Iterator       begin( size_t i );
   inline ConstIterator  begin( size_t i ) const;
   inline Iterator       end  ( size_t i );
   inline ConstIterator  end  ( size_t i ) const;
   //@}
   //**********************************************************************************************

   //**Assignment operators************************************************************************
   /*!\name Assignment operators */
   //@{
                                    inline CompressedMatrix& operator= ( const CompressedMatrix& rhs );
   template< typename MT, bool SO > inline CompressedMatrix& operator= ( const DenseMatrix<MT,SO>&  rhs );
   template< typename MT, bool SO > inline CompressedMatrix& operator= ( const SparseMatrix<MT,SO>& rhs );
   template< typename MT, bool SO > inline CompressedMatrix& operator+=( const Matrix<MT,SO>& rhs );
   template< typename MT, bool SO > inline CompressedMatrix& operator-=( const Matrix<MT,SO>& rhs );
   template< typename MT, bool SO > inline CompressedMatrix& operator*=( const Matrix<MT,SO>& rhs );

   template< typename Other >
   inline typename EnableIf< IsNumeric<Other>, CompressedMatrix >::Type&
      operator*=( Other rhs );

   template< typename Other >
   inline typename EnableIf< IsNumeric<Other>, CompressedMatrix >::Type&
      operator/=( Other rhs );
   //@}
   //**********************************************************************************************

   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
                              inline size_t            rows() const;
                              inline size_t            columns() const;
                              inline size_t            capacity() const;
                              inline size_t            capacity( size_t j ) const;
                              inline size_t            nonZeros() const;
                              inline size_t            nonZeros( size_t j ) const;
                              inline void              reset();
                              inline void              clear();
                                     Iterator          insert ( size_t i, size_t j, const Type& value );
                              inline Iterator          find   ( size_t i, size_t j );
                              inline ConstIterator     find   ( size_t i, size_t j ) const;
                                     void              resize ( size_t m, size_t n, bool preserve=true );
                              inline void              reserve( size_t nonzeros );
                                     void              reserve( size_t j, size_t nonzeros );
                              inline CompressedMatrix& transpose();
                                     bool              isDiagonal() const;
                                     bool              isSymmetric() const;
   template< typename Other > inline CompressedMatrix& scale( Other scalar );
   template< typename Other > inline CompressedMatrix& scaleDiagonal( Other scalar );
                              inline void              swap( CompressedMatrix& sm ) /* throw() */;
   //@}
   //**********************************************************************************************

   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
   inline void append  ( size_t i, size_t j, const Type& value );
   inline void finalize( size_t j );
   //@}
   //**********************************************************************************************

   //**Expression template evaluation functions****************************************************
   /*!\name Expression template evaluation functions */
   //@{
   template< typename Other > inline bool isAliased ( const Other* alias ) const;
   template< typename MT, bool SO > inline void assign   ( const DenseMatrix<MT,SO>&     rhs );
   template< typename MT >          inline void assign   ( const SparseMatrix<MT,true>&  rhs );
   template< typename MT >          inline void assign   ( const SparseMatrix<MT,false>& rhs );
   template< typename MT, bool SO > inline void addAssign( const DenseMatrix<MT,SO>&     rhs );
   template< typename MT, bool SO > inline void addAssign( const SparseMatrix<MT,SO>&    rhs );
   template< typename MT, bool SO > inline void subAssign( const DenseMatrix<MT,SO>&     rhs );
   template< typename MT, bool SO > inline void subAssign( const SparseMatrix<MT,SO>&    rhs );
   //@}
   //**********************************************************************************************

 private:
   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
   inline size_t extendCapacity() const;
   inline void   reserveElements( size_t nonzeros );
   //@}
   //**********************************************************************************************

   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   size_t m_;         //!< The current number of rows of the sparse matrix.
   size_t n_;         //!< The current number of columns of the sparse matrix.
   size_t capacity_;  //!< The current capacity of the pointer array.
   Iterator* begin_;  //!< Pointers to the first non-zero element of each column.
   Iterator* end_;    //!< Pointers one past the last non-zero element of each column.

   static const Type zero_;  //!< Neutral element for accesses to zero elements.
   //@}
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_NOT_BE_POINTER_TYPE  ( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_REFERENCE_TYPE( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_CONST         ( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_VOLATILE      ( Type );
   BLAZE_CONSTRAINT_MUST_HAVE_SAME_SIZE       ( ElementBase, Element );
   /*! \endcond */
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  DEFINITION AND INITIALIZATION OF THE STATIC MEMBER VARIABLES
//
//=================================================================================================

template< typename Type >
const Type CompressedMatrix<Type,true>::zero_ = Type();




//=================================================================================================
//
//  CONSTRUCTORS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief The default constructor for CompressedMatrix.
*/
template< typename Type >  // Data type of the sparse matrix
inline CompressedMatrix<Type,true>::CompressedMatrix()
   : m_       ( 0UL )             // The current number of rows of the sparse matrix
   , n_       ( 0UL )             // The current number of columns of the sparse matrix
   , capacity_( 0UL )             // The current capacity of the pointer array
   , begin_( new Iterator[2UL] )  // Pointers to the first non-zero element of each column
   , end_  ( begin_+1UL )         // Pointers one past the last non-zero element of each column
{
   begin_[0UL] = end_[0UL] = NULL;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Constructor for a matrix of size \f$ M \times N \f$.
//
// \param m The number of rows of the matrix.
// \param n The number of columns of the matrix.
//
// The matrix is initialized to the zero matrix and has no free capacity.
*/
template< typename Type >  // Data type of the sparse matrix
inline CompressedMatrix<Type,true>::CompressedMatrix( size_t m, size_t n )
   : m_       ( m )                     // The current number of rows of the sparse matrix
   , n_       ( n )                     // The current number of columns of the sparse matrix
   , capacity_( n )                     // The current capacity of the pointer array
   , begin_( new Iterator[2UL*n+2UL] )  // Pointers to the first non-zero element of each column
   , end_  ( begin_+(n+1UL) )           // Pointers one past the last non-zero element of each column
{
   for( size_t j=0UL; j<2UL*n_+2UL; ++j )
      begin_[j] = NULL;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Constructor for a matrix of size \f$ M \times N \f$.
//
// \param m The number of rows of the matrix.
// \param n The number of columns of the matrix.
// \param nonzeros The number of expected non-zero elements.
//
// The matrix is initialized to the zero matrix.
*/
template< typename Type >  // Data type of the sparse matrix
inline CompressedMatrix<Type,true>::CompressedMatrix( size_t m, size_t n, size_t nonzeros )
   : m_       ( m )                     // The current number of rows of the sparse matrix
   , n_       ( n )                     // The current number of columns of the sparse matrix
   , capacity_( n )                     // The current capacity of the pointer array
   , begin_( new Iterator[2UL*n+2UL] )  // Pointers to the first non-zero element of each column
   , end_  ( begin_+(n+1UL) )           // Pointers one past the last non-zero element of each column
{
   begin_[0UL] = new Element[nonzeros];
   for( size_t j=1UL; j<(2UL*n_+1UL); ++j )
      begin_[j] = begin_[0UL];
   end_[n_] = begin_[0UL]+nonzeros;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Constructor for a matrix of size \f$ M \times N \f$.
//
// \param m The number of rows of the matrix.
// \param n The number of columns of the matrix.
// \param nonzeros The expected number of non-zero elements in each column.
//
// The matrix is initialized to the zero matrix.
*/
template< typename Type >  // Data type of the sparse matrix
CompressedMatrix<Type,true>::CompressedMatrix( size_t m, size_t n, const std::vector<size_t>& nonzeros )
   : m_       ( m )                      // The current number of rows of the sparse matrix
   , n_       ( n )                      // The current number of columns of the sparse matrix
   , capacity_( n )                      // The current capacity of the pointer array
   , begin_( new Iterator[2UL*n_+2UL] )  // Pointers to the first non-zero element of each column
   , end_  ( begin_+(n_+1UL) )           // Pointers one past the last non-zero element of each column
{
   BLAZE_USER_ASSERT( nonzeros.size() == n, "Size of capacity vector and number of columns don't match" );

   size_t newCapacity( 0UL );
   for( std::vector<size_t>::const_iterator it=nonzeros.begin(); it!=nonzeros.end(); ++it )
      newCapacity += *it;

   begin_[0UL] = end_[0UL] = new Element[newCapacity];
   for( size_t j=0UL; j<n_; ++j ) {
      begin_[j+1UL] = end_[j+1UL] = begin_[j] + nonzeros[j];
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief The copy constructor for CompressedMatrix.
//
// \param sm Sparse matrix to be copied.
*/
template< typename Type >  // Data type of the sparse matrix
inline CompressedMatrix<Type,true>::CompressedMatrix( const CompressedMatrix& sm )
   : m_       ( sm.m_ )                     // The current number of rows of the sparse matrix
   , n_       ( sm.n_ )                     // The current number of columns of the sparse matrix
   , capacity_( sm.n_ )                     // The current capacity of the pointer array
   , begin_   ( new Iterator[2UL*n_+2UL] )  // Pointers to the first non-zero element of each column
   , end_     ( begin_+(n_+1UL) )           // Pointers one past the last non-zero element of each column
{
   const size_t nonzeros( sm.nonZeros() );

   begin_[0UL] = new Element[nonzeros];
   for( size_t j=0UL; j<n_; ++j )
      begin_[j+1UL] = end_[j] = std::copy( sm.begin(j), sm.end(j), begin_[j] );
   end_[n_] = begin_[0UL]+nonzeros;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Conversion constructor from dense matrices.
//
// \param dm Dense matrix to be copied.
*/
template< typename Type >  // Data type of the sparse matrix
template< typename MT      // Type of the foreign dense matrix
        , bool SO >        // Storage order of the foreign dense matrix
inline CompressedMatrix<Type,true>::CompressedMatrix( const DenseMatrix<MT,SO>& dm )
   : m_       ( (~dm).rows() )              // The current number of rows of the sparse matrix
   , n_       ( (~dm).columns() )           // The current number of columns of the sparse matrix
   , capacity_( n_ )                        // The current capacity of the pointer array
   , begin_   ( new Iterator[2UL*n_+2UL] )  // Pointers to the first non-zero element of each column
   , end_     ( begin_+(n_+1UL) )           // Pointers one past the last non-zero element of each column
{
   using blaze::assign;

   for( size_t j=0UL; j<2UL*n_+2UL; ++j )
      begin_[j] = NULL;

   assign( *this, ~dm );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Conversion constructor from different sparse matrices.
//
// \param sm Sparse matrix to be copied.
*/
template< typename Type >  // Data type of the sparse matrix
template< typename MT      // Type of the foreign sparse matrix
        , bool SO >        // Storage order of the foreign sparse matrix
inline CompressedMatrix<Type,true>::CompressedMatrix( const SparseMatrix<MT,SO>& sm )
   : m_       ( (~sm).rows() )              // The current number of rows of the sparse matrix
   , n_       ( (~sm).columns() )           // The current number of columns of the sparse matrix
   , capacity_( n_ )                        // The current capacity of the pointer array
   , begin_   ( new Iterator[2UL*n_+2UL] )  // Pointers to the first non-zero element of each column
   , end_     ( begin_+(n_+1UL) )           // Pointers one past the last non-zero element of each column
{
   using blaze::assign;

   const size_t nonzeros( (~sm).nonZeros() );

   begin_[0UL] = new Element[nonzeros];
   for( size_t j=0UL; j<n_; ++j )
      begin_[j+1UL] = end_[j] = begin_[0UL];
   end_[n_] = begin_[0UL]+nonzeros;

   assign( *this, ~sm );
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  DESTRUCTOR
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief The destructor for CompressedMatrix.
*/
template< typename Type >  // Data type of the sparse matrix
inline CompressedMatrix<Type,true>::~CompressedMatrix()
{
   delete [] begin_[0UL];
   delete [] begin_;
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  DATA ACCESS FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief 2D-access to the sparse matrix elements.
//
// \param i Access index for the row. The index has to be in the range \f$[0..M-1]\f$.
// \param j Access index for the column. The index has to be in the range \f$[0..N-1]\f$.
// \return Reference to the accessed value.
*/
template< typename Type >  // Data type of the sparse matrix
inline typename CompressedMatrix<Type,true>::Reference
   CompressedMatrix<Type,true>::operator()( size_t i, size_t j )
{
   BLAZE_USER_ASSERT( i < rows()   , "Invalid row access index"    );
   BLAZE_USER_ASSERT( j < columns(), "Invalid column access index" );

   return Reference( *this, i, j );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief 2D-access to the sparse matrix elements.
//
// \param i Access index for the row. The index has to be in the range \f$[0..M-1]\f$.
// \param j Access index for the column. The index has to be in the range \f$[0..N-1]\f$.
// \return Reference to the accessed value.
*/
template< typename Type >  // Data type of the sparse matrix
inline typename CompressedMatrix<Type,true>::ConstReference
   CompressedMatrix<Type,true>::operator()( size_t i, size_t j ) const
{
   BLAZE_USER_ASSERT( i < rows()   , "Invalid row access index"    );
   BLAZE_USER_ASSERT( j < columns(), "Invalid column access index" );

   const ConstIterator pos( std::lower_bound( begin_[j], end_[j], i, FindIndex() ) );

   if( pos == end_[j] || pos->index_ != i )
      return zero_;
   else
      return pos->value_;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator to the first non-zero element of column \a j.
//
// \param j The column index.
// \return Iterator to the first non-zero element of column \a j.
*/
template< typename Type >  // Data type of the sparse matrix
inline typename CompressedMatrix<Type,true>::Iterator
   CompressedMatrix<Type,true>::begin( size_t j )
{
   BLAZE_USER_ASSERT( j < n_, "Invalid sparse matrix column access index" );
   return begin_[j];
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator to the first non-zero element of column \a j.
//
// \param j The column index.
// \return Iterator to the first non-zero element of column \a j.
*/
template< typename Type >  // Data type of the sparse matrix
inline typename CompressedMatrix<Type,true>::ConstIterator
   CompressedMatrix<Type,true>::begin( size_t j ) const
{
   BLAZE_USER_ASSERT( j < n_, "Invalid sparse matrix column access index" );
   return begin_[j];
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator just past the last non-zero element of column \a j.
//
// \param j The column index.
// \return Iterator just past the last non-zero element of column \a j.
*/
template< typename Type >  // Data type of the sparse matrix
inline typename CompressedMatrix<Type,true>::Iterator
   CompressedMatrix<Type,true>::end( size_t j )
{
   BLAZE_USER_ASSERT( j < n_, "Invalid sparse matrix column access index" );
   return end_[j];
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator just past the last non-zero element of column \a j.
//
// \param j The column index.
// \return Iterator just past the last non-zero element of column \a j.
*/
template< typename Type >  // Data type of the sparse matrix
inline typename CompressedMatrix<Type,true>::ConstIterator
   CompressedMatrix<Type,true>::end( size_t j ) const
{
   BLAZE_USER_ASSERT( j < n_, "Invalid sparse matrix column access index" );
   return end_[j];
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ASSIGNMENT OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Copy assignment operator for CompressedMatrix.
//
// \param rhs Sparse matrix to be copied.
// \return Reference to the assigned sparse matrix.
//
// The sparse matrix is resized according to the given sparse matrix and initialized as a
// copy of this matrix.
*/
template< typename Type >  // Data type of the sparse matrix
inline CompressedMatrix<Type,true>&
   CompressedMatrix<Type,true>::operator=( const CompressedMatrix& rhs )
{
   if( &rhs == this ) return *this;

   const size_t nonzeros( rhs.nonZeros() );

   if( rhs.n_ > capacity_ || nonzeros > capacity() )
   {
      Iterator* newBegin( new Iterator[2UL*rhs.n_+2UL] );
      Iterator* newEnd  ( newBegin+(rhs.n_+1UL) );

      newBegin[0UL] = new Element[nonzeros];
      for( size_t j=0UL; j<rhs.n_; ++j ) {
         newBegin[j+1UL] = newEnd[j] = std::copy( rhs.begin_[j], rhs.end_[j], newBegin[j] );
      }
      newEnd[rhs.n_] = newBegin[0UL]+nonzeros;

      std::swap( begin_, newBegin );
      end_ = newEnd;
      delete [] newBegin[0UL];
      delete [] newBegin;
      capacity_ = rhs.n_;
   }
   else {
     for( size_t j=0UL; j<rhs.n_; ++j ) {
         begin_[j+1UL] = end_[j] = std::copy( rhs.begin_[j], rhs.end_[j], begin_[j] );
      }
   }

   m_ = rhs.m_;
   n_ = rhs.n_;

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Assignment operator for dense matrices.
//
// \param rhs Dense matrix to be copied.
// \return Reference to the assigned matrix.
//
// The matrix is resized according to the given \f$ M \times N \f$ matrix and initialized as a
// copy of this matrix.
*/
template< typename Type >  // Data type of the sparse matrix
template< typename MT      // Type of the right-hand side dense matrix
        , bool SO >        // Storage order of the right-hand side dense matrix
inline CompressedMatrix<Type,true>&
   CompressedMatrix<Type,true>::operator=( const DenseMatrix<MT,SO>& rhs )
{
   using blaze::assign;

   if( CanAlias<MT>::value && (~rhs).isAliased( this ) ) {
      CompressedMatrix tmp( rhs );
      swap( tmp );
   }
   else {
      resize( (~rhs).rows(), (~rhs).columns(), false );
      assign( *this, ~rhs );
   }

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Assignment operator for different sparse matrices.
//
// \param rhs Sparse matrix to be copied.
// \return Reference to the assigned matrix.
//
// The matrix is resized according to the given \f$ M \times N \f$ matrix and initialized as a
// copy of this matrix.
*/
template< typename Type >  // Data type of the sparse matrix
template< typename MT      // Type of the right-hand side sparse matrix
        , bool SO >        // Storage order of the right-hand side sparse matrix
inline CompressedMatrix<Type,true>&
   CompressedMatrix<Type,true>::operator=( const SparseMatrix<MT,SO>& rhs )
{
   using blaze::assign;

   if( ( CanAlias<MT>::value && (~rhs).isAliased( this ) ) ||
       (~rhs).columns()  > capacity_ ||
       (~rhs).nonZeros() > capacity() ) {
      CompressedMatrix tmp( rhs );
      swap( tmp );
   }
   else {
      resize( (~rhs).rows(), (~rhs).columns(), false );
      reset();
      assign( *this, ~rhs );
   }

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Addition assignment operator for the addition of a matrix (\f$ A+=B \f$).
//
// \param rhs The right-hand side matrix to be added to the matrix.
// \return Reference to the matrix.
// \exception std::invalid_argument Matrix sizes do not match.
//
// In case the current sizes of the two matrices don't match, a \a std::invalid_argument exception
// is thrown.
*/
template< typename Type >  // Data type of the sparse matrix
template< typename MT      // Type of the right-hand side matrix
        , bool SO >        // Storage order of the right-hand side matrix
inline CompressedMatrix<Type,true>& CompressedMatrix<Type,true>::operator+=( const Matrix<MT,SO>& rhs )
{
   using blaze::addAssign;

   if( (~rhs).rows() != m_ || (~rhs).columns() != n_ )
      throw std::invalid_argument( "Matrix sizes do not match" );

   addAssign( *this, ~rhs );
   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Subtraction assignment operator for the subtraction of a matrix (\f$ A-=B \f$).
//
// \param rhs The right-hand side matrix to be subtracted from the matrix.
// \return Reference to the matrix.
// \exception std::invalid_argument Matrix sizes do not match.
//
// In case the current sizes of the two matrices don't match, a \a std::invalid_argument exception
// is thrown.
*/
template< typename Type >  // Data type of the sparse matrix
template< typename MT      // Type of the right-hand side matrix
        , bool SO >        // Storage order of the right-hand side matrix
inline CompressedMatrix<Type,true>& CompressedMatrix<Type,true>::operator-=( const Matrix<MT,SO>& rhs )
{
   using blaze::subAssign;

   if( (~rhs).rows() != m_ || (~rhs).columns() != n_ )
      throw std::invalid_argument( "Matrix sizes do not match" );

   subAssign( *this, ~rhs );
   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Multiplication assignment operator for the multiplication of a matrix (\f$ A*=B \f$).
//
// \param rhs The right-hand side matrix for the multiplication.
// \return Reference to the matrix.
// \exception std::invalid_argument Matrix sizes do not match.
//
// In case the current sizes of the two given matrices don't match, a \a std::invalid_argument
// is thrown.
*/
template< typename Type >  // Data type of the sparse matrix
template< typename MT      // Type of the right-hand side matrix
        , bool SO >        // Storage order of the right-hand side matrix
inline CompressedMatrix<Type,true>&
   CompressedMatrix<Type,true>::operator*=( const Matrix<MT,SO>& rhs )
{
   if( (~rhs).rows() != n_ )
      throw std::invalid_argument( "Matrix sizes do not match" );

   CompressedMatrix tmp( *this * (~rhs) );
   swap( tmp );

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Multiplication assignment operator for the multiplication between a sparse matrix and
// \brief a scalar value (\f$ A*=s \f$).
//
// \param rhs The right-hand side scalar value for the multiplication.
// \return Reference to the sparse matrix.
*/
template< typename Type >   // Data type of the sparse matrix
template< typename Other >  // Data type of the right-hand side scalar
inline typename EnableIf< IsNumeric<Other>, CompressedMatrix<Type,true> >::Type&
   CompressedMatrix<Type,true>::operator*=( Other rhs )
{
   for( size_t j=0UL; j<n_; ++j ) {
      const Iterator endElem( end(j) );
      for( Iterator elem=begin(j); elem<endElem; ++elem )
         elem->value_ *= rhs;
   }

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Division assignment operator for the division of a sparse matrix by a scalar value
// \brief (\f$ A/=s \f$).
//
// \param rhs The right-hand side scalar value for the division.
// \return Reference to the matrix.
*/
template< typename Type >   // Data type of the sparse matrix
template< typename Other >  // Data type of the right-hand side scalar
inline typename EnableIf< IsNumeric<Other>, CompressedMatrix<Type,true> >::Type&
   CompressedMatrix<Type,true>::operator/=( Other rhs )
{
   BLAZE_USER_ASSERT( rhs != Other(0), "Division by zero detected" );

   typedef typename MathTrait<Type,Other>::DivType  DT;
   typedef typename If< IsNumeric<DT>, DT, Other >::Type  Tmp;

   // Depending on the two involved data types, an integer division is applied or a
   // floating point division is selected.
   if( IsNumeric<DT>::value && IsFloatingPoint<DT>::value ) {
      const Tmp tmp( Tmp(1)/static_cast<Tmp>( rhs ) );
      for( size_t j=0UL; j<n_; ++j ) {
         const Iterator endElem( end(j) );
         for( Iterator elem=begin(j); elem<endElem; ++elem )
            elem->value_ *= tmp;
      }
   }
   else {
      for( size_t j=0UL; j<n_; ++j ) {
         const Iterator endElem( end(j) );
         for( Iterator elem=begin(j); elem<endElem; ++elem )
            elem->value_ /= rhs;
      }
   }

   return *this;
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  UTILITY FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the current number of rows of the sparse matrix.
//
// \return The number of rows of the sparse matrix.
*/
template< typename Type >  // Data type of the sparse matrix
inline size_t CompressedMatrix<Type,true>::rows() const
{
   return m_;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the current number of columns of the sparse matrix.
//
// \return The number of columns of the sparse matrix.
*/
template< typename Type >  // Data type of the sparse matrix
inline size_t CompressedMatrix<Type,true>::columns() const
{
   return n_;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the maximum capacity of the sparse matrix.
//
// \return The capacity of the sparse matrix.
*/
template< typename Type >  // Data type of the sparse matrix
inline size_t CompressedMatrix<Type,true>::capacity() const
{
   return end_[n_] - begin_[0UL];
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the current capacity of the specified column.
//
// \param j The index of the column.
// \return The current capacity of column \a j.
*/
template< typename Type >  // Data type of the sparse matrix
inline size_t CompressedMatrix<Type,true>::capacity( size_t j ) const
{
   BLAZE_USER_ASSERT( j < columns(), "Invalid column access index" );
   return begin_[j+1UL] - begin_[j];
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the number of non-zero elements in the sparse matrix
//
// \return The number of non-zero elements in the sparse matrix.
*/
template< typename Type >  // Data type of the sparse matrix
inline size_t CompressedMatrix<Type,true>::nonZeros() const
{
   size_t nonzeros( 0UL );

   for( size_t j=0UL; j<n_; ++j )
      nonzeros += nonZeros( j );

   return nonzeros;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the number of non-zero elements in the specified column.
//
// \param j The index of the column.
// \return The number of non-zero elements of column \a j.
*/
template< typename Type >  // Data type of the sparse matrix
inline size_t CompressedMatrix<Type,true>::nonZeros( size_t j ) const
{
   BLAZE_USER_ASSERT( j < columns(), "Invalid column access index" );
   return end_[j] - begin_[j];
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Reset to the default initial values.
//
// \return void
*/
template< typename Type >  // Data type of the sparse matrix
inline void CompressedMatrix<Type,true>::reset()
{
   for( size_t j=0UL; j<n_; ++j )
      end_[j] = begin_[j];
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Clearing the sparse matrix.
//
// \return void
//
// After the clear() function, the size of the sparse matrix is 0.
*/
template< typename Type >  // Data type of the sparse matrix
inline void CompressedMatrix<Type,true>::clear()
{
   end_[0UL] = end_[n_];
   m_ = 0UL;
   n_ = 0UL;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Inserting an element into the sparse matrix.
//
// \param i The row index of the new element. The index has to be in the range \f$[0..M-1]\f$.
// \param j The column index of the new element. The index has to be in the range \f$[0..N-1]\f$.
// \param value The value of the element to be inserted.
// \return Iterator to the newly inserted element.
// \exception std::invalid_argument Invalid sparse matrix access index.
//
// This function inserts a new element into the sparse matrix. However, duplicate elements are
// not allowed. In case the sparse matrix already contains an element with row index \a i and
// column index \a j, a \a std::invalid_argument exception is thrown.
*/
template< typename Type >  // Data type of the sparse matrix
inline typename CompressedMatrix<Type,true>::Iterator
   CompressedMatrix<Type,true>::insert( size_t i, size_t j, const Type& value )
{
   BLAZE_USER_ASSERT( i < rows()   , "Invalid row access index"    );
   BLAZE_USER_ASSERT( j < columns(), "Invalid column access index" );

   const Iterator pos( std::lower_bound( begin_[j], end_[j], i, FindIndex() ) );

   if( pos != end_[j] && pos->index_ == i )
      throw std::invalid_argument( "Bad access index" );

   if( begin_[j+1UL] - end_[j] != 0 ) {
      std::copy_backward( pos, end_[j], end_[j]+1 );
      pos->value_ = value;
      pos->index_ = i;
      ++end_[j];

      return pos;
   }
   else if( end_[n_] - begin_[n_] != 0 ) {
      std::copy_backward( pos, end_[n_-1UL], end_[n_-1]+1 );

      pos->value_ = value;
      pos->index_ = i;

      for( size_t k=j+1UL; k<n_+1UL; ++k ) {
         ++begin_[k];
         ++end_[k-1UL];
      }

      return pos;
   }
   else {
      size_t newCapacity( extendCapacity() );

      Iterator* newBegin = new Iterator[2UL*capacity_+2UL];
      Iterator* newEnd   = newBegin+capacity_+1UL;

      newBegin[0UL] = new Element[newCapacity];

      for( size_t k=0UL; k<j; ++k ) {
         const size_t nonzeros( end_[k] - begin_[k] );
         const size_t total( begin_[k+1UL] - begin_[k] );
         newEnd  [k]     = newBegin[k] + nonzeros;
         newBegin[k+1UL] = newBegin[k] + total;
      }
      newEnd  [j]     = newBegin[j] + ( end_[j] - begin_[j] ) + 1;
      newBegin[j+1UL] = newBegin[j] + ( begin_[j+1UL] - begin_[j] ) + 1;
      for( size_t k=j+1UL; k<n_; ++k ) {
         const size_t nonzeros( end_[k] - begin_[k] );
         const size_t total( begin_[k+1UL] - begin_[k] );
         newEnd  [k]     = newBegin[k] + nonzeros;
         newBegin[k+1UL] = newBegin[k] + total;
      }

      newEnd[n_] = newEnd[capacity_] = newBegin[0UL]+newCapacity;

      Iterator tmp = std::copy( begin_[0UL], pos, newBegin[0UL] );
      tmp->value_ = value;
      tmp->index_ = i;
      std::copy( pos, end_[n_-1UL], tmp+1UL );

      std::swap( newBegin, begin_ );
      end_ = newEnd;
      delete [] newBegin[0UL];
      delete [] newBegin;

      return tmp;
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Searches for a specific matrix element.
//
// \param i The row index of the search element. The index has to be in the range \f$[0..M-1]\f$.
// \param j The column index of the search element. The index has to be in the range \f$[0..N-1]\f$.
// \return Iterator to the element in case the index is found, end() iterator otherwise.
//
// This function can be used to check whether a specific element is contained in the sparse
// matrix. It specifically searches for the element with row index \a i and column index \a j.
// In case the element is found, the function returns an iterator to the element. Otherwise an
// iterator just past the last non-zero element of column \a j (the end() iterator) is returned.
// Note that the returned sparse matrix iterator is subject to invalidation due to inserting
// operations via the subscript operator or the insert() function!
*/
template< typename Type >  // Data type of the sparse matrix
inline typename CompressedMatrix<Type,true>::Iterator
   CompressedMatrix<Type,true>::find( size_t i, size_t j )
{
   return const_cast<Iterator>( const_cast<const This&>( *this ).find( i, j ) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Searches for a specific matrix element.
//
// \param i The row index of the search element. The index has to be in the range \f$[0..M-1]\f$.
// \param j The column index of the search element. The index has to be in the range \f$[0..N-1]\f$.
// \return Iterator to the element in case the index is found, end() iterator otherwise.
//
// This function can be used to check whether a specific element is contained in the sparse
// matrix. It specifically searches for the element with row index \a i and column index \a j.
// In case the element is found, the function returns an iterator to the element. Otherwise an
// iterator just past the last non-zero element of column \a j (the end() iterator) is returned.
// Note that the returned sparse matrix iterator is subject to invalidation due to inserting
// operations via the subscript operator or the insert() function!
*/
template< typename Type >  // Data type of the sparse matrix
inline typename CompressedMatrix<Type,true>::ConstIterator
   CompressedMatrix<Type,true>::find( size_t i, size_t j ) const
{
   const Iterator pos( std::lower_bound( begin_[j], end_[j], i, FindIndex() ) );
   if( pos != end_[j] && pos->index_ == i )
      return pos;
   else return end_[j];
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Changing the size of the sparse matrix.
//
// \param m The new number of rows of the sparse matrix.
// \param n The new number of columns of the sparse matrix.
// \param preserve \a true if the old values of the matrix should be preserved, \a false if not.
// \return void
//
// This function resizes the matrix using the given size to \f$ m \times n \f$. During this
// operation, new dynamic memory may be allocated in case the capacity of the matrix is too
// small. Therefore this function potentially changes all matrix elements. In order to preserve
// the old matrix values, the \a preserve flag can be set to \a true.
*/
template< typename Type >  // Data type of the sparse matrix
void CompressedMatrix<Type,true>::resize( size_t m, size_t n, bool preserve )
{
   if( m == m_ && n == n_ ) return;

   if( n > capacity_ )
   {
      Iterator* newBegin( new Iterator[2UL*n+2UL] );
      Iterator* newEnd  ( newBegin+n+1 );

      newBegin[0UL] = begin_[0UL];

      if( preserve ) {
         for( size_t j=0UL; j<n_; ++j ) {
            newEnd  [j]     = end_  [j];
            newBegin[j+1UL] = begin_[j+1UL];
         }
         for( size_t j=n_; j<n; ++j ) {
            newBegin[j+1UL] = newEnd[j] = begin_[n_];
         }
      }
      else {
         for( size_t j=0UL; j<n; ++j ) {
            newBegin[j+1UL] = newEnd[j] = begin_[0UL];
         }
      }

      newEnd[n] = end_[n_];

      std::swap( newBegin, begin_ );
      delete [] newBegin;

      end_ = newEnd;
      capacity_ = n;
   }
   else if( n > n_ )
   {
      end_[n] = end_[n_];

      if( !preserve ) {
         for( size_t j=0UL; j<n_; ++j )
            end_[j] = begin_[j];
      }

      for( size_t j=n_; j<n; ++j )
         begin_[j+1UL] = end_[j] = begin_[n_];
   }
   else
   {
      if( preserve ) {
         for( size_t j=0UL; j<n; ++j )
            end_[j] = std::lower_bound( begin_[j], end_[j], m, FindIndex() );
      }
      else {
         for( size_t j=0UL; j<n; ++j )
            end_[j] = begin_[j];
      }

      end_[n] = end_[n_];
   }

   m_ = m;
   n_ = n;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Setting the minimum capacity of the sparse matrix.
//
// \param nonzeros The new minimum capacity of the sparse matrix.
// \return void
//
// This function increases the capacity of the sparse matrix to at least \a nonzeros elements.
// The current values of the matrix elements and the individual capacities of the matrix rows
// are preserved.
*/
template< typename Type >  // Data type of the sparse matrix
void CompressedMatrix<Type,true>::reserve( size_t nonzeros )
{
   if( nonzeros > capacity() )
      reserveElements( nonzeros );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Setting the minimum capacity of a specific column of the sparse matrix.
//
// \param j The column index. The index has to be in the range \f$[0..M-1]\f$.
// \param nonzeros The new minimum capacity of the specified column.
// \return void
//
// This function increases the capacity of column \a j of the sparse matrix to at least \a nonzeros
// elements. The current values of the sparse matrix and all other individual column capacities are
// preserved.
*/
template< typename Type >  // Data type of the sparse matrix
void CompressedMatrix<Type,true>::reserve( size_t j, size_t nonzeros )
{
   BLAZE_USER_ASSERT( j < columns(), "Invalid column access index" );

   const size_t current( capacity(j) );

   if( current >= nonzeros ) return;

   const ptrdiff_t additional( nonzeros - current );

   if( end_[n_] - begin_[n_] < additional )
   {
      const size_t newCapacity( begin_[n_] - begin_[0UL] + additional );
      BLAZE_INTERNAL_ASSERT( newCapacity > capacity(), "Invalid capacity value" );

      Iterator* newBegin( new Iterator[2UL*n_+2UL] );
      Iterator* newEnd  ( newBegin+n_+1UL );

      newBegin[0UL] = new Element[newCapacity];
      newEnd  [n_ ] = newBegin[0UL]+newCapacity;

      for( size_t k=0UL; k<j; ++k ) {
         newEnd  [k    ] = std::copy( begin_[k], end_[k], newBegin[k] );
         newBegin[k+1UL] = newBegin[k] + capacity(k);
      }
      newEnd  [j    ] = std::copy( begin_[j], end_[j], newBegin[j] );
      newBegin[j+1UL] = newBegin[j] + nonzeros;
      for( size_t k=j+1UL; k<n_; ++k ) {
         newEnd  [k    ] = std::copy( begin_[k], end_[k], newBegin[k] );
         newBegin[k+1UL] = newBegin[k] + capacity(k);
      }

      BLAZE_INTERNAL_ASSERT( newBegin[n_] == newEnd[n_], "Invalid pointer calculations" );

      std::swap( newBegin, begin_ );
      delete [] newBegin[0UL];
      delete [] newBegin;
      end_ = newEnd;
   }
   else
   {
      begin_[n_] += additional;
      for( size_t k=n_-1UL; k>j; --k ) {
         begin_[k]  = std::copy_backward( begin_[k], end_[k], end_[k]+additional );
         end_  [k] += additional;
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Transposing the matrix.
//
// \return Reference to the transposed matrix.
*/
template< typename Type >  // Data type of the sparse matrix
CompressedMatrix<Type,true>& CompressedMatrix<Type,true>::transpose()
{
   CompressedMatrix tmp( trans( *this ) );
   swap( tmp );
   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Checks if the matrix is diagonal.
//
// \return \a true if the matrix is diagonal, \a false if not.
//
// This function tests whether the matrix is diagonal, i.e. if the non-diagonal elements are
// default elements. In case of integral or floating point data types, a diagonal matrix has
// the form

                        \f[\left(\begin{array}{*{5}{c}}
                        aa     & 0      & 0      & \cdots & 0  \\
                        0      & bb     & 0      & \cdots & 0  \\
                        0      & 0      & cc     & \cdots & 0  \\
                        \vdots & \vdots & \vdots & \ddots & 0  \\
                        0      & 0      & 0      & 0      & mn \\
                        \end{array}\right)\f]
*/
template< typename Type >  // Data type of the sparse matrix
bool CompressedMatrix<Type,true>::isDiagonal() const
{
   for( size_t j=0UL; j<columns(); ++j ) {
      for( ConstIterator element=begin_[j]; element!=end_[j]; ++element )
         if( element->index_ != j && !isDefault( element->value_ ) )
            return false;
   }

   return true;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Checks if the matrix is symmetric.
//
// \return \a true if the matrix is symmetric, \a false if not.
*/
template< typename Type >  // Data type of the sparse matrix
inline bool CompressedMatrix<Type,true>::isSymmetric() const
{
   if( m_ != n_ ) return false;

   for( size_t j=0UL; j<rows(); ++j ) {
      for( ConstIterator element=begin_[j]; element!=end_[j]; ++element )
      {
         const size_t index( element->index_ );

         if( isDefault( element->value_ ) )
            continue;

         const Iterator pos( std::lower_bound( begin_[index], end_[index], j, FindIndex() ) );
         if( pos == end_[index] || pos->index_ != j || !equal( pos->value_, element->value_ ) )
            return false;
      }
   }

   return true;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Scaling of the sparse matrix by the scalar value \a scalar (\f$ A=B*s \f$).
//
// \param scalar The scalar value for the matrix scaling.
// \return Reference to the sparse matrix.
*/
template< typename Type >   // Data type of the sparse matrix
template< typename Other >  // Data type of the scalar value
inline CompressedMatrix<Type,true>& CompressedMatrix<Type,true>::scale( Other scalar )
{
   for( size_t j=0UL; j<n_; ++j )
      for( Iterator element=begin_[j]; element!=end_[j]; ++element )
         element->value_ *= scalar;

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Scaling the diagonal of the sparse matrix by the scalar value \a scalar.
//
// \param scalar The scalar value for the diagonal scaling.
// \return Reference to the sparse matrix.
*/
template< typename Type >   // Data type of the sparse matrix
template< typename Other >  // Data type of the scalar value
inline CompressedMatrix<Type,true>& CompressedMatrix<Type,true>::scaleDiagonal( Other scalar )
{
   const size_t size( blaze::min( m_, n_ ) );

   for( size_t j=0UL; j<size; ++j ) {
      Iterator pos = std::lower_bound( begin_[j], end_[j], j, FindIndex() );
      if( pos != end_[j] && pos->index_ == j )
         pos->value_ *= scalar;
   }

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Swapping the contents of two sparse matrices.
//
// \param sm The sparse matrix to be swapped.
// \return void
// \exception no-throw guarantee.
*/
template< typename Type >  // Data type of the sparse matrix
inline void CompressedMatrix<Type,true>::swap( CompressedMatrix& sm ) /* throw() */
{
   std::swap( m_, sm.m_ );
   std::swap( n_, sm.n_ );
   std::swap( capacity_, sm.capacity_ );
   std::swap( begin_, sm.begin_ );
   std::swap( end_  , sm.end_   );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Calculating a new matrix capacity.
//
// \return The new sparse matrix capacity.
//
// This function calculates a new matrix capacity based on the current capacity of the sparse
// matrix. Note that the new capacity is restricted to the interval \f$[7..M \cdot N]\f$.
*/
template< typename Type >  // Data type of the sparse matrix
inline size_t CompressedMatrix<Type,true>::extendCapacity() const
{
   size_t nonzeros( 2UL*capacity()+1UL );
   nonzeros = blaze::max( nonzeros, 7UL );
   nonzeros = blaze::min( nonzeros, m_*n_ );

   BLAZE_INTERNAL_ASSERT( nonzeros > capacity(), "Invalid capacity value" );

   return nonzeros;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Reserving the specified number of sparse matrix elements.
//
// \param nonzeros The number of matrix elements to be reserved.
// \return void
*/
template< typename Type >  // Data type of the sparse matrix
void CompressedMatrix<Type,true>::reserveElements( size_t nonzeros )
{
   Iterator* newBegin = new Iterator[2UL*n_+2UL];
   Iterator* newEnd   = newBegin+n_+1UL;

   newBegin[0UL] = new Element[nonzeros];

   for( size_t k=0UL; k<n_; ++k ) {
      BLAZE_INTERNAL_ASSERT( begin_[k] <= end_[k], "Invalid column pointers" );
      newEnd  [k]     = std::copy( begin_[k], end_[k], newBegin[k] );
      newBegin[k+1UL] = newBegin[k] + ( begin_[k+1UL] - begin_[k] );
   }

   newEnd[n_] = newBegin[0UL]+nonzeros;

   std::swap( newBegin, begin_ );
   delete [] newBegin[0UL];
   delete [] newBegin;
   end_ = newEnd;
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  LOW-LEVEL UTILITY FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Appending an element to the specified column of the sparse matrix.
//
// \param i The row index of the new element. The index has to be in the range \f$[0..M-1]\f$.
// \param j The column index of the new element. The index has to be in the range \f$[0..N-1]\f$.
// \param value The value of the element to be appended.
// \return void
//
// This function provides a very efficient way to fill a sparse matrix with elements. It appends
// a new element to the end of the specified column without any additional parameter verification
// or memory allocation. Therefore it is strictly necessary to keep the following preconditions
// in mind:
//
//  - the index of the new element must be strictly larger than the largest index of non-zero
//    elements in the specified column of the sparse matrix
//  - the current number of non-zero elements in column \a j must be smaller than the capacity of
//    column \a j.
//
// Ignoring these preconditions might result in undefined behavior!
//
// \b Note: Although append() does not allocate new memory, it still invalidates all iterators
// returned by the end() functions!
*/
template< typename Type >  // Data type of the sparse matrix
inline void CompressedMatrix<Type,true>::append( size_t i, size_t j, const Type& value )
{
   BLAZE_USER_ASSERT( i < m_, "Invalid row access index"    );
   BLAZE_USER_ASSERT( j < n_, "Invalid column access index" );
   BLAZE_USER_ASSERT( end_[j] < end_[n_], "Not enough reserved space left" );
   BLAZE_USER_ASSERT( begin_[j] == end_[j] || i > ( end_[j]-1UL )->index_, "Index is not strictly increasing" );

   end_[j]->value_ = value;
   end_[j]->index_ = i;
   ++end_[j];
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Finalizing the element insertion of a column.
//
// \param j The index of the column to be finalized \f$[0..M-1]\f$.
// \return void
//
// This function is part of the low-level interface to efficiently fill the matrix with elements.
// After completion of column \a j via the append() function, this function can be called to
// finalize column \a j and prepare the next column for insertion process via append().
//
// \b Note: Although finalize() does not allocate new memory, it still invalidates all iterators
// returned by the end() functions!
*/
template< typename Type >  // Data type of the sparse matrix
inline void CompressedMatrix<Type,true>::finalize( size_t j )
{
   BLAZE_USER_ASSERT( j < n_, "Invalid row access index" );

   begin_[j+1UL] = end_[j];
   if( j != n_-1UL )
      end_[j+1UL] = end_[j];
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  EXPRESSION TEMPLATE EVALUATION FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the matrix is aliased with the given address \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this matrix, \a false if not.
*/
template< typename Type >   // Data type of the sparse matrix
template< typename Other >  // Data type of the foreign expression
inline bool CompressedMatrix<Type,true>::isAliased( const Other* alias ) const
{
   return static_cast<const void*>( this ) == static_cast<const void*>( alias );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type >  // Data type of the sparse matrix
template< typename MT      // Type of the right-hand side dense matrix
        , bool SO >        // Storage order of the right-hand side dense matrix
inline void CompressedMatrix<Type,true>::assign( const DenseMatrix<MT,SO>& rhs )
{
   size_t nonzeros( 0UL );

   for( size_t j=1UL; j<=n_; ++j )
      begin_[j] = end_[j] = end_[n_];

   for( size_t j=0UL; j<n_; ++j )
   {
      begin_[j] = end_[j] = begin_[0UL]+nonzeros;

      for( size_t i=0UL; i<m_; ++i ) {
         if( !isDefault( (~rhs)(i,j) ) ) {
            if( nonzeros++ == capacity() ) {
               reserveElements( extendCapacity() );
               for( size_t k=j+1UL; k<=n_; ++k )
                  begin_[k] = end_[k] = end_[n_];
            }
            append( i, j, (~rhs)(i,j) );
         }
      }
   }

   begin_[n_] = begin_[0UL]+nonzeros;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the assignment of a column-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type >  // Data type of the sparse matrix
template< typename MT >    // Type of the right-hand side sparse matrix
inline void CompressedMatrix<Type,true>::assign( const SparseMatrix<MT,true>& rhs )
{
   for( size_t j=0UL; j<(~rhs).columns(); ++j ) {
      begin_[j+1UL] = end_[j] = std::copy( (~rhs).begin(j), (~rhs).end(j), begin_[j] );
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the assignment of a row-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type >  // Data type of the sparse matrix
template< typename MT >    // Type of the right-hand side sparse matrix
inline void CompressedMatrix<Type,true>::assign( const SparseMatrix<MT,false>& rhs )
{
   typedef typename MT::ConstIterator  RhsIterator;

   // Counting the number of elements per column
   std::vector<size_t> columnLengths( n_, 0UL );
   for( size_t i=0UL; i<m_; ++i ) {
      for( RhsIterator element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
         ++columnLengths[element->index()];
   }

   // Resizing the sparse matrix
   for( size_t j=0UL; j<n_; ++j ) {
      begin_[j+1UL] = end_[j+1UL] = begin_[j] + columnLengths[j];
   }

   // Appending the elements to the columns of the sparse matrix
   for( size_t i=0UL; i<m_; ++i ) {
      for( RhsIterator element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
         append( i, element->index(), element->value() );
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the addition assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type >  // Data type of the sparse matrix
template< typename MT      // Type of the right-hand side dense matrix
        , bool SO >        // Storage order of the right-hand side dense matrix
inline void CompressedMatrix<Type,true>::addAssign( const DenseMatrix<MT,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   CompressedMatrix tmp( *this + (~rhs) );
   swap( tmp );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the addition assignment of a sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type >  // Data type of the sparse matrix
template< typename MT      // Type of the right-hand side sparse matrix
        , bool SO >        // Storage order of the right-hand side sparse matrix
inline void CompressedMatrix<Type,true>::addAssign( const SparseMatrix<MT,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   CompressedMatrix tmp( *this + (~rhs) );
   swap( tmp );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the subtraction assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type >  // Data type of the sparse matrix
template< typename MT      // Type of the right-hand side dense matrix
        , bool SO >        // Storage order of the right-hand side dense matrix
inline void CompressedMatrix<Type,true>::subAssign( const DenseMatrix<MT,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   CompressedMatrix tmp( *this - (~rhs) );
   swap( tmp );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the subtraction assignment of a sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type >  // Data type of the sparse matrix
template< typename MT      // Type of the right-hand side sparse matrix
        , bool SO >        // Storage order of the right-hand side sparse matrix
inline void CompressedMatrix<Type,true>::subAssign( const SparseMatrix<MT,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   CompressedMatrix tmp( *this - (~rhs) );
   swap( tmp );
}
/*! \endcond */
//*************************************************************************************************








//=================================================================================================
//
//  GLOBAL OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*!\name CompressedMatrix operators */
//@{
template< typename Type, bool SO >
inline bool isnan( const CompressedMatrix<Type,SO>& m );

template< typename Type, bool SO >
inline void reset( CompressedMatrix<Type,SO>& m );

template< typename Type, bool SO >
inline void clear( CompressedMatrix<Type,SO>& m );

template< typename Type, bool SO >
inline bool isDefault( const CompressedMatrix<Type,SO>& m );

template< typename Type, bool SO >
inline const CompressedMatrix<Type,SO> inv( const CompressedMatrix<Type,SO>& m );

template< typename Type, bool SO >
inline const SMatSMatMultExpr< CompressedMatrix<Type,SO>, CompressedMatrix<Type,SO> >
   sq( const CompressedMatrix<Type,SO>& m );

template< typename Type, bool SO >
inline void swap( CompressedMatrix<Type,SO>& a, CompressedMatrix<Type,SO>& b ) /* throw() */;
//@}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checks the given sparse matrix for not-a-number elements.
// \ingroup compressed_matrix
//
// \param m The sparse matrix to be checked for not-a-number elements.
// \return \a true if at least one element of the sparse matrix is not-a-number, \a false otherwise.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline bool isnan( const CompressedMatrix<Type,SO>& m )
{
   typedef typename CompressedMatrix<Type,SO>::ConstIterator  ConstIterator;

   for( size_t i=0UL; i<m.rows(); ++i ) {
      for( ConstIterator element=m.begin(i); element!=m.end(i); ++element )
         if( isnan( *element ) ) return true;
   }
   return false;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Resetting the given sparse matrix.
// \ingroup compressed_matrix
//
// \param m The sparse matrix to be resetted.
// \return void
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline void reset( CompressedMatrix<Type,SO>& m )
{
   m.reset();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Clearing the given sparse matrix.
// \ingroup compressed_matrix
//
// \param m The sparse matrix to be cleared.
// \return void
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline void clear( CompressedMatrix<Type,SO>& m )
{
   m.clear();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns whether the given sparse matrix is in default state.
// \ingroup compressed_matrix
//
// \param m The sparse matrix to be tested for its default state.
// \return \a true in case the given matrix is component-wise zero, \a false otherwise.
//
// This function checks whether the compressed matrix is in default state. For instance, in
// case the matrix is instantiated for a built-in integral or floating point data type, the
// function returns \a true in case all matrix elements are 0 and \a false in case any matrix
// element is not 0. The following example demonstrates the use of the \a isDefault function:

   \code
   blaze::CompressedMatrix<int> A;
   // ... Resizing and initialization
   if( isDefault( A ) ) { ... }
   \endcode
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline bool isDefault( const CompressedMatrix<Type,SO>& m )
{
   typedef typename CompressedMatrix<Type,SO>::ConstIterator  ConstIterator;

   if( SO == rowMajor ) {
      for( size_t i=0UL; i<m.rows(); ++i ) {
         for( ConstIterator element=m.begin(i); element!=m.end(i); ++element )
            if( !isDefault( element->value() ) ) return false;
      }
   }
   else {
      for( size_t j=0UL; j<m.columns(); ++j ) {
         for( ConstIterator element=m.begin(j); element!=m.end(j); ++element )
            if( !isDefault( element->value() ) ) return false;
      }
   }

   return true;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Inverting the given sparse matrix.
// \ingroup compressed_matrix
//
// \param m The sparse matrix to be inverted.
// \return The inverse of the matrix.
//
// This function returns the inverse of the given sparse matrix.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline const CompressedMatrix<Type,SO> inv( const CompressedMatrix<Type,SO>& m )
{
   return CompressedMatrix<Type,SO>();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Squaring the given sparse matrix.
// \ingroup compressed_matrix
//
// \param m The sparse matrix to be squared.
// \return The result of the square operation.
//
// This function squares the given sparse matrix \a m. This function has the same effect as
// multiplying the matrix with itself (\f$ m * m \f$).
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline const SMatSMatMultExpr< CompressedMatrix<Type,SO>, CompressedMatrix<Type,SO> >
   sq( const CompressedMatrix<Type,SO>& m )
{
   return m * m;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Swapping the contents of two sparse matrices.
// \ingroup compressed_matrix
//
// \param a The first sparse matrix to be swapped.
// \param b The second sparse matrix to be swapped.
// \return void
// \exception no-throw guarantee.
*/
template< typename Type  // Data type of the sparse matrix
        , bool SO >      // Storage order
inline void swap( CompressedMatrix<Type,SO>& a, CompressedMatrix<Type,SO>& b ) /* throw() */
{
   a.swap( b );
}
//*************************************************************************************************




//=================================================================================================
//
//  ISRESIZABLE SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T, bool SO >
struct IsResizable< CompressedMatrix<T,SO> > : public TrueType
{
   enum { value = 1 };
   typedef TrueType  Type;
};

template< typename T, bool SO >
struct IsResizable< const CompressedMatrix<T,SO> > : public TrueType
{
   enum { value = 1 };
   typedef TrueType  Type;
};

template< typename T, bool SO >
struct IsResizable< volatile CompressedMatrix<T,SO> > : public TrueType
{
   enum { value = 1 };
   typedef TrueType  Type;
};

template< typename T, bool SO >
struct IsResizable< const volatile CompressedMatrix<T,SO> > : public TrueType
{
   enum { value = 1 };
   typedef TrueType  Type;
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  MATHTRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T1, bool SO, typename T2 >
struct MathTrait< CompressedMatrix<T1,SO>, T2 >
{
   typedef INVALID_TYPE                                                 HighType;
   typedef INVALID_TYPE                                                 LowType;
   typedef INVALID_TYPE                                                 AddType;
   typedef INVALID_TYPE                                                 SubType;
   typedef CompressedMatrix< typename MathTrait<T1,T2>::MultType, SO >  MultType;
   typedef CompressedMatrix< typename MathTrait<T1,T2>::DivType , SO >  DivType;
   BLAZE_CONSTRAINT_MUST_BE_NUMERIC_TYPE( T2 );
};

template< typename T1, typename T2, bool SO >
struct MathTrait< T1, CompressedMatrix<T2,SO> >
{
   typedef INVALID_TYPE                                                 HighType;
   typedef INVALID_TYPE                                                 LowType;
   typedef INVALID_TYPE                                                 AddType;
   typedef INVALID_TYPE                                                 SubType;
   typedef CompressedMatrix< typename MathTrait<T1,T2>::MultType, SO >  MultType;
   typedef INVALID_TYPE                                                 DivType;
   BLAZE_CONSTRAINT_MUST_BE_NUMERIC_TYPE( T1 );
};

template< typename T1, bool SO, typename T2, size_t N >
struct MathTrait< CompressedMatrix<T1,SO>, StaticVector<T2,N,false> >
{
   typedef INVALID_TYPE                                                 HighType;
   typedef INVALID_TYPE                                                 LowType;
   typedef INVALID_TYPE                                                 AddType;
   typedef INVALID_TYPE                                                 SubType;
   typedef DynamicVector< typename MathTrait<T1,T2>::MultType, false >  MultType;
   typedef INVALID_TYPE                                                 DivType;
};

template< typename T1, size_t N, typename T2, bool SO >
struct MathTrait< StaticVector<T1,N,true>, CompressedMatrix<T2,SO> >
{
   typedef INVALID_TYPE                                                HighType;
   typedef INVALID_TYPE                                                LowType;
   typedef INVALID_TYPE                                                AddType;
   typedef INVALID_TYPE                                                SubType;
   typedef DynamicVector< typename MathTrait<T1,T2>::MultType, true >  MultType;
   typedef INVALID_TYPE                                                DivType;
};

template< typename T1, bool SO, typename T2 >
struct MathTrait< CompressedMatrix<T1,SO>, DynamicVector<T2,false> >
{
   typedef INVALID_TYPE                                                 HighType;
   typedef INVALID_TYPE                                                 LowType;
   typedef INVALID_TYPE                                                 AddType;
   typedef INVALID_TYPE                                                 SubType;
   typedef DynamicVector< typename MathTrait<T1,T2>::MultType, false >  MultType;
   typedef INVALID_TYPE                                                 DivType;
};

template< typename T1, typename T2, bool SO >
struct MathTrait< DynamicVector<T1,true>, CompressedMatrix<T2,SO> >
{
   typedef INVALID_TYPE                                                HighType;
   typedef INVALID_TYPE                                                LowType;
   typedef INVALID_TYPE                                                AddType;
   typedef INVALID_TYPE                                                SubType;
   typedef DynamicVector< typename MathTrait<T1,T2>::MultType, true >  MultType;
   typedef INVALID_TYPE                                                DivType;
};

template< typename T1, bool SO, typename T2 >
struct MathTrait< CompressedMatrix<T1,SO>, CompressedVector<T2,false> >
{
   typedef INVALID_TYPE                                                    HighType;
   typedef INVALID_TYPE                                                    LowType;
   typedef INVALID_TYPE                                                    AddType;
   typedef INVALID_TYPE                                                    SubType;
   typedef CompressedVector< typename MathTrait<T1,T2>::MultType, false >  MultType;
   typedef INVALID_TYPE                                                    DivType;
};

template< typename T1, typename T2, bool SO >
struct MathTrait< CompressedVector<T1,true>, CompressedMatrix<T2,SO> >
{
   typedef INVALID_TYPE                                                   HighType;
   typedef INVALID_TYPE                                                   LowType;
   typedef INVALID_TYPE                                                   AddType;
   typedef INVALID_TYPE                                                   SubType;
   typedef CompressedVector< typename MathTrait<T1,T2>::MultType, true >  MultType;
   typedef INVALID_TYPE                                                   DivType;
};

template< typename T1, bool SO, typename T2, size_t M, size_t N >
struct MathTrait< CompressedMatrix<T1,SO>, StaticMatrix<T2,M,N,SO> >
{
   typedef INVALID_TYPE                                                  HighType;
   typedef INVALID_TYPE                                                  LowType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::AddType, M, N, SO >  AddType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::SubType, M, N, SO >  SubType;
   typedef DynamicMatrix< typename MathTrait<T1,T2>::MultType, SO >      MultType;
   typedef INVALID_TYPE                                                  DivType;
};

template< typename T1, bool SO1, typename T2, size_t M, size_t N, bool SO2 >
struct MathTrait< CompressedMatrix<T1,SO1>, StaticMatrix<T2,M,N,SO2> >
{
   typedef INVALID_TYPE                                                     HighType;
   typedef INVALID_TYPE                                                     LowType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::AddType, M, N, false >  AddType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::SubType, M, N, false >  SubType;
   typedef DynamicMatrix< typename MathTrait<T1,T2>::MultType, SO1 >        MultType;
   typedef INVALID_TYPE                                                     DivType;
};

template< typename T1, size_t M, size_t N, bool SO, typename T2 >
struct MathTrait< StaticMatrix<T1,M,N,SO>, CompressedMatrix<T2,SO> >
{
   typedef INVALID_TYPE                                                  HighType;
   typedef INVALID_TYPE                                                  LowType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::AddType, M, N, SO >  AddType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::SubType, M, N, SO >  SubType;
   typedef DynamicMatrix< typename MathTrait<T1,T2>::MultType, SO >      MultType;
   typedef INVALID_TYPE                                                  DivType;
};

template< typename T1, size_t M, size_t N, bool SO1, typename T2, bool SO2 >
struct MathTrait< StaticMatrix<T1,M,N,SO1>, CompressedMatrix<T2,SO2> >
{
   typedef INVALID_TYPE                                                     HighType;
   typedef INVALID_TYPE                                                     LowType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::AddType, M, N, false >  AddType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::SubType, M, N, false >  SubType;
   typedef DynamicMatrix< typename MathTrait<T1,T2>::MultType, SO1 >        MultType;
   typedef INVALID_TYPE                                                     DivType;
};

template< typename T1, bool SO, typename T2 >
struct MathTrait< CompressedMatrix<T1,SO>, DynamicMatrix<T2,SO> >
{
   typedef INVALID_TYPE                                              HighType;
   typedef INVALID_TYPE                                              LowType;
   typedef DynamicMatrix< typename MathTrait<T1,T2>::AddType , SO >  AddType;
   typedef DynamicMatrix< typename MathTrait<T1,T2>::SubType , SO >  SubType;
   typedef DynamicMatrix< typename MathTrait<T1,T2>::MultType, SO >  MultType;
   typedef INVALID_TYPE                                              DivType;
};

template< typename T1, bool SO1, typename T2, bool SO2 >
struct MathTrait< CompressedMatrix<T1,SO1>, DynamicMatrix<T2,SO2> >
{
   typedef INVALID_TYPE                                                 HighType;
   typedef INVALID_TYPE                                                 LowType;
   typedef DynamicMatrix< typename MathTrait<T1,T2>::AddType , false >  AddType;
   typedef DynamicMatrix< typename MathTrait<T1,T2>::SubType , false >  SubType;
   typedef DynamicMatrix< typename MathTrait<T1,T2>::MultType, SO1 >    MultType;
   typedef INVALID_TYPE                                                 DivType;
};

template< typename T1, bool SO, typename T2 >
struct MathTrait< DynamicMatrix<T1,SO>, CompressedMatrix<T2,SO> >
{
   typedef INVALID_TYPE                                              HighType;
   typedef INVALID_TYPE                                              LowType;
   typedef DynamicMatrix< typename MathTrait<T1,T2>::AddType , SO >  AddType;
   typedef DynamicMatrix< typename MathTrait<T1,T2>::SubType , SO >  SubType;
   typedef DynamicMatrix< typename MathTrait<T1,T2>::MultType, SO >  MultType;
   typedef INVALID_TYPE                                              DivType;
};

template< typename T1, bool SO1, typename T2, bool SO2 >
struct MathTrait< DynamicMatrix<T1,SO1>, CompressedMatrix<T2,SO2> >
{
   typedef INVALID_TYPE                                                 HighType;
   typedef INVALID_TYPE                                                 LowType;
   typedef DynamicMatrix< typename MathTrait<T1,T2>::AddType , false >  AddType;
   typedef DynamicMatrix< typename MathTrait<T1,T2>::SubType , false >  SubType;
   typedef DynamicMatrix< typename MathTrait<T1,T2>::MultType, SO1 >    MultType;
   typedef INVALID_TYPE                                                 DivType;
};

template< typename T1, bool SO, typename T2 >
struct MathTrait< CompressedMatrix<T1,SO>, CompressedMatrix<T2,SO> >
{
   typedef CompressedMatrix< typename MathTrait<T1,T2>::HighType, SO >  HighType;
   typedef CompressedMatrix< typename MathTrait<T1,T2>::LowType , SO >  LowType;
   typedef CompressedMatrix< typename MathTrait<T1,T2>::AddType , SO >  AddType;
   typedef CompressedMatrix< typename MathTrait<T1,T2>::SubType , SO >  SubType;
   typedef CompressedMatrix< typename MathTrait<T1,T2>::MultType, SO >  MultType;
   typedef INVALID_TYPE                                                 DivType;
};

template< typename T1, bool SO1, typename T2, bool SO2 >
struct MathTrait< CompressedMatrix<T1,SO1>, CompressedMatrix<T2,SO2> >
{
   typedef CompressedMatrix< typename MathTrait<T1,T2>::HighType, SO1 >    HighType;
   typedef CompressedMatrix< typename MathTrait<T1,T2>::LowType , SO1 >    LowType;
   typedef CompressedMatrix< typename MathTrait<T1,T2>::AddType , false >  AddType;
   typedef CompressedMatrix< typename MathTrait<T1,T2>::SubType , false >  SubType;
   typedef CompressedMatrix< typename MathTrait<T1,T2>::MultType, SO1 >    MultType;
   typedef INVALID_TYPE                                                    DivType;
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  TYPE DEFINITIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief MxN single-precision matrix.
// \ingroup compressed_matrix
*/
typedef CompressedMatrix<float,false>  CMatMxNf;
//*************************************************************************************************


//*************************************************************************************************
/*!\brief MxN double-precision matrix.
// \ingroup compressed_matrix
*/
typedef CompressedMatrix<double,false>  CMatMxNd;
//*************************************************************************************************


//*************************************************************************************************
/*!\brief MxN matrix with system-specific precision.
// \ingroup compressed_matrix
*/
typedef CompressedMatrix<real,false>  CMatMxN;
//*************************************************************************************************

} // namespace blaze

#endif
