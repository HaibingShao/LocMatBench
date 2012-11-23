//=================================================================================================
/*!
//  \file src/mathtest/smatdmatsub/MCaM16x8b.cpp
//  \brief Source file for the MCaM16x8a sparse matrix/dense matrix subtraction math test
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
#include <cstdlib>
#include <iostream>
#include <blaze/math/CompressedMatrix.h>
#include <blaze/math/StaticMatrix.h>
#include <blazetest/mathtest/SMatDMatSub.h>
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
   std::cout << "   Running 'MCaM16x8b'..." << std::endl;

   using blazetest::mathtest::TypeA;
   using blazetest::mathtest::TypeB;

   try
   {
      // Matrix type definitions
      typedef blaze::CompressedMatrix<TypeA>       MCa;
      typedef blaze::StaticMatrix<TypeB,16UL,8UL>  M16x8b;

      // Creator type definitions
      typedef blazetest::Creator<MCa>     CMCa;
      typedef blazetest::Creator<M16x8b>  CM16x8b;

      // Running the tests
      RUN_SMATDMATSUB_TEST( CMCa( 16UL, 8UL,   0UL ), CM16x8b() );
      RUN_SMATDMATSUB_TEST( CMCa( 16UL, 8UL,  32UL ), CM16x8b() );
      RUN_SMATDMATSUB_TEST( CMCa( 16UL, 8UL,  64UL ), CM16x8b() );
      RUN_SMATDMATSUB_TEST( CMCa( 16UL, 8UL,  96UL ), CM16x8b() );
      RUN_SMATDMATSUB_TEST( CMCa( 16UL, 8UL, 128UL ), CM16x8b() );
   }
   catch( std::exception& ex ) {
      std::cerr << "\n\n ERROR DETECTED during sparse matrix/dense matrix subtraction:\n"
                << ex.what() << "\n";
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}
//*************************************************************************************************
