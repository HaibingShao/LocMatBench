//=================================================================================================
/*!
//  \file blaze/math/SparseVector.h
//  \brief Header file for all basic SparseVector functionality
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

#ifndef _BLAZE_MATH_SPARSEVECTOR_H_
#define _BLAZE_MATH_SPARSEVECTOR_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <ostream>
#include <boost/type_traits/remove_reference.hpp>
#include <blaze/math/expressions/DenseVector.h>
#include <blaze/math/expressions/DVecSVecMultExpr.h>
#include <blaze/math/expressions/SparseVector.h>
#include <blaze/math/expressions/SVecAbsExpr.h>
#include <blaze/math/expressions/SVecDVecMultExpr.h>
#include <blaze/math/expressions/SVecEvalExpr.h>
#include <blaze/math/expressions/SVecScalarDivExpr.h>
#include <blaze/math/expressions/SVecScalarMultExpr.h>
#include <blaze/math/expressions/SVecSVecAddExpr.h>
#include <blaze/math/expressions/SVecSVecMultExpr.h>
#include <blaze/math/expressions/SVecSVecSubExpr.h>
#include <blaze/math/expressions/SVecTransExpr.h>
#include <blaze/math/expressions/TDVecSVecMultExpr.h>
#include <blaze/math/expressions/TSVecSVecMultExpr.h>
#include <blaze/math/expressions/TSVecDVecMultExpr.h>
#include <blaze/math/shims/Equal.h>
#include <blaze/math/shims/IsDefault.h>
#include <blaze/math/Vector.h>
#include <blaze/util/Assert.h>
#include <blaze/util/Types.h>


namespace blaze {

//=================================================================================================
//
//  GLOBAL OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*!\name SparseVector operators */
//@{
template< typename T1, bool TF1, typename T2, bool TF2 >
inline bool operator==( const SparseVector<T1,TF1>& lhs, const SparseVector<T2,TF2>& rhs );

template< typename T1, bool TF1, typename T2, bool TF2 >
inline bool operator!=( const SparseVector<T1,TF1>& lhs, const SparseVector<T2,TF2>& rhs );

template< typename VT, bool TF >
inline std::ostream& operator<<( std::ostream& os, const SparseVector<VT,TF>& sv );
//@}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Equality operator for the comparison of two sparse vectors.
// \ingroup sparse_vector
//
// \param lhs The left-hand side sparse vector for the comparison.
// \param rhs The right-hand side sparse vector for the comparison.
// \return \a true if the two sparse vectors are equal, \a false if not.
*/
template< typename T1  // Type of the left-hand side sparse vector
        , bool TF1     // Transpose flag of the left-hand side sparse vector
        , typename T2  // Type of the right-hand side sparse vector
        , bool TF2 >   // Transpose flag of the right-hand side sparse vector
inline bool operator==( const SparseVector<T1,TF1>& lhs, const SparseVector<T2,TF2>& rhs )
{
   typedef typename T1::CompositeType  CT1;
   typedef typename T2::CompositeType  CT2;
   typedef typename boost::remove_reference<CT1>::type::ConstIterator  LhsConstIterator;
   typedef typename boost::remove_reference<CT2>::type::ConstIterator  RhsConstIterator;

   // Early exit in case the vector sizes don't match
   if( (~lhs).size() != (~rhs).size() ) return false;

   // Evaluation of the two sparse vector operands
   CT1 a( ~lhs );
   CT2 b( ~rhs );

   // In order to compare the two vectors, the data values of the lower-order data
   // type are converted to the higher-order data type within the equal function.
   const LhsConstIterator lend( a.end() );
   const RhsConstIterator rend( b.end() );

   LhsConstIterator lelem( a.begin() );
   RhsConstIterator relem( b.begin() );

   while( lelem < lend && relem < rend )
   {
      if( isDefault( lelem->value() ) ) { ++lelem; continue; }
      if( isDefault( relem->value() ) ) { ++relem; continue; }

      if( lelem->index() != relem->index() || !equal( lelem->value(), relem->value() ) ) {
         return false;
      }
      else {
         ++lelem;
         ++relem;
      }
   }

   while( lelem < lend ) {
      if( !isDefault( lelem->value() ) )
         return false;
      ++lelem;
   }

   while( relem < rend ) {
      if( !isDefault( relem->value() ) )
         return false;
      ++relem;
   }

   return true;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Inequality operator for the comparison of two sparse vectors.
// \ingroup sparse_vector
//
// \param lhs The left-hand side sparse vector for the comparison.
// \param rhs The right-hand side sparse vector for the comparison.
// \return \a true if the two vectors are not equal, \a false if they are equal.
*/
template< typename T1  // Type of the left-hand side sparse vector
        , bool TF1     // Transpose flag of the left-hand side sparse vector
        , typename T2  // Type of the right-hand side sparse vector
        , bool TF2 >   // Transpose flag of the right-hand side sparse vector
inline bool operator!=( const SparseVector<T1,TF1>& lhs, const SparseVector<T2,TF2>& rhs )
{
   return !( lhs == rhs );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Global output operator for sparse vectors.
// \ingroup sparse_vector
//
// \param os Reference to the output stream.
// \param sv Reference to a constant sparse vector object.
// \return Reference to the output stream.
*/
template< typename VT  // Type of the sparse vector
        , bool TF >    // Transpose flag
inline std::ostream& operator<<( std::ostream& os, const SparseVector<VT,TF>& sv )
{
   for( size_t i=0; i<(~sv).size(); ++i )
      os << (~sv)[i] << "\n";
   return os;
}
//*************************************************************************************************




//=================================================================================================
//
//  GLOBAL FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\name SparseVector functions */
//@{
template< typename VT, bool TF >
inline const typename VT::ElementType min( const SparseVector<VT,TF>& sv );

template< typename VT, bool TF >
inline const typename VT::ElementType max( const SparseVector<VT,TF>& sv );
//@}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the smallest element of the sparse vector.
//
// \return The smallest sparse vector element.
//
// This function returns the smallest element of the given sparse vector. This function can
// only be used for element types that support the smaller-than relationship. In case the
// vector currently has a size of 0, the returned value is the default value (e.g. 0 in case
// of fundamental data types).
//
// \b Note: In case the sparse vector is not completely filled, the zero elements are also
// taken into account. Example: the following compressed vector has only 2 non-zero elements.
// However, the minimum of this vector is 0:

                              \f[
                              \left(\begin{array}{*{4}{c}}
                              1 & 0 & 3 & 0 \\
                              \end{array}\right)
                              \f]
*/
template< typename VT  // Type of the sparse vector
        , bool TF >    // Transpose flag
inline const typename VT::ElementType min( const SparseVector<VT,TF>& sv )
{
   using boost::remove_reference;
   using blaze::min;

   typedef typename VT::ElementType    ET;
   typedef typename VT::CompositeType  SV;
   typedef typename remove_reference<SV>::type::ConstIterator  ConstIterator;

   SV a( ~sv );  // Evaluation of the sparse vector operand

   const ConstIterator end( a.end() );
   ConstIterator element( a.begin() );

   if( element == end ) {
      return ET();
   }
   else if( a.nonZeros() == a.size() ) {
      ET minimum( element->value() );
      ++element;
      for( ; element!=end; ++element )
         minimum = min( minimum, element->value() );
      return minimum;
   }
   else {
      ET minimum = ET();
      for( ; element!=end; ++element )
         minimum = min( minimum, element->value() );
      return minimum;
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the largest element of the sparse vector.
//
// \return The largest sparse vector element.
//
// This function returns the largest element of the given sparse vector. This function can
// only be used for element types that support the smaller-than relationship. In case the
// vector currently has a size of 0, the returned value is the default value (e.g. 0 in case
// of fundamental data types).
//
// \b Note: In case the compressed vector is not completely filled, the zero elements are also
// taken into account. Example: the following compressed vector has only 2 non-zero elements.
// However, the maximum of this vector is 0:

                              \f[
                              \left(\begin{array}{*{4}{c}}
                              -1 & 0 & -3 & 0 \\
                              \end{array}\right)
                              \f]
*/
template< typename VT  // Type of the sparse vector
        , bool TF >    // Transpose flag
inline const typename VT::ElementType max( const SparseVector<VT,TF>& sv )
{
   using boost::remove_reference;
   using blaze::max;

   typedef typename VT::ElementType    ET;
   typedef typename VT::CompositeType  SV;
   typedef typename remove_reference<SV>::type::ConstIterator  ConstIterator;

   SV a( ~sv );  // Evaluation of the sparse vector operand

   const ConstIterator end( a.end() );
   ConstIterator element( a.begin() );

   if( element == end ) {
      return ET();
   }
   else if( a.nonZeros() == a.size() ) {
      ET maximum( element->value() );
      ++element;
      for( ; element!=end; ++element )
         maximum = max( maximum, element->value() );
      return maximum;
   }
   else {
      ET maximum = ET();
      for( ; element!=end; ++element )
         maximum = max( maximum, element->value() );
      return maximum;
   }
}
//*************************************************************************************************

} // namespace blaze

#endif
