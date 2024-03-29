//=================================================================================================
/*!
//  \file blaze/math/dense/StaticVector.h
//  \brief Header file for the implementation of a fixed-size vector
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

#ifndef _BLAZE_MATH_DENSE_STATICVECTOR_H_
#define _BLAZE_MATH_DENSE_STATICVECTOR_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <algorithm>
#include <cmath>
#include <ostream>
#include <stdexcept>
#include <blaze/math/CMathTrait.h>
#include <blaze/math/DenseVector.h>
#include <blaze/math/Functions.h>
#include <blaze/math/Intrinsics.h>
#include <blaze/math/MathTrait.h>
#include <blaze/math/shims/IsDefault.h>
#include <blaze/math/shims/IsNaN.h>
#include <blaze/math/shims/Reset.h>
#include <blaze/math/Types.h>
#include <blaze/math/typetraits/CanAlias.h>
#include <blaze/math/typetraits/IsSparseVector.h>
#include <blaze/system/TransposeFlag.h>
#include <blaze/util/AlignedStorage.h>
#include <blaze/util/Assert.h>
#include <blaze/util/constraints/Builtin.h>
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
/*!\defgroup static_vector StaticVector
// \ingroup dense_vector
*/
/*!\brief Efficient implementation of a fixed-sized vector.
// \ingroup static_vector
//
// The StaticVector class template is the representation of a fixed-size vector with statically
// allocated elements of arbitrary type. The type of the elements, the number of elements and
// the transpose flag of the vector can be specified via the three template parameters:

   \code
   template< typename Type, size_t N, bool TF >
   class StaticVector;
   \endcode

//  - Type: specifies the type of the vector elements. StaticVector can be used with any
//          non-cv-qualified element type. The arithmetic operators for vector/vector and
//          vector/element operations with the same element type work for any element type
//          as long as the element type supports the arithmetic operation. Arithmetic operations
//          between vectors and elements of different element types are only supported for
//          all data types supported by the MathTrait class template (for details see the
//          MathTrait class description).
//  - N   : specifies the total number of vector elements. It is expected that StaticVector is
//          only used for tiny and small vectors.
//  - TF  : specifies whether the vector is a row vector (\a blaze::rowVector) or a column
//          vector (\a blaze::columnVector). The default value is \a blaze::columnVector.
//
// These contiguously stored elements can be direclty accessed with the subscript operator. The
// numbering of the vector elements is

                             \f[\left(\begin{array}{*{4}{c}}
                             0 & 1 & \cdots & N-1 \\
                             \end{array}\right)\f]

// The use of StaticVector is very natural and intuitive. All operations (addition, subtraction,
// multiplication, scaling, ...) can be performed on all possible combinations of dense and sparse
// vectors with fitting element types. The following example gives an impression of the use of a
// 2-dimensional StaticVector:

   \code
   using blaze::StaticVector;
   using blaze::CompressedVector;
   using blaze::StaticMatrix;

   StaticVector<double,2UL> a,  // Default initialized 2D vectors
   a[0] = 1.0;                  // Initialization of the first element
   a[1] = 2.0;                  // Initialization of the second element

   StaticVector<double,2UL> b( 3.0, 2.0 );  // Directly initialized 2D vector
   CompressedVector<float>  c( 4.0F );      // Empty single precision vector
   StaticVector<double,2UL> d;              // Default constructed static vector
   StaticMatrix<double,2UL,2UL> A;          // Default constructed static row-major matrix

   d = a + b;  // Vector addition between vectors of equal element type
   d = a - c;  // Vector subtraction between vector of different element type
   d = a * b;  // Component-wise vector multiplication

   a *= 2.0;      // In-place scaling of vector
   d  = a * 2.0;  // Scaling of vector a
   d  = 2.0 * a;  // Scaling of vector a

   d += a - b;  // Addition assignment
   d -= a + c;  // Subtraction assignment
   d *= a * b;  // Multiplication assignment

   double scalar = trans( a ) * b;  // Scalar/dot/inner product between two vectors

   A = a * trans( b );  // Outer product between two vectors
   \endcode
*/
template< typename Type                     // Data type of the vector
        , size_t N                          // Number of elements
        , bool TF = defaultTransposeFlag >  // Transpose flag
class StaticVector : public DenseVector< StaticVector<Type,N,TF>, TF >
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
   typedef StaticVector<Type,N,TF>   This;           //!< Type of this StaticVector instance.
   typedef This                      ResultType;     //!< Result type for expression template evaluations.
   typedef StaticVector<Type,N,!TF>  TransposeType;  //!< Transpose type for expression template evaluations.
   typedef Type                      ElementType;    //!< Type of the vector elements.
   typedef typename IT::Type         IntrinsicType;  //!< Intrinsic type of the vector elements.
   typedef const StaticVector&       CompositeType;  //!< Data type for composite expression templates.

   //! Vector length return type.
   /*! Return type of the StaticVector<Type,N,TF>::length function. */
   typedef typename CMathTrait<Type>::Type  LengthType;
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
                              explicit inline StaticVector();
                              explicit inline StaticVector( const Type& init );
                                       inline StaticVector( const StaticVector& v );
   template< typename Other >          inline StaticVector( const StaticVector<Other,N,TF>& v );
   template< typename VT >             inline StaticVector( const Vector<VT,TF>& v );
   template< typename Other > explicit inline StaticVector( const Other (&rhs)[N] );

   inline StaticVector( const Type& v1, const Type& v2 );
   inline StaticVector( const Type& v1, const Type& v2, const Type& v3 );
   inline StaticVector( const Type& v1, const Type& v2, const Type& v3, const Type& v4 );
   inline StaticVector( const Type& v1, const Type& v2, const Type& v3,
                        const Type& v4, const Type& v5 );
   inline StaticVector( const Type& v1, const Type& v2, const Type& v3,
                        const Type& v4, const Type& v5, const Type& v6 );
   //@}
   //**********************************************************************************************

   //**Destructor**********************************************************************************
   // No explicitly declared destructor.
   //**********************************************************************************************

   //**Data access functions***********************************************************************
   /*!\name Data access functions */
   //@{
   inline Type&       operator[]( size_t index );
   inline const Type& operator[]( size_t index ) const;
   inline Type*       data();
   inline const Type* data() const;
   //@}
   //**********************************************************************************************

   //**Assignment operators************************************************************************
   /*!\name Assignment operators */
   //@{
                              inline StaticVector& operator= ( Type rhs );
                              inline StaticVector& operator= ( const StaticVector& rhs );
   template< typename Other > inline StaticVector& operator= ( const StaticVector<Other,N,TF>& rhs );
   template< typename VT >    inline StaticVector& operator= ( const Vector<VT,TF>& rhs );
   template< typename Other > inline StaticVector& operator= ( const Other (&rhs)[N] );
   template< typename VT >    inline StaticVector& operator+=( const Vector<VT,TF>& rhs );
   template< typename VT >    inline StaticVector& operator-=( const Vector<VT,TF>& rhs );
   template< typename VT >    inline StaticVector& operator*=( const Vector<VT,TF>& rhs );

   template< typename Other >
   inline typename EnableIf< IsNumeric<Other>, StaticVector >::Type&
      operator*=( Other rhs );

   template< typename Other >
   inline typename EnableIf< IsNumeric<Other>, StaticVector >::Type&
      operator/=( Other rhs );
   //@}
   //**********************************************************************************************

   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
                              inline size_t             size() const;
                              inline size_t             nonZeros() const;
                              inline void               reset();
                              inline LengthType         length() const;
                              inline Type               sqrLength() const;
                              inline StaticVector&      normalize();
                              inline const StaticVector getNormalized() const;
   template< typename Other > inline StaticVector&      scale( Other scalar );
                              inline Type               min() const;
                              inline Type               max() const;
                              inline void               swap( StaticVector& v ) /* throw() */;
   //@}
   //**********************************************************************************************

 private:
   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename VT >
   struct VectorizedAssign {
      enum { value = vectorizable && VT::vectorizable &&
                     IsSame<Type,typename VT::ElementType>::value };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename VT >
   struct VectorizedAddAssign {
      enum { value = vectorizable && VT::vectorizable &&
                     IsSame<Type,typename VT::ElementType>::value &&
                     IntrinsicTrait<Type>::addition };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename VT >
   struct VectorizedSubAssign {
      enum { value = vectorizable && VT::vectorizable &&
                     IsSame<Type,typename VT::ElementType>::value &&
                     IntrinsicTrait<Type>::subtraction };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename VT >
   struct VectorizedMultAssign {
      enum { value = vectorizable && VT::vectorizable &&
                     IsSame<Type,typename VT::ElementType>::value &&
                     IntrinsicTrait<Type>::multiplication };
   };
   /*! \endcond */
   //**********************************************************************************************

 public:
   //**Expression template evaluation functions****************************************************
   /*!\name Expression template evaluation functions */
   //@{
   template< typename Other > inline bool          isAliased( const Other* alias ) const;
                              inline IntrinsicType get      ( size_t index ) const;

   template< typename VT >
   inline typename DisableIf< VectorizedAssign<VT> >::Type
      assign( const DenseVector<VT,TF>& rhs );

   template< typename VT >
   inline typename EnableIf< VectorizedAssign<VT> >::Type
      assign( const DenseVector<VT,TF>& rhs );

   template< typename VT > inline void assign( const SparseVector<VT,TF>& rhs );

   template< typename VT >
   inline typename DisableIf< VectorizedAddAssign<VT> >::Type
      addAssign( const DenseVector<VT,TF>& rhs );

   template< typename VT >
   inline typename EnableIf< VectorizedAddAssign<VT> >::Type
      addAssign( const DenseVector<VT,TF>& rhs );

   template< typename VT > inline void addAssign( const SparseVector<VT,TF>& rhs );

   template< typename VT >
   inline typename DisableIf< VectorizedSubAssign<VT> >::Type
      subAssign( const DenseVector<VT,TF>& rhs );

   template< typename VT >
   inline typename EnableIf< VectorizedSubAssign<VT> >::Type
      subAssign( const DenseVector<VT,TF>& rhs );

   template< typename VT > inline void subAssign( const SparseVector<VT,TF>& rhs );

   template< typename VT >
   inline typename DisableIf< VectorizedMultAssign<VT> >::Type
      multAssign( const DenseVector<VT,TF>& rhs );

   template< typename VT >
   inline typename EnableIf< VectorizedMultAssign<VT> >::Type
      multAssign( const DenseVector<VT,TF>& rhs );

   template< typename VT > inline void multAssign( const SparseVector<VT,TF>& rhs );
   //@}
   //**********************************************************************************************

 private:
   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   Type v_[NN];  //!< The statically allocated vector elements.
                 /*!< Access to the vector values is gained via the subscript operator.
                      The order of the elements is
                      \f[\left(\begin{array}{*{4}{c}}
                      0 & 1 & \cdots & N-1 \\
                      \end{array}\right)\f] */
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
/*!\brief The default constructor for StaticVector.
//
// All vector elements are initialized to the default value (i.e. 0 for integral data types).
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline StaticVector<Type,N,TF>::StaticVector()
{
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   if( IsBuiltin<Type>::value ) {
      for( size_t i=0UL; i<NN; ++i )
         v_[i] = Type();
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constructor for a homogenous initialization of all elements.
//
// \param init Initial value for all vector elements.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline StaticVector<Type,N,TF>::StaticVector( const Type& init )
{
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   for( size_t i=0UL; i<N; ++i )
      v_[i] = init;

   if( IsBuiltin<Type>::value ) {
      for( size_t i=N; i<NN; ++i )
         v_[i] = Type();
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief The copy constructor for StaticVector.
//
// \param v Vector to be copied.
//
// The copy constructor is explicitly defined in order to enable/facilitate NRV optimization.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline StaticVector<Type,N,TF>::StaticVector( const StaticVector& v )
{
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   for( size_t i=0UL; i<NN; ++i )
      v_[i] = v.v_[i];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Conversion constructor from different StaticVector instances.
//
// \param v Vector to be copied.
*/
template< typename Type     // Data type of the vector
        , size_t N          // Number of elements
        , bool TF >         // Transpose flag
template< typename Other >  // Data type of the foreign vector
inline StaticVector<Type,N,TF>::StaticVector( const StaticVector<Other,N,TF>& v )
{
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   for( size_t i=0UL; i<N; ++i )
      v_[i] = v[i];

   if( IsBuiltin<Type>::value ) {
      for( size_t i=N; i<NN; ++i )
         v_[i] = Type();
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Conversion constructor from different vectors.
//
// \param v Vector to be copied.
// \exception std::invalid_argument Invalid setup of static vector.
//
// This constructor initializes the static vector from the given vector. In case the size
// of the given vector does not match the size of the static vector (i.e. is not N), a
// \a std::invalid_argument exception is thrown.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the foreign vector
inline StaticVector<Type,N,TF>::StaticVector( const Vector<VT,TF>& v )
{
   using blaze::assign;

   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   if( (~v).size() != N )
      throw std::invalid_argument( "Invalid setup of static vector" );

   if( IsBuiltin<Type>::value ) {
      for( size_t i=( IsSparseVector<VT>::value )?( 0UL ):( N ); i<NN; ++i )
         v_[i] = Type();
   }

   assign( *this, ~v );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Array initialization of all vector elements.
//
// \param rhs N-dimensional array for the initialization.
//
// This assignment operator offers the option to directly initialize the elements of the vector:

   \code
   const double init[2] = { 1.0, 2.0 };
   blaze::StaticVector<double,2> v( init );
   \endcode

// The vector is initialized with the given values. Missing values are initialized with zero.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
template< typename Other >  // Data type of the initialization array
inline StaticVector<Type,N,TF>::StaticVector( const Other (&rhs)[N] )
{
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   for( size_t i=0UL; i<N; ++i )
      v_[i] = rhs[i];

   if( IsBuiltin<Type>::value ) {
      for( size_t i=N; i<NN; ++i )
         v_[i] = Type();
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constructor for 2-dimensional vectors.
//
// \param v1 The initializer for the first vector element.
// \param v2 The initializer for the second vector element.
//
// This constructor offers the option to create 2-dimensional vectors with specific elements:

   \code
   blaze::StaticVector<int,2> v( 1, 2 );
   \endcode
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline StaticVector<Type,N,TF>::StaticVector( const Type& v1, const Type& v2 )
{
   BLAZE_STATIC_ASSERT( N == 2UL );
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   v_[0] = v1;
   v_[1] = v2;

   if( IsBuiltin<Type>::value ) {
      for( size_t i=N; i<NN; ++i )
         v_[i] = Type();
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constructor for 3-dimensional vectors.
//
// \param v1 The initializer for the first vector element.
// \param v2 The initializer for the second vector element.
// \param v3 The initializer for the third vector element.
//
// This constructor offers the option to create 3-dimensional vectors with specific elements:

   \code
   blaze::StaticVector<int,3> v( 1, 2, 3 );
   \endcode
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline StaticVector<Type,N,TF>::StaticVector( const Type& v1, const Type& v2, const Type& v3 )
{
   BLAZE_STATIC_ASSERT( N == 3UL );
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   v_[0] = v1;
   v_[1] = v2;
   v_[2] = v3;

   if( IsBuiltin<Type>::value ) {
      for( size_t i=N; i<NN; ++i )
         v_[i] = Type();
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constructor for 4-dimensional vectors.
//
// \param v1 The initializer for the first vector element.
// \param v2 The initializer for the second vector element.
// \param v3 The initializer for the third vector element.
// \param v4 The initializer for the fourth vector element.
//
// This constructor offers the option to create 4-dimensional vectors with specific elements:

   \code
   blaze::StaticVector<int,4> v( 1, 2, 3, 4 );
   \endcode
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline StaticVector<Type,N,TF>::StaticVector( const Type& v1, const Type& v2,
                                              const Type& v3, const Type& v4 )
{
   BLAZE_STATIC_ASSERT( N == 4UL );
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   v_[0] = v1;
   v_[1] = v2;
   v_[2] = v3;
   v_[3] = v4;

   if( IsBuiltin<Type>::value ) {
      for( size_t i=N; i<NN; ++i )
         v_[i] = Type();
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constructor for 5-dimensional vectors.
//
// \param v1 The initializer for the first vector element.
// \param v2 The initializer for the second vector element.
// \param v3 The initializer for the third vector element.
// \param v4 The initializer for the fourth vector element.
// \param v5 The initializer for the fifth vector element.
//
// This constructor offers the option to create 5-dimensional vectors with specific elements:

   \code
   blaze::StaticVector<int,5> v( 1, 2, 3, 4, 5 );
   \endcode
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline StaticVector<Type,N,TF>::StaticVector( const Type& v1, const Type& v2, const Type& v3,
                                              const Type& v4, const Type& v5 )
{
   BLAZE_STATIC_ASSERT( N == 5UL );
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   v_[0] = v1;
   v_[1] = v2;
   v_[2] = v3;
   v_[3] = v4;
   v_[4] = v5;

   if( IsBuiltin<Type>::value ) {
      for( size_t i=N; i<NN; ++i )
         v_[i] = Type();
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constructor for 6-dimensional vectors.
//
// \param v1 The initializer for the first vector element.
// \param v2 The initializer for the second vector element.
// \param v3 The initializer for the third vector element.
// \param v4 The initializer for the fourth vector element.
// \param v5 The initializer for the fifth vector element.
// \param v6 The initializer for the sixth vector element.
//
// This constructor offers the option to create 6-dimensional vectors with specific elements:

   \code
   blaze::StaticVector<int,6> v( 1, 2, 3, 4, 5, 6 );
   \endcode
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline StaticVector<Type,N,TF>::StaticVector( const Type& v1, const Type& v2, const Type& v3,
                                              const Type& v4, const Type& v5, const Type& v6 )
{
   BLAZE_STATIC_ASSERT( N == 6UL );
   BLAZE_INTERNAL_ASSERT( !( reinterpret_cast<size_t>( v_ ) % IT::alignment ), "Invalid alignment detected" );

   v_[0] = v1;
   v_[1] = v2;
   v_[2] = v3;
   v_[3] = v4;
   v_[4] = v5;
   v_[5] = v6;

   if( IsBuiltin<Type>::value ) {
      for( size_t i=N; i<NN; ++i )
         v_[i] = Type();
   }
}
//*************************************************************************************************




//=================================================================================================
//
//  DATA ACCESS FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Subscript operator for the direct access to the vector elements.
//
// \param index Access index. The index has to be in the range \f$[0..N-1]\f$.
// \return Reference to the accessed value.
//
// In case BLAZE_USER_ASSERT() is active, this operator performs an index check.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline Type& StaticVector<Type,N,TF>::operator[]( size_t index )
{
   BLAZE_USER_ASSERT( index < N, "Invalid vector access index" );
   return v_[index];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Subscript operator for the direct access to the vector elements.
//
// \param index Access index. The index has to be in the range \f$[0..N-1]\f$.
// \return Reference-to-const to the accessed value.
//
// In case BLAZE_USER_ASSERT() is active, this operator performs an index check.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline const Type& StaticVector<Type,N,TF>::operator[]( size_t index ) const
{
   BLAZE_USER_ASSERT( index < N, "Invalid vector access index" );
   return v_[index];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Low-level data access to the vector elements.
//
// \return Pointer to the internal element storage.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline Type* StaticVector<Type,N,TF>::data()
{
   return v_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Low-level data access to the vector elements.
//
// \return Pointer to the internal element storage.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline const Type* StaticVector<Type,N,TF>::data() const
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
/*!\brief Homogenous assignment to all vector elements.
//
// \param rhs Scalar value to be assigned to all vector elements.
// \return Reference to the assigned vector.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline StaticVector<Type,N,TF>& StaticVector<Type,N,TF>::operator=( Type rhs )
{
   for( size_t i=0UL; i<N; ++i )
      v_[i] = rhs;
   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Copy assignment operator for StaticVector.
//
// \param rhs Vector to be copied.
// \return Reference to the assigned vector.
//
// Explicit definition of a copy assignment operator for performance reasons.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline StaticVector<Type,N,TF>& StaticVector<Type,N,TF>::operator=( const StaticVector& rhs )
{
   // This implementation was chosen for several reasons:
   // - it works for all possible element types (even types that could not be copied by 'memcpy')
   // - it is faster than the synthesized default copy assignment operator
   // - it is faster than an implementation with the C library function 'memcpy' in combination
   //   with a protection against self-assignment
   // - it goes without a protection against self-assignment
   for( size_t i=0UL; i<N; ++i )
      v_[i] = rhs.v_[i];
   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Assignment operator for different StaticVector instances.
//
// \param rhs Vector to be copied.
// \return Reference to the assigned vector.
*/
template< typename Type     // Data type of the vector
        , size_t N          // Number of elements
        , bool TF >         // Transpose flag
template< typename Other >  // Data type of the foreign vector
inline StaticVector<Type,N,TF>& StaticVector<Type,N,TF>::operator=( const StaticVector<Other,N,TF>& rhs )
{
   // This implementation was chosen for several reasons:
   // - it works for all possible element types (even types that could not be copied by 'memcpy')
   // - it is faster than the synthesized default copy assignment operator
   // - it is faster than an implementation with the C library function 'memcpy' in combination
   //   with a protection against self-assignment
   // - it goes without a protection against self-assignment
   for( size_t i=0UL; i<N; ++i )
      v_[i] = rhs[i];
   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Assignment operator for different vectors.
//
// \param rhs Vector to be copied.
// \return Reference to the assigned vector.
// \exception std::invalid_argument Invalid assignment to static vector.
//
// This constructor initializes the vector as a copy of the given vector. In case the
// size of the given vector is not N, a \a std::invalid_argument exception is thrown.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side vector
inline StaticVector<Type,N,TF>& StaticVector<Type,N,TF>::operator=( const Vector<VT,TF>& rhs )
{
   using blaze::assign;

   if( (~rhs).size() != N )
      throw std::invalid_argument( "Invalid assignment to static vector" );

   if( CanAlias<VT>::value && (~rhs).isAliased( this ) ) {
      StaticVector tmp( ~rhs );
      swap( tmp );
   }
   else {
      if( IsSparseVector<VT>::value )
         reset();
      assign( *this, ~rhs );
   }

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Array assignment to all vector elements.
//
// \param rhs N-dimensional array for the assignment.
// \return Reference to the assigned vector.
//
// This assignment operator offers the option to directly set all elements of the vector:

   \code
   const double init[2] = { 1.0, 2.0 };
   blaze::StaticVector<double,2> v;
   v = init;
   \endcode

// The vector is initialized with the given values. Missing values are initialized with zero.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
template< typename Other >  // Data type of the initialization array
inline StaticVector<Type,N,TF>& StaticVector<Type,N,TF>::operator=( const Other (&rhs)[N] )
{
   for( size_t i=0UL; i<N; ++i )
      v_[i] = rhs[i];
   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Addition assignment operator for the addition of a vector (\f$ \vec{a}+=\vec{b} \f$).
//
// \param rhs The right-hand side vector to be added to the vector.
// \return Reference to the vector.
// \exception std::invalid_argument Vector sizes do not match.
//
// In case the current sizes of the two vectors don't match, a \a std::invalid_argument exception
// is thrown.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side vector
inline StaticVector<Type,N,TF>& StaticVector<Type,N,TF>::operator+=( const Vector<VT,TF>& rhs )
{
   using blaze::addAssign;

   if( (~rhs).size() != N )
      throw std::invalid_argument( "Vector sizes do not match" );

   if( CanAlias<VT>::value && (~rhs).isAliased( this ) ) {
      StaticVector tmp( ~rhs );
      addAssign( *this, tmp );
   }
   else {
      addAssign( *this, ~rhs );
   }

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Subtraction assignment operator for the subtraction of a vector (\f$ \vec{a}-=\vec{b} \f$).
//
// \param rhs The right-hand side vector to be subtracted from the vector.
// \return Reference to the vector.
// \exception std::invalid_argument Vector sizes do not match.
//
// In case the current sizes of the two vectors don't match, a \a std::invalid_argument exception
// is thrown.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side vector
inline StaticVector<Type,N,TF>& StaticVector<Type,N,TF>::operator-=( const Vector<VT,TF>& rhs )
{
   using blaze::subAssign;

   if( (~rhs).size() != N )
      throw std::invalid_argument( "Vector sizes do not match" );

   if( CanAlias<VT>::value && (~rhs).isAliased( this ) ) {
      StaticVector tmp( ~rhs );
      subAssign( *this, tmp );
   }
   else {
      subAssign( *this, ~rhs );
   }

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Multiplication assignment operator for the multiplication of a vector
//        (\f$ \vec{a}*=\vec{b} \f$).
//
// \param rhs The right-hand side vector to be multiplied with the vector.
// \return Reference to the vector.
// \exception std::invalid_argument Vector sizes do not match.
//
// In case the current sizes of the two vectors don't match, a \a std::invalid_argument exception
// is thrown.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side vector
inline StaticVector<Type,N,TF>& StaticVector<Type,N,TF>::operator*=( const Vector<VT,TF>& rhs )
{
   using blaze::assign;

   if( (~rhs).size() != N )
      throw std::invalid_argument( "Vector sizes do not match" );

   if( CanAlias<VT>::value && (~rhs).isAliased( this ) ) {
      StaticVector tmp( *this * (~rhs) );
      this->operator=( tmp );
   }
   else {
      assign( *this, *this * (~rhs) );
   }

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Multiplication assignment operator for the multiplication between a vector and
//        a scalar value (\f$ \vec{a}*=s \f$).
//
// \param rhs The right-hand side scalar value for the multiplication.
// \return Reference to the vector.
*/
template< typename Type     // Data type of the vector
        , size_t N          // Number of elements
        , bool TF >         // Transpose flag
template< typename Other >  // Data type of the right-hand side scalar
inline typename EnableIf< IsNumeric<Other>, StaticVector<Type,N,TF> >::Type&
   StaticVector<Type,N,TF>::operator*=( Other rhs )
{
   for( size_t i=0UL; i<N; ++i )
      v_[i] *= rhs;
   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Division assignment operator for the division of a vector by a scalar value
//        (\f$ \vec{a}/=s \f$).
//
// \param rhs The right-hand side scalar value for the division.
// \return Reference to the vector.
//
// \b Note: A division by zero is only checked by an user assert.
*/
template< typename Type     // Data type of the vector
        , size_t N          // Number of elements
        , bool TF >         // Transpose flag
template< typename Other >  // Data type of the right-hand side scalar
inline typename EnableIf< IsNumeric<Other>, StaticVector<Type,N,TF> >::Type&
   StaticVector<Type,N,TF>::operator/=( Other rhs )
{
   BLAZE_USER_ASSERT( rhs != Other(0), "Division by zero detected" );

   typedef typename MathTrait<Type,Other>::DivType  DT;
   typedef typename If< IsNumeric<DT>, DT, Other >::Type  Tmp;

   // Depending on the two involved data types, an integer division is applied or a
   // floating point division is selected.
   if( IsNumeric<DT>::value && IsFloatingPoint<DT>::value ) {
      const Tmp tmp( Tmp(1)/static_cast<Tmp>( rhs ) );
      for( size_t i=0UL; i<N; ++i )
         v_[i] *= tmp;
   }
   else {
      for( size_t i=0UL; i<N; ++i )
         v_[i] /= rhs;
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
/*!\brief Returns the current size/dimension of the vector.
//
// \return The size of the vector.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline size_t StaticVector<Type,N,TF>::size() const
{
   return N;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the number of non-zero elements in the vector.
//
// \return The number of non-zero elements in the vector.
//
// Note that the number of non-zero elements is always less than or equal to the current size
// of the vector.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline size_t StaticVector<Type,N,TF>::nonZeros() const
{
   size_t nonzeros( 0 );

   for( size_t i=0UL; i<N; ++i ) {
      if( !isDefault( v_[i] ) )
         ++nonzeros;
   }

   return nonzeros;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Reset to the default initial values.
//
// \return void
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline void StaticVector<Type,N,TF>::reset()
{
   using blaze::reset;
   for( size_t i=0UL; i<N; ++i )
      reset( v_[i] );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Calculation of the vector length \f$|\vec{a}|\f$.
//
// \return The length of the vector.
//
// This function calculates the actual length of the vector. The return type of the length()
// function depends on the actual type of the vector instance:
//
// <table border="0" cellspacing="0" cellpadding="1">
//    <tr>
//       <td width="250px"> \b Type </td>
//       <td width="100px"> \b LengthType </td>
//    </tr>
//    <tr>
//       <td>float</td>
//       <td>float</td>
//    </tr>
//    <tr>
//       <td>integral data types and double</td>
//       <td>double</td>
//    </tr>
//    <tr>
//       <td>long double</td>
//       <td>long double</td>
//    </tr>
// </table>
//
// \b Note: This operation is only defined for built-in data types. In case \a Type is a user
// defined data type the attempt to use the length() function results in a compile time error!
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
#ifndef WIN32
inline typename StaticVector<Type,N,TF>::LengthType StaticVector<Type,N,TF>::length() const
#else
inline typename CMathTrait<Type>::Type StaticVector<Type,N,TF>::length() const
#endif
{
   BLAZE_CONSTRAINT_MUST_BE_BUILTIN_TYPE( Type );

   LengthType sum( 0 );
   for( size_t i=0UL; i<N; ++i )
      sum += v_[i] * v_[i];
   return std::sqrt( sum );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Calculation of the vector square length \f$|\vec{a}|^2\f$.
//
// \return The square length of the vector.
//
// This function calculates the actual square length of the vector.
//
// \b Note: This operation is only defined for built-in data types. In case \a Type is a user
// defined data type the attempt to use the length() function results in a compile time error!
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline Type StaticVector<Type,N,TF>::sqrLength() const
{
   BLAZE_CONSTRAINT_MUST_BE_BUILTIN_TYPE( Type );

   Type sum( 0 );
   for( size_t i=0UL; i<N; ++i )
      sum += v_[i] * v_[i];
   return sum;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Normalization of the vector (\f$|\vec{a}|=1\f$).
//
// \return Reference to the vector.
//
// Normalization of the vector to a length of 1. This operation is only defined for floating
// point vectors. The attempt to use this function for an integral vector results in a compile
// time error.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline StaticVector<Type,N,TF>& StaticVector<Type,N,TF>::normalize()
{
   BLAZE_CONSTRAINT_MUST_BE_FLOATING_POINT_TYPE( Type );

   const Type len( length() );

   if( len == Type(0) )
      return *this;

   const Type ilen( Type(1) / len );

   for( size_t i=0UL; i<N; ++i )
      v_[i] *= ilen;

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Calculation of the normalized vector (\f$|\vec{a}|=1\f$).
//
// \return The normalized vector.
//
// The function returns the normalized vector. This operation is only defined for floating
// point vectors. The attempt to use this function for an integral vector results in a compile
// time error.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline const StaticVector<Type,N,TF> StaticVector<Type,N,TF>::getNormalized() const
{
   BLAZE_CONSTRAINT_MUST_BE_FLOATING_POINT_TYPE( Type );

   const Type len( length() );

   if( len == Type(0) )
      return *this;

   const Type ilen( Type(1) / len );
   StaticVector tmp;

   for( size_t i=0UL; i<N; ++i )
      tmp[i] = v_[i] * ilen;

   return tmp;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Scaling of the vector by the scalar value \a scalar (\f$ \vec{a}*=s \f$).
//
// \param scalar The scalar value for the vector scaling.
// \return Reference to the vector.
*/
template< typename Type     // Data type of the vector
        , size_t N          // Number of elements
        , bool TF >         // Transpose flag
template< typename Other >  // Data type of the scalar value
inline StaticVector<Type,N,TF>& StaticVector<Type,N,TF>::scale( Other scalar )
{
   for( size_t i=0; i<N; ++i )
      v_[i] *= scalar;
   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the smallest element of the vector.
//
// \return The smallest vector element.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline Type StaticVector<Type,N,TF>::min() const
{
   using blaze::min;

   Type minimum( v_[0] );
   for( size_t i=1UL; i<N; ++i )
      minimum = min( minimum, v_[i] );
   return minimum;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the largest element of the vector.
//
// \return The largest vector element.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline Type StaticVector<Type,N,TF>::max() const
{
   using blaze::max;

   Type maximum( v_[0] );
   for( size_t i=1UL; i<N; ++i )
      maximum = max( maximum, v_[i] );
   return maximum;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Swapping the contents of two static vectors.
//
// \param v The vector to be swapped.
// \return void
// \exception no-throw guarantee.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline void StaticVector<Type,N,TF>::swap( StaticVector& v ) /* throw() */
{
   using std::swap;

   for( size_t i=0UL; i<N; ++i )
      swap( v_[i], v.v_[i] );
}
//*************************************************************************************************




//=================================================================================================
//
//  EXPRESSION TEMPLATE EVALUATION FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Returns whether the vector is aliased with the given address \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this vector, \a false if not.
*/
template< typename Type     // Data type of the vector
        , size_t N          // Number of elements
        , bool TF >         // Transpose flag
template< typename Other >  // Data type of the foreign expression
inline bool StaticVector<Type,N,TF>::isAliased( const Other* alias ) const
{
   return static_cast<const void*>( this ) == static_cast<const void*>( alias );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Access to the intrinsic elements of the vector.
//
// \param index Access index. The index has to be in the range \f$[0..N-1]\f$.
// \return Reference to the accessed values.
//
// This function offers a direct access to the intrinsic elements of the vector. It must \b NOT
// be called explicitly! It is used internally for the performance optimized evaluation of
// expression templates. Calling this function explicitly might result in erroneous results
// and/or in compilation errors.
*/
template< typename Type     // Data type of the vector
        , size_t N          // Number of elements
        , bool TF >         // Transpose flag
inline typename StaticVector<Type,N,TF>::IntrinsicType
   StaticVector<Type,N,TF>::get( size_t index ) const
{
   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( index            <  N  , "Invalid vector access index" );
   BLAZE_INTERNAL_ASSERT( index + IT::size <= NN , "Invalid vector access index" );
   BLAZE_INTERNAL_ASSERT( index % IT::size == 0UL, "Invalid vector access index" );

   return load( &v_[index] );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename DisableIf< typename StaticVector<Type,N,TF>::BLAZE_TEMPLATE VectorizedAssign<VT> >::Type
   StaticVector<Type,N,TF>::assign( const DenseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).size() == N, "Invalid vector sizes" );

   for( size_t i=0UL; i<N; ++i )
      v_[i] = (~rhs)[i];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Intrinsic optimized implementation of the assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< typename StaticVector<Type,N,TF>::BLAZE_TEMPLATE VectorizedAssign<VT> >::Type
   StaticVector<Type,N,TF>::assign( const DenseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).size() == N, "Invalid vector sizes" );

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   for( size_t i=0UL; i<N; i+=IT::size ) {
      store( v_+i, (~rhs).get(i) );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the assignment of a sparse vector.
//
// \param rhs The right-hand side sparse vector to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side sparse vector
inline void StaticVector<Type,N,TF>::assign( const SparseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).size() == N, "Invalid vector sizes" );

   typedef typename VT::ConstIterator  ConstIterator;

   for( ConstIterator element=(~rhs).begin(); element!=(~rhs).end(); ++element )
      v_[element->index()] = element->value();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the addition assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename DisableIf< typename StaticVector<Type,N,TF>::BLAZE_TEMPLATE VectorizedAddAssign<VT> >::Type
   StaticVector<Type,N,TF>::addAssign( const DenseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).size() == N, "Invalid vector sizes" );

   for( size_t i=0UL; i<N; ++i )
      v_[i] += (~rhs)[i];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Intrinsic optimized implementation of the addition assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< typename StaticVector<Type,N,TF>::BLAZE_TEMPLATE VectorizedAddAssign<VT> >::Type
   StaticVector<Type,N,TF>::addAssign( const DenseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).size() == N, "Invalid vector sizes" );

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   for( size_t i=0UL; i<N; i+=IT::size ) {
      store( v_+i, load( v_+i ) + (~rhs).get(i) );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the addition assignment of a sparse vector.
//
// \param rhs The right-hand side sparse vector to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side sparse vector
inline void StaticVector<Type,N,TF>::addAssign( const SparseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).size() == N, "Invalid vector sizes" );

   typedef typename VT::ConstIterator  ConstIterator;

   for( ConstIterator element=(~rhs).begin(); element!=(~rhs).end(); ++element )
      v_[element->index()] += element->value();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the subtraction assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename DisableIf< typename StaticVector<Type,N,TF>::BLAZE_TEMPLATE VectorizedSubAssign<VT> >::Type
   StaticVector<Type,N,TF>::subAssign( const DenseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).size() == N, "Invalid vector sizes" );

   for( size_t i=0UL; i<N; ++i )
      v_[i] -= (~rhs)[i];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Intrinsic optimized implementation of the subtraction assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< typename StaticVector<Type,N,TF>::BLAZE_TEMPLATE VectorizedSubAssign<VT> >::Type
   StaticVector<Type,N,TF>::subAssign( const DenseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).size() == N, "Invalid vector sizes" );

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   for( size_t i=0UL; i<N; i+=IT::size ) {
      store( v_+i, load( v_+i ) - (~rhs).get(i) );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the subtraction assignment of a sparse vector.
//
// \param rhs The right-hand side sparse vector to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side sparse vector
inline void StaticVector<Type,N,TF>::subAssign( const SparseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).size() == N, "Invalid vector sizes" );

   typedef typename VT::ConstIterator  ConstIterator;

   for( ConstIterator element=(~rhs).begin(); element!=(~rhs).end(); ++element )
      v_[element->index()] -= element->value();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the multiplication assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be multiplied.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename DisableIf< typename StaticVector<Type,N,TF>::BLAZE_TEMPLATE VectorizedMultAssign<VT> >::Type
   StaticVector<Type,N,TF>::multAssign( const DenseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).size() == N, "Invalid vector sizes" );

   for( size_t i=0UL; i<N; ++i )
      v_[i] *= (~rhs)[i];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Intrinsic optimized implementation of the multiplication assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be multiplied.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< typename StaticVector<Type,N,TF>::BLAZE_TEMPLATE VectorizedMultAssign<VT> >::Type
   StaticVector<Type,N,TF>::multAssign( const DenseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).size() == N, "Invalid vector sizes" );

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   for( size_t i=0UL; i<N; i+=IT::size ) {
      store( v_+i, load( v_+i ) * (~rhs).get(i) );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the multiplication assignment of a sparse vector.
//
// \param rhs The right-hand side sparse vector to be multiplied.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side sparse vector
inline void StaticVector<Type,N,TF>::multAssign( const SparseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( (~rhs).size() == N, "Invalid vector sizes" );

   typedef typename VT::ConstIterator  ConstIterator;

   const StaticVector tmp( *this );
   const ConstIterator end( (~rhs).end() );

   reset();

   for( ConstIterator element=(~rhs).begin(); element!=end; ++element )
      v_[element->index()] = tmp[element->index()] * element->value();
}
//*************************************************************************************************








//=================================================================================================
//
//  UNDEFINED CLASS TEMPLATE SPECIALIZATION
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Specialization of StaticVector for 0 elements.
// \ingroup static_vector
//
// This specialization of the StaticVector class template is left undefined and therefore
// prevents the instantiation for 0 elements.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
class StaticVector<Type,0UL,TF>;
/*! \endcond */
//*************************************************************************************************








//=================================================================================================
//
//  GLOBAL OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*!\name StaticVector operators */
//@{
template< typename Type, size_t N, bool TF >
std::ostream& operator<<( std::ostream& os, const StaticVector<Type,N,TF>& v );

template< typename Type, size_t N, bool TF >
inline bool isnan( const StaticVector<Type,N,TF>& v );

template< typename Type, size_t N, bool TF >
inline void reset( StaticVector<Type,N,TF>& v );

template< typename Type, size_t N, bool TF >
inline void clear( StaticVector<Type,N,TF>& v );

template< typename Type, size_t N, bool TF >
inline bool isDefault( const StaticVector<Type,N,TF>& v );

template< typename Type, size_t N, bool TF >
inline const DVecDVecMultExpr<StaticVector<Type,N,TF>,StaticVector<Type,N,TF>,TF>
   sq( const StaticVector<Type,N,TF>& v );

template< typename Type, bool TF >
inline const StaticVector<Type,2UL,TF> perp( const StaticVector<Type,2UL,TF>& v );

template< typename Type, bool TF >
inline const StaticVector<Type,3UL,TF> perp( const StaticVector<Type,3UL,TF>& v );

template< typename Type, size_t N, bool TF >
inline void swap( StaticVector<Type,N,TF>& a, StaticVector<Type,N,TF>& b ) /* throw() */;
//@}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Global output operator for static vectors.
// \ingroup static_vector
//
// \param os Reference to the output stream.
// \param v Reference to a constant vector object.
// \return Reference to the output stream.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
std::ostream& operator<<( std::ostream& os, const StaticVector<Type,N,TF>& v )
{
   os << "(" << v[0UL];
   for( size_t i=1UL; i<N; ++i )
      os << "," << v[i];
   return os << ")";
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checks the given vector for not-a-number elements.
// \ingroup static_vector
//
// \param v The vector to be checked for not-a-number elements.
// \return \a true if at least one element of the vector is not-a-number, \a false otherwise.
//
// This function checks the static vector for not-a-number (NaN) elements. If at least one
// element of the vector is not-a-number, the function returns \a true, otherwise it returns
// \a false.

   \code
   blaze::StaticVector<double,3> a;
   // ... Initialization
   if( isnan( a ) ) { ... }
   \endcode
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline bool isnan( const StaticVector<Type,N,TF>& v )
{
   for( size_t i=0UL; i<N; ++i ) {
      if( isnan( v[i] ) ) return true;
   }
   return false;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Resetting the given static vector.
// \ingroup static_vector
//
// \param v The vector to be resetted.
// \return void
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline void reset( StaticVector<Type,N,TF>& v )
{
   v.reset();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Clearing the given static vector.
// \ingroup static_vector
//
// \param v The vector to be cleared.
// \return void
//
// Clearing a static vector is equivalent to resetting it via the reset() function.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline void clear( StaticVector<Type,N,TF>& v )
{
   v.reset();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns whether the given static vector is in default state.
// \ingroup static_vector
//
// \param v The vector to be tested for its default state.
// \return \a true in case the given vector is component-wise zero, \a false otherwise.
//
// This function checks whether the static vector is in default state. For instance, in case
// the static vector is instantiated for a built-in integral or floating point data type, the
// function returns \a true in case all vector elements are 0 and \a false in case any vector
// element is not 0. Following example demonstrates the use of the \a isDefault function:

   \code
   blaze::StaticVector<double,3> a;
   // ... Initialization
   if( isDefault( a ) ) { ... }
   \endcode
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline bool isDefault( const StaticVector<Type,N,TF>& v )
{
   for( size_t i=0UL; i<N; ++i )
      if( !isDefault( v[i] ) ) return false;
   return true;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Squaring the given static vector.
// \ingroup static_vector
//
// \param v The static vector to be squared.
// \return The result of the square operation.
//
// This function calculates the component product of the given static vector. It has the same
// effect as multiplying the vector with itself (\f$ v * v \f$).
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline const DVecDVecMultExpr<StaticVector<Type,N,TF>,StaticVector<Type,N,TF>,TF>
   sq( const StaticVector<Type,N,TF>& v )
{
   return v * v;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Unary perp dot product operator for the calculation of a perpendicular vector
//        (\f$ \vec{a}=\vec{b}^\perp \f$).
//
// \param v The vector to be rotated.
// \return The perpendicular vector.
//
// The "perp dot product" \f$ \vec{a}^\perp \cdot b \f$ for the vectors \f$ \vec{a} \f$ and
// \f$ \vec{b} \f$ is a modification of the two-dimensional dot product in which \f$ \vec{a} \f$
// is replaced by the perpendicular vector rotated 90 degrees to the left defined by Hill (1994).
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline const StaticVector<Type,2UL,TF> perp( const StaticVector<Type,2UL,TF>& v )
{
   return StaticVector<Type,2UL,TF>( -v[1UL], v[0UL] );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Creates a perpendicular vector b which satisfies \f$ \vec{a} \cdot \vec{b} = 0 \f$.
//
// \param v The vector to be rotated.
// \return The perpendicular vector.
//
// \b Note: The perpendicular vector may have any length!
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline const StaticVector<Type,3UL,TF> perp( const StaticVector<Type,3UL,TF>& v )
{
   if( v[0] != Type() || v[1] != Type() )
      return StaticVector<Type,3UL,TF>( v[1UL], -v[0UL], Type() );
   else
      return StaticVector<Type,3UL,TF>( Type(), v[2UL], -v[1UL] );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Swapping the contents of two 2D vectors.
// \ingroup static_vector
//
// \param a The first vector to be swapped.
// \param b The second vector to be swapped.
// \return void
// \exception no-throw guarantee.
*/
template< typename Type  // Data type of the vector
        , size_t N       // Number of elements
        , bool TF >      // Transpose flag
inline void swap( StaticVector<Type,N,TF>& a, StaticVector<Type,N,TF>& b ) /* throw() */
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
template< typename T1, size_t N, bool TF, typename T2 >
struct MathTrait< StaticVector<T1,N,TF>, T2 >
{
   typedef INVALID_TYPE                                                HighType;
   typedef INVALID_TYPE                                                LowType;
   typedef INVALID_TYPE                                                AddType;
   typedef INVALID_TYPE                                                SubType;
   typedef StaticVector< typename MathTrait<T1,T2>::MultType, N, TF >  MultType;
   typedef StaticVector< typename MathTrait<T1,T2>::DivType , N, TF >  DivType;
   BLAZE_CONSTRAINT_MUST_BE_NUMERIC_TYPE( T2 );
};

template< typename T1, typename T2, size_t N, bool TF >
struct MathTrait< T1, StaticVector<T2,N,TF> >
{
   typedef INVALID_TYPE                                                HighType;
   typedef INVALID_TYPE                                                LowType;
   typedef INVALID_TYPE                                                AddType;
   typedef INVALID_TYPE                                                SubType;
   typedef StaticVector< typename MathTrait<T1,T2>::MultType, N, TF >  MultType;
   typedef INVALID_TYPE                                                DivType;
   BLAZE_CONSTRAINT_MUST_BE_NUMERIC_TYPE( T1 );
};

template< typename T1, size_t N, bool TF, typename T2 >
struct MathTrait< StaticVector<T1,N,TF>, StaticVector<T2,N,TF> >
{
   typedef StaticVector< typename MathTrait<T1,T2>::HighType, N, TF >  HighType;
   typedef StaticVector< typename MathTrait<T1,T2>::LowType , N, TF >  LowType;
   typedef StaticVector< typename MathTrait<T1,T2>::AddType , N, TF >  AddType;
   typedef StaticVector< typename MathTrait<T1,T2>::SubType , N, TF >  SubType;
   typedef StaticVector< typename MathTrait<T1,T2>::MultType, N, TF >  MultType;
   typedef INVALID_TYPE                                                DivType;
};

template< typename T1, size_t N, typename T2 >
struct MathTrait< StaticVector<T1,N,true>, StaticVector<T2,N,false> >
{
   typedef INVALID_TYPE                         HighType;
   typedef INVALID_TYPE                         LowType;
   typedef INVALID_TYPE                         AddType;
   typedef INVALID_TYPE                         SubType;
   typedef typename MathTrait<T1,T2>::MultType  MultType;
   typedef INVALID_TYPE                         DivType;
};

template< typename T1, size_t M, typename T2, size_t N >
struct MathTrait< StaticVector<T1,M,false>, StaticVector<T2,N,true> >
{
   typedef INVALID_TYPE                                                      HighType;
   typedef INVALID_TYPE                                                      LowType;
   typedef INVALID_TYPE                                                      AddType;
   typedef INVALID_TYPE                                                      SubType;
   typedef StaticMatrix< typename MathTrait<T1,T2>::MultType, M, N, false >  MultType;
   typedef INVALID_TYPE                                                      DivType;
};
/*! \endcond */
//*************************************************************************************************

} // namespace blaze

#endif
