//=================================================================================================
/*!
//  \file src/mathtest/dmatdmatmult/MDaM7x9b.cpp
//  \brief Source file for the MDaM7x9b dense matrix/dense matrix multiplication math test
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
#include <blaze/math/DynamicMatrix.h>
#include <blaze/math/StaticMatrix.h>
#include <blazetest/mathtest/DMatDMatMult.h>
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
   std::cout << "   Running 'MDaM7x9b'..." << std::endl;

   using blazetest::mathtest::TypeA;
   using blazetest::mathtest::TypeB;

   try
   {
      // Matrix type definitions
      typedef blaze::DynamicMatrix<TypeA>         MDa;
      typedef blaze::StaticMatrix<TypeB,7UL,9UL>  M7x9b;

      // Creator type definitions
      typedef blazetest::Creator<MDa>    CMDa;
      typedef blazetest::Creator<M7x9b>  CM7x9b;

      // Running the tests
      RUN_DMATDMATMULT_TEST( CMDa( 5UL, 7UL ), CM7x9b() );
      RUN_DMATDMATMULT_TEST( CMDa( 7UL, 7UL ), CM7x9b() );
      RUN_DMATDMATMULT_TEST( CMDa( 9UL, 7UL ), CM7x9b() );
   }
   catch( std::exception& ex ) {
      std::cerr << "\n\n ERROR DETECTED during dense matrix/dense matrix multiplication:\n"
                << ex.what() << "\n";
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}
//*************************************************************************************************
