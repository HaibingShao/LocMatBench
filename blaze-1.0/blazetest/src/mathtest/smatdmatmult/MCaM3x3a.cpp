//=================================================================================================
/*!
//  \file src/mathtest/smatdmatmult/MCaM3x3a.cpp
//  \brief Source file for the MCaM3x3a sparse matrix/dense matrix multiplication math test
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

#include <cstdlib>
#include <iostream>
#include <blaze/math/CompressedMatrix.h>
#include <blaze/math/StaticMatrix.h>
#include <blazetest/mathtest/SMatDMatMult.h>
#include <blazetest/system/MathTest.h>
#include <blazetest/util/Creator.h>


//=================================================================================================
//
//  MAIN FUNCTION
//
//=================================================================================================

//*************************************************************************************************
int main()
{
   std::cout << "   Running 'MCaM3x3a'..." << std::endl;

   using blazetest::mathtest::TypeA;

   try
   {
      // Matrix type definitions
      typedef blaze::CompressedMatrix<TypeA>      MCa;
      typedef blaze::StaticMatrix<TypeA,3UL,3UL>  M3x3a;

      // Creator type definitions
      typedef blazetest::Creator<MCa>    CMCa;
      typedef blazetest::Creator<M3x3a>  CM3x3a;

      // Running the tests
      for( size_t i=0UL; i<=5UL; ++i ) {
         RUN_SMATDMATMULT_TEST( CMCa( i, 3UL, 0UL        ), CM3x3a() );
         RUN_SMATDMATMULT_TEST( CMCa( i, 3UL, i*3UL*0.25 ), CM3x3a() );
         RUN_SMATDMATMULT_TEST( CMCa( i, 3UL, i*3UL*0.5  ), CM3x3a() );
         RUN_SMATDMATMULT_TEST( CMCa( i, 3UL, i*3UL*0.75 ), CM3x3a() );
         RUN_SMATDMATMULT_TEST( CMCa( i, 3UL, i*3UL      ), CM3x3a() );
      }
   }
   catch( std::exception& ex ) {
      std::cerr << "\n\n ERROR DETECTED during sparse matrix/dense matrix multiplication:\n"
                << ex.what() << "\n";
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}
//*************************************************************************************************
