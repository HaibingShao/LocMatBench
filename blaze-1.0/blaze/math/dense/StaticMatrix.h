//=================================================================================================
/*!
//  \file blaze/math/dense/StaticMatrix.h
//  \brief Header file for the implementation of a fixed-size matrix
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

#ifndef _BLAZE_MATH_DENSE_STATICMATRIX_H_
#define _BLAZE_MATH_DENSE_STATICMATRIX_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <algorithm>
#include <cmath>
#include <ostream>
#include <stdexcept>
#include <blaze/math/DenseMatrix.h>
#include <blaze/math/Intrinsics.h>
#include <blaze/math/MathTrait.h>
#include <blaze/math/shims/Equal.h>
#include <blaze/math/shims/IsDefault.h>
#include <blaze/math/shims/IsNaN.h>
#include <blaze/math/shims/Reset.h>
#include <blaze/math/Types.h>
#include <blaze/math/typetraits/CanAlias.h>
#include <blaze/math/typetraits/IsColumnMajorMatrix.h>
#include <blaze/math/typetraits/IsRowMajorMatrix.h>
#include <blaze/math/typetraits/IsSparseMatrix.h>
#include <blaze/system/StorageOrder.h>
#include <blaze/util/AlignedStorage.h>
#include <blaze/util/Assert.h>
#include <blaze/util/constraints/Const.h>
#include <blaze/util/constraints/FloatingPoint.h>
#include <blaze/util/constraints/Numeric.h>
#include <blaze/util/constraints/Pointer.h>
#include <blaze/util/constraints/Reference.h>
#include <blaze/util/constraints/Vectorizable.h>
#include <blaze/util/constraints/Volatile.h>
#include <blaze/util/DisableIf.h>
#include <blaze/util/EnableIf.h>
#include <blaze/util/mpl/If.h>
#include <blaze/util/StaticAssert.h>
#include <blaze/util/Template.h>
#include <blaze/util/Types.h>
#include <blaze/util/typetraits/IsBuiltin.h>
#include <blaze/util/typetraits/IsFloatingPoint.h>
#include <blaze/util/typetraits/IsNumeric.h>
#include <blaze/util/typetraits/IsSame.h>
#include <blaze/util/typetraits/IsVectorizable.h>


namespace blaze {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*!\defgroup static_matrix StaticMatrix
// \ingroup dense_matrix
*/
/*!\brief Efficient implementation of a fixed-sized matrix.
// \ingroup static_matrix
//
// The StaticMatrix class template is the representation of a fixed-size matrix with statically
// allocated elements of arbitrary type. The type of the elements, the number of rows and columns
// and the storage order of the matrix can be specified via the four template parameters:

   \code
   template< typename Type, size_t M, size_t N, bool SO >
   class StaticMatrix;
   \endcode

//  - Type: specifies the type of the matrix elements. StaticMatrix can be used with any
//          non-cv-qualified element type. The arithmetic operators for matrix/matrix,
//          matrix/vector and matrix/element operations with the same element type work
//          for any element type as long as the element type supports the arithmetic
//          operation. Arithmetic operations between matrices, vectors and elements of
//          different element types are only supported for all data types supported by
//          the MathTrait class template (for details see the MathTrait class description).
//  - M   : specifies the total number of rows of the matrix.
//  - N   : specifies the total number of columns of the matrix. Note that it is expected
//          that StaticMatrix is only used for tiny and small matrices.
//  - SO  : specifies the storage order (blaze::rowMajor, blaze::columnMajor) of the matrix.
//          The default value is blaze::rowMajor.
//
// Depending on the storage order, the matrix elements are either stored in a row-wise fashion
// or in a column-wise fashion. Given the 2x3 matrix

                          \f[\left(\begin{array}{*{3}{c}}
                          1 & 2 & 3 \\
                          4 & 5 & 6 \\
                          \end{array}\right)\f]\n

// in case of row-major order the elements are stored in the order

                          \f[\left(\begin{array}{*{6}{c}}
                          1 & 2 & 3 & 4 & 5 & 6. \\
                          \end{array}\right)\f]

// In case of column-major order the elements are stored in the order

                          \f[\left(\begin{array}{*{6}{c}}
                          1 & 4 & 2 & 5 & 3 & 6. \\
                          \end{array}\right)\f]

// The use of StaticMatrix is very natural and intuitive. All operations (addition, subtraction,
// multiplication, scaling, ...) can be performed on all possible combination of row-major and
// column-major dense and sparse matrices with fitting element types. The following example gives
// an impression of the use of StaticMatrix:

   \code
   using blaze::StaticMatrix;
   using blaze::CompressedMatrix;
   using blaze::rowMajor;
   using blaze::columnMajor;

   StaticMatrix<double,2UL,3UL,rowMajor> A;   // Default constructed, non-initialized, row-major 2x3 matrix
   A(0,0) = 1.0; A(0,1) = 2.0; A(0,2) = 3.0;  // Initialization of the first row
   A(1,0) = 4.0; A(1,1) = 5.0; A(1,2) = 6.0;  // Initialization of the second row

   DynamicMatrix<float,2UL,3UL,columnMajor> B;  // Default constructed column-major single precision 2x3 matrix
   B(0,0) = 1.0; B(0,1) = 3.0; B(0,2) = 5.0;    // Initialization of the first row
   B(1,0) = 2.0; B(1,1) = 4.0; B(1,2) = 6.0;    // Initialization of the second row

   CompressedMatrix<float>     C( 2, 3 );  // Empty row-major sparse single precision matrix
   StaticMatrix<float,3UL,2UL> D( 4.0F );  // Directly, homogeneously initialized single precision 3x2 matrix

   StaticMatrix<double,2UL,3UL,rowMajor>    E( A );  // Creation of a new row-major matrix as a copy of A
   StaticMatrix<double,2UL,2UL,columnMajor> F;       // Creation of a default column-major matrix

   E = A + B;     // Matrix addition and assignment to a row-major matrix
   E = A - C;     // Matrix subtraction and assignment to a column-major matrix
   F = A * D;     // Matrix multiplication between two matrices of different element types

   A *= 2.0;      // In-place scaling of matrix A
   E  = 2.0 * B;  // Scaling of matrix B
   E  = B * 2.0;  // Scaling of matrix B

   E += A - B;    // Addition assignment
   E -= A + C;    // Subtraction assignment
   F *= A * D;    // Multiplication assignment
   \endcode
*/
template< typename Type                    // Data type of the matrix
        , size_t M                         // Number of rows
        , size_t N                         // Number of columns
        , bool SO = defaultStorageOrder >  // Storage order
class StaticMatrix : public DenseMatrix< StaticMatrix<Type,M,N,SO>, SO >
                   , private AlignedStorage<Type>
{
 private:
   //**Type definitions****************************************************************************
   typedef IntrinsicTrait<Type>  IT;  //!< Intrinsic trait for the vector element type.
   //**********************************************************************************************

   //**********************************************************************************************
   //! Alignment adjustment
   enum { NN = N + ( IT::size - ( N % IT::size ) ) % IT::size };
   //**********************************************************************************************

 public:
   //**Type definitions****************************************************************************
   typedef StaticMatrix<Type,M,N,SO>   This;           //!< Type of this StaticMatrix instance.
   typedef This                        ResultType;     //!< Result type for expression template evaluations.
   typedef StaticMatrix<Type,M,N,!SO>  OppositeType;   //!< Result type with opposite storage order for expression template evaluations.
   typedef StaticMatrix<Type,N,M,!SO>  TransposeType;  //!< Transpose type for expression template evaluations.
   typedef Type                        ElementType;    //!< Type of the matrix elements.
   typedef typename IT::Type           IntrinsicType;  //!< Intrinsic type of the matrix elements.
   typedef const This&                 CompositeType;  //!< Data type for composite expression templates.
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation flag for intrinsic optimization.
   /*! The \a vectorizable compilation flag indicates whether expressions the vector is involved
       in can be optimized via intrinsics. In case the element type of the vector is a vectorizable
       data type, the \a vectorizable compilation flag is set to \a true, otherwise it is set to
       \a false. */
   enum { vectorizable = IsVectorizable<Type>::value };

   //! Compilation flag for the detection of aliasing effects.
   /*! This compilation switch indicates whether this type potentially causes compuation errors
       due to aliasing effects. In case the type can cause aliasing effects, the \a canAlias
       switch is set to \a true, otherwise it is set to \a false. */
   enum { canAlias = 0 };
   //**********************************************************************************************

   //**Constructors********************************************************************************
   /*!\name Constructors */
   //@{
   explicit inline StaticMatrix();
   explicit inline StaticMatrix( const Type& init );

                                        inline StaticMatrix( const StaticMatrix& m );
   template< typename Other, bool SO2 > inline StaticMatrix( const StaticMatrix<Other,M,N,SO2>&  m );
   template< typename MT   , bool SO2 > inline StaticMatrix( const Matrix<MT,SO2>& m );

   inline StaticMatrix( const Type& v1, const Type& v2 );
   inline StaticMatrix( const Type& v1, const Type& v2, const Type& v3 );
   inline StaticMatrix( const Type& v1, const Type& v2, const Type& v3, const Type& v4 );
   inline StaticMatrix( const Type& v1, const Type& v2, const Type& v3, const Type& v4, const Type& v5 );
   inline StaticMatrix( const Type& v1, const Type& v2, const Type& v3, const Type& v4, const Type& v5,
                        const Type& v6 );
   inline StaticMatrix( const Type& v1, const Type& v2, const Type& v3, const Type& v4, const Type& v5,
                        const Type& v6, const Type& v7 );
   inline StaticMatrix( const Type& v1, const Type& v2, const Type& v3, const Type& v4, const Type& v5,
                        const Type& v6, const Type& v7, const Type& v8 );
   inline StaticMatrix( const Type& v1, const Type& v2, const Type& v3, const Type& v4, const Type& v5,
                        const Type& v6, const Type& v7, const Type& v8, const Type& v9 );
   inline StaticMatrix( const Type& v1, const Type& v2, const Type& v3, const Type& v4, const Type& v5,
                        const Type& v6, const Type& v7, const Type& v8, const Type& v9, const Type& v10 );
   //@}
   //**********************************************************************************************

   //**Destructor**********************************************************************************
   // No explicitly declared destructor.
   //**********************************************************************************************

   //**Data access functions***********************************************************************
   /*!\name Data access functions */
   //@{
   inline Type&       operator()( size_t i, size_t j );
   inline const Type& operator()( size_t i, size_t j ) const;
   inline Type*       data();
   inline const Type* data() const;
   //@}
   //**********************************************************************************************

   //**Assignment operators************************************************************************
   /*!\name Assignment operators */
   //@{
                                        inline StaticMatrix& operator= ( const Type& set );
                                        inline StaticMatrix& operator= ( const StaticMatrix& rhs );
   template< typename Other, bool SO2 > inline StaticMatrix& operator= ( const StaticMatrix<Other,M,N,SO2>& rhs );
   template< typename MT   , bool SO2 > inline StaticMatrix& operator= ( const Matrix<MT,SO2>& rhs );
   template< typename MT   , bool SO2 > inline StaticMatrix& operator+=( const Matrix<MT,SO2>& rhs );
   template< typename MT   , bool SO2 > inline StaticMatrix& operator-=( const Matrix<MT,SO2>& rhs );
   template< typename MT   , bool SO2 > inline StaticMatrix& operator*=( const Matrix<MT,SO2>& rhs );

   template< typename Other >
   inline typename EnableIf< IsNumeric<Other>, StaticMatrix >::Type&
      operator*=( Other rhs );

   template< typename Other >
   inline typename EnableIf< IsNumeric<Other>, StaticMatrix >::Type&
      operator/=( Other rhs );
   //@}
   //**********************************************************************************************

   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
                              inline size_t        rows() const;
                              inline size_t        columns() const;
                              inline size_t        spacing() const;
                              inline size_t        nonZeros() const;
                              inline size_t        nonZeros( size_t i ) const;
                              inline void          reset();
                              inline StaticMatrix& transpose();
                              inline bool          isDiagonal() const;
                              inline bool          isSymmetric() const;
   template< typename Other > inline StaticMatrix& scale( const Other& scalar );
                              inline void          swap( StaticMatrix& m ) /* throw() */;
   //@}
   //**********************************************************************************************

 private:
   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename MT >
   struct VectorizedAssign {
      enum { value = vectorizable && MT::vectorizable &&
                     IsSame<Type,typename MT::ElementType>::value &&
                     IsRowMajorMatrix<MT>::value };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename MT >
   struct VectorizedAddAssign {
      enum { value = vectorizable && MT::vectorizable &&
                     IsSame<Type,typename MT::ElementType>::value &&
                     IntrinsicTrait<Type>::addition &&
                     IsRowMajorMatrix<MT>::value };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename MT >
   struct VectorizedSubAssign {
      enum { value = vectorizable && MT::vectorizable &&
                     IsSame<Type,typename MT::ElementType>::value &&
                     IntrinsicTrait<Type>::subtraction &&
                     IsRowMajorMatrix<MT>::value };
   };
   /*! \endcond */
   //**********************************************************************************************

 public:
   //**Expression template evaluation functions****************************************************
   /*!\name Expression template evaluation functions */
   //@{
   template< typename Other > inline bool          isAliased( const Other* alias ) const;
                              inline IntrinsicType get      ( size_t i, size_t j ) const;

   template< typename MT, bool SO2 >
   inline typename DisableIf< VectorizedAssign<MT> >::Type
      assign( const DenseMatrix<MT,SO2>& rhs );

   template< typename MT, bool SO2 >
   inline typename EnableIf< VectorizedAssign<MT> >::Type
      assign( const DenseMatrix<MT,SO2>& rhs );

   template< typename MT > inline void assign( const SparseMatrix<MT,SO>&  rhs );
   template< typename MT > inline void assign( const SparseMatrix<MT,!SO>& rhs );

   template< typename MT, bool SO2 >
   inline typename DisableIf< VectorizedAddAssign<MT> >::Type
      addAssign( const DenseMatrix<MT,SO2>& rhs );

   template< typename MT, bool SO2 >
   inline typename EnableIf< VectorizedAddAssign<MT> >::Type
      addAssign( const DenseMatrix<MT,SO2>& rhs );

   template< typename MT > inline void addAssign( const SparseMatrix<MT,SO>&  rhs );
   template< typename MT > inline void addAssign( const SparseMatrix<MT,!SO>& rhs );

   template< typename MT, bool SO2 >
   inline typename DisableIf< VectorizedSubAssign<MT> >::Type
      subAssign( const DenseMatrix<MT,SO2>& rhs );

   template< typename MT, bool SO2 >
   inline typename EnableIf< VectorizedSubAssign<MT> >::Type
      subAssign( const DenseMatrix<MT,SO2>& rhs );

   template< typename MT > inline void subAssign( const SparseMatrix<MT,SO>&  rhs );
   template< typename MT > inline void subAssign( const SparseMatrix<MT,!SO>& rhs );
   //@}
   //**********************************************************************************************

 private:
   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   Type v_[M*NN];  //!< The statically allocated matrix elements.
                   /*!< Access to the matrix elements is gained via the function call operator.
                        In case of row-major order the memory layout of the elements is
                        \f[\left(\begin{array}{*{5}{c}}
                        0            & 1             & 2             & \cdots & N-1         \\
                        N            & N+1           & N+2           & \cdots & 2 \cdot N-1 \\
                        \vdots       & \vdots        & \vdots        & \ddots & \vdots      \\
                        M \cdot N-N  & M \cdot N-N+1 & M \cdot N-N+2 & \cdots & M \cdot N-1 \\
                        \end{array}\right)\f]. */
   //@}
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_NOT_BE_POINTER_TYPE  ( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_REFERENCE_TYPE( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_CONST         ( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_VOLATILE      ( Type );
   BLAZE_STATIC_ASSERT( NN % IT::size == 0UL );
   BLAZE_STATIC_ASSERT( NN >= N );
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************




//=================================================================================================
//
//  CONSTRUCTORS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief The default constructor for StaticMatrix.
//
// All matrix elements are initialized to the default value (i.e. 0 for integral data types).
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline StaticMatrix<Type,M,N,SO>::StaticMatrix()
{
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   if( IsBuiltin<Type>::value ) {
      for( size_t i=0UL; i<M*NN; ++i )
         v_[i] = Type();
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constructor for a homogenous initialization of all elements.
//
// \param init Initial value for all matrix elements.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline StaticMatrix<Type,M,N,SO>::StaticMatrix( const Type& init )
{
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   for( size_t i=0UL; i<M; ++i ) {
      for( size_t j=0UL; j<N; ++j )
         v_[i*NN+j] = init;

      if( IsBuiltin<Type>::value ) {
         for( size_t j=N; j<NN; ++j )
            v_[i*NN+j] = Type();
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief The copy constructor for StaticMatrix.
//
// \param m Matrix to be copied.
//
// The copy constructor is explicitly defined in order to enable/facilitate NRV optimization.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline StaticMatrix<Type,M,N,SO>::StaticMatrix( const StaticMatrix& m )
{
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   for( size_t i=0UL; i<M*NN; ++i )
      v_[i] = m.v_[i];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Conversion constructor from different StaticMatrix instances.
//
// \param m Matrix to be copied.
*/
template< typename Type   // Data type of the matrix
        , size_t M        // Number of rows
        , size_t N        // Number of columns
        , bool SO >       // Storage order
template< typename Other  // Data type of the foreign matrix
        , bool SO2 >      // Storage order of the foreign matrix
inline StaticMatrix<Type,M,N,SO>::StaticMatrix( const StaticMatrix<Other,M,N,SO2>& m )
{
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   for( size_t i=0UL; i<M; ++i ) {
      for( size_t j=0UL; j<N; ++j )
         v_[i*NN+j] = m(i,j);

      if( IsBuiltin<Type>::value ) {
         for( size_t j=N; j<NN; ++j )
            v_[i*NN+j] = Type();
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Conversion constructor from different matrices.
//
// \param m Matrix to be copied.
// \exception std::invalid_argument Invalid setup of static matrix.
//
// This constructor initializes the static matrix from the given matrix. In case the size of
// the given matrix does not match the size of the static matrix (i.e. the number of rows is
// not M or the number of columns is not N), a \a std::invalid_argument exception is thrown.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT    // Type of the foreign matrix
        , bool SO2 >     // Storage order of the foreign matrix
inline StaticMatrix<Type,M,N,SO>::StaticMatrix( const Matrix<MT,SO2>& m )
{
   using blaze::assign;

   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   if( (~m).rows() != M || (~m).columns() != N )
      throw std::invalid_argument( "Invalid setup of static matrix" );

   if( IsBuiltin<Type>::value ) {
      for( size_t i=0UL; i<M; ++i ) {
         for( size_t j=( IsSparseMatrix<MT>::value )?( 0UL ):( N ); j<NN; ++j )
            v_[i*NN+j] = Type();
      }
   }

   assign( *this, ~m );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constructor for \f$ 1 \times 2 \f$ and \f$ 2 \times 1 \f$ matrices.
//
// \param v1 The initializer for the first matrix element.
// \param v2 The initializer for the second matrix element.
//
// This constructor offers the option to directly initialize a newly created \f$ 1 \times 2 \f$
// and \f$ 2 \times 1 \f$ matrix. The following example demonstrates this by creating the matrix

                     \f[\left(\begin{array}{*{2}{c}}
                     1 & 2 \\
                     \end{array}\right)\f]:

   \code
   blaze::StaticMatrix<int,1,2,false> A( 1, 2 );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline StaticMatrix<Type,M,N,SO>::StaticMatrix( const Type& v1, const Type& v2 )
{
   BLAZE_STATIC_ASSERT( M*N == 2UL );
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   // Initialization of a 1x2 matrix
   if( M == 1UL ) {
      v_[0UL] = v1;
      v_[1UL] = v2;
   }

   // Initialization of a 2x1 matrix
   else {
      v_[0UL] = v1;
      v_[ NN] = v2;
   }

   if( IsBuiltin<Type>::value ) {
      for( size_t i=0UL; i<M; ++i ) {
         for( size_t j=N; j<NN; ++j )
            v_[i*NN+j] = Type();
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constructor for \f$ 1 \times 3 \f$ and \f$ 3 \times 1 \f$ matrices.
//
// \param v1 The initializer for the first matrix element.
// \param v2 The initializer for the second matrix element.
// \param v3 The initializer for the third matrix element.
//
// This constructor offers the option to directly initialize a newly created \f$ 1 \times 3 \f$
// and \f$ 3 \times 1 \f$ matrix. The following example demonstrates this by creating the matrix

                     \f[\left(\begin{array}{*{3}{c}}
                     1 & 2 & 3 \\
                     \end{array}\right)\f]:

   \code
   blaze::StaticMatrix<int,1,3,false> A( 1, 2, 3 );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline StaticMatrix<Type,M,N,SO>::StaticMatrix( const Type& v1, const Type& v2, const Type& v3 )
{
   BLAZE_STATIC_ASSERT( M*N == 3UL );
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   // Initialization of a 1x3 matrix
   if( M == 1UL ) {
      v_[0UL] = v1;
      v_[1UL] = v2;
      v_[2UL] = v3;
   }

   // Initialization of a 3x1 matrix
   else {
      v_[   0UL] = v1;
      v_[    NN] = v2;
      v_[2UL*NN] = v3;
   }

   if( IsBuiltin<Type>::value ) {
      for( size_t i=0UL; i<M; ++i ) {
         for( size_t j=N; j<NN; ++j )
            v_[i*NN+j] = Type();
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constructor for \f$ 1 \times 4 \f$, \f$ 2 \times 2 \f$, and \f$ 4 \times 1 \f$ matrices.
//
// \param v1 The initializer for the first matrix element.
// \param v2 The initializer for the second matrix element.
// \param v3 The initializer for the third matrix element.
// \param v4 The initializer for the fourth matrix element.
//
// This constructor offers the option to directly initialize a newly created \f$ 1 \times 4 \f$,
// \f$ 2 \times 2 \f$ and \f$ 3 \times 1 \f$ matrix. The following example demonstrates this by
// creating the matrix

                     \f[\left(\begin{array}{*{2}{c}}
                     1 & 2 \\
                     3 & 4 \\
                     \end{array}\right)\f]:

   \code
   blaze::StaticMatrix<int,2,2,false> A( 1, 2, 3, 4 );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline StaticMatrix<Type,M,N,SO>::StaticMatrix( const Type& v1, const Type& v2,
                                                const Type& v3, const Type& v4 )
{
   BLAZE_STATIC_ASSERT( M*N == 4UL );
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   // Initialization of a 1x4 matrix
   if( M == 1UL ) {
      v_[0UL] = v1;
      v_[1UL] = v2;
      v_[2UL] = v3;
      v_[3UL] = v4;
   }

   // Initialization of a 2x2 matrix
   else if( M == 2UL ) {
      v_[   0UL] = v1;
      v_[   1UL] = v2;
      v_[NN    ] = v3;
      v_[NN+1UL] = v4;
   }

   // Initialization of a 4x1 matrix
   else {
      v_[   0UL] = v1;
      v_[    NN] = v2;
      v_[2UL*NN] = v3;
      v_[3UL*NN] = v4;
   }

   if( IsBuiltin<Type>::value ) {
      for( size_t i=0UL; i<M; ++i ) {
         for( size_t j=N; j<NN; ++j )
            v_[i*NN+j] = Type();
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constructor for \f$ 1 \times 5 \f$ and \f$ 5 \times 1 \f$ matrices.
//
// \param v1 The initializer for the first matrix element.
// \param v2 The initializer for the second matrix element.
// \param v3 The initializer for the third matrix element.
// \param v4 The initializer for the fourth matrix element.
// \param v5 The initializer for the fifth matrix element.
//
// This constructor offers the option to directly initialize a newly created \f$ 1 \times 5 \f$,
// and \f$ 5 \times 1 \f$ matrix. The following example demonstrates this by creating the matrix

                     \f[\left(\begin{array}{*{5}{c}}
                     1 & 2 & 3 & 4 & 5 \\
                     \end{array}\right)\f]:

   \code
   blaze::StaticMatrix<int,1,5,false> A( 1, 2, 3, 4, 5 );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline StaticMatrix<Type,M,N,SO>::StaticMatrix( const Type& v1, const Type& v2, const Type& v3,
                                                const Type& v4, const Type& v5 )
{
   BLAZE_STATIC_ASSERT( M*N == 5UL );
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   // Initialization of a 1x5 matrix
   if( M == 1UL ) {
      v_[0UL] = v1;
      v_[1UL] = v2;
      v_[2UL] = v3;
      v_[3UL] = v4;
      v_[4UL] = v5;
   }

   // Initialization of a 5x1 matrix
   else {
      v_[   0UL] = v1;
      v_[    NN] = v2;
      v_[2UL*NN] = v3;
      v_[3UL*NN] = v4;
      v_[4UL*NN] = v5;
   }

   if( IsBuiltin<Type>::value ) {
      for( size_t i=0UL; i<M; ++i ) {
         for( size_t j=N; j<NN; ++j )
            v_[i*NN+j] = Type();
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constructor for \f$ 1 \times 6 \f$, \f$ 2 \times 3 \f$, \f$ 3 \times 2 \f$, and
//        \f$ 6 \times 1 \f$ matrices.
//
// \param v1 The initializer for the first matrix element.
// \param v2 The initializer for the second matrix element.
// \param v3 The initializer for the third matrix element.
// \param v4 The initializer for the fourth matrix element.
// \param v5 The initializer for the fifth matrix element.
// \param v6 The initializer for the sixth matrix element.
//
// This constructor offers the option to directly initialize a newly created \f$ 1 \times 6 \f$,
// \f$ 2 \times 3 \f$, \f$ 3 \times 2 \f$, and \f$ 6 \times 1 \f$ matrix. The following example
// demonstrates this by creating the matrix

                     \f[\left(\begin{array}{*{3}{c}}
                     1 & 2 & 3 \\
                     4 & 5 & 6 \\
                     \end{array}\right)\f]:

   \code
   blaze::StaticMatrix<int,2,3,false> A( 1, 2, 3, 4, 5, 6 );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline StaticMatrix<Type,M,N,SO>::StaticMatrix( const Type& v1, const Type& v2, const Type& v3,
                                                const Type& v4, const Type& v5, const Type& v6 )
{
   BLAZE_STATIC_ASSERT( M*N == 6UL );
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   // Initialization of a 1x6 matrix
   if( M == 1UL ) {
      v_[0UL] = v1;
      v_[1UL] = v2;
      v_[2UL] = v3;
      v_[3UL] = v4;
      v_[4UL] = v5;
      v_[5UL] = v6;
   }

   // Initialization of a 2x3 matrix
   else if( M == 2UL ) {
      v_[   0UL] = v1;
      v_[   1UL] = v2;
      v_[   2UL] = v3;
      v_[NN    ] = v4;
      v_[NN+1UL] = v5;
      v_[NN+2UL] = v6;
   }

   // Initialization of a 3x2 matrix
   else if( M == 3UL ) {
      v_[       0UL] = v1;
      v_[       1UL] = v2;
      v_[    NN    ] = v3;
      v_[    NN+1UL] = v4;
      v_[2UL*NN    ] = v5;
      v_[2UL*NN+1UL] = v6;
   }

   // Initialization of a 6x1 matrix
   else {
      v_[   0UL] = v1;
      v_[    NN] = v2;
      v_[2UL*NN] = v3;
      v_[3UL*NN] = v4;
      v_[4UL*NN] = v5;
      v_[5UL*NN] = v6;
   }

   if( IsBuiltin<Type>::value ) {
      for( size_t i=0UL; i<M; ++i ) {
         for( size_t j=N; j<NN; ++j )
            v_[i*NN+j] = Type();
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constructor for \f$ 1 \times 7 \f$ and \f$ 7 \times 1 \f$ matrices.
//
// \param v1 The initializer for the first matrix element.
// \param v2 The initializer for the second matrix element.
// \param v3 The initializer for the third matrix element.
// \param v4 The initializer for the fourth matrix element.
// \param v5 The initializer for the fifth matrix element.
// \param v6 The initializer for the sixth matrix element.
// \param v7 The initializer for the seventh matrix element.
//
// This constructor offers the option to directly initialize a newly created \f$ 1 \times 7 \f$,
// and \f$ 7 \times 1 \f$ matrix. The following example demonstrates this by creating the matrix

                     \f[\left(\begin{array}{*{7}{c}}
                     1 & 2 & 3 & 4 & 5 & 6 & 7 \\
                     \end{array}\right)\f]:

   \code
   blaze::StaticMatrix<int,1,7,false> A( 1, 2, 3, 4, 5, 6, 7 );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline StaticMatrix<Type,M,N,SO>::StaticMatrix( const Type& v1, const Type& v2, const Type& v3,
                                                const Type& v4, const Type& v5, const Type& v6,
                                                const Type& v7 )
{
   BLAZE_STATIC_ASSERT( M*N == 7UL );
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   // Initialization of a 1x7 matrix
   if( M == 1UL ) {
      v_[0UL] = v1;
      v_[1UL] = v2;
      v_[2UL] = v3;
      v_[3UL] = v4;
      v_[4UL] = v5;
      v_[5UL] = v6;
      v_[6UL] = v7;
   }

   // Initialization of a 7x1 matrix
   else {
      v_[   0UL] = v1;
      v_[    NN] = v2;
      v_[2UL*NN] = v3;
      v_[3UL*NN] = v4;
      v_[4UL*NN] = v5;
      v_[5UL*NN] = v6;
      v_[6UL*NN] = v7;
   }

   if( IsBuiltin<Type>::value ) {
      for( size_t i=0UL; i<M; ++i ) {
         for( size_t j=N; j<NN; ++j )
            v_[i*NN+j] = Type();
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constructor for \f$ 1 \times 8 \f$ and \f$ 8 \times 1 \f$ matrices.
//
// \param v1 The initializer for the first matrix element.
// \param v2 The initializer for the second matrix element.
// \param v3 The initializer for the third matrix element.
// \param v4 The initializer for the fourth matrix element.
// \param v5 The initializer for the fifth matrix element.
// \param v6 The initializer for the sixth matrix element.
// \param v7 The initializer for the seventh matrix element.
// \param v8 The initializer for the eigth matrix element.
//
// This constructor offers the option to directly initialize a newly created \f$ 1 \times 8 \f$,
// \f$ 2 \times 4 \f$, \f$ 4 \times 2 \f$, and \f$ 8 \times 1 \f$ matrix. The following example
// demonstrates this by creating the matrix

                     \f[\left(\begin{array}{*{4}{c}}
                     1 & 2 & 3 & 4 \\
                     5 & 6 & 7 & 8 \\
                     \end{array}\right)\f]:

   \code
   blaze::StaticMatrix<int,2,4,false> A( 1, 2, 3, 4, 5, 6, 7, 8 );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline StaticMatrix<Type,M,N,SO>::StaticMatrix( const Type& v1, const Type& v2, const Type& v3,
                                                const Type& v4, const Type& v5, const Type& v6,
                                                const Type& v7, const Type& v8 )
{
   BLAZE_STATIC_ASSERT( M*N == 8UL );
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   // Initialization of a 1x8 matrix
   if( M == 1UL ) {
      v_[0UL] = v1;
      v_[1UL] = v2;
      v_[2UL] = v3;
      v_[3UL] = v4;
      v_[4UL] = v5;
      v_[5UL] = v6;
      v_[6UL] = v7;
      v_[7UL] = v8;
   }

   // Initialization of a 2x4 matrix
   else if( M == 2UL ) {
      v_[   0UL] = v1;
      v_[   1UL] = v2;
      v_[   2UL] = v3;
      v_[   3UL] = v4;
      v_[NN    ] = v5;
      v_[NN+1UL] = v6;
      v_[NN+2UL] = v7;
      v_[NN+3UL] = v8;
   }

   // Initialization of a 4x2 matrix
   else if( M == 4UL ) {
      v_[       0UL] = v1;
      v_[       1UL] = v2;
      v_[    NN    ] = v3;
      v_[    NN+1UL] = v4;
      v_[2UL*NN    ] = v5;
      v_[2UL*NN+1UL] = v6;
      v_[3UL*NN    ] = v7;
      v_[3UL*NN+1UL] = v8;
   }

   // Initialization of a 8x1 matrix
   else {
      v_[   0UL] = v1;
      v_[    NN] = v2;
      v_[2UL*NN] = v3;
      v_[3UL*NN] = v4;
      v_[4UL*NN] = v5;
      v_[5UL*NN] = v6;
      v_[6UL*NN] = v7;
      v_[7UL*NN] = v8;
   }

   if( IsBuiltin<Type>::value ) {
      for( size_t i=0UL; i<M; ++i ) {
         for( size_t j=N; j<NN; ++j )
            v_[i*NN+j] = Type();
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constructor for \f$ 1 \times 9 \f$, \f$ 3 \times 3 \f$, and \f$ 9 \times 1 \f$ matrices.
//
// \param v1 The initializer for the first matrix element.
// \param v2 The initializer for the second matrix element.
// \param v3 The initializer for the third matrix element.
// \param v4 The initializer for the fourth matrix element.
// \param v5 The initializer for the fifth matrix element.
// \param v6 The initializer for the sixth matrix element.
// \param v7 The initializer for the seventh matrix element.
// \param v8 The initializer for the eigth matrix element.
// \param v9 The initializer for the ninth matrix element.
//
// This constructor offers the option to directly initialize a newly created \f$ 1 \times 9 \f$,
// \f$ 3 \times 3 \f$, and \f$ 9 \times 1 \f$ matrix. The following example demonstrates this by
// creating the matrix

                     \f[\left(\begin{array}{*{3}{c}}
                     1 & 2 & 3 \\
                     4 & 5 & 6 \\
                     7 & 8 & 9 \\
                     \end{array}\right)\f]:

   \code
   blaze::StaticMatrix<int,3,3,false> A( 1, 2, 3, 4, 5, 6, 7, 8, 9 );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline StaticMatrix<Type,M,N,SO>::StaticMatrix( const Type& v1, const Type& v2, const Type& v3,
                                                const Type& v4, const Type& v5, const Type& v6,
                                                const Type& v7, const Type& v8, const Type& v9 )
{
   BLAZE_STATIC_ASSERT( M*N == 9UL );
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   // Initialization of a 1x9 matrix
   if( M == 1UL ) {
      v_[0UL] = v1;
      v_[1UL] = v2;
      v_[2UL] = v3;
      v_[3UL] = v4;
      v_[4UL] = v5;
      v_[5UL] = v6;
      v_[6UL] = v7;
      v_[7UL] = v8;
      v_[8UL] = v9;
   }

   // Initialization of a 3x3 matrix
   else if( M == 3UL ) {
      v_[       0UL] = v1;
      v_[       1UL] = v2;
      v_[       2UL] = v3;
      v_[    NN    ] = v4;
      v_[    NN+1UL] = v5;
      v_[    NN+2UL] = v6;
      v_[2UL*NN    ] = v7;
      v_[2UL*NN+1UL] = v8;
      v_[2UL*NN+2UL] = v9;
   }

   // Initialization of a 9x1 matrix
   else {
      v_[   0UL] = v1;
      v_[    NN] = v2;
      v_[2UL*NN] = v3;
      v_[3UL*NN] = v4;
      v_[4UL*NN] = v5;
      v_[5UL*NN] = v6;
      v_[6UL*NN] = v7;
      v_[7UL*NN] = v8;
      v_[8UL*NN] = v9;
   }

   if( IsBuiltin<Type>::value ) {
      for( size_t i=0UL; i<M; ++i ) {
         for( size_t j=N; j<NN; ++j )
            v_[i*NN+j] = Type();
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constructor for \f$ 1 \times 10 \f$, \f$ 2 \times 5 \f$, \f$ 5 \times 2 \f$, and
//        \f$ 10 \times 1 \f$ matrices.
//
// \param v1 The initializer for the first matrix element.
// \param v2 The initializer for the second matrix element.
// \param v3 The initializer for the third matrix element.
// \param v4 The initializer for the fourth matrix element.
// \param v5 The initializer for the fifth matrix element.
// \param v6 The initializer for the sixth matrix element.
// \param v7 The initializer for the seventh matrix element.
// \param v8 The initializer for the eigth matrix element.
// \param v9 The initializer for the ninth matrix element.
// \param v10 The initializer for the tenth matrix element.
//
// This constructor offers the option to directly initialize a newly created \f$ 1 \times 10 \f$,
// \f$ 2 \times 5 \f$, \f$ 5 \times 2 \f$, and \f$ 10 \times 1 \f$ matrix. The following example
// demonstrates this by creating the matrix

                     \f[\left(\begin{array}{*{5}{c}}
                     1 & 2 & 3 & 4 & 5  \\
                     6 & 7 & 8 & 8 & 10 \\
                     \end{array}\right)\f]:

   \code
   blaze::StaticMatrix<int,2,5,false> A( 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline StaticMatrix<Type,M,N,SO>::StaticMatrix( const Type& v1, const Type& v2, const Type& v3,
                                                const Type& v4, const Type& v5, const Type& v6,
                                                const Type& v7, const Type& v8, const Type& v9,
                                                const Type& v10 )
{
   BLAZE_STATIC_ASSERT( M*N == 10UL );
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   // Initialization of a 1x10 matrix
   if( M == 1UL ) {
      v_[0UL] = v1;
      v_[1UL] = v2;
      v_[2UL] = v3;
      v_[3UL] = v4;
      v_[4UL] = v5;
      v_[5UL] = v6;
      v_[6UL] = v7;
      v_[7UL] = v8;
      v_[8UL] = v9;
      v_[9UL] = v10;
   }

   // Initialization of a 2x5 matrix
   else if( M == 2UL ) {
      v_[   0UL] = v1;
      v_[   1UL] = v2;
      v_[   2UL] = v3;
      v_[   3UL] = v4;
      v_[   4UL] = v5;
      v_[NN    ] = v6;
      v_[NN+1UL] = v7;
      v_[NN+2UL] = v8;
      v_[NN+3UL] = v9;
      v_[NN+4UL] = v10;
   }

   // Initialization of a 5x2 matrix
   else if( M == 5UL ) {
      v_[       0UL] = v1;
      v_[       1UL] = v2;
      v_[    NN    ] = v3;
      v_[    NN+1UL] = v4;
      v_[2UL*NN    ] = v5;
      v_[2UL*NN+1UL] = v6;
      v_[3UL*NN    ] = v7;
      v_[3UL*NN+1UL] = v8;
      v_[4UL*NN    ] = v9;
      v_[4UL*NN+1UL] = v10;
   }

   // Initialization of a 10x1 matrix
   else {
      v_[   0UL] = v1;
      v_[    NN] = v2;
      v_[2UL*NN] = v3;
      v_[3UL*NN] = v4;
      v_[4UL*NN] = v5;
      v_[5UL*NN] = v6;
      v_[6UL*NN] = v7;
      v_[7UL*NN] = v8;
      v_[8UL*NN] = v9;
      v_[9UL*NN] = v10;
   }

   if( IsBuiltin<Type>::value ) {
      for( size_t i=0UL; i<M; ++i ) {
         for( size_t j=N; j<NN; ++j )
            v_[i*NN+j] = Type();
      }
   }
}
//*************************************************************************************************




//=================================================================================================
//
//  DATA ACCESS FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief 2D-access to the matrix elements.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \return Reference to the accessed value.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline Type& StaticMatrix<Type,M,N,SO>::operator()( size_t i, size_t j )
{
   BLAZE_USER_ASSERT( i<M, "Invalid row access index"    );
   BLAZE_USER_ASSERT( j<N, "Invalid column access index" );
   return v_[i*NN+j];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief 2D-access to the matrix elements.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \return Reference-to-const to the accessed value.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline const Type& StaticMatrix<Type,M,N,SO>::operator()( size_t i, size_t j ) const
{
   BLAZE_USER_ASSERT( i<M, "Invalid row access index"    );
   BLAZE_USER_ASSERT( j<N, "Invalid column access index" );
   return v_[i*NN+j];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Low-level data access to the matrix elements.
//
// \return Pointer to the internal element storage.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline Type* StaticMatrix<Type,M,N,SO>::data()
{
   return v_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Low-level data access to the matrix elements.
//
// \return Pointer to the internal element storage.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline const Type* StaticMatrix<Type,M,N,SO>::data() const
{
   return v_;
}
//*************************************************************************************************




//=================================================================================================
//
//  ASSIGNMENT OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Homogenous assignment to all matrix elements.
//
// \param set Scalar value to be assigned to all matrix elements.
// \return Reference to the assigned matrix.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline StaticMatrix<Type,M,N,SO>& StaticMatrix<Type,M,N,SO>::operator=( const Type& set )
{
   for( size_t i=0UL; i<M; ++i )
      for( size_t j=0UL; j<N; ++j )
         v_[i*NN+j] = set;

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Copy assignment operator for StaticMatrix.
//
// \param rhs Matrix to be copied.
// \return Reference to the assigned matrix.
//
// Explicit definition of a copy assignment operator for performance reasons.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline StaticMatrix<Type,M,N,SO>& StaticMatrix<Type,M,N,SO>::operator=( const StaticMatrix& rhs )
{
   for( size_t i=0UL; i<M; ++i )
      for( size_t j=0UL; j<N; ++j )
         v_[i*NN+j] = rhs(i,j);

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Assignment operator for different StaticMatrix instances.
//
// \param rhs Matrix to be copied.
// \return Reference to the assigned matrix.
*/
template< typename Type   // Data type of the matrix
        , size_t M        // Number of rows
        , size_t N        // Number of columns
        , bool SO >       // Storage order
template< typename Other  // Data type of the foreign matrix
        , bool SO2 >      // Storage order of the foreign matrix
inline StaticMatrix<Type,M,N,SO>&
   StaticMatrix<Type,M,N,SO>::operator=( const StaticMatrix<Other,M,N,SO2>& rhs )
{
   for( size_t i=0UL; i<M; ++i )
      for( size_t j=0UL; j<N; ++j )
         v_[i*NN+j] = rhs(i,j);

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Assignment operator for different matrices.
//
// \param rhs Matrix to be copied.
// \return Reference to the assigned matrix.
// \exception std::invalid_argument Invalid assignment to static matrix.
//
// This constructor initializes the matrix as a copy of the given matrix. In case the
// number of rows of the given matrix is not M or the number of columns is not N, a
// \a std::invalid_argument exception is thrown.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side matrix
        , bool SO2 >     // Storage order of the right-hand side matrix
inline StaticMatrix<Type,M,N,SO>& StaticMatrix<Type,M,N,SO>::operator=( const Matrix<MT,SO2>& rhs )
{
   using blaze::assign;

   if( (~rhs).rows() != M || (~rhs).columns() != N )
      throw std::invalid_argument( "Invalid assignment to static matrix" );

   if( CanAlias<MT>::value && (~rhs).isAliased( this ) ) {
      StaticMatrix tmp( ~rhs );
      swap( tmp );
   }
   else {
      if( IsSparseMatrix<MT>::value )
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
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side matrix
        , bool SO2 >     // Storage order of the right-hand side matrix
inline StaticMatrix<Type,M,N,SO>& StaticMatrix<Type,M,N,SO>::operator+=( const Matrix<MT,SO2>& rhs )
{
   using blaze::addAssign;

   if( (~rhs).rows() != M || (~rhs).columns() != N )
      throw std::invalid_argument( "Matrix sizes do not match" );

   if( CanAlias<MT>::value && (~rhs).isAliased( this ) ) {
      StaticMatrix tmp( ~rhs );
      addAssign( *this, tmp );
   }
   else {
      addAssign( *this, ~rhs );
   }

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
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side matrix
        , bool SO2 >     // Storage order of the right-hand side matrix
inline StaticMatrix<Type,M,N,SO>& StaticMatrix<Type,M,N,SO>::operator-=( const Matrix<MT,SO2>& rhs )
{
   using blaze::subAssign;

   if( (~rhs).rows() != M || (~rhs).columns() != N )
      throw std::invalid_argument( "Matrix sizes do not match" );

   if( CanAlias<MT>::value && (~rhs).isAliased( this ) ) {
      StaticMatrix tmp( ~rhs );
      subAssign( *this, tmp );
   }
   else {
      subAssign( *this, ~rhs );
   }

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
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side matrix
        , bool SO2 >     // Storage order of the right-hand side matrix
inline StaticMatrix<Type,M,N,SO>& StaticMatrix<Type,M,N,SO>::operator*=( const Matrix<MT,SO2>& rhs )
{
   if( M != N || (~rhs).rows() != M || (~rhs).columns() != M )
      throw std::invalid_argument( "Matrix sizes do not match" );

   StaticMatrix tmp( *this * (~rhs) );
   return this->operator=( tmp );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Multiplication assignment operator for the multiplication between a matrix and
//        a scalar value (\f$ A*=s \f$).
//
// \param rhs The right-hand side scalar value for the multiplication.
// \return Reference to the matrix.
*/
template< typename Type     // Data type of the matrix
        , size_t M          // Number of rows
        , size_t N          // Number of columns
        , bool SO >         // Storage order
template< typename Other >  // Data type of the right-hand side scalar
inline typename EnableIf< IsNumeric<Other>, StaticMatrix<Type,M,N,SO> >::Type&
   StaticMatrix<Type,M,N,SO>::operator*=( Other rhs )
{
   for( size_t i=0UL; i<M; ++i )
      for( size_t j=0UL; j<N; ++j )
         v_[i*NN+j] *= rhs;

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Division assignment operator for the division of a matrix by a scalar value
//        (\f$ A/=s \f$).
//
// \param rhs The right-hand side scalar value for the division.
// \return Reference to the matrix.
//
// \b Note: A division by zero is only checked by an user assert.
*/
template< typename Type     // Data type of the matrix
        , size_t M          // Number of rows
        , size_t N          // Number of columns
        , bool SO >         // Storage order
template< typename Other >  // Data type of the right-hand side scalar
inline typename EnableIf< IsNumeric<Other>, StaticMatrix<Type,M,N,SO> >::Type&
   StaticMatrix<Type,M,N,SO>::operator/=( Other rhs )
{
   BLAZE_USER_ASSERT( rhs != Other(0), "Division by zero detected" );

   typedef typename MathTrait<Type,Other>::DivType  DT;
   typedef typename If< IsNumeric<DT>, DT, Other >::Type  Tmp;

   // Depending on the two involved data types, an integer division is applied or a
   // floating point division is selected.
   if( IsNumeric<DT>::value && IsFloatingPoint<DT>::value ) {
      const Tmp tmp( Tmp(1)/static_cast<Tmp>( rhs ) );
      for( size_t i=0UL; i<M; ++i )
         for( size_t j=0UL; j<N; ++j )
            v_[i*NN+j] *= tmp;
   }
   else {
      for( size_t i=0UL; i<M; ++i )
         for( size_t j=0UL; j<N; ++j )
            v_[i*NN+j] /= rhs;
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
/*!\brief Returns the current number of rows of the matrix.
//
// \return The number of rows of the matrix.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline size_t StaticMatrix<Type,M,N,SO>::rows() const
{
   return M;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the current number of columns of the matrix.
//
// \return The number of columns of the matrix.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline size_t StaticMatrix<Type,M,N,SO>::columns() const
{
   return N;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the spacing between the beginning of two rows.
//
// \return The spacing between the beginning of two rows.
//
// This function returns the spacing between the beginning of two rows, i.e. the total number
// of elements of a row.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline size_t StaticMatrix<Type,M,N,SO>::spacing() const
{
   return NN;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the total number of non-zero elements in the matrix
//
// \return The number of non-zero elements in the matrix.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline size_t StaticMatrix<Type,M,N,SO>::nonZeros() const
{
   size_t nonzeros( 0UL );

   for( size_t i=0UL; i<M; ++i )
      for( size_t j=0UL; j<N; ++j )
         if( !isDefault( v_[i*NN+j] ) )
            ++nonzeros;

   return nonzeros;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the number of non-zero elements in the specified row.
//
// \param i The index of the row.
// \return The number of non-zero elements of row \a i.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline size_t StaticMatrix<Type,M,N,SO>::nonZeros( size_t i ) const
{
   BLAZE_USER_ASSERT( i < rows(), "Invalid row access index" );

   const size_t end( (i+1UL)*NN );
   size_t nonzeros( 0UL );

   for( size_t j=i*NN; j<end; ++j )
      if( !isDefault( v_[j] ) )
         ++nonzeros;

   return nonzeros;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Reset to the default initial values.
//
// \return void
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline void StaticMatrix<Type,M,N,SO>::reset()
{
   using blaze::reset;

   for( size_t i=0UL; i<M; ++i )
      for( size_t j=0UL; j<N; ++j )
         reset( v_[i*NN+j] );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Transposing the matrix.
//
// \return Reference to the transposed matrix.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline StaticMatrix<Type,M,N,SO>& StaticMatrix<Type,M,N,SO>::transpose()
{
   using std::swap;

   for( size_t i=1UL; i<M; ++i )
      for( size_t j=0UL; j<i; ++j )
         swap( v_[i*NN+j], v_[j*NN+i] );

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
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline bool StaticMatrix<Type,M,N,SO>::isDiagonal() const
{
   if( M != N ) return false;

   for( size_t i=1UL; i<M; ++i ) {
      for( size_t j=0UL; j<i; ++j ) {
         if( !isDefault( v_[i*NN+j] ) || !isDefault( v_[j*NN+i] ) )
            return false;
      }
   }

   return true;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checks if the matrix is symmetric.
//
// \return \a true if the matrix is symmetric, \a false if not.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline bool StaticMatrix<Type,M,N,SO>::isSymmetric() const
{
   if( M != N ) return false;

   for( size_t i=1UL; i<M; ++i ) {
      for( size_t j=0UL; j<i; ++j ) {
         if( !equal( v_[i*NN+j], v_[j*NN+i] ) )
            return false;
      }
   }

   return true;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Scaling of the matrix by the scalar value \a scalar (\f$ A*=s \f$).
//
// \param scalar The scalar value for the matrix scaling.
// \return Reference to the matrix.
*/
template< typename Type     // Data type of the matrix
        , size_t M          // Number of rows
        , size_t N          // Number of columns
        , bool SO >         // Storage order
template< typename Other >  // Data type of the scalar value
inline StaticMatrix<Type,M,N,SO>& StaticMatrix<Type,M,N,SO>::scale( const Other& scalar )
{
   for( size_t i=0UL; i<M; ++i )
      for( size_t j=0UL; j<N; ++j )
         v_[i*NN+j] *= scalar;

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Swapping the contents of two static matrices.
//
// \param m The matrix to be swapped.
// \return void
// \exception no-throw guarantee.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline void StaticMatrix<Type,M,N,SO>::swap( StaticMatrix& m ) /* throw() */
{
   using std::swap;

   for( size_t i=0UL; i<M; ++i ) {
      for( size_t j=0UL; j<N; ++j ) {
         swap( v_[i*NN+j], m(i,j) );
      }
   }
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
template< typename Type     // Data type of the matrix
        , size_t M          // Number of rows
        , size_t N          // Number of columns
        , bool SO >         // Storage order
template< typename Other >  // Data type of the foreign expression
inline bool StaticMatrix<Type,M,N,SO>::isAliased( const Other* alias ) const
{
   return static_cast<const void*>( this ) == static_cast<const void*>( alias );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Access to the intrinsic elements of the matrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \return Reference to the accessed values.
//
// This function offers a direct access to the intrinsic elements of the matrix. It must \b NOT
// be called explicitly! It is used internally for the performance optimized evaluation of
// expression templates. Calling this function explicitly might result in erroneous results
// and/or in compilation errors.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline typename StaticMatrix<Type,M,N,SO>::IntrinsicType
   StaticMatrix<Type,M,N,SO>::get( size_t i, size_t j ) const
{
   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( i            <  M  , "Invalid row access index"    );
   BLAZE_INTERNAL_ASSERT( j            <  N  , "Invalid column access index" );
   BLAZE_INTERNAL_ASSERT( j + IT::size <= NN , "Invalid column access index" );
   BLAZE_INTERNAL_ASSERT( j % IT::size == 0UL, "Invalid column access index" );

   return load( &v_[i*NN+j] );
}
//*************************************************************************************************


//*************************************************************************************************
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
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO2 >     // Storage order of the right-hand side dense matrix
inline typename DisableIf< typename StaticMatrix<Type,M,N,SO>::BLAZE_TEMPLATE VectorizedAssign<MT> >::Type
   StaticMatrix<Type,M,N,SO>::assign( const DenseMatrix<MT,SO2>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t i=0UL; i<M; ++i ) {
      for( size_t j=0UL; j<N; ++j ) {
         v_[i*NN+j] = (~rhs)(i,j);
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Intrinsic optimized implementation of the assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO2 >     // Storage order of the right-hand side dense matrix
inline typename EnableIf< typename StaticMatrix<Type,M,N,SO>::BLAZE_TEMPLATE VectorizedAssign<MT> >::Type
   StaticMatrix<Type,M,N,SO>::assign( const DenseMatrix<MT,SO2>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   for( size_t i=0UL; i<M; ++i ) {
      for( size_t j=0UL; j<N; j+=IT::size ) {
         store( &v_[i*NN+j], (~rhs).get(i,j) );
      }
   }
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
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,SO>::assign( const SparseMatrix<MT,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   typedef typename MT::ConstIterator  ConstIterator;

   for( size_t i=0UL; i<M; ++i )
      for( ConstIterator element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
         v_[i*NN+element->index()] = element->value();
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
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,SO>::assign( const SparseMatrix<MT,!SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   typedef typename MT::ConstIterator  ConstIterator;

   for( size_t j=0UL; j<N; ++j )
      for( ConstIterator element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
         v_[element->index()*NN+j] = element->value();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the addition assignment of a row-major dense matrix.
//
// \param rhs The right-hand side dense matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO2 >     // Storage order of the right-hand side dense matrix
inline typename DisableIf< typename StaticMatrix<Type,M,N,SO>::BLAZE_TEMPLATE VectorizedAddAssign<MT> >::Type
   StaticMatrix<Type,M,N,SO>::addAssign( const DenseMatrix<MT,SO2>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t i=0UL; i<M; ++i ) {
      for( size_t j=0UL; j<N; ++j ) {
         v_[i*NN+j] += (~rhs)(i,j);
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Intrinsic optimized implementation of the addition assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO2 >     // Storage order of the right-hand side dense matrix
inline typename EnableIf< typename StaticMatrix<Type,M,N,SO>::BLAZE_TEMPLATE VectorizedAddAssign<MT> >::Type
   StaticMatrix<Type,M,N,SO>::addAssign( const DenseMatrix<MT,SO2>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   for( size_t i=0UL; i<M; ++i ) {
      for( size_t j=0UL; j<N; j+=IT::size ) {
         store( &v_[i*NN+j], load( &v_[i*NN+j] ) + (~rhs).get(i,j) );
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the addition assignment of a row-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,SO>::addAssign( const SparseMatrix<MT,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   typedef typename MT::ConstIterator  ConstIterator;

   for( size_t i=0UL; i<M; ++i )
      for( ConstIterator element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
         v_[i*NN+element->index()] += element->value();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the addition assignment of a column-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,SO>::addAssign( const SparseMatrix<MT,!SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   typedef typename MT::ConstIterator  ConstIterator;

   for( size_t j=0UL; j<N; ++j )
      for( ConstIterator element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
         v_[element->index()*NN+j] += element->value();
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
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO2 >     // Storage order of the right-hand side dense matrix
inline typename DisableIf< typename StaticMatrix<Type,M,N,SO>::BLAZE_TEMPLATE VectorizedSubAssign<MT> >::Type
   StaticMatrix<Type,M,N,SO>::subAssign( const DenseMatrix<MT,SO2>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t i=0UL; i<M; ++i ) {
      for( size_t j=0UL; j<N; ++j ) {
         v_[i*NN+j] -= (~rhs)(i,j);
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Intrinsic optimized implementation of the subtraction assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO2 >     // Storage order of the right-hand side dense matrix
inline typename EnableIf< typename StaticMatrix<Type,M,N,SO>::BLAZE_TEMPLATE VectorizedSubAssign<MT> >::Type
   StaticMatrix<Type,M,N,SO>::subAssign( const DenseMatrix<MT,SO2>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   for( size_t i=0UL; i<M; ++i ) {
      for( size_t j=0UL; j<N; j+=IT::size ) {
         store( &v_[i*NN+j], load( &v_[i*NN+j] ) - (~rhs).get(i,j) );
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the subtraction assignment of a row-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,SO>::subAssign( const SparseMatrix<MT,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   typedef typename MT::ConstIterator  ConstIterator;

   for( size_t i=0UL; i<M; ++i )
      for( ConstIterator element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
         v_[i*NN+element->index()] -= element->value();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the subtraction assignment of a column-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,SO>::subAssign( const SparseMatrix<MT,!SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   typedef typename MT::ConstIterator  ConstIterator;

   for( size_t j=0UL; j<N; ++j )
      for( ConstIterator element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
         v_[element->index()*NN+j] -= element->value();
}
//*************************************************************************************************








//=================================================================================================
//
//  CLASS TEMPLATE SPECIALIZATION FOR COLUMN-MAJOR MATRICES
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Specialization of StaticMatrix for column-major matrices.
// \ingroup static_matrix
//
// This specialization of StaticMatrix adapts the class template to the requirements of
// column-major matrices.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
class StaticMatrix<Type,M,N,true> : public DenseMatrix< StaticMatrix<Type,M,N,true>, true >
                                  , private AlignedStorage<Type>
{
 private:
   //**Type definitions****************************************************************************
   typedef IntrinsicTrait<Type>  IT;  //!< Intrinsic trait for the vector element type.
   //**********************************************************************************************

   //**********************************************************************************************
   //! Alignment adjustment
   enum { MM = M + ( IT::size - ( M % IT::size ) ) % IT::size };
   //**********************************************************************************************

 public:
   //**Type definitions****************************************************************************
   typedef StaticMatrix<Type,M,N,true>   This;           //!< Type of this StaticMatrix instance.
   typedef This                          ResultType;     //!< Result type for expression template evaluations.
   typedef StaticMatrix<Type,M,N,false>  OppositeType;   //!< Result type with opposite storage order for expression template evaluations.
   typedef StaticMatrix<Type,N,M,false>  TransposeType;  //!< Transpose type for expression template evaluations.
   typedef Type                          ElementType;    //!< Type of the matrix elements.
   typedef typename IT::Type             IntrinsicType;  //!< Intrinsic type of the matrix elements.
   typedef const This&                   CompositeType;  //!< Data type for composite expression templates.
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation flag for intrinsic optimization.
   /*! The \a vectorizable compilation flag indicates whether expressions the vector is involved
       in can be optimized via intrinsics. In case the element type of the vector is a vectorizable
       data type, the \a vectorizable compilation flag is set to \a true, otherwise it is set to
       \a false. */
   enum { vectorizable = IsVectorizable<Type>::value };

   //! Compilation flag for the detection of aliasing effects.
   /*! This compilation switch indicates whether this type potentially causes compuation errors
       due to aliasing effects. In case the type can cause aliasing effects, the \a canAlias
       switch is set to \a true, otherwise it is set to \a false. */
   enum { canAlias = 0 };
   //**********************************************************************************************

   //**Constructors********************************************************************************
   /*!\name Constructors */
   //@{
   explicit inline StaticMatrix();
   explicit inline StaticMatrix( const Type& init );

                                       inline StaticMatrix( const StaticMatrix& m );
   template< typename Other, bool SO > inline StaticMatrix( const StaticMatrix<Other,M,N,SO>&  m );
   template< typename MT   , bool SO > inline StaticMatrix( const Matrix<MT,SO>& m );

   inline StaticMatrix( const Type& v1, const Type& v2 );
   inline StaticMatrix( const Type& v1, const Type& v2, const Type& v3 );
   inline StaticMatrix( const Type& v1, const Type& v2, const Type& v3, const Type& v4 );
   inline StaticMatrix( const Type& v1, const Type& v2, const Type& v3, const Type& v4, const Type& v5 );
   inline StaticMatrix( const Type& v1, const Type& v2, const Type& v3, const Type& v4, const Type& v5,
                        const Type& v6 );
   inline StaticMatrix( const Type& v1, const Type& v2, const Type& v3, const Type& v4, const Type& v5,
                        const Type& v6, const Type& v7 );
   inline StaticMatrix( const Type& v1, const Type& v2, const Type& v3, const Type& v4, const Type& v5,
                        const Type& v6, const Type& v7, const Type& v8 );
   inline StaticMatrix( const Type& v1, const Type& v2, const Type& v3, const Type& v4, const Type& v5,
                        const Type& v6, const Type& v7, const Type& v8, const Type& v9 );
   inline StaticMatrix( const Type& v1, const Type& v2, const Type& v3, const Type& v4, const Type& v5,
                        const Type& v6, const Type& v7, const Type& v8, const Type& v9, const Type& v10 );
   //@}
   //**********************************************************************************************

   //**Destructor**********************************************************************************
   // No explicitly declared destructor.
   //**********************************************************************************************

   //**Data access functions***********************************************************************
   /*!\name Data access functions */
   //@{
   inline Type&       operator()( size_t i, size_t j );
   inline const Type& operator()( size_t i, size_t j ) const;
   inline Type*       data();
   inline const Type* data() const;
   //@}
   //**********************************************************************************************

   //**Assignment operators************************************************************************
   /*!\name Assignment operators */
   //@{
                                       inline StaticMatrix& operator= ( const Type& set );
                                       inline StaticMatrix& operator= ( const StaticMatrix& rhs );
   template< typename Other, bool SO > inline StaticMatrix& operator= ( const StaticMatrix<Other,M,N,SO>&  rhs );
   template< typename MT   , bool SO > inline StaticMatrix& operator= ( const Matrix<MT,SO>& rhs );
   template< typename MT   , bool SO > inline StaticMatrix& operator+=( const Matrix<MT,SO>& rhs );
   template< typename MT   , bool SO > inline StaticMatrix& operator-=( const Matrix<MT,SO>& rhs );
   template< typename MT   , bool SO > inline StaticMatrix& operator*=( const Matrix<MT,SO>& rhs );

   template< typename Other >
   inline typename EnableIf< IsNumeric<Other>, StaticMatrix >::Type&
      operator*=( Other rhs );

   template< typename Other >
   inline typename EnableIf< IsNumeric<Other>, StaticMatrix >::Type&
      operator/=( Other rhs );
   //@}
   //**********************************************************************************************

   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
                              inline size_t        rows() const;
                              inline size_t        columns() const;
                              inline size_t        spacing() const;
                              inline size_t        nonZeros() const;
                              inline size_t        nonZeros( size_t j ) const;
                              inline void          reset();
                              inline StaticMatrix& transpose();
                              inline bool          isDiagonal() const;
                              inline bool          isSymmetric() const;
   template< typename Other > inline StaticMatrix& scale( const Other& scalar );
                              inline void          swap( StaticMatrix& m ) /* throw() */;
   //@}
   //**********************************************************************************************

 private:
   //**********************************************************************************************
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename MT >
   struct VectorizedAssign {
      enum { value = vectorizable && MT::vectorizable &&
                     IsSame<Type,typename MT::ElementType>::value &&
                     IsColumnMajorMatrix<MT>::value };
   };
   //**********************************************************************************************

   //**********************************************************************************************
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename MT >
   struct VectorizedAddAssign {
      enum { value = vectorizable && MT::vectorizable &&
                     IsSame<Type,typename MT::ElementType>::value &&
                     IntrinsicTrait<Type>::addition &&
                     IsColumnMajorMatrix<MT>::value };
   };
   //**********************************************************************************************

   //**********************************************************************************************
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename MT >
   struct VectorizedSubAssign {
      enum { value = vectorizable && MT::vectorizable &&
                     IsSame<Type,typename MT::ElementType>::value &&
                     IntrinsicTrait<Type>::subtraction &&
                     IsColumnMajorMatrix<MT>::value };
   };
   //**********************************************************************************************

 public:
   //**Expression template evaluation functions****************************************************
   /*!\name Expression template evaluation functions */
   //@{
   template< typename Other > inline bool          isAliased( const Other* alias ) const;
                              inline IntrinsicType get      ( size_t i, size_t j ) const;

   template< typename MT, bool SO >
   inline typename DisableIf< VectorizedAssign<MT> >::Type
      assign( const DenseMatrix<MT,SO>& rhs );

   template< typename MT, bool SO >
   inline typename EnableIf< VectorizedAssign<MT> >::Type
      assign( const DenseMatrix<MT,SO>& rhs );

   template< typename MT > inline void assign( const SparseMatrix<MT,true>&  rhs );
   template< typename MT > inline void assign( const SparseMatrix<MT,false>& rhs );

   template< typename MT, bool SO >
   inline typename DisableIf< VectorizedAddAssign<MT> >::Type
      addAssign( const DenseMatrix<MT,SO>& rhs );

   template< typename MT, bool SO >
   inline typename EnableIf< VectorizedAddAssign<MT> >::Type
      addAssign( const DenseMatrix<MT,SO>& rhs );

   template< typename MT > inline void addAssign( const SparseMatrix<MT,true>&  rhs );
   template< typename MT > inline void addAssign( const SparseMatrix<MT,false>& rhs );

   template< typename MT, bool SO >
   inline typename DisableIf< VectorizedSubAssign<MT> >::Type
      subAssign( const DenseMatrix<MT,SO>& rhs );

   template< typename MT, bool SO >
   inline typename EnableIf< VectorizedSubAssign<MT> >::Type
      subAssign( const DenseMatrix<MT,SO>& rhs );

   template< typename MT > inline void subAssign( const SparseMatrix<MT,true>&  rhs );
   template< typename MT > inline void subAssign( const SparseMatrix<MT,false>& rhs );
   //@}
   //**********************************************************************************************

 private:
   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   Type v_[MM*N];  //!< The statically allocated matrix elements.
                   /*!< Access to the matrix elements is gained via the function call operator. */
   //@}
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   BLAZE_CONSTRAINT_MUST_NOT_BE_POINTER_TYPE  ( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_REFERENCE_TYPE( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_CONST         ( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_VOLATILE      ( Type );
   BLAZE_STATIC_ASSERT( MM % IT::size == 0UL );
   BLAZE_STATIC_ASSERT( MM >= M );
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  CONSTRUCTORS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief The default constructor for StaticMatrix.
//
// All matrix elements are initialized to the default value (i.e. 0 for integral data types).
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline StaticMatrix<Type,M,N,true>::StaticMatrix()
{
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   if( IsBuiltin<Type>::value ) {
      for( size_t i=0UL; i<MM*N; ++i )
         v_[i] = Type();
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Constructor for a homogenous initialization of all elements.
//
// \param init Initial value for all matrix elements.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline StaticMatrix<Type,M,N,true>::StaticMatrix( const Type& init )
{
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   for( size_t j=0UL; j<N; ++j ) {
      for( size_t i=0UL; i<M; ++i )
         v_[i+j*MM] = init;

      if( IsBuiltin<Type>::value ) {
         for( size_t i=M; i<MM; ++i )
            v_[i+j*MM] = Type();
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief The copy constructor for StaticMatrix.
//
// \param m Matrix to be copied.
//
// The copy constructor is explicitly defined in order to enable/facilitate NRV optimization.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline StaticMatrix<Type,M,N,true>::StaticMatrix( const StaticMatrix& m )
{
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   for( size_t i=0UL; i<MM*N; ++i )
      v_[i] = m.v_[i];
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Conversion constructor from different StaticMatrix instances.
//
// \param m Matrix to be copied.
*/
template< typename Type   // Data type of the matrix
        , size_t M        // Number of rows
        , size_t N >      // Number of columns
template< typename Other  // Data type of the foreign matrix
        , bool SO >       // Storage order of the foreign matrix
inline StaticMatrix<Type,M,N,true>::StaticMatrix( const StaticMatrix<Other,M,N,SO>& m )
{
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   for( size_t j=0UL; j<N; ++j ) {
      for( size_t i=0UL; i<M; ++i )
         v_[i+j*MM] = m(i,j);

      if( IsBuiltin<Type>::value ) {
         for( size_t i=M; i<MM; ++i )
            v_[i+j*MM] = Type();
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Conversion constructor from different matrices.
//
// \param m Matrix to be copied.
// \exception std::invalid_argument Invalid setup of static matrix.
//
// This constructor initializes the static matrix from the given matrix. In case the size of
// the given matrix does not match the size of the static matrix (i.e. the number of rows is
// not M or the number of columns is not N), a \a std::invalid_argument exception is thrown.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT    // Type of the foreign matrix
        , bool SO >      // Storage order of the foreign matrix
inline StaticMatrix<Type,M,N,true>::StaticMatrix( const Matrix<MT,SO>& m )
{
   using blaze::assign;

   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   if( (~m).rows() != M || (~m).columns() != N )
      throw std::invalid_argument( "Invalid setup of static matrix" );

   if( IsBuiltin<Type>::value ) {
      for( size_t j=0UL; j<N; ++j )
         for( size_t i=( IsSparseMatrix<MT>::value )?( 0UL ):( M ); i<MM; ++i ) {
            v_[i+j*MM] = Type();
      }
   }

   assign( *this, ~m );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Constructor for \f$ 1 \times 2 \f$ and \f$ 2 \times 1 \f$ matrices.
//
// \param v1 The initializer for the first matrix element.
// \param v2 The initializer for the second matrix element.
//
// This constructor offers the option to directly initialize a newly created \f$ 1 \times 2 \f$
// and \f$ 2 \times 1 \f$ matrix. The following example demonstrates this by creating the matrix

                     \f[\left(\begin{array}{*{2}{c}}
                     1 & 2 \\
                     \end{array}\right)\f]:

   \code
   blaze::StaticMatrix<int,1,2,true> A( 1, 2 );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline StaticMatrix<Type,M,N,true>::StaticMatrix( const Type& v1, const Type& v2 )
{
   BLAZE_STATIC_ASSERT( M*N == 2UL );
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   // Initialization of a 2x1 matrix
   if( N == 1UL ) {
      v_[0UL] = v1;
      v_[1UL] = v2;
   }

   // Initialization of a 1x2 matrix
   else {
      v_[0UL] = v1;
      v_[ MM] = v2;
   }

   if( IsBuiltin<Type>::value ) {
      for( size_t j=0UL; j<N; ++j )
         for( size_t i=M; i<MM; ++i ) {
            v_[i+j*MM] = Type();
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Constructor for \f$ 1 \times 3 \f$ and \f$ 3 \times 1 \f$ matrices.
//
// \param v1 The initializer for the first matrix element.
// \param v2 The initializer for the second matrix element.
// \param v3 The initializer for the third matrix element.
//
// This constructor offers the option to directly initialize a newly created \f$ 1 \times 3 \f$
// and \f$ 3 \times 1 \f$ matrix. The following example demonstrates this by creating the matrix

                     \f[\left(\begin{array}{*{3}{c}}
                     1 & 2 & 3 \\
                     \end{array}\right)\f]:

   \code
   blaze::StaticMatrix<int,1,3,true> A( 1, 2, 3 );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline StaticMatrix<Type,M,N,true>::StaticMatrix( const Type& v1, const Type& v2, const Type& v3 )
{
   BLAZE_STATIC_ASSERT( M*N == 3UL );
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   // Initialization of a 3x1 matrix
   if( N == 1UL ) {
      v_[0UL] = v1;
      v_[1UL] = v2;
      v_[2UL] = v3;
   }

   // Initialization of a 1x3 matrix
   else {
      v_[   0UL] = v1;
      v_[    MM] = v2;
      v_[2UL*MM] = v3;
   }

   if( IsBuiltin<Type>::value ) {
      for( size_t j=0UL; j<N; ++j )
         for( size_t i=M; i<MM; ++i ) {
            v_[i+j*MM] = Type();
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Constructor for \f$ 1 \times 3 \f$, \f$ 2 \times 2 \f$, and \f$ 3 \times 1 \f$ matrices.
//
// \param v1 The initializer for the first matrix element.
// \param v2 The initializer for the second matrix element.
// \param v3 The initializer for the third matrix element.
// \param v4 The initializer for the fourth matrix element.
//
// This constructor offers the option to directly initialize a newly created \f$ 1 \times 4 \f$,
// \f$ 2 \times 2 \f$, and \f$ 4 \times 1 \f$ matrix. The following example demonstrates this by
// creating the matrix

                     \f[\left(\begin{array}{*{2}{c}}
                     1 & 3 \\
                     2 & 4 \\
                     \end{array}\right)\f]:

   \code
   blaze::StaticMatrix<int,2,2,true> A( 1, 2, 3, 4 );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline StaticMatrix<Type,M,N,true>::StaticMatrix( const Type& v1, const Type& v2, const Type& v3,
                                                  const Type& v4 )
{
   BLAZE_STATIC_ASSERT( M*N == 4UL );
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   // Initialization of a 4x1 matrix
   if( N == 1UL ) {
      v_[0UL] = v1;
      v_[1UL] = v2;
      v_[2UL] = v3;
      v_[3UL] = v4;
   }

   // Initialization of a 2x2 matrix
   else if( N == 2UL ) {
      v_[   0UL] = v1;
      v_[   1UL] = v2;
      v_[MM    ] = v3;
      v_[MM+1UL] = v4;
   }

   // Initialization of a 1x4 matrix
   else {
      v_[   0UL] = v1;
      v_[    MM] = v2;
      v_[2UL*MM] = v3;
      v_[3UL*MM] = v4;
   }

   if( IsBuiltin<Type>::value ) {
      for( size_t j=0UL; j<N; ++j )
         for( size_t i=M; i<MM; ++i ) {
            v_[i+j*MM] = Type();
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Constructor for \f$ 1 \times 5 \f$ and \f$ 5 \times 1 \f$ matrices.
//
// \param v1 The initializer for the first matrix element.
// \param v2 The initializer for the second matrix element.
// \param v3 The initializer for the third matrix element.
// \param v4 The initializer for the fourth matrix element.
// \param v5 The initializer for the fifth matrix element.
//
// This constructor offers the option to directly initialize a newly created \f$ 1 \times 5 \f$
// and \f$ 5 \times 1 \f$ matrix. The following example demonstrates this by creating the matrix

                     \f[\left(\begin{array}{*{5}{c}}
                     1 & 2 & 3 & 4 & 5 \\
                     \end{array}\right)\f]:

   \code
   blaze::StaticMatrix<int,1,5,true> A( 1, 2, 3, 4, 5 );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline StaticMatrix<Type,M,N,true>::StaticMatrix( const Type& v1, const Type& v2, const Type& v3,
                                                  const Type& v4, const Type& v5 )
{
   BLAZE_STATIC_ASSERT( M*N == 5UL );
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   // Initialization of a 5x1 matrix
   if( N == 1UL ) {
      v_[0UL] = v1;
      v_[1UL] = v2;
      v_[2UL] = v3;
      v_[3UL] = v4;
      v_[4UL] = v5;
   }

   // Initialization of a 1x5 matrix
   else {
      v_[   0UL] = v1;
      v_[    MM] = v2;
      v_[2UL*MM] = v3;
      v_[3UL*MM] = v4;
      v_[4UL*MM] = v5;
   }

   if( IsBuiltin<Type>::value ) {
      for( size_t j=0UL; j<N; ++j )
         for( size_t i=M; i<MM; ++i ) {
            v_[i+j*MM] = Type();
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Constructor for \f$ 1 \times 6 \f$, \f$ 2 \times 3 \f$, \f$ 3 \times 2 \f$, and
//        \f$ 6 \times 1 \f$ matrices.
//
// \param v1 The initializer for the first matrix element.
// \param v2 The initializer for the second matrix element.
// \param v3 The initializer for the third matrix element.
// \param v4 The initializer for the fourth matrix element.
// \param v5 The initializer for the fifth matrix element.
// \param v6 The initializer for the sixth matrix element.
//
// This constructor offers the option to directly initialize a newly created \f$ 1 \times 6 \f$,
// \f$ 2 \times 3 \f$, \f$ 3 \times 2 \f$, and \f$ 6 \times 1 \f$ matrix. The following example
// demonstrates this by creating the matrix

                     \f[\left(\begin{array}{*{3}{c}}
                     1 & 3 & 5 \\
                     2 & 4 & 6 \\
                     \end{array}\right)\f]:

   \code
   blaze::StaticMatrix<int,2,3,true> A( 1, 2, 3, 4, 5, 6 );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline StaticMatrix<Type,M,N,true>::StaticMatrix( const Type& v1, const Type& v2, const Type& v3,
                                                  const Type& v4, const Type& v5, const Type& v6 )
{
   BLAZE_STATIC_ASSERT( M*N == 6UL );
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   // Initialization of a 6x1 matrix
   if( N == 1UL ) {
      v_[0UL] = v1;
      v_[1UL] = v2;
      v_[2UL] = v3;
      v_[3UL] = v4;
      v_[4UL] = v5;
      v_[5UL] = v6;
   }

   // Initialization of a 3x2 matrix
   else if( N == 2UL ) {
      v_[   0UL] = v1;
      v_[   1UL] = v2;
      v_[   2UL] = v3;
      v_[MM    ] = v4;
      v_[MM+1UL] = v5;
      v_[MM+2UL] = v6;
   }

   // Initialization of a 2x3 matrix
   else if( N == 3UL ) {
      v_[       0UL] = v1;
      v_[       1UL] = v2;
      v_[    MM    ] = v3;
      v_[    MM+1UL] = v4;
      v_[2UL*MM    ] = v5;
      v_[2UL*MM+1UL] = v6;
   }

   // Initialization of a 1x6 matrix
   else {
      v_[   0UL] = v1;
      v_[    MM] = v2;
      v_[2UL*MM] = v3;
      v_[3UL*MM] = v4;
      v_[4UL*MM] = v5;
      v_[5UL*MM] = v6;
   }

   if( IsBuiltin<Type>::value ) {
      for( size_t j=0UL; j<N; ++j )
         for( size_t i=M; i<MM; ++i ) {
            v_[i+j*MM] = Type();
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Constructor for \f$ 1 \times 7 \f$ and \f$ 7 \times 1 \f$ matrices.
//
// \param v1 The initializer for the first matrix element.
// \param v2 The initializer for the second matrix element.
// \param v3 The initializer for the third matrix element.
// \param v4 The initializer for the fourth matrix element.
// \param v5 The initializer for the fifth matrix element.
// \param v6 The initializer for the sixth matrix element.
// \param v7 The initializer for the seventh matrix element.
//
// This constructor offers the option to directly initialize a newly created \f$ 1 \times 7 \f$
// and \f$ 7 \times 1 \f$ matrix. The following example demonstrates this by creating the matrix

                     \f[\left(\begin{array}{*{7}{c}}
                     1 & 2 & 3 & 4 & 5 & 6 & 7 \\
                     \end{array}\right)\f]:

   \code
   blaze::StaticMatrix<int,1,7,true> A( 1, 2, 3, 4, 5, 6, 7 );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline StaticMatrix<Type,M,N,true>::StaticMatrix( const Type& v1, const Type& v2, const Type& v3,
                                                  const Type& v4, const Type& v5, const Type& v6,
                                                  const Type& v7 )
{
   BLAZE_STATIC_ASSERT( M*N == 7UL );
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   // Initialization of a 7x1 matrix
   if( N == 1UL ) {
      v_[0UL] = v1;
      v_[1UL] = v2;
      v_[2UL] = v3;
      v_[3UL] = v4;
      v_[4UL] = v5;
      v_[5UL] = v6;
      v_[6UL] = v7;
   }

   // Initialization of a 1x7 matrix
   else {
      v_[   0UL] = v1;
      v_[    MM] = v2;
      v_[2UL*MM] = v3;
      v_[3UL*MM] = v4;
      v_[4UL*MM] = v5;
      v_[5UL*MM] = v6;
      v_[6UL*MM] = v7;
   }

   if( IsBuiltin<Type>::value ) {
      for( size_t j=0UL; j<N; ++j )
         for( size_t i=M; i<MM; ++i ) {
            v_[i+j*MM] = Type();
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Constructor for \f$ 1 \times 8 \f$, \f$ 2 \times 4 \f$, \f$ 4 \times 2 \f$, and
//        \f$ 8 \times 1 \f$ matrices.
//
// \param v1 The initializer for the first matrix element.
// \param v2 The initializer for the second matrix element.
// \param v3 The initializer for the third matrix element.
// \param v4 The initializer for the fourth matrix element.
// \param v5 The initializer for the fifth matrix element.
// \param v6 The initializer for the sixth matrix element.
// \param v7 The initializer for the seventh matrix element.
// \param v8 The initializer for the eigth matrix element.
//
// This constructor offers the option to directly initialize a newly created \f$ 1 \times 8 \f$,
// \f$ 2 \times 4 \f$, \f$ 4 \times 2 \f$, and \f$ 8 \times 1 \f$ matrix. The following example
// demonstrates this by creating the matrix

                     \f[\left(\begin{array}{*{4}{c}}
                     1 & 3 & 5 & 7 \\
                     2 & 4 & 6 & 8 \\
                     \end{array}\right)\f]:

   \code
   blaze::StaticMatrix<int,2,4,true> A( 1, 2, 3, 4, 5, 6, 7, 8 );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline StaticMatrix<Type,M,N,true>::StaticMatrix( const Type& v1, const Type& v2, const Type& v3,
                                                  const Type& v4, const Type& v5, const Type& v6,
                                                  const Type& v7, const Type& v8 )
{
   BLAZE_STATIC_ASSERT( M*N == 8UL );
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   // Initialization of a 8x1 matrix
   if( N == 1UL ) {
      v_[0UL] = v1;
      v_[1UL] = v2;
      v_[2UL] = v3;
      v_[3UL] = v4;
      v_[4UL] = v5;
      v_[5UL] = v6;
      v_[6UL] = v7;
      v_[7UL] = v8;
   }

   // Initialization of a 4x2 matrix
   else if( N == 2UL ) {
      v_[   0UL] = v1;
      v_[   1UL] = v2;
      v_[   2UL] = v3;
      v_[   3UL] = v4;
      v_[MM    ] = v5;
      v_[MM+1UL] = v6;
      v_[MM+2UL] = v7;
      v_[MM+3UL] = v8;
   }

   // Initialization of a 2x4 matrix
   else if( N == 4UL ) {
      v_[       0UL] = v1;
      v_[       1UL] = v2;
      v_[    MM    ] = v3;
      v_[    MM+1UL] = v4;
      v_[2UL*MM    ] = v5;
      v_[2UL*MM+1UL] = v6;
      v_[3UL*MM    ] = v7;
      v_[3UL*MM+1UL] = v8;
   }

   // Initialization of a 1x8 matrix
   else {
      v_[   0UL] = v1;
      v_[    MM] = v2;
      v_[2UL*MM] = v3;
      v_[3UL*MM] = v4;
      v_[4UL*MM] = v5;
      v_[5UL*MM] = v6;
      v_[6UL*MM] = v7;
      v_[7UL*MM] = v8;
   }

   if( IsBuiltin<Type>::value ) {
      for( size_t j=0UL; j<N; ++j )
         for( size_t i=M; i<MM; ++i ) {
            v_[i+j*MM] = Type();
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Constructor for \f$ 1 \times 9 \f$, \f$ 3 \times 3 \f$, and \f$ 9 \times 1 \f$ matrices.
//
// \param v1 The initializer for the first matrix element.
// \param v2 The initializer for the second matrix element.
// \param v3 The initializer for the third matrix element.
// \param v4 The initializer for the fourth matrix element.
// \param v5 The initializer for the fifth matrix element.
// \param v6 The initializer for the sixth matrix element.
// \param v7 The initializer for the seventh matrix element.
// \param v8 The initializer for the eigth matrix element.
// \param v9 The initializer for the ninth matrix element.
//
// This constructor offers the option to directly initialize a newly created \f$ 1 \times 9 \f$,
// \f$ 3 \times 3 \f$, and \f$ 9 \times 1 \f$ matrix. The following example demonstrates this by
// creating the matrix

                     \f[\left(\begin{array}{*{3}{c}}
                     1 & 4 & 7 \\
                     2 & 5 & 8 \\
                     3 & 6 & 9 \\
                     \end{array}\right)\f]:

   \code
   blaze::StaticMatrix<int,3,3,true> A( 1, 2, 3, 4, 5, 6, 7, 8, 9 );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline StaticMatrix<Type,M,N,true>::StaticMatrix( const Type& v1, const Type& v2, const Type& v3,
                                                  const Type& v4, const Type& v5, const Type& v6,
                                                  const Type& v7, const Type& v8, const Type& v9 )
{
   BLAZE_STATIC_ASSERT( M*N == 9UL );
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   // Initialization of a 9x1 matrix
   if( N == 1 ) {
      v_[0UL] = v1;
      v_[1UL] = v2;
      v_[2UL] = v3;
      v_[3UL] = v4;
      v_[4UL] = v5;
      v_[5UL] = v6;
      v_[6UL] = v7;
      v_[7UL] = v8;
      v_[8UL] = v9;
   }

   // Initialization of a 3x3 matrix
   else if( N == 3UL ) {
      v_[       0UL] = v1;
      v_[       1UL] = v2;
      v_[       2UL] = v3;
      v_[    MM    ] = v4;
      v_[    MM+1UL] = v5;
      v_[    MM+2UL] = v6;
      v_[2UL*MM    ] = v7;
      v_[2UL*MM+1UL] = v8;
      v_[2UL*MM+2UL] = v9;
   }

   // Initialization of a 1x9 matrix
   else {
      v_[   0UL] = v1;
      v_[    MM] = v2;
      v_[2UL*MM] = v3;
      v_[3UL*MM] = v4;
      v_[4UL*MM] = v5;
      v_[5UL*MM] = v6;
      v_[6UL*MM] = v7;
      v_[7UL*MM] = v8;
      v_[8UL*MM] = v9;
   }

   if( IsBuiltin<Type>::value ) {
      for( size_t j=0UL; j<N; ++j )
         for( size_t i=M; i<MM; ++i ) {
            v_[i+j*MM] = Type();
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Constructor for \f$ 1 \times 10 \f$, \f$ 2 \times 5 \f$, \f$ 5 \times 2 \f$, and
// \f$ 10 \times 1 \f$ matrices.
//
// \param v1 The initializer for the first matrix element.
// \param v2 The initializer for the second matrix element.
// \param v3 The initializer for the third matrix element.
// \param v4 The initializer for the fourth matrix element.
// \param v5 The initializer for the fifth matrix element.
// \param v6 The initializer for the sixth matrix element.
// \param v7 The initializer for the seventh matrix element.
// \param v8 The initializer for the eigth matrix element.
// \param v9 The initializer for the ninth matrix element.
// \param v10 The initializer for the tenth matrix element.
//
// This constructor offers the option to directly initialize a newly created \f$ 1 \times 10 \f$,
// \f$ 2 \times 5 \f$, \f$ 5 \times 2 \f$, and \f$ 10 \times 1 \f$ matrix. The following example
// demonstrates this by creating the matrix

                     \f[\left(\begin{array}{*{5}{c}}
                     1 & 3 & 5 & 7 & 9  \\
                     2 & 4 & 6 & 8 & 10 \\
                     \end{array}\right)\f]:

   \code
   blaze::StaticMatrix<int,2,5,true> A( 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 );
   \endcode
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline StaticMatrix<Type,M,N,true>::StaticMatrix( const Type& v1, const Type& v2, const Type& v3,
                                                  const Type& v4, const Type& v5, const Type& v6,
                                                  const Type& v7, const Type& v8, const Type& v9,
                                                  const Type& v10 )
{
   BLAZE_STATIC_ASSERT( M*N == 10UL );
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   // Initialization of a 10x1 matrix
   if( N == 1UL ) {
      v_[0UL] = v1;
      v_[1UL] = v2;
      v_[2UL] = v3;
      v_[3UL] = v4;
      v_[4UL] = v5;
      v_[5UL] = v6;
      v_[6UL] = v7;
      v_[7UL] = v8;
      v_[8UL] = v9;
      v_[9UL] = v10;
   }

   // Initialization of a 5x2 matrix
   else if( N == 2UL ) {
      v_[   0UL] = v1;
      v_[   1UL] = v2;
      v_[   2UL] = v3;
      v_[   3UL] = v4;
      v_[   4UL] = v5;
      v_[MM    ] = v6;
      v_[MM+1UL] = v7;
      v_[MM+2UL] = v8;
      v_[MM+3UL] = v9;
      v_[MM+4UL] = v10;
   }

   // Initialization of a 2x5 matrix
   else if( N == 5UL ) {
      v_[       0UL] = v1;
      v_[       1UL] = v2;
      v_[    MM    ] = v3;
      v_[    MM+1UL] = v4;
      v_[2UL*MM    ] = v5;
      v_[2UL*MM+1UL] = v6;
      v_[3UL*MM    ] = v7;
      v_[3UL*MM+1UL] = v8;
      v_[4UL*MM    ] = v9;
      v_[4UL*MM+1UL] = v10;
   }

   // Initialization of a 1x10 matrix
   else {
      v_[   0UL] = v1;
      v_[    MM] = v2;
      v_[2UL*MM] = v3;
      v_[3UL*MM] = v4;
      v_[4UL*MM] = v5;
      v_[5UL*MM] = v6;
      v_[6UL*MM] = v7;
      v_[7UL*MM] = v8;
      v_[8UL*MM] = v9;
      v_[9UL*MM] = v10;
   }

   if( IsBuiltin<Type>::value ) {
      for( size_t j=0UL; j<N; ++j )
         for( size_t i=M; i<MM; ++i ) {
            v_[i+j*MM] = Type();
      }
   }
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
/*!\brief 2D-access to the matrix elements.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \return Reference to the accessed value.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline Type& StaticMatrix<Type,M,N,true>::operator()( size_t i, size_t j )
{
   BLAZE_USER_ASSERT( i<M, "Invalid row access index"    );
   BLAZE_USER_ASSERT( j<N, "Invalid column access index" );
   return v_[i+j*MM];
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief 2D-access to the matrix elements.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \return Reference-to-const to the accessed value.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline const Type& StaticMatrix<Type,M,N,true>::operator()( size_t i, size_t j ) const
{
   BLAZE_USER_ASSERT( i<M, "Invalid row access index"    );
   BLAZE_USER_ASSERT( j<N, "Invalid column access index" );
   return v_[i+j*MM];
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Low-level data access to the matrix elements.
//
// \return Pointer to the internal element storage.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline Type* StaticMatrix<Type,M,N,true>::data()
{
   return v_;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Low-level data access to the matrix elements.
//
// \return Pointer to the internal element storage.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline const Type* StaticMatrix<Type,M,N,true>::data() const
{
   return v_;
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
/*!\brief Homogenous assignment to all matrix elements.
//
// \param set Scalar value to be assigned to all matrix elements.
// \return Reference to the assigned matrix.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline StaticMatrix<Type,M,N,true>&
   StaticMatrix<Type,M,N,true>::operator=( const Type& set )
{
   for( size_t j=0UL; j<N; ++j )
      for( size_t i=0UL; i<M; ++i )
         v_[i+j*MM] = set;

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Copy assignment operator for StaticMatrix.
//
// \param rhs Matrix to be copied.
// \return Reference to the assigned matrix.
//
// Explicit definition of a copy assignment operator for performance reasons.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline StaticMatrix<Type,M,N,true>&
   StaticMatrix<Type,M,N,true>::operator=( const StaticMatrix& rhs )
{
   for( size_t j=0UL; j<N; ++j )
      for( size_t i=0UL; i<M; ++i )
         v_[i+j*MM] = rhs(i,j);

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Assignment operator for different StaticMatrix instances.
//
// \param rhs Matrix to be copied.
// \return Reference to the assigned matrix.
*/
template< typename Type   // Data type of the matrix
        , size_t M        // Number of rows
        , size_t N >      // Number of columns
template< typename Other  // Data type of the foreign matrix
        , bool SO >       // Storage order of the foreign matrix
inline StaticMatrix<Type,M,N,true>&
   StaticMatrix<Type,M,N,true>::operator=( const StaticMatrix<Other,M,N,SO>& rhs )
{
   for( size_t j=0UL; j<N; ++j )
      for( size_t i=0UL; i<M; ++i )
         v_[i+j*MM] = rhs(i,j);

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Assignment operator for different matrices.
//
// \param rhs Matrix to be copied.
// \return Reference to the assigned matrix.
// \exception std::invalid_argument Invalid assignment to static matrix.
//
// This constructor initializes the matrix as a copy of the given matrix. In case the
// number of rows of the given matrix is not M or the number of columns is not N, a
// \a std::invalid_argument exception is thrown.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT    // Type of the right-hand side matrix
        , bool SO >      // Storage order of the right-hand side matrix
inline StaticMatrix<Type,M,N,true>& StaticMatrix<Type,M,N,true>::operator=( const Matrix<MT,SO>& rhs )
{
   using blaze::assign;

   if( (~rhs).rows() != M || (~rhs).columns() != N )
      throw std::invalid_argument( "Invalid assignment to static matrix" );

   if( CanAlias<MT>::value && (~rhs).isAliased( this ) ) {
      StaticMatrix tmp( ~rhs );
      swap( tmp );
   }
   else {
      if( IsSparseMatrix<MT>::value )
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
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT    // Type of the right-hand side matrix
        , bool SO >      // Storage order of the right-hand side matrix
inline StaticMatrix<Type,M,N,true>& StaticMatrix<Type,M,N,true>::operator+=( const Matrix<MT,SO>& rhs )
{
   using blaze::addAssign;

   if( (~rhs).rows() != M || (~rhs).columns() != N )
      throw std::invalid_argument( "Matrix sizes do not match" );

   if( CanAlias<MT>::value && (~rhs).isAliased( this ) ) {
      StaticMatrix tmp( ~rhs );
      addAssign( *this, tmp );
   }
   else {
      addAssign( *this, ~rhs );
   }

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
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT    // Type of the right-hand side matrix
        , bool SO >      // Storage order of the right-hand side matrix
inline StaticMatrix<Type,M,N,true>& StaticMatrix<Type,M,N,true>::operator-=( const Matrix<MT,SO>& rhs )
{
   using blaze::subAssign;

   if( (~rhs).rows() != M || (~rhs).columns() != N )
      throw std::invalid_argument( "Matrix sizes do not match" );

   if( CanAlias<MT>::value && (~rhs).isAliased( this ) ) {
      StaticMatrix tmp( ~rhs );
      subAssign( *this, tmp );
   }
   else {
      subAssign( *this, ~rhs );
   }

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
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT    // Type of the right-hand side matrix
        , bool SO >      // Storage order of the right-hand side matrix
inline StaticMatrix<Type,M,N,true>& StaticMatrix<Type,M,N,true>::operator*=( const Matrix<MT,SO>& rhs )
{
   if( M != N || (~rhs).rows() != M || (~rhs).columns() != M )
      throw std::invalid_argument( "Matrix sizes do not match" );

   StaticMatrix tmp( *this * (~rhs) );
   return this->operator=( tmp );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Multiplication assignment operator for the multiplication between a matrix and
//        a scalar value (\f$ A*=s \f$).
//
// \param rhs The right-hand side scalar value for the multiplication.
// \return Reference to the matrix.
*/
template< typename Type     // Data type of the matrix
        , size_t M          // Number of rows
        , size_t N >        // Number of columns
template< typename Other >  // Data type of the right-hand side scalar
inline typename EnableIf< IsNumeric<Other>, StaticMatrix<Type,M,N,true> >::Type&
   StaticMatrix<Type,M,N,true>::operator*=( Other rhs )
{
   for( size_t j=0UL; j<N; ++j )
      for( size_t i=0UL; i<M; ++i )
         v_[i+j*MM] *= rhs;

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Division assignment operator for the division of a matrix by a scalar value
//        (\f$ A/=s \f$).
//
// \param rhs The right-hand side scalar value for the division.
// \return Reference to the matrix.
//
// \b Note: A division by zero is only checked by an user assert.
*/
template< typename Type     // Data type of the matrix
        , size_t M          // Number of rows
        , size_t N >        // Number of columns
template< typename Other >  // Data type of the right-hand side scalar
inline typename EnableIf< IsNumeric<Other>, StaticMatrix<Type,M,N,true> >::Type&
   StaticMatrix<Type,M,N,true>::operator/=( Other rhs )
{
   BLAZE_USER_ASSERT( rhs != Other(0), "Division by zero detected" );

   typedef typename MathTrait<Type,Other>::DivType  DT;
   typedef typename If< IsNumeric<DT>, DT, Other >::Type  Tmp;

   // Depending on the two involved data types, an integer division is applied or a
   // floating point division is selected.
   if( IsNumeric<DT>::value && IsFloatingPoint<DT>::value ) {
      const Tmp tmp( Tmp(1)/static_cast<Tmp>( rhs ) );
      for( size_t j=0UL; j<N; ++j )
         for( size_t i=0UL; i<M; ++i )
            v_[i+j*MM] *= tmp;
   }
   else {
      for( size_t j=0UL; j<N; ++j )
         for( size_t i=0UL; i<M; ++i )
            v_[i+j*MM] /= rhs;
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
/*!\brief Returns the current number of rows of the matrix.
//
// \return The number of rows of the matrix.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline size_t StaticMatrix<Type,M,N,true>::rows() const
{
   return M;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the current number of columns of the matrix.
//
// \return The number of columns of the matrix.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline size_t StaticMatrix<Type,M,N,true>::columns() const
{
   return N;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the spacing between the beginning of two columns.
//
// \return The spacing between the beginning of two columns.
//
// This function returns the spacing between the beginning of two column, i.e. the total number
// of elements of a column.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline size_t StaticMatrix<Type,M,N,true>::spacing() const
{
   return MM;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the total number of non-zero elements in the matrix
//
// \return The number of non-zero elements in the dense matrix.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline size_t StaticMatrix<Type,M,N,true>::nonZeros() const
{
   size_t nonzeros( 0UL );

   for( size_t j=0UL; j<N; ++j )
      for( size_t i=0UL; i<M; ++i )
         if( !isDefault( v_[i+j*MM] ) )
            ++nonzeros;

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
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline size_t StaticMatrix<Type,M,N,true>::nonZeros( size_t j ) const
{
   BLAZE_USER_ASSERT( j < columns(), "Invalid column access index" );

   const size_t end( (j+1UL)*MM );
   size_t nonzeros( 0UL );

   for( size_t i=j*MM; i<end; ++i )
      if( !isDefault( v_[i] ) )
         ++nonzeros;

   return nonzeros;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Reset to the default initial values.
//
// \return void
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline void StaticMatrix<Type,M,N,true>::reset()
{
   using blaze::reset;

   for( size_t j=0UL; j<N; ++j )
      for( size_t i=0UL; i<M; ++i )
         reset( v_[i+j*MM] );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Transposing the matrix.
//
// \return Reference to the transposed matrix.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline StaticMatrix<Type,M,N,true>& StaticMatrix<Type,M,N,true>::transpose()
{
   using std::swap;

   for( size_t j=1UL; j<N; ++j )
      for( size_t i=0UL; i<j; ++i )
         swap( v_[i+j*MM], v_[j+i*MM] );

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
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline bool StaticMatrix<Type,M,N,true>::isDiagonal() const
{
   if( M != N ) return false;

   for( size_t j=1UL; j<N; ++j ) {
      for( size_t i=0UL; i<j; ++i ) {
         if( !isDefault( v_[i+j*MM] ) || !isDefault( v_[j+i*MM] ) )
            return false;
      }
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
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline bool StaticMatrix<Type,M,N,true>::isSymmetric() const
{
   if( M != N ) return false;

   for( size_t j=1; j<N; ++j ) {
      for( size_t i=0; i<j; ++i ) {
         if( !equal( v_[i+j*MM], v_[j+i*MM] ) )
            return false;
      }
   }

   return true;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Scaling of the matrix by the scalar value \a scalar (\f$ A*=s \f$).
//
// \param scalar The scalar value for the matrix scaling.
// \return Reference to the matrix.
*/
template< typename Type     // Data type of the matrix
        , size_t M          // Number of rows
        , size_t N >        // Number of columns
template< typename Other >  // Data type of the scalar value
inline StaticMatrix<Type,M,N,true>&
   StaticMatrix<Type,M,N,true>::scale( const Other& scalar )
{
   for( size_t j=0UL; j<N; ++j )
      for( size_t i=0UL; i<M; ++i )
         v_[i+j*MM] *= scalar;

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Swapping the contents of two static matrices.
//
// \param m The matrix to be swapped.
// \return void
// \exception no-throw guarantee.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline void StaticMatrix<Type,M,N,true>::swap( StaticMatrix& m ) /* throw() */
{
   using std::swap;

   for( size_t j=0UL; j<N; ++j ) {
      for( size_t i=0UL; i<M; ++i ) {
         swap( v_[i+j*MM], m(i,j) );
      }
   }
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
template< typename Type     // Data type of the matrix
        , size_t M          // Number of rows
        , size_t N >        // Number of columns
template< typename Other >  // Data type of the foreign expression
inline bool StaticMatrix<Type,M,N,true>::isAliased( const Other* alias ) const
{
   return static_cast<const void*>( this ) == static_cast<const void*>( alias );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Access to the intrinsic elements of the matrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \return Reference to the accessed values.
//
// This function offers a direct access to the intrinsic elements of the matrix. It must \b NOT
// be called explicitly! It is used internally for the performance optimized evaluation of
// expression templates. Calling this function explicitly might result in erroneous results
// and/or in compilation errors.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
inline typename StaticMatrix<Type,M,N,true>::IntrinsicType
   StaticMatrix<Type,M,N,true>::get( size_t i, size_t j ) const
{
   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( i            <  M  , "Invalid row access index"    );
   BLAZE_INTERNAL_ASSERT( i + IT::size <= MM , "Invalid row access index"    );
   BLAZE_INTERNAL_ASSERT( i % IT::size == 0UL, "Invalid row access index"    );
   BLAZE_INTERNAL_ASSERT( j            <  N  , "Invalid column access index" );

   return load( &v_[i+j*MM] );
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
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO >      // Storage order of the right-hand side dense matrix
inline typename DisableIf< typename StaticMatrix<Type,M,N,true>::BLAZE_TEMPLATE VectorizedAssign<MT> >::Type
   StaticMatrix<Type,M,N,true>::assign( const DenseMatrix<MT,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t j=0UL; j<N; ++j ) {
      for( size_t i=0UL; i<M; ++i ) {
         v_[i+j*MM] = (~rhs)(i,j);
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Intrinsic optimized implementation of the assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO >      // Storage order of the right-hand side dense matrix
inline typename EnableIf< typename StaticMatrix<Type,M,N,true>::BLAZE_TEMPLATE VectorizedAssign<MT> >::Type
   StaticMatrix<Type,M,N,true>::assign( const DenseMatrix<MT,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   for( size_t j=0UL; j<N; ++j ) {
      for( size_t i=0UL; i<M; i+=IT::size ) {
         store( &v_[i+j*MM], (~rhs).get(i,j) );
      }
   }
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
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,true>::assign( const SparseMatrix<MT,true>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   typedef typename MT::ConstIterator  ConstIterator;

   for( size_t j=0UL; j<N; ++j )
      for( ConstIterator element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
         v_[element->index()+j*MM] = element->value();
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
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,true>::assign( const SparseMatrix<MT,false>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   typedef typename MT::ConstIterator  ConstIterator;

   for( size_t i=0UL; i<M; ++i )
      for( ConstIterator element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
         v_[i+element->index()*MM] = element->value();
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
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO >      // Storage order of the right-hand side dense matrix
inline typename DisableIf< typename StaticMatrix<Type,M,N,true>::BLAZE_TEMPLATE VectorizedAddAssign<MT> >::Type
   StaticMatrix<Type,M,N,true>::addAssign( const DenseMatrix<MT,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t j=0UL; j<N; ++j ) {
      for( size_t i=0UL; i<M; ++i ) {
         v_[i+j*MM] += (~rhs)(i,j);
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Intrinsic optimized implementation of the addition assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO >      // Storage order of the right-hand side dense matrix
inline typename EnableIf< typename StaticMatrix<Type,M,N,true>::BLAZE_TEMPLATE VectorizedAddAssign<MT> >::Type
   StaticMatrix<Type,M,N,true>::addAssign( const DenseMatrix<MT,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   for( size_t j=0UL; j<N; ++j ) {
      for( size_t i=0UL; i<M; i+=IT::size ) {
         store( &v_[i+j*MM], load( &v_[i+j*MM] ) + (~rhs).get(i,j) );
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the addition assignment of a column-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,true>::addAssign( const SparseMatrix<MT,true>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   typedef typename MT::ConstIterator  ConstIterator;

   for( size_t j=0UL; j<N; ++j )
      for( ConstIterator element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
         v_[element->index()+j*MM] += element->value();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the addition assignment of a row-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,true>::addAssign( const SparseMatrix<MT,false>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   typedef typename MT::ConstIterator  ConstIterator;

   for( size_t i=0UL; i<M; ++i )
      for( ConstIterator element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
         v_[i+element->index()*MM] += element->value();
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
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO >      // Storage order of the right-hand side dense matrix
inline typename DisableIf< typename StaticMatrix<Type,M,N,true>::BLAZE_TEMPLATE VectorizedSubAssign<MT> >::Type
   StaticMatrix<Type,M,N,true>::subAssign( const DenseMatrix<MT,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   for( size_t j=0UL; j<N; ++j ) {
      for( size_t i=0UL; i<M; ++i ) {
         v_[i+j*MM] -= (~rhs)(i,j);
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Intrinsic optimized implementation of the subtraction assignment of a dense matrix.
//
// \param rhs The right-hand side dense matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT    // Type of the right-hand side dense matrix
        , bool SO >      // Storage order of the right-hand side dense matrix
inline typename EnableIf< typename StaticMatrix<Type,M,N,true>::BLAZE_TEMPLATE VectorizedSubAssign<MT> >::Type
   StaticMatrix<Type,M,N,true>::subAssign( const DenseMatrix<MT,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   for( size_t j=0UL; j<N; ++j ) {
      for( size_t i=0UL; i<M; i+=IT::size ) {
         store( &v_[i+j*MM], load( &v_[i+j*MM] ) - (~rhs).get(i,j) );
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the subtraction assignment of a column-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,true>::subAssign( const SparseMatrix<MT,true>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   typedef typename MT::ConstIterator  ConstIterator;

   for( size_t j=0UL; j<N; ++j )
      for( ConstIterator element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
         v_[element->index()+j*MM] -= element->value();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the subtraction assignment of a row-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N >     // Number of columns
template< typename MT >  // Type of the right-hand side sparse matrix
inline void StaticMatrix<Type,M,N,true>::subAssign( const SparseMatrix<MT,false>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).rows() == M && (~rhs).columns() == N, "Invalid matrix size" );

   typedef typename MT::ConstIterator  ConstIterator;

   for( size_t i=0UL; i<M; ++i )
      for( ConstIterator element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
         v_[i+element->index()*MM] -= element->value();
}
/*! \endcond */
//*************************************************************************************************








//=================================================================================================
//
//  UNDEFINED CLASS TEMPLATE SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Specialization of StaticMatrix for zero columns.
// \ingroup static_matrix
//
// This specialization of the StaticMatrix class template is left undefined and therefore
// prevents the instantiation for zero columns.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , bool SO >      // Storage order
class StaticMatrix<Type,M,0UL,SO>;
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Specialization of StaticMatrix for zero rows.
// \ingroup static_matrix
//
// This specialization of the StaticMatrix class template is left undefined and therefore
// prevents the instantiation for zero rows.
*/
template< typename Type  // Data type of the matrix
        , size_t N       // Number of columns
        , bool SO >      // Storage order
class StaticMatrix<Type,0UL,N,SO>;
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Specialization of StaticMatrix for 0 rows and 0 columns.
// \ingroup static_matrix
//
// This specialization of the StaticMatrix class template is left undefined and therefore
// prevents the instantiation for 0 rows and 0 columns.
*/
template< typename Type  // Data type of the matrix
        , bool SO >      // Storage order
class StaticMatrix<Type,0UL,0UL,SO>;
/*! \endcond */
//*************************************************************************************************








//=================================================================================================
//
//  GLOBAL OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*!\name StaticMatrix operators */
//@{
template< typename Type, size_t M, size_t N, bool SO >
std::ostream& operator<<( std::ostream& os, const StaticMatrix<Type,M,N,SO>& m );

template< typename Type, size_t M, size_t N, bool SO >
inline bool isnan( const StaticMatrix<Type,M,N,SO>& m );

template< typename Type, size_t M, size_t N, bool SO >
inline void reset( StaticMatrix<Type,M,N,SO>& m );

template< typename Type, size_t M, size_t N, bool SO >
inline void clear( StaticMatrix<Type,M,N,SO>& m );

template< typename Type, size_t M, size_t N, bool SO >
inline bool isDefault( const StaticMatrix<Type,M,N,SO>& m );

template< typename Type, size_t M, size_t N, bool SO >
inline const DMatDMatMultExpr< StaticMatrix<Type,M,N,SO>, StaticMatrix<Type,M,N,SO> >
   sq( const StaticMatrix<Type,M,N,SO>& m );

template< typename Type, size_t M, size_t N, bool SO >
inline void swap( StaticMatrix<Type,M,N,SO>& a, StaticMatrix<Type,M,N,SO>& b ) /* throw() */;
//@}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Global output operator for static matrices.
// \ingroup static_matrix
//
// \param os Reference to the output stream.
// \param m Reference to a constant matrix object.
// \return Reference to the output stream.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
std::ostream& operator<<( std::ostream& os, const StaticMatrix<Type,M,N,SO>& m )
{
   for( size_t i=0UL; i<M; ++i ) {
      os << "( ";
      for( size_t j=0UL; j<N; ++j )
         os << m(i,j) << " ";
      os << ")\n";
   }

   return os;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checks the given matrix for not-a-number elements.
// \ingroup static_matrix
//
// \param m The matrix to be checked for not-a-number elements.
// \return \a true if at least one element of the matrix is not-a-number, \a false otherwise.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline bool isnan( const StaticMatrix<Type,M,N,SO>& m )
{
   for( size_t i=0UL; i<M*N; ++i ) {
      if( isnan( m[i] ) )
         return true;
   }

   return false;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Resetting the given static matrix.
// \ingroup static_matrix
//
// \param m The matrix to be resetted.
// \return void
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline void reset( StaticMatrix<Type,M,N,SO>& m )
{
   m.reset();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Clearing the given static matrix.
// \ingroup static_matrix
//
// \param m The matrix to be cleared.
// \return void
//
// Clearing a static matrix is equivalent to resetting it via the reset() function.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline void clear( StaticMatrix<Type,M,N,SO>& m )
{
   m.reset();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns whether the given static matrix is in default state.
// \ingroup static_matrix
//
// \param m The matrix to be tested for its default state.
// \return \a true in case the given matrix is component-wise zero, \a false otherwise.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline bool isDefault( const StaticMatrix<Type,M,N,SO>& m )
{
   for( size_t i=0UL; i<M*N; ++i ) {
      if( !isDefault( m[i] ) )
         return false;
   }

   return true;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Squaring the given 2x2 matrix.
// \ingroup static_matrix
//
// \param m The 2x2 matrix to be squared.
// \return The result of the square operation.
//
// This function squares the given 2x2 matrix \a m. This function has the same effect as
// multiplying the matrix with itself (\f$ m * m \f$).
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline const DMatDMatMultExpr< StaticMatrix<Type,M,N,SO>, StaticMatrix<Type,M,N,SO> >
   sq( const StaticMatrix<Type,M,N,SO>& m )
{
   return m * m;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Swapping the contents of two static matrices.
// \ingroup static_matrix
//
// \param a The first matrix to be swapped.
// \param b The second matrix to be swapped.
// \return void
// \exception no-throw guarantee.
*/
template< typename Type  // Data type of the matrix
        , size_t M       // Number of rows
        , size_t N       // Number of columns
        , bool SO >      // Storage order
inline void swap( StaticMatrix<Type,M,N,SO>& a, StaticMatrix<Type,M,N,SO>& b ) /* throw() */
{
   a.swap( b );
}
//*************************************************************************************************




//=================================================================================================
//
//  MATHTRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T1, size_t M, size_t N, bool SO, typename T2 >
struct MathTrait< StaticMatrix<T1,M,N,SO>, T2 >
{
   typedef INVALID_TYPE                                                   HighType;
   typedef INVALID_TYPE                                                   LowType;
   typedef INVALID_TYPE                                                   AddType;
   typedef INVALID_TYPE                                                   SubType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::MultType, M, N, SO >  MultType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::DivType , M, N, SO >  DivType;
   BLAZE_CONSTRAINT_MUST_BE_NUMERIC_TYPE( T2 );
};

template< typename T1, typename T2, size_t M, size_t N, bool SO >
struct MathTrait< T1, StaticMatrix<T2,M,N,SO> >
{
   typedef INVALID_TYPE                                                   HighType;
   typedef INVALID_TYPE                                                   LowType;
   typedef INVALID_TYPE                                                   AddType;
   typedef INVALID_TYPE                                                   SubType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::MultType, M, N, SO >  MultType;
   typedef INVALID_TYPE                                                   DivType;
   BLAZE_CONSTRAINT_MUST_BE_NUMERIC_TYPE( T1 );
};

template< typename T1, size_t M, size_t N, bool SO, typename T2 >
struct MathTrait< StaticMatrix<T1,M,N,SO>, StaticVector<T2,N,false> >
{
   typedef INVALID_TYPE                                                   HighType;
   typedef INVALID_TYPE                                                   LowType;
   typedef INVALID_TYPE                                                   AddType;
   typedef INVALID_TYPE                                                   SubType;
   typedef StaticVector< typename MathTrait<T1,T2>::MultType, M, false >  MultType;
   typedef INVALID_TYPE                                                   DivType;
};

template< typename T1, size_t M, typename T2, size_t N, bool SO >
struct MathTrait< StaticVector<T1,M,true>, StaticMatrix<T2,M,N,SO> >
{
   typedef INVALID_TYPE                                                  HighType;
   typedef INVALID_TYPE                                                  LowType;
   typedef INVALID_TYPE                                                  AddType;
   typedef INVALID_TYPE                                                  SubType;
   typedef StaticVector< typename MathTrait<T1,T2>::MultType, N, true >  MultType;
   typedef INVALID_TYPE                                                  DivType;
};

template< typename T1, size_t M, size_t N, bool SO, typename T2 >
struct MathTrait< StaticMatrix<T1,M,N,SO>, DynamicVector<T2,false> >
{
   typedef INVALID_TYPE                                                   HighType;
   typedef INVALID_TYPE                                                   LowType;
   typedef INVALID_TYPE                                                   AddType;
   typedef INVALID_TYPE                                                   SubType;
   typedef StaticVector< typename MathTrait<T1,T2>::MultType, M, false >  MultType;
   typedef INVALID_TYPE                                                   DivType;
};

template< typename T1, typename T2, size_t M, size_t N, bool SO >
struct MathTrait< DynamicVector<T1,true>, StaticMatrix<T2,M,N,SO> >
{
   typedef INVALID_TYPE                                                  HighType;
   typedef INVALID_TYPE                                                  LowType;
   typedef INVALID_TYPE                                                  AddType;
   typedef INVALID_TYPE                                                  SubType;
   typedef StaticVector< typename MathTrait<T1,T2>::MultType, N, true >  MultType;
   typedef INVALID_TYPE                                                  DivType;
};

template< typename T1, size_t M, size_t N, bool SO, typename T2 >
struct MathTrait< StaticMatrix<T1,M,N,SO>, CompressedVector<T2,false> >
{
   typedef INVALID_TYPE                                                   HighType;
   typedef INVALID_TYPE                                                   LowType;
   typedef INVALID_TYPE                                                   AddType;
   typedef INVALID_TYPE                                                   SubType;
   typedef StaticVector< typename MathTrait<T1,T2>::MultType, M, false >  MultType;
   typedef INVALID_TYPE                                                   DivType;
};

template< typename T1, typename T2, size_t M, size_t N, bool SO >
struct MathTrait< CompressedVector<T1,true>, StaticMatrix<T2,M,N,SO> >
{
   typedef INVALID_TYPE                                                  HighType;
   typedef INVALID_TYPE                                                  LowType;
   typedef INVALID_TYPE                                                  AddType;
   typedef INVALID_TYPE                                                  SubType;
   typedef StaticVector< typename MathTrait<T1,T2>::MultType, N, true >  MultType;
   typedef INVALID_TYPE                                                  DivType;
};

template< typename T1, size_t N, bool SO, typename T2 >
struct MathTrait< StaticMatrix<T1,N,N,SO>, StaticMatrix<T2,N,N,SO> >
{
   typedef StaticMatrix< typename MathTrait<T1,T2>::HighType, N, N, SO >  HighType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::LowType , N, N, SO >  LowType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::AddType , N, N, SO >  AddType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::SubType , N, N, SO >  SubType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::MultType, N, N, SO >  MultType;
   typedef INVALID_TYPE                                                   DivType;
};

template< typename T1, size_t N, bool SO1, typename T2, bool SO2 >
struct MathTrait< StaticMatrix<T1,N,N,SO1>, StaticMatrix<T2,N,N,SO2> >
{
   typedef StaticMatrix< typename MathTrait<T1,T2>::HighType, N, N, SO1 >    HighType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::LowType , N, N, SO1 >    LowType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::AddType , N, N, false >  AddType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::SubType , N, N, false >  SubType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::MultType, N, N, SO1 >    MultType;
   typedef INVALID_TYPE                                                      DivType;
};

template< typename T1, size_t M, size_t N, bool SO, typename T2 >
struct MathTrait< StaticMatrix<T1,M,N,SO>, StaticMatrix<T2,M,N,SO> >
{
   typedef StaticMatrix< typename MathTrait<T1,T2>::HighType, M, N, SO >  HighType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::LowType , M, N, SO >  LowType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::AddType , M, N, SO >  AddType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::SubType , M, N, SO >  SubType;
   typedef INVALID_TYPE                                                   MultType;
   typedef INVALID_TYPE                                                   DivType;
};

template< typename T1, size_t M, size_t N, bool SO1, typename T2, bool SO2 >
struct MathTrait< StaticMatrix<T1,M,N,SO1>, StaticMatrix<T2,M,N,SO2> >
{
   typedef StaticMatrix< typename MathTrait<T1,T2>::HighType, M, N, SO1 >    HighType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::LowType , M, N, SO1 >    LowType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::AddType , M, N, false >  AddType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::SubType , M, N, false >  SubType;
   typedef INVALID_TYPE                                                      MultType;
   typedef INVALID_TYPE                                                      DivType;
};

template< typename T1, size_t M, size_t K, bool SO1, typename T2, size_t N, bool SO2 >
struct MathTrait< StaticMatrix<T1,M,K,SO1>, StaticMatrix<T2,K,N,SO2> >
{
   typedef INVALID_TYPE                                                    HighType;
   typedef INVALID_TYPE                                                    LowType;
   typedef INVALID_TYPE                                                    AddType;
   typedef INVALID_TYPE                                                    SubType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::MultType, M, N, SO1 >  MultType;
   typedef INVALID_TYPE                                                    DivType;
};
/*! \endcond */
//*************************************************************************************************

} // namespace blaze

#endif
