//=================================================================================================
/*!
//  \file src/clike/DVecDVecSub.cpp
//  \brief Source file for the C-like dense vector/dense vector subtraction kernel
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
#include <blazemark/clike/DVecDVecSub.h>
#include <blazemark/system/Config.h>
#include <blazemark/system/Precision.h>


namespace blazemark {

namespace clike {

//=================================================================================================
//
//  KERNEL FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief C-like dense vector/dense vector subtraction kernel.
//
// \param N The size of the vectors for the subtraction.
// \param steps The number of iteration steps to perform.
// \return Minimum runtime of the kernel function.
//
// This kernel function implements the dense vector/dense vector subtraction by means of a
// C-like implementation.
*/
double dvecdvecsub( size_t N, size_t steps )
{
   using ::blazemark::real;

   ::blaze::setSeed( seed );

   real* a = new real[N];
   real* b = new real[N];
   real* c = new real[N];
   ::blaze::timing::WcTimer timer;

   for( size_t i=0UL; i<N; ++i ) {
      a[i] = ::blaze::rand<real>();
      b[i] = ::blaze::rand<real>();
   }

   for( size_t i=0UL; i<N; ++i )
      c[i] = a[i] + b[i];

   for( size_t rep=0UL; rep<reps; ++rep )
   {
      timer.start();
      for( size_t step=0UL; step<steps; ++step ) {
         for( size_t i=0UL; i<N; ++i )
            c[i] = a[i] - b[i];
      }
      timer.end();

      if( c[0] < real(0) )
         std::cerr << " Line " << __LINE__ << ": ERROR detected!!!\n";

      if( timer.last() > maxtime )
         break;
   }

   delete[] a;
   delete[] b;
   delete[] c;

   const double minTime( timer.min()     );
   const double avgTime( timer.average() );

   if( minTime * ( 1.0 + deviation*0.01 ) < avgTime )
      std::cerr << " C-like kernel 'dvecdvecsub': Time deviation too large!!!\n";

   return minTime;
}
//*************************************************************************************************

} // namespace clike

} // namespace blazemark
