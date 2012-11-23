//=================================================================================================
/*!
//  \file src/eigen/SMatTDMatMult.cpp
//  \brief Source file for the Eigen sparse matrix/transpose dense matrix multiplication kernel
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
#include <boost/cast.hpp>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <blaze/util/Random.h>
#include <blaze/util/Timing.h>
#include <blazemark/eigen/SMatTDMatMult.h>
#include <blazemark/system/Config.h>
#include <blazemark/system/Precision.h>
#include <blazemark/util/Indices.h>


namespace blazemark {

namespace eigen {

//=================================================================================================
//
//  KERNEL FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Eigen sparse matrix/transpose dense matrix multiplication kernel.
//
// \param N The number of rows and columns of the matrices.
// \param F The number of non-zero elements in each row of the sparse matrix.
// \param steps The number of iteration steps to perform.
// \return Minimum runtime of the kernel function.
//
// This kernel function implements the sparse matrix/transpose dense matrix multiplication by
// means of the Eigen functionality.
*/
double smattdmatmult( size_t N, size_t F, size_t steps )
{
   using ::blazemark::real;
   using ::boost::numeric_cast;
   using ::Eigen::Dynamic;
   using ::Eigen::RowMajor;
   using ::Eigen::ColMajor;

   ::blaze::setSeed( seed );

   ::Eigen::SparseMatrix<real,RowMajor,EigenSparseIndexType> A( N, N );
   ::Eigen::Matrix<real,Dynamic,Dynamic,ColMajor> B( N, N );
   ::Eigen::Matrix<real,Dynamic,Dynamic,RowMajor> C( N, N );
   ::blaze::timing::WcTimer timer;

   A.reserve( N*F );

   for( size_t i=0UL; i<N; ++i ) {
      A.startVec( i );
      ::blazemark::Indices indices( N, F );
      for( ::blazemark::Indices::Iterator it=indices.begin(); it!=indices.end(); ++it ) {
         A.insertBack(i,*it) = ::blaze::rand<real>();
      }
   }

   A.finalize();

   for( size_t j=0UL; j<N; ++j ) {
      for( size_t i=0UL; i<N; ++i ) {
         B(i,j) = ::blaze::rand<real>();
      }
   }

   C.noalias() = A * B;

   for( size_t rep=0UL; rep<reps; ++rep )
   {
      timer.start();
      for( size_t step=0UL; step<steps; ++step ) {
         C.noalias() = A * B;
      }
      timer.end();

      if( numeric_cast<size_t>( C.rows() ) != N )
         std::cerr << " Line " << __LINE__ << ": ERROR detected!!!\n";

      if( timer.last() > maxtime )
         break;
   }

   const double minTime( timer.min()     );
   const double avgTime( timer.average() );

   if( minTime * ( 1.0 + deviation*0.01 ) < avgTime )
      std::cerr << " Eigen kernel 'smattdmatmult': Time deviation too large!!!\n";

   return minTime;
}
//*************************************************************************************************

} // namespace eigen

} // namespace blazemark
