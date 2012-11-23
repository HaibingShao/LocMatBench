//=================================================================================================
/*!
//  \file blaze/math/traits/DivExprTrait.h
//  \brief Header file for the DivExprTrait class template
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

#ifndef _BLAZE_MATH_TRAITS_DIVEXPRTRAIT_H_
#define _BLAZE_MATH_TRAITS_DIVEXPRTRAIT_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <blaze/math/MathTrait.h>
#include <blaze/math/traits/DMatScalarDivTrait.h>
#include <blaze/math/traits/DVecScalarDivTrait.h>
#include <blaze/math/traits/SMatScalarDivTrait.h>
#include <blaze/math/traits/SVecScalarDivTrait.h>
#include <blaze/math/traits/TDMatScalarDivTrait.h>
#include <blaze/math/traits/TDVecScalarDivTrait.h>
#include <blaze/math/traits/TSMatScalarDivTrait.h>
#include <blaze/math/traits/TSVecScalarDivTrait.h>
#include <blaze/math/typetraits/IsDenseMatrix.h>
#include <blaze/math/typetraits/IsMatrix.h>
#include <blaze/math/typetraits/IsRowMajorMatrix.h>
#include <blaze/math/typetraits/IsTransposeVector.h>
#include <blaze/math/typetraits/IsVector.h>
#include <blaze/util/InvalidType.h>
#include <blaze/util/mpl/If.h>
#include <blaze/util/typetraits/IsNumeric.h>


namespace blaze {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Evaluation of the resulting expression type of a division.
// \ingroup math_traits
//
// Via this type trait it is possible to evaluate the return type of a division expression
// between scalars, vectors, and matrices. Given the two types \a T1 and \a T2, where \a T1 must
// be either a scalar, vector, or matrix type and \a T2 which must be a scalar type, the nested
// type \a Type corresponds to the resulting return type. In case \a T1 or \a T2 don't fit or if
// the two types cannot be divided, the resulting data type \a Type is set to \a INVALID_TYPE.
*/
template< typename T1    // Type of the left-hand side division operand
        , typename T2 >  // Type of the right-hand side division operand
struct DivExprTrait
{
 private:
   //**struct ScalarAdd****************************************************************************
   /*! \cond BLAZE_INTERNAL */
   template< typename ST1, typename ST2 >
   struct ScalarDiv { typedef typename MathTrait<ST1,ST2>::DivType  Type; };
   /*! \endcond */
   //**********************************************************************************************

   //**struct Failure******************************************************************************
   /*! \cond BLAZE_INTERNAL */
   struct Failure { typedef INVALID_TYPE  Type; };
   /*! \endcond */
   //**********************************************************************************************

 public:
   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   typedef typename If< IsMatrix<T1>
                      , typename If< IsDenseMatrix<T1>
                                   , typename If< IsRowMajorMatrix<T1>
                                                , typename If< IsNumeric<T2>
                                                             , DMatScalarDivTrait<T1,T2>
                                                             , Failure
                                                             >::Type
                                                , typename If< IsNumeric<T2>
                                                             , TDMatScalarDivTrait<T1,T2>
                                                             , Failure
                                                             >::Type
                                                >::Type
                                   , typename If< IsRowMajorMatrix<T1>
                                                , typename If< IsNumeric<T2>
                                                             , SMatScalarDivTrait<T1,T2>
                                                             , Failure
                                                             >::Type
                                                , typename If< IsNumeric<T2>
                                                             , TSMatScalarDivTrait<T1,T2>
                                                             , Failure
                                                             >::Type
                                                >::Type
                                   >::Type
                      , typename If< IsVector<T1>
                                   , typename If< IsDenseVector<T1>
                                                , typename If< IsTransposeVector<T1>
                                                             , typename If< IsNumeric<T2>
                                                                          , TDVecScalarDivTrait<T1,T2>
                                                                          , Failure
                                                                          >::Type
                                                             , typename If< IsNumeric<T2>
                                                                          , DVecScalarDivTrait<T1,T2>
                                                                          , Failure
                                                                          >::Type
                                                             >::Type
                                                , typename If< IsTransposeVector<T1>
                                                             , typename If< IsNumeric<T2>
                                                                          , TSVecScalarDivTrait<T1,T2>
                                                                          , Failure
                                                                          >::Type
                                                             , typename If< IsNumeric<T2>
                                                                          , SVecScalarDivTrait<T1,T2>
                                                                          , Failure
                                                                          >::Type
                                                             >::Type
                                                >::Type
                                   , typename If< IsNumeric<T1>
                                                , typename If< IsNumeric<T2>
                                                             , ScalarDiv<T1,T2>
                                                             , Failure
                                                             >::Type
                                                , Failure
                                                >::Type
                                   >::Type
                      >::Type::Type  Type;
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************

} // namespace blaze

#endif
