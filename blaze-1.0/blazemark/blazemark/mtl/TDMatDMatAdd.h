//=================================================================================================
/*!
//  \file blazemark/mtl/TDMatDMatAdd.h
//  \brief Header file for the MTL transpose dense matrix/dense matrix addition kernel
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

#ifndef _BLAZEMARK_MTL_TDMATDMATADD_H_
#define _BLAZEMARK_MTL_TDMATDMATADD_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <blazemark/system/Types.h>


namespace blazemark {

namespace mtl {

//=================================================================================================
//
//  KERNEL FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\name MTL kernel functions */
//@{
double tdmatdmatadd( size_t N, size_t steps );
//@}
//*************************************************************************************************

} // namespace mtl

} // namespace blazemark

#endif
