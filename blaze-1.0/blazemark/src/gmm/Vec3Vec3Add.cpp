//=================================================================================================
/*!
//  \file src/gmm/Vec3Vec3Add.cpp
//  \brief Source file for the GMM++ 3D vector/vector addition kernel
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
#include <vector>
#include <gmm/gmm.h>
#include <blaze/util/Random.h>
#include <blaze/util/Timing.h>
#include <blazemark/gmm/Vec3Vec3Add.h>
#include <blazemark/system/Config.h>
#include <blazemark/system/Precision.h>


namespace blazemark {

namespace gmm {

//=================================================================================================
//
//  KERNEL FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief GMM++ 3-dimensional vector/vector addition kernel.
//
// \param N The number of 3D vectors to be computed.
// \param steps The number of iteration steps to perform.
// \return Minimum runtime of the kernel function.
//
// This kernel function implements the 3-dimensional vector/vector addition by means of the
// GMM++ functionality.
*/
double vec3vec3add( size_t N, size_t steps )
{
   using ::blazemark::real;

   ::blaze::setSeed( seed );

   ::std::vector< ::std::vector<real> > a( N ), b( N ), c( N );
   ::blaze::timing::WcTimer timer;

   for( size_t i=0UL; i<N; ++i ) {
      a[i].resize( 3UL );
      b[i].resize( 3UL );
      c[i].resize( 3UL );
      for( size_t j=0UL; j<3UL; ++j ) {
         a[i][j] = ::blaze::rand<real>();
         b[i][j] = ::blaze::rand<real>();
      }
   }

   for( size_t i=0UL; i<N; ++i ) {
      ::gmm::add( a[i], b[i], c[i] );
   }

   for( size_t rep=0UL; rep<reps; ++rep )
   {
      timer.start();
      for( size_t step=0UL, i=0UL; step<steps; ++step, ++i ) {
         if( i == N ) i = 0UL;
         ::gmm::add( a[i], b[i], c[i] );
      }
      timer.end();

      for( size_t i=0UL; i<N; ++i )
         if( c[i][0] < real(0) )
            std::cerr << " Line " << __LINE__ << ": ERROR detected!!!\n";

      if( timer.last() > maxtime )
         break;
   }

   const double minTime( timer.min()     );
   const double avgTime( timer.average() );

   if( minTime * ( 1.0 + deviation*0.01 ) < avgTime )
      std::cerr << " GMM++ kernel 'vec3vec3add': Time deviation too large!!!\n";

   return minTime;
}
//*************************************************************************************************

} // namespace gmm

} // namespace blazemark