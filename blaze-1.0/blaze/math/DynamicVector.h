//=================================================================================================
/*!
//  \file blaze/math/DynamicVector.h
//  \brief Implementation of an arbitrary sized vector
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

#ifndef _BLAZE_MATH_DYNAMICVECTOR_H_
#define _BLAZE_MATH_DYNAMICVECTOR_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <algorithm>
#include <cmath>
#include <fstream>
#include <ostream>
#include <stdexcept>
#include <blaze/math/CMathTrait.h>
#include <blaze/math/DenseVector.h>
#include <blaze/math/DynamicMatrix.h>
#include <blaze/math/Functions.h>
#include <blaze/math/Intrinsics.h>
#include <blaze/math/MathTrait.h>
#include <blaze/math/shims/IsDefault.h>
#include <blaze/math/shims/IsNaN.h>
#include <blaze/math/shims/Reset.h>
#include <blaze/math/Types.h>
#include <blaze/math/typetraits/CanAlias.h>
#include <blaze/math/typetraits/IsResizable.h>
#include <blaze/math/typetraits/IsSparseVector.h>
#include <blaze/system/CacheSize.h>
#include <blaze/system/Precision.h>
#include <blaze/system/Restrict.h>
#include <blaze/system/TransposeFlag.h>
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
#include <blaze/util/Memory.h>
#include <blaze/util/mpl/If.h>
#include <blaze/util/Null.h>
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
/*!\defgroup dynamic_vector DynamicVector
// \ingroup dense_vector
*/
/*!\brief Efficient implementation of an arbitrary sized vector.
// \ingroup dynamic_vector
//
// The DynamicVector class template is the representation of an arbitrary sized vector with
// dynamically allocated elements of arbitrary type. The type of the elements and the transpose
// flag of the vector can be specified via the two template parameters:

   \code
   template< typename Type, bool TF >
   class DynamicVector;
   \endcode

//  - Type: specifies the type of the vector elements. DynamicVector can be used with any
//          non-cv-qualified element type. The arithmetic operators for vector/vector and
//          vector/element operations with the same element type work for any element type
//          as long as the element type supports the arithmetic operation. Arithmetic operations
//          between vectors and elements of different element types are only supported for
//          all data types supported by the MathTrait class template (for details see the
//          MathTrait class description).
//  - TF  : specifies whether the vector is a row vector (\a blaze::rowVector) or a column
//          vector (\a blaze::columnVector). The default value is \a blaze::columnVector.
//
// These contiguously stored elements can be direclty accessed with the subscript operator. The
// numbering of the vector elements is

                             \f[\left(\begin{array}{*{5}{c}}
                             0 & 1 & 2 & \cdots & N-1 \\
                             \end{array}\right)\f]

// The use of DynamicVector is very natural and intuitive. All operations (addition, subtraction,
// multiplication, scaling, ...) can be performed on all possible combinations of dense and sparse
// vectors with fitting element types. The following example gives an impression of the use of
// DynamicVector:

   \code
   using blaze::DynamicVector;
   using blaze::CompressedVector;
   using blaze::DynamicMatrix;

   DynamicVector<double> a( 2 );  // Default constructed, non-initialized 2D vectors
   a[0] = 1.0;                    // Initialization of the first element
   a[1] = 2.0;                    // Initialization of the second element

   DynamicVector<double>   b( 2, 2.0  );  // Directly, homogeneously initialized 2D vector
   CompressedVector<float> c( 2 );        // Empty sparse single precision vector
   DynamicVector<double>   d;             // Default constructed dynamic vector
   DynamicMatrix<double>   A;             // Default constructed row-major matrix

   d = a + b;  // Vector addition between vectors of equal element type
   d = a - c;  // Vector subtraction between a dense and sparse vector with different element types
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
        , bool TF = defaultTransposeFlag >  // Transpose flag
class DynamicVector : public DenseVector< DynamicVector<Type,TF>, TF >
{
 private:
   //**Type definitions****************************************************************************
   typedef IntrinsicTrait<Type>  IT;  //!< Intrinsic trait for the vector element type.
   //**********************************************************************************************

 public:
   //**Type definitions****************************************************************************
   typedef DynamicVector<Type,TF>   This;           //!< Type of this DynamicVector instance.
   typedef This                     ResultType;     //!< Result type for expression template evaluations.
   typedef DynamicVector<Type,!TF>  TransposeType;  //!< Transpose type for expression template evaluations.
   typedef Type                     ElementType;    //!< Type of the vector elements.
   typedef typename IT::Type        IntrinsicType;  //!< Intrinsic type of the vector elements.
   typedef const DynamicVector&     CompositeType;  //!< Data type for composite expression templates.

   //! Vector length return type.
   /*! Return type of the DynamicVector<Type,TF>::length function. */
   typedef typename CMathTrait<Type>::Type  LengthType;
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation flag for intrinsic optimization.
   /*! The \a vectorizable compilation flag indicates whether expressions the vector is involved
       in can be optimized via intrinsics. In case the element type of the vector is an intrinsic
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
                           explicit inline DynamicVector();
                           explicit inline DynamicVector( size_t n );
                           explicit inline DynamicVector( size_t n, Type init );
                                    inline DynamicVector( const DynamicVector& v );
   template< typename VT >          inline DynamicVector( const Vector<VT,TF>& v );

   template< typename Other, size_t N >
   explicit inline DynamicVector( const Other (&rhs)[N] );
   //@}
   //**********************************************************************************************

   //**Destructor**********************************************************************************
   /*!\name Destructor */
   //@{
   inline ~DynamicVector();
   //@}
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
   template< typename Other, size_t N >
   inline DynamicVector& operator=( const Other (&rhs)[N] );

                           inline DynamicVector& operator= ( Type rhs );
                           inline DynamicVector& operator= ( const DynamicVector& rhs );
   template< typename VT > inline DynamicVector& operator= ( const Vector<VT,TF>& rhs );
   template< typename VT > inline DynamicVector& operator+=( const Vector<VT,TF>& rhs );
   template< typename VT > inline DynamicVector& operator-=( const Vector<VT,TF>& rhs );
   template< typename VT > inline DynamicVector& operator*=( const Vector<VT,TF>& rhs );

   template< typename Other >
   inline typename EnableIf< IsNumeric<Other>, DynamicVector >::Type&
      operator*=( Other rhs );

   template< typename Other >
   inline typename EnableIf< IsNumeric<Other>, DynamicVector >::Type&
      operator/=( Other rhs );
   //@}
   //**********************************************************************************************

   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
                              inline size_t              size() const;
                              inline size_t              capacity() const;
                              inline size_t              nonZeros() const;
                              inline void                reset();
                              inline void                clear();
                              inline void                resize( size_t n, bool preserve=true );
                              inline void                extend( size_t n, bool preserve=true );
                              inline void                reserve( size_t n );
                              inline LengthType          length() const;
                              inline const Type          sqrLength() const;
                              inline DynamicVector&      normalize();
                              inline const DynamicVector getNormalized() const;
   template< typename Other > inline DynamicVector&      scale( Other scalar );
                              inline void                swap( DynamicVector& v ) /* throw() */;
   //@}
   //**********************************************************************************************

   //**Read/Write functions************************************************************************
   /*!\name Read/Write functions */
   //@{
   void read ( const char* file );
   void write( const char* file, std::streamsize prec=6 ) const;
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
   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
   inline size_t adjustCapacity( size_t minCapacity ) const;
   //@}
   //**********************************************************************************************

   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   size_t size_;             //!< The current size/dimension of the vector.
   size_t capacity_;         //!< The maximum capacity of the vector.
   Type* BLAZE_RESTRICT v_;  //!< The dynamically allocated vector elements.
                             /*!< Access to the vector elements is gained via the subscript operator.
                                  The order of the elements is
                                  \f[\left(\begin{array}{*{5}{c}}
                                  0 & 1 & 2 & \cdots & N-1 \\
                                  \end{array}\right)\f] */
   //@}
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_NOT_BE_POINTER_TYPE  ( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_REFERENCE_TYPE( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_CONST         ( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_VOLATILE      ( Type );
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
/*!\brief The default constructor for DynamicVector.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline DynamicVector<Type,TF>::DynamicVector()
   : size_    ( 0UL )   // The current size/dimension of the vector
   , capacity_( 0UL )   // The maximum capacity of the vector
   , v_       ( NULL )  // The vector elements
{}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constructor for a vector of size \a n. No element initialization is performed!
//
// \param n The size of the vector.
//
// \b Note: This constructor is only responsible to allocate the required dynamic memory. No
//          element initialization is performed!
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline DynamicVector<Type,TF>::DynamicVector( size_t n )
   : size_    ( n )                            // The current size/dimension of the vector
   , capacity_( adjustCapacity( n ) )          // The maximum capacity of the vector
   , v_       ( allocate<Type>( capacity_ ) )  // The vector elements
{
   if( IsBuiltin<Type>::value ) {
      for( size_t i=size_; i<capacity_; ++i )
         v_[i] = Type();
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constructor for a homogeneous initialization of all \a n vector elements.
//
// \param n The size of the vector.
// \param init The initial value of the vector elements.
//
// All vector elements are initialized with the specified value.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline DynamicVector<Type,TF>::DynamicVector( size_t n, Type init )
   : size_    ( n )                            // The current size/dimension of the vector
   , capacity_( adjustCapacity( n ) )          // The maximum capacity of the vector
   , v_       ( allocate<Type>( capacity_ ) )  // The vector elements
{
   for( size_t i=0UL; i<size_; ++i )
      v_[i] = init;

   if( IsBuiltin<Type>::value ) {
      for( size_t i=size_; i<capacity_; ++i )
         v_[i] = Type();
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief The copy constructor for DynamicVector.
//
// \param v Vector to be copied.
//
// The copy constructor is explicitly defined due to the required dynamic memory management
// and in order to enable/facilitate NRV optimization.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline DynamicVector<Type,TF>::DynamicVector( const DynamicVector& v )
   : size_    ( v.size_ )                      // The current size/dimension of the vector
   , capacity_( adjustCapacity( v.size_ ) )    // The maximum capacity of the vector
   , v_       ( allocate<Type>( capacity_ ) )  // The vector elements
{
   BLAZE_INTERNAL_ASSERT( capacity_ <= v.capacity_, "Invalid capacity estimation" );

   for( size_t i=0UL; i<capacity_; ++i )
      v_[i] = v.v_[i];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Conversion constructor from different vectors.
//
// \param v Vector to be copied.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the foreign vector
inline DynamicVector<Type,TF>::DynamicVector( const Vector<VT,TF>& v )
   : size_    ( (~v).size() )                  // The current size/dimension of the vector
   , capacity_( adjustCapacity( size_ ) )      // The maximum capacity of the vector
   , v_       ( allocate<Type>( capacity_ ) )  // The vector elements
{
   using blaze::assign;

   if( IsBuiltin<Type>::value ) {
      if( IsSparseVector<VT>::value )
         reset();
      for( size_t i=size_; i<capacity_; ++i )
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
   const real init[4] = { 1, 2, 3 };
   blaze::DynamicVector<real> v( init );
   \endcode

// The vector is sized accoring to the size of the array and initialized with the given values.
// Missing values are initialized with zero (as e.g. the fourth element in the example).
*/
template< typename Type   // Data type of the vector
        , bool TF >       // Transpose flag
template< typename Other  // Data type of the initialization array
        , size_t N >      // Dimension of the initialization array
inline DynamicVector<Type,TF>::DynamicVector( const Other (&rhs)[N] )
   : size_    ( N )                            // The current size/dimension of the vector
   , capacity_( adjustCapacity( N ) )          // The maximum capacity of the vector
   , v_       ( allocate<Type>( capacity_ ) )  // The vector elements
{
   for( size_t i=0UL; i<N; ++i )
      v_[i] = rhs[i];

   if( IsBuiltin<Type>::value ) {
      for( size_t i=size_; i<capacity_; ++i )
         v_[i] = Type();
   }
}
//*************************************************************************************************




//=================================================================================================
//
//  DESTRUCTOR
//
//=================================================================================================

//*************************************************************************************************
/*!\brief The destructor for DynamicVector.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline DynamicVector<Type,TF>::~DynamicVector()
{
   deallocate( v_ );
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
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline Type& DynamicVector<Type,TF>::operator[]( size_t index )
{
   BLAZE_USER_ASSERT( index < size_, "Invalid vector access index" );
   return v_[index];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Subscript operator for the direct access to the vector elements.
//
// \param index Access index. The index has to be in the range \f$[0..N-1]\f$.
// \return Reference to the accessed value.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline const Type& DynamicVector<Type,TF>::operator[]( size_t index ) const
{
   BLAZE_USER_ASSERT( index < size_, "Invalid vector access index" );
   return v_[index];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Low-level data access to the vector elements.
//
// \return Pointer to the internal element storage.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline Type* DynamicVector<Type,TF>::data()
{
   return v_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Low-level data access to the matrix elements.
//
// \return Pointer to the internal element storage.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline const Type* DynamicVector<Type,TF>::data() const
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
/*!\brief Array assignment to all vector elements.
//
// \param rhs N-dimensional array for the assignment.
// \return Reference to the assigned vector.
//
// This assignment operator offers the option to directly set all elements of the vector:

   \code
   const real init[4] = { 1, 2, 3 };
   blaze::DynamicVector<real> v;
   v = init;
   \endcode

// The vector is resized accoring to the size of the array and initialized with the given values.
// Missing values are initialized with zero (as e.g. the fourth element in the example).
*/
template< typename Type   // Data type of the vector
        , bool TF >       // Transpose flag
template< typename Other  // Data type of the initialization array
        , size_t N >      // Dimension of the initialization array
inline DynamicVector<Type,TF>& DynamicVector<Type,TF>::operator=( const Other (&rhs)[N] )
{
   resize( N, false );

   for( size_t i=0UL; i<N; ++i )
      v_[i] = rhs[i];

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Homogenous assignment to all vector elements.
//
// \param rhs Scalar value to be assigned to all vector elements.
// \return Reference to the assigned vector.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline DynamicVector<Type,TF>& DynamicVector<Type,TF>::operator=( Type rhs )
{
   for( size_t i=0UL; i<size_; ++i )
      v_[i] = rhs;
   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Copy assignment operator for DynamicVector.
//
// \param rhs Vector to be copied.
// \return Reference to the assigned vector.
//
// The vector is resized according to the given N-dimensional vector and initialized as a
// copy of this vector.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline DynamicVector<Type,TF>& DynamicVector<Type,TF>::operator=( const DynamicVector& rhs )
{
   if( &rhs == this ) return *this;

   resize( rhs.size_, false );

   for( size_t i=0UL; i<size_; ++i )
      v_[i] = rhs.v_[i];

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Assignment operator for different vectors.
//
// \param rhs Vector to be copied.
// \return Reference to the assigned vector.
//
// The vector is resized according to the given vector and initialized as a copy of this vector.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side vector
inline DynamicVector<Type,TF>& DynamicVector<Type,TF>::operator=( const Vector<VT,TF>& rhs )
{
   using blaze::assign;

   if( CanAlias<VT>::value && (~rhs).isAliased( this ) ) {
      DynamicVector tmp( ~rhs );
      swap( tmp );
   }
   else {
      resize( (~rhs).size(), false );
      if( IsSparseVector<VT>::value )
         reset();
      assign( *this, ~rhs );
   }

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
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side vector
inline DynamicVector<Type,TF>& DynamicVector<Type,TF>::operator+=( const Vector<VT,TF>& rhs )
{
   using blaze::addAssign;

   if( (~rhs).size() != size_ )
      throw std::invalid_argument( "Vector sizes do not match" );

   if( CanAlias<VT>::value && (~rhs).isAliased( this ) ) {
      typename VT::ResultType tmp( ~rhs );
      addAssign( *this, tmp );
   }
   else {
      addAssign( *this, ~rhs );
   }

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Subtraction assignment operator for the subtraction of a vector
//        (\f$ \vec{a}-=\vec{b} \f$).
//
// \param rhs The right-hand side vector to be subtracted from the vector.
// \return Reference to the vector.
// \exception std::invalid_argument Vector sizes do not match.
//
// In case the current sizes of the two vectors don't match, a \a std::invalid_argument exception
// is thrown.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side vector
inline DynamicVector<Type,TF>& DynamicVector<Type,TF>::operator-=( const Vector<VT,TF>& rhs )
{
   using blaze::subAssign;

   if( (~rhs).size() != size_ )
      throw std::invalid_argument( "Vector sizes do not match" );

   if( CanAlias<VT>::value && (~rhs).isAliased( this ) ) {
      typename VT::ResultType tmp( ~rhs );
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
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side vector
inline DynamicVector<Type,TF>& DynamicVector<Type,TF>::operator*=( const Vector<VT,TF>& rhs )
{
   using blaze::assign;

   if( (~rhs).size() != size_ )
      throw std::invalid_argument( "Vector sizes do not match" );

   if( CanAlias<VT>::value && (~rhs).isAliased( this ) ) {
      DynamicVector<Type,TF> tmp( *this * (~rhs) );
      swap( tmp );
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
        , bool TF >         // Transpose flag
template< typename Other >  // Data type of the right-hand side scalar
inline typename EnableIf< IsNumeric<Other>, DynamicVector<Type,TF> >::Type&
   DynamicVector<Type,TF>::operator*=( Other rhs )
{
   for( size_t i=0UL; i<size_; ++i )
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
        , bool TF >         // Transpose flag
template< typename Other >  // Data type of the right-hand side scalar
inline typename EnableIf< IsNumeric<Other>, DynamicVector<Type,TF> >::Type&
   DynamicVector<Type,TF>::operator/=( Other rhs )
{
   BLAZE_USER_ASSERT( rhs != Other(0), "Division by zero detected" );

   typedef typename MathTrait<Type,Other>::DivType  DT;
   typedef typename If< IsNumeric<DT>, DT, Other >::Type  Tmp;

   // Depending on the two involved data types, an integer division is applied or a
   // floating point division is selected.
   if( IsNumeric<DT>::value && IsFloatingPoint<DT>::value ) {
      const Tmp tmp( Tmp(1)/static_cast<Tmp>( rhs ) );
      for( size_t i=0UL; i<size_; ++i )
         v_[i] *= tmp;
   }
   else {
      for( size_t i=0UL; i<size_; ++i )
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
        , bool TF >      // Transpose flag
inline size_t DynamicVector<Type,TF>::size() const
{
   return size_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the maximum capacity of the vector.
//
// \return The capacity of the vector.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline size_t DynamicVector<Type,TF>::capacity() const
{
   return capacity_;
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
        , bool TF >      // Transpose flag
inline size_t DynamicVector<Type,TF>::nonZeros() const
{
   size_t nonzeros( 0 );

   for( size_t i=0UL; i<size_; ++i ) {
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
        , bool TF >      // Transpose flag
inline void DynamicVector<Type,TF>::reset()
{
   using blaze::reset;
   for( size_t i=0UL; i<size_; ++i )
      reset( v_[i] );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Clearing the vector.
//
// \return void
//
// After the clear() function, the size of the vector is 0.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline void DynamicVector<Type,TF>::clear()
{
   size_ = 0UL;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Changing the size of the vector.
//
// \param n The new size of the vector.
// \param preserve \a true if the old values of the vector should be preserved, \a false if not.
// \return void
//
// This function resizes the vector using the given size to \a n. During this operation, new
// dynamic memory may be allocated in case the capacity of the vector is too small. Therefore
// this function potentially changes all vector elements. In order to preserve the old vector
// values, the \a preserve flag can be set to \a true. However, new vector elements are not
// initialized!\n
// The following example illustrates the resize operation of a vector of size 2 to a vector of
// size 4. The new, uninitialized elements are marked with \a x:

                              \f[
                              \left(\begin{array}{*{2}{c}}
                              1 & 2 \\
                              \end{array}\right)

                              \Longrightarrow

                              \left(\begin{array}{*{4}{c}}
                              1 & 2 & x & x \\
                              \end{array}\right)
                              \f]
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline void DynamicVector<Type,TF>::resize( size_t n, bool preserve )
{
   if( n > capacity_ )
   {
      // Allocating a new array
      const size_t newCapacity( adjustCapacity( n ) );
      Type* BLAZE_RESTRICT tmp = allocate<Type>( newCapacity );

      // Initializing the new array
      if( preserve ) {
         std::copy( v_, v_+size_, tmp );
      }

      if( IsBuiltin<Type>::value ) {
         for( size_t i=size_; i<newCapacity; ++i )
            tmp[i] = Type();
      }

      // Replacing the old array
      std::swap( v_, tmp );
      deallocate( tmp );
      capacity_ = newCapacity;
   }

   size_ = n;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Extending the size of the vector.
//
// \param n Number of additional vector elements.
// \param preserve \a true if the old values of the vector should be preserved, \a false if not.
// \return void
//
// This function increases the vector size by \a n elements. During this operation, new dynamic
// memory may be allocated in case the capacity of the vector is too small. Therefore this
// function potentially changes all vector elements. In order to preserve the old vector values,
// the \a preserve flag can be set to \a true. However, new vector elements are not initialized!
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline void DynamicVector<Type,TF>::extend( size_t n, bool preserve )
{
   resize( size_+n, preserve );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Setting the minimum capacity of the vector.
//
// \param n The new minimum capacity of the vector.
// \return void
//
// This function increases the capacity of the vector to at least \a n elements. The current
// values of the vector elements are preserved.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline void DynamicVector<Type,TF>::reserve( size_t n )
{
   if( n > capacity_ )
   {
      // Allocating a new array
      const size_t newCapacity( adjustCapacity( n ) );
      Type* BLAZE_RESTRICT tmp = allocate<Type>( newCapacity );

      // Initializing the new array
      std::copy( v_, v_+size_, tmp );

      if( IsBuiltin<Type>::value ) {
         for( size_t i=size_; i<newCapacity; ++i )
            tmp[i] = Type();
      }

      // Replacing the old array
      std::swap( tmp, v_ );
      deallocate( tmp );
      capacity_ = newCapacity;
   }
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
        , bool TF >      // Transpose flag
#ifndef WIN32
inline typename DynamicVector<Type,TF>::LengthType DynamicVector<Type,TF>::length() const
#else
inline typename CMathTrait<Type>::Type DynamicVector<Type,TF>::length() const
#endif
{
   BLAZE_CONSTRAINT_MUST_BE_BUILTIN_TYPE( Type );

   LengthType sum( 0 );
   for( size_t i=0UL; i<size_; ++i )
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
        , bool TF >      // Transpose flag
inline const Type DynamicVector<Type,TF>::sqrLength() const
{
   BLAZE_CONSTRAINT_MUST_BE_BUILTIN_TYPE( Type );

   Type sum( 0 );
   for( size_t i=0UL; i<size_; ++i )
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
        , bool TF >      // Transpose flag
inline DynamicVector<Type,TF>& DynamicVector<Type,TF>::normalize()
{
   BLAZE_CONSTRAINT_MUST_BE_FLOATING_POINT_TYPE( Type );

   const Type len( length() );

   if( len == Type(0) )
      return *this;

   const Type ilen( Type(1) / len );

   for( size_t i=0; i<size_; ++i )
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
        , bool TF >      // Transpose flag
inline const DynamicVector<Type,TF> DynamicVector<Type,TF>::getNormalized() const
{
   BLAZE_CONSTRAINT_MUST_BE_FLOATING_POINT_TYPE( Type );

   const Type len( length() );

   if( len == Type(0) )
      return *this;

   const Type ilen( Type(1) / len );
   DynamicVector tmp( size_ );

   for( size_t i=0UL; i<size_; ++i )
      tmp[i] = v_[i] * ilen;

   return tmp;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Scaling of the vector by the scalar value \a scalar (\f$ \vec{a}=\vec{b}*s \f$).
//
// \param scalar The scalar value for the vector scaling.
// \return Reference to the vector.
*/
template< typename Type     // Data type of the vector
        , bool TF >         // Transpose flag
template< typename Other >  // Data type of the scalar value
inline DynamicVector<Type,TF>& DynamicVector<Type,TF>::scale( Other scalar )
{
   for( size_t i=0UL; i<size_; ++i )
      v_[i] *= scalar;
   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Swapping the contents of two vectors.
//
// \param v The vector to be swapped.
// \return void
// \exception no-throw guarantee.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline void DynamicVector<Type,TF>::swap( DynamicVector& v ) /* throw() */
{
   std::swap( size_, v.size_ );
   std::swap( capacity_, v.capacity_ );
   std::swap( v_, v.v_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Adjusting the new capacity of the vector according to its data type \a Type.
//
// \param minCapacity The minimum necessary capacity.
// \return The new capacity.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline size_t DynamicVector<Type,TF>::adjustCapacity( size_t minCapacity ) const
{
   if( IsBuiltin<Type>::value )
      return minCapacity + ( IT::size - ( minCapacity % IT::size ) ) % IT::size;
   else return minCapacity;
}
//*************************************************************************************************




//=================================================================================================
//
//  READ/WRITE FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Reading the vector from a file.
//
// \param file The name of the input file.
// \return void
// \exception std::runtime_error Input error.
//
// This function reads a vector from the specified file \a file. The file has to contain the
// vector data in the following format:

   \code
   #size
   v[0]
   v[1]
   v[2]
   ...
   \endcode

// where \f$ v[i], i \in [0..N-1], \f$ specifies the vector elements. In case the output file
// could not be opened or an input error occured, a \a std::runtime_error is thrown.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
void DynamicVector<Type,TF>::read( const char* file )
{
   std::ifstream in( file, std::ifstream::in );
   if( !in.is_open() ) {
      throw std::runtime_error( "File could not be opened!" );
   }

   size_t vsize(0UL);
   if( !(in >> vsize) || vsize == 0UL ) {
      throw std::runtime_error( "Vector size could not be extracted!" );
   }

   DynamicVector tmp( vsize );

   for( size_t i=0UL; i<vsize; ++i ) {
      if( !(in >> tmp.v_[i]) ) {
         throw std::runtime_error( "Error during vector extraction!" );
      }
   }

   swap( tmp );

   in.close();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Writing the vector to a file.
//
// \param file The name of the output file.
// \param prec The number of non-zero digits displayed in the output file.
// \return void
// \exception std::runtime_error Output error.
//
// This function writes the vector to the specified file \a file using the following format:

   \code
   #size
   v[0]
   v[1]
   v[2]
   ...
   \endcode

// where \f$ v[i], i \in [0..N-1], \f$ specifies the vector elements. In case the output file
// could not be opened, a \a std::runtime_error is thrown.
//
// \b Note: All previous data is replaced by the vector data!
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
void DynamicVector<Type,TF>::write( const char* file, std::streamsize prec ) const
{
   std::ofstream out( file, std::ofstream::out | std::ostream::trunc );
   if( !out.is_open() ) {
      throw std::runtime_error( "File could not be opened!" );
   }

   out << size_ << "\n";

   out.precision( prec );
   for( size_t i=0UL; i<size_; ++i )
      out << v_[i] << "\n";

   out.close();
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
        , bool TF >         // Transpose flag
template< typename Other >  // Data type of the foreign expression
inline bool DynamicVector<Type,TF>::isAliased( const Other* alias ) const
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
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline typename DynamicVector<Type,TF>::IntrinsicType
   DynamicVector<Type,TF>::get( size_t index ) const
{
   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( index            <  size_    , "Invalid vector access index" );
   BLAZE_INTERNAL_ASSERT( index + IT::size <= capacity_, "Invalid vector access index" );
   BLAZE_INTERNAL_ASSERT( index % IT::size == 0UL      , "Invalid vector access index" );

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
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename DisableIf< typename DynamicVector<Type,TF>::BLAZE_TEMPLATE VectorizedAssign<VT> >::Type
   DynamicVector<Type,TF>::assign( const DenseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( size_ == (~rhs).size(), "Invalid vector sizes" );

   const size_t end( (~rhs).size() & size_t(-2) );
   for( size_t i=0UL; i<end; i+=2UL ) {
      v_[i    ] = (~rhs)[i    ];
      v_[i+1UL] = (~rhs)[i+1UL];
   }
   if( end < (~rhs).size() )
      v_[end] = (~rhs)[end];
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
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< typename DynamicVector<Type,TF>::BLAZE_TEMPLATE VectorizedAssign<VT> >::Type
   DynamicVector<Type,TF>::assign( const DenseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( size_ == (~rhs).size(), "Invalid vector sizes" );

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   if( size_ > ( cacheSize/( sizeof(Type) * 3UL ) ) && !(~rhs).isAliased( this ) )
   {
      for( size_t i=0UL; i<size_; i+=IT::size ) {
         stream( v_+i, (~rhs).get(i) );
      }
   }
   else
   {
      BLAZE_INTERNAL_ASSERT( ( size_ - ( size_ % (IT::size*4UL) ) ) == ( size_ & size_t(-IT::size*4) ), "Invalid end calculation" );
      const size_t end( size_ & size_t(-IT::size*4) );

      for( size_t i=0UL; i<end; i+=IT::size*4UL ) {
         store( v_+i             , (~rhs).get(i             ) );
         store( v_+i+IT::size    , (~rhs).get(i+IT::size    ) );
         store( v_+i+IT::size*2UL, (~rhs).get(i+IT::size*2UL) );
         store( v_+i+IT::size*3UL, (~rhs).get(i+IT::size*3UL) );
      }
      for( size_t i=end; i<size_; i+=IT::size ) {
         store( v_+i, (~rhs).get(i) );
      }
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
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side sparse vector
inline void DynamicVector<Type,TF>::assign( const SparseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( size_ == (~rhs).size(), "Invalid vector sizes" );

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
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename DisableIf< typename DynamicVector<Type,TF>::BLAZE_TEMPLATE VectorizedAddAssign<VT> >::Type
   DynamicVector<Type,TF>::addAssign( const DenseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( size_ == (~rhs).size(), "Invalid vector sizes" );

   const size_t end( (~rhs).size() & size_t(-2) );
   for( size_t i=0UL; i<end; i+=2UL ) {
      v_[i    ] += (~rhs)[i    ];
      v_[i+1UL] += (~rhs)[i+1UL];
   }
   if( end < (~rhs).size() )
      v_[end] += (~rhs)[end];
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
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< typename DynamicVector<Type,TF>::BLAZE_TEMPLATE VectorizedAddAssign<VT> >::Type
   DynamicVector<Type,TF>::addAssign( const DenseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( size_ == (~rhs).size(), "Invalid vector sizes" );

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( ( size_ - ( size_ % (IT::size*4UL) ) ) == ( size_ & size_t(-IT::size*4) ), "Invalid end calculation" );
   const size_t end( size_ & size_t(-IT::size*4) );

   for( size_t i=0UL; i<end; i+=IT::size*4UL ) {
      store( v_+i             , load(v_+i             ) + (~rhs).get(i             ) );
      store( v_+i+IT::size    , load(v_+i+IT::size    ) + (~rhs).get(i+IT::size    ) );
      store( v_+i+IT::size*2UL, load(v_+i+IT::size*2UL) + (~rhs).get(i+IT::size*2UL) );
      store( v_+i+IT::size*3UL, load(v_+i+IT::size*3UL) + (~rhs).get(i+IT::size*3UL) );
   }
   for( size_t i=end; i<size_; i+=IT::size ) {
      store( v_+i, load(v_+i) + (~rhs).get(i) );
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
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side sparse vector
inline void DynamicVector<Type,TF>::addAssign( const SparseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( size_ == (~rhs).size(), "Invalid vector sizes" );

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
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename DisableIf< typename DynamicVector<Type,TF>::BLAZE_TEMPLATE VectorizedSubAssign<VT> >::Type
   DynamicVector<Type,TF>::subAssign( const DenseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( size_ == (~rhs).size(), "Invalid vector sizes" );

   const size_t end( (~rhs).size() & size_t(-2) );
   for( size_t i=0UL; i<end; i+=2UL ) {
      v_[i    ] -= (~rhs)[i    ];
      v_[i+1UL] -= (~rhs)[i+1UL];
   }
   if( end < (~rhs).size() )
      v_[end] -= (~rhs)[end];
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
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< typename DynamicVector<Type,TF>::BLAZE_TEMPLATE VectorizedSubAssign<VT> >::Type
   DynamicVector<Type,TF>::subAssign( const DenseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( size_ == (~rhs).size(), "Invalid vector sizes" );

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( ( size_ - ( size_ % (IT::size*4UL) ) ) == ( size_ & size_t(-IT::size*4) ), "Invalid end calculation" );
   const size_t end( size_ & size_t(-IT::size*4) );

   for( size_t i=0UL; i<end; i+=IT::size*4UL ) {
      store( v_+i             , load(v_+i             ) - (~rhs).get(i             ) );
      store( v_+i+IT::size    , load(v_+i+IT::size    ) - (~rhs).get(i+IT::size    ) );
      store( v_+i+IT::size*2UL, load(v_+i+IT::size*2UL) - (~rhs).get(i+IT::size*2UL) );
      store( v_+i+IT::size*3UL, load(v_+i+IT::size*3UL) - (~rhs).get(i+IT::size*3UL) );
   }
   for( size_t i=end; i<size_; i+=IT::size ) {
      store( v_+i, load(v_+i) - (~rhs).get(i) );
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
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side sparse vector
inline void DynamicVector<Type,TF>::subAssign( const SparseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( size_ == (~rhs).size(), "Invalid vector sizes" );

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
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename DisableIf< typename DynamicVector<Type,TF>::BLAZE_TEMPLATE VectorizedMultAssign<VT> >::Type
   DynamicVector<Type,TF>::multAssign( const DenseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( size_ == (~rhs).size(), "Invalid vector sizes" );

   const size_t end( (~rhs).size() & size_t(-2) );
   for( size_t i=0UL; i<end; i+=2UL ) {
      v_[i    ] *= (~rhs)[i    ];
      v_[i+1UL] *= (~rhs)[i+1UL];
   }
   if( end < (~rhs).size() )
      v_[end] *= (~rhs)[end];
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
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< typename DynamicVector<Type,TF>::BLAZE_TEMPLATE VectorizedMultAssign<VT> >::Type
   DynamicVector<Type,TF>::multAssign( const DenseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( size_ == (~rhs).size(), "Invalid vector sizes" );

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( ( size_ - ( size_ % (IT::size*4UL) ) ) == ( size_ & size_t(-IT::size*4) ), "Invalid end calculation" );
   const size_t end( size_ & size_t(-IT::size*4) );

   for( size_t i=0UL; i<end; i+=IT::size*4UL ) {
      store( v_+i             , load(v_+i             ) * (~rhs).get(i             ) );
      store( v_+i+IT::size    , load(v_+i+IT::size    ) * (~rhs).get(i+IT::size    ) );
      store( v_+i+IT::size*2UL, load(v_+i+IT::size*2UL) * (~rhs).get(i+IT::size*2UL) );
      store( v_+i+IT::size*3UL, load(v_+i+IT::size*3UL) * (~rhs).get(i+IT::size*3UL) );
   }
   for( size_t i=end; i<size_; i+=IT::size ) {
      store( v_+i, load(v_+i) * (~rhs).get(i) );
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
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side sparse vector
inline void DynamicVector<Type,TF>::multAssign( const SparseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( size_ == (~rhs).size(), "Invalid vector sizes" );

   typedef typename VT::ConstIterator  ConstIterator;

   const DynamicVector tmp( *this );
   const ConstIterator end( (~rhs).end() );

   reset();

   for( ConstIterator element=(~rhs).begin(); element!=end; ++element )
      v_[element->index()] = tmp[element->index()] * element->value();
}
//*************************************************************************************************




//=================================================================================================
//
//  GLOBAL OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*!\name DynamicVector operators */
//@{
template< typename Type, bool TF >
inline void reset( DynamicVector<Type,TF>& v );

template< typename Type, bool TF >
inline void clear( DynamicVector<Type,TF>& v );

template< typename Type, bool TF >
inline bool isnan( const DynamicVector<Type,TF>& v );

template< typename Type, bool TF >
inline bool isDefault( const DynamicVector<Type,TF>& v );

template< typename Type, bool TF >
inline const DVecDVecMultExpr<DynamicVector<Type,TF>,DynamicVector<Type,TF>,TF>
   sq( const DynamicVector<Type,TF>& v );

template< typename Type, bool TF >
inline void swap( DynamicVector<Type,TF>& a, DynamicVector<Type,TF>& b ) /* throw() */;
//@}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Resetting the given dynamic vector.
// \ingroup dense_vector_N
//
// \param v The dynamic vector to be resetted.
// \return void
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline void reset( DynamicVector<Type,TF>& v )
{
   v.reset();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Clearing the given dynamic vector.
// \ingroup dense_vector_N
//
// \param v The dynamic vector to be cleared.
// \return void
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline void clear( DynamicVector<Type,TF>& v )
{
   v.clear();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checks the given dynamic vector for not-a-number elements.
// \ingroup dense_vector_N
//
// \param v The vector to be checked for not-a-number elements.
// \return \a true if at least one element of the vector is not-a-number, \a false otherwise.
//
// This function checks the N-dimensional dynamic vector for not-a-number (NaN) elements. If at
// least one element of the vector is not-a-number, the function returns \a true, otherwise it
// returns \a false.

   \code
   blaze::VecN a;
   // ... Resizing and initialization
   if( isnan( a ) ) { ... }
   \endcode
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline bool isnan( const DynamicVector<Type,TF>& v )
{
   for( size_t i=0UL; i<v.size(); ++i ) {
      if( isnan( v[i] ) ) return true;
   }
   return false;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns whether the given dynamic vector is in default state.
// \ingroup dense_vector_N
//
// \param v The dynamic vector to be tested for its default state.
// \return \a true in case the given vector is component-wise zero, \a false otherwise.
//
// This function checks whether the N-dimensional vector is in default state. For instance,
// in case the vector is instantiated for a built-in integral or floating point data type,
// the function returns \a true in case all vector elements are 0 and \a false in case any
// vector element is not 0. The following example demonstrates the use of the \a isDefault
// function:

   \code
   blaze::DynamicVector<int> a;
   // ... Resizing and initialization
   if( isDefault( a ) ) { ... }
   \endcode
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline bool isDefault( const DynamicVector<Type,TF>& v )
{
   for( size_t i=0UL; i<v.size(); ++i )
      if( !isDefault( v[i] ) ) return false;
   return true;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Squaring the given dynamic vector.
// \ingroup dense_vector_N
//
// \param v The dynamic vector to be squared.
// \return The result of the square operation.
//
// This function calculates the component product of the given dynamic vector. It has the same
// effect as multiplying the vector with itself (\f$ v * v \f$).
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline const DVecDVecMultExpr<DynamicVector<Type,TF>,DynamicVector<Type,TF>,TF>
   sq( const DynamicVector<Type,TF>& v )
{
   return v * v;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Swapping the contents of two vectors.
// \ingroup dense_vector_N
//
// \param a The first vector to be swapped.
// \param b The second vector to be swapped.
// \return void
// \exception no-throw guarantee.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline void swap( DynamicVector<Type,TF>& a, DynamicVector<Type,TF>& b ) /* throw() */
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
template< typename T, bool TF >
struct IsResizable< DynamicVector<T,TF> > : public TrueType
{
   enum { value = 1 };
   typedef TrueType  Type;
};

template< typename T, bool TF >
struct IsResizable< const DynamicVector<T,TF> > : public TrueType
{
   enum { value = 1 };
   typedef TrueType  Type;
};

template< typename T, bool TF >
struct IsResizable< volatile DynamicVector<T,TF> > : public TrueType
{
   enum { value = 1 };
   typedef TrueType  Type;
};

template< typename T, bool TF >
struct IsResizable< const volatile DynamicVector<T,TF> > : public TrueType
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
template< typename T1, bool TF, typename T2 >
struct MathTrait< DynamicVector<T1,TF>, T2 >
{
   typedef INVALID_TYPE                                              HighType;
   typedef INVALID_TYPE                                              LowType;
   typedef INVALID_TYPE                                              AddType;
   typedef INVALID_TYPE                                              SubType;
   typedef DynamicVector< typename MathTrait<T1,T2>::MultType, TF >  MultType;
   typedef DynamicVector< typename MathTrait<T1,T2>::DivType , TF >  DivType;
   BLAZE_CONSTRAINT_MUST_BE_NUMERIC_TYPE( T2 );
};

template< typename T1, typename T2, bool TF >
struct MathTrait< T1, DynamicVector<T2,TF> >
{
   typedef INVALID_TYPE                                              HighType;
   typedef INVALID_TYPE                                              LowType;
   typedef INVALID_TYPE                                              AddType;
   typedef INVALID_TYPE                                              SubType;
   typedef DynamicVector< typename MathTrait<T1,T2>::MultType, TF >  MultType;
   typedef INVALID_TYPE                                              DivType;
   BLAZE_CONSTRAINT_MUST_BE_NUMERIC_TYPE( T1 );
};

template< typename T1, bool TF, typename T2, size_t N >
struct MathTrait< DynamicVector<T1,TF>, StaticVector<T2,N,TF> >
{
   typedef INVALID_TYPE                                                HighType;
   typedef INVALID_TYPE                                                LowType;
   typedef StaticVector< typename MathTrait<T1,T2>::AddType , N, TF >  AddType;
   typedef StaticVector< typename MathTrait<T1,T2>::SubType , N, TF >  SubType;
   typedef StaticVector< typename MathTrait<T1,T2>::MultType, N, TF >  MultType;
   typedef INVALID_TYPE                                                DivType;
};

template< typename T1, typename T2, size_t N >
struct MathTrait< DynamicVector<T1,true>, StaticVector<T2,N,false> >
{
   typedef INVALID_TYPE                         HighType;
   typedef INVALID_TYPE                         LowType;
   typedef INVALID_TYPE                         AddType;
   typedef INVALID_TYPE                         SubType;
   typedef typename MathTrait<T1,T2>::MultType  MultType;
   typedef INVALID_TYPE                         DivType;
};

template< typename T1, typename T2, size_t N >
struct MathTrait< DynamicVector<T1,false>, StaticVector<T2,N,true> >
{
   typedef INVALID_TYPE                                                 HighType;
   typedef INVALID_TYPE                                                 LowType;
   typedef INVALID_TYPE                                                 AddType;
   typedef INVALID_TYPE                                                 SubType;
   typedef DynamicMatrix< typename MathTrait<T1,T2>::MultType, false >  MultType;
   typedef INVALID_TYPE                                                 DivType;
};

template< typename T1, size_t N, bool TF, typename T2 >
struct MathTrait< StaticVector<T1,N,TF>, DynamicVector<T2,TF> >
{
   typedef INVALID_TYPE                                                HighType;
   typedef INVALID_TYPE                                                LowType;
   typedef StaticVector< typename MathTrait<T1,T2>::AddType , N, TF >  AddType;
   typedef StaticVector< typename MathTrait<T1,T2>::SubType , N, TF >  SubType;
   typedef StaticVector< typename MathTrait<T1,T2>::MultType, N, TF >  MultType;
   typedef INVALID_TYPE                                                DivType;
};

template< typename T1, size_t N, typename T2 >
struct MathTrait< StaticVector<T1,N,true>, DynamicVector<T2,false> >
{
   typedef INVALID_TYPE                         HighType;
   typedef INVALID_TYPE                         LowType;
   typedef INVALID_TYPE                         AddType;
   typedef INVALID_TYPE                         SubType;
   typedef typename MathTrait<T1,T2>::MultType  MultType;
   typedef INVALID_TYPE                         DivType;
};

template< typename T1, size_t N, typename T2 >
struct MathTrait< StaticVector<T1,N,false>, DynamicVector<T2,true> >
{
   typedef INVALID_TYPE                                                 HighType;
   typedef INVALID_TYPE                                                 LowType;
   typedef INVALID_TYPE                                                 AddType;
   typedef INVALID_TYPE                                                 SubType;
   typedef DynamicMatrix< typename MathTrait<T1,T2>::MultType, false >  MultType;
   typedef INVALID_TYPE                                                 DivType;
};

template< typename T1, bool TF, typename T2 >
struct MathTrait< DynamicVector<T1,TF>, DynamicVector<T2,TF> >
{
   typedef DynamicVector< typename MathTrait<T1,T2>::HighType, TF >  HighType;
   typedef DynamicVector< typename MathTrait<T1,T2>::LowType , TF >  LowType;
   typedef DynamicVector< typename MathTrait<T1,T2>::AddType , TF >  AddType;
   typedef DynamicVector< typename MathTrait<T1,T2>::SubType , TF >  SubType;
   typedef DynamicVector< typename MathTrait<T1,T2>::MultType, TF >  MultType;
   typedef INVALID_TYPE                                              DivType;
};

template< typename T1, typename T2 >
struct MathTrait< DynamicVector<T1,true>, DynamicVector<T2,false> >
{
   typedef INVALID_TYPE                         HighType;
   typedef INVALID_TYPE                         LowType;
   typedef INVALID_TYPE                         AddType;
   typedef INVALID_TYPE                         SubType;
   typedef typename MathTrait<T1,T2>::MultType  MultType;
   typedef INVALID_TYPE                         DivType;
};

template< typename T1, typename T2 >
struct MathTrait< DynamicVector<T1,false>, DynamicVector<T2,true> >
{
   typedef INVALID_TYPE                                                 HighType;
   typedef INVALID_TYPE                                                 LowType;
   typedef INVALID_TYPE                                                 AddType;
   typedef INVALID_TYPE                                                 SubType;
   typedef DynamicMatrix< typename MathTrait<T1,T2>::MultType, false >  MultType;
   typedef INVALID_TYPE                                                 DivType;
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  TYPE DEFINITIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief N-dimensional single-precision vector.
// \ingroup dynamic_vector
*/
typedef DynamicVector<float,false>  VecNf;
//*************************************************************************************************


//*************************************************************************************************
/*!\brief N-dimensional double-precision vector.
// \ingroup dynamic_vector
*/
typedef DynamicVector<double,false>  VecNd;
//*************************************************************************************************


//*************************************************************************************************
/*!\brief N-dimensional vector with system-specific precision.
// \ingroup dynamic_vector
*/
typedef DynamicVector<real,false>  VecN;
//*************************************************************************************************

} // namespace blaze

#endif
