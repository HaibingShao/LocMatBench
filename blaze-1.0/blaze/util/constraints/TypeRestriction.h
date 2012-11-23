//=================================================================================================
/*!
//  \file blaze/util/constraints/TypeRestriction.h
//  \brief Constraint on the data type
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

#ifndef _BLAZE_UTIL_CONSTRAINTS_TYPERESTRICTION_H_
#define _BLAZE_UTIL_CONSTRAINTS_TYPERESTRICTION_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <blaze/util/constraints/ConstraintTest.h>
#include <blaze/util/Suffix.h>
#include <blaze/util/TypeList.h>


namespace blaze {

//=================================================================================================
//
//  SOFT_TYPE_RESTRICTION CONSTRAINT
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Compile time constraint.
// \ingroup constraints
//
// Helper template class for the compile time constraint enforcement. Based on the compile time
// constant expression used for the template instantiation, either the undefined basic template
// or the specialization is selected. If the undefined basic template is selected, a compilation
// error is created.
*/
template< bool > struct CONSTRAINT_SOFT_TYPE_RESTRICTION_FAILED;
template<> struct CONSTRAINT_SOFT_TYPE_RESTRICTION_FAILED<true> { enum { value = 1 }; };
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constraint on the data type.
// \ingroup constraints
//
// In case the given type list \a TYPELIST does not contain a type related to \a T, a compilation
// error is created.
*/
#define BLAZE_CONSTRAINT_SOFT_TYPE_RESTRICTION(T,TYPELIST) \
   typedef \
      ::blaze::CONSTRAINT_TEST< \
         ::blaze::CONSTRAINT_SOFT_TYPE_RESTRICTION_FAILED< ::blaze::ContainsRelated<TYPELIST,T>::value >::value > \
      BLAZE_JOIN( CONSTRAINT_SOFT_TYPE_RESTRICTION_TYPEDEF, __LINE__ )
//*************************************************************************************************




//=================================================================================================
//
//  TYPE_RESTRICTION CONSTRAINT
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Compile time constraint.
// \ingroup constraints
//
// Helper template class for the compile time constraint enforcement. Based on the compile time
// constant expression used for the template instantiation, either the undefined basic template
// or the specialization is selected. If the undefined basic template is selected, a compilation
// error is created.
*/
template< bool > struct CONSTRAINT_TYPE_RESTRICTION_FAILED;
template<> struct CONSTRAINT_TYPE_RESTRICTION_FAILED<true> { enum { value = 1 }; };
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constraint on the data type.
// \ingroup constraints
//
// In case the given type list \a TYPELIST does not contain the data type \a T, a compilation
// error is created.
*/
#define BLAZE_CONSTRAINT_TYPE_RESTRICTION(T,TYPELIST) \
   typedef \
      ::blaze::CONSTRAINT_TEST< \
         ::blaze::CONSTRAINT_TYPE_RESTRICTION_FAILED< ::blaze::Contains<TYPELIST,T>::value >::value > \
      BLAZE_JOIN( CONSTRAINT_TYPE_RESTRICTION_TYPEDEF, __LINE__ )
//*************************************************************************************************

} // namespace blaze

#endif
