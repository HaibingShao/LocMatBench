//=================================================================================================
/*!
//  \file src/mathtest/svecdvecadd/VCbV5b.cpp
//  \brief Source file for the VCbV5b sparse vector/dense vector addition math test
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
#include <blaze/math/StaticVector.h>
#include <blazetest/mathtest/SVecDVecAdd.h>
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
   std::cout << "   Running 'VCbV5b'..." << std::endl;

   using blazetest::mathtest::TypeB;

   try
   {
      // Vector type definitions
      typedef blaze::CompressedVector<TypeB>  VCb;
      typedef blaze::StaticVector<TypeB,5UL>  V5b;

      // Creator type definitions
      typedef blazetest::Creator<VCb>  CVCb;
      typedef blazetest::Creator<V5b>  CV5b;

      // Running the tests
      for( size_t i=0UL; i<=5UL; ++i ) {
         RUN_SVECDVECADD_TEST( CVCb( 5UL, i ), CV5b() );
      }
   }
   catch( std::exception& ex ) {
      std::cerr << "\n\n ERROR DETECTED during sparse vector/dense vector addition:\n"
                << ex.what() << "\n";
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}
//*************************************************************************************************
