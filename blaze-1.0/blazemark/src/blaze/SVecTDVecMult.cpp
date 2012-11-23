//=================================================================================================
/*!
//  \file src/blaze/SVecTDVecMult.cpp
//  \brief Source file for the Blaze sparse vector/dense vector outer product kernel
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
#include <blaze/math/CompressedMatrix.h>
#include <blaze/math/CompressedVector.h>
#include <blaze/math/DynamicVector.h>
#include <blaze/util/Random.h>
#include <blaze/util/Timing.h>
#include <blazemark/blaze/SVecTDVecMult.h>
#include <blazemark/system/Config.h>
#include <blazemark/system/Precision.h>
#include <blazemark/util/Indices.h>


namespace blazemark {

namespace blaze {

//=================================================================================================
//
//  KERNEL FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Blaze sparse vector/dense vector outer product kernel.
//
// \param N The size of the vectors for the outer product.
// \param F The number of non-zero elements for the sparse vector.
// \param steps The number of iteration steps to perform.
// \return Minimum runtime of the kernel function.
//
// This kernel function implements the sparse vector/dense vector outer product by means of the
// Blaze functionality.
*/
double svectdvecmult( size_t N, size_t F, size_t steps )
{
   using ::blazemark::real;
   using ::blaze::rowVector;
   using ::blaze::columnVector;
   using ::blaze::rowMajor;

   ::blaze::setSeed( seed );

   ::blaze::CompressedVector<real,columnVector> a( N );
   ::blaze::DynamicVector<real,rowVector> b( N );
   ::blaze::CompressedMatrix<real,rowMajor> A( N, N );
   ::blaze::timing::WcTimer timer;

   {
      ::blazemark::Indices indices( N, F );
      for( ::blazemark::Indices::Iterator it=indices.begin(); it!=indices.end(); ++it ) {
         a[*it] = ::blaze::rand<real>();
      }
   }

   for( size_t i=0UL; i<N; ++i ) {
      b[i] = ::blaze::rand<real>();
   }

   A = a * b;

   for( size_t rep=0UL; rep<reps; ++rep )
   {
      timer.start();
      for( size_t step=0UL; step<steps; ++step ) {
         A = a * b;
      }
      timer.end();

      if( A.rows() != N )
         std::cerr << " Line " << __LINE__ << ": ERROR detected!!!\n";

      if( timer.last() > maxtime )
         break;
   }

   const double minTime( timer.min()     );
   const double avgTime( timer.average() );

   if( minTime * ( 1.0 + deviation*0.01 ) < avgTime )
      std::cerr << " Blaze kernel 'svectdvecmult': Time deviation too large!!!\n";

   return minTime;
}
//*************************************************************************************************

} // namespace blaze

} // namespace blazemark
