//=================================================================================================
/*!
//  \file blazetest/config/MathTest.h
//  \brief General configuration file for the math tests of the blaze test suite
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
/*!\brief First data type for the math tests.
//
// The math tests are always conducted with two data types to test mixed type arithmetic
// expressions. This type definition defines the first of these two data types.
*/
typedef int  TypeA;
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Second data type for the math tests.
//
// The math tests are always conducted with two data types to test mixed type arithmetic
// expressions. This type definition defines the second of these two data types.
*/
typedef double  TypeB;
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Compilation switch for the basic tests.
//
// This compilation switch triggers the basic tests for all test scenarios. In case the
// basic tests are activated, each operation is tested separately and without any other
// combined operation. The following example demonstrates this by means of the vector
// addition:

   \code
   blaze::DynamicVector<double> a, b, c;
   c = a + b;  // Basic vector addition
   \endcode

// The following settings are possible:
//
//   - 0: The basic tests are not included in the compilation process and not executed
//   - 1: The basic tests are included in the compilation process, but not executed
//   - 2: The basic tests are included in the compilation process and executed
*/
#define BLAZETEST_MATHTEST_TEST_BASIC_OPERATION 2
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Compilation switch for the negation tests.
//
// This compilation switch triggers the negation tests for all test scenarios. In case the
// negation tests are activated, each operation is tested in combination with a negation.
// The following example demonstrates this by means of the vector addition:

   \code
   blaze::DynamicVector<double> a, b, c;
   c = -( a + b );  // Negated vector addition
   \endcode

// The following settings are possible:
//
//   - 0: The negation tests are not included in the compilation process and not executed
//   - 1: The negation tests are included in the compilation process, but not executed
//   - 2: The negation tests are included in the compilation process and executed
*/
#define BLAZETEST_MATHTEST_TEST_NEGATED_OPERATION 2
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Compilation switch for the scaling tests.
//
// This compilation switch triggers the scaling tests for all test scenarios. In case the
// scaling tests are activated, each operation is tested in combination with a scalar
// multiplication or division. The following example demonstrates this by means of the
// vector addition:

   \code
   blaze::DynamicVector<double> a, b, c;
   c = 1.1 * ( a + b );  // Left-multiplied vector addition
   c = ( a + b ) * 1.1;  // Right-multiplied vector addition
   c = ( a + b ) / 1.1;  // Scalar-divided vector addition
   \endcode

// The following settings are possible:
//
//   - 0: The scaling tests are not included in the compilation process and not executed
//   - 1: The scaling tests are included in the compilation process, but not executed
//   - 2: The scaling tests are included in the compilation process and executed
*/
#define BLAZETEST_MATHTEST_TEST_SCALED_OPERATION 2
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Compilation switch for the transpose tests.
//
// This compilation switch triggers the transpose tests for all test scenarios. In case the
// transpose tests are activated, each operation is tested in combination with a transpose
// operation. The following example demonstrates this by means of the vector addition:

   \code
   blaze::DynamicVector<double,false> a, b, ;
   blaze::DynamicVector<double,true> c;
   c = trans( a + b );  // Transpose vector addition
   \endcode

// The following settings are possible:
//
//   - 0: The transpose tests are not included in the compilation process and not executed
//   - 1: The transpose tests are included in the compilation process, but not executed
//   - 2: The transpose tests are included in the compilation process and executed
*/
#define BLAZETEST_MATHTEST_TEST_TRANSPOSE_OPERATION 2
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Compilation switch for the abs tests.
//
// This compilation switch triggers the abs tests for all test scenarios. In case the abs
// tests are activated, each operation is tested in combination with an abs operation. The
// following example demonstrates this by means of the vector addition:

   \code
   blaze::DynamicVector<double> a, b, c;
   c = abs( a + b );  // Absolute value vector addition
   \endcode

// The following settings are possible:
//
//   - 0: The abs tests are not included in the compilation process and not executed
//   - 1: The abs tests are included in the compilation process, but not executed
//   - 2: The abs tests are included in the compilation process and executed
*/
#define BLAZETEST_MATHTEST_TEST_ABS_OPERATION 2
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Number of repetitions for a single test case.
//
// The \a repetitions value specifies the number of repetitions for each single test case. In
// each repetition the test case is run with random input values. Therefore a higher number of
// repetitions increases the likelihood of detecting implementationn errors. On the downside
// the execution of all tests takes longer.
*/
const size_t repetitions = 10;
//*************************************************************************************************
