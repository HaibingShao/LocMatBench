//=================================================================================================
/*!
//  \file blaze/util/constraints/DerivedFrom.h
//  \brief Constraint on the inheritance relationship of a data type
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

#ifndef _BLAZE_UTIL_CONSTRAINTS_DERIVEDFROM_H_
#define _BLAZE_UTIL_CONSTRAINTS_DERIVEDFROM_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <blaze/util/constraints/ConstraintTest.h>
#include <blaze/util/Suffix.h>
#include <blaze/util/typetraits/IsBaseOf.h>


namespace blaze {

//=================================================================================================
//
//  MUST_BE_DERIVED_FROM CONSTRAINT
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
template< bool > struct CONSTRAINT_MUST_BE_DERIVED_FROM_FAILED;
template<> struct CONSTRAINT_MUST_BE_DERIVED_FROM_FAILED<true> { enum { value = 1 }; };
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constraint on the inheritance relationship of a data type.
// \ingroup constraints
//
// In case \a D is not derived from \a B, a compilation error is created.
*/
#define BLAZE_CONSTRAINT_MUST_BE_DERIVED_FROM(D,B) \
   typedef \
      ::blaze::CONSTRAINT_TEST< \
         ::blaze::CONSTRAINT_MUST_BE_DERIVED_FROM_FAILED< ::blaze::IsBaseOf<B,D>::value >::value > \
      BLAZE_JOIN( CONSTRAINT_MUST_BE_DERIVED_FROM_TYPEDEF, __LINE__ )
//*************************************************************************************************




//=================================================================================================
//
//  MUST_NOT_BE_DERIVED_FROM CONSTRAINT
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
template< bool > struct CONSTRAINT_MUST_NOT_BE_DERIVED_FROM_FAILED;
template<> struct CONSTRAINT_MUST_NOT_BE_DERIVED_FROM_FAILED<true> { enum { value = 1 }; };
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constraint on the inheritance relationship of a data type.
// \ingroup constraints
//
// In case \a D is derived from \a B or in case \a D is the same type as \a B, a compilation
// error is created.
*/
#define BLAZE_CONSTRAINT_MUST_NOT_BE_DERIVED_FROM(D,B) \
   typedef \
      ::blaze::CONSTRAINT_TEST< \
         ::blaze::CONSTRAINT_MUST_NOT_BE_DERIVED_FROM_FAILED< !::blaze::IsBaseOf<B,D>::value >::value > \
      BLAZE_JOIN( CONSTRAINT_MUST_NOT_BE_DERIVED_FROM_TYPEDEF, __LINE__ )
//*************************************************************************************************




//=================================================================================================
//
//  MUST_BE_STRICTLY_DERIVED_FROM CONSTRAINT
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
template< bool > struct CONSTRAINT_MUST_BE_STRICTLY_DERIVED_FROM_FAILED;
template<> struct CONSTRAINT_MUST_BE_STRICTLY_DERIVED_FROM_FAILED<true> { enum { value = 1 }; };
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constraint on the inheritance relationship of a data type.
// \ingroup constraints
//
// In case \a D is not derived from \a B, a compilation error is created. In contrast to the
// BLAZE_CONSTRAINT_MUST_BE_DERIVED_FROM constraint, a compilation error is also created in
// case \a D and \a B are the same type.
*/
#define BLAZE_CONSTRAINT_MUST_BE_STRICTLY_DERIVED_FROM(D,B) \
   typedef \
      ::blaze::CONSTRAINT_TEST< \
         ::blaze::CONSTRAINT_MUST_BE_STRICTLY_DERIVED_FROM_FAILED< ::blaze::IsBaseOf<B,D>::value && \
                                                                  !::blaze::IsBaseOf<D,B>::value >::value > \
      BLAZE_JOIN( CONSTRAINT_MUST_BE_STRICTLY_DERIVED_FROM_TYPEDEF, __LINE__ )
//*************************************************************************************************




//=================================================================================================
//
//  MUST_BE_STRICTLY_DERIVED_FROM CONSTRAINT
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
template< bool > struct CONSTRAINT_MUST_NOT_BE_STRICTLY_DERIVED_FROM_FAILED;
template<> struct CONSTRAINT_MUST_NOT_BE_STRICTLY_DERIVED_FROM_FAILED<true> { enum { value = 1 }; };
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constraint on the inheritance relationship of a data type.
// \ingroup constraints
//
// In case \a D is derived from \a B, a compilation error is created. In contrast to the
// BLAZE_CONSTRAINT_MUST_NOT_BE_DERIVED_FROM constraint, no compilation error is created
// in case \a D and \a B are the same type.
*/
#define BLAZE_CONSTRAINT_MUST_NOT_BE_STRICTLY_DERIVED_FROM(D,B) \
   typedef \
      ::blaze::CONSTRAINT_TEST< \
         ::blaze::CONSTRAINT_MUST_NOT_BE_STRICTLY_DERIVED_FROM_FAILED< !::blaze::IsBaseOf<B,D>::value || \
                                                                        ::blaze::IsBaseOf<D,B>::value >::value > \
      BLAZE_JOIN( CONSTRAINT_MUST_NOT_BE_STRICTLY_DERIVED_FROM_TYPEDEF, __LINE__ )
//*************************************************************************************************

} // namespace blaze

#endif
