//=================================================================================================
/*!
//  \file src/mathtest/smatdmatmult/MCaM6x6a.cpp
//  \brief Source file for the MCaM6x6a sparse matrix/dense matrix multiplication math test
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
   std::cout << "   Running 'MCaM6x6a'..." << std::endl;

   using blazetest::mathtest::TypeA;

   try
   {
      // Matrix type definitions
      typedef blaze::CompressedMatrix<TypeA>      MCa;
      typedef blaze::StaticMatrix<TypeA,6UL,6UL>  M6x6a;

      // Creator type definitions
      typedef blazetest::Creator<MCa>    CMCa;
      typedef blazetest::Creator<M6x6a>  CM6x6a;

      // Running the tests
      for( size_t i=0UL; i<=8UL; ++i ) {
         RUN_SMATDMATMULT_TEST( CMCa( i, 6UL, 0UL        ), CM6x6a() );
         RUN_SMATDMATMULT_TEST( CMCa( i, 6UL, i*6UL*0.25 ), CM6x6a() );
         RUN_SMATDMATMULT_TEST( CMCa( i, 6UL, i*6UL*0.5  ), CM6x6a() );
         RUN_SMATDMATMULT_TEST( CMCa( i, 6UL, i*6UL*0.75 ), CM6x6a() );
         RUN_SMATDMATMULT_TEST( CMCa( i, 6UL, i*6UL      ), CM6x6a() );
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
