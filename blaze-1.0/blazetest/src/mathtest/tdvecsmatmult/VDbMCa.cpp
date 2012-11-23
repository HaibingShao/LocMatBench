//=================================================================================================
/*!
//  \file src/mathtest/tdvecsmatmult/VDbMCa.cpp
//  \brief Source file for the VDbMCa dense vector/sparse matrix multiplication math test
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
#include <blaze/math/DynamicVector.h>
#include <blazetest/mathtest/TDVecSMatMult.h>
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
   std::cout << "   Running 'VDbMCa'..." << std::endl;

   using blazetest::mathtest::TypeA;
   using blazetest::mathtest::TypeB;

   try
   {
      // Matrix type definitions
      typedef blaze::DynamicVector<TypeB>     VDb;
      typedef blaze::CompressedMatrix<TypeA>  MCa;

      // Creator type definitions
      typedef blazetest::Creator<VDb>  CVDb;
      typedef blazetest::Creator<MCa>  CMCa;

      // Running tests with small vectors and matrices
      for( size_t i=0UL; i<=6UL; ++i ) {
         for( size_t j=0UL; j<=6UL; ++j ) {
            for( size_t k=0UL; k<=i*j; ++k ) {
               RUN_TDVECSMATMULT_TEST( CVDb( i ), CMCa( i, j, k ) );
            }
         }
      }

      // Running tests with large vectors and matrices
      RUN_TDVECSMATMULT_TEST( CVDb(  67UL ), CMCa(  67UL, 127UL, 13UL ) );
      RUN_TDVECSMATMULT_TEST( CVDb( 127UL ), CMCa( 127UL,  67UL,  7UL ) );
      RUN_TDVECSMATMULT_TEST( CVDb(  64UL ), CMCa(  64UL, 128UL, 16UL ) );
      RUN_TDVECSMATMULT_TEST( CVDb( 128UL ), CMCa( 128UL,  64UL,  8UL ) );
   }
   catch( std::exception& ex ) {
      std::cerr << "\n\n ERROR DETECTED during dense vector/sparse matrix multiplication:\n"
                << ex.what() << "\n";
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}
//*************************************************************************************************