//=================================================================================================
/*!
//  \file src/mathtest/dvecdvecmult/V2bVDb.cpp
//  \brief Source file for the V2bVDb dense vector/dense vector multiplication math test
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
#include <blaze/math/DynamicVector.h>
#include <blaze/math/StaticVector.h>
#include <blazetest/mathtest/DVecDVecMult.h>
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
   std::cout << "   Running 'V2bVDb'..." << std::endl;

   using blazetest::mathtest::TypeB;

   try
   {
      // Vector type definitions
      typedef blaze::StaticVector<TypeB,2UL>  V2b;
      typedef blaze::DynamicVector<TypeB>     VDb;

      // Creator type definitions
      typedef blazetest::Creator<V2b>  CV2b;
      typedef blazetest::Creator<VDb>  CVDb;

      // Running the tests
      RUN_DVECDVECMULT_TEST( CV2b(), CVDb( 2UL ) );
   }
   catch( std::exception& ex ) {
      std::cerr << "\n\n ERROR DETECTED during dense vector/dense vector multiplication:\n"
                << ex.what() << "\n";
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}
//*************************************************************************************************
