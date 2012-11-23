//=================================================================================================
/*!
//  \file src/blaze/CG.cpp
//  \brief Source file for the conjugate gradient benchmark
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

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <blaze/math/CompressedMatrix.h>
#include <blaze/math/DynamicVector.h>
#include <blaze/math/Functions.h>
#include <blaze/util/Random.h>
#include <blaze/util/Timing.h>
#include <blazemark/blaze/CG.h>
#include <blazemark/boost/CG.h>
#include <blazemark/eigen/CG.h>
#include <blazemark/gmm/CG.h>
#include <blazemark/mtl/CG.h>
#include <blazemark/system/Config.h>
#include <blazemark/system/Eigen.h>
#include <blazemark/system/GMM.h>
#include <blazemark/system/MTL.h>
#include <blazemark/system/Precision.h>
#include <blazemark/system/Types.h>
#include <blazemark/util/Benchmarks.h>
#include <blazemark/util/Parser.h>
#include <blazemark/util/SolverRun.h>


//*************************************************************************************************
// Using declarations
//*************************************************************************************************

using blazemark::Benchmarks;
using blazemark::Parser;
using blazemark::SolverRun;




//=================================================================================================
//
//  UTILITY FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Estimating the necessary number of steps and iterations for each benchmark.
//
// \param run The parameters for the benchmark run.
// \return void
//
// This function estimates the necessary number of steps for the given benchmark based on the
// performance of the Blaze library.
*/
void estimateSteps( SolverRun& run )
{
   using blazemark::real;
   using blaze::columnVector;
   using blaze::rowMajor;

   const size_t N ( run.getSize() );
   const size_t NN( N*N );

   size_t iterations( run.getIterations() );
   if( iterations == 0UL || iterations > NN ) {
      iterations = NN;
   }

   std::vector<size_t> nnz( NN, 5UL );
   for( size_t i=0UL; i<N; ++i ) {
      for( size_t j=0UL; j<N; ++j ) {
         if( i == 0UL || i == N-1UL ) --nnz[i*N+j];
         if( j == 0UL || j == N-1UL ) --nnz[i*N+j];
      }
   }

   ::blaze::CompressedMatrix<real,rowMajor> A( NN, NN, nnz );
   ::blaze::DynamicVector<real,columnVector> x( NN ), b( NN, 0 ), r( NN ), d( NN ), h( NN );
   real alpha, beta, delta;
   size_t iteration( 0UL );
   ::blaze::timing::WcTimer timer;

   for( size_t i=0UL; i<N; ++i ) {
      for( size_t j=0UL; j<N; ++j ) {
         if( i > 0UL   ) A.append( i*N+j, (i-1UL)*N+j, -1.0 );  // Top neighbor
         if( j > 0UL   ) A.append( i*N+j, i*N+j-1UL  , -1.0 );  // Left neighbor
         A.append( i*N+j, i*N+j, 4.0 );
         if( j < N-1UL ) A.append( i*N+j, i*N+j+1UL  , -1.0 );  // Right neighbor
         if( i < N-1UL ) A.append( i*N+j, (i+1UL)*N+j, -1.0 );  // Bottom neighbor
      }
   }

   ::blaze::setSeed( ::blazemark::seed );

   for( size_t i=0UL; i<NN; ++i ) {
      x[i] = ::blaze::rand<real>();
   }

   timer.start();

   r = A * x + b;
   delta = trans(r) * r;
   d = -r;

   for( ; iteration<iterations; ++iteration ) {
      h = A * d;
      alpha = delta / ( trans(d) * h );
      x += alpha * d;
      r += alpha * h;
      beta = trans(r) * r;
      if( std::sqrt( beta ) < 1E-8 ) break;
      d = ( beta / delta ) * d - r;
      delta = beta;
   }

   timer.end();

   if( x.size() != NN )
      std::cerr << " Line " << __LINE__ << ": ERROR detected!!!\n";

   if( timer.last() > blazemark::runtime ) {
      iteration = blaze::max( 1UL, iteration * ( blazemark::runtime / timer.last() ) );
   }
   run.setIterations( iteration );

   if( run.getSteps() == 0UL ) {
      if( timer.last() != 0.0 )
         run.setSteps( blaze::max( 1UL, blazemark::runtime / timer.last() ) );
      else
         run.setSteps( static_cast<size_t>( blazemark::runtime / 1E-8 ) );
   }
}
//*************************************************************************************************




//=================================================================================================
//
//  BENCHMARK FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Conjugate gradient benchmark function.
//
// \param runs The specified benchmark runs.
// \param benchmarks The selection of benchmarks.
// \return void
*/
void cg( std::vector<SolverRun>& runs, Benchmarks benchmarks )
{
   std::cout << std::left;

   std::sort( runs.begin(), runs.end() );

   for( std::vector<SolverRun>::iterator run=runs.begin(); run!=runs.end(); ++run ) {
      estimateSteps( *run );
   }

   if( benchmarks.runBlaze ) {
      std::cout << "   Blaze [MFlop/s]:\n";
      for( std::vector<SolverRun>::iterator run=runs.begin(); run!=runs.end(); ++run ) {
         const size_t N         ( run->getSize()  );
         const size_t steps     ( run->getSteps() );
         const size_t iterations( run->getIterations() );
         run->setBlazeResult( blazemark::blaze::cg( N, steps, iterations ) );
         const double mflops( ( ( 13UL*N*N - 8UL*N - 1UL ) * steps +
                                ( 19UL*N*N - 8UL*N ) * steps * iterations ) / run->getBlazeResult() / 1E6 );
         std::cout << "     " << std::setw(12) << N << mflops << std::endl;
      }
   }

   if( benchmarks.runBoost ) {
      std::cout << "   Boost uBLAS [MFlop/s]:\n";
      for( std::vector<SolverRun>::iterator run=runs.begin(); run!=runs.end(); ++run ) {
         const size_t N         ( run->getSize()  );
         const size_t steps     ( run->getSteps() );
         const size_t iterations( run->getIterations() );
         run->setBoostResult( blazemark::boost::cg( N, steps, iterations ) );
         const double mflops( ( ( 13UL*N*N - 8UL*N - 1UL ) * steps +
                                ( 19UL*N*N - 8UL*N ) * steps * iterations ) / run->getBoostResult() / 1E6 );
         std::cout << "     " << std::setw(12) << N << mflops << std::endl;
      }
   }

#if BLAZEMARK_GMM_MODE
   if( benchmarks.runGMM ) {
      std::cout << "   GMM++ [MFlop/s]:\n";
      for( std::vector<SolverRun>::iterator run=runs.begin(); run!=runs.end(); ++run ) {
         const size_t N         ( run->getSize()  );
         const size_t steps     ( run->getSteps() );
         const size_t iterations( run->getIterations() );
         run->setGMMResult( blazemark::gmm::cg( N, steps, iterations ) );
         const double mflops( ( ( 13UL*N*N - 8UL*N - 1UL ) * steps +
                                ( 19UL*N*N - 8UL*N ) * steps * iterations ) / run->getGMMResult() / 1E6 );
         std::cout << "     " << std::setw(12) << N << mflops << std::endl;
      }
   }
#endif

#if BLAZEMARK_MTL_MODE
   if( benchmarks.runMTL ) {
      std::cout << "   MTL [MFlop/s]:\n";
      for( std::vector<SolverRun>::iterator run=runs.begin(); run!=runs.end(); ++run ) {
         const size_t N         ( run->getSize()  );
         const size_t steps     ( run->getSteps() );
         const size_t iterations( run->getIterations() );
         run->setMTLResult( blazemark::mtl::cg( N, steps, iterations ) );
         const double mflops( ( ( 13UL*N*N - 8UL*N - 1UL ) * steps +
                                ( 19UL*N*N - 8UL*N ) * steps * iterations ) / run->getMTLResult() / 1E6 );
         std::cout << "     " << std::setw(12) << N << mflops << std::endl;
      }
   }
#endif

#if BLAZEMARK_EIGEN_MODE
   if( benchmarks.runEigen ) {
      std::cout << "   Eigen [MFlop/s]:\n";
      for( std::vector<SolverRun>::iterator run=runs.begin(); run!=runs.end(); ++run ) {
         const size_t N         ( run->getSize()  );
         const size_t steps     ( run->getSteps() );
         const size_t iterations( run->getIterations() );
         run->setEigenResult( blazemark::eigen::cg( N, steps, iterations ) );
         const double mflops( ( ( 13UL*N*N - 8UL*N - 1UL ) * steps +
                                ( 19UL*N*N - 8UL*N ) * steps * iterations ) / run->getEigenResult() / 1E6 );
         std::cout << "     " << std::setw(12) << N << mflops << std::endl;
      }
   }
#endif

   for( std::vector<SolverRun>::iterator run=runs.begin(); run!=runs.end(); ++run ) {
      std::cout << *run;
   }
}
//*************************************************************************************************




//=================================================================================================
//
//  MAIN FUNCTION
//
//=================================================================================================

//*************************************************************************************************
/*!\brief The main function for the conjugate gradient benchmark.
//
// \param argc The total number of command line arguments.
// \param argv The array of command line arguments.
// \return void
*/
int main( int argc, char** argv )
{
   std::cout << "\n Conjugate Gradient Method:\n";

   Benchmarks benchmarks;
   parseCommandLineArguments( argc, argv, benchmarks );

   const std::string installPath( INSTALL_PATH );
   const std::string parameterFile( installPath + "/params/cg.prm" );
   Parser<SolverRun> parser;
   std::vector<SolverRun> runs;

   try {
      parser.parse( parameterFile.c_str(), runs );
   }
   catch( std::exception& ex ) {
      std::cerr << "   Error during parameter extraction: " << ex.what() << "\n";
      return EXIT_FAILURE;
   }

   try {
      cg( runs, benchmarks );
   }
   catch( std::exception& ex ) {
      std::cerr << "   Error during benchmark execution: " << ex.what() << "\n";
      return EXIT_FAILURE;
   }
}
//*************************************************************************************************
