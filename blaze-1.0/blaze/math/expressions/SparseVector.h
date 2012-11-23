//=================================================================================================
/*!
//  \file blaze/math/expressions/SparseVector.h
//  \brief Header file for the SparseVector base class
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

#ifndef _BLAZE_MATH_EXPRESSIONS_SPARSEVECTOR_H_
#define _BLAZE_MATH_EXPRESSIONS_SPARSEVECTOR_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <blaze/math/expressions/Vector.h>


namespace blaze {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*!\defgroup sparse_vector Sparse Vectors
// \ingroup vector
*/
/*!\defgroup sparse_vector_expression Expressions
// \ingroup sparse_vector
*/
/*!\brief Base class for sparse vectors.
// \ingroup sparse_vector
//
// The SparseVector class is a base class for all arbitrarily sized (N-dimensional) sparse
// vectors. It provides an abstraction from the actual type of the sparse vector, but enables
// a conversion back to this type via the Vector base class.
*/
template< typename VT  // Type of the sparse vector
        , bool TF >    // Transpose flag
struct SparseVector : public Vector<VT,TF>
{};
//*************************************************************************************************

} // namespace blaze

#endif
