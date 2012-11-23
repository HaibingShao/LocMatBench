//=================================================================================================
/*!
//  \file blazemark/blitz/TMat6Mat6Mult.h
//  \brief Header file for the Blitz++ 6D transpose matrix/matrix multiplication kernel
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

#ifndef _BLAZEMARK_BLITZ_TMAT6MAT6MULT_H_
#define _BLAZEMARK_BLITZ_TMAT6MAT6MULT_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <blazemark/system/Types.h>


namespace blazemark {

namespace blitz {

//=================================================================================================
//
//  KERNEL FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\name Blitz++ kernel functions */
//@{
double tmat6mat6mult( size_t N, size_t steps );
//@}
//*************************************************************************************************

} // namespace blitz

} // namespace blazemark

#endif