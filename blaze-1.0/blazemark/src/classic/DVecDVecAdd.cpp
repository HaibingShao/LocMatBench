//=================================================================================================
/*!
//  \file src/classic/DVecDVecAdd.cpp
//  \brief Source file for the classic dense vector/dense vector addition kernel
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


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <iostream>
#include <blaze/util/Random.h>
#include <blaze/util/Timing.h>
#include <blazemark/classic/DVecDVecAdd.h>
#include <blazemark/classic/Vector.h>
#include <blazemark/system/Config.h>
#include <blazemark/system/Precision.h>


namespace blazemark {

namespace classic {

//=================================================================================================
//
//  KERNEL FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Classic dense vector/dense vector addition kernel.
//
// \param N The size of the vectors for the addition.
// \param steps The number of iteration steps to perform.
// \return Minimum runtime of the kernel function.
//
// This kernel function implements the dense vector/dense vector addition by means of
// classic C++ operator overloading.
*/
double dvecdvecadd( size_t N, size_t steps )
{
   using ::blazemark::real;

   ::blaze::setSeed( seed );

   ::blazemark::classic::Vector<real> a( N ), b( N ), c( N );
   ::blaze::timing::WcTimer timer;

   for( size_t i=0UL; i<N; ++i ) {
      a[i] = ::blaze::rand<real>();
      b[i] = ::blaze::rand<real>();
   }

   c = a + b;

   for( size_t rep=0UL; rep<reps; ++rep )
   {
      timer.start();
      for( size_t step=0UL; step<steps; ++step ) {
         c = a + b;
      }
      timer.end();

      if( c.size() != N )
         std::cerr << " Line " << __LINE__ << ": ERROR detected!!!\n";

      if( timer.last() > maxtime )
         break;
   }

   const double minTime( timer.min()     );
   const double avgTime( timer.average() );

   if( minTime * ( 1.0 + deviation*0.01 ) < avgTime )
      std::cerr << " Classic kernel 'dvecdvecadd': Time deviation too large!!!\n";

   return minTime;
}
//*************************************************************************************************

} // namespace classic

} // namespace blazemark