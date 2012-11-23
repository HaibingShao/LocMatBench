//=================================================================================================
/*!
//  \file src/mathtest/dmatsvecmult/MDaVCa.cpp
//  \brief Source file for the MDaVCa dense matrix/sparse vector multiplication math test
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
#include <blaze/math/CompressedVector.h>
#include <blaze/math/DynamicMatrix.h>
#include <blazetest/mathtest/DMatSVecMult.h>
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
   std::cout << "   Running 'MDaVCa'..." << std::endl;

   using blazetest::mathtest::TypeA;

   try
   {
      // Matrix type definitions
      typedef blaze::DynamicMatrix<TypeA>     MDa;
      typedef blaze::CompressedVector<TypeA>  VCa;

      // Creator type definitions
      typedef blazetest::Creator<MDa>  CMDa;
      typedef blazetest::Creator<VCa>  CVCa;

      // Running tests with large matrices and vectors
      for( size_t i=0UL; i<=6UL; ++i ) {
         for( size_t j=0UL; j<=6UL; ++j ) {
            for( size_t k=0UL; k<=i; ++k ) {
               RUN_DMATSVECMULT_TEST( CMDa( j, i ), CVCa( i, k ) );
            }
         }
      }

      // Running tests with large matrices and vectors
      RUN_DMATSVECMULT_TEST( CMDa(  67UL, 127UL ), CVCa( 127UL, 13UL ) );
      RUN_DMATSVECMULT_TEST( CMDa( 127UL,  67UL ), CVCa(  67UL,  7UL ) );
      RUN_DMATSVECMULT_TEST( CMDa(  64UL, 128UL ), CVCa( 128UL, 16UL ) );
      RUN_DMATSVECMULT_TEST( CMDa( 128UL,  64UL ), CVCa(  64UL,  8UL ) );
   }
   catch( std::exception& ex ) {
      std::cerr << "\n\n ERROR DETECTED during dense matrix/sparse vector multiplication:\n"
                << ex.what() << "\n";
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}
//*************************************************************************************************
