//=================================================================================================
/*!
//  \file blaze/math/expressions/TDVecTDMatMultExpr.h
//  \brief Header file for the transpose dense vector/transpose dense matrix multiplication expression
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

#ifndef _BLAZE_MATH_EXPRESSIONS_TDVECTDMATMULTEXPR_H_
#define _BLAZE_MATH_EXPRESSIONS_TDVECTDMATMULTEXPR_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <stdexcept>
#include <boost/cast.hpp>
#include <blaze/math/constraints/DenseMatrix.h>
#include <blaze/math/constraints/DenseVector.h>
#include <blaze/math/constraints/StorageOrder.h>
#include <blaze/math/constraints/TransposeFlag.h>
#include <blaze/math/Expression.h>
#include <blaze/math/expressions/DenseVector.h>
#include <blaze/math/expressions/Forward.h>
#include <blaze/math/Intrinsics.h>
#include <blaze/math/MathTrait.h>
#include <blaze/math/shims/Reset.h>
#include <blaze/math/typetraits/CanAlias.h>
#include <blaze/math/typetraits/IsBlasCompatible.h>
#include <blaze/math/typetraits/IsExpression.h>
#include <blaze/math/typetraits/IsMatMatMultExpr.h>
#include <blaze/system/BLAS.h>
#include <blaze/system/Thresholds.h>
#include <blaze/util/Assert.h>
#include <blaze/util/Complex.h>
#include <blaze/util/constraints/Complex.h>
#include <blaze/util/constraints/Double.h>
#include <blaze/util/constraints/Float.h>
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
//  CLASS TDVECTDMATMULTEXPR
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Expression object for transpose dense vector-transpose dense matrix multiplications.
// \ingroup dense_vector_expression
//
// The TDVecTDMatMultExpr class represents the compile time expression for multiplications
// between transpose dense vectors and column-major dense matrices.
*/
template< typename VT    // Type of the left-hand side dense vector
        , typename MT >  // Type of the right-hand side dense matrix
class TDVecTDMatMultExpr : public DenseVector< TDVecTDMatMultExpr<VT,MT>, true >
                         , private Expression
{
 private:
   //**Type definitions****************************************************************************
   typedef typename VT::ResultType     VRT;  //!< Result type of the left-hand side dense vector expression.
   typedef typename MT::ResultType     MRT;  //!< Result type of the right-hand side dense matrix expression.
   typedef typename VRT::ElementType   VET;  //!< Element type of the left-hand side dense vector epxression.
   typedef typename MRT::ElementType   MET;  //!< Element type of the right-hand side dense matrix expression.
   typedef typename VT::CompositeType  VCT;  //!< Composite type of the left-hand side dense vector expression.
   typedef typename MT::CompositeType  MCT;  //!< Composite type of the right-hand side dense matrix expression.
   //**********************************************************************************************

   //**********************************************************************************************
   //! Compilation switch for the composite type of the right-hand side dense matrix expression.
   enum { evaluate = IsExpression<MT>::value && !MT::vectorizable &&
                     IsSame<VET,MET>::value && IsBlasCompatible<VET>::value };
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   /*! In case the data type of the two involved vectors and the matrix is \a float and the
       single precision kernel can be used, the nested \a value will be set to 1, otherwise it
       will be 0. */
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
   /*! In case the data type of the two involved vectors and the matrix is \a double and the
       double precision kernel can be used, the nested \a value will be set to 1, otherwise it
       will be 0. */
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
   /*! In case the data type of the two involved vectors and the matrix is \a complex<float>
       and the single precision complex kernel can be used, the nested \a value will be set
       to 1, otherwise it will be 0. */
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
   /*! In case the data type of the two involved vectors and the matrix is \a complex<double>
       and the double precision complex kernel can be used, the nested \a value will be set
       to 1, otherwise it will be 0. */
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
   /*! In case the two involved vector types and the matrix type are suited for a vectorized
       computation of the vector/matrix multiplication, the nested \value will be set to 1,
       otherwise it will be 0. */
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
   typedef TDVecTDMatMultExpr<VT,MT>                   This;           //!< Type of this TDVecTDMatMultExpr instance.
   typedef typename MathTrait<VRT,MRT>::MultType       ResultType;     //!< Result type for expression template evaluations.
   typedef typename ResultType::TransposeType          TransposeType;  //!< Transpose type for expression template evaluations.
   typedef typename ResultType::ElementType            ElementType;    //!< Resulting element type.
   typedef typename IntrinsicTrait<ElementType>::Type  IntrinsicType;  //!< Resulting intrinsic element type.
   typedef const ResultType                            CompositeType;  //!< Data type for composite expression templates.

   //! Composite type of the left-hand side dense vector expression.
   typedef typename SelectType< IsExpression<VT>::value, const VT, const VT& >::Type  LeftOperand;

   //! Composite type of the right-hand side dense matrix expression.
   typedef typename SelectType< IsExpression<MT>::value, const MT, const MT& >::Type  RightOperand;

   //! Type for the assignment of the left-hand side dense matrix operand.
   typedef typename SelectType< IsExpression<VT>::value, const VRT, VCT >::Type  LT;

   //! Type for the assignment of the right-hand side dense vector operand.
   typedef typename SelectType< evaluate, const MRT, MCT >::Type  RT;
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation switch for the expression template evaluation strategy.
   enum { vectorizable = 0 };

   //! Compilation flag for the detection of aliasing effects.
   enum { canAlias = ( !IsExpression<VT>::value ) ||
                     ( !evaluate && IsReference<MCT>::value && CanAlias<MT>::value ) };
   //**********************************************************************************************

   //**Constructor*********************************************************************************
   /*!\brief Constructor for the TDVecTDMatMultExpr class.
   //
   // \param vec The left-hand side vector operand of the multiplication expression.
   // \param mat The right-hand side matrix operand of the multiplication expression.
   */
   explicit inline TDVecTDMatMultExpr( const VT& vec, const MT& mat )
      : vec_( vec )                                      // Left-hand side dense vector of the multiplication expression
      , mat_( mat )                                      // Right-hand side dense matrix of the multiplication expression
      , end_( ( (mat.rows()-1UL) & size_t(-2) ) + 1UL )  // End of the unrolled calculation loop
   {
      BLAZE_INTERNAL_ASSERT( vec_.size() == mat_.rows(), "Invalid vector and matrix sizes" );
   }
   //**********************************************************************************************

   //**Subscript operator**************************************************************************
   /*!\brief Subscript operator for the direct access to the vector elements.
   //
   // \param index Access index. The index has to be in the range \f$[0..N-1]\f$.
   // \return The accessed value.
   */
   inline const ElementType operator[]( size_t index ) const {
      BLAZE_INTERNAL_ASSERT( index < mat_.columns(), "Invalid vector access index" );

      ElementType res;

      if( mat_.rows() != 0UL ) {
         res = vec_[0UL] * mat_(0UL,index);
         for( size_t j=1UL; j<end_; j+=2UL ) {
            res += vec_[j] * mat_(j,index) + vec_[j+1UL] * mat_(j+1UL,index);
         }
         if( end_ < mat_.rows() ) {
            res += vec_[end_] * mat_(end_,index);
         }
      }
      else {
         reset( res );
      }

      return res;
   }
   //**********************************************************************************************

   //**Size function*******************************************************************************
   /*!\brief Returns the current size/dimension of the vector.
   //
   // \return The size of the vector.
   */
   inline size_t size() const {
      return mat_.columns();
   }
   //**********************************************************************************************

   //**Left operand access*************************************************************************
   /*!\brief Returns the left-hand side dense vector operand.
   //
   // \return The left-hand side dense vector operand.
   */
   inline LeftOperand leftOperand() const {
      return vec_;
   }
   //**********************************************************************************************

   //**Right operand access************************************************************************
   /*!\brief Returns the right-hand side transpose dense matrix operand.
   //
   // \return The right-hand side transpose dense matrix operand.
   */
   inline RightOperand rightOperand() const {
      return mat_;
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the expression is aliased with the given address \a alias.
   //
   // \param alias The alias to be checked.
   // \return \a true in case the given alias is contained in this expression, \a false if not.
   */
   template< typename T >
   inline bool isAliased( const T* alias ) const {
      return ( !IsExpression<VT>::value && vec_.isAliased( alias ) ) ||
             ( !evaluate && IsReference<MCT>::value && mat_.isAliased( alias ) );
   }
   //**********************************************************************************************

 private:
   //**Member variables****************************************************************************
   LeftOperand  vec_;  //!< Left-hand side dense vector of the multiplication expression.
   RightOperand mat_;  //!< Right-hand side dense matrix of the multiplication expression.
   const size_t end_;  //!< End of the unrolled calculation loop.
   //**********************************************************************************************

   //**Assignment to dense vectors*****************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Assignment of a transpose dense vector-transpose dense matrix multiplication to a
   //        transpose dense vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side multiplication expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized assignment of a transpose dense vector-
   // transpose dense matrix multiplication expression to a dense vector.
   */
   template< typename VT1 >  // Type of the target dense vector
   friend inline void assign( DenseVector<VT1,true>& lhs, const TDVecTDMatMultExpr& rhs )
   {
      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      if( rhs.mat_.rows() == 0UL ) {
         reset( ~lhs );
         return;
      }
      else if( rhs.mat_.columns() == 0UL ) {
         return;
      }

      LT x( rhs.vec_ );  // Evaluation of the left-hand side dense vector operand
      RT A( rhs.mat_ );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( x.size()    == rhs.vec_.size()   , "Invalid vector size"       );
      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.mat_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.mat_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.columns() == (~lhs).size()     , "Invalid vector size"       );

      if( ( IsExpression<MT>::value && !evaluate ) ||
          ( A.rows() * A.columns() < TDVECTDMATMULT_THRESHOLD ) )
         TDVecTDMatMultExpr::selectDefaultAssignKernel( ~lhs, x, A );
      else
         TDVecTDMatMultExpr::selectBlasAssignKernel( ~lhs, x, A );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Default assignment to dense vectors*********************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Default assignment of a transpose dense vector-transpose dense matrix multiplication
   //        (\f$ \vec{y}^T=\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side transpose dense vector operand.
   // \param A The right-hand side column-major dense matrix operand.
   // \return void
   //
   // This function implements the default assignment kernel for the transpose dense vector-
   // transpose dense matrix multiplication.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1 >  // Type of the right-hand side matrix operand
   static inline typename DisableIf< UseVectorizedDefaultKernel<VT1,VT2,MT1> >::Type
      selectDefaultAssignKernel( VT1& y, const VT2& x, const MT1& A )
   {
      y.assign( x * A );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Vectorized default assignment to dense vectors**********************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Vectorized default assignment of a transpose dense vector-transpose dense matrix
   //        multiplication (\f$ \vec{y}^T=\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side transpose dense vector operand.
   // \param A The right-hand side column-major dense matrix operand.
   // \return void
   //
   // This function implements the vectorized default assignment kernel for the transpose dense
   // vector-transpose dense matrix multiplication.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseVectorizedDefaultKernel<VT1,VT2,MT1> >::Type
      selectDefaultAssignKernel( VT1& y, const VT2& x, const MT1& A )
   {
      typedef IntrinsicTrait<ElementType>  IT;

      const size_t M( A.rows()    );
      const size_t N( A.columns() );

      size_t j( 0UL );

      for( ; (j+8UL) <= N; j+=8UL ) {
         IntrinsicType xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            const IntrinsicType x1( x.get(i) );
            xmm1 = xmm1 + x1 * A.get(i,j    );
            xmm2 = xmm2 + x1 * A.get(i,j+1UL);
            xmm3 = xmm3 + x1 * A.get(i,j+2UL);
            xmm4 = xmm4 + x1 * A.get(i,j+3UL);
            xmm5 = xmm5 + x1 * A.get(i,j+4UL);
            xmm6 = xmm6 + x1 * A.get(i,j+5UL);
            xmm7 = xmm7 + x1 * A.get(i,j+6UL);
            xmm8 = xmm8 + x1 * A.get(i,j+7UL);
         }
         y[j    ] = sum( xmm1 );
         y[j+1UL] = sum( xmm2 );
         y[j+2UL] = sum( xmm3 );
         y[j+3UL] = sum( xmm4 );
         y[j+4UL] = sum( xmm5 );
         y[j+5UL] = sum( xmm6 );
         y[j+6UL] = sum( xmm7 );
         y[j+7UL] = sum( xmm8 );
      }
      for( ; (j+4UL) <= N; j+=4UL ) {
         IntrinsicType xmm1, xmm2, xmm3, xmm4;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            const IntrinsicType x1( x.get(i) );
            xmm1 = xmm1 + x1 * A.get(i,j    );
            xmm2 = xmm2 + x1 * A.get(i,j+1UL);
            xmm3 = xmm3 + x1 * A.get(i,j+2UL);
            xmm4 = xmm4 + x1 * A.get(i,j+3UL);
         }
         y[j    ] = sum( xmm1 );
         y[j+1UL] = sum( xmm2 );
         y[j+2UL] = sum( xmm3 );
         y[j+3UL] = sum( xmm4 );
      }
      for( ; (j+3UL) <= N; j+=3UL ) {
         IntrinsicType xmm1, xmm2, xmm3;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            const IntrinsicType x1( x.get(i) );
            xmm1 = xmm1 + x1 * A.get(i,j    );
            xmm2 = xmm2 + x1 * A.get(i,j+1UL);
            xmm3 = xmm3 + x1 * A.get(i,j+2UL);
         }
         y[j    ] = sum( xmm1 );
         y[j+1UL] = sum( xmm2 );
         y[j+2UL] = sum( xmm3 );
      }
      for( ; (j+2UL) <= N; j+=2UL ) {
         IntrinsicType xmm1, xmm2;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            const IntrinsicType x1( x.get(i) );
            xmm1 = xmm1 + x1 * A.get(i,j    );
            xmm2 = xmm2 + x1 * A.get(i,j+1UL);
         }
         y[j    ] = sum( xmm1 );
         y[j+1UL] = sum( xmm2 );
      }
      if( j < N ) {
         IntrinsicType xmm1;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            xmm1 = xmm1 + A.get(i,j) * x.get(i);
         }
         y[j] = sum( xmm1 );
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**BLAS-based assignment to dense vectors (default)********************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Default assignment of a transpose dense vector-transpose dense matrix multiplication
   //        (\f$ \vec{y}^T=\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side dense vector operand.
   // \param A The right-hand side dense matrix operand.
   // \return void
   //
   // This function relays to the default implementation of the assignment of a transpose dense
   // vector-transpose dense matrix multiplication expression to a dense vector.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseDefaultKernel<VT1,VT2,MT1> >::Type
      selectBlasAssignKernel( VT1& y, const VT2& x, const MT1& A )
   {
      selectDefaultAssignKernel( y, x, A );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**BLAS-based assignment to dense vectors (single precision)***********************************
#if BLAZE_BLAS_MODE
   /*! \cond BLAZE_INTERNAL */
   /*!\brief BLAS-based assignment of a transpose dense vector-transpose dense matrix multiplication
   //        for single precision operands (\f$ \vec{y}^T=\vec{x}^T*x \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side transpose dense vector operand.
   // \param A The right-hand side column-major dense matrix operand.
   // \return void
   //
   // This function performs the transpose dense vector-transpose dense matrix multiplication for
   // single precision operands based on the BLAS cblas_sgemv() function.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseSinglePrecisionKernel<VT1,VT2,MT1> >::Type
      selectBlasAssignKernel( VT1& y, const VT2& x, const MT1& A )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename VT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename VT2::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename MT1::ElementType );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );

      cblas_sgemv( CblasColMajor, CblasTrans, M, N, 1.0F,
                   A.data(), lda, x.data(), 1, 0.0F, y.data(), 1 );
   }
   /*! \endcond */
#endif
   //**********************************************************************************************

   //**BLAS-based assignment to dense vectors (double precision)***********************************
#if BLAZE_BLAS_MODE
   /*! \cond BLAZE_INTERNAL */
   /*!\brief BLAS-based assignment of a transpose dense vector-transpose dense matrix multiplication
   //        for double precision operands (\f$ \vec{y}^T=\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side transpose dense vector operand.
   // \param A The right-hand side column-major dense matrix operand.
   // \return void
   //
   // This function performs the transpose dense vector-transpose dense matrix multiplication for
   // double precision operands based on the BLAS cblas_dgemv() function.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseDoublePrecisionKernel<VT1,VT2,MT1> >::Type
      selectBlasAssignKernel( VT1& y, const VT2& x, const MT1& A )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename VT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename VT2::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename MT1::ElementType );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );

      cblas_dgemv( CblasColMajor, CblasTrans, M, N, 1.0,
                   A.data(), lda, x.data(), 1, 0.0, y.data(), 1 );
   }
   /*! \endcond */
#endif
   //**********************************************************************************************

   //**BLAS-based assignment to dense vectors (single precision complex)***************************
#if BLAZE_BLAS_MODE
   /*! \cond BLAZE_INTERNAL */
   /*!\brief BLAS-based assignment of a transpose dense vector-transpose dense matrix multiplication
   //        for single precision complex operands (\f$ \vec{y}^T=\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side transpose dense vector operand.
   // \param A The right-hand side column-major dense matrix operand.
   // \return void
   //
   // This function performs the transpose dense vector-transpose dense matrix multiplication for
   // single precision complex operands based on the BLAS cblas_cgemv() function.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseSinglePrecisionComplexKernel<VT1,VT2,MT1> >::Type
      selectBlasAssignKernel( VT1& y, const VT2& x, const MT1& A )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename VT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename VT2::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename VT1::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename VT2::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename MT1::ElementType::value_type );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const complex<float> alpha( 1.0F, 0.0F );
      const complex<float> beta ( 0.0F, 0.0F );

      cblas_cgemv( CblasColMajor, CblasTrans, M, N, &alpha,
                   A.data(), lda, x.data(), 1, &beta, y.data(), 1 );
   }
   /*! \endcond */
#endif
   //**********************************************************************************************

   //**BLAS-based assignment to dense vectors (double precision complex)***************************
#if BLAZE_BLAS_MODE
   /*! \cond BLAZE_INTERNAL */
   /*!\brief BLAS-based assignment of a transpose dense vector-transpose dense matrix multiplication
   //        for double precision complex operands (\f$ \vec{y}^T=\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side transpose dense vector operand.
   // \param A The right-hand side column-major dense matrix operand.
   // \return void
   //
   // This function performs the transpose dense vector-transpose dense matrix multiplication for
   // double precision complex operands based on the BLAS cblas_zgemv() function.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseDoublePrecisionComplexKernel<VT1,VT2,MT1> >::Type
      selectBlasAssignKernel( VT1& y, const VT2& x, const MT1& A )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename VT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename VT2::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename VT1::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename VT2::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename MT1::ElementType::value_type );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const complex<double> alpha( 1.0, 0.0 );
      const complex<double> beta ( 0.0, 0.0 );

      cblas_zgemv( CblasColMajor, CblasTrans, M, N, &alpha,
                   A.data(), lda, x.data(), 1, &beta, y.data(), 1 );
   }
   /*! \endcond */
#endif
   //**********************************************************************************************

   //**Assignment to sparse vectors****************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Assignment of a transpose dense vector-transpose dense matrix multiplication to a
   //        transpose sparse vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side sparse vector.
   // \param rhs The right-hand side multiplication expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized assignment of a transpose dense vector-
   // transpose dense matrix multiplication expression to a sparse vector.
   */
   template< typename VT1 >  // Type of the target sparse vector
   friend inline void assign( SparseVector<VT1,true>& lhs, const TDVecTDMatMultExpr& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_TRANSPOSE_VECTOR_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_REFERENCE_TYPE( typename ResultType::CompositeType );

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const ResultType tmp( rhs );
      assign( ~lhs, tmp );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Addition assignment to dense vectors********************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Addition assignment of a transpose dense vector-transpose dense matrix multiplication
   //        to a transpose dense vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side multiplication expression to be added.
   // \return void
   //
   // This function implements the performance optimized addition assignment of a transpose dense
   // vector-transpose dense matrix multiplication expression to a dense vector.
   */
   template< typename VT1 >  // Type of the target dense vector
   friend inline void addAssign( DenseVector<VT1,true>& lhs, const TDVecTDMatMultExpr& rhs )
   {
      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      if( rhs.mat_.rows() == 0UL || rhs.mat_.columns() == 0UL ) {
         return;
      }

      LT x( rhs.vec_ );  // Evaluation of the left-hand side dense vector operand
      RT A( rhs.mat_ );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( x.size()    == rhs.vec_.size()   , "Invalid vector size"       );
      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.mat_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.mat_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.columns() == (~lhs).size()     , "Invalid vector size"       );

      if( ( IsExpression<MT>::value && !evaluate ) ||
          ( A.rows() * A.columns() < TDVECTDMATMULT_THRESHOLD ) )
         TDVecTDMatMultExpr::selectDefaultAddAssignKernel( ~lhs, x, A );
      else
         TDVecTDMatMultExpr::selectBlasAddAssignKernel( ~lhs, x, A );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Default addition assignment to dense vectors************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Default addition assignment of a transpose dense vector-transpose dense matrix
   //        multiplication (\f$ \vec{y}^T+=\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side transpose dense vector operand.
   // \param A The right-hand side column-major dense matrix operand.
   // \return void
   //
   // This function implements the default addition assignment kernel for the transpose dense
   // vector-transpose dense matrix multiplication.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1 >  // Type of the right-hand side matrix operand
   static inline typename DisableIf< UseVectorizedDefaultKernel<VT1,VT2,MT1> >::Type
      selectDefaultAddAssignKernel( VT1& y, const VT2& x, const MT1& A )
   {
      y.addAssign( x * A );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Vectorized default addition assignment to dense vectors*************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Vectorized default addition assignment of a transpose dense vector-transpose dense
   //        matrix multiplication (\f$ \vec{y}^T+=\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side transpose dense vector operand.
   // \param A The right-hand side column-major dense matrix operand.
   // \return void
   //
   // This function implements the vectorized default addition assignment kernel for the transpose
   // dense vector-transpose dense matrix multiplication.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseVectorizedDefaultKernel<VT1,VT2,MT1> >::Type
      selectDefaultAddAssignKernel( VT1& y, const VT2& x, const MT1& A )
   {
      typedef IntrinsicTrait<ElementType>  IT;

      const size_t M( A.rows()    );
      const size_t N( A.columns() );

      size_t j( 0UL );

      for( ; (j+8UL) <= N; j+=8UL ) {
         IntrinsicType xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            const IntrinsicType x1( x.get(i) );
            xmm1 = xmm1 + x1 * A.get(i,j    );
            xmm2 = xmm2 + x1 * A.get(i,j+1UL);
            xmm3 = xmm3 + x1 * A.get(i,j+2UL);
            xmm4 = xmm4 + x1 * A.get(i,j+3UL);
            xmm5 = xmm5 + x1 * A.get(i,j+4UL);
            xmm6 = xmm6 + x1 * A.get(i,j+5UL);
            xmm7 = xmm7 + x1 * A.get(i,j+6UL);
            xmm8 = xmm8 + x1 * A.get(i,j+7UL);
         }
         y[j    ] += sum( xmm1 );
         y[j+1UL] += sum( xmm2 );
         y[j+2UL] += sum( xmm3 );
         y[j+3UL] += sum( xmm4 );
         y[j+4UL] += sum( xmm5 );
         y[j+5UL] += sum( xmm6 );
         y[j+6UL] += sum( xmm7 );
         y[j+7UL] += sum( xmm8 );
      }
      for( ; (j+4UL) <= N; j+=4UL ) {
         IntrinsicType xmm1, xmm2, xmm3, xmm4;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            const IntrinsicType x1( x.get(i) );
            xmm1 = xmm1 + x1 * A.get(i,j    );
            xmm2 = xmm2 + x1 * A.get(i,j+1UL);
            xmm3 = xmm3 + x1 * A.get(i,j+2UL);
            xmm4 = xmm4 + x1 * A.get(i,j+3UL);
         }
         y[j    ] += sum( xmm1 );
         y[j+1UL] += sum( xmm2 );
         y[j+2UL] += sum( xmm3 );
         y[j+3UL] += sum( xmm4 );
      }
      for( ; (j+3UL) <= N; j+=3UL ) {
         IntrinsicType xmm1, xmm2, xmm3;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            const IntrinsicType x1( x.get(i) );
            xmm1 = xmm1 + x1 * A.get(i,j    );
            xmm2 = xmm2 + x1 * A.get(i,j+1UL);
            xmm3 = xmm3 + x1 * A.get(i,j+2UL);
         }
         y[j    ] += sum( xmm1 );
         y[j+1UL] += sum( xmm2 );
         y[j+2UL] += sum( xmm3 );
      }
      for( ; (j+2UL) <= N; j+=2UL ) {
         IntrinsicType xmm1, xmm2;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            const IntrinsicType x1( x.get(i) );
            xmm1 = xmm1 + x1 * A.get(i,j    );
            xmm2 = xmm2 + x1 * A.get(i,j+1UL);
         }
         y[j    ] += sum( xmm1 );
         y[j+1UL] += sum( xmm2 );
      }
      if( j < N ) {
         IntrinsicType xmm1;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            xmm1 = xmm1 + A.get(i,j) * x.get(i);
         }
         y[j] += sum( xmm1 );
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**BLAS-based addition assignment to dense vectors (default)***********************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Default addition assignment of a transpose dense vector-transpose dense matrix
   //        multiplication (\f$ \vec{y}^T+=\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side dense vector operand.
   // \param A The right-hand side dense matrix operand.
   // \return void
   //
   // This function relays to the default implementation of the addition assignment of a transpose
   // dense vector-transpose dense matrix multiplication expression to a dense vector.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseDefaultKernel<VT1,VT2,MT1> >::Type
      selectBlasAddAssignKernel( VT1& y, const VT2& x, const MT1& A )
   {
      selectDefaultAddAssignKernel( y, x, A );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**BLAS-based addition assignment to dense vectors (single precision)**************************
#if BLAZE_BLAS_MODE
   /*! \cond BLAZE_INTERNAL */
   /*!\brief BLAS-based addition assignment of a vector-matrix multiplication for single
   //        precision operands (\f$ \vec{y}^T+=\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side transpose dense vector operand.
   // \param A The right-hand side column-major dense matrix operand.
   // \return void
   //
   // This function performs the transpose dense vector-transpose dense matrix multiplication for
   // single precision operands based on the BLAS cblas_sgemv() function.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseSinglePrecisionKernel<VT1,VT2,MT1> >::Type
      selectBlasAddAssignKernel( VT1& y, const VT2& x, const MT1& A )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename VT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename VT2::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename MT1::ElementType );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );

      cblas_sgemv( CblasColMajor, CblasTrans, M, N, 1.0F,
                   A.data(), lda, x.data(), 1, 1.0F, y.data(), 1 );
   }
   /*! \endcond */
#endif
   //**********************************************************************************************

   //**BLAS-based addition assignment to dense vectors (double precision)**************************
#if BLAZE_BLAS_MODE
   /*! \cond BLAZE_INTERNAL */
   /*!\brief BLAS-based addition assignment of a vector-matrix multiplication for double
   //        precision operands (\f$ \vec{y}+=A*\vec{x} \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side transpose dense vector operand.
   // \param A The right-hand side column-major dense matrix operand.
   // \return void
   //
   // This function performs the transpose dense vector-transpose dense matrix multiplication for
   // double precision operands based on the BLAS cblas_dgemv() function.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseDoublePrecisionKernel<VT1,VT2,MT1> >::Type
      selectBlasAddAssignKernel( VT1& y, const VT2& x, const MT1& A )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename VT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename VT2::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename MT1::ElementType );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );

      cblas_dgemv( CblasColMajor, CblasTrans, M, N, 1.0,
                   A.data(), lda, x.data(), 1, 1.0, y.data(), 1 );
   }
   /*! \endcond */
#endif
   //**********************************************************************************************

   //**BLAS-based addition assignment to dense vectors (single precision complex)******************
#if BLAZE_BLAS_MODE
   /*! \cond BLAZE_INTERNAL */
   /*!\brief BLAS-based addition assignment of a vector-matrix multiplication for single
   //        precision complex operands (\f$ \vec{y}+=A*\vec{x} \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side transpose dense vector operand.
   // \param A The right-hand side column-major dense matrix operand.
   // \return void
   //
   // This function performs the transpose dense vector-transpose dense matrix multiplication for
   // single precision complex operands based on the BLAS cblas_cgemv() function.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseSinglePrecisionComplexKernel<VT1,VT2,MT1> >::Type
      selectBlasAddAssignKernel( VT1& y, const VT2& x, const MT1& A )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename VT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename VT2::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename VT1::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename VT2::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename MT1::ElementType::value_type );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const complex<float> alpha( 1.0F, 0.0F );
      const complex<float> beta ( 1.0F, 0.0F );

      cblas_cgemv( CblasColMajor, CblasTrans, M, N, &alpha,
                   A.data(), lda, x.data(), 1, &beta, y.data(), 1 );
   }
   /*! \endcond */
#endif
   //**********************************************************************************************

   //**BLAS-based addition assignment to dense vectors (double precision complex)******************
#if BLAZE_BLAS_MODE
   /*! \cond BLAZE_INTERNAL */
   /*!\brief BLAS-based addition assignment of a vector-matrix multiplication for double
   //        precision complex operands (\f$ \vec{y}+=A*\vec{x} \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side transpose dense vector operand.
   // \param A The right-hand side column-major dense matrix operand.
   // \return void
   //
   // This function performs the transpose dense vector-transpose dense matrix multiplication for
   // double precision complex operands based on the BLAS cblas_zgemv() function.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseDoublePrecisionComplexKernel<VT1,VT2,MT1> >::Type
      selectBlasAddAssignKernel( VT1& y, const VT2& x, const MT1& A )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename VT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename VT2::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename VT1::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename VT2::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename MT1::ElementType::value_type );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const complex<double> alpha( 1.0, 0.0 );
      const complex<double> beta ( 1.0, 0.0 );

      cblas_zgemv( CblasColMajor, CblasTrans, M, N, &alpha,
                   A.data(), lda, x.data(), 1, &beta, y.data(), 1 );
   }
   /*! \endcond */
#endif
   //**********************************************************************************************

   //**Addition assignment to sparse vectors*******************************************************
   // No special implementation for the addition assignment to sparse vectors.
   //**********************************************************************************************

   //**Subtraction assignment to dense vectors*****************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Subtraction assignment of a transpose dense vector-transpose dense matrix
   //        multiplication to a transpose dense vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side multiplication expression to be subtracted.
   // \return void
   //
   // This function implements the performance optimized subtraction assignment of a transpose
   // dense vector-transpose dense matrix multiplication expression to a dense vector.
   */
   template< typename VT1 >  // Type of the target dense vector
   friend inline void subAssign( DenseVector<VT1,true>& lhs, const TDVecTDMatMultExpr& rhs )
   {
      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      if( rhs.mat_.rows() == 0UL || rhs.mat_.columns() == 0UL ) {
         return;
      }

      LT x( rhs.vec_ );  // Evaluation of the left-hand side dense vector operand
      RT A( rhs.mat_ );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( x.size()    == rhs.vec_.size()   , "Invalid vector size"       );
      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.mat_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.mat_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.columns() == (~lhs).size()     , "Invalid vector size"       );

      if( ( IsExpression<MT>::value && !evaluate ) ||
          ( A.rows() * A.columns() < TDVECTDMATMULT_THRESHOLD ) )
         TDVecTDMatMultExpr::selectDefaultSubAssignKernel( ~lhs, x, A );
      else
         TDVecTDMatMultExpr::selectBlasSubAssignKernel( ~lhs, x, A );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Default subtraction assignment to dense vectors*********************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Default subtraction assignment of a transpose dense vector-transpose dense matrix
   //        multiplication (\f$ \vec{y}^T-=\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side transpose dense vector operand.
   // \param A The right-hand side column-major dense matrix operand.
   // \return void
   //
   // This function implements the default subtraction assignment kernel for the transpose dense
   // vector-transpose dense matrix multiplication.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1 >  // Type of the right-hand side matrix operand
   static inline typename DisableIf< UseVectorizedDefaultKernel<VT1,VT2,MT1> >::Type
      selectDefaultSubAssignKernel( VT1& y, const VT2& x, const MT1& A )
   {
      y.subAssign( x * A );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Vectorized default subtraction assignment to dense vectors**********************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Vectorized default subtraction assignment of a transpose dense vector-transpose
   //        dense matrix multiplication (\f$ \vec{y}^T-=\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side transpose dense vector operand.
   // \param A The right-hand side column-major dense matrix operand.
   // \return void
   //
   // This function implements the vectorized default subtraction assignment kernel for the
   // transpose dense vector-transpose dense matrix multiplication.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseVectorizedDefaultKernel<VT1,VT2,MT1> >::Type
      selectDefaultSubAssignKernel( VT1& y, const VT2& x, const MT1& A )
   {
      typedef IntrinsicTrait<ElementType>  IT;

      const size_t M( A.rows()    );
      const size_t N( A.columns() );

      size_t j( 0UL );

      for( ; (j+8UL) <= N; j+=8UL ) {
         IntrinsicType xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            const IntrinsicType x1( x.get(i) );
            xmm1 = xmm1 + x1 * A.get(i,j    );
            xmm2 = xmm2 + x1 * A.get(i,j+1UL);
            xmm3 = xmm3 + x1 * A.get(i,j+2UL);
            xmm4 = xmm4 + x1 * A.get(i,j+3UL);
            xmm5 = xmm5 + x1 * A.get(i,j+4UL);
            xmm6 = xmm6 + x1 * A.get(i,j+5UL);
            xmm7 = xmm7 + x1 * A.get(i,j+6UL);
            xmm8 = xmm8 + x1 * A.get(i,j+7UL);
         }
         y[j    ] -= sum( xmm1 );
         y[j+1UL] -= sum( xmm2 );
         y[j+2UL] -= sum( xmm3 );
         y[j+3UL] -= sum( xmm4 );
         y[j+4UL] -= sum( xmm5 );
         y[j+5UL] -= sum( xmm6 );
         y[j+6UL] -= sum( xmm7 );
         y[j+7UL] -= sum( xmm8 );
      }
      for( ; (j+4UL) <= N; j+=4UL ) {
         IntrinsicType xmm1, xmm2, xmm3, xmm4;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            const IntrinsicType x1( x.get(i) );
            xmm1 = xmm1 + x1 * A.get(i,j    );
            xmm2 = xmm2 + x1 * A.get(i,j+1UL);
            xmm3 = xmm3 + x1 * A.get(i,j+2UL);
            xmm4 = xmm4 + x1 * A.get(i,j+3UL);
         }
         y[j    ] -= sum( xmm1 );
         y[j+1UL] -= sum( xmm2 );
         y[j+2UL] -= sum( xmm3 );
         y[j+3UL] -= sum( xmm4 );
      }
      for( ; (j+3UL) <= N; j+=3UL ) {
         IntrinsicType xmm1, xmm2, xmm3;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            const IntrinsicType x1( x.get(i) );
            xmm1 = xmm1 + x1 * A.get(i,j    );
            xmm2 = xmm2 + x1 * A.get(i,j+1UL);
            xmm3 = xmm3 + x1 * A.get(i,j+2UL);
         }
         y[j    ] -= sum( xmm1 );
         y[j+1UL] -= sum( xmm2 );
         y[j+2UL] -= sum( xmm3 );
      }
      for( ; (j+2UL) <= N; j+=2UL ) {
         IntrinsicType xmm1, xmm2;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            const IntrinsicType x1( x.get(i) );
            xmm1 = xmm1 + x1 * A.get(i,j    );
            xmm2 = xmm2 + x1 * A.get(i,j+1UL);
         }
         y[j    ] -= sum( xmm1 );
         y[j+1UL] -= sum( xmm2 );
      }
      if( j < N ) {
         IntrinsicType xmm1;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            xmm1 = xmm1 + A.get(i,j) * x.get(i);
         }
         y[j] -= sum( xmm1 );
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**BLAS-based subtraction assignment to dense vectors (default)********************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Default subtraction assignment of a transpose dense vector-transpose dense matrix
   //        multiplication (\f$ \vec{y}^T-=\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side dense vector operand.
   // \param A The right-hand side dense matrix operand.
   // \return void
   //
   // This function relays to the default implementation of the subtraction assignment of a
   // transpose dense vector-transpose dense matrix multiplication expression to a dense vector.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseDefaultKernel<VT1,VT2,MT1> >::Type
      selectBlasSubAssignKernel( VT1& y, const VT2& x, const MT1& A )
   {
      selectDefaultSubAssignKernel( y, x, A );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**BLAS-based subtraction assignment to dense vectors (single precision)***********************
#if BLAZE_BLAS_MODE
   /*! \cond BLAZE_INTERNAL */
   /*!\brief BLAS-based subtraction assignment of a vector-matrix multiplication for single
   //        precision operands (\f$ \vec{y}^T-=\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side transpose dense vector operand.
   // \param A The right-hand side column-major dense matrix operand.
   // \return void
   //
   // This function performs the transpose dense vector-transpose dense matrix multiplication for
   // single precision operands based on the BLAS cblas_sgemv() function.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseSinglePrecisionKernel<VT1,VT2,MT1> >::Type
      selectBlasSubAssignKernel( VT1& y, const VT2& x, const MT1& A )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename VT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename VT2::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename MT1::ElementType );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );

      cblas_sgemv( CblasColMajor, CblasTrans, M, N, -1.0F,
                   A.data(), lda, x.data(), 1, 1.0F, y.data(), 1 );
   }
   /*! \endcond */
#endif
   //**********************************************************************************************

   //**BLAS-based subtraction assignment to dense vectors (double precision)***********************
#if BLAZE_BLAS_MODE
   /*! \cond BLAZE_INTERNAL */
   /*!\brief BLAS-based subtraction assignment of a vector-matrix multiplication for double
   //        precision operands (\f$ \vec{y}-=A*\vec{x} \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side transpose dense vector operand.
   // \param A The right-hand side column-major dense matrix operand.
   // \return void
   //
   // This function performs the transpose dense vector-transpose dense matrix multiplication for
   // double precision operands based on the BLAS cblas_dgemv() function.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseDoublePrecisionKernel<VT1,VT2,MT1> >::Type
      selectBlasSubAssignKernel( VT1& y, const VT2& x, const MT1& A )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename VT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename VT2::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename MT1::ElementType );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );

      cblas_dgemv( CblasColMajor, CblasTrans, M, N, -1.0,
                   A.data(), lda, x.data(), 1, 1.0, y.data(), 1 );
   }
   /*! \endcond */
#endif
   //**********************************************************************************************

   //**BLAS-based subtraction assignment to dense vectors (single precision complex)***************
#if BLAZE_BLAS_MODE
   /*! \cond BLAZE_INTERNAL */
   /*!\brief BLAS-based subtraction assignment of a vector-matrix multiplication for single
   //        precision complex operands (\f$ \vec{y}-=A*\vec{x} \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side transpose dense vector operand.
   // \param A The right-hand side column-major dense matrix operand.
   // \return void
   //
   // This function performs the transpose dense vector-transpose dense matrix multiplication for
   // single precision complex operands based on the BLAS cblas_cgemv() function.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseSinglePrecisionComplexKernel<VT1,VT2,MT1> >::Type
      selectBlasSubAssignKernel( VT1& y, const VT2& x, const MT1& A )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename VT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename VT2::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename VT1::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename VT2::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename MT1::ElementType::value_type );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const complex<float> alpha( -1.0F, 0.0F );
      const complex<float> beta (  1.0F, 0.0F );

      cblas_cgemv( CblasColMajor, CblasTrans, M, N, &alpha,
                   A.data(), lda, x.data(), 1, &beta, y.data(), 1 );
   }
   /*! \endcond */
#endif
   //**********************************************************************************************

   //**BLAS-based subtraction assignment to dense vectors (double precision complex)***************
#if BLAZE_BLAS_MODE
   /*! \cond BLAZE_INTERNAL */
   /*!\brief BLAS-based subtraction assignment of a vector-matrix multiplication for double
   //        precision complex operands (\f$ \vec{y}-=A*\vec{x} \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side transpose dense vector operand.
   // \param A The right-hand side column-major dense matrix operand.
   // \return void
   //
   // This function performs the transpose dense vector-transpose dense matrix multiplication for
   // double precision complex operands based on the BLAS cblas_zgemv() function.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseDoublePrecisionComplexKernel<VT1,VT2,MT1> >::Type
      selectBlasSubAssignKernel( VT1& y, const VT2& x, const MT1& A )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename VT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename VT2::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename VT1::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename VT2::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename MT1::ElementType::value_type );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const complex<double> alpha( -1.0, 0.0 );
      const complex<double> beta (  1.0, 0.0 );

      cblas_zgemv( CblasColMajor, CblasTrans, M, N, &alpha,
                   A.data(), lda, x.data(), 1, &beta, y.data(), 1 );
   }
   /*! \endcond */
#endif
   //**********************************************************************************************

   //**Subtraction assignment to sparse vectors****************************************************
   // No special implementation for the subtraction assignment to sparse vectors.
   //**********************************************************************************************

   //**Multiplication assignment to dense vectors**************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Multiplication assignment of a transpose dense vector-transpose dense matrix
   //        multiplication to a transpose dense vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side multiplication expression to be multiplied.
   // \return void
   //
   // This function implements the performance optimized subtraction assignment of a transpose
   // dense vector-transpose dense matrix multiplication expression to a dense vector.
   */
   template< typename VT1 >  // Type of the target dense vector
   friend inline void multAssign( DenseVector<VT1,true>& lhs, const TDVecTDMatMultExpr& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_TRANSPOSE_VECTOR_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_REFERENCE_TYPE( typename ResultType::CompositeType );

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const ResultType tmp( rhs );
      multAssign( ~lhs, tmp );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Multiplication assignment to sparse vectors*******************************************************
   // No special implementation for the multiplication assignment to sparse vectors.
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( VT );
   BLAZE_CONSTRAINT_MUST_BE_TRANSPOSE_VECTOR_TYPE( VT );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( MT );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( MT );
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************




//=================================================================================================
//
//  DVECSCALARMULTEXPR SPECIALIZATION
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Expression object for scaled transpose dense vector-transpose dense matrix multiplications.
// \ingroup dense_vector_expression
//
// This specialization of the DVecScalarMultExpr class represents the compile time expression
// for scaled multiplications between a non-transpose dense vector and a column-major dense matrix.
*/
template< typename VT    // Type of the left-hand side dense vector
        , typename MT    // Type of the right-hand side dense matrix
        , typename ST >  // Type of the side scalar value
class DVecScalarMultExpr< TDVecTDMatMultExpr<VT,MT>, ST, true >
   : public DenseVector< DVecScalarMultExpr< TDVecTDMatMultExpr<VT,MT>, ST, true >, true >
   , private Expression
{
 private:
   //**Type definitions****************************************************************************
   typedef TDVecTDMatMultExpr<VT,MT>   VMM;  //!< Type of the dense vector-dense matrix multiplication expression.
   typedef typename VMM::ResultType    RES;  //!< Result type of the dense vector-dense matrix multiplication expression.
   typedef typename VT::ResultType     VRT;  //!< Result type of the left-hand side dense vector expression.
   typedef typename MT::ResultType     MRT;  //!< Result type of the right-hand side dense matrix expression.
   typedef typename VRT::ElementType   VET;  //!< Element type of the left-hand side dense vector epxression.
   typedef typename MRT::ElementType   MET;  //!< Element type of the right-hand side dense matrix expression.
   typedef typename VT::CompositeType  VCT;  //!< Composite type of the left-hand side dense vector expression.
   typedef typename MT::CompositeType  MCT;  //!< Composite type of the right-hand side dense matrix expression.
   //**********************************************************************************************

   //**********************************************************************************************
   //! Compilation switch for the composite type of the right-hand side dense matrix expression.
   enum { evaluate = IsExpression<MT>::value && !MT::vectorizable &&
                     IsSame<VET,MET>::value && IsBlasCompatible<VET>::value };
   //**********************************************************************************************

   //**********************************************************************************************
   //! Helper structure for the explicit application of the SFINAE principle.
   /*! In case the data type of the two involved vectors and the matrix is \a float, the scalar
       value is not a complex data type, and the single precision kernel can be used, the nested
       \a value will be set to 1, otherwise it will be 0. */
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
   /*! In case the data type of the two involved vectors and the matrix is \a double, the scalar
       value is not a complex data type and the double precision kernel can be used, the nested
       \a value will be set to 1, otherwise it will be 0. */
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
   /*! In case the data type of the two involved vectors and the matrix is \a complex<float>
       and the single precision complex kernel can be used, the nested \a value will be set to
       1, otherwise it will be 0. */
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
   /*! In case the data type of the two involved vectors and the matrix is \a complex<double>
       and the double precision complex kernel can be used, the nested \a value will be set to
       1, otherwise it will be 0. */
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
   /*! In case the two involved vector types, the matrix type, and the scalar type are suited
       for a vectorized computation of the scaled vector/matrix multiplication, the nested
       \value will be set to 1, otherwise it will be 0. */
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
   typedef DVecScalarMultExpr<VMM,ST,true>             This;           //!< Type of this DVecScalarMultExpr instance.
   typedef typename MathTrait<RES,ST>::MultType        ResultType;     //!< Result type for expression template evaluations.
   typedef typename ResultType::TransposeType          TransposeType;  //!< Transpose type for expression template evaluations.
   typedef typename ResultType::ElementType            ElementType;    //!< Resulting element type.
   typedef typename IntrinsicTrait<ElementType>::Type  IntrinsicType;  //!< Resulting intrinsic element type.
   typedef const ResultType                            CompositeType;  //!< Data type for composite expression templates.

   //! Composite type of the left-hand side dense matrix expression.
   typedef const TDVecTDMatMultExpr<VT,MT>  LeftOperand;

   //! Composite type of the right-hand side dense vector expression.
   typedef typename SelectType< IsNumeric<ElementType>::value, ElementType, ST >::Type  RightOperand;

   //! Type for the assignment of the left-hand side dense matrix operand.
   typedef typename SelectType< IsExpression<VT>::value, const VRT, VCT >::Type  LT;

   //! Type for the assignment of the right-hand side dense vector operand.
   typedef typename SelectType< evaluate, const MRT, MCT >::Type  RT;
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation switch for the expression template evaluation strategy.
   enum { vectorizable = 0 };

   //! Compilation flag for the detection of aliasing effects.
   enum { canAlias = CanAlias<VMM>::value };
   //**********************************************************************************************

   //**Constructor*********************************************************************************
   /*!\brief Constructor for the DVecScalarMultExpr class.
   //
   // \param vector The left-hand side dense vector of the multiplication expression.
   // \param scalar The right-hand side scalar of the multiplication expression.
   */
   explicit inline DVecScalarMultExpr( const VMM& vector, ST scalar )
      : vector_( vector )  // Left-hand side dense vector of the multiplication expression
      , scalar_( scalar )  // Right-hand side scalar of the multiplication expression
   {}
   //**********************************************************************************************

   //**Subscript operator**************************************************************************
   /*!\brief Subscript operator for the direct access to the vector elements.
   //
   // \param index Access index. The index has to be in the range \f$[0..N-1]\f$.
   // \return The accessed value.
   */
   inline const ElementType operator[]( size_t index ) const {
      BLAZE_INTERNAL_ASSERT( index < vector_.size(), "Invalid vector access index" );
      return vector_[index] * scalar_;
   }
   //**********************************************************************************************

   //**Size function*******************************************************************************
   /*!\brief Returns the current size/dimension of the vector.
   //
   // \return The size of the vector.
   */
   inline size_t size() const {
      return vector_.size();
   }
   //**********************************************************************************************

   //**Left operand access*************************************************************************
   /*!\brief Returns the left-hand side dense vector operand.
   //
   // \return The left-hand side dense vector operand.
   */
   inline LeftOperand leftOperand() const {
      return vector_;
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
      return vector_.isAliased( alias );
   }
   //**********************************************************************************************

 private:
   //**Member variables****************************************************************************
   LeftOperand  vector_;  //!< Left-hand side dense vector of the multiplication expression.
   RightOperand scalar_;  //!< Right-hand side scalar of the multiplication expression.
   //**********************************************************************************************

   //**Assignment to dense vectors*****************************************************************
   /*!\brief Assignment of a scaled transpose dense vector-transpose dense matrix multiplication
   //        to a transpose dense vector (\f$ \vec{y}^T=s*\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side scaled multiplication expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized assignment of a scaled transpose dense
   // vector-transpose dense matrix multiplication expression to a dense vector.
   */
   template< typename VT1  // Type of the target dense vector
           , bool TF >     // Transpose flag of the target dense vector
   friend inline void assign( DenseVector<VT1,TF>& lhs, const DVecScalarMultExpr& rhs )
   {
      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      typename VMM::LeftOperand  left ( rhs.vector_.leftOperand()  );
      typename VMM::RightOperand right( rhs.vector_.rightOperand() );

      if( right.rows() == 0UL ) {
         reset( ~lhs );
         return;
      }
      else if( right.columns() == 0UL ) {
         return;
      }

      LT x( left  );  // Evaluation of the left-hand side dense vector operand
      RT A( right );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( x.size()    == left.size()    , "Invalid vector size"       );
      BLAZE_INTERNAL_ASSERT( A.rows()    == right.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == right.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.columns() == (~lhs).size()  , "Invalid vector size"       );

      if( ( IsExpression<MT>::value && !evaluate ) ||
          ( A.rows() * A.columns() < TDVECTDMATMULT_THRESHOLD ) )
         DVecScalarMultExpr::selectDefaultAssignKernel( ~lhs, x, A, rhs.scalar_ );
      else
         DVecScalarMultExpr::selectBlasAssignKernel( ~lhs, x, A, rhs.scalar_ );
   }
   //**********************************************************************************************

   //**Default assignment to dense vectors*********************************************************
   /*!\brief Default assignment of a scaled transpose dense vector-transpose dense matrix
   //        multiplication (\f$ \vec{y}^T=s*\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side dense vector operand.
   // \param A The right-hand side dense matrix operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function implements the default assignment kernel for the scaled transpose dense vector-
   // transpose dense matrix multiplication.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename DisableIf< UseVectorizedDefaultKernel<VT1,VT2,MT1,ST2> >::Type
      selectDefaultAssignKernel( VT1& y, const VT2& x, const MT1& A, ST2 scalar )
   {
      y.assign( x * A * scalar );
   }
   //**********************************************************************************************

   //**Vectorized default assignment to dense vectors**********************************************
   /*!\brief Vectorized default assignment of a scaled transpose dense vector-transpose dense
   //        matrix multiplication (\f$ \vec{y}^T=s*\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side dense vector operand.
   // \param A The right-hand side dense matrix operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function implements the vectorized default assignment kernel for the scaled transpose
   // dense vector-transpose dense matrix multiplication.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseVectorizedDefaultKernel<VT1,VT2,MT1,ST2> >::Type
      selectDefaultAssignKernel( VT1& y, const VT2& x, const MT1& A, ST2 scalar )
   {
      typedef IntrinsicTrait<ElementType>  IT;

      const size_t M( A.rows()    );
      const size_t N( A.columns() );

      size_t j( 0UL );

      for( ; (j+8UL) <= N; j+=8UL ) {
         IntrinsicType xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            const IntrinsicType x1( x.get(i) );
            xmm1 = xmm1 + x1 * A.get(i,j    );
            xmm2 = xmm2 + x1 * A.get(i,j+1UL);
            xmm3 = xmm3 + x1 * A.get(i,j+2UL);
            xmm4 = xmm4 + x1 * A.get(i,j+3UL);
            xmm5 = xmm5 + x1 * A.get(i,j+4UL);
            xmm6 = xmm6 + x1 * A.get(i,j+5UL);
            xmm7 = xmm7 + x1 * A.get(i,j+6UL);
            xmm8 = xmm8 + x1 * A.get(i,j+7UL);
         }
         y[j    ] = sum( xmm1 ) * scalar;
         y[j+1UL] = sum( xmm2 ) * scalar;
         y[j+2UL] = sum( xmm3 ) * scalar;
         y[j+3UL] = sum( xmm4 ) * scalar;
         y[j+4UL] = sum( xmm5 ) * scalar;
         y[j+5UL] = sum( xmm6 ) * scalar;
         y[j+6UL] = sum( xmm7 ) * scalar;
         y[j+7UL] = sum( xmm8 ) * scalar;
      }
      for( ; (j+4UL) <= N; j+=4UL ) {
         IntrinsicType xmm1, xmm2, xmm3, xmm4;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            const IntrinsicType x1( x.get(i) );
            xmm1 = xmm1 + x1 * A.get(i,j    );
            xmm2 = xmm2 + x1 * A.get(i,j+1UL);
            xmm3 = xmm3 + x1 * A.get(i,j+2UL);
            xmm4 = xmm4 + x1 * A.get(i,j+3UL);
         }
         y[j    ] = sum( xmm1 ) * scalar;
         y[j+1UL] = sum( xmm2 ) * scalar;
         y[j+2UL] = sum( xmm3 ) * scalar;
         y[j+3UL] = sum( xmm4 ) * scalar;
      }
      for( ; (j+3UL) <= N; j+=3UL ) {
         IntrinsicType xmm1, xmm2, xmm3;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            const IntrinsicType x1( x.get(i) );
            xmm1 = xmm1 + x1 * A.get(i,j    );
            xmm2 = xmm2 + x1 * A.get(i,j+1UL);
            xmm3 = xmm3 + x1 * A.get(i,j+2UL);
         }
         y[j    ] = sum( xmm1 ) * scalar;
         y[j+1UL] = sum( xmm2 ) * scalar;
         y[j+2UL] = sum( xmm3 ) * scalar;
      }
      for( ; (j+2UL) <= N; j+=2UL ) {
         IntrinsicType xmm1, xmm2;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            const IntrinsicType x1( x.get(i) );
            xmm1 = xmm1 + x1 * A.get(i,j    );
            xmm2 = xmm2 + x1 * A.get(i,j+1UL);
         }
         y[j    ] = sum( xmm1 ) * scalar;
         y[j+1UL] = sum( xmm2 ) * scalar;
      }
      if( j < N ) {
         IntrinsicType xmm1;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            xmm1 = xmm1 + A.get(i,j) * x.get(i);
         }
         y[j] = sum( xmm1 ) * scalar;
      }
   }
   //**********************************************************************************************

   //**BLAS-based assignment to dense vectors (default)********************************************
   /*!\brief Default assignment of a scaled transpose dense vector-transpose dense matrix
   //        multiplication (\f$ \vec{y}^T=\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side dense vector operand.
   // \param A The right-hand side dense matrix operand.
   // \return void
   //
   // This function relays to the default implementation of the assignment of a scaled transpose
   // dense vector-transpose dense matrix multiplication expression to a dense vector.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseDefaultKernel<VT1,VT2,MT1,ST2> >::Type
      selectBlasAssignKernel( VT1& y, const VT2& x, const MT1& A, ST2 scalar )
   {
      selectDefaultAssignKernel( y, x, A, scalar );
   }
   //**********************************************************************************************

   //**BLAS-based assignment to dense vectors (single precision)***********************************
#if BLAZE_BLAS_MODE
   /*!\brief BLAS-based assignment of a scaled transpose dense vector-transpose dense matrix
   //        multiplication for single precision operands (\f$ \vec{y}^T=s*\vec{x}^T*x \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side dense vector operand.
   // \param A The right-hand side dense matrix operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function performs the scaled transpose dense vector-transpose dense matrix
   // multiplication for single precision operands based on the BLAS cblas_sgemv() function.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseSinglePrecisionKernel<VT1,VT2,MT1,ST2> >::Type
      selectBlasAssignKernel( VT1& y, const VT2& x, const MT1& A, ST2 scalar )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename VT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename VT2::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename MT1::ElementType );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );

      cblas_sgemv( CblasColMajor, CblasTrans, M, N, scalar,
                   A.data(), lda, x.data(), 1, 0.0F, y.data(), 1 );
   }
#endif
   //**********************************************************************************************

   //**BLAS-based assignment to dense vectors (double precision)***********************************
#if BLAZE_BLAS_MODE
   /*!\brief BLAS-based assignment of a scaled transpose dense vector-transpose dense matrix
   //        multiplication for double precision operands (\f$ \vec{y}^T=s*\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side dense vector operand.
   // \param A The right-hand side dense matrix operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function performs the scaled transpose dense vector-transpose dense matrix
   // multiplication for double precision operands based on the BLAS cblas_dgemv() function.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseDoublePrecisionKernel<VT1,VT2,MT1,ST2> >::Type
      selectBlasAssignKernel( VT1& y, const VT2& x, const MT1& A, ST2 scalar )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename VT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename VT2::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename MT1::ElementType );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );

      cblas_dgemv( CblasColMajor, CblasTrans, M, N, scalar,
                   A.data(), lda, x.data(), 1, 0.0, y.data(), 1 );
   }
#endif
   //**********************************************************************************************

   //**BLAS-based assignment to dense vectors (single precision complex)***************************
#if BLAZE_BLAS_MODE
   /*!\brief BLAS-based assignment of a scaled transpose dense vector-transpose dense matrix
   //        multiplication for single precision complex operands (\f$ \vec{y}^T=s*\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side dense vector operand.
   // \param A The right-hand side dense matrix operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function performs the scaled transpose dense vector-transpose dense matrix
   // multiplication for single precision complex operands based on the BLAS cblas_cgemv()
   // function.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseSinglePrecisionComplexKernel<VT1,VT2,MT1> >::Type
      selectBlasAssignKernel( VT1& y, const VT2& x, const MT1& A, ST2 scalar )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename VT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename VT2::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( ST2 );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename VT1::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename VT2::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename MT1::ElementType::value_type );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const complex<float> alpha( scalar );
      const complex<float> beta ( 0.0F, 0.0F );

      cblas_cgemv( CblasColMajor, CblasTrans, M, N, &alpha,
                   A.data(), lda, x.data(), 1, &beta, y.data(), 1 );
   }
#endif
   //**********************************************************************************************

   //**BLAS-based assignment to dense vectors (double precision complex)***************************
#if BLAZE_BLAS_MODE
   /*!\brief BLAS-based assignment of a scaled transpose dense vector-transpose dense matrix
   //        multiplication for double precision complex operands (\f$ \vec{y}^T=s*\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side dense vector operand.
   // \param A The right-hand side dense matrix operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function performs the scaled transpose dense vector-transpose dense matrix
   // multiplication for double precision complex operands based on the BLAS cblas_zgemv()
   // function.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseDoublePrecisionComplexKernel<VT1,VT2,MT1> >::Type
      selectBlasAssignKernel( VT1& y, const VT2& x, const MT1& A, ST2 scalar )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename VT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename VT2::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( ST2 );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename VT1::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename VT2::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename MT1::ElementType::value_type );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const complex<double> alpha( scalar );
      const complex<double> beta ( 0.0, 0.0 );

      cblas_zgemv( CblasColMajor, CblasTrans, M, N, &alpha,
                      A.data(), lda, x.data(), 1, &beta, y.data(), 1 );
   }
#endif
   //**********************************************************************************************

   //**Assignment to sparse vectors****************************************************************
   /*!\brief Assignment of a scaled transpose dense vector-transpose dense matrix multiplication
   //        to a transpose sparse vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side sparse vector.
   // \param rhs The right-hand side multiplication expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized assignment of a scaled transpose dense
   // vector-transpose dense matrix multiplication expression to a sparse vector.
   */
   template< typename VT1  // Type of the target sparse vector
           , bool TF >     // Transpose flag of the target sparse vector
   friend inline void assign( SparseVector<VT1,TF>& lhs, const DVecScalarMultExpr& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_TRANSPOSE_VECTOR_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_REFERENCE_TYPE( typename ResultType::CompositeType );

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const ResultType tmp( rhs );
      assign( ~lhs, tmp );
   }
   //**********************************************************************************************

   //**Addition assignment to dense vectors********************************************************
   /*!\brief Addition assignment of a scaled transpose dense vector-transpose dense matrix
   //        multiplication to a transpose dense vector (\f$ \vec{y}^T+=s*\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side scaled multiplication expression to be added.
   // \return void
   //
   // This function implements the performance optimized addition assignment of a scaled transpose
   // dense vector-transpose dense matrix multiplication expression to a dense vector.
   */
   template< typename VT1  // Type of the target dense vector
           , bool TF >     // Transpose flag of the target dense vector
   friend inline void addAssign( DenseVector<VT1,TF>& lhs, const DVecScalarMultExpr& rhs )
   {
      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      typename VMM::LeftOperand  left ( rhs.vector_.leftOperand()  );
      typename VMM::RightOperand right( rhs.vector_.rightOperand() );

      if( right.rows() == 0UL || right.columns() == 0UL ) {
         return;
      }

      LT x( left  );  // Evaluation of the left-hand side dense vector operand
      RT A( right );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( x.size()    == left.size()    , "Invalid vector size"       );
      BLAZE_INTERNAL_ASSERT( A.rows()    == right.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == right.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.columns() == (~lhs).size()  , "Invalid vector size"       );

      if( ( IsExpression<MT>::value && !evaluate ) ||
          ( A.rows() * A.columns() < TDVECTDMATMULT_THRESHOLD ) )
         DVecScalarMultExpr::selectDefaultAddAssignKernel( ~lhs, x, A, rhs.scalar_ );
      else
         DVecScalarMultExpr::selectBlasAddAssignKernel( ~lhs, x, A, rhs.scalar_ );
   }
   //**********************************************************************************************

   //**Default addition assignment to dense vectors************************************************
   /*!\brief Default addition assignment of a scaled transpose dense vector-transpose dense
   //        matrix multiplication (\f$ \vec{y}^T+=s*\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side dense vector operand.
   // \param A The right-hand side dense matrix operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function implements the default addition assignment kernel for the scaled transpose
   // dense vector-transpose dense matrix multiplication.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename DisableIf< UseVectorizedDefaultKernel<VT1,VT2,MT1,ST2> >::Type
      selectDefaultAddAssignKernel( VT1& y, const VT2& x, const MT1& A, ST2 scalar )
   {
      y.addAssign( x * A * scalar );
   }
   //**********************************************************************************************

   //**Vectorized default addition assignment to dense vectors*************************************
   /*!\brief Vectorized default addition assignment of a scaled transpose dense vector-transpose
   //        dense matrix multiplication (\f$ \vec{y}^T+=s*\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side dense vector operand.
   // \param A The right-hand side dense matrix operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function implements the vectorized default addition assignment kernel for the scaled
   // transpose dense vector-transpose dense matrix multiplication.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseVectorizedDefaultKernel<VT1,VT2,MT1,ST2> >::Type
      selectDefaultAddAssignKernel( VT1& y, const VT2& x, const MT1& A, ST2 scalar )
   {
      typedef IntrinsicTrait<ElementType>  IT;

      const size_t M( A.rows()    );
      const size_t N( A.columns() );

      size_t j( 0UL );

      for( ; (j+8UL) <= N; j+=8UL ) {
         IntrinsicType xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            const IntrinsicType x1( x.get(i) );
            xmm1 = xmm1 + x1 * A.get(i,j    );
            xmm2 = xmm2 + x1 * A.get(i,j+1UL);
            xmm3 = xmm3 + x1 * A.get(i,j+2UL);
            xmm4 = xmm4 + x1 * A.get(i,j+3UL);
            xmm5 = xmm5 + x1 * A.get(i,j+4UL);
            xmm6 = xmm6 + x1 * A.get(i,j+5UL);
            xmm7 = xmm7 + x1 * A.get(i,j+6UL);
            xmm8 = xmm8 + x1 * A.get(i,j+7UL);
         }
         y[j    ] += sum( xmm1 ) * scalar;
         y[j+1UL] += sum( xmm2 ) * scalar;
         y[j+2UL] += sum( xmm3 ) * scalar;
         y[j+3UL] += sum( xmm4 ) * scalar;
         y[j+4UL] += sum( xmm5 ) * scalar;
         y[j+5UL] += sum( xmm6 ) * scalar;
         y[j+6UL] += sum( xmm7 ) * scalar;
         y[j+7UL] += sum( xmm8 ) * scalar;
      }
      for( ; (j+4UL) <= N; j+=4UL ) {
         IntrinsicType xmm1, xmm2, xmm3, xmm4;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            const IntrinsicType x1( x.get(i) );
            xmm1 = xmm1 + x1 * A.get(i,j    );
            xmm2 = xmm2 + x1 * A.get(i,j+1UL);
            xmm3 = xmm3 + x1 * A.get(i,j+2UL);
            xmm4 = xmm4 + x1 * A.get(i,j+3UL);
         }
         y[j    ] += sum( xmm1 ) * scalar;
         y[j+1UL] += sum( xmm2 ) * scalar;
         y[j+2UL] += sum( xmm3 ) * scalar;
         y[j+3UL] += sum( xmm4 ) * scalar;
      }
      for( ; (j+3UL) <= N; j+=3UL ) {
         IntrinsicType xmm1, xmm2, xmm3;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            const IntrinsicType x1( x.get(i) );
            xmm1 = xmm1 + x1 * A.get(i,j    );
            xmm2 = xmm2 + x1 * A.get(i,j+1UL);
            xmm3 = xmm3 + x1 * A.get(i,j+2UL);
         }
         y[j    ] += sum( xmm1 ) * scalar;
         y[j+1UL] += sum( xmm2 ) * scalar;
         y[j+2UL] += sum( xmm3 ) * scalar;
      }
      for( ; (j+2UL) <= N; j+=2UL ) {
         IntrinsicType xmm1, xmm2;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            const IntrinsicType x1( x.get(i) );
            xmm1 = xmm1 + x1 * A.get(i,j    );
            xmm2 = xmm2 + x1 * A.get(i,j+1UL);
         }
         y[j    ] += sum( xmm1 ) * scalar;
         y[j+1UL] += sum( xmm2 ) * scalar;
      }
      if( j < N ) {
         IntrinsicType xmm1;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            xmm1 = xmm1 + A.get(i,j) * x.get(i);
         }
         y[j] += sum( xmm1 ) * scalar;
      }
   }
   //**********************************************************************************************

   //**BLAS-based addition assignment to dense vectors (default)***********************************
   /*!\brief Default addition assignment of a scaled transpose dense vector-transpose dense matrix
   //        multiplication (\f$ \vec{y}^T+=s*\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side dense vector operand.
   // \param A The right-hand side dense matrix operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function relays to the default implementation of the addition assignment of a scaled
   // transpose dense vector-transpose dense matrix multiplication expression to a dense vector.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseDefaultKernel<VT1,VT2,MT1,ST2> >::Type
      selectBlasAddAssignKernel( VT1& y, const VT2& x, const MT1& A, ST2 scalar )
   {
      selectDefaultAddAssignKernel( y, x, A, scalar );
   }
   //**********************************************************************************************

   //**BLAS-based addition assignment to dense vectors (single precision)**************************
#if BLAZE_BLAS_MODE
   /*!\brief BLAS-based addition assignment of a scaled transpose dense vector-transpose dense
   //        matrix multiplication for single precision operands (\f$ \vec{y}^T+=s*\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side dense vector operand.
   // \param A The right-hand side dense matrix operand.
   // \param scalar The scaling factor
   // \return void
   //
   // This function performs the scaled transpose dense vector-transpose dense matrix
   // multiplication for single precision operands based on the BLAS cblas_sgemv() function.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseSinglePrecisionKernel<VT1,VT2,MT1,ST2> >::Type
      selectBlasAddAssignKernel( VT1& y, const VT2& x, const MT1& A, ST2 scalar )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename VT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename VT2::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename MT1::ElementType );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );

      cblas_sgemv( CblasColMajor, CblasTrans, M, N, scalar,
                   A.data(), lda, x.data(), 1, 1.0F, y.data(), 1 );
   }
#endif
   //**********************************************************************************************

   //**BLAS-based addition assignment to dense vectors (double precision)**************************
#if BLAZE_BLAS_MODE
   /*!\brief BLAS-based addition assignment of a scaled transpose dense vector-transose dense
   //        matrix multiplication for double precision operands (\f$ \vec{y}+=s*A*\vec{x} \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side dense vector operand.
   // \param A The right-hand side dense matrix operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function performs the scaled transpose dense vector-transpose dense matrix
   // multiplication for double precision operands based on the BLAS cblas_dgemv() function.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseDoublePrecisionKernel<VT1,VT2,MT1,ST2> >::Type
      selectBlasAddAssignKernel( VT1& y, const VT2& x, const MT1& A, ST2 scalar )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename VT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename VT2::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename MT1::ElementType );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );

      cblas_dgemv( CblasColMajor, CblasTrans, M, N, scalar,
                   A.data(), lda, x.data(), 1, 1.0, y.data(), 1 );
   }
#endif
   //**********************************************************************************************

   //**BLAS-based addition assignment to dense vectors (single precision complex)******************
#if BLAZE_BLAS_MODE
   /*!\brief BLAS-based addition assignment of a scaled transpose dense vector-transose dense matrix
   //        multiplication for single precision complex operands (\f$ \vec{y}+=s*A*\vec{x} \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side dense vector operand.
   // \param A The right-hand side dense matrix operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function performs the scaled transpose dense vector-transpose dense matrix
   // multiplication for single precision complex operands based on the BLAS cblas_cgemv()
   // function.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseSinglePrecisionComplexKernel<VT1,VT2,MT1> >::Type
      selectBlasAddAssignKernel( VT1& y, const VT2& x, const MT1& A, ST2 scalar )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename VT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename VT2::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( ST2 );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename VT1::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename VT2::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename MT1::ElementType::value_type );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const complex<float> alpha( scalar );
      const complex<float> beta ( 1.0F, 0.0F );

      cblas_cgemv( CblasColMajor, CblasTrans, M, N, &alpha,
                   A.data(), lda, x.data(), 1, &beta, y.data(), 1 );
   }
#endif
   //**********************************************************************************************

   //**BLAS-based addition assignment to dense vectors (double precision complex)******************
#if BLAZE_BLAS_MODE
   /*!\brief BLAS-based addition assignment of a scaled transpose dense vector-transose dense matrix
   //        multiplication for double precision complex operands (\f$ \vec{y}+=s*A*\vec{x} \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side dense vector operand.
   // \param A The right-hand side dense matrix operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function performs the scaled transpose dense vector-transpose dense matrix
   // multiplication for double precision complex operands based on the BLAS cblas_zgemv()
   // function.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseDoublePrecisionComplexKernel<VT1,VT2,MT1> >::Type
      selectBlasAddAssignKernel( VT1& y, const VT2& x, const MT1& A, ST2 scalar )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename VT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename VT2::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( ST2 );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename VT1::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename VT2::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename MT1::ElementType::value_type );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const complex<double> alpha( scalar );
      const complex<double> beta ( 1.0, 0.0 );

      cblas_zgemv( CblasColMajor, CblasTrans, M, N, &alpha,
                   A.data(), lda, x.data(), 1, &beta, y.data(), 1 );
   }
#endif
   //**********************************************************************************************

   //**Addition assignment to sparse vectors*******************************************************
   // No special implementation for the addition assignment to sparse vectors.
   //**********************************************************************************************

   //**Subtraction assignment to dense vectors*****************************************************
   /*!\brief Subtraction assignment of a scaled transpose dense vector-transpose dense matrix
   //        multiplication to a transpose dense vector (\f$ \vec{y}^T-=s*\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side scaled multiplication expression to be subtracted.
   // \return void
   //
   // This function implements the performance optimized subtraction assignment of a scaled
   // transpose dense vector-transpose dense matrix multiplication expression to a dense vector.
   */
   template< typename VT1  // Type of the target dense vector
           , bool TF >     // Transpose flag of the target dense vector
   friend inline void subAssign( DenseVector<VT1,TF>& lhs, const DVecScalarMultExpr& rhs )
   {
      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      typename VMM::LeftOperand  left ( rhs.vector_.leftOperand()  );
      typename VMM::RightOperand right( rhs.vector_.rightOperand() );

      if( right.rows() == 0UL || right.columns() == 0UL ) {
         return;
      }

      LT x( left  );  // Evaluation of the left-hand side dense vector operand
      RT A( right );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( x.size()    == left.size()    , "Invalid vector size"       );
      BLAZE_INTERNAL_ASSERT( A.rows()    == right.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == right.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.columns() == (~lhs).size()  , "Invalid vector size"       );

      if( ( IsExpression<MT>::value && !evaluate ) ||
          ( A.rows() * A.columns() < TDVECTDMATMULT_THRESHOLD ) )
         DVecScalarMultExpr::selectDefaultSubAssignKernel( ~lhs, x, A, rhs.scalar_ );
      else
         DVecScalarMultExpr::selectBlasSubAssignKernel( ~lhs, x, A, rhs.scalar_ );
   }
   //**********************************************************************************************

   //**Default subtraction assignment to dense vectors*********************************************
   /*!\brief Default subtraction assignment of a scaled transpose dense vector-transpose dense
   //        matrix multiplication (\f$ \vec{y}^T-=s*\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side dense vector operand.
   // \param A The right-hand side dense matrix operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function implements the default subtraction assignment kernel for the scaled transpose
   // dense vector-transpose dense matrix multiplication.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename DisableIf< UseVectorizedDefaultKernel<VT1,VT2,MT1,ST2> >::Type
      selectDefaultSubAssignKernel( VT1& y, const VT2& x, const MT1& A, ST2 scalar )
   {
      y.subAssign( x * A * scalar );
   }
   //**********************************************************************************************

   //**Vectorized default subtraction assignment to dense vectors**********************************
   /*!\brief Vectorized default subtraction assignment of a scaled transpose dense vector-transpose
   //        matrix multiplication (\f$ \vec{y}^T-=s*\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side dense vector operand.
   // \param A The right-hand side dense matrix operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function implements the vectorized default subtraction assignment kernel for the scaled
   // transpose dense vector-transpose dense matrix multiplication.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseVectorizedDefaultKernel<VT1,VT2,MT1,ST2> >::Type
      selectDefaultSubAssignKernel( VT1& y, const VT2& x, const MT1& A, ST2 scalar )
   {
      typedef IntrinsicTrait<ElementType>  IT;

      const size_t M( A.rows()    );
      const size_t N( A.columns() );

      size_t j( 0UL );

      for( ; (j+8UL) <= N; j+=8UL ) {
         IntrinsicType xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            const IntrinsicType x1( x.get(i) );
            xmm1 = xmm1 + x1 * A.get(i,j    );
            xmm2 = xmm2 + x1 * A.get(i,j+1UL);
            xmm3 = xmm3 + x1 * A.get(i,j+2UL);
            xmm4 = xmm4 + x1 * A.get(i,j+3UL);
            xmm5 = xmm5 + x1 * A.get(i,j+4UL);
            xmm6 = xmm6 + x1 * A.get(i,j+5UL);
            xmm7 = xmm7 + x1 * A.get(i,j+6UL);
            xmm8 = xmm8 + x1 * A.get(i,j+7UL);
         }
         y[j    ] -= sum( xmm1 ) * scalar;
         y[j+1UL] -= sum( xmm2 ) * scalar;
         y[j+2UL] -= sum( xmm3 ) * scalar;
         y[j+3UL] -= sum( xmm4 ) * scalar;
         y[j+4UL] -= sum( xmm5 ) * scalar;
         y[j+5UL] -= sum( xmm6 ) * scalar;
         y[j+6UL] -= sum( xmm7 ) * scalar;
         y[j+7UL] -= sum( xmm8 ) * scalar;
      }
      for( ; (j+4UL) <= N; j+=4UL ) {
         IntrinsicType xmm1, xmm2, xmm3, xmm4;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            const IntrinsicType x1( x.get(i) );
            xmm1 = xmm1 + x1 * A.get(i,j    );
            xmm2 = xmm2 + x1 * A.get(i,j+1UL);
            xmm3 = xmm3 + x1 * A.get(i,j+2UL);
            xmm4 = xmm4 + x1 * A.get(i,j+3UL);
         }
         y[j    ] -= sum( xmm1 ) * scalar;
         y[j+1UL] -= sum( xmm2 ) * scalar;
         y[j+2UL] -= sum( xmm3 ) * scalar;
         y[j+3UL] -= sum( xmm4 ) * scalar;
      }
      for( ; (j+3UL) <= N; j+=3UL ) {
         IntrinsicType xmm1, xmm2, xmm3;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            const IntrinsicType x1( x.get(i) );
            xmm1 = xmm1 + x1 * A.get(i,j    );
            xmm2 = xmm2 + x1 * A.get(i,j+1UL);
            xmm3 = xmm3 + x1 * A.get(i,j+2UL);
         }
         y[j    ] -= sum( xmm1 ) * scalar;
         y[j+1UL] -= sum( xmm2 ) * scalar;
         y[j+2UL] -= sum( xmm3 ) * scalar;
      }
      for( ; (j+2UL) <= N; j+=2UL ) {
         IntrinsicType xmm1, xmm2;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            const IntrinsicType x1( x.get(i) );
            xmm1 = xmm1 + x1 * A.get(i,j    );
            xmm2 = xmm2 + x1 * A.get(i,j+1UL);
         }
         y[j    ] -= sum( xmm1 ) * scalar;
         y[j+1UL] -= sum( xmm2 ) * scalar;
      }
      if( j < N ) {
         IntrinsicType xmm1;
         for( size_t i=0UL; i<M; i+=IT::size ) {
            xmm1 = xmm1 + A.get(i,j) * x.get(i);
         }
         y[j] -= sum( xmm1 ) * scalar;
      }
   }
   //**********************************************************************************************

   //**BLAS-based subtraction assignment to dense vectors (default)********************************
   /*!\brief Default subtraction assignment of a scaled transpose dense vector-transpose dense
   //        matrix multiplication (\f$ \vec{y}^T-=s*\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side dense vector operand.
   // \param A The right-hand side dense matrix operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function relays to the default implementation of the subtraction assignment of a
   // scaled transpose dense vector-transpose dense matrix multiplication expression to a dense
   // vector.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseDefaultKernel<VT1,VT2,MT1,ST2> >::Type
      selectBlasSubAssignKernel( VT1& y, const VT2& x, const MT1& A, ST2 scalar )
   {
      selectDefaultSubAssignKernel( y, x, A, scalar );
   }
   //**********************************************************************************************

   //**BLAS-based subtraction assignment to dense vectors (single precision)***********************
#if BLAZE_BLAS_MODE
   /*!\brief BLAS-based subtraction assignment of a scaled transpose dense vector-transpose dense
   //        matrix multiplication for single precision operands (\f$ \vec{y}^T-=s*\vec{x}^T*A \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side dense vector operand.
   // \param A The right-hand side dense matrix operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function performs the scaled transpose dense vector-transpose dense matrix
   // multiplication for single precision operands based on the BLAS cblas_sgemv() function.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseSinglePrecisionKernel<VT1,VT2,MT1,ST2> >::Type
      selectBlasSubAssignKernel( VT1& y, const VT2& x, const MT1& A, ST2 scalar )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename VT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename VT2::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE( typename MT1::ElementType );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );

      cblas_sgemv( CblasColMajor, CblasTrans, M, N, -scalar,
                   A.data(), lda, x.data(), 1, 1.0F, y.data(), 1 );
   }
#endif
   //**********************************************************************************************

   //**BLAS-based subtraction assignment to dense vectors (double precision)***********************
#if BLAZE_BLAS_MODE
   /*!\brief BLAS-based subtraction assignment of a scaled transpose dense vector-transpose dense
   //        matrix multiplication for double precision operands (\f$ \vec{y}-=s*A*\vec{x} \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side dense vector operand.
   // \param A The right-hand side dense matrix operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function performs the scaled transpose dense vector-transpose dense matrix
   // multiplication for double precision operands based on the BLAS cblas_dgemv() function.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseDoublePrecisionKernel<VT1,VT2,MT1,ST2> >::Type
      selectBlasSubAssignKernel( VT1& y, const VT2& x, const MT1& A, ST2 scalar )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename VT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename VT2::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE( typename MT1::ElementType );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );

      cblas_dgemv( CblasColMajor, CblasTrans, M, N, -scalar,
                   A.data(), lda, x.data(), 1, 1.0, y.data(), 1 );
   }
#endif
   //**********************************************************************************************

   //**BLAS-based subtraction assignment to dense vectors (single precision complex)***************
#if BLAZE_BLAS_MODE
   /*!\brief BLAS-based subtraction assignment of a scaled transpose dense vector-transpose
   //        dense matrix multiplication for single precision complex operands
   //        (\f$ \vec{y}-=s*A*\vec{x} \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side dense vector operand.
   // \param A The right-hand side dense matrix operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function performs the scaled transpose dense vector-transpose dense matrix
   // multiplication for single precision complex operands based on the BLAS cblas_cgemv()
   // function.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseSinglePrecisionComplexKernel<VT1,VT2,MT1> >::Type
      selectBlasSubAssignKernel( VT1& y, const VT2& x, const MT1& A, ST2 scalar )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename VT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename VT2::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( ST2 );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename VT1::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename VT2::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_FLOAT_TYPE  ( typename MT1::ElementType::value_type );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const complex<float> alpha( -scalar );
      const complex<float> beta ( 1.0F, 0.0F );

      cblas_cgemv( CblasColMajor, CblasTrans, M, N, &alpha,
                   A.data(), lda, x.data(), 1, &beta, y.data(), 1 );
   }
#endif
   //**********************************************************************************************

   //**BLAS-based subtraction assignment to dense vectors (double precision complex)***************
#if BLAZE_BLAS_MODE
   /*!\brief BLAS-based subtraction assignment of a scaled transpose dense vector-transpose
   //        dense matrix multiplication for double precision complex operands
   //        (\f$ \vec{y}-=s*A*\vec{x} \f$).
   // \ingroup dense_vector
   //
   // \param y The target left-hand side dense vector.
   // \param x The left-hand side dense vector operand.
   // \param A The right-hand side dense matrix operand.
   // \param scalar The scaling factor.
   // \return void
   //
   // This function performs the scaled transpose dense vector-transpose dense matrix
   // multiplication for double precision complex operands based on the BLAS cblas_zgemv()
   // function.
   */
   template< typename VT1    // Type of the left-hand side target vector
           , typename VT2    // Type of the left-hand side vector operand
           , typename MT1    // Type of the right-hand side matrix operand
           , typename ST2 >  // Type of the scalar value
   static inline typename EnableIf< UseDoublePrecisionComplexKernel<VT1,VT2,MT1> >::Type
      selectBlasSubAssignKernel( VT1& y, const VT2& x, const MT1& A, ST2 scalar )
   {
      using boost::numeric_cast;

      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename VT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename VT2::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( typename MT1::ElementType );
      BLAZE_CONSTRAINT_MUST_BE_COMPLEX_TYPE( ST2 );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename VT1::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename VT2::ElementType::value_type );
      BLAZE_CONSTRAINT_MUST_BE_DOUBLE_TYPE ( typename MT1::ElementType::value_type );

      const int M  ( numeric_cast<int>( A.rows() )    );
      const int N  ( numeric_cast<int>( A.columns() ) );
      const int lda( numeric_cast<int>( A.spacing() ) );
      const complex<double> alpha( -scalar );
      const complex<double> beta ( 1.0, 0.0 );

      cblas_zgemv( CblasColMajor, CblasTrans, M, N, &alpha,
                   A.data(), lda, x.data(), 1, &beta, y.data(), 1 );
   }
#endif
   //**********************************************************************************************

   //**Subtraction assignment to sparse vectors****************************************************
   // No special implementation for the subtraction assignment to sparse vectors.
   //**********************************************************************************************

   //**Multiplication assignment to dense vectors**************************************************
   /*!\brief Multiplication assignment of a scaled transpose dense vector-transpose dense matrix
   //        multiplication to a transpose dense vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side multiplication expression to be multiplied.
   // \return void
   //
   // This function implements the performance optimized subtraction assignment of a scaled
   // transpose dense vector-transpose dense matrix multiplication expression to a dense vector.
   */
   template< typename VT1  // Type of the target dense vector
           , bool TF >     // Transpose flag of the target dense vector
   friend inline void multAssign( DenseVector<VT1,TF>& lhs, const DVecScalarMultExpr& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_TRANSPOSE_VECTOR_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_REFERENCE_TYPE( typename ResultType::CompositeType );

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const ResultType tmp( rhs );
      multAssign( ~lhs, tmp );
   }
   //**********************************************************************************************

   //**Multiplication assignment to sparse vectors*******************************************************
   // No special implementation for the multiplication assignment to sparse vectors.
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( VMM );
   BLAZE_CONSTRAINT_MUST_BE_TRANSPOSE_VECTOR_TYPE( VMM );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( VT );
   BLAZE_CONSTRAINT_MUST_BE_TRANSPOSE_VECTOR_TYPE( VT );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( MT );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( MT );
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
/*!\brief Multiplication operator for the multiplication of a transpose dense vector and a
//        column-major dense matrix (\f$ \vec{y}^T=\vec{x}^T*A \f$).
// \ingroup dense_matrix
//
// \param vec The left-hand side transpose dense vector for the multiplication.
// \param mat The right-hand side column-major dense matrix for the multiplication.
// \return The resulting transpose vector.
// \exception std::invalid_argument Vector and matrix sizes do not match.
//
// This operator represents the multiplication between a transpose dense vector and a column-major
// dense matrix:

   \code
   using blaze::rowVector;
   using blaze::columnMajor;

   blaze::DynamicVector<double,rowVector> x, y;
   blaze::DynamicMatrix<double,columnMajor> A;
   // ... Resizing and initialization
   y = x * A;
   \endcode

// The operator returns an expression representing a transpose dense vector of the higher-order
// element type of the two involved element types \a T1::ElementType and \a T2::ElementType.
// Both the dense matrix type \a T1 and the dense vector type \a T2 as well as the two element
// types \a T1::ElementType and \a T2::ElementType have to be supported by the MathTrait class
// template.\n
// In case the current size of the vector \a vec doesn't match the current number of rows of
// the matrix \a mat, a \a std::invalid_argument is thrown.
*/
template< typename T1    // Type of the left-hand side dense vector
        , typename T2 >  // Type of the right-hand side dense matrix
inline const typename DisableIf< IsMatMatMultExpr<T2>, TDVecTDMatMultExpr<T1,T2> >::Type
   operator*( const DenseVector<T1,true>& vec, const DenseMatrix<T2,true>& mat )
{
   if( (~vec).size() != (~mat).rows() )
      throw std::invalid_argument( "Vector and matrix sizes do not match" );

   return TDVecTDMatMultExpr<T1,T2>( ~vec, ~mat );
}
//*************************************************************************************************

} // namespace blaze

#endif
