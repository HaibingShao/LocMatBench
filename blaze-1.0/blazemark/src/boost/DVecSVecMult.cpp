//=================================================================================================
/*!
//  \file src/boost/DVecSVecMult.cpp
//  \brief Source file for the Boost dense vector/sparse vector multiplication kernel
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
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_sparse.hpp>
#include <blaze/util/Random.h>
#include <blaze/util/Timing.h>
#include <blazemark/boost/DVecSVecMult.h>
#include <blazemark/system/Config.h>
#include <blazemark/system/Precision.h>
#include <blazemark/util/Indices.h>


namespace blazemark {

namespace boost {

//=================================================================================================
//
//  KERNEL FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Boost uBLASL dense vector/sparse vector multiplication kernel.
//
// \param N The size of the vectors for the multiplication.
// \param F The number of non-zero elements for the sparse vector.
// \param steps The number of iteration steps to perform.
// \return Minimum runtime of the kernel function.
//
// This kernel function implements the dense vector/sparse vector multiplication by means of
// the Boost uBLAS functionality.
*/
double dvecsvecmult( size_t N, size_t F, size_t steps )
{
   using ::blazemark::real;

   ::blaze::setSeed( seed );

   ::boost::numeric::ublas::vector<real> a( N );
   ::boost::numeric::ublas::compressed_vector<real> b( N ), c( N );
   ::blaze::timing::WcTimer timer;

   for( size_t i=0UL; i<N; ++i ) {
      a[i] = ::blaze::rand<real>();
   }

   {
      ::blazemark::Indices indices( N, F );
      for( ::blazemark::Indices::Iterator it=indices.begin(); it!=indices.end(); ++it ) {
         b[*it] = ::blaze::rand<real>();
      }
   }

   noalias( c ) = element_prod( a, b );

   for( size_t rep=0UL; rep<reps; ++rep )
   {
      timer.start();
      for( size_t step=0UL; step<steps; ++step ) {
         noalias( c ) = element_prod( a, b );
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
      std::cerr << " Boost uBLAS kernel 'dvecsvecmult': Time deviation too large!!!\n";

   return minTime;
}
//*************************************************************************************************

} // namespace boost

} // namespace blazemark
