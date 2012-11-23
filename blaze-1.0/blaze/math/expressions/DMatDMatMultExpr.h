//=================================================================================================
/*!
//  \file blaze/math/expressions/DMatDMatMultExpr.h
//  \brief Header file for the dense matrix/dense matrix multiplication expression
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

#ifndef _BLAZE_MATH_EXPRESSIONS_DMATDMATMULTEXPR_H_
#define _BLAZE_MATH_EXPRESSIONS_DMATDMATMULTEXPR_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <stdexcept>
#include <boost/cast.hpp>
#include <blaze/math/constraints/DenseMatrix.h>
#include <blaze/math/constraints/DenseVector.h>
#include <blaze/math/constraints/SparseVector.h>
#include <blaze/math/constraints/StorageOrder.h>
#include <blaze/math/constraints/TransposeFlag.h>
#include <blaze/math/Expression.h>
#include <blaze/math/expressions/DenseMatrix.h>
#include <blaze/math/expressions/DenseVector.h>
#include <blaze/math/expressions/SparseVector.h>
#include <blaze/math/expressions/Forward.h>
#include <blaze/math/Intrinsics.h>
#include <blaze/math/MathTrait.h>
#include <blaze/math/shims/Reset.h>
#include <blaze/math/traits/DMatDVecMultTrait.h>
#include <blaze/math/traits/DMatSVecMultTrait.h>
#include <blaze/math/traits/TDVecDMatMultTrait.h>
#include <blaze/math/traits/TSVecDMatMultTrait.h>
#include <blaze/math/typetraits/IsExpression.h>
#include <blaze/math/typetraits/IsResizable.h>
#include <blaze/math/typetraits/IsRowMajorMatrix.h>
#include <blaze/system/BLAS.h>
#include <blaze/system/Thresholds.h>
#include <blaze/util/Assert.h>
#include <blaze/util/Complex.h>
#include <blaze/util/constraints/Complex.h>
#include <blaze/util/constraints/Double.h>
#include <blaze/util/constraints/Float.h>
#include <blaze/util/constraints/Numeric.h>
#include <blaze/util/constraints/Reference.h>
#include <blaze/util/DisableIf.h>
#include <blaze/util/EnableIf.h>
#include <blaze/util/SelectType.h>
#include <blaze/util/Types.h>
#include <blaze/util/typetraits/IsComplex.h>
#include <blaze/util/typetraits/IsDouble.h>
#include <blaze/util/typetraits/IsFloat.h>
#include <blaze/util/typetraits/IsNumeric.h>
#include <blaze/util/typetraits/IsSame.h>


namespace blaze {

//=================================================================================================
//
//  CLASS DMATDMATMULTEXPR
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Expression object for dense matrix-dense matrix multiplications.
// \ingroup dense_matrix_expression
//
// The DMatDMatMultExpr class represents the compile time expression for multiplications between
// row-major dense matrices.
*/
template< typename MT1    // Type of the left-hand side dense matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
class DMatDMatMultExpr : public DenseMatrix< DMatDMatMultExpr<MT1,MT2>, false >
                       , private Expression
{
 private:
   //**Type definitions****************************************************************************
   typedef typename MT1::ResultType     RT1;  //!< Result type of the left-hand side dense matrix expression.
   typedef typename MT2::ResultType     RT2;  //!< Result type of the right-hand side dense matrix expression.
   typedef typename MT1::CompositeType  CT1;  //!< Composite type of the left-hand side dense matrix expression.
   typedef typename MT2::CompositeType  CT2;  //!< Composite type of the right-hand side dense matrix expression.
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   /*! In case the data type of all three involved matrices is \a float and the single precision
       kernel can be used, the nested \a value will be set to 1, otherwise it will be 0. */
   template< typename T1, typename T2, typename T3 >
   struct UseSinglePrecisionKernel {
      enum { value = IsFloat<typename T1::ElementType>::value &&
                     IsFloat<typename T2::ElementType>::value &&
                     IsFloat<typename T3::ElementType>::value };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   /*! In case the data type of all three involved matrices is \a double and the double precision
       kernel can be used, the nested \a value will be set to 1, otherwise it will be 0. */
   template< typename T1, typename T2, typename T3 >
   struct UseDoublePrecisionKernel {
      enum { value = IsDouble<typename T1::ElementType>::value &&
                     IsDouble<typename T2::ElementType>::value &&
                     IsDouble<typename T3::ElementType>::value };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   /*! In case the data type of all three involved matrices is \a complex<float> and the single
       precision complex kernel can be used, the nested \a value will be set to 1, otherwise it
       will be 0. */
   template< typename T1, typename T2, typename T3 >
   struct UseSinglePrecisionComplexKernel {
      typedef complex<float>  Type;
      enum { value = IsSame<typename T1::ElementType,Type>::value &&
                     IsSame<typename T2::ElementType,Type>::value &&
                     IsSame<typename T3::ElementType,Type>::value };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   /*! In case the data type of all three involved matrices is \a complex<double> and the double
       precision complex kernel can be used, the nested \a value will be set to 1, otherwise it
       will be 0. */
   template< typename T1, typename T2, typename T3 >
   struct UseDoublePrecisionComplexKernel {
      typedef complex<double>  Type;
      enum { value = IsSame<typename T1::ElementType,Type>::value &&
                     IsSame<typename T2::ElementType,Type>::value &&
                     IsSame<typename T3::ElementType,Type>::value };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   /*! In case no optimized BLAS kernel can be used, the nested \a value will be set to 1,
       otherwise it will be 0. */
   template< typename T1, typename T2, typename T3 >
   struct UseDefaultKernel {
      enum { value = !BLAZE_BLAS_MODE || ( !UseSinglePrecisionKernel<T1,T2,T3>::value &&
                                           !UseDoublePrecisionKernel<T1,T2,T3>::value &&
                                           !UseSinglePrecisionComplexKernel<T1,T2,T3>::value &&
                                           !UseDoublePrecisionComplexKernel<T1,T2,T3>::value ) };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   /*! In case all three involved data types are suited for a vectorized computation of the
       matrix multiplication, the nested \value will be set to 1, otherwise it will be 0. */
   template< typename T1, typename T2, typename T3 >
   struct UseVectorizedDefaultKernel {
      enum { value = T1::vectorizable && T2::vectorizable && T3::vectorizable &&
                     IsSame<typename T1::ElementType,typename T2::ElementType>::value &&
                     IsSame<typename T1::ElementType,typename T3::ElementType>::value &&
                     IntrinsicTrait<typename T1::ElementType>::addition &&
                     IntrinsicTrait<typename T1::ElementType>::multiplication };
   };
   /*! \endcond */
   //**********************************************************************************************

 public:
   //**Type definitions****************************************************************************
   typedef DMatDMatMultExpr<MT1,MT2>                   This;           //!< Type of this DMatDMatMultExpr instance.
   typedef typename MathTrait<RT1,RT2>::MultType       ResultType;     //!< Result type for expression template evaluations.
   typedef typename ResultType::OppositeType           OppositeType;   //!< Result type with opposite storage order for expression template evaluations.
   typedef typename ResultType::TransposeType          TransposeType;  //!< Transpose type for expression template evaluations.
   typedef typename ResultType::ElementType            ElementType;    //!< Resulting element type.
   typedef typename IntrinsicTrait<ElementType>::Type  IntrinsicType;  //!< Resulting intrinsic element type.
   typedef const ResultType                            CompositeType;  //!< Data type for composite expression templates.

   //! Composite type of the left-hand side dense matrix expression.
   typedef typename SelectType< IsExpression<MT1>::value, const MT1, const MT1& >::Type  LeftOperand;

   //! Composite type of the right-hand side dense matrix expression.
   typedef typename SelectType< IsExpression<MT2>::value, const MT2, const MT2& >::Type  RightOperand;

   //! Type for the assignment of the left-hand side dense matrix operand.
   typedef typename SelectType< IsExpression<MT1>::value, const RT1, CT1 >::Type  LT;

   //! Type for the assignment of the right-hand side dense matrix operand.
   typedef typename SelectType< IsExpression<MT2>::value, const RT2, CT2 >::Type  RT;
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation switch for the expression template evaluation strategy.
   enum { vectorizable = 0 };

   //! Compilation flag for the detection of aliasing effects.
   enum { canAlias = !IsExpression<MT1>::value || !IsExpression<MT2>::value };
   //**********************************************************************************************

   //**Constructor*********************************************************************************
   /*!\brief Constructor for the DMatDMatMultExpr class.
   //
   // \param lhs The left-hand side operand of the multiplication expression.
   // \param rhs The right-hand side operand of the multiplication expression.
   */
   explicit inline DMatDMatMultExpr( const MT1& lhs, const MT2& rhs )
      : lhs_( lhs )  // Left-hand side dense matrix of the multiplication expression
      , rhs_( rhs )  // Right-hand side dense matrix of the multiplication expression
   {
      BLAZE_INTERNAL_ASSERT( lhs.columns() == rhs.rows(), "Invalid matrix sizes" );
   }
   //**********************************************************************************************

   //**Access operator*****************************************************************************
   /*!\brief 2D-access to the matrix elements.
   //
   // \param i Access index for the row. The index has to be in the range \f$[0..M-1]\f$.
   // \param j Access index for the column. The index has to be in the range \f$[0..N-1]\f$.
   // \return Reference to the accessed value.
   */
   inline const ElementType operator()( size_t i, size_t j ) const {
      BLAZE_INTERNAL_ASSERT( i < lhs_.rows()   , "Invalid row access index"    );
      BLAZE_INTERNAL_ASSERT( j < rhs_.columns(), "Invalid column access index" );

      ElementType tmp;

      if( lhs_.columns() != 0UL ) {
         const size_t end( ( ( lhs_.columns()-1UL ) & size_t(-2) ) + 1UL );
         tmp = lhs_(i,0UL) * rhs_(0UL,j);
         for( size_t k=1UL; k<end; k+=2UL ) {
            tmp += lhs_(i,k    ) * rhs_(k    ,j);
            tmp += lhs_(i,k+1UL) * rhs_(k+1UL,j);
         }
         if( end < lhs_.columns() ) {
            tmp += lhs_(i,end) * rhs_(end,j);
         }
      }
      else {
         reset( tmp );
      }

      return tmp;
   }
   //**********************************************************************************************

   //**Rows function*******************************************************************************
   /*!\brief Returns the current number of rows of the matrix.
   //
   // \return The number of rows of the matrix.
   */
   inline size_t rows() const {
      return lhs_.rows();
   }
   //**********************************************************************************************

   //**Columns function****************************************************************************
   /*!\brief Returns the current number of columns of the matrix.
   //
   // \return The number of columns of the matrix.
   */
   inline size_t columns() const {
      return rhs_.columns();
   }
   //**********************************************************************************************

   //**Left operand access*************************************************************************
   /*!\brief Returns the left-hand side dense matrix operand.
   //
   // \return The left-hand side dense matrix operand.
   */
   inline LeftOperand leftOperand() const {
      return lhs_;
   }
   //**********************************************************************************************

   //**Right operand access************************************************************************
   /*!\brief Returns the right-hand side dense matrix operand.
   //
   // \return The right-hand side dense matrix operand.
   */
   inline RightOperand rightOperand() const {
      return rhs_;
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the expression is aliased with the given address \a alias.
   //
   // \param alias The alias to be checked.
   // \return \a true in case an alias effect is detected, \a false otherwise.
   */
   template< typename T >
   inline bool isAliased( const T* alias ) const {
      return ( lhs_.isAliased( alias ) || rhs_.isAliased( alias ) );
   }
   //**********************************************************************************************

 private:
   //**Member variables****************************************************************************
   LeftOperand  lhs_;  //!< Left-hand side dense matrix of the multiplication expression.
   RightOperand rhs_;  //!< Right-hand side dense matrix of the multiplication expression.
   //**********************************************************************************************

   //**Assignment to dense matrices****************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Assignment of a dense matrix-dense matrix multiplication to a dense matrix
   //        (\f$ C=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side multiplication expression to be assigned.
   // \return void
   */
   template< typename MT3  // Type of the target dense matrix
           , bool SO >     // Storage order of the target dense matrix
   friend inline void assign( DenseMatrix<MT3,SO>& lhs, const DMatDMatMultExpr& rhs )
   {
      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      if( (~lhs).rows() == 0UL || (~lhs).columns() == 0UL ) {
         return;
      }
      else if( rhs.lhs_.columns() == 0UL ) {
         reset( ~lhs );
         return;
      }

      LT A( rhs.lhs_ );  // Evaluation of the left-hand side dense matrix operand
      RT B( rhs.rhs_ );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.lhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.lhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == rhs.rhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == rhs.rhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns()  , "Invalid number of columns" );

      if( (~lhs).rows() * (~lhs).columns() < DMATDMATMULT_THRESHOLD )
         DMatDMatMultExpr::selectDefaultAssignKernel( ~lhs, A, B );
      else
         DMatDMatMultExpr::selectBlasAssignKernel( ~lhs, A, B );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Default assignment to dense matrices********************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Default assignment of a dense matrix-dense matrix multiplication (\f$ C=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function implements the default assignment of a dense matrix-dense matrix
   // multiplication expression to a dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename DisableIf< UseVectorizedDefaultKernel<MT3,MT4,MT5> >::Type
      selectDefaultAssignKernel( MT3& C, const MT4& A, const MT5& B )
   {
      const size_t M( A.rows()    );
      const size_t N( B.columns() );
      const size_t K( A.columns() );

      for( size_t i=0UL; i<M; ++i ) {
         for( size_t j=0UL; j<N; ++j ) {
            C(i,j) = A(i,0UL) * B(0UL,j);
         }
         for( size_t k=1UL; k<K; ++k ) {
            for( size_t j=0UL; j<N; ++j ) {
               C(i,j) += A(i,k) * B(k,j);
            }
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Vectorized default assignment to row-major dense matrices***********************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Vectorized default assignment of a dense matrix-dense matrix multiplication
   //        (\f$ C=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function implements the vectorized default assignment of a dense matrix-dense matrix
   // multiplication expression to a row-major dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseVectorizedDefaultKernel<MT3,MT4,MT5> >::Type
      selectDefaultAssignKernel( DenseMatrix<MT3,false>& C, const MT4& A, const MT5& B )
   {
      typedef IntrinsicTrait<ElementType>  IT;

      const size_t M( A.rows()    );
      const size_t N( B.spacing() );
      const size_t K( A.columns() );

      size_t j( 0UL );

      for( ; (j+IT::size*8UL) <= N; j+=IT::size*8UL ) {
         for( size_t i=0UL; i<M; ++i ) {
            IntrinsicType xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8;
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i,k) ) );
               xmm1 = xmm1 + a1 * B.get(k,j             );
               xmm2 = xmm2 + a1 * B.get(k,j+IT::size    );
               xmm3 = xmm3 + a1 * B.get(k,j+IT::size*2UL);
               xmm4 = xmm4 + a1 * B.get(k,j+IT::size*3UL);
               xmm5 = xmm5 + a1 * B.get(k,j+IT::size*4UL);
               xmm6 = xmm6 + a1 * B.get(k,j+IT::size*5UL);
               xmm7 = xmm7 + a1 * B.get(k,j+IT::size*6UL);
               xmm8 = xmm8 + a1 * B.get(k,j+IT::size*7UL);
            }
            store( &(~C)(i,j             ), xmm1 );
            store( &(~C)(i,j+IT::size    ), xmm2 );
            store( &(~C)(i,j+IT::size*2UL), xmm3 );
            store( &(~C)(i,j+IT::size*3UL), xmm4 );
            store( &(~C)(i,j+IT::size*4UL), xmm5 );
            store( &(~C)(i,j+IT::size*5UL), xmm6 );
            store( &(~C)(i,j+IT::size*6UL), xmm7 );
            store( &(~C)(i,j+IT::size*7UL), xmm8 );
         }
      }
      for( ; (j+IT::size*4UL) <= N; j+=IT::size*4UL ) {
         size_t i( 0UL );
         for( ; (i+2UL) <= M; i+=2UL ) {
            IntrinsicType xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8;
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i    ,k) ) );
               const IntrinsicType a2( set( A(i+1UL,k) ) );
               const IntrinsicType b1( B.get(k,j             ) );
               const IntrinsicType b2( B.get(k,j+IT::size    ) );
               const IntrinsicType b3( B.get(k,j+IT::size*2UL) );
               const IntrinsicType b4( B.get(k,j+IT::size*3UL) );
               xmm1 = xmm1 + a1 * b1;
               xmm2 = xmm2 + a1 * b2;
               xmm3 = xmm3 + a1 * b3;
               xmm4 = xmm4 + a1 * b4;
               xmm5 = xmm5 + a2 * b1;
               xmm6 = xmm6 + a2 * b2;
               xmm7 = xmm7 + a2 * b3;
               xmm8 = xmm8 + a2 * b4;
            }
            store( &(~C)(i    ,j             ), xmm1 );
            store( &(~C)(i    ,j+IT::size    ), xmm2 );
            store( &(~C)(i    ,j+IT::size*2UL), xmm3 );
            store( &(~C)(i    ,j+IT::size*3UL), xmm4 );
            store( &(~C)(i+1UL,j             ), xmm5 );
            store( &(~C)(i+1UL,j+IT::size    ), xmm6 );
            store( &(~C)(i+1UL,j+IT::size*2UL), xmm7 );
            store( &(~C)(i+1UL,j+IT::size*3UL), xmm8 );
         }
         if( i < M ) {
            IntrinsicType xmm1, xmm2, xmm3, xmm4;
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i,k) ) );
               xmm1 = xmm1 + a1 * B.get(k,j             );
               xmm2 = xmm2 + a1 * B.get(k,j+IT::size    );
               xmm3 = xmm3 + a1 * B.get(k,j+IT::size*2UL);
               xmm4 = xmm4 + a1 * B.get(k,j+IT::size*3UL);
            }
            store( &(~C)(i,j             ), xmm1 );
            store( &(~C)(i,j+IT::size    ), xmm2 );
            store( &(~C)(i,j+IT::size*2UL), xmm3 );
            store( &(~C)(i,j+IT::size*3UL), xmm4 );
         }
      }
      for( ; (j+IT::size*2UL) <= N; j+=IT::size*2UL ) {
         size_t i( 0UL );
         for( ; (i+2UL) <= M; i+=2UL ) {
            IntrinsicType xmm1, xmm2, xmm3, xmm4;
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i    ,k) ) );
               const IntrinsicType a2( set( A(i+1UL,k) ) );
               const IntrinsicType b1( B.get(k,j         ) );
               const IntrinsicType b2( B.get(k,j+IT::size) );
               xmm1 = xmm1 + a1 * b1;
               xmm2 = xmm2 + a1 * b2;
               xmm3 = xmm3 + a2 * b1;
               xmm4 = xmm4 + a2 * b2;
            }
            store( &(~C)(i    ,j         ), xmm1 );
            store( &(~C)(i    ,j+IT::size), xmm2 );
            store( &(~C)(i+1UL,j         ), xmm3 );
            store( &(~C)(i+1UL,j+IT::size), xmm4 );
         }
         if( i < M ) {
            IntrinsicType xmm1, xmm2;
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i,k) ) );
               xmm1 = xmm1 + a1 * B.get(k,j         );
               xmm2 = xmm2 + a1 * B.get(k,j+IT::size);
            }
            store( &(~C)(i,j         ), xmm1 );
            store( &(~C)(i,j+IT::size), xmm2 );
         }
      }
      if( j < N ) {
         size_t i( 0UL );
         for( ; (i+2UL) <= M; i+=2UL ) {
            IntrinsicType xmm1, xmm2;
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType b1( B.get(k,j) );
               xmm1 = xmm1 + set( A(i    ,k) ) * b1;
               xmm2 = xmm2 + set( A(i+1UL,k) ) * b1;
            }
            store( &(~C)(i    ,j), xmm1 );
            store( &(~C)(i+1UL,j), xmm2 );
         }
         if( i < M ) {
            IntrinsicType xmm1;
            for( size_t k=0UL; k<K; ++k ) {
               xmm1 = xmm1 + set( A(i,k) ) * B.get(k,j);
            }
            store( &(~C)(i,j), xmm1 );
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Vectorized default assignment to column-major dense matrices********************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Vectorized default assignment of a dense matrix-dense matrix multiplication
   //        (\f$ C=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function implements the vectorized default assignment of a dense matrix-dense matrix
   // multiplication expression to a column-major dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseVectorizedDefaultKernel<MT3,MT4,MT5> >::Type
      selectDefaultAssignKernel( DenseMatrix<MT3,true>& C, const MT4& A, const MT5& B )
   {
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( typename MT4::OppositeType );
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( typename MT5::OppositeType );

      if( !IsResizable<MT4>::value && IsResizable<MT5>::value ) {
         const typename MT4::OppositeType tmp( A );
         assign( ~C, tmp * B );
      }
      else if( IsResizable<MT4>::value && !IsResizable<MT5>::value ) {
         const typename MT5::OppositeType tmp( B );
         assign( ~C, A * tmp );
      }
      else if( A.rows() * A.columns() <= B.rows() * B.columns() ) {
         const typename MT4::OppositeType tmp( A );
         assign( ~C, tmp * B );
      }
      else {
         const typename MT5::OppositeType tmp( B );
         assign( ~C, A * tmp );
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**BLAS-based assignment to dense matrices (default)*******************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Default assignment of a dense matrix-dense matrix multiplication (\f$ C=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function relays to the default implementation of the assignment of a dense matrix-
   // dense matrix multiplication expression to a dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseDefaultKernel<MT3,MT4,MT5> >::Type
      selectBlasAssignKernel( MT3& C, const MT4& A, const MT5& B )
   {
      selectDefaultAssignKernel( C, A, B );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**BLAS-based assignment to dense matrices (single precision)**********************************
#if BLAZE_BLAS_MODE
   /*! \cond BLAZE_INTERNAL */
   /*!\brief BLAS-based assignment of a dense matrix-dense matrix multiplication for single
   //        precision matrices (\f$ C=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function performs the dense matrix-dense matrix multiplication for single precision
   // matrices based on the BLAS cblas_sgemm() function.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseSinglePrecisionKernel<MT3,MT4,MT5> >::Type
      selectBlasAssignKernel( MT3& C, const MT4& A, const MT5& B )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename MT3::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename MT4::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename MT5::ElementType );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( B.columns() ) );
      const int K  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const int ldb( numeric_cast<int>( B.spacing() ) );
      const int ldc( numeric_cast<int>( C.spacing() ) );

      cblas_sgemm( ( IsRowMajorMatrix<MT3>::value )?( CblasRowMajor ):( CblasColMajor ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   M, N, K, 1.0F, A.data(), lda, B.data(), ldb, 0.0F, C.data(), ldc );
   }
   /*! \endcond */
#endif
   //**********************************************************************************************

   //**BLAS-based assignment to dense matrices (double precision)**********************************
#if BLAZE_BLAS_MODE
   /*! \cond BLAZE_INTERNAL */
   /*!\brief BLAS-based assignment of a dense matrix-dense matrix multiplication for double
   //        precision matrices (\f$ C=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function performs the dense matrix-dense matrix multiplication for double precision
   // matrices based on the BLAS cblas_dgemm() function.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseDoublePrecisionKernel<MT3,MT4,MT5> >::Type
      selectBlasAssignKernel( MT3& C, const MT4& A, const MT5& B )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename MT3::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename MT4::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename MT5::ElementType );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( B.columns() ) );
      const int K  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const int ldb( numeric_cast<int>( B.spacing() ) );
      const int ldc( numeric_cast<int>( C.spacing() ) );

      cblas_dgemm( ( IsRowMajorMatrix<MT3>::value )?( CblasRowMajor ):( CblasColMajor ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   M, N, K, 1.0, A.data(), lda, B.data(), ldb, 0.0, C.data(), ldc );
   }
   /*! \endcond */
#endif
   //**********************************************************************************************

   //**BLAS-based assignment to dense matrices (single precision complex)**************************
#if BLAZE_BLAS_MODE
   /*! \cond BLAZE_INTERNAL */
   /*!\brief BLAS-based assignment of a dense matrix-dense matrix multiplication for single
   //        precision complex matrices (\f$ C=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function performs the dense matrix-dense matrix multiplication for single precision
   // complex matrices based on the BLAS cblas_cgemm() function.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseSinglePrecisionComplexKernel<MT3,MT4,MT5> >::Type
      selectBlasAssignKernel( MT3& C, const MT4& A, const MT5& B )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT3::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT4::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT5::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename MT3::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename MT4::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename MT5::ElementType::value_type );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( B.columns() ) );
      const int K  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const int ldb( numeric_cast<int>( B.spacing() ) );
      const int ldc( numeric_cast<int>( C.spacing() ) );
      const complex<float> alpha( 1.0F, 0.0F );
      const complex<float> beta ( 0.0F, 0.0F );

      cblas_cgemm( ( IsRowMajorMatrix<MT3>::value )?( CblasRowMajor ):( CblasColMajor ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   M, N, K, &alpha, A.data(), lda, B.data(), ldb, &beta, C.data(), ldc );
   }
   /*! \endcond */
#endif
   //**********************************************************************************************

   //**BLAS-based assignment to dense matrices (double precision complex)**************************
#if BLAZE_BLAS_MODE
   /*! \cond BLAZE_INTERNAL */
   /*!\brief BLAS-based assignment of a dense matrix-dense matrix multiplication for double
   //        precision complex matrices (\f$ C=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function performs the dense matrix-dense matrix multiplication for double precision
   // complex matrices based on the BLAS cblas_zgemm() function.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseDoublePrecisionComplexKernel<MT3,MT4,MT5> >::Type
      selectBlasAssignKernel( MT3& C, const MT4& A, const MT5& B )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT3::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT4::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT5::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename MT3::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename MT4::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename MT5::ElementType::value_type );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( B.columns() ) );
      const int K  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const int ldb( numeric_cast<int>( B.spacing() ) );
      const int ldc( numeric_cast<int>( C.spacing() ) );
      const complex<double> alpha( 1.0, 0.0 );
      const complex<double> beta ( 0.0, 0.0 );

      cblas_zgemm( ( IsRowMajorMatrix<MT3>::value )?( CblasRowMajor ):( CblasColMajor ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   M, N, K, &alpha, A.data(), lda, B.data(), ldb, &beta, C.data(), ldc );
   }
   /*! \endcond */
#endif
   //**********************************************************************************************

   //**Assignment to sparse matrices***************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Assignment of a dense matrix-dense matrix multiplication to a sparse matrix.
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side sparse matrix.
   // \param rhs The right-hand side multiplication expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized assignment of a dense matrix-dense
   // matrix multiplication expression to a sparse matrix.
   */
   template< typename MT  // Type of the target sparse matrix
           , bool SO >    // Storage order of the target sparse matrix
   friend inline void assign( SparseMatrix<MT,SO>& lhs, const DMatDMatMultExpr& rhs )
   {
      typedef typename SelectType< SO, OppositeType, ResultType >::Type  TmpType;

      BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( OppositeType );
      BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( OppositeType );
      BLAZE_CONSTRAINT_MATRICES_MUST_HAVE_SAME_STORAGE_ORDER( MT, TmpType );
      BLAZE_CONSTRAINT_MUST_BE_REFERENCE_TYPE( typename TmpType::CompositeType );

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      const TmpType tmp( rhs );
      assign( ~lhs, tmp );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Addition assignment to dense matrices*******************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Addition assignment of a dense matrix-dense matrix multiplication to a dense matrix
   //        (\f$ C+=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side multiplication expression to be added.
   // \return void
   //
   // This function implements the performance optimized addition assignment of a dense matrix-
   // dense matrix multiplication expression to a dense matrix.
   */
   template< typename MT3  // Type of the target dense matrix
           , bool SO >     // Storage order of the target dense matrix
   friend inline void addAssign( DenseMatrix<MT3,SO>& lhs, const DMatDMatMultExpr& rhs )
   {
      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      if( (~lhs).rows() == 0UL || (~lhs).columns() == 0UL || rhs.lhs_.columns() == 0UL ) {
         return;
      }

      LT A( rhs.lhs_ );  // Evaluation of the left-hand side dense matrix operand
      RT B( rhs.rhs_ );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.lhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.lhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == rhs.rhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == rhs.rhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns()  , "Invalid number of columns" );

      if( (~lhs).rows() * (~lhs).columns() < DMATDMATMULT_THRESHOLD )
         DMatDMatMultExpr::selectDefaultAddAssignKernel( ~lhs, A, B );
      else
         DMatDMatMultExpr::selectBlasAddAssignKernel( ~lhs, A, B );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Default addition assignment to dense matrices***********************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Default addition assignment of a dense matrix-dense matrix multiplication
   //        (\f$ C+=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function implements the default addition assignment of a dense matrix-dense matrix
   // multiplication expression to a dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename DisableIf< UseVectorizedDefaultKernel<MT3,MT4,MT5> >::Type
      selectDefaultAddAssignKernel( MT3& C, const MT4& A, const MT5& B )
   {
      const size_t M( A.rows()    );
      const size_t N( B.columns() );
      const size_t K( A.columns() );

      BLAZE_INTERNAL_ASSERT( ( N - ( N % 2UL ) ) == ( N & size_t(-2) ), "Invalid end calculation" );
      const size_t end( N & size_t(-2) );

      for( size_t i=0UL; i<M; ++i ) {
         for( size_t k=0UL; k<K; ++k ) {
            for( size_t j=0UL; j<end; j+=2UL ) {
               C(i,j    ) += A(i,k) * B(k,j    );
               C(i,j+1UL) += A(i,k) * B(k,j+1UL);
            }
            if( end < N ) {
               C(i,end) += A(i,k) * B(k,end);
            }
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Vectorized default addition assignment to row-major dense matrices**************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Vectorized default addition assignment of a dense matrix-dense matrix multiplication
   //        (\f$ C+=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function implements the vectorized default addition assignment of a dense matrix-dense
   // matrix multiplication expression to a row-major dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseVectorizedDefaultKernel<MT3,MT4,MT5> >::Type
      selectDefaultAddAssignKernel( DenseMatrix<MT3,false>& C, const MT4& A, const MT5& B )
   {
      typedef IntrinsicTrait<ElementType>  IT;

      const size_t M( A.rows()    );
      const size_t N( B.spacing() );
      const size_t K( A.columns() );

      size_t j( 0UL );

      for( ; (j+IT::size*8UL) <= N; j+=IT::size*8UL ) {
         for( size_t i=0UL; i<M; ++i ) {
            IntrinsicType xmm1( load( &(~C)(i,j             ) ) );
            IntrinsicType xmm2( load( &(~C)(i,j+IT::size    ) ) );
            IntrinsicType xmm3( load( &(~C)(i,j+IT::size*2UL) ) );
            IntrinsicType xmm4( load( &(~C)(i,j+IT::size*3UL) ) );
            IntrinsicType xmm5( load( &(~C)(i,j+IT::size*4UL) ) );
            IntrinsicType xmm6( load( &(~C)(i,j+IT::size*5UL) ) );
            IntrinsicType xmm7( load( &(~C)(i,j+IT::size*6UL) ) );
            IntrinsicType xmm8( load( &(~C)(i,j+IT::size*7UL) ) );
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i,k) ) );
               xmm1 = xmm1 + a1 * B.get(k,j             );
               xmm2 = xmm2 + a1 * B.get(k,j+IT::size    );
               xmm3 = xmm3 + a1 * B.get(k,j+IT::size*2UL);
               xmm4 = xmm4 + a1 * B.get(k,j+IT::size*3UL);
               xmm5 = xmm5 + a1 * B.get(k,j+IT::size*4UL);
               xmm6 = xmm6 + a1 * B.get(k,j+IT::size*5UL);
               xmm7 = xmm7 + a1 * B.get(k,j+IT::size*6UL);
               xmm8 = xmm8 + a1 * B.get(k,j+IT::size*7UL);
            }
            store( &(~C)(i,j             ), xmm1 );
            store( &(~C)(i,j+IT::size    ), xmm2 );
            store( &(~C)(i,j+IT::size*2UL), xmm3 );
            store( &(~C)(i,j+IT::size*3UL), xmm4 );
            store( &(~C)(i,j+IT::size*4UL), xmm5 );
            store( &(~C)(i,j+IT::size*5UL), xmm6 );
            store( &(~C)(i,j+IT::size*6UL), xmm7 );
            store( &(~C)(i,j+IT::size*7UL), xmm8 );
         }
      }
      for( ; (j+IT::size*4UL) <= N; j+=IT::size*4UL ) {
         size_t i( 0UL );
         for( ; (i+2UL) <= M; i+=2UL ) {
            IntrinsicType xmm1( load( &(~C)(i    ,j             ) ) );
            IntrinsicType xmm2( load( &(~C)(i    ,j+IT::size    ) ) );
            IntrinsicType xmm3( load( &(~C)(i    ,j+IT::size*2UL) ) );
            IntrinsicType xmm4( load( &(~C)(i    ,j+IT::size*3UL) ) );
            IntrinsicType xmm5( load( &(~C)(i+1UL,j             ) ) );
            IntrinsicType xmm6( load( &(~C)(i+1UL,j+IT::size    ) ) );
            IntrinsicType xmm7( load( &(~C)(i+1UL,j+IT::size*2UL) ) );
            IntrinsicType xmm8( load( &(~C)(i+1UL,j+IT::size*3UL) ) );
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i    ,k) ) );
               const IntrinsicType a2( set( A(i+1UL,k) ) );
               const IntrinsicType b1( B.get(k,j             ) );
               const IntrinsicType b2( B.get(k,j+IT::size    ) );
               const IntrinsicType b3( B.get(k,j+IT::size*2UL) );
               const IntrinsicType b4( B.get(k,j+IT::size*3UL) );
               xmm1 = xmm1 + a1 * b1;
               xmm2 = xmm2 + a1 * b2;
               xmm3 = xmm3 + a1 * b3;
               xmm4 = xmm4 + a1 * b4;
               xmm5 = xmm5 + a2 * b1;
               xmm6 = xmm6 + a2 * b2;
               xmm7 = xmm7 + a2 * b3;
               xmm8 = xmm8 + a2 * b4;
            }
            store( &(~C)(i    ,j             ), xmm1 );
            store( &(~C)(i    ,j+IT::size    ), xmm2 );
            store( &(~C)(i    ,j+IT::size*2UL), xmm3 );
            store( &(~C)(i    ,j+IT::size*3UL), xmm4 );
            store( &(~C)(i+1UL,j             ), xmm5 );
            store( &(~C)(i+1UL,j+IT::size    ), xmm6 );
            store( &(~C)(i+1UL,j+IT::size*2UL), xmm7 );
            store( &(~C)(i+1UL,j+IT::size*3UL), xmm8 );
         }
         if( i < M ) {
            IntrinsicType xmm1( load( &(~C)(i,j             ) ) );
            IntrinsicType xmm2( load( &(~C)(i,j+IT::size    ) ) );
            IntrinsicType xmm3( load( &(~C)(i,j+IT::size*2UL) ) );
            IntrinsicType xmm4( load( &(~C)(i,j+IT::size*3UL) ) );
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i,k) ) );
               xmm1 = xmm1 + a1 * B.get(k,j             );
               xmm2 = xmm2 + a1 * B.get(k,j+IT::size    );
               xmm3 = xmm3 + a1 * B.get(k,j+IT::size*2UL);
               xmm4 = xmm4 + a1 * B.get(k,j+IT::size*3UL);
            }
            store( &(~C)(i,j             ), xmm1 );
            store( &(~C)(i,j+IT::size    ), xmm2 );
            store( &(~C)(i,j+IT::size*2UL), xmm3 );
            store( &(~C)(i,j+IT::size*3UL), xmm4 );
         }
      }
      for( ; (j+IT::size*2UL) <= N; j+=IT::size*2UL ) {
         size_t i( 0UL );
         for( ; (i+2UL) <= M; i+=2UL ) {
            IntrinsicType xmm1( load( &(~C)(i    ,j         ) ) );
            IntrinsicType xmm2( load( &(~C)(i    ,j+IT::size) ) );
            IntrinsicType xmm3( load( &(~C)(i+1UL,j         ) ) );
            IntrinsicType xmm4( load( &(~C)(i+1UL,j+IT::size) ) );
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i    ,k) ) );
               const IntrinsicType a2( set( A(i+1UL,k) ) );
               const IntrinsicType b1( B.get(k,j         ) );
               const IntrinsicType b2( B.get(k,j+IT::size) );
               xmm1 = xmm1 + a1 * b1;
               xmm2 = xmm2 + a1 * b2;
               xmm3 = xmm3 + a2 * b1;
               xmm4 = xmm4 + a2 * b2;
            }
            store( &(~C)(i    ,j         ), xmm1 );
            store( &(~C)(i    ,j+IT::size), xmm2 );
            store( &(~C)(i+1UL,j         ), xmm3 );
            store( &(~C)(i+1UL,j+IT::size), xmm4 );
         }
         if( i < M ) {
            IntrinsicType xmm1( load( &(~C)(i,j         ) ) );
            IntrinsicType xmm2( load( &(~C)(i,j+IT::size) ) );
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i,k) ) );
               xmm1 = xmm1 + a1 * B.get(k,j         );
               xmm2 = xmm2 + a1 * B.get(k,j+IT::size);
            }
            store( &(~C)(i,j         ), xmm1 );
            store( &(~C)(i,j+IT::size), xmm2 );
         }
      }
      if( j < N ) {
         size_t i( 0UL );
         for( ; (i+2UL) <= M; i+=2UL ) {
            IntrinsicType xmm1( load( &(~C)(i    ,j) ) );
            IntrinsicType xmm2( load( &(~C)(i+1UL,j) ) );
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType b1( B.get(k,j) );
               xmm1 = xmm1 + set( A(i    ,k) ) * b1;
               xmm2 = xmm2 + set( A(i+1UL,k) ) * b1;
            }
            store( &(~C)(i    ,j), xmm1 );
            store( &(~C)(i+1UL,j), xmm2 );
         }
         if( i < M ) {
            IntrinsicType xmm1( load( &(~C)(i,j) ) );
            for( size_t k=0UL; k<K; ++k ) {
               xmm1 = xmm1 + set( A(i,k) ) * B.get(k,j);
            }
            store( &(~C)(i,j), xmm1 );
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Vectorized default addition assignment to column-major dense matrices***********************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Vectorized default addition assignment of a dense matrix-dense matrix multiplication
   //        (\f$ C+=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function implements the vectorized default addition assignment of a dense matrix-dense
   // matrix multiplication expression to a column-major dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseVectorizedDefaultKernel<MT3,MT4,MT5> >::Type
      selectDefaultAddAssignKernel( DenseMatrix<MT3,true>& C, const MT4& A, const MT5& B )
   {
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( typename MT4::OppositeType );
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( typename MT5::OppositeType );

      if( !IsResizable<MT4>::value && IsResizable<MT5>::value ) {
         const typename MT4::OppositeType tmp( A );
         addAssign( ~C, tmp * B );
      }
      else if( IsResizable<MT4>::value && !IsResizable<MT5>::value ) {
         const typename MT5::OppositeType tmp( B );
         addAssign( ~C, A * tmp );
      }
      else if( A.rows() * A.columns() <= B.rows() * B.columns() ) {
         const typename MT4::OppositeType tmp( A );
         addAssign( ~C, tmp * B );
      }
      else {
         const typename MT5::OppositeType tmp( B );
         addAssign( ~C, A * tmp );
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**BLAS-based addition assignment to dense matrices (default)**********************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Default addition assignment of a dense matrix-dense matrix multiplication
   //        (\f$ C+=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function relays to the default implementation of the addition assignment of a dense
   // matrix-dense matrix multiplication expression to a dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseDefaultKernel<MT3,MT4,MT5> >::Type
      selectBlasAddAssignKernel( MT3& C, const MT4& A, const MT5& B )
   {
      selectDefaultAddAssignKernel( C, A, B );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**BLAS-based addition assignment to dense matrices (single precision)*************************
#if BLAZE_BLAS_MODE
   /*! \cond BLAZE_INTERNAL */
   /*!\brief BLAS-based addition assignment of a dense matrix-dense matrix multiplication for
   //        single precision matrices (\f$ C+=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function performs the dense matrix-dense matrix multiplication for single precision
   // matrices based on the BLAS cblas_sgemm() function.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseSinglePrecisionKernel<MT3,MT4,MT5> >::Type
      selectBlasAddAssignKernel( MT3& C, const MT4& A, const MT5& B )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename MT3::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename MT4::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename MT5::ElementType );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( B.columns() ) );
      const int K  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const int ldb( numeric_cast<int>( B.spacing() ) );
      const int ldc( numeric_cast<int>( C.spacing() ) );

      cblas_sgemm( ( IsRowMajorMatrix<MT3>::value )?( CblasRowMajor ):( CblasColMajor ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   M, N, K, 1.0F, A.data(), lda, B.data(), ldb, 1.0F, C.data(), ldc );
   }
   /*! \endcond */
#endif
   //**********************************************************************************************

   //**BLAS-based addition assignment to dense matrices (double precision)*************************
#if BLAZE_BLAS_MODE
   /*! \cond BLAZE_INTERNAL */
   /*!\brief BLAS-based addition assignment of a dense matrix-dense matrix multiplication for
   //        double precision matrices (\f$ C+=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function performs the dense matrix-dense matrix multiplication for double precision
   // matrices based on the BLAS cblas_dgemm() function.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseDoublePrecisionKernel<MT3,MT4,MT5> >::Type
      selectBlasAddAssignKernel( MT3& C, const MT4& A, const MT5& B )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename MT3::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename MT4::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename MT5::ElementType );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( B.columns() ) );
      const int K  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const int ldb( numeric_cast<int>( B.spacing() ) );
      const int ldc( numeric_cast<int>( C.spacing() ) );

      cblas_dgemm( ( IsRowMajorMatrix<MT3>::value )?( CblasRowMajor ):( CblasColMajor ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   M, N, K, 1.0, A.data(), lda, B.data(), ldb, 1.0, C.data(), ldc );
   }
   /*! \endcond */
#endif
   //**********************************************************************************************

   //**BLAS-based addition assignment to dense matrices (single precision complex)*****************
#if BLAZE_BLAS_MODE
   /*! \cond BLAZE_INTERNAL */
   /*!\brief BLAS-based addition assignment of a dense matrix-dense matrix multiplication for
   //        single precision complex matrices (\f$ C+=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function performs the dense matrix-dense matrix multiplication for single precision
   // complex matrices based on the BLAS cblas_cgemm() function.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseSinglePrecisionComplexKernel<MT3,MT4,MT5> >::Type
      selectBlasAddAssignKernel( MT3& C, const MT4& A, const MT5& B )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT3::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT4::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT5::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename MT3::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename MT4::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename MT5::ElementType::value_type );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( B.columns() ) );
      const int K  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const int ldb( numeric_cast<int>( B.spacing() ) );
      const int ldc( numeric_cast<int>( C.spacing() ) );
      const complex<float> alpha( 1.0F, 0.0F );
      const complex<float> beta ( 1.0F, 0.0F );

      cblas_cgemm( ( IsRowMajorMatrix<MT3>::value )?( CblasRowMajor ):( CblasColMajor ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   M, N, K, &alpha, A.data(), lda, B.data(), ldb, &beta, C.data(), ldc );
   }
   /*! \endcond */
#endif
   //**********************************************************************************************

   //**BLAS-based addition assignment to dense matrices (double precision complex)*****************
#if BLAZE_BLAS_MODE
   /*! \cond BLAZE_INTERNAL */
   /*!\brief BLAS-based addition assignment of a dense matrix-dense matrix multiplication for
   //        double precision complex matrices (\f$ C+=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function performs the dense matrix-dense matrix multiplication for double precision
   // complex matrices based on the BLAS cblas_zgemm() function.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseDoublePrecisionComplexKernel<MT3,MT4,MT5> >::Type
      selectBlasAddAssignKernel( MT3& C, const MT4& A, const MT5& B )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT3::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT4::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT5::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename MT3::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename MT4::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename MT5::ElementType::value_type );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( B.columns() ) );
      const int K  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const int ldb( numeric_cast<int>( B.spacing() ) );
      const int ldc( numeric_cast<int>( C.spacing() ) );
      const complex<double> alpha( 1.0, 0.0 );
      const complex<double> beta ( 1.0, 0.0 );

      cblas_zgemm( ( IsRowMajorMatrix<MT3>::value )?( CblasRowMajor ):( CblasColMajor ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   M, N, K, &alpha, A.data(), lda, B.data(), ldb, &beta, C.data(), ldc );
   }
   /*! \endcond */
#endif
   //**********************************************************************************************

   //**Addition assignment to sparse matrices******************************************************
   // No special implementation for the addition assignment to sparse matrices.
   //**********************************************************************************************

   //**Subtraction assignment to dense matrices****************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Subtraction assignment of a dense matrix-dense matrix multiplication to a
   //        dense matrix (\f$ C-=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side multiplication expression to be subtracted.
   // \return void
   //
   // This function implements the performance optimized subtraction assignment of a dense matrix-
   // dense matrix multiplication expression to a dense matrix.
   */
   template< typename MT3  // Type of the target dense matrix
           , bool SO >     // Storage order of the target dense matrix
   friend inline void subAssign( DenseMatrix<MT3,SO>& lhs, const DMatDMatMultExpr& rhs )
   {
      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      if( (~lhs).rows() == 0UL || (~lhs).columns() == 0UL || rhs.lhs_.columns() == 0UL ) {
         return;
      }

      LT A( rhs.lhs_ );  // Evaluation of the left-hand side dense matrix operand
      RT B( rhs.rhs_ );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.lhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.lhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == rhs.rhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == rhs.rhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns()  , "Invalid number of columns" );

      if( (~lhs).rows() * (~lhs).columns() < DMATDMATMULT_THRESHOLD )
         DMatDMatMultExpr::selectDefaultSubAssignKernel( ~lhs, A, B );
      else
         DMatDMatMultExpr::selectBlasSubAssignKernel( ~lhs, A, B );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Default subtraction assignment to dense matrices********************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Default subtraction assignment of a dense matrix-dense matrix multiplication
   //        (\f$ C-=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function implements the default subtraction assignment of a dense matrix-dense matrix
   // multiplication expression to a dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename DisableIf< UseVectorizedDefaultKernel<MT3,MT4,MT5> >::Type
      selectDefaultSubAssignKernel( MT3& C, const MT4& A, const MT5& B )
   {
      const size_t M( A.rows()    );
      const size_t N( B.columns() );
      const size_t K( A.columns() );

      BLAZE_INTERNAL_ASSERT( ( N - ( N % 2UL ) ) == ( N & size_t(-2) ), "Invalid end calculation" );
      const size_t end( N & size_t(-2) );

      for( size_t i=0UL; i<M; ++i ) {
         for( size_t k=0UL; k<K; ++k ) {
            for( size_t j=0UL; j<end; j+=2UL ) {
               C(i,j    ) -= A(i,k) * B(k,j    );
               C(i,j+1UL) -= A(i,k) * B(k,j+1UL);
            }
            if( end < N ) {
               C(i,end) -= A(i,k) * B(k,end);
            }
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Vectorized default subtraction assignment to row-major dense matrices***********************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Vectorized default subtraction assignment of a dense matrix-dense matrix
   //        multiplication (\f$ C-=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function implements the vectorized default subtraction assignment of a dense matrix-
   // dense matrix multiplication expression to a row-major dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseVectorizedDefaultKernel<MT3,MT4,MT5> >::Type
      selectDefaultSubAssignKernel( DenseMatrix<MT3,false>& C, const MT4& A, const MT5& B )
   {
      typedef IntrinsicTrait<ElementType>  IT;

      const size_t M( A.rows()    );
      const size_t N( B.spacing() );
      const size_t K( A.columns() );

      size_t j( 0UL );

      for( ; (j+IT::size*8UL) <= N; j+=IT::size*8UL ) {
         for( size_t i=0UL; i<M; ++i ) {
            IntrinsicType xmm1( load( &(~C)(i,j             ) ) );
            IntrinsicType xmm2( load( &(~C)(i,j+IT::size    ) ) );
            IntrinsicType xmm3( load( &(~C)(i,j+IT::size*2UL) ) );
            IntrinsicType xmm4( load( &(~C)(i,j+IT::size*3UL) ) );
            IntrinsicType xmm5( load( &(~C)(i,j+IT::size*4UL) ) );
            IntrinsicType xmm6( load( &(~C)(i,j+IT::size*5UL) ) );
            IntrinsicType xmm7( load( &(~C)(i,j+IT::size*6UL) ) );
            IntrinsicType xmm8( load( &(~C)(i,j+IT::size*7UL) ) );
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i,k) ) );
               xmm1 = xmm1 - a1 * B.get(k,j             );
               xmm2 = xmm2 - a1 * B.get(k,j+IT::size    );
               xmm3 = xmm3 - a1 * B.get(k,j+IT::size*2UL);
               xmm4 = xmm4 - a1 * B.get(k,j+IT::size*3UL);
               xmm5 = xmm5 - a1 * B.get(k,j+IT::size*4UL);
               xmm6 = xmm6 - a1 * B.get(k,j+IT::size*5UL);
               xmm7 = xmm7 - a1 * B.get(k,j+IT::size*6UL);
               xmm8 = xmm8 - a1 * B.get(k,j+IT::size*7UL);
            }
            store( &(~C)(i,j             ), xmm1 );
            store( &(~C)(i,j+IT::size    ), xmm2 );
            store( &(~C)(i,j+IT::size*2UL), xmm3 );
            store( &(~C)(i,j+IT::size*3UL), xmm4 );
            store( &(~C)(i,j+IT::size*4UL), xmm5 );
            store( &(~C)(i,j+IT::size*5UL), xmm6 );
            store( &(~C)(i,j+IT::size*6UL), xmm7 );
            store( &(~C)(i,j+IT::size*7UL), xmm8 );
         }
      }
      for( ; (j+IT::size*4UL) <= N; j+=IT::size*4UL ) {
         size_t i( 0UL );
         for( ; (i+2UL) <= M; i+=2UL ) {
            IntrinsicType xmm1( load( &(~C)(i    ,j             ) ) );
            IntrinsicType xmm2( load( &(~C)(i    ,j+IT::size    ) ) );
            IntrinsicType xmm3( load( &(~C)(i    ,j+IT::size*2UL) ) );
            IntrinsicType xmm4( load( &(~C)(i    ,j+IT::size*3UL) ) );
            IntrinsicType xmm5( load( &(~C)(i+1UL,j             ) ) );
            IntrinsicType xmm6( load( &(~C)(i+1UL,j+IT::size    ) ) );
            IntrinsicType xmm7( load( &(~C)(i+1UL,j+IT::size*2UL) ) );
            IntrinsicType xmm8( load( &(~C)(i+1UL,j+IT::size*3UL) ) );
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i    ,k) ) );
               const IntrinsicType a2( set( A(i+1UL,k) ) );
               const IntrinsicType b1( B.get(k,j             ) );
               const IntrinsicType b2( B.get(k,j+IT::size    ) );
               const IntrinsicType b3( B.get(k,j+IT::size*2UL) );
               const IntrinsicType b4( B.get(k,j+IT::size*3UL) );
               xmm1 = xmm1 - a1 * b1;
               xmm2 = xmm2 - a1 * b2;
               xmm3 = xmm3 - a1 * b3;
               xmm4 = xmm4 - a1 * b4;
               xmm5 = xmm5 - a2 * b1;
               xmm6 = xmm6 - a2 * b2;
               xmm7 = xmm7 - a2 * b3;
               xmm8 = xmm8 - a2 * b4;
            }
            store( &(~C)(i    ,j             ), xmm1 );
            store( &(~C)(i    ,j+IT::size    ), xmm2 );
            store( &(~C)(i    ,j+IT::size*2UL), xmm3 );
            store( &(~C)(i    ,j+IT::size*3UL), xmm4 );
            store( &(~C)(i+1UL,j             ), xmm5 );
            store( &(~C)(i+1UL,j+IT::size    ), xmm6 );
            store( &(~C)(i+1UL,j+IT::size*2UL), xmm7 );
            store( &(~C)(i+1UL,j+IT::size*3UL), xmm8 );
         }
         if( i < M ) {
            IntrinsicType xmm1( load( &(~C)(i,j             ) ) );
            IntrinsicType xmm2( load( &(~C)(i,j+IT::size    ) ) );
            IntrinsicType xmm3( load( &(~C)(i,j+IT::size*2UL) ) );
            IntrinsicType xmm4( load( &(~C)(i,j+IT::size*3UL) ) );
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i,k) ) );
               xmm1 = xmm1 - a1 * B.get(k,j             );
               xmm2 = xmm2 - a1 * B.get(k,j+IT::size    );
               xmm3 = xmm3 - a1 * B.get(k,j+IT::size*2UL);
               xmm4 = xmm4 - a1 * B.get(k,j+IT::size*3UL);
            }
            store( &(~C)(i,j             ), xmm1 );
            store( &(~C)(i,j+IT::size    ), xmm2 );
            store( &(~C)(i,j+IT::size*2UL), xmm3 );
            store( &(~C)(i,j+IT::size*3UL), xmm4 );
         }
      }
      for( ; (j+IT::size*2UL) <= N; j+=IT::size*2UL ) {
         size_t i( 0UL );
         for( ; (i+2UL) <= M; i+=2UL ) {
            IntrinsicType xmm1( load( &(~C)(i    ,j         ) ) );
            IntrinsicType xmm2( load( &(~C)(i    ,j+IT::size) ) );
            IntrinsicType xmm3( load( &(~C)(i+1UL,j         ) ) );
            IntrinsicType xmm4( load( &(~C)(i+1UL,j+IT::size) ) );
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i    ,k) ) );
               const IntrinsicType a2( set( A(i+1UL,k) ) );
               const IntrinsicType b1( B.get(k,j         ) );
               const IntrinsicType b2( B.get(k,j+IT::size) );
               xmm1 = xmm1 - a1 * b1;
               xmm2 = xmm2 - a1 * b2;
               xmm3 = xmm3 - a2 * b1;
               xmm4 = xmm4 - a2 * b2;
            }
            store( &(~C)(i    ,j         ), xmm1 );
            store( &(~C)(i    ,j+IT::size), xmm2 );
            store( &(~C)(i+1UL,j         ), xmm3 );
            store( &(~C)(i+1UL,j+IT::size), xmm4 );
         }
         if( i < M ) {
            IntrinsicType xmm1( load( &(~C)(i,j         ) ) );
            IntrinsicType xmm2( load( &(~C)(i,j+IT::size) ) );
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i,k) ) );
               xmm1 = xmm1 - a1 * B.get(k,j         );
               xmm2 = xmm2 - a1 * B.get(k,j+IT::size);
            }
            store( &(~C)(i,j         ), xmm1 );
            store( &(~C)(i,j+IT::size), xmm2 );
         }
      }
      if( j < N ) {
         size_t i( 0UL );
         for( ; (i+2UL) <= M; i+=2UL ) {
            IntrinsicType xmm1( load( &(~C)(i    ,j) ) );
            IntrinsicType xmm2( load( &(~C)(i+1UL,j) ) );
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType b1( B.get(k,j) );
               xmm1 = xmm1 - set( A(i    ,k) ) * b1;
               xmm2 = xmm2 - set( A(i+1UL,k) ) * b1;
            }
            store( &(~C)(i    ,j), xmm1 );
            store( &(~C)(i+1UL,j), xmm2 );
         }
         if( i < M ) {
            IntrinsicType xmm1( load( &(~C)(i,j) ) );
            for( size_t k=0UL; k<K; ++k ) {
               xmm1 = xmm1 - set( A(i,k) ) * B.get(k,j);
            }
            store( &(~C)(i,j), xmm1 );
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Vectorized default subtraction assignment to column-major dense matrices********************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Vectorized default subtraction assignment of a dense matrix-dense matrix
   //        multiplication (\f$ C-=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function implements the vectorized default subtraction assignment of a dense matrix-
   // dense matrix multiplication expression to a column-major dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseVectorizedDefaultKernel<MT3,MT4,MT5> >::Type
      selectDefaultSubAssignKernel( DenseMatrix<MT3,true>& C, const MT4& A, const MT5& B )
   {
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( typename MT4::OppositeType );
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( typename MT5::OppositeType );

      if( !IsResizable<MT4>::value && IsResizable<MT5>::value ) {
         const typename MT4::OppositeType tmp( A );
         subAssign( ~C, tmp * B );
      }
      else if( IsResizable<MT4>::value && !IsResizable<MT5>::value ) {
         const typename MT5::OppositeType tmp( B );
         subAssign( ~C, A * tmp );
      }
      else if( A.rows() * A.columns() <= B.rows() * B.columns() ) {
         const typename MT4::OppositeType tmp( A );
         subAssign( ~C, tmp * B );
      }
      else {
         const typename MT5::OppositeType tmp( B );
         subAssign( ~C, A * tmp );
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**BLAS-based subtraction assignment to dense matrices (default)*******************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Default subtraction assignment of a dense matrix-dense matrix multiplication
   //        (\f$ C-=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function relays to the default implementation of the subtraction assignment of a dense
   // matrix-dense matrix multiplication expression to a dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseDefaultKernel<MT3,MT4,MT5> >::Type
      selectBlasSubAssignKernel( MT3& C, const MT4& A, const MT5& B )
   {
      selectDefaultSubAssignKernel( C, A, B );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**BLAS-based subraction assignment to dense matrices (single precision)***********************
#if BLAZE_BLAS_MODE
   /*! \cond BLAZE_INTERNAL */
   /*!\brief BLAS-based subraction assignment of a dense matrix-dense matrix multiplication for
   //        single precision matrices (\f$ C-=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function performs the dense matrix-dense matrix multiplication for single precision
   // matrices based on the BLAS cblas_sgemm() function.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseSinglePrecisionKernel<MT3,MT4,MT5> >::Type
      selectBlasSubAssignKernel( MT3& C, const MT4& A, const MT5& B )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename MT3::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename MT4::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename MT5::ElementType );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( B.columns() ) );
      const int K  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const int ldb( numeric_cast<int>( B.spacing() ) );
      const int ldc( numeric_cast<int>( C.spacing() ) );

      cblas_sgemm( ( IsRowMajorMatrix<MT3>::value )?( CblasRowMajor ):( CblasColMajor ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   M, N, K, -1.0F, A.data(), lda, B.data(), ldb, 1.0F, C.data(), ldc );
   }
   /*! \endcond */
#endif
   //**********************************************************************************************

   //**BLAS-based subraction assignment to dense matrices (double precision)***********************
#if BLAZE_BLAS_MODE
   /*! \cond BLAZE_INTERNAL */
   /*!\brief BLAS-based subraction assignment of a dense matrix-dense matrix multiplication for
   //        double precision matrices (\f$ C-=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function performs the dense matrix-dense matrix multiplication for double precision
   // matrices based on the BLAS cblas_dgemm() function.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseDoublePrecisionKernel<MT3,MT4,MT5> >::Type
      selectBlasSubAssignKernel( MT3& C, const MT4& A, const MT5& B )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename MT3::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename MT4::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename MT5::ElementType );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( B.columns() ) );
      const int K  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const int ldb( numeric_cast<int>( B.spacing() ) );
      const int ldc( numeric_cast<int>( C.spacing() ) );

      cblas_dgemm( ( IsRowMajorMatrix<MT3>::value )?( CblasRowMajor ):( CblasColMajor ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   M, N, K, -1.0, A.data(), lda, B.data(), ldb, 1.0, C.data(), ldc );
   }
   /*! \endcond */
#endif
   //**********************************************************************************************

   //**BLAS-based subraction assignment to dense matrices (single precision complex)***************
#if BLAZE_BLAS_MODE
   /*! \cond BLAZE_INTERNAL */
   /*!\brief BLAS-based subraction assignment of a dense matrix-dense matrix multiplication for
   //        single precision complex matrices (\f$ C-=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function performs the dense matrix-dense matrix multiplication for single precision
   // complex matrices based on the BLAS cblas_cgemm() function.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseSinglePrecisionComplexKernel<MT3,MT4,MT5> >::Type
      selectBlasSubAssignKernel( MT3& C, const MT4& A, const MT5& B )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT3::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT4::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT5::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename MT3::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename MT4::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename MT5::ElementType::value_type );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( B.columns() ) );
      const int K  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const int ldb( numeric_cast<int>( B.spacing() ) );
      const int ldc( numeric_cast<int>( C.spacing() ) );
      const complex<float> alpha( -1.0F, 0.0F );
      const complex<float> beta (  1.0F, 0.0F );

      cblas_cgemm( ( IsRowMajorMatrix<MT3>::value )?( CblasRowMajor ):( CblasColMajor ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   M, N, K, &alpha, A.data(), lda, B.data(), ldb, &beta, C.data(), ldc );
   }
   /*! \endcond */
#endif
   //**********************************************************************************************

   //**BLAS-based subraction assignment to dense matrices (double precision complex)***************
#if BLAZE_BLAS_MODE
   /*! \cond BLAZE_INTERNAL */
   /*!\brief BLAS-based subraction assignment of a dense matrix-dense matrix multiplication for
   //        double precision complex matrices (\f$ C-=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function performs the dense matrix-dense matrix multiplication for double precision
   // complex matrices based on the BLAS cblas_zgemm() function.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseDoublePrecisionComplexKernel<MT3,MT4,MT5> >::Type
      selectBlasSubAssignKernel( MT3& C, const MT4& A, const MT5& B )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT3::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT4::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT5::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename MT3::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename MT4::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename MT5::ElementType::value_type );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( B.columns() ) );
      const int K  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const int ldb( numeric_cast<int>( B.spacing() ) );
      const int ldc( numeric_cast<int>( C.spacing() ) );
      const complex<double> alpha( -1.0, 0.0 );
      const complex<double> beta (  1.0, 0.0 );

      cblas_zgemm( ( IsRowMajorMatrix<MT3>::value )?( CblasRowMajor ):( CblasColMajor ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   M, N, K, &alpha, A.data(), lda, B.data(), ldb, &beta, C.data(), ldc );
   }
   /*! \endcond */
#endif
   //**********************************************************************************************

   //**Subtraction assignment to sparse matrices***************************************************
   // No special implementation for the subtraction assignment to sparse matrices.
   //**********************************************************************************************

   //**Multiplication assignment to dense matrices*************************************************
   // No special implementation for the multiplication assignment to dense matrices.
   //**********************************************************************************************

   //**Multiplication assignment to sparse matrices************************************************
   // No special implementation for the multiplication assignment to sparse matrices.
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( MT1 );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT1 );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( MT2 );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT2 );
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************




//=================================================================================================
//
//  DMATSCALARMULTEXPR SPECIALIZATION
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Expression object for scaled dense matrix-dense matrix multiplications.
// \ingroup dense_matrix_expression
//
// This specialization of the DMatScalarMultExpr class represents the compile time expression
// for scaled multiplications between row-major dense matrices.
*/
template< typename MT1   // Type of the left-hand side dense matrix
        , typename MT2   // Type of the right-hand side dense matrix
        , typename ST >  // Type of the right-hand side scalar value
class DMatScalarMultExpr< DMatDMatMultExpr<MT1,MT2>, ST, false >
   : public DenseMatrix< DMatScalarMultExpr< DMatDMatMultExpr<MT1,MT2>, ST, false >, false >
   , private Expression
{
 private:
   //**Type definitions****************************************************************************
   typedef DMatDMatMultExpr<MT1,MT2>    MMM;  //!< Type of the dense matrix multiplication expression.
   typedef typename MMM::ResultType     RES;  //!< Result type of the dense matrix multiplication expression.
   typedef typename MT1::ResultType     RT1;  //!< Result type of the left-hand side dense matrix expression.
   typedef typename MT2::ResultType     RT2;  //!< Result type of the right-hand side dense matrix expression.
   typedef typename MT1::CompositeType  CT1;  //!< Composite type of the left-hand side dense matrix expression.
   typedef typename MT2::CompositeType  CT2;  //!< Composite type of the right-hand side dense matrix expression.
   //**********************************************************************************************

   //**********************************************************************************************
   //! Helper structure for the explicit application of the SFINAE principle.
   /*! In case the data type of all three involved matrices is \a float, the scalar value is
       not a complex data type and the single precision kernel can be used, the nested \a value
       will be set to 1, otherwise it will be 0. */
   template< typename T1, typename T2, typename T3, typename T4 >
   struct UseSinglePrecisionKernel {
      enum { value = IsFloat<typename T1::ElementType>::value &&
                     IsFloat<typename T2::ElementType>::value &&
                     IsFloat<typename T3::ElementType>::value &&
                     !IsComplex<T4>::value };
   };
   //**********************************************************************************************

   //**********************************************************************************************
   //! Helper structure for the explicit application of the SFINAE principle.
   /*! In case the data type of all three involved matrices is \a double, the scalar value is
       not a complex data type and the double precision kernel can be used, the nested \a value
       will be set to 1, otherwise it will be 0. */
   template< typename T1, typename T2, typename T3, typename T4 >
   struct UseDoublePrecisionKernel {
      enum { value = IsDouble<typename T1::ElementType>::value &&
                     IsDouble<typename T2::ElementType>::value &&
                     IsDouble<typename T3::ElementType>::value &&
                     !IsComplex<T4>::value };
   };
   //**********************************************************************************************

   //**********************************************************************************************
   //! Helper structure for the explicit application of the SFINAE principle.
   /*! In case the data type of all three involved matrices is \a complex<float> and the single
       precision complex kernel can be used, the nested \a value will be set to 1, otherwise it
       will be 0. */
   template< typename T1, typename T2, typename T3 >
   struct UseSinglePrecisionComplexKernel {
      typedef complex<float>  Type;
      enum { value = IsSame<typename T1::ElementType,Type>::value &&
                     IsSame<typename T2::ElementType,Type>::value &&
                     IsSame<typename T3::ElementType,Type>::value };
   };
   //**********************************************************************************************

   //**********************************************************************************************
   //! Helper structure for the explicit application of the SFINAE principle.
   /*! In case the data type of all three involved matrices is \a complex<double> and the double
       precision complex kernel can be used, the nested \a value will be set to 1, otherwise it
       will be 0. */
   template< typename T1, typename T2, typename T3 >
   struct UseDoublePrecisionComplexKernel {
      typedef complex<double>  Type;
      enum { value = IsSame<typename T1::ElementType,Type>::value &&
                     IsSame<typename T2::ElementType,Type>::value &&
                     IsSame<typename T3::ElementType,Type>::value };
   };
   //**********************************************************************************************

   //**********************************************************************************************
   //! Helper structure for the explicit application of the SFINAE principle.
   /*! In case no optimized BLAS kernel can be used, the nested \a value will be set to 1,
       otherwise it will be 0. */
   template< typename T1, typename T2, typename T3, typename T4 >
   struct UseDefaultKernel {
      enum { value = !BLAZE_BLAS_MODE || ( !UseSinglePrecisionKernel<T1,T2,T3,T4>::value &&
                                           !UseDoublePrecisionKernel<T1,T2,T3,T4>::value &&
                                           !UseSinglePrecisionComplexKernel<T1,T2,T3>::value &&
                                           !UseDoublePrecisionComplexKernel<T1,T2,T3>::value ) };
   };
   //**********************************************************************************************

   //**********************************************************************************************
   //! Helper structure for the explicit application of the SFINAE principle.
   /*! In case all four involved data types are suited for a vectorized computation of the
       matrix multiplication, the nested \value will be set to 1, otherwise it will be 0. */
   template< typename T1, typename T2, typename T3, typename T4 >
   struct UseVectorizedDefaultKernel {
      enum { value = T1::vectorizable && T2::vectorizable && T3::vectorizable &&
                     IsSame<typename T1::ElementType,typename T2::ElementType>::value &&
                     IsSame<typename T1::ElementType,typename T3::ElementType>::value &&
                     IsSame<typename T1::ElementType,T4>::value &&
                     IntrinsicTrait<typename T1::ElementType>::addition &&
                     IntrinsicTrait<typename T1::ElementType>::multiplication };
   };
   //**********************************************************************************************

 public:
   //**Type definitions****************************************************************************
   typedef DMatScalarMultExpr<MMM,ST,false>            This;           //!< Type of this DMatScalarMultExpr instance.
   typedef typename MathTrait<RES,ST>::MultType        ResultType;     //!< Result type for expression template evaluations.
   typedef typename ResultType::OppositeType           OppositeType;   //!< Result type with opposite storage order for expression template evaluations.
   typedef typename ResultType::TransposeType          TransposeType;  //!< Transpose type for expression template evaluations.
   typedef typename ResultType::ElementType            ElementType;    //!< Resulting element type.
   typedef typename IntrinsicTrait<ElementType>::Type  IntrinsicType;  //!< Resulting intrinsic element type.
   typedef const ResultType                            CompositeType;  //!< Data type for composite expression templates.

   //! Composite type of the left-hand side dense matrix expression.
   typedef const DMatDMatMultExpr<MT1,MT2>  LeftOperand;

   //! Composite type of the right-hand side scalar value.
   typedef typename SelectType< IsNumeric<ElementType>::value, ElementType, ST >::Type  RightOperand;

   //! Type for the assignment of the left-hand side dense matrix operand.
   typedef typename SelectType< IsExpression<MT1>::value, const RT1, CT1 >::Type  LT;

   //! Type for the assignment of the right-hand side dense matrix operand.
   typedef typename SelectType< IsExpression<MT2>::value, const RT2, CT2 >::Type  RT;
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation switch for the expression template evaluation strategy.
   enum { vectorizable = 0 };

   //! Compilation flag for the detection of aliasing effects.
   enum { canAlias = CanAlias<MMM>::value };
   //**********************************************************************************************

   //**Constructor*********************************************************************************
   /*!\brief Constructor for the DMatScalarMultExpr class.
   //
   // \param matrix The left-hand side dense matrix of the multiplication expression.
   // \param scalar The right-hand side scalar of the multiplication expression.
   */
   explicit inline DMatScalarMultExpr( const MMM& matrix, ST scalar )
      : matrix_( matrix )  // Left-hand side dense matrix of the multiplication expression
      , scalar_( scalar )  // Right-hand side scalar of the multiplication expression
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
      BLAZE_INTERNAL_ASSERT( i < matrix_.rows()   , "Invalid row access index"    );
      BLAZE_INTERNAL_ASSERT( j < matrix_.columns(), "Invalid column access index" );
      return matrix_(i,j) * scalar_;
   }
   //**********************************************************************************************

   //**Rows function*******************************************************************************
   /*!\brief Returns the current number of rows of the matrix.
   //
   // \return The number of rows of the matrix.
   */
   inline size_t rows() const {
      return matrix_.rows();
   }
   //**********************************************************************************************

   //**Columns function****************************************************************************
   /*!\brief Returns the current number of columns of the matrix.
   //
   // \return The number of columns of the matrix.
   */
   inline size_t columns() const {
      return matrix_.columns();
   }
   //**********************************************************************************************

   //**Left operand access*************************************************************************
   /*!\brief Returns the left-hand side dense matrix operand.
   //
   // \return The left-hand side dense matrix operand.
   */
   inline LeftOperand leftOperand() const {
      return matrix_;
   }
   //**********************************************************************************************

   //**Right operand access************************************************************************
   /*!\brief Returns the right-hand side scalar operand.
   //
   // \return The right-hand side scalar operand.
   */
   inline RightOperand rightOperand() const {
      return scalar_;
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the expression is aliased with the given address \a alias.
   //
   // \param alias The alias to be checked.
   // \return \a true in case an alias effect is detected, \a false otherwise.
   */
   template< typename T >
   inline bool isAliased( const T* alias ) const {
      return matrix_.isAliased( alias );
   }
   //**********************************************************************************************

 private:
   //**Member variables****************************************************************************
   LeftOperand  matrix_;  //!< Left-hand side dense matrix of the multiplication expression.
   RightOperand scalar_;  //!< Right-hand side scalar of the multiplication expression.
   //**********************************************************************************************

   //**Assignment to dense matrices****************************************************************
   /*!\brief Assignment of a scaled dense matrix-dense matrix multiplication to a dense matrix
   //        (\f$ C=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side scaled multiplication expression to be assigned.
   // \return void
   */
   template< typename MT3  // Type of the target dense matrix
           , bool SO >     // Storage order of the target dense matrix
   friend inline void assign( DenseMatrix<MT3,SO>& lhs, const DMatScalarMultExpr& rhs )
   {
      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      typename MMM::LeftOperand  left ( rhs.matrix_.leftOperand()  );
      typename MMM::RightOperand right( rhs.matrix_.rightOperand() );

      if( (~lhs).rows() == 0UL || (~lhs).columns() == 0UL ) {
         return;
      }
      else if( left.columns() == 0UL ) {
         reset( ~lhs );
         return;
      }

      LT A( left  );  // Evaluation of the left-hand side dense matrix operand
      RT B( right );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == left.rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == left.columns()  , "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == right.rows()    , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == right.columns() , "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns(), "Invalid number of columns" );

      if( (~lhs).rows() * (~lhs).columns() < DMATDMATMULT_THRESHOLD )
         DMatScalarMultExpr::selectDefaultAssignKernel( ~lhs, A, B, rhs.scalar_ );
      else
         DMatScalarMultExpr::selectBlasAssignKernel( ~lhs, A, B, rhs.scalar_ );
   }
   //**********************************************************************************************

   //**Default assignment to dense matrices********************************************************
   /*!\brief Default assignment of a scaled dense matrix-dense matrix multiplication
   //        (\f$ C=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function implements the default assignment of a scaled dense matrix-dense matrix
   // multiplication expression to a dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename DisableIf< UseVectorizedDefaultKernel<MT3,MT4,MT5,ST2> >::Type
      selectDefaultAssignKernel( MT3& C, const MT4& A, const MT5& B, ST2 scalar )
   {
      const size_t M( A.rows()    );
      const size_t N( B.columns() );
      const size_t K( A.columns() );

      for( size_t i=0UL; i<M; ++i ) {
         for( size_t j=0UL; j<N; ++j ) {
            C(i,j) = A(i,0UL) * B(0UL,j);
         }
         for( size_t k=1UL; k<K; ++k ) {
            for( size_t j=0UL; j<N; ++j ) {
               C(i,j) += A(i,k) * B(k,j);
            }
         }
         for( size_t j=0UL; j<N; ++j ) {
            C(i,j) *= scalar;
         }
      }
   }
   //**********************************************************************************************

   //**Vectorized default assignment to row-major dense matrices***********************************
   /*!\brief Vectorized default assignment of a scaled dense matrix-dense matrix multiplication
   //        (\f$ C=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function implements the vectorized default assignment of a scaled dense matrix-dense
   // matrix multiplication expression to a row-major dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseVectorizedDefaultKernel<MT3,MT4,MT5,ST2> >::Type
      selectDefaultAssignKernel( DenseMatrix<MT3,false>& C, const MT4& A, const MT5& B, ST2 scalar )
   {
      typedef IntrinsicTrait<ElementType>  IT;

      const size_t M( A.rows()    );
      const size_t N( B.spacing() );
      const size_t K( A.columns() );

      const IntrinsicType factor( set( scalar ) );

      size_t j( 0UL );

      for( ; (j+IT::size*8UL) <= N; j+=IT::size*8UL ) {
         for( size_t i=0UL; i<M; ++i ) {
            IntrinsicType xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8;
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i,k) ) );
               xmm1 = xmm1 + a1 * B.get(k,j             );
               xmm2 = xmm2 + a1 * B.get(k,j+IT::size    );
               xmm3 = xmm3 + a1 * B.get(k,j+IT::size*2UL);
               xmm4 = xmm4 + a1 * B.get(k,j+IT::size*3UL);
               xmm5 = xmm5 + a1 * B.get(k,j+IT::size*4UL);
               xmm6 = xmm6 + a1 * B.get(k,j+IT::size*5UL);
               xmm7 = xmm7 + a1 * B.get(k,j+IT::size*6UL);
               xmm8 = xmm8 + a1 * B.get(k,j+IT::size*7UL);
            }
            store( &(~C)(i,j             ), xmm1 * factor );
            store( &(~C)(i,j+IT::size    ), xmm2 * factor );
            store( &(~C)(i,j+IT::size*2UL), xmm3 * factor );
            store( &(~C)(i,j+IT::size*3UL), xmm4 * factor );
            store( &(~C)(i,j+IT::size*4UL), xmm5 * factor );
            store( &(~C)(i,j+IT::size*5UL), xmm6 * factor );
            store( &(~C)(i,j+IT::size*6UL), xmm7 * factor );
            store( &(~C)(i,j+IT::size*7UL), xmm8 * factor );
         }
      }
      for( ; (j+IT::size*4UL) <= N; j+=IT::size*4UL ) {
         size_t i( 0UL );
         for( ; (i+2UL) <= M; i+=2UL ) {
            IntrinsicType xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8;
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i    ,k) ) );
               const IntrinsicType a2( set( A(i+1UL,k) ) );
               const IntrinsicType b1( B.get(k,j             ) );
               const IntrinsicType b2( B.get(k,j+IT::size    ) );
               const IntrinsicType b3( B.get(k,j+IT::size*2UL) );
               const IntrinsicType b4( B.get(k,j+IT::size*3UL) );
               xmm1 = xmm1 + a1 * b1;
               xmm2 = xmm2 + a1 * b2;
               xmm3 = xmm3 + a1 * b3;
               xmm4 = xmm4 + a1 * b4;
               xmm5 = xmm5 + a2 * b1;
               xmm6 = xmm6 + a2 * b2;
               xmm7 = xmm7 + a2 * b3;
               xmm8 = xmm8 + a2 * b4;
            }
            store( &(~C)(i    ,j             ), xmm1 * factor );
            store( &(~C)(i    ,j+IT::size    ), xmm2 * factor );
            store( &(~C)(i    ,j+IT::size*2UL), xmm3 * factor );
            store( &(~C)(i    ,j+IT::size*3UL), xmm4 * factor );
            store( &(~C)(i+1UL,j             ), xmm5 * factor );
            store( &(~C)(i+1UL,j+IT::size    ), xmm6 * factor );
            store( &(~C)(i+1UL,j+IT::size*2UL), xmm7 * factor );
            store( &(~C)(i+1UL,j+IT::size*3UL), xmm8 * factor );
         }
         if( i < M ) {
            IntrinsicType xmm1, xmm2, xmm3, xmm4;
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i,k) ) );
               xmm1 = xmm1 + a1 * B.get(k,j             );
               xmm2 = xmm2 + a1 * B.get(k,j+IT::size    );
               xmm3 = xmm3 + a1 * B.get(k,j+IT::size*2UL);
               xmm4 = xmm4 + a1 * B.get(k,j+IT::size*3UL);
            }
            store( &(~C)(i,j             ), xmm1 * factor );
            store( &(~C)(i,j+IT::size    ), xmm2 * factor );
            store( &(~C)(i,j+IT::size*2UL), xmm3 * factor );
            store( &(~C)(i,j+IT::size*3UL), xmm4 * factor );
         }
      }
      for( ; (j+IT::size*2UL) <= N; j+=IT::size*2UL ) {
         size_t i( 0UL );
         for( ; (i+2UL) <= M; i+=2UL ) {
            IntrinsicType xmm1, xmm2, xmm3, xmm4;
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i    ,k) ) );
               const IntrinsicType a2( set( A(i+1UL,k) ) );
               const IntrinsicType b1( B.get(k,j         ) );
               const IntrinsicType b2( B.get(k,j+IT::size) );
               xmm1 = xmm1 + a1 * b1;
               xmm2 = xmm2 + a1 * b2;
               xmm3 = xmm3 + a2 * b1;
               xmm4 = xmm4 + a2 * b2;
            }
            store( &(~C)(i    ,j         ), xmm1 * factor );
            store( &(~C)(i    ,j+IT::size), xmm2 * factor );
            store( &(~C)(i+1UL,j         ), xmm3 * factor );
            store( &(~C)(i+1UL,j+IT::size), xmm4 * factor );
         }
         if( i < M ) {
            IntrinsicType xmm1, xmm2;
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i,k) ) );
               xmm1 = xmm1 + a1 * B.get(k,j         );
               xmm2 = xmm2 + a1 * B.get(k,j+IT::size);
            }
            store( &(~C)(i,j         ), xmm1 * factor );
            store( &(~C)(i,j+IT::size), xmm2 * factor );
         }
      }
      if( j < N ) {
         size_t i( 0UL );
         for( ; (i+2UL) <= M; i+=2UL ) {
            IntrinsicType xmm1, xmm2;
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType b1( B.get(k,j) );
               xmm1 = xmm1 + set( A(i    ,k) ) * b1;
               xmm2 = xmm2 + set( A(i+1UL,k) ) * b1;
            }
            store( &(~C)(i    ,j), xmm1 * factor );
            store( &(~C)(i+1UL,j), xmm2 * factor );
         }
         if( i < M ) {
            IntrinsicType xmm1;
            for( size_t k=0UL; k<K; ++k ) {
               xmm1 = xmm1 + set( A(i,k) ) * B.get(k,j);
            }
            store( &(~C)(i,j), xmm1 * factor );
         }
      }
   }
   //**********************************************************************************************

   //**Vectorized default assignment to column-major dense matrices********************************
   /*!\brief Vectorized default assignment of a scaled dense matrix-dense matrix multiplication
   //        (\f$ C=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function implements the vectorized default assignment of a scaled dense matrix-dense
   // matrix multiplication expression to a column-major dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseVectorizedDefaultKernel<MT3,MT4,MT5,ST2> >::Type
      selectDefaultAssignKernel( DenseMatrix<MT3,true>& C, const MT4& A, const MT5& B, ST2 scalar )
   {
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( typename MT4::OppositeType );
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( typename MT5::OppositeType );

      if( !IsResizable<MT4>::value && IsResizable<MT5>::value ) {
         const typename MT4::OppositeType tmp( A );
         assign( ~C, tmp * B * scalar );
      }
      else if( IsResizable<MT4>::value && !IsResizable<MT5>::value ) {
         const typename MT5::OppositeType tmp( B );
         assign( ~C, A * tmp * scalar );
      }
      else if( A.rows() * A.columns() <= B.rows() * B.columns() ) {
         const typename MT4::OppositeType tmp( A );
         assign( ~C, tmp * B * scalar );
      }
      else {
         const typename MT5::OppositeType tmp( B );
         assign( ~C, A * tmp * scalar );
      }
   }
   //**********************************************************************************************

   //**BLAS-based assignment to dense matrices (default)*******************************************
   /*!\brief Default assignment of a scaled dense matrix-dense matrix multiplication
   //        (\f$ C=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function relays to the default implementation of the assignment of a scaled dense
   // matrix-dense matrix multiplication expression to a dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseDefaultKernel<MT3,MT4,MT5,ST2> >::Type
      selectBlasAssignKernel( MT3& C, const MT4& A, const MT5& B, ST2 scalar )
   {
      selectDefaultAssignKernel( C, A, B, scalar );
   }
   //**********************************************************************************************

   //**BLAS-based assignment to dense matrices (single precision)**********************************
#if BLAZE_BLAS_MODE
   /*!\brief BLAS-based assignment of a scaled dense matrix-dense matrix multiplication for
   //        single precision matrices (\f$ C=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function performs the scaled dense matrix-dense matrix multiplication for single
   // precision matrices based on the BLAS cblas_sgemm() function.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseSinglePrecisionKernel<MT3,MT4,MT5,ST2> >::Type
      selectBlasAssignKernel( MT3& C, const MT4& A, const MT5& B, ST2 scalar )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename MT3::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename MT4::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename MT5::ElementType );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( B.columns() ) );
      const int K  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const int ldb( numeric_cast<int>( B.spacing() ) );
      const int ldc( numeric_cast<int>( C.spacing() ) );

      cblas_sgemm( ( IsRowMajorMatrix<MT3>::value )?( CblasRowMajor ):( CblasColMajor ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   M, N, K, scalar, A.data(), lda, B.data(), ldb, 0.0F, C.data(), ldc );
   }
#endif
   //**********************************************************************************************

   //**BLAS-based assignment to dense matrices (double precision)**********************************
#if BLAZE_BLAS_MODE
   /*!\brief BLAS-based assignment of a scaled dense matrix-dense matrix multiplication for
   //        double precision matrices (\f$ C=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function performs the scaled dense matrix-dense matrix multiplication for double
   // precision matrices based on the BLAS cblas_dgemm() function.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseDoublePrecisionKernel<MT3,MT4,MT5,ST2> >::Type
      selectBlasAssignKernel( MT3& C, const MT4& A, const MT5& B, ST2 scalar )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename MT3::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename MT4::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename MT5::ElementType );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( B.columns() ) );
      const int K  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const int ldb( numeric_cast<int>( B.spacing() ) );
      const int ldc( numeric_cast<int>( C.spacing() ) );

      cblas_dgemm( ( IsRowMajorMatrix<MT3>::value )?( CblasRowMajor ):( CblasColMajor ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   M, N, K, scalar, A.data(), lda, B.data(), ldb, 0.0, C.data(), ldc );
   }
#endif
   //**********************************************************************************************

   //**BLAS-based assignment to dense matrices (single precision complex)**************************
#if BLAZE_BLAS_MODE
   /*!\brief BLAS-based assignment of a scaled dense matrix-dense matrix multiplication for
   //        single precision complex matrices (\f$ C=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function performs the scaled dense matrix-dense matrix multiplication for single
   // precision complex matrices based on the BLAS cblas_cgemm() function.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseSinglePrecisionComplexKernel<MT3,MT4,MT5> >::Type
      selectBlasAssignKernel( MT3& C, const MT4& A, const MT5& B, ST2 scalar )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT3::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT4::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT5::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( ST2 );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename MT3::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename MT4::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename MT5::ElementType::value_type );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( B.columns() ) );
      const int K  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const int ldb( numeric_cast<int>( B.spacing() ) );
      const int ldc( numeric_cast<int>( C.spacing() ) );
      const complex<float> alpha( scalar );
      const complex<float> beta ( 0.0F, 0.0F );

      cblas_cgemm( ( IsRowMajorMatrix<MT3>::value )?( CblasRowMajor ):( CblasColMajor ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   M, N, K, &alpha, A.data(), lda, B.data(), ldb, &beta, C.data(), ldc );
   }
#endif
   //**********************************************************************************************

   //**BLAS-based assignment to dense matrices (double precision complex)**************************
#if BLAZE_BLAS_MODE
   /*!\brief BLAS-based assignment of a scaled dense matrix-dense matrix multiplication for
   //        double precision complex matrices (\f$ C=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function performs the scaled dense matrix-dense matrix multiplication for double
   // precision complex matrices based on the BLAS cblas_zgemm() function.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar
   static inline typename EnableIf< UseDoublePrecisionComplexKernel<MT3,MT4,MT5> >::Type
      selectBlasAssignKernel( MT3& C, const MT4& A, const MT5& B, ST2 scalar )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT3::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT4::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT5::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( ST2 );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename MT3::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename MT4::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename MT5::ElementType::value_type );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( B.columns() ) );
      const int K  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const int ldb( numeric_cast<int>( B.spacing() ) );
      const int ldc( numeric_cast<int>( C.spacing() ) );
      const complex<double> alpha( scalar );
      const complex<double> beta ( 0.0, 0.0 );

      cblas_zgemm( ( IsRowMajorMatrix<MT3>::value )?( CblasRowMajor ):( CblasColMajor ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   M, N, K, &alpha, A.data(), lda, B.data(), ldb, &beta, C.data(), ldc );
   }
#endif
   //**********************************************************************************************

   //**Assignment to sparse matrices***************************************************************
   /*!\brief Assignment of a scaled dense matrix-dense matrix multiplication to a sparse matrix.
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side sparse matrix.
   // \param rhs The right-hand side scaled multiplication expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized assignment of a scaled dense matrix-
   // dense matrix multiplication expression to a sparse matrix.
   */
   template< typename MT  // Type of the target sparse matrix
           , bool SO >    // Storage order of the target sparse matrix
   friend inline void assign( SparseMatrix<MT,SO>& lhs, const DMatScalarMultExpr& rhs )
   {
      typedef typename SelectType< SO, OppositeType, ResultType >::Type  TmpType;

      BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( OppositeType );
      BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( OppositeType );
      BLAZE_CONSTRAINT_MATRICES_MUST_HAVE_SAME_STORAGE_ORDER( MT, TmpType );
      BLAZE_CONSTRAINT_MUST_BE_REFERENCE_TYPE( typename TmpType::CompositeType );

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      const TmpType tmp( rhs );
      assign( ~lhs, tmp );
   }
   //**********************************************************************************************

   //**Addition assignment to dense matrices*******************************************************
   /*!\brief Addition assignment of a scaled dense matrix-dense matrix multiplication to a
   //        dense matrix (\f$ C+=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side multiplication expression to be added.
   // \return void
   //
   // This function implements the performance optimized addition assignment of a scaled dense
   // matrix-dense matrix multiplication expression to a dense matrix.
   */
   template< typename MT3  // Type of the target dense matrix
           , bool SO >     // Storage order of the target dense matrix
   friend inline void addAssign( DenseMatrix<MT3,SO>& lhs, const DMatScalarMultExpr& rhs )
   {
      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      typename MMM::LeftOperand  left ( rhs.matrix_.leftOperand()  );
      typename MMM::RightOperand right( rhs.matrix_.rightOperand() );

      if( (~lhs).rows() == 0UL || (~lhs).columns() == 0UL || left.columns() == 0UL ) {
         return;
      }

      LT A( left  );  // Evaluation of the left-hand side dense matrix operand
      RT B( right );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == left.rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == left.columns()  , "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == right.rows()    , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == right.columns() , "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns(), "Invalid number of columns" );

      if( (~lhs).rows() * (~lhs).columns() < DMATDMATMULT_THRESHOLD )
         DMatScalarMultExpr::selectDefaultAddAssignKernel( ~lhs, A, B, rhs.scalar_ );
      else
         DMatScalarMultExpr::selectBlasAddAssignKernel( ~lhs, A, B, rhs.scalar_ );
   }
   //**********************************************************************************************

   //**Default addition assignment to dense matrices***********************************************
   /*!\brief Default addition assignment of a scaled dense matrix-dense matrix multiplication
   //        (\f$ C+=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function implements the default addition assignment of a scaled dense matrix-dense
   // matrix multiplication expression to a dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename DisableIf< UseVectorizedDefaultKernel<MT3,MT4,MT5,ST2> >::Type
      selectDefaultAddAssignKernel( MT3& C, const MT4& A, const MT5& B, ST2 scalar )
   {
      const ResultType tmp( A * B * scalar );
      addAssign( C, tmp );
   }
   //**********************************************************************************************

   //**Vectorized default addition assignment to row-major dense matrices**************************
   /*!\brief Vectorized default addition assignment of a scaled dense matrix-dense matrix
   //        multiplication (\f$ C+=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function implements the vectorized default addition assignment of a scaled dense
   // matrix-dense matrix multiplication expression to a row-major dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseVectorizedDefaultKernel<MT3,MT4,MT5,ST2> >::Type
      selectDefaultAddAssignKernel( DenseMatrix<MT3,false>& C, const MT4& A, const MT5& B, ST2 scalar )
   {
      typedef IntrinsicTrait<ElementType>  IT;

      const size_t M( A.rows()    );
      const size_t N( B.spacing() );
      const size_t K( A.columns() );

      const IntrinsicType factor( set( scalar ) );

      size_t j( 0UL );

      for( ; (j+IT::size*8UL) <= N; j+=IT::size*8UL ) {
         for( size_t i=0UL; i<M; ++i ) {
            IntrinsicType xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8;
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i,k) ) );
               xmm1 = xmm1 + a1 * B.get(k,j             );
               xmm2 = xmm2 + a1 * B.get(k,j+IT::size    );
               xmm3 = xmm3 + a1 * B.get(k,j+IT::size*2UL);
               xmm4 = xmm4 + a1 * B.get(k,j+IT::size*3UL);
               xmm5 = xmm5 + a1 * B.get(k,j+IT::size*4UL);
               xmm6 = xmm6 + a1 * B.get(k,j+IT::size*5UL);
               xmm7 = xmm7 + a1 * B.get(k,j+IT::size*6UL);
               xmm8 = xmm8 + a1 * B.get(k,j+IT::size*7UL);
            }
            store( &(~C)(i,j             ), load( &(~C)(i,j             ) ) + xmm1 * factor );
            store( &(~C)(i,j+IT::size    ), load( &(~C)(i,j+IT::size    ) ) + xmm2 * factor );
            store( &(~C)(i,j+IT::size*2UL), load( &(~C)(i,j+IT::size*2UL) ) + xmm3 * factor );
            store( &(~C)(i,j+IT::size*3UL), load( &(~C)(i,j+IT::size*3UL) ) + xmm4 * factor );
            store( &(~C)(i,j+IT::size*4UL), load( &(~C)(i,j+IT::size*4UL) ) + xmm5 * factor );
            store( &(~C)(i,j+IT::size*5UL), load( &(~C)(i,j+IT::size*5UL) ) + xmm6 * factor );
            store( &(~C)(i,j+IT::size*6UL), load( &(~C)(i,j+IT::size*6UL) ) + xmm7 * factor );
            store( &(~C)(i,j+IT::size*7UL), load( &(~C)(i,j+IT::size*7UL) ) + xmm8 * factor );
         }
      }
      for( ; (j+IT::size*4UL) <= N; j+=IT::size*4UL ) {
         size_t i( 0UL );
         for( ; (i+2UL) <= M; i+=2UL ) {
            IntrinsicType xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8;
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i    ,k) ) );
               const IntrinsicType a2( set( A(i+1UL,k) ) );
               const IntrinsicType b1( B.get(k,j             ) );
               const IntrinsicType b2( B.get(k,j+IT::size    ) );
               const IntrinsicType b3( B.get(k,j+IT::size*2UL) );
               const IntrinsicType b4( B.get(k,j+IT::size*3UL) );
               xmm1 = xmm1 + a1 * b1;
               xmm2 = xmm2 + a1 * b2;
               xmm3 = xmm3 + a1 * b3;
               xmm4 = xmm4 + a1 * b4;
               xmm5 = xmm5 + a2 * b1;
               xmm6 = xmm6 + a2 * b2;
               xmm7 = xmm7 + a2 * b3;
               xmm8 = xmm8 + a2 * b4;
            }
            store( &(~C)(i    ,j             ), load( &(~C)(i    ,j             ) ) + xmm1 * factor );
            store( &(~C)(i    ,j+IT::size    ), load( &(~C)(i    ,j+IT::size    ) ) + xmm2 * factor );
            store( &(~C)(i    ,j+IT::size*2UL), load( &(~C)(i    ,j+IT::size*2UL) ) + xmm3 * factor );
            store( &(~C)(i    ,j+IT::size*3UL), load( &(~C)(i    ,j+IT::size*3UL) ) + xmm4 * factor );
            store( &(~C)(i+1UL,j             ), load( &(~C)(i+1UL,j             ) ) + xmm5 * factor );
            store( &(~C)(i+1UL,j+IT::size    ), load( &(~C)(i+1UL,j+IT::size    ) ) + xmm6 * factor );
            store( &(~C)(i+1UL,j+IT::size*2UL), load( &(~C)(i+1UL,j+IT::size*2UL) ) + xmm7 * factor );
            store( &(~C)(i+1UL,j+IT::size*3UL), load( &(~C)(i+1UL,j+IT::size*3UL) ) + xmm8 * factor );
         }
         if( i < M ) {
            IntrinsicType xmm1, xmm2, xmm3, xmm4;
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i,k) ) );
               xmm1 = xmm1 + a1 * B.get(k,j             );
               xmm2 = xmm2 + a1 * B.get(k,j+IT::size    );
               xmm3 = xmm3 + a1 * B.get(k,j+IT::size*2UL);
               xmm4 = xmm4 + a1 * B.get(k,j+IT::size*3UL);
            }
            store( &(~C)(i,j             ), load( &(~C)(i,j             ) ) + xmm1 * factor );
            store( &(~C)(i,j+IT::size    ), load( &(~C)(i,j+IT::size    ) ) + xmm2 * factor );
            store( &(~C)(i,j+IT::size*2UL), load( &(~C)(i,j+IT::size*2UL) ) + xmm3 * factor );
            store( &(~C)(i,j+IT::size*3UL), load( &(~C)(i,j+IT::size*3UL) ) + xmm4 * factor );
         }
      }
      for( ; (j+IT::size*2UL) <= N; j+=IT::size*2UL ) {
         size_t i( 0UL );
         for( ; (i+2UL) <= M; i+=2UL ) {
            IntrinsicType xmm1, xmm2, xmm3, xmm4;
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i    ,k) ) );
               const IntrinsicType a2( set( A(i+1UL,k) ) );
               const IntrinsicType b1( B.get(k,j         ) );
               const IntrinsicType b2( B.get(k,j+IT::size) );
               xmm1 = xmm1 + a1 * b1;
               xmm2 = xmm2 + a1 * b2;
               xmm3 = xmm3 + a2 * b1;
               xmm4 = xmm4 + a2 * b2;
            }
            store( &(~C)(i    ,j         ), load( &(~C)(i    ,j         ) ) + xmm1 * factor );
            store( &(~C)(i    ,j+IT::size), load( &(~C)(i    ,j+IT::size) ) + xmm2 * factor );
            store( &(~C)(i+1UL,j         ), load( &(~C)(i+1UL,j         ) ) + xmm3 * factor );
            store( &(~C)(i+1UL,j+IT::size), load( &(~C)(i+1UL,j+IT::size) ) + xmm4 * factor );
         }
         if( i < M ) {
            IntrinsicType xmm1, xmm2;
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i,k) ) );
               xmm1 = xmm1 + a1 * B.get(k,j         );
               xmm2 = xmm2 + a1 * B.get(k,j+IT::size);
            }
            store( &(~C)(i,j         ), load( &(~C)(i,j         ) ) + xmm1 * factor );
            store( &(~C)(i,j+IT::size), load( &(~C)(i,j+IT::size) ) + xmm2 * factor );
         }
      }
      if( j < N ) {
         size_t i( 0UL );
         for( ; (i+2UL) <= M; i+=2UL ) {
            IntrinsicType xmm1, xmm2;
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType b1( B.get(k,j) );
               xmm1 = xmm1 + set( A(i    ,k) ) * b1;
               xmm2 = xmm2 + set( A(i+1UL,k) ) * b1;
            }
            store( &(~C)(i    ,j), load( &(~C)(i    ,j) ) + xmm1 * factor );
            store( &(~C)(i+1UL,j), load( &(~C)(i+1UL,j) ) + xmm2 * factor );
         }
         if( i < M ) {
            IntrinsicType xmm1;
            for( size_t k=0UL; k<K; ++k ) {
               xmm1 = xmm1 + set( A(i,k) ) * B.get(k,j);
            }
            store( &(~C)(i,j), load( &(~C)(i,j) ) + xmm1 * factor );
         }
      }
   }
   //**********************************************************************************************

   //**Vectorized default addition assignment to column-major dense matrices***********************
   /*!\brief Vectorized default addition assignment of a scaled dense matrix-dense matrix
   //        multiplication (\f$ C+=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function implements the vectorized default addition assignment of a scaled dense
   // matrix-dense matrix multiplication expression to a column-major dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseVectorizedDefaultKernel<MT3,MT4,MT5,ST2> >::Type
      selectDefaultAddAssignKernel( DenseMatrix<MT3,true>& C, const MT4& A, const MT5& B, ST2 scalar )
   {
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( typename MT4::OppositeType );
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( typename MT5::OppositeType );

      if( !IsResizable<MT4>::value && IsResizable<MT5>::value ) {
         const typename MT4::OppositeType tmp( A );
         addAssign( ~C, tmp * B * scalar );
      }
      else if( IsResizable<MT4>::value && !IsResizable<MT5>::value ) {
         const typename MT5::OppositeType tmp( B );
         addAssign( ~C, A * tmp * scalar );
      }
      else if( A.rows() * A.columns() <= B.rows() * B.columns() ) {
         const typename MT4::OppositeType tmp( A );
         addAssign( ~C, tmp * B * scalar );
      }
      else {
         const typename MT5::OppositeType tmp( B );
         addAssign( ~C, A * tmp * scalar );
      }
   }
   //**********************************************************************************************

   //**BLAS-based addition assignment to dense matrices (default)**********************************
   /*!\brief Default addition assignment of a scaled dense matrix-dense matrix multiplication
   //        (\f$ C+=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function relays to the default implementation of the addition assignment of a scaled
   // dense matrix-dense matrix multiplication expression to a dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseDefaultKernel<MT3,MT4,MT5,ST2> >::Type
      selectBlasAddAssignKernel( MT3& C, const MT4& A, const MT5& B, ST2 scalar )
   {
      selectDefaultAddAssignKernel( C, A, B, scalar );
   }
   //**********************************************************************************************

   //**BLAS-based addition assignment to dense matrices (single precision)*************************
#if BLAZE_BLAS_MODE
   /*!\brief BLAS-based addition assignment of a scaled dense matrix-dense matrix multiplication
   //        for single precision matrices (\f$ C+=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function performs the scaled dense matrix-dense matrix multiplication for single
   // precision matrices based on the BLAS cblas_sgemm() function.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseSinglePrecisionKernel<MT3,MT4,MT5,ST2> >::Type
      selectBlasAddAssignKernel( MT3& C, const MT4& A, const MT5& B, ST2 scalar )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename MT3::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename MT4::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename MT5::ElementType );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( B.columns() ) );
      const int K  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const int ldb( numeric_cast<int>( B.spacing() ) );
      const int ldc( numeric_cast<int>( C.spacing() ) );

      cblas_sgemm( ( IsRowMajorMatrix<MT3>::value )?( CblasRowMajor ):( CblasColMajor ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   M, N, K, scalar, A.data(), lda, B.data(), ldb, 1.0F, C.data(), ldc );
   }
#endif
   //**********************************************************************************************

   //**BLAS-based addition assignment to dense matrices (double precision)*************************
#if BLAZE_BLAS_MODE
   /*!\brief BLAS-based addition assignment of a scaled dense matrix-dense matrix multiplication
   //        for double precision matrices (\f$ C+=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \param scalar The scaling factor
   // \return void
   //
   // This function performs the scaled dense matrix-dense matrix multiplication for double
   // precision matrices based on the BLAS cblas_dgemm() function.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseDoublePrecisionKernel<MT3,MT4,MT5,ST2> >::Type
      selectBlasAddAssignKernel( MT3& C, const MT4& A, const MT5& B, ST2 scalar )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename MT3::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename MT4::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename MT5::ElementType );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( B.columns() ) );
      const int K  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const int ldb( numeric_cast<int>( B.spacing() ) );
      const int ldc( numeric_cast<int>( C.spacing() ) );

      cblas_dgemm( ( IsRowMajorMatrix<MT3>::value )?( CblasRowMajor ):( CblasColMajor ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   M, N, K, scalar, A.data(), lda, B.data(), ldb, 1.0, C.data(), ldc );
   }
#endif
   //**********************************************************************************************

   //**BLAS-based addition assignment to dense matrices (single precision complex)*****************
#if BLAZE_BLAS_MODE
   /*!\brief BLAS-based addition assignment of a scaled dense matrix-dense matrix multiplication
   //        for single precision complex matrices (\f$ C+=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \param scalar The scaling factor
   // \return void
   //
   // This function performs the scaled dense matrix-dense matrix multiplication for single
   // precision complex matrices based on the BLAS cblas_cgemm() function.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseSinglePrecisionComplexKernel<MT3,MT4,MT5> >::Type
      selectBlasAddAssignKernel( MT3& C, const MT4& A, const MT5& B, ST2 scalar )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT3::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT4::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT5::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( ST2 );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename MT3::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename MT4::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename MT5::ElementType::value_type );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( B.columns() ) );
      const int K  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const int ldb( numeric_cast<int>( B.spacing() ) );
      const int ldc( numeric_cast<int>( C.spacing() ) );
      const complex<float> alpha( scalar );
      const complex<float> beta ( 1.0F, 0.0F );

      cblas_cgemm( ( IsRowMajorMatrix<MT3>::value )?( CblasRowMajor ):( CblasColMajor ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   M, N, K, &alpha, A.data(), lda, B.data(), ldb, &beta, C.data(), ldc );
   }
#endif
   //**********************************************************************************************

   //**BLAS-based addition assignment to dense matrices (double precision complex)*****************
#if BLAZE_BLAS_MODE
   /*!\brief BLAS-based addition assignment of a scaled dense matrix-dense matrix multiplication
   //        for double precision complex matrices (\f$ C+=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \param scalar The scaling factor
   // \return void
   //
   // This function performs the scaled dense matrix-dense matrix multiplication for double
   // precision complex matrices based on the BLAS cblas_zgemm() function.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseDoublePrecisionComplexKernel<MT3,MT4,MT5> >::Type
      selectBlasAddAssignKernel( MT3& C, const MT4& A, const MT5& B, ST2 scalar )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT3::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT4::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT5::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( ST2 );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename MT3::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename MT4::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename MT5::ElementType::value_type );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( B.columns() ) );
      const int K  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const int ldb( numeric_cast<int>( B.spacing() ) );
      const int ldc( numeric_cast<int>( C.spacing() ) );
      const complex<double> alpha( scalar );
      const complex<double> beta ( 1.0, 0.0 );

      cblas_zgemm( ( IsRowMajorMatrix<MT3>::value )?( CblasRowMajor ):( CblasColMajor ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   M, N, K, &alpha, A.data(), lda, B.data(), ldb, &beta, C.data(), ldc );
   }
#endif
   //**********************************************************************************************

   //**Addition assignment to sparse matrices******************************************************
   // No special implementation for the addition assignment to sparse matrices.
   //**********************************************************************************************

   //**Subtraction assignment to dense matrices****************************************************
   /*!\brief Subtraction assignment of a scaled dense matrix-dense matrix multiplication to a
   //        dense matrix (\f$ C-=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side multiplication expression to be subtracted.
   // \return void
   //
   // This function implements the performance optimized subtraction assignment of a scaled dense
   // matrix-dense matrix multiplication expression to a dense matrix.
   */
   template< typename MT3  // Type of the target dense matrix
           , bool SO >     // Storage order of the target dense matrix
   friend inline void subAssign( DenseMatrix<MT3,SO>& lhs, const DMatScalarMultExpr& rhs )
   {
      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      typename MMM::LeftOperand  left ( rhs.matrix_.leftOperand()  );
      typename MMM::RightOperand right( rhs.matrix_.rightOperand() );

      if( (~lhs).rows() == 0UL || (~lhs).columns() == 0UL || left.columns() == 0UL ) {
         return;
      }

      LT A( left  );  // Evaluation of the left-hand side dense matrix operand
      RT B( right );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == left.rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == left.columns()  , "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == right.rows()    , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == right.columns() , "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns(), "Invalid number of columns" );

      if( (~lhs).rows() * (~lhs).columns() < DMATDMATMULT_THRESHOLD )
         DMatScalarMultExpr::selectDefaultSubAssignKernel( ~lhs, A, B, rhs.scalar_ );
      else
         DMatScalarMultExpr::selectBlasSubAssignKernel( ~lhs, A, B, rhs.scalar_ );
   }
   //**********************************************************************************************

   //**Default subtraction assignment to dense matrices********************************************
   /*!\brief Default subtraction assignment of a scaled dense matrix-dense matrix multiplication
   //        (\f$ C-=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function implements the default subtraction assignment of a scaled dense matrix-dense
   // matrix multiplication expression to a dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename DisableIf< UseVectorizedDefaultKernel<MT3,MT4,MT5,ST2> >::Type
      selectDefaultSubAssignKernel( MT3& C, const MT4& A, const MT5& B, ST2 scalar )
   {
      const ResultType tmp( A * B * scalar );
      subAssign( C, tmp );
   }
   //**********************************************************************************************

   //**Vectorized default subtraction assignment to row-major dense matrices***********************
   /*!\brief Vectorized default subtraction assignment of a scaled dense matrix-dense matrix
   //        multiplication (\f$ C-=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function implements the vectorized default subtraction assignment of a scaled dense
   // matrix-dense matrix multiplication expression to a row-major dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseVectorizedDefaultKernel<MT3,MT4,MT5,ST2> >::Type
      selectDefaultSubAssignKernel( DenseMatrix<MT3,false>& C, const MT4& A, const MT5& B, ST2 scalar )
   {
      typedef IntrinsicTrait<ElementType>  IT;

      const size_t M( A.rows()    );
      const size_t N( B.spacing() );
      const size_t K( A.columns() );

      const IntrinsicType factor( set( scalar ) );

      size_t j( 0UL );

      for( ; (j+IT::size*8UL) <= N; j+=IT::size*8UL ) {
         for( size_t i=0UL; i<M; ++i ) {
            IntrinsicType xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8;
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i,k) ) );
               xmm1 = xmm1 + a1 * B.get(k,j             );
               xmm2 = xmm2 + a1 * B.get(k,j+IT::size    );
               xmm3 = xmm3 + a1 * B.get(k,j+IT::size*2UL);
               xmm4 = xmm4 + a1 * B.get(k,j+IT::size*3UL);
               xmm5 = xmm5 + a1 * B.get(k,j+IT::size*4UL);
               xmm6 = xmm6 + a1 * B.get(k,j+IT::size*5UL);
               xmm7 = xmm7 + a1 * B.get(k,j+IT::size*6UL);
               xmm8 = xmm8 + a1 * B.get(k,j+IT::size*7UL);
            }
            store( &(~C)(i,j             ), load( &(~C)(i,j             ) ) - xmm1 * factor );
            store( &(~C)(i,j+IT::size    ), load( &(~C)(i,j+IT::size    ) ) - xmm2 * factor );
            store( &(~C)(i,j+IT::size*2UL), load( &(~C)(i,j+IT::size*2UL) ) - xmm3 * factor );
            store( &(~C)(i,j+IT::size*3UL), load( &(~C)(i,j+IT::size*3UL) ) - xmm4 * factor );
            store( &(~C)(i,j+IT::size*4UL), load( &(~C)(i,j+IT::size*4UL) ) - xmm5 * factor );
            store( &(~C)(i,j+IT::size*5UL), load( &(~C)(i,j+IT::size*5UL) ) - xmm6 * factor );
            store( &(~C)(i,j+IT::size*6UL), load( &(~C)(i,j+IT::size*6UL) ) - xmm7 * factor );
            store( &(~C)(i,j+IT::size*7UL), load( &(~C)(i,j+IT::size*7UL) ) - xmm8 * factor );
         }
      }
      for( ; (j+IT::size*4UL) <= N; j+=IT::size*4UL ) {
         size_t i( 0UL );
         for( ; (i+2UL) <= M; i+=2UL ) {
            IntrinsicType xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8;
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i    ,k) ) );
               const IntrinsicType a2( set( A(i+1UL,k) ) );
               const IntrinsicType b1( B.get(k,j             ) );
               const IntrinsicType b2( B.get(k,j+IT::size    ) );
               const IntrinsicType b3( B.get(k,j+IT::size*2UL) );
               const IntrinsicType b4( B.get(k,j+IT::size*3UL) );
               xmm1 = xmm1 + a1 * b1;
               xmm2 = xmm2 + a1 * b2;
               xmm3 = xmm3 + a1 * b3;
               xmm4 = xmm4 + a1 * b4;
               xmm5 = xmm5 + a2 * b1;
               xmm6 = xmm6 + a2 * b2;
               xmm7 = xmm7 + a2 * b3;
               xmm8 = xmm8 + a2 * b4;
            }
            store( &(~C)(i    ,j             ), load( &(~C)(i    ,j             ) ) - xmm1 * factor );
            store( &(~C)(i    ,j+IT::size    ), load( &(~C)(i    ,j+IT::size    ) ) - xmm2 * factor );
            store( &(~C)(i    ,j+IT::size*2UL), load( &(~C)(i    ,j+IT::size*2UL) ) - xmm3 * factor );
            store( &(~C)(i    ,j+IT::size*3UL), load( &(~C)(i    ,j+IT::size*3UL) ) - xmm4 * factor );
            store( &(~C)(i+1UL,j             ), load( &(~C)(i+1UL,j             ) ) - xmm5 * factor );
            store( &(~C)(i+1UL,j+IT::size    ), load( &(~C)(i+1UL,j+IT::size    ) ) - xmm6 * factor );
            store( &(~C)(i+1UL,j+IT::size*2UL), load( &(~C)(i+1UL,j+IT::size*2UL) ) - xmm7 * factor );
            store( &(~C)(i+1UL,j+IT::size*3UL), load( &(~C)(i+1UL,j+IT::size*3UL) ) - xmm8 * factor );
         }
         if( i < M ) {
            IntrinsicType xmm1, xmm2, xmm3, xmm4;
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i,k) ) );
               xmm1 = xmm1 + a1 * B.get(k,j             );
               xmm2 = xmm2 + a1 * B.get(k,j+IT::size    );
               xmm3 = xmm3 + a1 * B.get(k,j+IT::size*2UL);
               xmm4 = xmm4 + a1 * B.get(k,j+IT::size*3UL);
            }
            store( &(~C)(i,j             ), load( &(~C)(i,j             ) ) - xmm1 * factor );
            store( &(~C)(i,j+IT::size    ), load( &(~C)(i,j+IT::size    ) ) - xmm2 * factor );
            store( &(~C)(i,j+IT::size*2UL), load( &(~C)(i,j+IT::size*2UL) ) - xmm3 * factor );
            store( &(~C)(i,j+IT::size*3UL), load( &(~C)(i,j+IT::size*3UL) ) - xmm4 * factor );
         }
      }
      for( ; (j+IT::size*2UL) <= N; j+=IT::size*2UL ) {
         size_t i( 0UL );
         for( ; (i+2UL) <= M; i+=2UL ) {
            IntrinsicType xmm1, xmm2, xmm3, xmm4;
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i    ,k) ) );
               const IntrinsicType a2( set( A(i+1UL,k) ) );
               const IntrinsicType b1( B.get(k,j         ) );
               const IntrinsicType b2( B.get(k,j+IT::size) );
               xmm1 = xmm1 + a1 * b1;
               xmm2 = xmm2 + a1 * b2;
               xmm3 = xmm3 + a2 * b1;
               xmm4 = xmm4 + a2 * b2;
            }
            store( &(~C)(i    ,j         ), load( &(~C)(i    ,j         ) ) - xmm1 * factor );
            store( &(~C)(i    ,j+IT::size), load( &(~C)(i    ,j+IT::size) ) - xmm2 * factor );
            store( &(~C)(i+1UL,j         ), load( &(~C)(i+1UL,j         ) ) - xmm3 * factor );
            store( &(~C)(i+1UL,j+IT::size), load( &(~C)(i+1UL,j+IT::size) ) - xmm4 * factor );
         }
         if( i < M ) {
            IntrinsicType xmm1, xmm2;
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType a1( set( A(i,k) ) );
               xmm1 = xmm1 + a1 * B.get(k,j         );
               xmm2 = xmm2 + a1 * B.get(k,j+IT::size);
            }
            store( &(~C)(i,j         ), load( &(~C)(i,j         ) ) - xmm1 * factor );
            store( &(~C)(i,j+IT::size), load( &(~C)(i,j+IT::size) ) - xmm2 * factor );
         }
      }
      if( j < N ) {
         size_t i( 0UL );
         for( ; (i+2UL) <= M; i+=2UL ) {
            IntrinsicType xmm1, xmm2;
            for( size_t k=0UL; k<K; ++k ) {
               const IntrinsicType b1( B.get(k,j) );
               xmm1 = xmm1 + set( A(i    ,k) ) * b1;
               xmm2 = xmm2 + set( A(i+1UL,k) ) * b1;
            }
            store( &(~C)(i    ,j), load( &(~C)(i    ,j) ) - xmm1 * factor );
            store( &(~C)(i+1UL,j), load( &(~C)(i+1UL,j) ) - xmm2 * factor );
         }
         if( i < M ) {
            IntrinsicType xmm1;
            for( size_t k=0UL; k<K; ++k ) {
               xmm1 = xmm1 + set( A(i,k) ) * B.get(k,j);
            }
            store( &(~C)(i,j), load( &(~C)(i,j) ) - xmm1 * factor );
         }
      }
   }
   //**********************************************************************************************

   //**Vectorized default subtraction assignment to column-major dense matrices********************
   /*!\brief Vectorized default subtraction assignment of a scaled dense matrix-dense matrix
   //        multiplication (\f$ C-=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function implements the vectorized default subtraction assignment of a scaled dense
   // matrix-dense matrix multiplication expression to a column-major dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseVectorizedDefaultKernel<MT3,MT4,MT5,ST2> >::Type
      selectDefaultSubAssignKernel( DenseMatrix<MT3,true>& C, const MT4& A, const MT5& B, ST2 scalar )
   {
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( typename MT4::OppositeType );
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( typename MT5::OppositeType );

      if( !IsResizable<MT4>::value && IsResizable<MT5>::value ) {
         const typename MT4::OppositeType tmp( A );
         subAssign( ~C, tmp * B * scalar );
      }
      else if( IsResizable<MT4>::value && !IsResizable<MT5>::value ) {
         const typename MT5::OppositeType tmp( B );
         subAssign( ~C, A * tmp * scalar );
      }
      else if( A.rows() * A.columns() <= B.rows() * B.columns() ) {
         const typename MT4::OppositeType tmp( A );
         subAssign( ~C, tmp * B * scalar );
      }
      else {
         const typename MT5::OppositeType tmp( B );
         subAssign( ~C, A * tmp * scalar );
      }
   }
   //**********************************************************************************************

   //**BLAS-based subtraction assignment to dense matrices (default)*******************************
   /*!\brief Default subtraction assignment of a scaled dense matrix-dense matrix multiplication
   //        (\f$ C-=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function relays to the default implementation of the subtraction assignment of a scaled
   // dense matrix-dense matrix multiplication expression to a dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseDefaultKernel<MT3,MT4,MT5,ST2> >::Type
      selectBlasSubAssignKernel( MT3& C, const MT4& A, const MT5& B, ST2 scalar )
   {
      selectDefaultSubAssignKernel( C, A, B, scalar );
   }
   //**********************************************************************************************

   //**BLAS-based subraction assignment to dense matrices (single precision)***********************
#if BLAZE_BLAS_MODE
   /*!\brief BLAS-based subraction assignment of a scaled dense matrix-dense matrix multiplication
   //        for single precision matrices (\f$ C-=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function performs the scaled dense matrix-dense matrix multiplication for single
   // precision matrices based on the BLAS cblas_sgemm() function.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseSinglePrecisionKernel<MT3,MT4,MT5,ST2> >::Type
      selectBlasSubAssignKernel( MT3& C, const MT4& A, const MT5& B, ST2 scalar )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename MT3::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename MT4::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename MT5::ElementType );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( B.columns() ) );
      const int K  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const int ldb( numeric_cast<int>( B.spacing() ) );
      const int ldc( numeric_cast<int>( C.spacing() ) );

      cblas_sgemm( ( IsRowMajorMatrix<MT3>::value )?( CblasRowMajor ):( CblasColMajor ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   M, N, K, -scalar, A.data(), lda, B.data(), ldb, 1.0F, C.data(), ldc );
   }
#endif
   //**********************************************************************************************

   //**BLAS-based subraction assignment to dense matrices (double precision)***********************
#if BLAZE_BLAS_MODE
   /*!\brief BLAS-based subraction assignment of a scaled dense matrix-dense matrix multiplication
   //        for double precision matrices (\f$ C-=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function performs the scaled dense matrix-dense matrix multiplication for double
   // precision matrices based on the BLAS cblas_dgemm() function.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseDoublePrecisionKernel<MT3,MT4,MT5,ST2> >::Type
      selectBlasSubAssignKernel( MT3& C, const MT4& A, const MT5& B, ST2 scalar )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename MT3::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename MT4::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename MT5::ElementType );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( B.columns() ) );
      const int K  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const int ldb( numeric_cast<int>( B.spacing() ) );
      const int ldc( numeric_cast<int>( C.spacing() ) );

      cblas_dgemm( ( IsRowMajorMatrix<MT3>::value )?( CblasRowMajor ):( CblasColMajor ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   M, N, K, -scalar, A.data(), lda, B.data(), ldb, 1.0, C.data(), ldc );
   }
#endif
   //**********************************************************************************************

   //**BLAS-based subraction assignment to dense matrices (single precision complex)***************
#if BLAZE_BLAS_MODE
   /*!\brief BLAS-based subraction assignment of a scaled dense matrix-dense matrix multiplication
   //        for single precision complex matrices (\f$ C-=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function performs the scaled dense matrix-dense matrix multiplication for single
   // precision complex matrices based on the BLAS cblas_cgemm() function.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseSinglePrecisionComplexKernel<MT3,MT4,MT5> >::Type
      selectBlasSubAssignKernel( MT3& C, const MT4& A, const MT5& B, ST2 scalar )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT3::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT4::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT5::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( ST2 );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename MT3::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename MT4::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename MT5::ElementType::value_type );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( B.columns() ) );
      const int K  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const int ldb( numeric_cast<int>( B.spacing() ) );
      const int ldc( numeric_cast<int>( C.spacing() ) );
      const complex<float> alpha( -scalar );
      const complex<float> beta ( 1.0F, 0.0F );

      cblas_cgemm( ( IsRowMajorMatrix<MT3>::value )?( CblasRowMajor ):( CblasColMajor ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   M, N, K, &alpha, A.data(), lda, B.data(), ldb, &beta, C.data(), ldc );
   }
#endif
   //**********************************************************************************************

   //**BLAS-based subraction assignment to dense matrices (double precision complex)***************
#if BLAZE_BLAS_MODE
   /*!\brief BLAS-based subraction assignment of a scaled dense matrix-dense matrix multiplication
   //        for double precision complex matrices (\f$ C-=s*A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function performs the scaled dense matrix-dense matrix multiplication for double
   // precision complex matrices based on the BLAS cblas_zgemm() function.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseDoublePrecisionComplexKernel<MT3,MT4,MT5> >::Type
      selectBlasSubAssignKernel( MT3& C, const MT4& A, const MT5& B, ST2 scalar )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT3::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT4::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT5::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( ST2 );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename MT3::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename MT4::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename MT5::ElementType::value_type );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( B.columns() ) );
      const int K  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const int ldb( numeric_cast<int>( B.spacing() ) );
      const int ldc( numeric_cast<int>( C.spacing() ) );
      const complex<double> alpha( -scalar );
      const complex<double> beta ( 1.0, 0.0 );

      cblas_zgemm( ( IsRowMajorMatrix<MT3>::value )?( CblasRowMajor ):( CblasColMajor ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   ( IsRowMajorMatrix<MT3>::value )?( CblasNoTrans  ):( CblasTrans    ),
                   M, N, K, &alpha, A.data(), lda, B.data(), ldb, &beta, C.data(), ldc );
   }
#endif
   //**********************************************************************************************

   //**Subtraction assignment to sparse matrices***************************************************
   // No special implementation for the subtraction assignment to sparse matrices.
   //**********************************************************************************************

   //**Multiplication assignment to dense matrices*************************************************
   // No special implementation for the multiplication assignment to dense matrices.
   //**********************************************************************************************

   //**Multiplication assignment to sparse matrices************************************************
   // No special implementation for the multiplication assignment to sparse matrices.
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( MMM );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MMM );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( MT1 );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT1 );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( MT2 );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT2 );
   BLAZE_CONSTRAINT_MUST_BE_NUMERIC_TYPE( ST );
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  GLOBAL BINARY ARITHMETIC OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Multiplication operator for the multiplication of two row-major dense matrices
//        (\f$ A=B*C \f$).
// \ingroup dense_matrix
//
// \param lhs The left-hand side matrix for the multiplication.
// \param rhs The right-hand side matrix for the multiplication.
// \return The resulting matrix.
//
// This operator represents the multiplication of two row-major dense matrices:

   \code
   using blaze::rowMajor;

   blaze::DynamicMatrix<double,rowMajor> A, B, C;
   // ... Resizing and initialization
   C = A * B;
   \endcode

// The operator returns an expression representing a dense matrix of the higher-order element
// type of the two involved matrix element types \a T1::ElementType and \a T2::ElementType.
// Both matrix types \a T1 and \a T2 as well as the two element types \a T1::ElementType and
// \a T2::ElementType have to be supported by the MathTrait class template.\n
// In case the current number of columns of \a lhs and the current number of rows of \a rhs
// don't match, a \a std::invalid_argument is thrown.
*/
template< typename T1    // Type of the left-hand side dense matrix
        , typename T2 >  // Type of the right-hand side dense matrix
inline const DMatDMatMultExpr<T1,T2>
   operator*( const DenseMatrix<T1,false>& lhs, const DenseMatrix<T2,false>& rhs )
{
   if( (~lhs).columns() != (~rhs).rows() )
      throw std::invalid_argument( "Matrix sizes do not match" );

   return DMatDMatMultExpr<T1,T2>( ~lhs, ~rhs );
}
//*************************************************************************************************




//=================================================================================================
//
//  EXPRESSION TRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2, typename VT >
struct DMatDVecMultTrait< DMatDMatMultExpr<MT1,MT2>, VT >
{
 public:
   //**********************************************************************************************
   typedef typename DMatDVecMultTrait< MT1, typename DMatDVecMultTrait<MT2,VT>::Type >::Type  Type;
   //**********************************************************************************************

 private:
   //**Compile time checks*************************************************************************
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( MT1 );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT1 );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( MT2 );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT2 );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( VT );
   BLAZE_CONSTRAINT_MUST_BE_NONTRANSPOSE_VECTOR_TYPE( VT );
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2, typename VT >
struct DMatSVecMultTrait< DMatDMatMultExpr<MT1,MT2>, VT >
{
 public:
   //**********************************************************************************************
   typedef typename DMatDVecMultTrait< MT1, typename DMatSVecMultTrait<MT2,VT>::Type >::Type  Type;
   //**********************************************************************************************

 private:
   //**Compile time checks*************************************************************************
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( MT1 );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT1 );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( MT2 );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT2 );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_VECTOR_TYPE( VT );
   BLAZE_CONSTRAINT_MUST_BE_NONTRANSPOSE_VECTOR_TYPE( VT );
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename VT, typename MT1, typename MT2 >
struct TDVecDMatMultTrait< VT, DMatDMatMultExpr<MT1,MT2> >
{
 public:
   //**********************************************************************************************
   typedef typename TDVecDMatMultTrait< typename TDVecDMatMultTrait<VT,MT1>::Type, MT2 >::Type  Type;
   //**********************************************************************************************

 private:
   //**Compile time checks*************************************************************************
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( VT );
   BLAZE_CONSTRAINT_MUST_BE_TRANSPOSE_VECTOR_TYPE( VT );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( MT1 );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT1 );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( MT2 );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT2 );
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename VT, typename MT1, typename MT2 >
struct TSVecDMatMultTrait< VT, DMatDMatMultExpr<MT1,MT2> >
{
 public:
   //**********************************************************************************************
   typedef typename TDVecDMatMultTrait< typename TSVecDMatMultTrait<VT,MT1>::Type, MT2 >::Type  Type;
   //**********************************************************************************************

 private:
   //**Compile time checks*************************************************************************
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_VECTOR_TYPE( VT );
   BLAZE_CONSTRAINT_MUST_BE_TRANSPOSE_VECTOR_TYPE( VT );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( MT1 );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT1 );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( MT2 );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT2 );
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************

} // namespace blaze

#endif
