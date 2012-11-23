//=================================================================================================
/*!
//  \file blaze/math/expressions/TSMatSVecMultExpr.h
//  \brief Header file for the transpose sparse matrix/sparse vector multiplication expression
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

#ifndef _BLAZE_MATH_EXPRESSIONS_TSMATSVECMULTEXPR_H_
#define _BLAZE_MATH_EXPRESSIONS_TSMATSVECMULTEXPR_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <stdexcept>
#include <vector>
#include <boost/type_traits/remove_reference.hpp>
#include <blaze/math/constraints/SparseMatrix.h>
#include <blaze/math/constraints/SparseVector.h>
#include <blaze/math/constraints/TransposeFlag.h>
#include <blaze/math/DynamicVector.h>
#include <blaze/math/Expression.h>
#include <blaze/math/expressions/Forward.h>
#include <blaze/math/expressions/SparseVector.h>
#include <blaze/math/MathTrait.h>
#include <blaze/math/shims/IsDefault.h>
#include <blaze/math/shims/Reset.h>
#include <blaze/math/typetraits/IsExpression.h>
#include <blaze/math/typetraits/IsMatMatMultExpr.h>
#include <blaze/math/typetraits/IsResizable.h>
#include <blaze/util/Assert.h>
#include <blaze/util/constraints/Reference.h>
#include <blaze/util/DisableIf.h>
#include <blaze/util/EnableIf.h>
#include <blaze/util/SelectType.h>
#include <blaze/util/Types.h>


namespace blaze {

//=================================================================================================
//
//  CLASS SMATDVECMULTEXPR
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Expression object for sparse matrix-sparse vector multiplications.
// \ingroup sparse_vector_expression
//
// The TSMatSVecMultExpr class represents the compile time expression for multiplications
// between column-major sparse matrices and sparse vectors.
*/
template< typename MT    // Type of the left-hand side sparse matrix
        , typename VT >  // Type of the right-hand side sparse vector
class TSMatSVecMultExpr : public SparseVector< TSMatSVecMultExpr<MT,VT>, false >
                        , private Expression
{
 private:
   //**Type definitions****************************************************************************
   typedef typename MT::ResultType     MRT;  //!< Result type of the left-hand side sparse matrix expression.
   typedef typename VT::ResultType     VRT;  //!< Result type of the right-hand side sparse vector expression.
   typedef typename MT::CompositeType  MCT;  //!< Composite type of the left-hand side sparse matrix expression.
   typedef typename VT::CompositeType  VCT;  //!< Composite type of the right-hand side sparse vector expression.
   //**********************************************************************************************

 public:
   //**Type definitions****************************************************************************
   typedef TSMatSVecMultExpr<MT,VT>               This;           //!< Type of this TSMatSVecMultExpr instance.
   typedef typename MathTrait<MRT,VRT>::MultType  ResultType;     //!< Result type for expression template evaluations.
   typedef typename ResultType::TransposeType     TransposeType;  //!< Transpose type for expression template evaluations.
   typedef typename ResultType::ElementType       ElementType;    //!< Resulting element type.

   //! Data type for composite expression templates.
   typedef const ResultType  CompositeType;

   //! Composite type of the left-hand side sparse matrix expression.
   typedef typename SelectType< IsExpression<MT>::value, const MT, const MT& >::Type  LeftOperand;

   //! Composite type of the right-hand side sparse vector expression.
   typedef typename SelectType< IsExpression<VT>::value, const VT, const VT& >::Type  RightOperand;

   //! Type for the assignment of the left-hand side sparse matrix operand.
   typedef MCT  LT;

   //! Type for the assignment of the right-hand side sparse vector operand.
   typedef typename SelectType< IsExpression<VT>::value, const VRT, VCT >::Type  RT;
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation flag for the detection of aliasing effects.
   enum { canAlias = ( IsReference<MCT>::value && ( !IsExpression<MT>::value || CanAlias<MT>::value ) ) ||
                     ( !IsExpression<VT>::value ) };
   //**********************************************************************************************

   //**Constructor*********************************************************************************
   /*!\brief Constructor for the TSMatSVecMultExpr class.
   //
   // \param mat The left-hand side sparse matrix operand of the multiplication expression.
   // \param vec The right-hand side sparse vector operand of the multiplication expression.
   */
   explicit inline TSMatSVecMultExpr( const MT& mat, const VT& vec )
      : mat_( mat )  // Left-hand side sparse matrix of the multiplication expression
      , vec_( vec )  // Right-hand side sparse vector of the multiplication expression
   {
      BLAZE_INTERNAL_ASSERT( mat_.columns() == vec_.size(), "Invalid matrix and vector sizes" );
   }
   //**********************************************************************************************

   //**Subscript operator**************************************************************************
   /*!\brief Subscript operator for the direct access to the vector elements.
   //
   // \param index Access index. The index has to be in the range \f$[0..N-1]\f$.
   // \return The accessed value.
   */
   inline const ElementType operator[]( size_t index ) const {
      BLAZE_INTERNAL_ASSERT( index < mat_.rows(), "Invalid vector access index" );

      typedef typename boost::remove_reference<MCT>::type::ConstIterator  MatrixIterator;
      typedef typename boost::remove_reference<VCT>::type::ConstIterator  VectorIterator;

      MCT A( mat_ );  // Evaluation of the left-hand side sparse matrix operand
      VCT x( vec_ );  // Evaluation of the right-hand side sparse vector operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == mat_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == mat_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( x.size()    == vec_.size()   , "Invalid vector size"       );

      ElementType res;

      const VectorIterator vend( x.end() );
      VectorIterator velem( x.begin() );

      if( vec_.size() > 0UL && velem != vend ) {
         res = A( index, velem->index() ) * velem->value();
         ++velem;
         for( ; velem!=vend; ++velem )
            res += A( index, velem->index() ) * velem->value();
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
      return mat_.rows();
   }
   //**********************************************************************************************

   //**NonZeros function***************************************************************************
   /*!\brief Returns an estimation for the number of non-zero elements in the sparse vector.
   //
   // \return The estimate for the number of non-zero elements in the sparse vector.
   */
   inline size_t nonZeros() const {
      return mat_.rows();
   }
   //**********************************************************************************************

   //**Left function*******************************************************************************
   /*!\brief Returns the left-hand side transpose sparse matrix operand.
   //
   // \return The left-hand side transpose sparse matrix operand.
   */
   inline LeftOperand leftOperand() const {
      return mat_;
   }
   //**********************************************************************************************

   //**Right function******************************************************************************
   /*!\brief Returns the right-hand side sparse vector operand.
   //
   // \return The right-hand side sparse vector operand.
   */
   inline RightOperand rightOperand() const {
      return vec_;
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
      return ( IsReference<MCT>::value && mat_.isAliased( alias ) ) ||
             ( !IsExpression<VT>::value && vec_.isAliased( alias ) );
   }
   //**********************************************************************************************

 private:
   //**Member variables****************************************************************************
   LeftOperand  mat_;  //!< Left-hand side sparse matrix of the multiplication expression.
   RightOperand vec_;  //!< Right-hand side sparse vector of the multiplication expression.
   //**********************************************************************************************

   //**Default assignment to dense vectors*********************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Default assignment of a transpose sparse matrix-sparse vector multiplication to a
   //        dense vector.
   // \ingroup sparse_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side multiplication expression to be assigned.
   // \return void
   //
   // This function implements the default assignment of a transpose sparse matrix-sparse vector
   // multiplication expression to a dense vector. This assign function is used in case the
   // element type of the target vector is resizable.
   */
   template< typename VT1 >  // Type of the target dense vector
   friend inline typename EnableIf< IsResizable<typename VT1::ElementType> >::Type
      assign( DenseVector<VT1,false>& lhs, const TSMatSVecMultExpr& rhs )
   {
      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      typedef typename boost::remove_reference<LT>::type::ConstIterator  MatrixIterator;
      typedef typename boost::remove_reference<RT>::type::ConstIterator  VectorIterator;

      // Resetting the left-hand side target dense vector
      reset( ~lhs );

      // Evaluation of the right-hand side sparse vector operand
      RT x( rhs.vec_ );
      if( x.nonZeros() == 0UL ) return;

      // Evaluation of the left-hand side sparse matrix operand
      LT A( rhs.mat_ );

      // Checking the evaluated operators
      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.mat_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.mat_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( x.size()    == rhs.vec_.size()   , "Invalid vector size"       );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).size()     , "Invalid vector size"       );

      // Performing the transpose sparse matrix-sparse vector multiplication
      const VectorIterator vend ( x.end() );
      VectorIterator       velem( x.begin() );

      for( ; velem!=vend; ++velem )
      {
         const MatrixIterator mend ( A.end( velem->index() )   );
         MatrixIterator       melem( A.begin( velem->index() ) );

         for( ; melem!=mend; ++melem ) {
            if( isDefault( (~lhs)[melem->index()] ) )
               (~lhs)[melem->index()] = melem->value() * velem->value();
            else
               (~lhs)[melem->index()] += melem->value() * velem->value();
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Optimized assignment to dense vectors*******************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Optimized assignment of a transpose sparse matrix-sparse vector multiplication to a
   //        dense vector.
   // \ingroup sparse_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side multiplication expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized assignment of a transpose sparse matrix-
   // sparse vector multiplication expression to a dense vector. This assign function is used in
   // case the element type of the target vector is not resizable.
   */
   template< typename VT1 >  // Type of the target dense vector
   friend inline typename DisableIf< IsResizable<typename VT1::ElementType> >::Type
      assign( DenseVector<VT1,false>& lhs, const TSMatSVecMultExpr& rhs )
   {
      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      typedef typename boost::remove_reference<LT>::type::ConstIterator  MatrixIterator;
      typedef typename boost::remove_reference<RT>::type::ConstIterator  VectorIterator;

      // Resetting the left-hand side target dense vector
      reset( ~lhs );

      // Evaluation of the right-hand side sparse vector operand
      RT x( rhs.vec_ );
      if( x.nonZeros() == 0UL ) return;

      // Evaluation of the left-hand side sparse matrix operand
      LT A( rhs.mat_ );

      // Checking the evaluated operators
      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.mat_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.mat_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( x.size()    == rhs.vec_.size()   , "Invalid vector size"       );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).size()     , "Invalid vector size"       );

      // Performing the transpose sparse matrix-sparse vector multiplication
      const VectorIterator vend ( x.end() );
      VectorIterator       velem( x.begin() );

      for( ; velem!=vend; ++velem )
      {
         const MatrixIterator mend ( A.end( velem->index() )   );
         MatrixIterator       melem( A.begin( velem->index() ) );

         for( ; melem!=mend; ++melem ) {
            (~lhs)[melem->index()] += melem->value() * velem->value();
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Assignment to sparse vectors****************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Assignment of a transpose sparse matrix-sparse vector multiplication to a sparse vector.
   // \ingroup sparse_vector
   //
   // \param lhs The target left-hand side sparse vector.
   // \param rhs The right-hand side multiplication expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized assignment of a transpose sparse matrix-
   // sparse vector multiplication expression to a sparse vector.
   */
   template< typename VT1 >  // Type of the target sparse vector
   friend inline void assign( SparseVector<VT1,false>& lhs, const TSMatSVecMultExpr& rhs )
   {
      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      typedef typename boost::remove_reference<LT>::type::ConstIterator  MatrixIterator;
      typedef typename boost::remove_reference<RT>::type::ConstIterator  VectorIterator;

      RT x( rhs.vec_ );  // Evaluation of the right-hand side sparse vector operand
      if( x.nonZeros() == 0UL ) return;

      LT A( rhs.mat_ );  // Evaluation of the left-hand side sparse matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.mat_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.mat_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( x.size()    == rhs.vec_.size()   , "Invalid vector size"       );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).size()     , "Invalid vector size"       );

      DynamicVector<ElementType> tmp( (~lhs).size() );
      std::vector<bool> indices( (~lhs).size(), false );
      size_t nonzeros( 0UL );

      const VectorIterator vend ( x.end() );
      VectorIterator       velem( x.begin() );

      for( ; velem!=vend; ++velem )
      {
         const MatrixIterator mend ( A.end( velem->index() )   );
         MatrixIterator       melem( A.begin( velem->index() ) );

         for( ; melem!=mend; ++melem ) {
            if( !indices[melem->index()] ) {
               indices[melem->index()] = true;
               ++nonzeros;
               tmp[melem->index()] = melem->value() * velem->value();
            }
            else {
               tmp[melem->index()] += melem->value() * velem->value();
            }
         }
      }

      (~lhs).reserve( nonzeros );

      for( size_t i=0UL; i<(~lhs).size(); ++i ) {
         if( indices[i] ) {
            (~lhs).append( i, tmp[i] );
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Addition assignment to dense vectors********************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Addition assignment of a transpose sparse matrix-sparse vector multiplication to a
   //        dense vector.
   // \ingroup sparse_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side multiplication expression to be added.
   // \return void
   //
   // This function implements the performance optimized addition assignment of a transpose
   // sparse matrix-sparse vector multiplication expression to a dense vector.
   */
   template< typename VT1 >  // Type of the target dense vector
   friend inline void addAssign( DenseVector<VT1,false>& lhs, const TSMatSVecMultExpr& rhs )
   {
      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      typedef typename boost::remove_reference<LT>::type::ConstIterator  MatrixIterator;
      typedef typename boost::remove_reference<RT>::type::ConstIterator  VectorIterator;

      // Evaluation of the right-hand side sparse vector operand
      RT x( rhs.vec_ );
      if( x.nonZeros() == 0UL ) return;

      // Evaluation of the left-hand side sparse matrix operand
      LT A( rhs.mat_ );

      // Checking the evaluated operators
      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.mat_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.mat_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( x.size()    == rhs.vec_.size()   , "Invalid vector size"       );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).size()     , "Invalid vector size"       );

      // Performing the transpose sparse matrix-sparse vector multiplication
      const VectorIterator vend ( x.end() );
      VectorIterator       velem( x.begin() );

      for( ; velem!=vend; ++velem )
      {
         const MatrixIterator mend ( A.end( velem->index() )   );
         MatrixIterator       melem( A.begin( velem->index() ) );

         for( ; melem!=mend; ++melem ) {
            (~lhs)[melem->index()] += melem->value() * velem->value();
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Addition assignment to sparse vectors*******************************************************
   // No special implementation for the addition assignment to sparse vectors.
   //**********************************************************************************************

   //**Subtraction assignment to dense vectors*****************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Subtraction assignment of a transpose sparse matrix-sparse vector multiplication to
   //        a dense vector.
   // \ingroup sparse_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side multiplication expression to be subtracted.
   // \return void
   //
   // This function implements the performance optimized subtraction assignment of a transpose
   // sparse matrix-sparse vector multiplication expression to a dense vector.
   */
   template< typename VT1 >  // Type of the target dense vector
   friend inline void subAssign( DenseVector<VT1,false>& lhs, const TSMatSVecMultExpr& rhs )
   {
      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      typedef typename boost::remove_reference<LT>::type::ConstIterator  MatrixIterator;
      typedef typename boost::remove_reference<RT>::type::ConstIterator  VectorIterator;

      // Evaluation of the right-hand side sparse vector operand
      RT x( rhs.vec_ );
      if( x.nonZeros() == 0UL ) return;

      // Evaluation of the left-hand side sparse matrix operand
      LT A( rhs.mat_ );

      // Checking the evaluated operators
      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.mat_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.mat_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( x.size()    == rhs.vec_.size()   , "Invalid vector size"       );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).size()     , "Invalid vector size"       );

      // Performing the transpose sparse matrix-sparse vector multiplication
      const VectorIterator vend ( x.end() );
      VectorIterator       velem( x.begin() );

      for( ; velem!=vend; ++velem )
      {
         const MatrixIterator mend ( A.end( velem->index() )   );
         MatrixIterator       melem( A.begin( velem->index() ) );

         for( ; melem!=mend; ++melem ) {
            (~lhs)[melem->index()] -= melem->value() * velem->value();
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Subtraction assignment to sparse vectors****************************************************
   // No special implementation for the subtraction assignment to sparse vectors.
   //**********************************************************************************************

   //**Multiplication assignment to dense vectors**************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Multiplication assignment of a transpose sparse matrix-sparse vector multiplication
   //        to a dense vector.
   // \ingroup sparse_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side multiplication expression to be multiplied.
   // \return void
   //
   // This function implements the performance optimized multiplication assignment of a transpose
   // sparse matrix-sparse vector multiplication expression to a dense vector.
   */
   template< typename VT1 >  // Type of the target dense vector
   friend inline void multAssign( DenseVector<VT1,false>& lhs, const TSMatSVecMultExpr& rhs )
   {
      BLAZE_CONSTRAINT_MUST_BE_SPARSE_VECTOR_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_NONTRANSPOSE_VECTOR_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_REFERENCE_TYPE( typename ResultType::CompositeType );

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const ResultType tmp( rhs );
      multAssign( ~lhs, tmp );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Multiplication assignment to sparse vectors*************************************************
   // No special implementation for the multiplication assignment to sparse vectors.
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE( MT );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( MT );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_VECTOR_TYPE( VT );
   BLAZE_CONSTRAINT_MUST_BE_NONTRANSPOSE_VECTOR_TYPE( VT );
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************




//=================================================================================================
//
//  GLOBAL BINARY ARITHMETIC OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Multiplication operator for the multiplication of a transpose sparse matrix and a
//        sparse vector (\f$ \vec{y}=A*\vec{x} \f$).
// \ingroup sparse_vector
//
// \param mat The left-hand side sparse matrix for the multiplication.
// \param vec The right-hand side sparse vector for the multiplication.
// \return The resulting vector.
// \exception std::invalid_argument Matrix and vector sizes do not match.
//
// This operator represents the multiplication between a transpose sparse matrix and a sparse
// vector:

   \code
   using blaze::columnMajor;
   using blaze::columnVector;

   blaze::CompressedMatrix<double,columnMajor> A;
   blaze::CompressedVector<double,columnVector> x, y;
   // ... Resizing and initialization
   y = A * x;
   \endcode

// The operator returns an expression representing a sparse vector of the higher-order element
// type of the two involved element types \a T1::ElementType and \a T2::ElementType. Both the
// sparse matrix type \a T1 and the sparse vector type \a T2 as well as the two element types
// \a T1::ElementType and \a T2::ElementType have to be supported by the MathTrait class
// template.\n
// In case the current size of the vector \a vec doesn't match the current number of columns
// of the matrix \a mat, a \a std::invalid_argument is thrown.
*/
template< typename T1    // Type of the left-hand side sparse matrix
        , typename T2 >  // Type of the right-hand side sparse vector
inline const typename DisableIf< IsMatMatMultExpr<T1>, TSMatSVecMultExpr<T1,T2> >::Type
   operator*( const SparseMatrix<T1,true>& mat, const SparseVector<T2,false>& vec )
{
   if( (~mat).columns() != (~vec).size() )
      throw std::invalid_argument( "Matrix and vector sizes do not match" );

   return TSMatSVecMultExpr<T1,T2>( ~mat, ~vec );
}
//*************************************************************************************************

} // namespace blaze

#endif
