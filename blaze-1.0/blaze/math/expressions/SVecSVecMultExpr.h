//=================================================================================================
/*!
//  \file blaze/math/expressions/SVecSVecMultExpr.h
//  \brief Header file for the sparse vector/sparse vector multiplication expression
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

#ifndef _BLAZE_MATH_EXPRESSIONS_SVECSVECMULTEXPR_H_
#define _BLAZE_MATH_EXPRESSIONS_SVECSVECMULTEXPR_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <stdexcept>
#include <boost/type_traits/remove_reference.hpp>
#include <blaze/math/constraints/SparseVector.h>
#include <blaze/math/constraints/TransposeFlag.h>
#include <blaze/math/Expression.h>
#include <blaze/math/expressions/Forward.h>
#include <blaze/math/expressions/SparseVector.h>
#include <blaze/math/Functions.h>
#include <blaze/math/MathTrait.h>
#include <blaze/math/typetraits/CanAlias.h>
#include <blaze/math/typetraits/IsExpression.h>
#include <blaze/util/Assert.h>
#include <blaze/util/constraints/Reference.h>
#include <blaze/util/SelectType.h>
#include <blaze/util/Types.h>


namespace blaze {

//=================================================================================================
//
//  CLASS SVECSVECMULTEXPR
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Expression object for sparse vector-sparse vector multiplications.
// \ingroup sparse_vector_expression
//
// The SVecSVecMultExpr class represents the compile time expression for componentwise
// multiplications between sparse vectors.
*/
template< typename VT1  // Type of the left-hand side sparse vector
        , typename VT2  // Type of the right-hand side sparse vector
        , bool TF >     // Transpose flag
class SVecSVecMultExpr : public SparseVector< SVecSVecMultExpr<VT1,VT2,TF>, TF >
                       , private Expression
{
 private:
   //**Type definitions****************************************************************************
   typedef typename VT1::ResultType     RT1;  //!< Result type of the left-hand side sparse vector expression.
   typedef typename VT2::ResultType     RT2;  //!< Result type of the right-hand side sparse vector expression.
   typedef typename VT1::CompositeType  CT1;  //!< Composite type of the left-hand side sparse vector expression.
   typedef typename VT2::CompositeType  CT2;  //!< Composite type of the right-hand side sparse vector expression.
   typedef typename VT1::TransposeType  TT1;  //!< Transpose type of the left-hand side sparse vector expression.
   typedef typename VT2::TransposeType  TT2;  //!< Transpose type of the right-hand side sparse vector expression.
   //**********************************************************************************************

 public:
   //**Type definitions****************************************************************************
   typedef SVecSVecMultExpr<VT1,VT2,TF>           This;           //!< Type of this SVecSVecMultExpr instance.
   typedef typename MathTrait<RT1,RT2>::MultType  ResultType;     //!< Result type for expression template evaluations.
   typedef typename ResultType::TransposeType     TransposeType;  //!< Transpose type for expression template evaluations.
   typedef typename ResultType::ElementType       ElementType;    //!< Resulting element type.
   typedef const ResultType                       CompositeType;  //!< Data type for composite expression templates.

   //! Composite type of the left-hand side sparse vector expression.
   typedef typename SelectType< IsExpression<VT1>::value, const VT1, const VT1& >::Type  LeftOperand;

   //! Composite type of the right-hand side sparse vector expression.
   typedef typename SelectType< IsExpression<VT2>::value, const VT2, const VT2& >::Type  RightOperand;
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation flag for the detection of aliasing effects.
   enum { canAlias = ( IsReference<CT1>::value && ( !IsExpression<VT1>::value || CanAlias<VT1>::value ) ) ||
                     ( IsReference<CT2>::value && ( !IsExpression<VT2>::value || CanAlias<VT2>::value ) ) };
   //**********************************************************************************************

   //**Constructor*********************************************************************************
   /*!\brief Constructor for the SVecSVecMultExpr class.
   */
   explicit inline SVecSVecMultExpr( const VT1& lhs, const VT2& rhs )
      : lhs_( lhs )  // Left-hand side sparse vector of the multiplication expression
      , rhs_( rhs )  // Right-hand side sparse vector of the multiplication expression
   {
      BLAZE_INTERNAL_ASSERT( lhs.size() == rhs.size(), "Invalid vector sizes" );
   }
   //**********************************************************************************************

   //**Subscript operator**************************************************************************
   /*!\brief Subscript operator for the direct access to the vector elements.
   //
   // \param index Access index. The index has to be in the range \f$[0..N-1]\f$.
   // \return The accessed value.
   */
   inline const ElementType operator[]( size_t index ) const {
      BLAZE_INTERNAL_ASSERT( index < lhs_.size(), "Invalid vector access index" );
      return lhs_[index] * rhs_[index];
   }
   //**********************************************************************************************

   //**Size function*******************************************************************************
   /*!\brief Returns the current size/dimension of the vector.
   //
   // \return The size of the vector.
   */
   inline size_t size() const {
      return lhs_.size();
   }
   //**********************************************************************************************

   //**NonZeros function***************************************************************************
   /*!\brief Returns the number of non-zero elements in the sparse vector.
   //
   // \return The number of non-zero elements in the sparse vector.
   */
   inline size_t nonZeros() const {
      return min( lhs_.nonZeros(), rhs_.nonZeros() );
   }
   //**********************************************************************************************

   //**Left operand access*************************************************************************
   /*!\brief Returns the left-hand side sparse vector operand.
   //
   // \return The left-hand side sparse vector operand.
   */
   inline LeftOperand leftOperand() const {
      return lhs_;
   }
   //**********************************************************************************************

   //**Right operand access************************************************************************
   /*!\brief Returns the right-hand side sparse vector operand.
   //
   // \return The right-hand side sparse vector operand.
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
      return ( IsReference<CT1>::value && lhs_.isAliased( alias ) ) ||
             ( IsReference<CT2>::value && rhs_.isAliased( alias ) );
   }
   //**********************************************************************************************

 private:
   //**Member variables****************************************************************************
   LeftOperand  lhs_;  //!< Left-hand side sparse vector of the multiplication expression.
   RightOperand rhs_;  //!< Right-hand side sparse vector of the multiplication expression.
   //**********************************************************************************************

   //**Assignment to dense vectors*****************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Assignment of a sparse vector-sparse vector multiplication to a dense vector.
   // \ingroup sparse_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side multiplication expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized assignment of a sparse vector-sparse
   // vector multiplication expression to a dense vector.
   */
   template< typename VT >  // Type of the target dense vector
   friend inline void assign( DenseVector<VT,TF>& lhs, const SVecSVecMultExpr& rhs )
   {
      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      typedef typename boost::remove_reference<CT1>::type::ConstIterator  LeftIterator;
      typedef typename boost::remove_reference<CT2>::type::ConstIterator  RightIterator;

      CT1 x( rhs.lhs_ );  // Evaluation of the left-hand side sparse vector operand
      CT2 y( rhs.rhs_ );  // Evaluation of the right-hand side sparse vector operand

      BLAZE_INTERNAL_ASSERT( x.size() == rhs.lhs_.size(), "Invalid vector size" );
      BLAZE_INTERNAL_ASSERT( y.size() == rhs.rhs_.size(), "Invalid vector size" );
      BLAZE_INTERNAL_ASSERT( x.size() == (~lhs).size()  , "Invalid vector size" );

      const LeftIterator  lend( x.end() );
      const RightIterator rend( y.end() );

      LeftIterator  l( x.begin()  );
      RightIterator r( y.begin() );

      for( ; l!=lend; ++l ) {
         while( r!=rend && r->index() < l->index() ) ++r;
         if( r==rend ) break;
         if( l->index() == r->index() ) {
            (~lhs)[l->index()] = l->value() * r->value();
            ++r;
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Assignment to sparse vectors****************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Assignment of a sparse vector-sparse vector multiplication to a sparse vector.
   // \ingroup sparse_vector
   //
   // \param lhs The target left-hand side sparse vector.
   // \param rhs The right-hand side multiplication expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized assignment of a sparse vector-sparse
   // vector multiplication expression to a sparse vector.
   */
   template< typename VT >  // Type of the target sparse vector
   friend inline void assign( SparseVector<VT,TF>& lhs, const SVecSVecMultExpr& rhs )
   {
      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      typedef typename boost::remove_reference<CT1>::type::ConstIterator  LeftIterator;
      typedef typename boost::remove_reference<CT2>::type::ConstIterator  RightIterator;

      CT1 x( rhs.lhs_ );  // Evaluation of the left-hand side sparse vector operand
      CT2 y( rhs.rhs_ );  // Evaluation of the right-hand side sparse vector operand

      BLAZE_INTERNAL_ASSERT( x.size() == rhs.lhs_.size(), "Invalid vector size" );
      BLAZE_INTERNAL_ASSERT( y.size() == rhs.rhs_.size(), "Invalid vector size" );
      BLAZE_INTERNAL_ASSERT( x.size() == (~lhs).size()  , "Invalid vector size" );

      const LeftIterator  lend( x.end() );
      const RightIterator rend( y.end() );

      LeftIterator  l( x.begin()  );
      RightIterator r( y.begin() );

      for( ; l!=lend; ++l ) {
         while( r!=rend && r->index() < l->index() ) ++r;
         if( r==rend ) break;
         if( l->index() == r->index() ) {
            (~lhs).append( l->index(), l->value() * r->value() );
            ++r;
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Addition assignment to dense vectors********************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Addition assignment of a sparse vector-sparse vector multiplication to a dense vector.
   // \ingroup sparse_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side multiplication expression to be added.
   // \return void
   //
   // This function implements the performance optimized addition assignment of a sparse vector-
   // sparse vector multiplication expression to a dense vector.
   */
   template< typename VT >  // Type of the target dense vector
   friend inline void addAssign( DenseVector<VT,TF>& lhs, const SVecSVecMultExpr& rhs )
   {
      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      typedef typename boost::remove_reference<CT1>::type::ConstIterator  LeftIterator;
      typedef typename boost::remove_reference<CT2>::type::ConstIterator  RightIterator;

      CT1 x( rhs.lhs_ );  // Evaluation of the left-hand side sparse vector operand
      CT2 y( rhs.rhs_ );  // Evaluation of the right-hand side sparse vector operand

      BLAZE_INTERNAL_ASSERT( x.size() == rhs.lhs_.size(), "Invalid vector size" );
      BLAZE_INTERNAL_ASSERT( y.size() == rhs.rhs_.size(), "Invalid vector size" );
      BLAZE_INTERNAL_ASSERT( x.size() == (~lhs).size()  , "Invalid vector size" );

      const LeftIterator  lend( x.end() );
      const RightIterator rend( y.end() );

      LeftIterator  l( x.begin() );
      RightIterator r( y.begin() );

      for( ; l!=lend; ++l ) {
         while( r!=rend && r->index() < l->index() ) ++r;
         if( r==rend ) break;
         if( l->index() == r->index() ) {
            (~lhs)[l->index()] += l->value() * r->value();
            ++r;
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
   /*!\brief Subtraction assignment of a sparse vector-sparse vector multiplication to a dense vector.
   // \ingroup sparse_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side multiplication expression to be subtracted.
   // \return void
   //
   // This function implements the performance optimized subtraction assignment of a sparse vector-
   // sparse vector multiplication expression to a dense vector.
   */
   template< typename VT >  // Type of the target dense vector
   friend inline void subAssign( DenseVector<VT,TF>& lhs, const SVecSVecMultExpr& rhs )
   {
      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      typedef typename boost::remove_reference<CT1>::type::ConstIterator  LeftIterator;
      typedef typename boost::remove_reference<CT2>::type::ConstIterator  RightIterator;

      CT1 x( rhs.lhs_ );  // Evaluation of the left-hand side sparse vector operand
      CT2 y( rhs.rhs_ );  // Evaluation of the right-hand side sparse vector operand

      BLAZE_INTERNAL_ASSERT( x.size() == rhs.lhs_.size(), "Invalid vector size" );
      BLAZE_INTERNAL_ASSERT( y.size() == rhs.rhs_.size(), "Invalid vector size" );
      BLAZE_INTERNAL_ASSERT( x.size() == (~lhs).size()  , "Invalid vector size" );

      const LeftIterator  lend( x.end() );
      const RightIterator rend( y.end() );

      LeftIterator  l( x.begin()  );
      RightIterator r( y.begin() );

      for( ; l!=lend; ++l ) {
         while( r!=rend && r->index() < l->index() ) ++r;
         if( r==rend ) break;
         if( l->index() == r->index() ) {
            (~lhs)[l->index()] -= l->value() * r->value();
            ++r;
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
   /*!\brief Multiplication assignment of a sparse vector-sparse vector multiplication to a dense vector.
   // \ingroup sparse_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side multiplication expression to be multiplied.
   // \return void
   //
   // This function implements the performance optimized multiplication assignment of a sparse
   // vector-sparse vector multiplication expression to a dense vector.
   */
   template< typename VT >  // Type of the target dense vector
   friend inline void multAssign( DenseVector<VT,TF>& lhs, const SVecSVecMultExpr& rhs )
   {
      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      typedef typename boost::remove_reference<CT1>::type::ConstIterator  LeftIterator;
      typedef typename boost::remove_reference<CT2>::type::ConstIterator  RightIterator;

      CT1 x( rhs.lhs_ );  // Evaluation of the left-hand side sparse vector operand
      CT2 y( rhs.rhs_ );  // Evaluation of the right-hand side sparse vector operand

      BLAZE_INTERNAL_ASSERT( x.size() == rhs.lhs_.size(), "Invalid vector size" );
      BLAZE_INTERNAL_ASSERT( y.size() == rhs.rhs_.size(), "Invalid vector size" );
      BLAZE_INTERNAL_ASSERT( x.size() == (~lhs).size()  , "Invalid vector size" );

      const LeftIterator  lend( x.end() );
      const RightIterator rend( y.end() );

      LeftIterator  l( x.begin()  );
      RightIterator r( y.begin() );

      size_t i( 0 );

      for( ; l!=lend; ++l ) {
         while( r!=rend && r->index() < l->index() ) ++r;
         if( r==rend ) break;
         if( l->index() == r->index() ) {
            for( ; i<r->index(); ++i )
               reset( (~lhs)[i] );
            (~lhs)[l->index()] *= l->value() * r->value();
            ++r;
            ++i;
         }
      }

      for( ; i<rhs.size(); ++i )
         reset( (~lhs)[i] );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Multiplication assignment to sparse vectors*************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Multiplication assignment of a sparse vector-sparse vector multiplication to a sparse vector.
   // \ingroup sparse_vector
   //
   // \param lhs The target left-hand side sparse vector.
   // \param rhs The right-hand side multiplication expression to be multiplied.
   // \return void
   //
   // This function implements the performance optimized multiplication assignment of a sparse
   // vector-sparse vector multiplication expression to a sparse vector.
   */
   template< typename VT >  // Type of the target sparse vector
   friend inline void multAssign( SparseVector<VT,TF>& lhs, const SVecSVecMultExpr& rhs )
   {
      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      typedef typename VT::ConstIterator                                  Iterator1;
      typedef typename boost::remove_reference<CT1>::type::ConstIterator  Iterator2;
      typedef typename boost::remove_reference<CT2>::type::ConstIterator  Iterator3;

      CT1 x( rhs.lhs_ );  // Evaluation of the left-hand side sparse vector operand
      CT2 y( rhs.rhs_ );  // Evaluation of the right-hand side sparse vector operand

      BLAZE_INTERNAL_ASSERT( x.size() == rhs.lhs_.size(), "Invalid vector size" );
      BLAZE_INTERNAL_ASSERT( y.size() == rhs.rhs_.size(), "Invalid vector size" );
      BLAZE_INTERNAL_ASSERT( x.size() == (~lhs).size()  , "Invalid vector size" );

      VT tmp( rhs.size(), rhs.nonZeros() );

      const Iterator1 end1( (~lhs).end() );
      const Iterator2 end2( x.end() );
      const Iterator3 end3( y.end() );

      Iterator1 i1( (~lhs).begin() );
      Iterator2 i2( x.begin() );
      Iterator3 i3( y.begin() );

      for( ; i1!=end1; ++i1 ) {
         while( i2!=end2 && i2->index() < i1->index() ) ++i2;
         if( i2==end2 ) break;
         while( i3!=end3 && i3->index() < i1->index() ) ++i3;
         if( i3==end3 ) break;
         if( i1->index() == i2->index() && i1->index() == i3->index() ) {
            tmp.append( i1->index(), i1->value() * i2->value() * i3->value() );
            ++i2;
            ++i3;
         }
      }

      swap( ~lhs, tmp );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_VECTOR_TYPE( VT1 );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_VECTOR_TYPE( VT2 );
   BLAZE_CONSTRAINT_MUST_BE_VECTOR_WITH_TRANSPOSE_FLAG( VT1, TF );
   BLAZE_CONSTRAINT_MUST_BE_VECTOR_WITH_TRANSPOSE_FLAG( VT2, TF );
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
/*!\brief Multiplication operator for the componentwise multiplication of two sparse vectors
//        (\f$ \vec{a}=\vec{b}*\vec{c} \f$).
// \ingroup sparse_vector
//
// \param lhs The left-hand side sparse vector for the component product.
// \param rhs The right-hand side sparse vector for the component product.
// \return The product of the two sparse vectors.
// \exception std::invalid_argument Vector sizes do not match.
//
// This operator represents the componentwise multiplication of two sparse vectors:

   \code
   blaze::CompressedVector<double> a, b, c;
   // ... Resizing and initialization
   c = a * b;
   \endcode

// The operator returns a sparse vector of the higher-order element type of the two involved
// vector element types \a T1::ElementType and \a T2::ElementType. Both vector types \a T1
// and \a T2 as well as the two element types \a T1::ElementType and \a T2::ElementType have
// to be supported by the MathTrait class template.\n
// In case the current sizes of the two given vectors don't match, a \a std::invalid_argument
// is thrown.
*/
template< typename T1  // Type of the left-hand side sparse vector
        , typename T2  // Type of the right-hand side sparse vector
        , bool TF >    // Transpose flag
inline const SVecSVecMultExpr<T1,T2,TF>
   operator*( const SparseVector<T1,TF>& lhs, const SparseVector<T2,TF>& rhs )
{
   if( (~lhs).size() != (~rhs).size() )
      throw std::invalid_argument( "Vector sizes do not match" );

   return SVecSVecMultExpr<T1,T2,TF>( ~lhs, ~rhs );
}
//*************************************************************************************************

} // namespace blaze

#endif
