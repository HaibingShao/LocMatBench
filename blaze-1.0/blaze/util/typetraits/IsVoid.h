//=================================================================================================
/*!
//  \file blaze/util/typetraits/IsVoid.h
//  \brief Header file for the IsVoid type trait
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

#ifndef _BLAZE_UTIL_TYPETRAITS_ISVOID_H_
#define _BLAZE_UTIL_TYPETRAITS_ISVOID_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <boost/type_traits/is_void.hpp>
#include <blaze/util/FalseType.h>
#include <blaze/util/SelectType.h>
#include <blaze/util/TrueType.h>


namespace blaze {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Auxiliary helper struct for the IsVoid type trait.
// \ingroup type_traits
*/
template< typename T >
struct IsVoidHelper
{
   //**********************************************************************************************
   enum { value = boost::is_void<T>::value };
   typedef typename SelectType<value,TrueType,FalseType>::Type  Type;
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Compile time check for the \a void data type.
// \ingroup type_traits
//
// This type trait tests whether or not the given template parameter is of type \a void
// (ignoring the cv-qualifiers). In case the type is of type \a void, the \a value member
// enumeration is set to 1, the nested type definition \a Type is \a TrueType, and the
// class derives from \a TrueType. Otherwise \a value is set to 0, \a Type is \a FalseType,
// and the class derives from \a FalseType.

   \code
   blaze::IsVoid<void>::value          // Evaluates to 1
   blaze::IsVoid<const void>::Type     // Results in TrueType
   blaze::IsVoid<const volatile void>  // Is derived from TrueType
   blaze::IsVoid<int>::value           // Evaluates to 0
   blaze::IsVoid<const char>::Type     // Results in FalseType
   blaze::IsVoid<volatile float>       // Is derived from FalseType
   \endcode
*/
template< typename T >
struct IsVoid : public IsVoidHelper<T>::Type
{
 public:
   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   enum { value = IsVoidHelper<T>::value };
   typedef typename IsVoidHelper<T>::Type  Type;
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************

} // namespace blaze

#endif
