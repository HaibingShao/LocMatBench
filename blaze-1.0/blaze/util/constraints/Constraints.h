//=================================================================================================
/*!
//  \file blaze/util/constraints/Constraints.h
//  \brief Header file for compile time constraints
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

#ifndef _BLAZE_UTIL_CONSTRAINTS_CONSTRAINTS_H_
#define _BLAZE_UTIL_CONSTRAINTS_CONSTRAINTS_H_


//=================================================================================================
//
//  DOXYGEN DOCUMENTATION
//
//=================================================================================================

//*************************************************************************************************
/*!\defgroup constraints Compile time constraints
// \ingroup util
//
// Compile time constraints offer the possibility to stop the compilation if a constraint that
// can be checked at compile time is not met. These constraints are especially useful for generic
// code, where the type of the involved objects is not known beforehands.\n
// Standard C++ doesn't offer compile time constraints per default. However, most constraints
// can be rebuilt using the standard language features. The most difficult problem is the error
// message the compiler generates if a constraint is not met. The compile time constraints
// implemented in the Blaze library are implemented in such a way that the error message
// contains the source of the constraint violation as close as possible. The following example
// demonstrates one of the Blaze compile time constraints and shows two possible error messages
// for two different compilers:

   \code
   //----- Test.cpp -----
   #include <blaze/Util.h>

   class A {};
   class B : public A {};
   class C {};

   int main()
   {
      BLAZE_CONSTRAINT_MUST_BE_DERIVED_FROM( B, A );  // No compile time error, B is derived from A
      BLAZE_CONSTRAINT_MUST_BE_DERIVED_FROM( C, A );  // Compile time error, C is not derived from A
   }
   \endcode

// The resulting error message generated by the GNU g++ compiler contains the name of the
// constraint and the source code line of the constraint violation:

   \code
   Test.cpp: In function 'int main()':
   Test.cpp:11: incomplete type ‘blaze::CONSTRAINT_MUST_BE_DERIVED_FROM_FAILED<false>’ used in nested name specifier
   \endcode

// The Intel C++-compiler gives the following error message that also contains the name of the
// violated constraint and the source code line of the violation:

   \code
   Test.cpp(11): error: incomplete type is not allowed
        BLAZE_CONSTRAINT_MUST_BE_DERIVED_FROM( C, A );
   \endcode
*/
//*************************************************************************************************

#endif
