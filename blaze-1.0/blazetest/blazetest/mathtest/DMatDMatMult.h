//=================================================================================================
/*!
//  \file blazetest/mathtest/DMatDMatMult.h
//  \brief Header file for the dense matrix/dense matrix multiplication math test
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

#ifndef _BLAZETEST_MATHTEST_DMATDMATMULT_H_
#define _BLAZETEST_MATHTEST_DMATDMATMULT_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <blaze/math/CompressedMatrix.h>
#include <blaze/math/constraints/DenseMatrix.h>
#include <blaze/math/constraints/SparseMatrix.h>
#include <blaze/math/constraints/StorageOrder.h>
#include <blaze/math/DynamicMatrix.h>
#include <blaze/math/MathTrait.h>
#include <blaze/math/shims/Equal.h>
#include <blaze/math/StaticMatrix.h>
#include <blaze/math/typetraits/IsRowMajorMatrix.h>
#include <blaze/util/constraints/Numeric.h>
#include <blaze/util/constraints/SameType.h>
#include <blazetest/system/MathTest.h>
#include <blazetest/util/Creator.h>
#include <blazetest/util/Utility.h>


namespace blazetest {

namespace mathtest {

namespace dmatdmatmult {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Auxiliary class template for the dense matrix/dense matrix multiplication math test.
//
// The DMatDMatMult class template represents one particular matrix multiplication test between two
// matrices of a particular type. The two template arguments \a MT1 and \a MT2 represent the
// types of the left-hand side and right-hand side matrix, respectively.
*/
template< typename MT1    // Type of the left-hand side dense matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
class DMatDMatMult
{
 private:
   //**Type definitions****************************************************************************
   typedef typename MT1::OppositeType                    OMT1;  //!< Matrix type 1 with opposite storage order
   typedef typename MT2::OppositeType                    OMT2;  //!< Matrix type 2 with opposite storage order
   typedef typename MT1::TransposeType                   TMT1;  //!< Transpose matrix type 1
   typedef typename MT2::TransposeType                   TMT2;  //!< Transpose matrix type 2
   typedef typename blaze::MathTrait<MT1,MT2>::MultType  RE;    //!< Default result type
   typedef typename RE::OppositeType                     ORE;   //!< Default result type with opposite storage order
   typedef typename RE::TransposeType                    TRE;   //!< Transpose default result type
   typedef typename ORE::TransposeType                   TORE;  //!< Transpose default result type with opposite storage order
   //**********************************************************************************************

   //**Type definitions****************************************************************************
   typedef typename MT1::ElementType           ET1;     //!< Element type 1
   typedef typename MT2::ElementType           ET2;     //!< Element type 2
   typedef typename RE::ElementType            RET;     //!< Resulting element type
   typedef blaze::DynamicMatrix<ET1,false>     RT1;     //!< Reference type 1
   typedef blaze::CompressedMatrix<ET2,false>  RT2;     //!< Reference type 2
   typedef blaze::DynamicMatrix<RET,false>     DRRE;    //!< Dense reference result type
   typedef blaze::CompressedMatrix<RET,false>  SRRE;    //!< Sparse reference result type
   typedef typename DRRE::OppositeType         ODRRE;   //!< Dense reference result type with opposite storage order
   typedef typename SRRE::OppositeType         OSRRE;   //!< Sparse reference result type with opposite storage order
   typedef typename DRRE::TransposeType        TDRRE;   //!< Transpose dense reference result type
   typedef typename SRRE::TransposeType        TSRRE;   //!< Transpose sparse reference result type
   typedef typename ODRRE::TransposeType       TODRRE;  //!< Transpose dense reference result type with opposite storage order
   typedef typename OSRRE::TransposeType       TOSRRE;  //!< Transpose sparse reference result type with opposite storage order
   typedef RE                                  DRE;     //!< Dense result type
   typedef SRRE                                SRE;     //!< Sparse result type
   typedef ORE                                 ODRE;    //!< Dense result type with opposite storage order
   typedef OSRRE                               OSRE;    //!< Sparse result type with opposite storage order
   typedef TRE                                 TDRE;    //!< Transpose dense result type
   typedef TSRRE                               TSRE;    //!< Transpose sparse result type
   typedef TORE                                TODRE;   //!< Transpose dense result type with opposite storage order
   typedef TOSRRE                              TOSRE;   //!< Transpose sparse result type with opposite storage order
   //**********************************************************************************************

 public:
   //**Constructors********************************************************************************
   /*!\name Constructors */
   //@{
   explicit DMatDMatMult( const Creator<MT1>& creator1, const Creator<MT2>& creator2 );
   // No explicitly declared copy constructor.
   //@}
   //**********************************************************************************************

   //**Destructor**********************************************************************************
   // No explicitly declared destructor.
   //**********************************************************************************************

 private:
   //**Test functions******************************************************************************
   /*!\name Test functions */
   //@{
                          void testInitialStatus     ();
                          void testAssignment        ();
                          void testElementAccess     ();
                          void testBasicOperation    ();
                          void testNegatedOperation  ();
   template< typename T > void testScaledOperation   ( T scalar );
                          void testTransposeOperation();
                          void testAbsOperation      ();
   //@}
   //**********************************************************************************************

   //**Error detection functions*******************************************************************
   /*!\name Error detection functions */
   //@{
   template< typename LT, typename RT > void checkResults();
   template< typename LT, typename RT > void checkTransposeResults();
   //@}
   //**********************************************************************************************

   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   MT1   lhs_;     //!< The left-hand side dense matrix.
   MT2   rhs_;     //!< The right-hand side dense matrix.
   OMT1  olhs_;    //!< The left-hand side dense matrix with opposite storage order.
   OMT2  orhs_;    //!< The right-hand side dense matrix with opposite storage order.
   DRE   dres_;    //!< The dense result matrix.
   SRE   sres_;    //!< The sparse result matrix.
   ODRE  odres_;   //!< The dense result matrix with opposite storage order.
   OSRE  osres_;   //!< The sparse result matrix with opposite storage order.
   TDRE  tdres_;   //!< The transpose dense result matrix.
   TSRE  tsres_;   //!< The transpose sparse result matrix.
   TODRE todres_;  //!< The transpose dense result matrix with opposite storage order.
   TOSRE tosres_;  //!< The transpose sparse result matrix with opposite storage order.
   RT1   reflhs_;  //!< The reference left-hand side matrix.
   RT2   refrhs_;  //!< The reference right-hand side matrix.
   DRRE  refres_;  //!< The reference result.

   std::string test_;  //!< Label of the currently performed test.
   //@}
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( MT1    );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( MT2    );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( OMT1   );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( OMT2   );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( TMT1   );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( TMT2   );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( RT1    );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE( RT2    );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( DRRE   );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE( SRRE   );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( ODRRE  );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE( OSRRE  );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( TDRRE  );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE( TSRRE  );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( TODRRE );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE( TOSRRE );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( DRE    );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE( SRE    );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( ODRE   );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE( OSRE   );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( TDRE   );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE( TSRE   );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( TODRE  );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE( TOSRE  );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE   ( MT1    );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE   ( MT2    );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( OMT1   );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( OMT2   );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( TMT1   );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( TMT1   );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE   ( RT1    );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE   ( RT2    );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE   ( DRRE   );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE   ( SRRE   );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( ODRRE  );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( OSRRE  );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( TDRRE  );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( TSRRE  );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE   ( TODRRE );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE   ( TOSRRE );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE   ( DRE    );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE   ( SRE    );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( ODRE   );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( OSRE   );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( TDRE   );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( TSRE   );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE   ( TODRE  );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE   ( TOSRE  );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( ET1, typename OMT1::ElementType   );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( ET2, typename OMT2::ElementType   );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( ET1, typename TMT1::ElementType   );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( ET2, typename TMT2::ElementType   );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( MT1, typename OMT1::OppositeType  );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( MT2, typename OMT2::OppositeType  );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( MT1, typename TMT1::TransposeType );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( MT2, typename TMT2::TransposeType );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( RE , typename ORE::OppositeType   );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( RE , typename TRE::TransposeType  );
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************




//=================================================================================================
//
//  CONSTRUCTORS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Constructor for the DMatDMatMult class template.
//
// \param creator1 The creator for the left-hand side dense matrix of the matrix multiplication.
// \param creator2 The creator for the right-hand side dense matrix of the matrix multiplication.
// \exception std::runtime_error Operation error detected.
*/
template< typename MT1    // Type of the left-hand side dense matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
DMatDMatMult<MT1,MT2>::DMatDMatMult( const Creator<MT1>& creator1, const Creator<MT2>& creator2 )
   : lhs_( creator1() )  // The left-hand side dense matrix
   , rhs_( creator2() )  // The right-hand side dense matrix
   , olhs_( lhs_ )       // The left-hand side dense matrix with opposite storage order
   , orhs_( rhs_ )       // The right-hand side dense matrix with opposite storage order
   , dres_()             // The dense result matrix
   , sres_()             // The sparse result matrix
   , odres_()            // The dense result matrix with opposite storage order
   , osres_()            // The sparse result matrix with opposite storage order
   , tdres_()            // The transpose dense result matrix
   , tsres_()            // The transpose sparse result matrix
   , todres_()           // The transpose dense result matrix with opposite storage order
   , tosres_()           // The transpose sparse result matrix with opposite storage order
   , reflhs_( lhs_ )     // The reference left-hand side matrix
   , refrhs_( rhs_ )     // The reference right-hand side matrix
   , refres_()           // The reference result
   , test_()             // Label of the currently performed test
{
   testInitialStatus();
   testAssignment();
   testElementAccess();
   testBasicOperation();
   testNegatedOperation();
   testScaledOperation( 2 );
   testScaledOperation( 2UL );
   testScaledOperation( 1.1F );
   testScaledOperation( 1.1 );
   testTransposeOperation();
   testAbsOperation();
}
//*************************************************************************************************




//=================================================================================================
//
//  TEST FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Tests on the initial status of the matrices.
//
// \return void
// \exception std::runtime_error Initialization error detected.
//
// This function runs tests on the initial status of the matrices. In case any initialization
// error is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side dense matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void DMatDMatMult<MT1,MT2>::testInitialStatus()
{
   //=====================================================================================
   // Performing initial tests with the row-major types
   //=====================================================================================

   // Checking the number of rows of the left-hand side operand
   if( lhs_.rows() != reflhs_.rows() ) {
      std::ostringstream oss;
      oss << " Test: Initial size comparison of left-hand side row-major dense operand\n"
          << " Error: Invalid number of rows\n"
          << " Details:\n"
          << "   Row-major dense matrix type:\n"
          << "     " << typeid( MT1 ).name() << "\n"
          << "   Detected number of rows = " << lhs_.rows() << "\n"
          << "   Expected number of rows = " << reflhs_.rows() << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the number of columns of the left-hand side operand
   if( lhs_.columns() != reflhs_.columns() ) {
      std::ostringstream oss;
      oss << " Test: Initial size comparison of left-hand side row-major dense operand\n"
          << " Error: Invalid number of columns\n"
          << " Details:\n"
          << "   Row-major dense matrix type:\n"
          << "     " << typeid( MT1 ).name() << "\n"
          << "   Detected number of columns = " << lhs_.columns() << "\n"
          << "   Expected number of columns = " << reflhs_.columns() << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the number of rows of the right-hand side operand
   if( rhs_.rows() != refrhs_.rows() ) {
      std::ostringstream oss;
      oss << " Test: Initial size comparison of right-hand side row-major dense operand\n"
          << " Error: Invalid number of rows\n"
          << " Details:\n"
          << "   Row-major dense matrix type:\n"
          << "     " << typeid( MT2 ).name() << "\n"
          << "   Detected number of rows = " << rhs_.rows() << "\n"
          << "   Expected number of rows = " << refrhs_.rows() << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the number of columns of the right-hand side operand
   if( rhs_.columns() != refrhs_.columns() ) {
      std::ostringstream oss;
      oss << " Test: Initial size comparison of right-hand side row-major dense operand\n"
          << " Error: Invalid number of columns\n"
          << " Details:\n"
          << "   Row-major dense matrix type:\n"
          << "     " << typeid( MT2 ).name() << "\n"
          << "   Detected number of columns = " << rhs_.columns() << "\n"
          << "   Expected number of columns = " << refrhs_.columns() << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the initialization of the left-hand side operand
   if( !isEqual( lhs_, reflhs_ ) ) {
      std::ostringstream oss;
      oss << " Test: Initial test of initialization of left-hand side row-major dense operand\n"
          << " Error: Invalid matrix initialization\n"
          << " Details:\n"
          << "   Row-major dense matrix type:\n"
          << "     " << typeid( MT1 ).name() << "\n"
          << "   Current initialization:\n" << lhs_ << "\n"
          << "   Expected initialization:\n" << reflhs_ << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the initialization of the right-hand side operand
   if( !isEqual( rhs_, refrhs_ ) ) {
      std::ostringstream oss;
      oss << " Test: Initial test of initialization of right-hand side row-major dense operand\n"
          << " Error: Invalid matrix initialization\n"
          << " Details:\n"
          << "   Row-major dense matrix type:\n"
          << "     " << typeid( MT2 ).name() << "\n"
          << "   Current initialization:\n" << rhs_ << "\n"
          << "   Expected initialization:\n" << refrhs_ << "\n";
      throw std::runtime_error( oss.str() );
   }


   //=====================================================================================
   // Performing initial tests with the column-major types
   //=====================================================================================

   // Checking the number of rows of the left-hand side operand
   if( olhs_.rows() != reflhs_.rows() ) {
      std::ostringstream oss;
      oss << " Test: Initial size comparison of left-hand side column-major dense operand\n"
          << " Error: Invalid number of rows\n"
          << " Details:\n"
          << "   Column-major dense matrix type:\n"
          << "     " << typeid( OMT1 ).name() << "\n"
          << "   Detected number of rows = " << olhs_.rows() << "\n"
          << "   Expected number of rows = " << reflhs_.rows() << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the number of columns of the left-hand side operand
   if( olhs_.columns() != reflhs_.columns() ) {
      std::ostringstream oss;
      oss << " Test: Initial size comparison of left-hand side column-major dense operand\n"
          << " Error: Invalid number of columns\n"
          << " Details:\n"
          << "   Column-major dense matrix type:\n"
          << "     " << typeid( OMT1 ).name() << "\n"
          << "   Detected number of columns = " << olhs_.columns() << "\n"
          << "   Expected number of columns = " << reflhs_.columns() << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the number of rows of the right-hand side operand
   if( orhs_.rows() != refrhs_.rows() ) {
      std::ostringstream oss;
      oss << " Test: Initial size comparison of right-hand side column-major dense operand\n"
          << " Error: Invalid number of rows\n"
          << " Details:\n"
          << "   Column-major dense matrix type:\n"
          << "     " << typeid( OMT2 ).name() << "\n"
          << "   Detected number of rows = " << orhs_.rows() << "\n"
          << "   Expected number of rows = " << refrhs_.rows() << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the number of columns of the right-hand side operand
   if( orhs_.columns() != refrhs_.columns() ) {
      std::ostringstream oss;
      oss << " Test: Initial size comparison of right-hand side column-major dense operand\n"
          << " Error: Invalid number of columns\n"
          << " Details:\n"
          << "   Column-major dense matrix type:\n"
          << "     " << typeid( OMT2 ).name() << "\n"
          << "   Detected number of columns = " << orhs_.columns() << "\n"
          << "   Expected number of columns = " << refrhs_.columns() << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the initialization of the left-hand side operand
   if( !isEqual( olhs_, reflhs_ ) ) {
      std::ostringstream oss;
      oss << " Test: Initial test of initialization of left-hand side column-major dense operand\n"
          << " Error: Invalid matrix initialization\n"
          << " Details:\n"
          << "   Column-major dense matrix type:\n"
          << "     " << typeid( OMT1 ).name() << "\n"
          << "   Current initialization:\n" << olhs_ << "\n"
          << "   Expected initialization:\n" << reflhs_ << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the initialization of the right-hand side operand
   if( !isEqual( orhs_, refrhs_ ) ) {
      std::ostringstream oss;
      oss << " Test: Initial test of initialization of right-hand side column-major dense operand\n"
          << " Error: Invalid matrix initialization\n"
          << " Details:\n"
          << "   Column-major dense matrix type:\n"
          << "     " << typeid( OMT2 ).name() << "\n"
          << "   Current initialization:\n" << orhs_ << "\n"
          << "   Expected initialization:\n" << refrhs_ << "\n";
      throw std::runtime_error( oss.str() );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the matrix assignment.
//
// \return void
// \exception std::runtime_error Assignment error detected.
//
// This function tests the matrix assignment. In case any error is detected, a
// \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side dense matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void DMatDMatMult<MT1,MT2>::testAssignment()
{
   //=====================================================================================
   // // Performing an assignment with the row-major types
   //=====================================================================================

   try {
      lhs_ = reflhs_;
      rhs_ = refrhs_;
   }
   catch( std::exception& ex ) {
      std::ostringstream oss;
      oss << " Test: Assignment with the row-major types\n"
          << " Error: Failed assignment\n"
          << " Details:\n"
          << "   Left-hand side row-major dense matrix type:\n"
          << "     " << typeid( MT1 ).name() << "\n"
          << "   Right-hand side row-major dense matrix type:\n"
          << "     " << typeid( MT2 ).name() << "\n"
          << "   Error message: " << ex.what() << "\n";
      throw std::runtime_error( oss.str() );
   }

   if( !isEqual( lhs_, reflhs_ ) ) {
      std::ostringstream oss;
      oss << " Test: Checking the assignment result of left-hand side row-major dense operand\n"
          << " Error: Invalid matrix initialization\n"
          << " Details:\n"
          << "   Row-major dense matrix type:\n"
          << "     " << typeid( MT1 ).name() << "\n"
          << "   Current initialization:\n" << lhs_ << "\n"
          << "   Expected initialization:\n" << reflhs_ << "\n";
      throw std::runtime_error( oss.str() );
   }

   if( !isEqual( rhs_, refrhs_ ) ) {
      std::ostringstream oss;
      oss << " Test: Checking the assignment result of right-hand side row-major dense operand\n"
          << " Error: Invalid matrix initialization\n"
          << " Details:\n"
          << "   Row-major dense matrix type:\n"
          << "     " << typeid( MT2 ).name() << "\n"
          << "   Current initialization:\n" << rhs_ << "\n"
          << "   Expected initialization:\n" << refrhs_ << "\n";
      throw std::runtime_error( oss.str() );
   }


   //=====================================================================================
   // Performing an assignment with the column-major types
   //=====================================================================================

   try {
      olhs_ = reflhs_;
      orhs_ = refrhs_;
   }
   catch( std::exception& ex ) {
      std::ostringstream oss;
      oss << " Test: Assignment with the column-major types\n"
          << " Error: Failed assignment\n"
          << " Details:\n"
          << "   Left-hand side column-major dense matrix type:\n"
          << "     " << typeid( OMT1 ).name() << "\n"
          << "   Right-hand side column-major dense matrix type:\n"
          << "     "  << typeid( OMT2 ).name() << "\n"
          << "   Error message: " << ex.what() << "\n";
      throw std::runtime_error( oss.str() );
   }

   if( !isEqual( olhs_, reflhs_ ) ) {
      std::ostringstream oss;
      oss << " Test: Checking the assignment result of left-hand side column-major dense operand\n"
          << " Error: Invalid matrix initialization\n"
          << " Details:\n"
          << "   Column-major dense matrix type:\n"
          << "     " << typeid( OMT1 ).name() << "\n"
          << "   Current initialization:\n" << olhs_ << "\n"
          << "   Expected initialization:\n" << reflhs_ << "\n";
      throw std::runtime_error( oss.str() );
   }

   if( !isEqual( orhs_, refrhs_ ) ) {
      std::ostringstream oss;
      oss << " Test: Checking the assignment result of right-hand side column-major dense operand\n"
          << " Error: Invalid matrix initialization\n"
          << " Details:\n"
          << "   Column-major dense matrix type:\n"
          << "     " << typeid( OMT2 ).name() << "\n"
          << "   Current initialization:\n" << orhs_ << "\n"
          << "   Expected initialization:\n" << refrhs_ << "\n";
      throw std::runtime_error( oss.str() );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the matrix element access.
//
// \return void
// \exception std::runtime_error Element access error detected.
//
// This function tests the element access via the subscript operator. In case any
// error is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side dense matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void DMatDMatMult<MT1,MT2>::testElementAccess()
{
   using blaze::equal;


   //=====================================================================================
   // Testing the element access with two row-major matrices
   //=====================================================================================

   if( lhs_.rows() > 0UL && rhs_.columns() > 0UL )
   {
      if( !equal( ( lhs_ * rhs_ )(0UL,0UL), ( reflhs_ * refrhs_ )(0UL,0UL) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of multiplication expression\n"
             << " Error: Unequal resulting elements at element (0,0) detected\n"
             << " Details:\n"
             << "   Left-hand side row-major dense matrix type:\n"
             << "     " << typeid( MT1 ).name() << "\n"
             << "   Right-hand side row-major dense matrix type:\n"
             << "     " << typeid( MT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( lhs_ * eval( rhs_ ) )(0UL,0UL), ( reflhs_ * eval( refrhs_ ) )(0UL,0UL) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of right evaluated multiplication expression\n"
             << " Error: Unequal resulting elements at element (0,0) detected\n"
             << " Details:\n"
             << "   Left-hand side row-major dense matrix type:\n"
             << "     " << typeid( MT1 ).name() << "\n"
             << "   Right-hand side row-major dense matrix type:\n"
             << "     " << typeid( MT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( eval( lhs_ ) * rhs_ )(0UL,0UL), ( eval( reflhs_ ) * refrhs_ )(0UL,0UL) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of left evaluated multiplication expression\n"
             << " Error: Unequal resulting elements at element (0,0) detected\n"
             << " Details:\n"
             << "   Left-hand side row-major dense matrix type:\n"
             << "     " << typeid( MT1 ).name() << "\n"
             << "   Right-hand side row-major dense matrix type:\n"
             << "     " << typeid( MT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( eval( lhs_ ) * eval( rhs_ ) )(0UL,0UL), ( eval( reflhs_ ) * eval( refrhs_ ) )(0UL,0UL) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of fully evaluated multiplication expression\n"
             << " Error: Unequal resulting elements at element (0,0) detected\n"
             << " Details:\n"
             << "   Left-hand side row-major dense matrix type:\n"
             << "     " << typeid( MT1 ).name() << "\n"
             << "   Right-hand side row-major dense matrix type:\n"
             << "     " << typeid( MT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }
   }


   //=====================================================================================
   // Testing the element access with a row-major matrix and a column-major matrix
   //=====================================================================================

   if( lhs_.rows() > 0UL && orhs_.columns() > 0UL )
   {
      if( !equal( ( lhs_ * orhs_ )(0UL,0UL), ( reflhs_ * refrhs_ )(0UL,0UL) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of multiplication expression\n"
             << " Error: Unequal resulting elements at element (0,0) detected\n"
             << " Details:\n"
             << "   Left-hand side row-major dense matrix type:\n"
             << "     " << typeid( MT1 ).name() << "\n"
             << "   Right-hand side column-major dense matrix type:\n"
             << "     " << typeid( OMT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( lhs_ * eval( orhs_ ) )(0UL,0UL), ( reflhs_ * eval( refrhs_ ) )(0UL,0UL) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of right evaluated multiplication expression\n"
             << " Error: Unequal resulting elements at element (0,0) detected\n"
             << " Details:\n"
             << "   Left-hand side row-major dense matrix type:\n"
             << "     " << typeid( MT1 ).name() << "\n"
             << "   Right-hand side column-major dense matrix type:\n"
             << "     " << typeid( OMT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( eval( lhs_ ) * orhs_ )(0UL,0UL), ( eval( reflhs_ ) * refrhs_ )(0UL,0UL) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of left evaluated multiplication expression\n"
             << " Error: Unequal resulting elements at element (0,0) detected\n"
             << " Details:\n"
             << "   Left-hand side row-major dense matrix type:\n"
             << "     " << typeid( MT1 ).name() << "\n"
             << "   Right-hand side column-major dense matrix type:\n"
             << "     " << typeid( OMT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( eval( lhs_ ) * eval( orhs_ ) )(0UL,0UL), ( eval( reflhs_ ) * eval( refrhs_ ) )(0UL,0UL) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of fully evaluated multiplication expression\n"
             << " Error: Unequal resulting elements at element (0,0) detected\n"
             << " Details:\n"
             << "   Left-hand side row-major dense matrix type:\n"
             << "     " << typeid( MT1 ).name() << "\n"
             << "   Right-hand side column-major dense matrix type:\n"
             << "     " << typeid( OMT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }
   }


   //=====================================================================================
   // Testing the element access with a column-major matrix and a row-major matrix
   //=====================================================================================

   if( olhs_.rows() > 0UL && rhs_.columns() > 0UL )
   {
      if( !equal( ( olhs_ * rhs_ )(0UL,0UL), ( reflhs_ * refrhs_ )(0UL,0UL) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of multiplication expression\n"
             << " Error: Unequal resulting elements at element (0,0) detected\n"
             << " Details:\n"
             << "   Left-hand side column-major dense matrix type:\n"
             << "     " << typeid( OMT1 ).name() << "\n"
             << "   Right-hand side row-major dense matrix type:\n"
             << "     " << typeid( MT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( olhs_ * eval( rhs_ ) )(0UL,0UL), ( reflhs_ * eval( refrhs_ ) )(0UL,0UL) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of right evaluated multiplication expression\n"
             << " Error: Unequal resulting elements at element (0,0) detected\n"
             << " Details:\n"
             << "   Left-hand side column-major dense matrix type:\n"
             << "     " << typeid( OMT1 ).name() << "\n"
             << "   Right-hand side row-major dense matrix type:\n"
             << "     " << typeid( MT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( eval( olhs_ ) * rhs_ )(0UL,0UL), ( eval( reflhs_ ) * refrhs_ )(0UL,0UL) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of left evaluated multiplication expression\n"
             << " Error: Unequal resulting elements at element (0,0) detected\n"
             << " Details:\n"
             << "   Left-hand side column-major dense matrix type:\n"
             << "     " << typeid( OMT1 ).name() << "\n"
             << "   Right-hand side row-major dense matrix type:\n"
             << "     " << typeid( MT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( eval( olhs_ ) * eval( rhs_ ) )(0UL,0UL), ( eval( reflhs_ ) * eval( refrhs_ ) )(0UL,0UL) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of fully evaluated multiplication expression\n"
             << " Error: Unequal resulting elements at element (0,0) detected\n"
             << " Details:\n"
             << "   Left-hand side column-major dense matrix type:\n"
             << "     " << typeid( OMT1 ).name() << "\n"
             << "   Right-hand side row-major dense matrix type:\n"
             << "     " << typeid( MT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }
   }


   //=====================================================================================
   // Testing the element access with two column-major matrices
   //=====================================================================================

   if( olhs_.rows() > 0UL && orhs_.columns() > 0UL )
   {
      if( !equal( ( olhs_ * orhs_ )(0UL,0UL), ( reflhs_ * refrhs_ )(0UL,0UL) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of transpose multiplication expression\n"
             << " Error: Unequal resulting elements at element (0,0) detected\n"
             << " Details:\n"
             << "   Transpose left-hand side dense matrix type:\n"
             << "     " << typeid( OMT1 ).name() << "\n"
             << "   Transpose right-hand side dense matrix type:\n"
             << "     " << typeid( OMT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( olhs_ * eval( orhs_ ) )(0UL,0UL), ( reflhs_ * eval( refrhs_ ) )(0UL,0UL) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of right evaluated transpose multiplication expression\n"
             << " Error: Unequal resulting elements at element (0,0) detected\n"
             << " Details:\n"
             << "   Transpose left-hand side dense matrix type:\n"
             << "     " << typeid( OMT1 ).name() << "\n"
             << "   Transpose right-hand side dense matrix type:\n"
             << "     " << typeid( OMT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( eval( olhs_ ) * orhs_ )(0UL,0UL), ( eval( reflhs_ ) * refrhs_ )(0UL,0UL) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of left evaluated transpose multiplication expression\n"
             << " Error: Unequal resulting elements at element (0,0) detected\n"
             << " Details:\n"
             << "   Transpose left-hand side dense matrix type:\n"
             << "     " << typeid( OMT1 ).name() << "\n"
             << "   Transpose right-hand side dense matrix type:\n"
             << "     " << typeid( OMT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( eval( olhs_ ) * eval( orhs_ ) )(0UL,0UL), ( eval( reflhs_ ) * eval( refrhs_ ) )(0UL,0UL) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of fully evaluated transpose multiplication expression\n"
             << " Error: Unequal resulting elements at element (0,0) detected\n"
             << " Details:\n"
             << "   Transpose left-hand side dense matrix type:\n"
             << "     " << typeid( OMT1 ).name() << "\n"
             << "   Transpose right-hand side dense matrix type:\n"
             << "     " << typeid( OMT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the plain dense matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the plain matrix multiplication with plain assignment, addition
// assignment, and subtraction assignment. In case any error resulting from the multiplication
// or the subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side dense matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void DMatDMatMult<MT1,MT2>::testBasicOperation()
{
#if BLAZETEST_MATHTEST_TEST_BASIC_OPERATION
   if( BLAZETEST_MATHTEST_TEST_BASIC_OPERATION > 1 )
   {
      //=====================================================================================
      // Multiplication
      //=====================================================================================

      // Multiplication with the given matrices
      {
         test_ = "Multiplication with the given matrices";

         try {
            dres_   = lhs_ * rhs_;
            odres_  = lhs_ * rhs_;
            sres_   = lhs_ * rhs_;
            osres_  = lhs_ * rhs_;
            refres_ = reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_  = lhs_ * orhs_;
            odres_ = lhs_ * orhs_;
            sres_  = lhs_ * orhs_;
            osres_ = lhs_ * orhs_;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_  = olhs_ * rhs_;
            odres_ = olhs_ * rhs_;
            sres_  = olhs_ * rhs_;
            osres_ = olhs_ * rhs_;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2  ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_  = olhs_ * orhs_;
            odres_ = olhs_ * orhs_;
            sres_  = olhs_ * orhs_;
            osres_ = olhs_ * orhs_;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }

      // Multiplication with evaluated matrices
      {
         test_ = "Multiplication with evaluated matrices";

         try {
            dres_  = eval( lhs_ ) * eval( rhs_ );
            odres_ = eval( lhs_ ) * eval( rhs_ );
            sres_  = eval( lhs_ ) * eval( rhs_ );
            osres_ = eval( lhs_ ) * eval( rhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_  = eval( lhs_ ) * eval( orhs_ );
            odres_ = eval( lhs_ ) * eval( orhs_ );
            sres_  = eval( lhs_ ) * eval( orhs_ );
            osres_ = eval( lhs_ ) * eval( orhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_  = eval( olhs_ ) * eval( rhs_ );
            odres_ = eval( olhs_ ) * eval( rhs_ );
            sres_  = eval( olhs_ ) * eval( rhs_ );
            osres_ = eval( olhs_ ) * eval( rhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2  ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_  = eval( olhs_ ) * eval( orhs_ );
            odres_ = eval( olhs_ ) * eval( orhs_ );
            sres_  = eval( olhs_ ) * eval( orhs_ );
            osres_ = eval( olhs_ ) * eval( orhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Multiplication with addition assignment
      //=====================================================================================

      // Multiplication with addition assignment with the given matrices
      {
         test_ = "Multiplication with addition assignment with the given matrices";

         try {
            dres_   += lhs_ * rhs_;
            odres_  += lhs_ * rhs_;
            sres_   += lhs_ * rhs_;
            osres_  += lhs_ * rhs_;
            refres_ += reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_   += lhs_ * orhs_;
            odres_  += lhs_ * orhs_;
            sres_   += lhs_ * orhs_;
            osres_  += lhs_ * orhs_;
            refres_ += reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   += olhs_ * rhs_;
            odres_  += olhs_ * rhs_;
            sres_   += olhs_ * rhs_;
            osres_  += olhs_ * rhs_;
            refres_ += reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_   += olhs_ * orhs_;
            odres_  += olhs_ * orhs_;
            sres_   += olhs_ * orhs_;
            osres_  += olhs_ * orhs_;
            refres_ += reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }

      // Multiplication with addition assignment with evaluated matrices
      {
         test_ = "Multiplication with addition assignment with evaluated matrices";

         try {
            dres_   += eval( lhs_ ) * eval( rhs_ );
            odres_  += eval( lhs_ ) * eval( rhs_ );
            sres_   += eval( lhs_ ) * eval( rhs_ );
            osres_  += eval( lhs_ ) * eval( rhs_ );
            refres_ += eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_   += eval( lhs_ ) * eval( orhs_ );
            odres_  += eval( lhs_ ) * eval( orhs_ );
            sres_   += eval( lhs_ ) * eval( orhs_ );
            osres_  += eval( lhs_ ) * eval( orhs_ );
            refres_ += eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   += eval( olhs_ ) * eval( rhs_ );
            odres_  += eval( olhs_ ) * eval( rhs_ );
            sres_   += eval( olhs_ ) * eval( rhs_ );
            osres_  += eval( olhs_ ) * eval( rhs_ );
            refres_ += eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_   += eval( olhs_ ) * eval( orhs_ );
            odres_  += eval( olhs_ ) * eval( orhs_ );
            sres_   += eval( olhs_ ) * eval( orhs_ );
            osres_  += eval( olhs_ ) * eval( orhs_ );
            refres_ += eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Multiplication with subtraction assignment with the given matrices
      //=====================================================================================

      // Multiplication with subtraction assignment with the given matrices
      {
         test_ = "Multiplication with subtraction assignment with the given matrices";

         try {
            dres_   -= lhs_ * rhs_;
            odres_  -= lhs_ * rhs_;
            sres_   -= lhs_ * rhs_;
            osres_  -= lhs_ * rhs_;
            refres_ -= reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_   -= lhs_ * orhs_;
            odres_  -= lhs_ * orhs_;
            sres_   -= lhs_ * orhs_;
            osres_  -= lhs_ * orhs_;
            refres_ -= reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   -= olhs_ * rhs_;
            odres_  -= olhs_ * rhs_;
            sres_   -= olhs_ * rhs_;
            osres_  -= olhs_ * rhs_;
            refres_ -= reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_   -= olhs_ * orhs_;
            odres_  -= olhs_ * orhs_;
            sres_   -= olhs_ * orhs_;
            osres_  -= olhs_ * orhs_;
            refres_ -= reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }

      // Multiplication with subtraction assignment with evaluated matrices
      {
         test_ = "Multiplication with subtraction assignment with evaluated matrices";

         try {
            dres_   -= eval( lhs_ ) * eval( rhs_ );
            odres_  -= eval( lhs_ ) * eval( rhs_ );
            sres_   -= eval( lhs_ ) * eval( rhs_ );
            osres_  -= eval( lhs_ ) * eval( rhs_ );
            refres_ -= eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_   -= eval( lhs_ ) * eval( orhs_ );
            odres_  -= eval( lhs_ ) * eval( orhs_ );
            sres_   -= eval( lhs_ ) * eval( orhs_ );
            osres_  -= eval( lhs_ ) * eval( orhs_ );
            refres_ -= eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   -= eval( olhs_ ) * eval( rhs_ );
            odres_  -= eval( olhs_ ) * eval( rhs_ );
            sres_   -= eval( olhs_ ) * eval( rhs_ );
            osres_  -= eval( olhs_ ) * eval( rhs_ );
            refres_ -= eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_   -= eval( olhs_ ) * eval( orhs_ );
            odres_  -= eval( olhs_ ) * eval( orhs_ );
            sres_   -= eval( olhs_ ) * eval( orhs_ );
            osres_  -= eval( olhs_ ) * eval( orhs_ );
            refres_ -= eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the negated dense matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the negated matrix multiplication with plain assignment, addition
// assignment, and subtraction assignment. In case any error resulting from the multiplication
// or the subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side dense matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void DMatDMatMult<MT1,MT2>::testNegatedOperation()
{
#if BLAZETEST_MATHTEST_TEST_NEGATED_OPERATION
   if( BLAZETEST_MATHTEST_TEST_NEGATED_OPERATION > 1 )
   {
      //=====================================================================================
      // Negated multiplication
      //=====================================================================================

      // Negated multiplication with the given matrices
      {
         test_ = "Negated multiplication with the given matrices";

         try {
            dres_   = -( lhs_ * rhs_ );
            odres_  = -( lhs_ * rhs_ );
            sres_   = -( lhs_ * rhs_ );
            osres_  = -( lhs_ * rhs_ );
            refres_ = -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_  = -( lhs_ * orhs_ );
            odres_ = -( lhs_ * orhs_ );
            sres_  = -( lhs_ * orhs_ );
            osres_ = -( lhs_ * orhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_  = -( olhs_ * rhs_ );
            odres_ = -( olhs_ * rhs_ );
            sres_  = -( olhs_ * rhs_ );
            osres_ = -( olhs_ * rhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_  = -( olhs_ * orhs_ );
            odres_ = -( olhs_ * orhs_ );
            sres_  = -( olhs_ * orhs_ );
            osres_ = -( olhs_ * orhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }

      // Negated multiplication with evaluated matrices
      {
         test_ = "Negated multiplication with evaluated matrices";

         try {
            dres_  = -( eval( lhs_ ) * eval( rhs_ ) );
            odres_ = -( eval( lhs_ ) * eval( rhs_ ) );
            sres_  = -( eval( lhs_ ) * eval( rhs_ ) );
            osres_ = -( eval( lhs_ ) * eval( rhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_  = -( eval( lhs_ ) * eval( orhs_ ) );
            odres_ = -( eval( lhs_ ) * eval( orhs_ ) );
            sres_  = -( eval( lhs_ ) * eval( orhs_ ) );
            osres_ = -( eval( lhs_ ) * eval( orhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_  = -( eval( olhs_ ) * eval( rhs_ ) );
            odres_ = -( eval( olhs_ ) * eval( rhs_ ) );
            sres_  = -( eval( olhs_ ) * eval( rhs_ ) );
            osres_ = -( eval( olhs_ ) * eval( rhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_  = -( eval( olhs_ ) * eval( orhs_ ) );
            odres_ = -( eval( olhs_ ) * eval( orhs_ ) );
            sres_  = -( eval( olhs_ ) * eval( orhs_ ) );
            osres_ = -( eval( olhs_ ) * eval( orhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Negated multiplication with addition assignment
      //=====================================================================================

      // Negated multiplication with addition assignment with the given matrices
      {
         test_ = "Negated multiplication with addition assignment with the given matrices";

         try {
            dres_   += -( lhs_ * rhs_ );
            odres_  += -( lhs_ * rhs_ );
            sres_   += -( lhs_ * rhs_ );
            osres_  += -( lhs_ * rhs_ );
            refres_ += -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_   += -( lhs_ * orhs_ );
            odres_  += -( lhs_ * orhs_ );
            sres_   += -( lhs_ * orhs_ );
            osres_  += -( lhs_ * orhs_ );
            refres_ += -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   += -( olhs_ * rhs_ );
            odres_  += -( olhs_ * rhs_ );
            sres_   += -( olhs_ * rhs_ );
            osres_  += -( olhs_ * rhs_ );
            refres_ += -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_   += -( olhs_ * orhs_ );
            odres_  += -( olhs_ * orhs_ );
            sres_   += -( olhs_ * orhs_ );
            osres_  += -( olhs_ * orhs_ );
            refres_ += -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }

      // Negated multiplication with addition assignment with the given matrices
      {
         test_ = "Negated multiplication with addition assignment with evaluated matrices";

         try {
            dres_   += -( eval( lhs_ ) * eval( rhs_ ) );
            odres_  += -( eval( lhs_ ) * eval( rhs_ ) );
            sres_   += -( eval( lhs_ ) * eval( rhs_ ) );
            osres_  += -( eval( lhs_ ) * eval( rhs_ ) );
            refres_ += -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_   += -( eval( lhs_ ) * eval( orhs_ ) );
            odres_  += -( eval( lhs_ ) * eval( orhs_ ) );
            sres_   += -( eval( lhs_ ) * eval( orhs_ ) );
            osres_  += -( eval( lhs_ ) * eval( orhs_ ) );
            refres_ += -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   += -( eval( olhs_ ) * eval( rhs_ ) );
            odres_  += -( eval( olhs_ ) * eval( rhs_ ) );
            sres_   += -( eval( olhs_ ) * eval( rhs_ ) );
            osres_  += -( eval( olhs_ ) * eval( rhs_ ) );
            refres_ += -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_   += -( eval( olhs_ ) * eval( orhs_ ) );
            odres_  += -( eval( olhs_ ) * eval( orhs_ ) );
            sres_   += -( eval( olhs_ ) * eval( orhs_ ) );
            osres_  += -( eval( olhs_ ) * eval( orhs_ ) );
            refres_ += -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Negated multiplication with subtraction assignment
      //=====================================================================================

      // Negated multiplication with subtraction assignment with the given matrices
      {
         test_ = "Negated multiplication with subtraction assignment with the given matrices";

         try {
            dres_   -= -( lhs_ * rhs_ );
            odres_  -= -( lhs_ * rhs_ );
            sres_   -= -( lhs_ * rhs_ );
            osres_  -= -( lhs_ * rhs_ );
            refres_ -= -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_   -= -( lhs_ * orhs_ );
            odres_  -= -( lhs_ * orhs_ );
            sres_   -= -( lhs_ * orhs_ );
            osres_  -= -( lhs_ * orhs_ );
            refres_ -= -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   -= -( olhs_ * rhs_ );
            odres_  -= -( olhs_ * rhs_ );
            sres_   -= -( olhs_ * rhs_ );
            osres_  -= -( olhs_ * rhs_ );
            refres_ -= -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_   -= -( olhs_ * orhs_ );
            odres_  -= -( olhs_ * orhs_ );
            sres_   -= -( olhs_ * orhs_ );
            osres_  -= -( olhs_ * orhs_ );
            refres_ -= -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }

      // Negated multiplication with subtraction assignment with evaluated matrices
      {
         test_ = "Negated multiplication with subtraction assignment with evaluated matrices";

         try {
            dres_   -= -( eval( lhs_ ) * eval( rhs_ ) );
            odres_  -= -( eval( lhs_ ) * eval( rhs_ ) );
            sres_   -= -( eval( lhs_ ) * eval( rhs_ ) );
            osres_  -= -( eval( lhs_ ) * eval( rhs_ ) );
            refres_ -= -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_   -= -( eval( lhs_ ) * eval( orhs_ ) );
            odres_  -= -( eval( lhs_ ) * eval( orhs_ ) );
            sres_   -= -( eval( lhs_ ) * eval( orhs_ ) );
            osres_  -= -( eval( lhs_ ) * eval( orhs_ ) );
            refres_ -= -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication with subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   -= -( eval( olhs_ ) * eval( rhs_ ) );
            odres_  -= -( eval( olhs_ ) * eval( rhs_ ) );
            sres_   -= -( eval( olhs_ ) * eval( rhs_ ) );
            osres_  -= -( eval( olhs_ ) * eval( rhs_ ) );
            refres_ -= -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_   -= -( eval( olhs_ ) * eval( orhs_ ) );
            odres_  -= -( eval( olhs_ ) * eval( orhs_ ) );
            sres_   -= -( eval( olhs_ ) * eval( orhs_ ) );
            osres_  -= -( eval( olhs_ ) * eval( orhs_ ) );
            refres_ -= -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the scaled dense matrix/dense matrix multiplication.
//
// \param scalar The scalar value.
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the scaled matrix multiplication with plain assignment, addition
// assignment, and subtraction assignment. In case any error resulting from the multiplication
// or the subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side dense matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
template< typename T >    // Type of the scalar
void DMatDMatMult<MT1,MT2>::testScaledOperation( T scalar )
{
   BLAZE_CONSTRAINT_MUST_BE_NUMERIC_TYPE( T );

   if( scalar == T(0) )
      throw std::invalid_argument( "Invalid scalar parameter" );


#if BLAZETEST_MATHTEST_TEST_SCALED_OPERATION
   if( BLAZETEST_MATHTEST_TEST_SCALED_OPERATION > 1 )
   {
      //=====================================================================================
      // Self-scaling (OP*=s)
      //=====================================================================================

      // Self-scaling (OP*=s)
      {
         test_ = "Self-scaling (OP*=s)";

         try {
            dres_   = lhs_ * rhs_;
            odres_  = dres_;
            sres_   = dres_;
            osres_  = dres_;
            refres_ = dres_;

            dres_   *= scalar;
            odres_  *= scalar;
            sres_   *= scalar;
            osres_  *= scalar;
            refres_ *= scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed self-scaling operation\n"
                << " Details:\n"
                << "   Scalar = " << scalar << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();
      }


      //=====================================================================================
      // Self-scaling (OP/=s)
      //=====================================================================================

      // Self-scaling (OP/=s)
      {
         test_ = "Self-scaling (OP/=s)";

         try {
            dres_   = lhs_ * rhs_;
            odres_  = dres_;
            sres_   = dres_;
            osres_  = dres_;
            refres_ = dres_;

            dres_   /= scalar;
            odres_  /= scalar;
            sres_   /= scalar;
            osres_  /= scalar;
            refres_ /= scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed self-scaling operation\n"
                << " Details:\n"
                << "   Scalar = " << scalar << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();
      }


      //=====================================================================================
      // Scaled multiplication (s*OP)
      //=====================================================================================

      // Scaled multiplication with the given matrices
      {
         test_ = "Scaled multiplication with the given matrices (s*OP)";

         try {
            dres_   = scalar * ( lhs_ * rhs_ );
            odres_  = scalar * ( lhs_ * rhs_ );
            sres_   = scalar * ( lhs_ * rhs_ );
            osres_  = scalar * ( lhs_ * rhs_ );
            refres_ = scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_  = scalar * ( lhs_ * orhs_ );
            odres_ = scalar * ( lhs_ * orhs_ );
            sres_  = scalar * ( lhs_ * orhs_ );
            osres_ = scalar * ( lhs_ * orhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_  = scalar * ( olhs_ * rhs_ );
            odres_ = scalar * ( olhs_ * rhs_ );
            sres_  = scalar * ( olhs_ * rhs_ );
            osres_ = scalar * ( olhs_ * rhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_  = scalar * ( olhs_ * orhs_ );
            odres_ = scalar * ( olhs_ * orhs_ );
            sres_  = scalar * ( olhs_ * orhs_ );
            osres_ = scalar * ( olhs_ * orhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }

      // Scaled multiplication with evaluated matrices
      {
         test_ = "Scaled multiplication with evaluated matrices (s*OP)";

         try {
            dres_  = scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            odres_ = scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            sres_  = scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            osres_ = scalar * ( eval( lhs_ ) * eval( rhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_  = scalar * ( eval( lhs_ ) * eval( orhs_ ) );
            odres_ = scalar * ( eval( lhs_ ) * eval( orhs_ ) );
            sres_  = scalar * ( eval( lhs_ ) * eval( orhs_ ) );
            osres_ = scalar * ( eval( lhs_ ) * eval( orhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_  = scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            odres_ = scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            sres_  = scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            osres_ = scalar * ( eval( olhs_ ) * eval( rhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_  = scalar * ( eval( olhs_ ) * eval( orhs_ ) );
            odres_ = scalar * ( eval( olhs_ ) * eval( orhs_ ) );
            sres_  = scalar * ( eval( olhs_ ) * eval( orhs_ ) );
            osres_ = scalar * ( eval( olhs_ ) * eval( orhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Scaled multiplication (OP*s)
      //=====================================================================================

      // Scaled multiplication with the given matrices
      {
         test_ = "Scaled multiplication with the given matrices (OP*s)";

         try {
            dres_   = ( lhs_ * rhs_ ) * scalar;
            odres_  = ( lhs_ * rhs_ ) * scalar;
            sres_   = ( lhs_ * rhs_ ) * scalar;
            osres_  = ( lhs_ * rhs_ ) * scalar;
            refres_ = ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_  = ( lhs_ * orhs_ ) * scalar;
            odres_ = ( lhs_ * orhs_ ) * scalar;
            sres_  = ( lhs_ * orhs_ ) * scalar;
            osres_ = ( lhs_ * orhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_  = ( olhs_ * rhs_ ) * scalar;
            odres_ = ( olhs_ * rhs_ ) * scalar;
            sres_  = ( olhs_ * rhs_ ) * scalar;
            osres_ = ( olhs_ * rhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_  = ( olhs_ * orhs_ ) * scalar;
            odres_ = ( olhs_ * orhs_ ) * scalar;
            sres_  = ( olhs_ * orhs_ ) * scalar;
            osres_ = ( olhs_ * orhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }

      // Scaled multiplication with evaluated matrices
      {
         test_ = "Scaled multiplication with evaluated matrices (OP*s)";

         try {
            dres_  = ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            odres_ = ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            sres_  = ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            osres_ = ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_  = ( eval( lhs_ ) * eval( orhs_ ) ) * scalar;
            odres_ = ( eval( lhs_ ) * eval( orhs_ ) ) * scalar;
            sres_  = ( eval( lhs_ ) * eval( orhs_ ) ) * scalar;
            osres_ = ( eval( lhs_ ) * eval( orhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_  = ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            odres_ = ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            sres_  = ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            osres_ = ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_  = ( eval( olhs_ ) * eval( orhs_ ) ) * scalar;
            odres_ = ( eval( olhs_ ) * eval( orhs_ ) ) * scalar;
            sres_  = ( eval( olhs_ ) * eval( orhs_ ) ) * scalar;
            osres_ = ( eval( olhs_ ) * eval( orhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Scaled multiplication (OP/s)
      //=====================================================================================

      // Scaled multiplication with the given matrices
      {
         test_ = "Scaled multiplication with the given matrices (OP/s)";

         try {
            dres_   = ( lhs_ * rhs_ ) / scalar;
            odres_  = ( lhs_ * rhs_ ) / scalar;
            sres_   = ( lhs_ * rhs_ ) / scalar;
            osres_  = ( lhs_ * rhs_ ) / scalar;
            refres_ = ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_  = ( lhs_ * orhs_ ) / scalar;
            odres_ = ( lhs_ * orhs_ ) / scalar;
            sres_  = ( lhs_ * orhs_ ) / scalar;
            osres_ = ( lhs_ * orhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_  = ( olhs_ * rhs_ ) / scalar;
            odres_ = ( olhs_ * rhs_ ) / scalar;
            sres_  = ( olhs_ * rhs_ ) / scalar;
            osres_ = ( olhs_ * rhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_  = ( olhs_ * orhs_ ) / scalar;
            odres_ = ( olhs_ * orhs_ ) / scalar;
            sres_  = ( olhs_ * orhs_ ) / scalar;
            osres_ = ( olhs_ * orhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }

      // Scaled multiplication with evaluated matrices
      {
         test_ = "Scaled multiplication with evaluated matrices (OP/s)";

         try {
            dres_  = ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            odres_ = ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            sres_  = ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            osres_ = ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_  = ( eval( lhs_ ) * eval( orhs_ ) ) / scalar;
            odres_ = ( eval( lhs_ ) * eval( orhs_ ) ) / scalar;
            sres_  = ( eval( lhs_ ) * eval( orhs_ ) ) / scalar;
            osres_ = ( eval( lhs_ ) * eval( orhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_  = ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            odres_ = ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            sres_  = ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            osres_ = ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_  = ( eval( olhs_ ) * eval( orhs_ ) ) / scalar;
            odres_ = ( eval( olhs_ ) * eval( orhs_ ) ) / scalar;
            sres_  = ( eval( olhs_ ) * eval( orhs_ ) ) / scalar;
            osres_ = ( eval( olhs_ ) * eval( orhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Scaled multiplication with addition assignment (s*OP)
      //=====================================================================================

      // Scaled multiplication with addition assignment with the given matrices
      {
         test_ = "Scaled multiplication with addition assignment with the given matrices (s*OP)";

         try {
            dres_   += scalar * ( lhs_ * rhs_ );
            odres_  += scalar * ( lhs_ * rhs_ );
            sres_   += scalar * ( lhs_ * rhs_ );
            osres_  += scalar * ( lhs_ * rhs_ );
            refres_ += scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_   += scalar * ( lhs_ * orhs_ );
            odres_  += scalar * ( lhs_ * orhs_ );
            sres_   += scalar * ( lhs_ * orhs_ );
            osres_  += scalar * ( lhs_ * orhs_ );
            refres_ += scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   += scalar * ( olhs_ * rhs_ );
            odres_  += scalar * ( olhs_ * rhs_ );
            sres_   += scalar * ( olhs_ * rhs_ );
            osres_  += scalar * ( olhs_ * rhs_ );
            refres_ += scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_   += scalar * ( olhs_ * orhs_ );
            odres_  += scalar * ( olhs_ * orhs_ );
            sres_   += scalar * ( olhs_ * orhs_ );
            osres_  += scalar * ( olhs_ * orhs_ );
            refres_ += scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }

      // Scaled multiplication with addition assignment with evaluated matrices
      {
         test_ = "Scaled multiplication with addition assignment with evaluated matrices (s*OP)";

         try {
            dres_   += scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            odres_  += scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            sres_   += scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            osres_  += scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            refres_ += scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_   += scalar * ( eval( lhs_ ) * eval( orhs_ ) );
            odres_  += scalar * ( eval( lhs_ ) * eval( orhs_ ) );
            sres_   += scalar * ( eval( lhs_ ) * eval( orhs_ ) );
            osres_  += scalar * ( eval( lhs_ ) * eval( orhs_ ) );
            refres_ += scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   += scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            odres_  += scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            sres_   += scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            osres_  += scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            refres_ += scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_   += scalar * ( eval( olhs_ ) * eval( orhs_ ) );
            odres_  += scalar * ( eval( olhs_ ) * eval( orhs_ ) );
            sres_   += scalar * ( eval( olhs_ ) * eval( orhs_ ) );
            osres_  += scalar * ( eval( olhs_ ) * eval( orhs_ ) );
            refres_ += scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Scaled multiplication with addition assignment (OP*s)
      //=====================================================================================

      // Scaled multiplication with addition assignment with the given matrices
      {
         test_ = "Scaled multiplication with addition assignment with the given matrices (OP*s)";

         try {
            dres_   += ( lhs_ * rhs_ ) * scalar;
            odres_  += ( lhs_ * rhs_ ) * scalar;
            sres_   += ( lhs_ * rhs_ ) * scalar;
            osres_  += ( lhs_ * rhs_ ) * scalar;
            refres_ += ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_   += ( lhs_ * orhs_ ) * scalar;
            odres_  += ( lhs_ * orhs_ ) * scalar;
            sres_   += ( lhs_ * orhs_ ) * scalar;
            osres_  += ( lhs_ * orhs_ ) * scalar;
            refres_ += ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   += ( olhs_ * rhs_ ) * scalar;
            odres_  += ( olhs_ * rhs_ ) * scalar;
            sres_   += ( olhs_ * rhs_ ) * scalar;
            osres_  += ( olhs_ * rhs_ ) * scalar;
            refres_ += ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_   += ( olhs_ * orhs_ ) * scalar;
            odres_  += ( olhs_ * orhs_ ) * scalar;
            sres_   += ( olhs_ * orhs_ ) * scalar;
            osres_  += ( olhs_ * orhs_ ) * scalar;
            refres_ += ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }

      // Scaled multiplication with addition assignment with evaluated matrices
      {
         test_ = "Scaled multiplication with addition assignment with evaluated matrices (OP*s)";

         try {
            dres_   += ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            odres_  += ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            sres_   += ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            osres_  += ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            refres_ += ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_   += ( eval( lhs_ ) * eval( orhs_ ) ) * scalar;
            odres_  += ( eval( lhs_ ) * eval( orhs_ ) ) * scalar;
            sres_   += ( eval( lhs_ ) * eval( orhs_ ) ) * scalar;
            osres_  += ( eval( lhs_ ) * eval( orhs_ ) ) * scalar;
            refres_ += ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   += ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            odres_  += ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            sres_   += ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            osres_  += ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            refres_ += ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_   += ( eval( olhs_ ) * eval( orhs_ ) ) * scalar;
            odres_  += ( eval( olhs_ ) * eval( orhs_ ) ) * scalar;
            sres_   += ( eval( olhs_ ) * eval( orhs_ ) ) * scalar;
            osres_  += ( eval( olhs_ ) * eval( orhs_ ) ) * scalar;
            refres_ += ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Scaled multiplication with addition assignment (OP/s)
      //=====================================================================================

      // Scaled multiplication with addition assignment with the given matrices
      {
         test_ = "Scaled multiplication with addition assignment with the given matrices (OP/s)";

         try {
            dres_   += ( lhs_ * rhs_ ) / scalar;
            odres_  += ( lhs_ * rhs_ ) / scalar;
            sres_   += ( lhs_ * rhs_ ) / scalar;
            osres_  += ( lhs_ * rhs_ ) / scalar;
            refres_ += ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_   += ( lhs_ * orhs_ ) / scalar;
            odres_  += ( lhs_ * orhs_ ) / scalar;
            sres_   += ( lhs_ * orhs_ ) / scalar;
            osres_  += ( lhs_ * orhs_ ) / scalar;
            refres_ += ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   += ( olhs_ * rhs_ ) / scalar;
            odres_  += ( olhs_ * rhs_ ) / scalar;
            sres_   += ( olhs_ * rhs_ ) / scalar;
            osres_  += ( olhs_ * rhs_ ) / scalar;
            refres_ += ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_   += ( olhs_ * orhs_ ) / scalar;
            odres_  += ( olhs_ * orhs_ ) / scalar;
            sres_   += ( olhs_ * orhs_ ) / scalar;
            osres_  += ( olhs_ * orhs_ ) / scalar;
            refres_ += ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }

      // Scaled multiplication with addition assignment with evaluated matrices
      {
         test_ = "Scaled multiplication with addition assignment with evaluated matrices (OP/s)";

         try {
            dres_   += ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            odres_  += ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            sres_   += ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            osres_  += ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            refres_ += ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_   += ( eval( lhs_ ) * eval( orhs_ ) ) / scalar;
            odres_  += ( eval( lhs_ ) * eval( orhs_ ) ) / scalar;
            sres_   += ( eval( lhs_ ) * eval( orhs_ ) ) / scalar;
            osres_  += ( eval( lhs_ ) * eval( orhs_ ) ) / scalar;
            refres_ += ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   += ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            odres_  += ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            sres_   += ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            osres_  += ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            refres_ += ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_   += ( eval( olhs_ ) * eval( orhs_ ) ) / scalar;
            odres_  += ( eval( olhs_ ) * eval( orhs_ ) ) / scalar;
            sres_   += ( eval( olhs_ ) * eval( orhs_ ) ) / scalar;
            osres_  += ( eval( olhs_ ) * eval( orhs_ ) ) / scalar;
            refres_ += ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Scaled multiplication with subtraction assignment (s*OP)
      //=====================================================================================

      // Scaled multiplication with subtraction assignment with the given matrices
      {
         test_ = "Scaled multiplication with subtraction assignment with the given matrices (s*OP)";

         try {
            dres_   -= scalar * ( lhs_ * rhs_ );
            odres_  -= scalar * ( lhs_ * rhs_ );
            sres_   -= scalar * ( lhs_ * rhs_ );
            osres_  -= scalar * ( lhs_ * rhs_ );
            refres_ -= scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_   -= scalar * ( lhs_ * orhs_ );
            odres_  -= scalar * ( lhs_ * orhs_ );
            sres_   -= scalar * ( lhs_ * orhs_ );
            osres_  -= scalar * ( lhs_ * orhs_ );
            refres_ -= scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   -= scalar * ( olhs_ * rhs_ );
            odres_  -= scalar * ( olhs_ * rhs_ );
            sres_   -= scalar * ( olhs_ * rhs_ );
            osres_  -= scalar * ( olhs_ * rhs_ );
            refres_ -= scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_   -= scalar * ( olhs_ * orhs_ );
            odres_  -= scalar * ( olhs_ * orhs_ );
            sres_   -= scalar * ( olhs_ * orhs_ );
            osres_  -= scalar * ( olhs_ * orhs_ );
            refres_ -= scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }

      // Scaled multiplication with subtraction assignment with evaluated matrices
      {
         test_ = "Scaled multiplication with subtraction assignment with evaluated matrices (s*OP)";

         try {
            dres_   -= scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            odres_  -= scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            sres_   -= scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            osres_  -= scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            refres_ -= scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_   -= scalar * ( eval( lhs_ ) * eval( orhs_ ) );
            odres_  -= scalar * ( eval( lhs_ ) * eval( orhs_ ) );
            sres_   -= scalar * ( eval( lhs_ ) * eval( orhs_ ) );
            osres_  -= scalar * ( eval( lhs_ ) * eval( orhs_ ) );
            refres_ -= scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   -= scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            odres_  -= scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            sres_   -= scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            osres_  -= scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            refres_ -= scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_   -= scalar * ( eval( olhs_ ) * eval( orhs_ ) );
            odres_  -= scalar * ( eval( olhs_ ) * eval( orhs_ ) );
            sres_   -= scalar * ( eval( olhs_ ) * eval( orhs_ ) );
            osres_  -= scalar * ( eval( olhs_ ) * eval( orhs_ ) );
            refres_ -= scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Scaled multiplication with subtraction assignment (OP*s)
      //=====================================================================================

      // Scaled multiplication with subtraction assignment with the given matrices
      {
         test_ = "Scaled multiplication with subtraction assignment with the given matrices (OP*s)";

         try {
            dres_   -= ( lhs_ * rhs_ ) * scalar;
            odres_  -= ( lhs_ * rhs_ ) * scalar;
            sres_   -= ( lhs_ * rhs_ ) * scalar;
            osres_  -= ( lhs_ * rhs_ ) * scalar;
            refres_ -= ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_   -= ( lhs_ * orhs_ ) * scalar;
            odres_  -= ( lhs_ * orhs_ ) * scalar;
            sres_   -= ( lhs_ * orhs_ ) * scalar;
            osres_  -= ( lhs_ * orhs_ ) * scalar;
            refres_ -= ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   -= ( olhs_ * rhs_ ) * scalar;
            odres_  -= ( olhs_ * rhs_ ) * scalar;
            sres_   -= ( olhs_ * rhs_ ) * scalar;
            osres_  -= ( olhs_ * rhs_ ) * scalar;
            refres_ -= ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_   -= ( olhs_ * orhs_ ) * scalar;
            odres_  -= ( olhs_ * orhs_ ) * scalar;
            sres_   -= ( olhs_ * orhs_ ) * scalar;
            osres_  -= ( olhs_ * orhs_ ) * scalar;
            refres_ -= ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }

      // Scaled multiplication with subtraction assignment with evaluated matrices
      {
         test_ = "Scaled multiplication with subtraction assignment with evaluated matrices (OP*s)";

         try {
            dres_   -= ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            odres_  -= ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            sres_   -= ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            osres_  -= ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            refres_ -= ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_   -= ( eval( lhs_ ) * eval( orhs_ ) ) * scalar;
            odres_  -= ( eval( lhs_ ) * eval( orhs_ ) ) * scalar;
            sres_   -= ( eval( lhs_ ) * eval( orhs_ ) ) * scalar;
            osres_  -= ( eval( lhs_ ) * eval( orhs_ ) ) * scalar;
            refres_ -= ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   -= ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            odres_  -= ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            sres_   -= ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            osres_  -= ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            refres_ -= ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_   -= ( eval( olhs_ ) * eval( orhs_ ) ) * scalar;
            odres_  -= ( eval( olhs_ ) * eval( orhs_ ) ) * scalar;
            sres_   -= ( eval( olhs_ ) * eval( orhs_ ) ) * scalar;
            osres_  -= ( eval( olhs_ ) * eval( orhs_ ) ) * scalar;
            refres_ -= ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Scaled multiplication with subtraction assignment (OP/s)
      //=====================================================================================

      // Scaled multiplication with subtraction assignment with the given matrices
      {
         test_ = "Scaled multiplication with subtraction assignment with the given matrices (OP/s)";

         try {
            dres_   -= ( lhs_ * rhs_ ) / scalar;
            odres_  -= ( lhs_ * rhs_ ) / scalar;
            sres_   -= ( lhs_ * rhs_ ) / scalar;
            osres_  -= ( lhs_ * rhs_ ) / scalar;
            refres_ -= ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_   -= ( lhs_ * orhs_ ) / scalar;
            odres_  -= ( lhs_ * orhs_ ) / scalar;
            sres_   -= ( lhs_ * orhs_ ) / scalar;
            osres_  -= ( lhs_ * orhs_ ) / scalar;
            refres_ -= ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   -= ( olhs_ * rhs_ ) / scalar;
            odres_  -= ( olhs_ * rhs_ ) / scalar;
            sres_   -= ( olhs_ * rhs_ ) / scalar;
            osres_  -= ( olhs_ * rhs_ ) / scalar;
            refres_ -= ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_   -= ( olhs_ * orhs_ ) / scalar;
            odres_  -= ( olhs_ * orhs_ ) / scalar;
            sres_   -= ( olhs_ * orhs_ ) / scalar;
            osres_  -= ( olhs_ * orhs_ ) / scalar;
            refres_ -= ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }

      // Scaled multiplication with subtraction assignment with evaluated matrices
      {
         test_ = "Scaled multiplication with subtraction assignment with evaluated matrices (OP/s)";

         try {
            dres_   -= ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            odres_  -= ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            sres_   -= ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            osres_  -= ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            refres_ -= ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_   -= ( eval( lhs_ ) * eval( orhs_ ) ) / scalar;
            odres_  -= ( eval( lhs_ ) * eval( orhs_ ) ) / scalar;
            sres_   -= ( eval( lhs_ ) * eval( orhs_ ) ) / scalar;
            osres_  -= ( eval( lhs_ ) * eval( orhs_ ) ) / scalar;
            refres_ -= ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   -= ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            odres_  -= ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            sres_   -= ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            osres_  -= ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            refres_ -= ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_   -= ( eval( olhs_ ) * eval( orhs_ ) ) / scalar;
            odres_  -= ( eval( olhs_ ) * eval( orhs_ ) ) / scalar;
            sres_   -= ( eval( olhs_ ) * eval( orhs_ ) ) / scalar;
            osres_  -= ( eval( olhs_ ) * eval( orhs_ ) ) / scalar;
            refres_ -= ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the transpose dense matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the transpose matrix multiplication with plain assignment, addition
// assignment, and subtraction assignment. In case any error resulting from the multiplication
// or the subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side dense matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void DMatDMatMult<MT1,MT2>::testTransposeOperation()
{
#if BLAZETEST_MATHTEST_TEST_TRANSPOSE_OPERATION
   if( BLAZETEST_MATHTEST_TEST_TRANSPOSE_OPERATION > 1 )
   {
      //=====================================================================================
      // Transpose multiplication
      //=====================================================================================

      // Transpose multiplication with the given matrices
      {
         test_ = "Transpose multiplication with the given matrices";

         try {
            tdres_  = trans( lhs_ * rhs_ );
            todres_ = trans( lhs_ * rhs_ );
            tsres_  = trans( lhs_ * rhs_ );
            tosres_ = trans( lhs_ * rhs_ );
            refres_ = trans( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkTransposeResults<MT1,MT2>();

         try {
            tdres_  = trans( lhs_ * orhs_ );
            todres_ = trans( lhs_ * orhs_ );
            tsres_  = trans( lhs_ * orhs_ );
            tosres_ = trans( lhs_ * orhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkTransposeResults<MT1,OMT2>();

         try {
            tdres_  = trans( olhs_ * rhs_ );
            todres_ = trans( olhs_ * rhs_ );
            tsres_  = trans( olhs_ * rhs_ );
            tosres_ = trans( olhs_ * rhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkTransposeResults<OMT1,MT2>();

         try {
            tdres_  = trans( olhs_ * orhs_ );
            todres_ = trans( olhs_ * orhs_ );
            tsres_  = trans( olhs_ * orhs_ );
            tosres_ = trans( olhs_ * orhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkTransposeResults<OMT1,OMT2>();
      }

      // Transpose multiplication with evaluated matrices
      {
         test_ = "Transpose multiplication with evaluated matrices";

         try {
            tdres_  = trans( eval( lhs_ ) * eval( rhs_ ) );
            todres_ = trans( eval( lhs_ ) * eval( rhs_ ) );
            tsres_  = trans( eval( lhs_ ) * eval( rhs_ ) );
            tosres_ = trans( eval( lhs_ ) * eval( rhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkTransposeResults<MT1,MT2>();

         try {
            tdres_  = trans( eval( lhs_ ) * eval( orhs_ ) );
            todres_ = trans( eval( lhs_ ) * eval( orhs_ ) );
            tsres_  = trans( eval( lhs_ ) * eval( orhs_ ) );
            tosres_ = trans( eval( lhs_ ) * eval( orhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkTransposeResults<MT1,OMT2>();

         try {
            tdres_  = trans( eval( olhs_ ) * eval( rhs_ ) );
            todres_ = trans( eval( olhs_ ) * eval( rhs_ ) );
            tsres_  = trans( eval( olhs_ ) * eval( rhs_ ) );
            tosres_ = trans( eval( olhs_ ) * eval( rhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkTransposeResults<OMT1,MT2>();

         try {
            tdres_  = trans( eval( olhs_ ) * eval( orhs_ ) );
            todres_ = trans( eval( olhs_ ) * eval( orhs_ ) );
            tsres_  = trans( eval( olhs_ ) * eval( orhs_ ) );
            tosres_ = trans( eval( olhs_ ) * eval( orhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkTransposeResults<OMT1,OMT2>();
      }
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the abs dense matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the abs matrix multiplication with plain assignment, addition assignment,
// and subtraction assignment. In case any error resulting from the multiplication or the
// subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side dense matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void DMatDMatMult<MT1,MT2>::testAbsOperation()
{
#if BLAZETEST_MATHTEST_TEST_ABS_OPERATION
   if( BLAZETEST_MATHTEST_TEST_ABS_OPERATION > 1 )
   {
      //=====================================================================================
      // Abs multiplication
      //=====================================================================================

      // Abs multiplication with the given matrices
      {
         test_ = "Abs multiplication with the given matrices";

         try {
            dres_   = abs( lhs_ * rhs_ );
            odres_  = abs( lhs_ * rhs_ );
            sres_   = abs( lhs_ * rhs_ );
            osres_  = abs( lhs_ * rhs_ );
            refres_ = abs( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_  = abs( lhs_ * orhs_ );
            odres_ = abs( lhs_ * orhs_ );
            sres_  = abs( lhs_ * orhs_ );
            osres_ = abs( lhs_ * orhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_  = abs( olhs_ * rhs_ );
            odres_ = abs( olhs_ * rhs_ );
            sres_  = abs( olhs_ * rhs_ );
            osres_ = abs( olhs_ * rhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_  = abs( olhs_ * orhs_ );
            odres_ = abs( olhs_ * orhs_ );
            sres_  = abs( olhs_ * orhs_ );
            osres_ = abs( olhs_ * orhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }

      // Abs multiplication with evaluated matrices
      {
         test_ = "Abs multiplication with evaluated matrices";

         try {
            dres_  = abs( eval( lhs_ ) * eval( rhs_ ) );
            odres_ = abs( eval( lhs_ ) * eval( rhs_ ) );
            sres_  = abs( eval( lhs_ ) * eval( rhs_ ) );
            osres_ = abs( eval( lhs_ ) * eval( rhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_  = abs( eval( lhs_ ) * eval( orhs_ ) );
            odres_ = abs( eval( lhs_ ) * eval( orhs_ ) );
            sres_  = abs( eval( lhs_ ) * eval( orhs_ ) );
            osres_ = abs( eval( lhs_ ) * eval( orhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_  = abs( eval( olhs_ ) * eval( rhs_ ) );
            odres_ = abs( eval( olhs_ ) * eval( rhs_ ) );
            sres_  = abs( eval( olhs_ ) * eval( rhs_ ) );
            osres_ = abs( eval( olhs_ ) * eval( rhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_  = abs( eval( olhs_ ) * eval( orhs_ ) );
            odres_ = abs( eval( olhs_ ) * eval( orhs_ ) );
            sres_  = abs( eval( olhs_ ) * eval( orhs_ ) );
            osres_ = abs( eval( olhs_ ) * eval( orhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Abs multiplication with addition assignment
      //=====================================================================================

      // Abs multiplication with addition assignment with the given matrices
      {
         test_ = "Abs multiplication with addition assignment with the given matrices";

         try {
            dres_   += abs( lhs_ * rhs_ );
            odres_  += abs( lhs_ * rhs_ );
            sres_   += abs( lhs_ * rhs_ );
            osres_  += abs( lhs_ * rhs_ );
            refres_ += abs( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_   += abs( lhs_ * orhs_ );
            odres_  += abs( lhs_ * orhs_ );
            sres_   += abs( lhs_ * orhs_ );
            osres_  += abs( lhs_ * orhs_ );
            refres_ += abs( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   += abs( olhs_ * rhs_ );
            odres_  += abs( olhs_ * rhs_ );
            sres_   += abs( olhs_ * rhs_ );
            osres_  += abs( olhs_ * rhs_ );
            refres_ += abs( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_   += abs( olhs_ * orhs_ );
            odres_  += abs( olhs_ * orhs_ );
            sres_   += abs( olhs_ * orhs_ );
            osres_  += abs( olhs_ * orhs_ );
            refres_ += abs( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }

      // Abs multiplication with addition assignment with evaluated matrices
      {
         test_ = "Abs multiplication with addition assignment with evaluated matrices";

         try {
            dres_   += abs( eval( lhs_ ) * eval( rhs_ ) );
            odres_  += abs( eval( lhs_ ) * eval( rhs_ ) );
            sres_   += abs( eval( lhs_ ) * eval( rhs_ ) );
            osres_  += abs( eval( lhs_ ) * eval( rhs_ ) );
            refres_ += abs( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_   += abs( eval( lhs_ ) * eval( orhs_ ) );
            odres_  += abs( eval( lhs_ ) * eval( orhs_ ) );
            sres_   += abs( eval( lhs_ ) * eval( orhs_ ) );
            osres_  += abs( eval( lhs_ ) * eval( orhs_ ) );
            refres_ += abs( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   += abs( eval( olhs_ ) * eval( rhs_ ) );
            odres_  += abs( eval( olhs_ ) * eval( rhs_ ) );
            sres_   += abs( eval( olhs_ ) * eval( rhs_ ) );
            osres_  += abs( eval( olhs_ ) * eval( rhs_ ) );
            refres_ += abs( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   += abs( eval( olhs_ ) * eval( orhs_ ) );
            odres_  += abs( eval( olhs_ ) * eval( orhs_ ) );
            sres_   += abs( eval( olhs_ ) * eval( orhs_ ) );
            osres_  += abs( eval( olhs_ ) * eval( orhs_ ) );
            refres_ += abs( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Abs multiplication with subtraction assignment
      //=====================================================================================

      // Abs multiplication with subtraction assignment with the given matrices
      {
         test_ = "Abs multiplication with subtraction assignment with the given matrices";

         try {
            dres_   -= abs( lhs_ * rhs_ );
            odres_  -= abs( lhs_ * rhs_ );
            sres_   -= abs( lhs_ * rhs_ );
            osres_  -= abs( lhs_ * rhs_ );
            refres_ -= abs( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_   -= abs( lhs_ * orhs_ );
            odres_  -= abs( lhs_ * orhs_ );
            sres_   -= abs( lhs_ * orhs_ );
            osres_  -= abs( lhs_ * orhs_ );
            refres_ -= abs( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   -= abs( olhs_ * rhs_ );
            odres_  -= abs( olhs_ * rhs_ );
            sres_   -= abs( olhs_ * rhs_ );
            osres_  -= abs( olhs_ * rhs_ );
            refres_ -= abs( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_   -= abs( olhs_ * orhs_ );
            odres_  -= abs( olhs_ * orhs_ );
            sres_   -= abs( olhs_ * orhs_ );
            osres_  -= abs( olhs_ * orhs_ );
            refres_ -= abs( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }

      // Abs multiplication with subtraction assignment with evaluated matrices
      {
         test_ = "Abs multiplication with subtraction assignment with evaluated matrices";

         try {
            dres_   -= abs( eval( lhs_ ) * eval( rhs_ ) );
            odres_  -= abs( eval( lhs_ ) * eval( rhs_ ) );
            sres_   -= abs( eval( lhs_ ) * eval( rhs_ ) );
            osres_  -= abs( eval( lhs_ ) * eval( rhs_ ) );
            refres_ -= abs( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();

         try {
            dres_   -= abs( eval( lhs_ ) * eval( orhs_ ) );
            odres_  -= abs( eval( lhs_ ) * eval( orhs_ ) );
            sres_   -= abs( eval( lhs_ ) * eval( orhs_ ) );
            osres_  -= abs( eval( lhs_ ) * eval( orhs_ ) );
            refres_ -= abs( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT1  ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,OMT2>();

         try {
            dres_   -= abs( eval( olhs_ ) * eval( rhs_ ) );
            odres_  -= abs( eval( olhs_ ) * eval( rhs_ ) );
            sres_   -= abs( eval( olhs_ ) * eval( rhs_ ) );
            osres_  -= abs( eval( olhs_ ) * eval( rhs_ ) );
            refres_ -= abs( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side row-major dense matrix type:\n"
                << "     " << typeid( MT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,MT2>();

         try {
            dres_   -= abs( eval( olhs_ ) * eval( orhs_ ) );
            odres_  -= abs( eval( olhs_ ) * eval( orhs_ ) );
            sres_   -= abs( eval( olhs_ ) * eval( orhs_ ) );
            osres_  -= abs( eval( olhs_ ) * eval( orhs_ ) );
            refres_ -= abs( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT1 ).name() << "\n"
                << "   Right-hand side column-major dense matrix type:\n"
                << "     " << typeid( OMT2 ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<OMT1,OMT2>();
      }
   }
#endif
}
//*************************************************************************************************




//=================================================================================================
//
//  ERROR DETECTION FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Checking and comparing the computed results.
//
// \return void
// \exception std::runtime_error Incorrect dense result detected.
// \exception std::runtime_error Incorrect sparse result detected.
//
// This function is called after each test case to check and compare the computed results. The
// two template arguments \a LT and \a RT indicate the types of the left-hand side and right-hand
// side operands used for the computations.
*/
template< typename MT1    // Type of the left-hand side dense matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
template< typename LT     // Type of the left-hand side operand
        , typename RT >   // Type of the right-hand side operand
void DMatDMatMult<MT1,MT2>::checkResults()
{
   using blaze::IsRowMajorMatrix;

   if( !isEqual( dres_, refres_ ) || !isEqual( odres_, refres_ ) ) {
      std::ostringstream oss;
      oss.precision( 20 );
      oss << " Test : " << test_ << "\n"
          << " Error: Incorrect dense result detected\n"
          << " Details:\n"
          << "   Left-hand side " << ( IsRowMajorMatrix<LT>::value ? ( "row-major" ) : ( "column-major" ) ) << " dense matrix type:\n"
          << "     " << typeid( LT ).name() << "\n"
          << "   Right-hand side " << ( IsRowMajorMatrix<RT>::value ? ( "row-major" ) : ( "column-major" ) ) << " dense matrix type:\n"
          << "     " << typeid( RT ).name() << "\n"
          << "   Result:\n" << dres_ << "\n"
          << "   Result with opposite storage order:\n" << odres_ << "\n"
          << "   Expected result:\n" << refres_ << "\n";
      throw std::runtime_error( oss.str() );
   }

   if( !isEqual( sres_, refres_ ) || !isEqual( osres_, refres_ ) ) {
      std::ostringstream oss;
      oss.precision( 20 );
      oss << " Test : " << test_ << "\n"
          << " Error: Incorrect sparse result detected\n"
          << " Details:\n"
          << "   Left-hand side " << ( IsRowMajorMatrix<LT>::value ? ( "row-major" ) : ( "column-major" ) ) << " dense matrix type:\n"
          << "     " << typeid( LT ).name() << "\n"
          << "   Right-hand side " << ( IsRowMajorMatrix<RT>::value ? ( "row-major" ) : ( "column-major" ) ) << " dense matrix type:\n"
          << "     " << typeid( RT ).name() << "\n"
          << "   Result:\n" << sres_ << "\n"
          << "   Result with opposite storage order:\n" << osres_ << "\n"
          << "   Expected result:\n" << refres_ << "\n";
      throw std::runtime_error( oss.str() );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checking and comparing the computed transpose results.
//
// \return void
// \exception std::runtime_error Incorrect dense result detected.
// \exception std::runtime_error Incorrect sparse result detected.
//
// This function is called after each test case to check and compare the computed transpose
// results. The two template arguments \a LT and \a RT indicate the types of the left-hand
// side and right-hand side operands used for the computations.
*/
template< typename MT1    // Type of the left-hand side dense matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
template< typename LT     // Type of the left-hand side operand
        , typename RT >   // Type of the right-hand side operand
void DMatDMatMult<MT1,MT2>::checkTransposeResults()
{
   using blaze::IsRowMajorMatrix;

   if( !isEqual( tdres_, refres_ ) || !isEqual( todres_, refres_ ) ) {
      std::ostringstream oss;
      oss.precision( 20 );
      oss << " Test : " << test_ << "\n"
          << " Error: Incorrect dense result detected\n"
          << " Details:\n"
          << "   Left-hand side " << ( IsRowMajorMatrix<LT>::value ? ( "row-major" ) : ( "column-major" ) ) << " dense matrix type:\n"
          << "     " << typeid( LT ).name() << "\n"
          << "   Right-hand side " << ( IsRowMajorMatrix<RT>::value ? ( "row-major" ) : ( "column-major" ) ) << " dense matrix type:\n"
          << "     " << typeid( RT ).name() << "\n"
          << "   Transpose result:\n" << tdres_ << "\n"
          << "   Transpose result with opposite storage order:\n" << todres_ << "\n"
          << "   Expected result:\n" << refres_ << "\n";
      throw std::runtime_error( oss.str() );
   }

   if( !isEqual( tsres_, refres_ ) || !isEqual( tosres_, refres_ ) ) {
      std::ostringstream oss;
      oss.precision( 20 );
      oss << " Test : " << test_ << "\n"
          << " Error: Incorrect sparse result detected\n"
          << " Details:\n"
          << "   Left-hand side " << ( IsRowMajorMatrix<LT>::value ? ( "row-major" ) : ( "column-major" ) ) << " dense matrix type:\n"
          << "     " << typeid( LT ).name() << "\n"
          << "   Right-hand side " << ( IsRowMajorMatrix<RT>::value ? ( "row-major" ) : ( "column-major" ) ) << " dense matrix type:\n"
          << "     " << typeid( RT ).name() << "\n"
          << "   Transpose result:\n" << tsres_ << "\n"
          << "   Transpose result with opposite storage order:\n" << tosres_ << "\n"
          << "   Expected result:\n" << refres_ << "\n";
      throw std::runtime_error( oss.str() );
   }
}
//*************************************************************************************************




//=================================================================================================
//
//  GLOBAL TEST FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Testing the matrix multiplication between two specific matrix types.
//
// \param creator1 The creator for the left-hand side matrix.
// \param creator2 The creator for the right-hand side matrix.
// \return void
*/
template< typename MT1    // Type of the left-hand side dense matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void runTest( const Creator<MT1>& creator1, const Creator<MT2>& creator2 )
{
   for( size_t rep=0; rep<repetitions; ++rep ) {
      DMatDMatMult<MT1,MT2>( creator1, creator2 );
   }
}
//*************************************************************************************************




//=================================================================================================
//
//  MACROS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Macro for the definition of a dense matrix/dense matrix multiplication test case.
*/
#define DEFINE_DMATDMATMULT_TEST( MT1, MT2 ) \
   extern template class blazetest::mathtest::dmatdmatmult::DMatDMatMult<MT1,MT2>
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Macro for the execution of a dense matrix/dense matrix multiplication test case.
*/
#define RUN_DMATDMATMULT_TEST( C1, C2 ) \
   blazetest::mathtest::dmatdmatmult::runTest( C1, C2 )
/*! \endcond */
//*************************************************************************************************

} // namespace dmatdmatmult

} // namespace mathtest

} // namespace blazetest

#endif
