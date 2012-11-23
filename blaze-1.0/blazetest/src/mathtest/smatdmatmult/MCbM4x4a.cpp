//=================================================================================================
/*!
//  \file src/mathtest/smatdmatmult/MCbM4x4a.cpp
//  \brief Source file for the MCbM4x4a sparse matrix/dense matrix multiplication math test
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
   std::cout << "   Running 'MCbM4x4a'..." << std::endl;

   using blazetest::mathtest::TypeA;
   using blazetest::mathtest::TypeB;

   try
   {
      // Matrix type definitions
      typedef blaze::CompressedMatrix<TypeB>      MCb;
      typedef blaze::StaticMatrix<TypeA,4UL,4UL>  M4x4a;

      // Creator type definitions
      typedef blazetest::Creator<MCb>    CMCb;
      typedef blazetest::Creator<M4x4a>  CM4x4a;

      // Running the tests
      for( size_t i=0UL; i<=6UL; ++i ) {
         RUN_SMATDMATMULT_TEST( CMCb( i, 4UL, 0UL        ), CM4x4a() );
         RUN_SMATDMATMULT_TEST( CMCb( i, 4UL, i*4UL*0.25 ), CM4x4a() );
         RUN_SMATDMATMULT_TEST( CMCb( i, 4UL, i*4UL*0.5  ), CM4x4a() );
         RUN_SMATDMATMULT_TEST( CMCb( i, 4UL, i*4UL*0.75 ), CM4x4a() );
         RUN_SMATDMATMULT_TEST( CMCb( i, 4UL, i*4UL      ), CM4x4a() );
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