//=================================================================================================
/*!
//  \file blaze/math/traits/DMatTSMatAddTrait.h
//  \brief Header file for the DMatTSMatAddTrait class template
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

#ifndef _BLAZE_MATH_TRAITS_DMATTSMATADDTRAIT_H_
#define _BLAZE_MATH_TRAITS_DMATTSMATADDTRAIT_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <blaze/math/expressions/Forward.h>
#include <blaze/math/typetraits/IsColumnMajorMatrix.h>
#include <blaze/math/typetraits/IsDenseMatrix.h>
#include <blaze/math/typetraits/IsRowMajorMatrix.h>
#include <blaze/math/typetraits/IsSparseMatrix.h>
#include <blaze/util/InvalidType.h>
#include <blaze/util/SelectType.h>


namespace blaze {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Evaluation of the expression type of a dense matrix/transpose sparse matrix addition.
// \ingroup math_traits
//
// Via this type trait it is possible to evaluate the resulting expression type of a dense
// matrix/transpose sparse matrix addition. Given the row-major dense matrix type \a MT1 and
// the column-major sparse matrix type \a MT2, the nested type \a Type corresponds to the
// resulting expression type. In case either \a MT1 is not a row-major dense matrix type or
// \a MT2 is not a column-major sparse matrix type, the resulting data type \a Type is set
// to \a INVALID_TYPE.
*/
template< typename MT1    // Type of the left-hand side row-major dense matrix
        , typename MT2 >  // Type of the right-hand side column-major sparse matrix
struct DMatTSMatAddTrait
{
 public:
   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   typedef typename SelectType< IsDenseMatrix<MT1>::value  && IsRowMajorMatrix<MT1>::value &&
                                IsSparseMatrix<MT2>::value && IsColumnMajorMatrix<MT2>::value
                              , DMatTSMatAddExpr<MT1,MT2>, INVALID_TYPE >::Type  Type;
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************

} // namespace blaze

#endif
