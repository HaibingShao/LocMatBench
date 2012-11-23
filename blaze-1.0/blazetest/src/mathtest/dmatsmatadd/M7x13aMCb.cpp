//=================================================================================================
/*!
//  \file src/mathtest/dmatsmatadd/M7x13aMCb.cpp
//  \brief Source file for the M7x13aMCb dense matrix/sparse matrix addition math test
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
#include <blazetest/mathtest/DMatSMatAdd.h>
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
   std::cout << "   Running 'M7x13aMCb'..." << std::endl;

   using blazetest::mathtest::TypeA;
   using blazetest::mathtest::TypeB;

   try
   {
      // Matrix type definitions
      typedef blaze::StaticMatrix<TypeA,7UL,13UL>  M7x13a;
      typedef blaze::CompressedMatrix<TypeB>       MCb;

      // Creator type definitions
      typedef blazetest::Creator<M7x13a>  CM7x13a;
      typedef blazetest::Creator<MCb>     CMCb;

      // Running the tests
      RUN_DMATSMATADD_TEST( CM7x13a(), CMCb( 7UL, 13UL,  0UL ) );
      RUN_DMATSMATADD_TEST( CM7x13a(), CMCb( 7UL, 13UL, 30UL ) );
      RUN_DMATSMATADD_TEST( CM7x13a(), CMCb( 7UL, 13UL, 60UL ) );
      RUN_DMATSMATADD_TEST( CM7x13a(), CMCb( 7UL, 13UL, 91UL ) );
   }
   catch( std::exception& ex ) {
      std::cerr << "\n\n ERROR DETECTED during dense matrix/sparse matrix addition:\n"
                << ex.what() << "\n";
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}
//*************************************************************************************************
