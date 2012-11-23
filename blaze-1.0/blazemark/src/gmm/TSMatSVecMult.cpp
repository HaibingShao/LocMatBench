//=================================================================================================
/*!
//  \file src/gmm/TSMatSVecMult.cpp
//  \brief Source file for the GMM++ transpose sparse matrix/sparse vector multiplication kernel
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
#include <blazemark/gmm/TSMatSVecMult.h>
#include <blazemark/system/Config.h>
#include <blazemark/system/Precision.h>
#include <blazemark/util/Indices.h>


namespace blazemark {

namespace gmm {

//=================================================================================================
//
//  KERNEL FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief GMM++ transpose sparse matrix/sparse vector multiplication kernel.
//
// \param N The number of rows and columns of the matrix and the size of the vector.
// \param F The number of non-zero elements in each column of the sparse matrix and in the sparse vector.
// \param steps The number of iteration steps to perform.
// \return Minimum runtime of the kernel function.
//
// This kernel function implements the transpose sparse matrix/sparse vector multiplication by
// means of the GMM++ functionality.
*/
double tsmatsvecmult( size_t N, size_t F, size_t steps )
{
   using ::blazemark::real;

   ::blaze::setSeed( seed );

   ::gmm::col_matrix< ::gmm::wsvector<double> > T( N, N );
   ::gmm::csc_matrix<real> A( N, N );
   ::gmm::rsvector<real> a( N ), b( N );
   ::blaze::timing::WcTimer timer;

   for( size_t j=0UL; j<N; ++j ) {
      ::blazemark::Indices indices( N, F );
      for( ::blazemark::Indices::Iterator it=indices.begin(); it!=indices.end(); ++it ) {
         T(*it,j) = ::blaze::rand<real>();
      }
   }

   copy( T, A );

   {
      ::blazemark::Indices indices( N, F );
      for( ::blazemark::Indices::Iterator it=indices.begin(); it!=indices.end(); ++it ) {
         a[*it] = ::blaze::rand<real>();
      }
   }

   mult( A, a, b );

   for( size_t rep=0UL; rep<reps; ++rep )
   {
      timer.start();
      for( size_t step=0UL; step<steps; ++step ) {
         mult( A, a, b );
      }
      timer.end();

      if( vect_size(b) != N )
         std::cerr << " Line " << __LINE__ << ": ERROR detected!!!\n";

      if( timer.last() > maxtime )
         break;
   }

   const double minTime( timer.min()     );
   const double avgTime( timer.average() );

   if( minTime * ( 1.0 + deviation*0.01 ) < avgTime )
      std::cerr << " GMM++ kernel 'tsmatsvecmult': Time deviation too large!!!\n";

   return minTime;
}
//*************************************************************************************************

} // namespace gmm

} // namespace blazemark