//=================================================================================================
/*!
//  \file src/gmm/Complex5.cpp
//  \brief Source file for the GMM++ kernel for the complex expression D = ( A * B ) + C
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
#include <gmm/gmm.h>
#include <blaze/util/Random.h>
#include <blaze/util/Timing.h>
#include <blazemark/gmm/Complex5.h>
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
/*!\brief GMM++ kernel for the complex expression D = ( A * B ) + C.
//
// \param N The number of rows and columns of the matrix and the size of the vectors.
// \param steps The number of iteration steps to perform.
// \return Minimum runtime of the kernel function.
//
// This kernel function implements the complex expression D = ( A * B ) + C by means of the
// GMM++ functionality.
*/
double complex5( size_t N, size_t steps )
{
   using ::blazemark::real;

   ::blaze::setSeed( seed );

   ::gmm::dense_matrix<real> A( N, N ), B( N, N ), C( N, N ), D( N, N ), T( N, N );
   ::blaze::timing::WcTimer timer;

   for( size_t j=0UL; j<N; ++j ) {
      for( size_t i=0UL; i<N; ++i ) {
         A(i,j) = ::blaze::rand<real>();
         B(i,j) = ::blaze::rand<real>();
         C(i,j) = ::blaze::rand<real>();
      }
   }

   ::gmm::mult( A, B, T );
   ::gmm::add( T, C, D );

   for( size_t rep=0UL; rep<reps; ++rep )
   {
      timer.start();
      for( size_t step=0UL; step<steps; ++step ) {
         ::gmm::mult( A, B, T );
         ::gmm::add( T, C, D );
      }
      timer.end();

      if( ::gmm::mat_ncols(D) != N )
         std::cerr << " Line " << __LINE__ << ": ERROR detected!!!\n";

      if( timer.last() > maxtime )
         break;
   }

   const double minTime( timer.min()     );
   const double avgTime( timer.average() );

   if( minTime * ( 1.0 + deviation*0.01 ) < avgTime )
      std::cerr << " GMM++ kernel 'complex5': Time deviation too large!!!\n";

   return minTime;
}
//*************************************************************************************************

} // namespace gmm

} // namespace blazemark
