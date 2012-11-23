//=================================================================================================
/*!
//  \file src/boost/TDMatDMatMult.cpp
//  \brief Source file for the Boost transpose dense matrix/dense matrix multiplication kernel
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
#include <boost/numeric/ublas/matrix.hpp>
#include <blaze/util/Random.h>
#include <blaze/util/Timing.h>
#include <blazemark/boost/TDMatDMatMult.h>
#include <blazemark/system/Config.h>
#include <blazemark/system/Precision.h>


namespace blazemark {

namespace boost {

//=================================================================================================
//
//  KERNEL FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Boost uBLAS transpose dense matrix/dense matrix multiplication kernel.
//
// \param N The number of rows and columns of the matrices.
// \param steps The number of iteration steps to perform.
// \return Minimum runtime of the kernel function.
//
// This kernel function implements the transpose dense matrix/dense matrix multiplication by
// means of the Boost uBLAS functionality.
*/
double tdmatdmatmult( size_t N, size_t steps )
{
   using ::blazemark::real;
   using ::boost::numeric::ublas::row_major;
   using ::boost::numeric::ublas::column_major;

   ::blaze::setSeed( seed );

   ::boost::numeric::ublas::matrix<real,column_major> A( N, N ), C( N, N );
   ::boost::numeric::ublas::matrix<real,row_major> B( N, N );
   ::blaze::timing::WcTimer timer;

   for( size_t j=0UL; j<N; ++j ) {
      for( size_t i=0UL; i<N; ++i ) {
         A(i,j) = ::blaze::rand<real>();
      }
   }

   for( size_t i=0UL; i<N; ++i ) {
      for( size_t j=0UL; j<N; ++j ) {
         B(i,j) = ::blaze::rand<real>();
      }
   }

   noalias( C ) = prod( A, B );

   for( size_t rep=0UL; rep<reps; ++rep )
   {
      timer.start();
      for( size_t step=0UL; step<steps; ++step ) {
         noalias( C ) = prod( A, B );
      }
      timer.end();

      if( C.size1() != N )
         std::cerr << " Line " << __LINE__ << ": ERROR detected!!!\n";

      if( timer.last() > maxtime )
         break;
   }

   const double minTime( timer.min()     );
   const double avgTime( timer.average() );

   if( minTime * ( 1.0 + deviation*0.01 ) < avgTime )
      std::cerr << " Boost uBLAS kernel 'tdmatdmatmult': Time deviation too large!!!\n";

   return minTime;
}
//*************************************************************************************************

} // namespace boost

} // namespace blazemark