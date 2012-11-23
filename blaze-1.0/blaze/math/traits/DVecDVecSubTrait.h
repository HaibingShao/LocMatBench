//=================================================================================================
/*!
//  \file blaze/math/traits/DVecDVecSubTrait.h
//  \brief Header file for the DVecDVecSubTrait class template
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

#ifndef _BLAZE_MATH_TRAITS_DVECDVECSUBTRAIT_H_
#define _BLAZE_MATH_TRAITS_DVECDVECSUBTRAIT_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <blaze/math/expressions/Forward.h>
#include <blaze/math/typetraits/IsDenseVector.h>
#include <blaze/math/typetraits/IsTransposeVector.h>
#include <blaze/util/InvalidType.h>
#include <blaze/util/SelectType.h>


namespace blaze {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Evaluation of the expression type of a dense vector/dense vector subtraction.
// \ingroup math_traits
//
// Via this type trait it is possible to evaluate the resulting expression type of a dense
// vector/dense vector subtraction. Given the two non-transpose dense vector types \a VT1
// and \a VT2, the nested type \a Type corresponds to the resulting expression type. In case
// either \a VT1 or \a VT2 is not a non-transpose dense vector type, the resulting \a Type
// is set to \a INVALID_TYPE.
*/
template< typename VT1    // Type of the left-hand side non-transpose dense vector
        , typename VT2 >  // Type of the right-hand side non-transpose dense vector
struct DVecDVecSubTrait
{
 public:
   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   typedef typename SelectType< IsDenseVector<VT1>::value && !IsTransposeVector<VT1>::value &&
                                IsDenseVector<VT2>::value && !IsTransposeVector<VT2>::value
                              , DVecDVecSubExpr<VT1,VT2,false>, INVALID_TYPE >::Type  Type;
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************

} // namespace blaze

#endif
