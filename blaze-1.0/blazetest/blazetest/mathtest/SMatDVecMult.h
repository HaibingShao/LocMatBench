//=================================================================================================
/*!
//  \file blazetest/mathtest/SMatDVecMult.h
//  \brief Header file for the sparse matrix/dense vector multiplication math test
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

#ifndef _BLAZETEST_MATHTEST_SMATDVECMULT_H_
#define _BLAZETEST_MATHTEST_SMATDVECMULT_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <blaze/math/CompressedMatrix.h>
#include <blaze/math/constraints/DenseMatrix.h>
#include <blaze/math/constraints/DenseVector.h>
#include <blaze/math/constraints/SparseMatrix.h>
#include <blaze/math/constraints/SparseVector.h>
#include <blaze/math/constraints/StorageOrder.h>
#include <blaze/math/constraints/TransposeFlag.h>
#include <blaze/math/DynamicMatrix.h>
#include <blaze/math/MathTrait.h>
#include <blaze/math/StaticMatrix.h>
#include <blaze/math/typetraits/IsRowMajorMatrix.h>
#include <blaze/util/constraints/Numeric.h>
#include <blaze/util/constraints/SameType.h>
#include <blazetest/system/MathTest.h>
#include <blazetest/util/Creator.h>
#include <blazetest/util/Utility.h>


namespace blazetest {

namespace mathtest {

namespace smatdvecmult {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Auxiliary class template for the sparse matrix/dense vector multiplication math test.
//
// The SMatDVecMult class template represents one particular matrix/vector multiplication test
// between a matrix and a vector of particular types. The two template arguments \a MT and \a VT
// represent the types of the left-hand side matrix and right-hand side vector, respectively.
*/
template< typename MT    // Type of the left-hand side sparse matrix
        , typename VT >  // Type of the right-hand side dense vector
class SMatDVecMult
{
 private:
   //**Type definitions****************************************************************************
   typedef typename MT::OppositeType                   OMT;  //!< Matrix type with opposite storage order
   typedef typename MT::TransposeType                  TMT;  //!< Transpose matrix type
   typedef typename VT::TransposeType                  TVT;  //!< Transpose vector type
   typedef typename blaze::MathTrait<MT,VT>::MultType  RE;   //!< Default result type
   typedef typename RE::TransposeType                  TRE;  //!< Transpose default result type
   //**********************************************************************************************

   //**Type definitions****************************************************************************
   typedef typename MT::ElementType            MET;    //!< Element type of the matrix type.
   typedef typename VT::ElementType            VET;    //!< Element type of the vector type.
   typedef typename RE::ElementType            RET;    //!< Resulting element type
   typedef blaze::DynamicMatrix<MET,false>     MRT;    //!< Matrix reference type
   typedef blaze::DynamicVector<VET,false>     VRT;    //!< Vector reference type
   typedef blaze::DynamicVector<RET,false>     DRRE;   //!< Dense reference result type
   typedef blaze::CompressedVector<RET,false>  SRRE;   //!< Sparse reference result type
   typedef typename DRRE::TransposeType        TDRRE;  //!< Transpose dense reference result type
   typedef typename SRRE::TransposeType        TSRRE;  //!< Transpose sparse reference result type
   typedef RE                                  DRE;    //!< Dense result type
   typedef SRRE                                SRE;    //!< Sparse result type
   typedef TRE                                 TDRE;   //!< Transpose dense result type
   typedef TSRRE                               TSRE;   //!< Transpose sparse result type
   //**********************************************************************************************

 public:
   //**Constructors********************************************************************************
   /*!\name Constructors */
   //@{
   explicit SMatDVecMult( const Creator<MT>& creator1, const Creator<VT>& creator2 );
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
   template< typename LT > void checkResults();
   template< typename LT > void checkTransposeResults();
   //@}
   //**********************************************************************************************

   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   MT    lhs_;      //!< The left-hand side sparse matrix.
   VT    rhs_;      //!< The right-hand side dense vector.
   DRE   dres_;     //!< The dense result vector.
   SRE   sres_;     //!< The sparse result vector.
   MRT   reflhs_;   //!< The reference left-hand side matrix.
   VRT   refrhs_;   //!< The reference right-hand side vector.
   DRRE  refres_;   //!< The reference result.
   OMT   olhs_;     //!< The left-hand side sparse matrix with opposite storage order.
   TDRE  tdres_;    //!< The transpose dense result vector.
   TSRE  tsres_;    //!< The transpose sparse result vector.
   TDRRE trefres_;  //!< The transpose reference result.

   std::string test_;  //!< Label of the currently performed test.
   //@}
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE( MT    );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE ( VT    );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE( TMT   );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE ( TVT   );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( MRT   );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE ( VRT   );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE ( DRE   );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_VECTOR_TYPE( SRE   );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE ( DRRE  );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_VECTOR_TYPE( SRRE  );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE ( TDRE  );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_VECTOR_TYPE( TSRE  );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE ( TDRRE );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_VECTOR_TYPE( TSRRE );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE   ( MT    );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( TMT   );
   BLAZE_CONSTRAINT_MUST_BE_NONTRANSPOSE_VECTOR_TYPE( VT    );
   BLAZE_CONSTRAINT_MUST_BE_TRANSPOSE_VECTOR_TYPE   ( TVT   );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE   ( MRT   );
   BLAZE_CONSTRAINT_MUST_BE_NONTRANSPOSE_VECTOR_TYPE( VRT   );
   BLAZE_CONSTRAINT_MUST_BE_NONTRANSPOSE_VECTOR_TYPE( DRE   );
   BLAZE_CONSTRAINT_MUST_BE_NONTRANSPOSE_VECTOR_TYPE( SRE   );
   BLAZE_CONSTRAINT_MUST_BE_NONTRANSPOSE_VECTOR_TYPE( DRRE  );
   BLAZE_CONSTRAINT_MUST_BE_NONTRANSPOSE_VECTOR_TYPE( SRRE  );
   BLAZE_CONSTRAINT_MUST_BE_TRANSPOSE_VECTOR_TYPE   ( TDRE  );
   BLAZE_CONSTRAINT_MUST_BE_TRANSPOSE_VECTOR_TYPE   ( TSRE  );
   BLAZE_CONSTRAINT_MUST_BE_TRANSPOSE_VECTOR_TYPE   ( TDRRE );
   BLAZE_CONSTRAINT_MUST_BE_TRANSPOSE_VECTOR_TYPE   ( TSRRE );
   BLAZE_CONSTRAINT_MATRICES_MUST_HAVE_DIFFERENT_STORAGE_ORDER( MT, TMT );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( MET, typename OMT::ElementType   );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( MET, typename TMT::ElementType   );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( VET, typename TVT::ElementType   );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( MT , typename OMT::OppositeType  );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( MT , typename TMT::TransposeType );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( VT , typename TVT::TransposeType );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( RE , typename TRE::TransposeType );
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
/*!\brief Constructor for the SMatDVecMult class template.
//
// \param creator1 The creator for the left-hand side sparse matrix of the multiplication.
// \param creator2 The creator for the right-hand side dense vector of the multiplication.
// \exception std::runtime_error Operation error detected.
*/
template< typename MT    // Type of the left-hand side sparse matrix
        , typename VT >  // Type of the right-hand side dense vector
SMatDVecMult<MT,VT>::SMatDVecMult( const Creator<MT>& creator1, const Creator<VT>& creator2 )
   : lhs_ ( creator1() )  // The left-hand side sparse matrix
   , rhs_ ( creator2() )  // The right-hand side dense vector
   , dres_()              // The dense result vector
   , sres_()              // The sparse result vector
   , reflhs_( lhs_ )      // The reference left-hand side matrix
   , refrhs_( rhs_ )      // The reference right-hand side vector
   , refres_()            // The reference result
   , olhs_( lhs_ )        // The left-hand side sparse matrix with opposite storage order.
   , tdres_()             // The transpose dense result vector.
   , tsres_()             // The transpose sparse result vector.
   , trefres_()           // The transpose reference result.
   , test_()              // Label of the currently performed test
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
/*!\brief Tests on the initial status of the operands.
//
// \return void
// \exception std::runtime_error Initialization error detected.
//
// This function runs tests on the initial status of the operands. In case any initialization
// error is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT    // Type of the left-hand side sparse matrix
        , typename VT >  // Type of the right-hand side dense vector
void SMatDVecMult<MT,VT>::testInitialStatus()
{
   //=====================================================================================
   // Performing initial tests with the given types
   //=====================================================================================

   // Checking the number of rows of the left-hand side operand
   if( lhs_.rows() != reflhs_.rows() ) {
      std::ostringstream oss;
      oss << " Test: Initial size comparison of left-hand side sparse operand\n"
          << " Error: Invalid number of rows\n"
          << " Details:\n"
          << "   Sparse matrix type:\n"
          << "     " << typeid( MT ).name() << "\n"
          << "   Detected number of rows = " << lhs_.rows() << "\n"
          << "   Expected number of rows = " << reflhs_.rows() << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the number of columns of the left-hand side operand
   if( lhs_.columns() != reflhs_.columns() ) {
      std::ostringstream oss;
      oss << " Test: Initial size comparison of left-hand side sparse operand\n"
          << " Error: Invalid number of columns\n"
          << " Details:\n"
          << "   Sparse matrix type:\n"
          << "     " << typeid( MT ).name() << "\n"
          << "   Detected number of columns = " << lhs_.columns() << "\n"
          << "   Expected number of columns = " << reflhs_.columns() << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the size of the right-hand side operand
   if( rhs_.size() != refrhs_.size() ) {
      std::ostringstream oss;
      oss << " Test: Initial size comparison of right-hand side dense operand\n"
          << " Error: Invalid vector size\n"
          << " Details:\n"
          << "   Dense vector type:\n"
          << "     " << typeid( VT ).name() << "\n"
          << "   Detected size = " << rhs_.size() << "\n"
          << "   Expected size = " << refrhs_.size() << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the initialization of the left-hand side operand
   if( !isEqual( lhs_, reflhs_ ) ) {
      std::ostringstream oss;
      oss << " Test: Initial test of initialization of left-hand side sparse operand\n"
          << " Error: Invalid matrix initialization\n"
          << " Details:\n"
          << "   Sparse matrix type:\n"
          << "     " << typeid( MT ).name() << "\n"
          << "   Current initialization:\n" << lhs_ << "\n"
          << "   Expected initialization:\n" << reflhs_ << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the initialization of the right-hand side operand
   if( !isEqual( rhs_, refrhs_ ) ) {
      std::ostringstream oss;
      oss << " Test: Initial test of initialization of right-hand side dense operand\n"
          << " Error: Invalid vector initialization\n"
          << " Details:\n"
          << "   Dense vector type:\n"
          << "     " << typeid( VT ).name() << "\n"
          << "   Current initialization:\n" << rhs_ << "\n"
          << "   Expected initialization:\n" << refrhs_ << "\n";
      throw std::runtime_error( oss.str() );
   }


   //=====================================================================================
   // Performing initial tests with the transpose types
   //=====================================================================================

   // Checking the number of rows of the transpose left-hand side operand
   if( olhs_.rows() != reflhs_.rows() ) {
      std::ostringstream oss;
      oss << " Test: Initial size comparison of transpose left-hand side sparse operand\n"
          << " Error: Invalid number of rows\n"
          << " Details:\n"
          << "   Transpose sparse matrix type:\n"
          << "     " << typeid( TMT ).name() << "\n"
          << "   Detected number of rows = " << olhs_.rows() << "\n"
          << "   Expected number of rows = " << reflhs_.rows() << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the number of columns of the transpose left-hand side operand
   if( olhs_.columns() != reflhs_.columns() ) {
      std::ostringstream oss;
      oss << " Test: Initial size comparison of transpose left-hand side sparse operand\n"
          << " Error: Invalid number of columns\n"
          << " Details:\n"
          << "   Transpose sparse matrix type:\n"
          << "     " << typeid( TMT ).name() << "\n"
          << "   Detected number of columns = " << olhs_.columns() << "\n"
          << "   Expected number of columns = " << reflhs_.columns() << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the initialization of the transpose left-hand side operand
   if( !isEqual( olhs_, reflhs_ ) ) {
      std::ostringstream oss;
      oss << " Test: Initial test of initialization of transpose left-hand side sparse operand\n"
          << " Error: Invalid matrix initialization\n"
          << " Details:\n"
          << "   Transpose sparse matrix type:\n"
          << "     " << typeid( TMT ).name() << "\n"
          << "   Current initialization:\n" << olhs_ << "\n"
          << "   Expected initialization:\n" << reflhs_ << "\n";
      throw std::runtime_error( oss.str() );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the vector assignment.
//
// \return void
// \exception std::runtime_error Assignment error detected.
//
// This function tests the vector assignment. In case any error is detected, a
// \a std::runtime_error exception is thrown.
*/
template< typename MT    // Type of the left-hand side sparse matrix
        , typename VT >  // Type of the right-hand side dense vector
void SMatDVecMult<MT,VT>::testAssignment()
{
   //=====================================================================================
   // Performing an assignment with the given types
   //=====================================================================================

   try {
      lhs_ = reflhs_;
      rhs_ = refrhs_;
   }
   catch( std::exception& ex ) {
      std::ostringstream oss;
      oss << " Test: Assignment with the given types\n"
          << " Error: Failed assignment\n"
          << " Details:\n"
          << "   Left-hand side sparse matrix type:\n"
          << "     " << typeid( MT ).name() << "\n"
          << "   Right-hand side dense vector type:\n"
          << "     " << typeid( VT ).name() << "\n"
          << "   Error message: " << ex.what() << "\n";
      throw std::runtime_error( oss.str() );
   }

   if( !isEqual( lhs_, reflhs_ ) ) {
      std::ostringstream oss;
      oss << " Test: Checking the assignment result of left-hand side sparse operand\n"
          << " Error: Invalid matrix initialization\n"
          << " Details:\n"
          << "   Sparse matrix type:\n"
          << "     " << typeid( MT ).name() << "\n"
          << "   Current initialization:\n" << lhs_ << "\n"
          << "   Expected initialization:\n" << reflhs_ << "\n";
      throw std::runtime_error( oss.str() );
   }

   if( !isEqual( rhs_, refrhs_ ) ) {
      std::ostringstream oss;
      oss << " Test: Checking the assignment result of right-hand side dense operand\n"
          << " Error: Invalid vector initialization\n"
          << " Details:\n"
          << "   Dense vector type:\n"
          << "     " << typeid( VT ).name() << "\n"
          << "   Current initialization:\n" << rhs_ << "\n"
          << "   Expected initialization:\n" << refrhs_ << "\n";
      throw std::runtime_error( oss.str() );
   }


   //=====================================================================================
   // Performing an assignment with the transpose types
   //=====================================================================================

   try {
      olhs_ = reflhs_;
   }
   catch( std::exception& ex ) {
      std::ostringstream oss;
      oss << " Test: Assignment with the transpose types\n"
          << " Error: Failed assignment\n"
          << " Details:\n"
          << "   Transpose left-hand side sparse matrix type:\n"
          << "     " << typeid( TMT ).name() << "\n"
          << "   Error message: " << ex.what() << "\n";
      throw std::runtime_error( oss.str() );
   }

   if( !isEqual( olhs_, reflhs_ ) ) {
      std::ostringstream oss;
      oss << " Test: Checking the assignment result of transpose left-hand side sparse operand\n"
          << " Error: Invalid matrix initialization\n"
          << " Details:\n"
          << "   Transpose sparse matrix type:\n"
          << "     " << typeid( TMT ).name() << "\n"
          << "   Current initialization:\n" << olhs_ << "\n"
          << "   Expected initialization:\n" << reflhs_ << "\n";
      throw std::runtime_error( oss.str() );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the vector element access.
//
// \return void
// \exception std::runtime_error Element access error detected.
//
// This function tests the element access via the subscript operator. In case any
// error is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT    // Type of the left-hand side sparse matrix
        , typename VT >  // Type of the right-hand side dense vector
void SMatDVecMult<MT,VT>::testElementAccess()
{
   using blaze::equal;


   //=====================================================================================
   // Testing the element access with the given types
   //=====================================================================================

   if( lhs_.rows() > 0UL )
   {
      if( !equal( ( lhs_ * rhs_ )[0UL], ( reflhs_ * refrhs_ )[0UL] ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of multiplication expression\n"
             << " Error: Unequal resulting elements at index 0 detected\n"
             << " Details:\n"
             << "   Left-hand side sparse matrix type:\n"
             << "     " << typeid( MT ).name() << "\n"
             << "   Right-hand side dense vector type:\n"
             << "     " << typeid( VT ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( lhs_ * eval( rhs_ ) )[0UL], ( reflhs_ * eval( refrhs_ ) )[0UL] ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of right evaluated multiplication expression\n"
             << " Error: Unequal resulting elements at index 0 detected\n"
             << " Details:\n"
             << "   Left-hand side sparse matrix type:\n"
             << "     " << typeid( MT ).name() << "\n"
             << "   Right-hand side dense vector type:\n"
             << "     " << typeid( VT ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( eval( lhs_ ) * rhs_ )[0UL], ( eval( reflhs_ ) * refrhs_ )[0UL] ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of left evaluated multiplication expression\n"
             << " Error: Unequal resulting elements at index 0 detected\n"
             << " Details:\n"
             << "   Left-hand side sparse matrix type:\n"
             << "     " << typeid( MT ).name() << "\n"
             << "   Right-hand side dense vector type:\n"
             << "     " << typeid( VT ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( eval( lhs_ ) * eval( rhs_ ) )[0UL], ( eval( reflhs_ ) * eval( refrhs_ ) )[0UL] ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of fully evaluated multiplication expression\n"
             << " Error: Unequal resulting elements at index 0 detected\n"
             << " Details:\n"
             << "   Left-hand side sparse matrix type:\n"
             << "     " << typeid( MT ).name() << "\n"
             << "   Right-hand side dense vector type:\n"
             << "     " << typeid( VT ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }
   }


   //=====================================================================================
   // Testing the element access with the transpose types
   //=====================================================================================

   if( olhs_.rows() > 0UL )
   {
      if( !equal( ( olhs_ * rhs_ )[0UL], ( reflhs_ * refrhs_ )[0UL] ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of transpose multiplication expression\n"
             << " Error: Unequal resulting elements at index 0 detected\n"
             << " Details:\n"
             << "   Transpose left-hand side sparse matrix type:\n"
             << "     " << typeid( TMT ).name() << "\n"
             << "   Transpose right-hand side dense vector type:\n"
             << "     " << typeid( TVT ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( olhs_ * eval( rhs_ ) )[0UL], ( reflhs_ * eval( refrhs_ ) )[0UL] ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of right evaluated transpose multiplication expression\n"
             << " Error: Unequal resulting elements at index 0 detected\n"
             << " Details:\n"
             << "   Transpose left-hand side sparse matrix type:\n"
             << "     " << typeid( TMT ).name() << "\n"
             << "   Transpose right-hand side dense vector type:\n"
             << "     " << typeid( TVT ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( eval( olhs_ ) * rhs_ )[0UL], ( eval( reflhs_ ) * refrhs_ )[0UL] ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of left evaluated transpose multiplication expression\n"
             << " Error: Unequal resulting elements at index 0 detected\n"
             << " Details:\n"
             << "   Transpose left-hand side sparse matrix type:\n"
             << "     " << typeid( TMT ).name() << "\n"
             << "   Transpose right-hand side dense vector type:\n"
             << "     " << typeid( TVT ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( eval( olhs_ ) * eval( rhs_ ) )[0UL], ( eval( reflhs_ ) * eval( refrhs_ ) )[0UL] ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of fully evaluated transpose multiplication expression\n"
             << " Error: Unequal resulting elements at index 0 detected\n"
             << " Details:\n"
             << "   Transpose left-hand side sparse matrix type:\n"
             << "     " << typeid( TMT ).name() << "\n"
             << "   Transpose right-hand side dense vector type:\n"
             << "     " << typeid( TVT ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the plain sparse matrix/dense vector multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the plain matrix/vector multiplication with plain assignment,
// addition assignment, subtraction assignment, and multiplication assignment. In case
// any error resulting from the multiplication or the subsequent assignment is detected,
// a \a std::runtime_error exception is thrown.
*/
template< typename MT    // Type of the left-hand side sparse matrix
        , typename VT >  // Type of the right-hand side dense vector
void SMatDVecMult<MT,VT>::testBasicOperation()
{
#if BLAZETEST_MATHTEST_TEST_BASIC_OPERATION
   if( BLAZETEST_MATHTEST_TEST_BASIC_OPERATION > 1 )
   {
      //=====================================================================================
      // Multiplication
      //=====================================================================================

      // Multiplication with the given matrix/vector
      {
         test_ = "Multiplication with the given matrix/vector";

         try {
            dres_   = lhs_ * rhs_;
            sres_   = lhs_ * rhs_;
            refres_ = reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_ = olhs_ * rhs_;
            sres_ = olhs_ * rhs_;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }

      // Multiplication with evaluated matrix/vector
      {
         test_ = "Multiplication with evaluated matrix/vector";

         try {
            dres_ = eval( lhs_ ) * eval( rhs_ );
            sres_ = eval( lhs_ ) * eval( rhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_ = eval( olhs_ ) * eval( rhs_ );
            sres_ = eval( olhs_ ) * eval( rhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }


      //=====================================================================================
      // Multiplication with addition assignment
      //=====================================================================================

      // Multiplication with addition assignment with the given matrix/vector
      {
         test_ = "Multiplication with addition assignment with the given matrix/vector";

         try {
            dres_   += lhs_ * rhs_;
            sres_   += lhs_ * rhs_;
            refres_ += reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   += olhs_ * rhs_;
            sres_   += olhs_ * rhs_;
            refres_ += reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }

      // Multiplication with addition assignment with evaluated matrix/vector
      {
         test_ = "Multiplication with addition assignment with evaluated matrix/vector";

         try {
            dres_   += eval( lhs_ ) * eval( rhs_ );
            sres_   += eval( lhs_ ) * eval( rhs_ );
            refres_ += eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   += eval( olhs_ ) * eval( rhs_ );
            sres_   += eval( olhs_ ) * eval( rhs_ );
            refres_ += eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }


      //=====================================================================================
      // Multiplication with subtraction assignment
      //=====================================================================================

      // Multiplication with subtraction assignment with the given matrix/vector
      {
         test_ = "Multiplication with subtraction assignment with the given matrix/vector";

         try {
            dres_   -= lhs_ * rhs_;
            sres_   -= lhs_ * rhs_;
            refres_ -= reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   -= olhs_ * rhs_;
            sres_   -= olhs_ * rhs_;
            refres_ -= reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }

      // Multiplication with subtraction assignment with evaluated matrix/vector
      {
         test_ = "Multiplication with subtraction assignment with evaluated matrix/vector";

         try {
            dres_   -= eval( lhs_ ) * eval( rhs_ );
            sres_   -= eval( lhs_ ) * eval( rhs_ );
            refres_ -= eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   -= eval( olhs_ ) * eval( rhs_ );
            sres_   -= eval( olhs_ ) * eval( rhs_ );
            refres_ -= eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }


      //=====================================================================================
      // Multiplication with multiplication assignment
      //=====================================================================================

      // Multiplication with multiplication assignment with the given matrix/vector
      {
         test_ = "Multiplication with multiplication assignment with the given matrix/vector";

         try {
            dres_   *= lhs_ * rhs_;
            sres_   *= lhs_ * rhs_;
            refres_ *= reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   *= olhs_ * rhs_;
            sres_   *= olhs_ * rhs_;
            refres_ *= reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }

      // Multiplication with multiplication assignment with evaluated matrix/vector
      {
         test_ = "Multiplication with multiplication assignment with evaluated matrix/vector";

         try {
            dres_   *= eval( lhs_ ) * eval( rhs_ );
            sres_   *= eval( lhs_ ) * eval( rhs_ );
            refres_ *= eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   *= eval( olhs_ ) * eval( rhs_ );
            sres_   *= eval( olhs_ ) * eval( rhs_ );
            refres_ *= eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the negated sparse matrix/dense vector multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the negated matrix/vector multiplication with plain assignment, addition
// assignment, subtraction assignment, and multiplication assignment. In case any error resulting
// from the multiplication or the subsequent assignment is detected, a \a std::runtime_error
// exception is thrown.
*/
template< typename MT    // Type of the left-hand side sparse matrix
        , typename VT >  // Type of the right-hand side dense vector
void SMatDVecMult<MT,VT>::testNegatedOperation()
{
#if BLAZETEST_MATHTEST_TEST_NEGATED_OPERATION
   if( BLAZETEST_MATHTEST_TEST_NEGATED_OPERATION > 1 )
   {
      //=====================================================================================
      // Negated multiplication
      //=====================================================================================

      // Negated multiplication with the given matrix/vector
      {
         test_ = "Negated multiplication with the given matrix/vector";

         try {
            dres_   = -( lhs_ * rhs_ );
            sres_   = -( lhs_ * rhs_ );
            refres_ = -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_ = -( olhs_ * rhs_ );
            sres_ = -( olhs_ * rhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }

      // Negated multiplication with evaluated matrix/vector
      {
         test_ = "Negated multiplication with evaluated matrix/vector";

         try {
            dres_ = -( eval( lhs_ ) * eval( rhs_ ) );
            sres_ = -( eval( lhs_ ) * eval( rhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_ = -( eval( olhs_ ) * eval( rhs_ ) );
            sres_ = -( eval( olhs_ ) * eval( rhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }


      //=====================================================================================
      // Negated multiplication with addition assignment
      //=====================================================================================

      // Negated multiplication with addition assignment with the given matrix/vector
      {
         test_ = "Negated multiplication with addition assignment with the given matrix/vector";

         try {
            dres_   += -( lhs_ * rhs_ );
            sres_   += -( lhs_ * rhs_ );
            refres_ += -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   += -( olhs_ * rhs_ );
            sres_   += -( olhs_ * rhs_ );
            refres_ += -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }

      // Negated multiplication with addition assignment with evaluated matrix/vector
      {
         test_ = "Negated multiplication with addition assignment with evaluated matrix/vector";

         try {
            dres_   += -( eval( lhs_ ) * eval( rhs_ ) );
            sres_   += -( eval( lhs_ ) * eval( rhs_ ) );
            refres_ += -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   += -( eval( olhs_ ) * eval( rhs_ ) );
            sres_   += -( eval( olhs_ ) * eval( rhs_ ) );
            refres_ += -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }


      //=====================================================================================
      // Negated multiplication with subtraction assignment
      //=====================================================================================

      // Negated multiplication with subtraction assignment with the given matrix/vector
      {
         test_ = "Negated multiplication with subtraction assignment with the given matrix/vector";

         try {
            dres_   -= -( lhs_ * rhs_ );
            sres_   -= -( lhs_ * rhs_ );
            refres_ -= -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   -= -( olhs_ * rhs_ );
            sres_   -= -( olhs_ * rhs_ );
            refres_ -= -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }

      // Negated multiplication with subtraction assignment with evaluated matrix/vector
      {
         test_ = "Negated multiplication with subtraction assignment with evaluated matrix/vector";

         try {
            dres_   -= -( eval( lhs_ ) * eval( rhs_ ) );
            sres_   -= -( eval( lhs_ ) * eval( rhs_ ) );
            refres_ -= -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   -= -( eval( olhs_ ) * eval( rhs_ ) );
            sres_   -= -( eval( olhs_ ) * eval( rhs_ ) );
            refres_ -= -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }


      //=====================================================================================
      // Negated multiplication with multiplication assignment
      //=====================================================================================

      // Negated multiplication with multiplication assignment with the given matrix/vector
      {
         test_ = "Negated multiplication with multiplication assignment with the given matrix/vector";

         try {
            dres_   *= -( lhs_ * rhs_ );
            sres_   *= -( lhs_ * rhs_ );
            refres_ *= -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   *= -( olhs_ * rhs_ );
            sres_   *= -( olhs_ * rhs_ );
            refres_ *= -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }

      // Negated multiplication with multiplication assignment with evaluated matrix/vector
      {
         test_ = "Negated multiplication with multiplication assignment with evaluated matrix/vector";

         try {
            dres_   *= -( eval( lhs_ ) * eval( rhs_ ) );
            sres_   *= -( eval( lhs_ ) * eval( rhs_ ) );
            refres_ *= -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   *= -( eval( olhs_ ) * eval( rhs_ ) );
            sres_   *= -( eval( olhs_ ) * eval( rhs_ ) );
            refres_ *= -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the scaled sparse matrix/dense vector multiplication.
//
// \param scalar The scalar value.
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the scaled matrix/vector multiplication with plain assignment, addition
// assignment, subtraction assignment, and multiplication assignment. In case any error resulting
// from the multiplication or the subsequent assignment is detected, a \a std::runtime_error
// exception is thrown.
*/
template< typename MT    // Type of the left-hand side sparse matrix
        , typename VT >  // Type of the right-hand side dense vector
template< typename T >   // Type of the scalar
void SMatDVecMult<MT,VT>::testScaledOperation( T scalar )
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
            sres_   = dres_;
            refres_ = dres_;

            dres_   *= scalar;
            sres_   *= scalar;
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

         checkResults<MT>();
      }


      //=====================================================================================
      // Self-scaling (OP/=s)
      //=====================================================================================

      // Self-scaling (OP/=s)
      {
         test_ = "Self-scaling (OP/=s)";

         try {
            dres_   = lhs_ * rhs_;
            sres_   = dres_;
            refres_ = dres_;

            dres_   /= scalar;
            sres_   /= scalar;
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

         checkResults<MT>();
      }


      //=====================================================================================
      // Scaled multiplication (s*OP)
      //=====================================================================================

      // Scaled multiplication with the given matrix/vector
      {
         test_ = "Scaled multiplication with the given matrix/vector (s*OP)";

         try {
            dres_   = scalar * ( lhs_ * rhs_ );
            sres_   = scalar * ( lhs_ * rhs_ );
            refres_ = scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_ = scalar * ( olhs_ * rhs_ );
            sres_ = scalar * ( olhs_ * rhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }

      // Scaled multiplication with evaluated matrix/vector
      {
         test_ = "Scaled multiplication with evaluated matrix/vector (s*OP)";

         try {
            dres_ = scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            sres_ = scalar * ( eval( lhs_ ) * eval( rhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_ = scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            sres_ = scalar * ( eval( olhs_ ) * eval( rhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }


      //=====================================================================================
      // Scaled multiplication (OP*s)
      //=====================================================================================

      // Scaled multiplication with the given matrix/vector
      {
         test_ = "Scaled multiplication with the given matrix/vector (OP*s)";

         try {
            dres_   = ( lhs_ * rhs_ ) * scalar;
            sres_   = ( lhs_ * rhs_ ) * scalar;
            refres_ = ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_ = ( olhs_ * rhs_ ) * scalar;
            sres_ = ( olhs_ * rhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }

      // Scaled multiplication with evaluated matrix/vector
      {
         test_ = "Scaled multiplication with evaluated matrix/vector (OP*s)";

         try {
            dres_  = ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            sres_  = ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_ = ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            sres_ = ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }


      //=====================================================================================
      // Scaled multiplication (OP/s)
      //=====================================================================================

      // Scaled multiplication with the given matrix/vector
      {
         test_ = "Scaled multiplication with the given matrix/vector (OP/s)";

         try {
            dres_   = ( lhs_ * rhs_ ) / scalar;
            sres_   = ( lhs_ * rhs_ ) / scalar;
            refres_ = ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_ = ( olhs_ * rhs_ ) / scalar;
            sres_ = ( olhs_ * rhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }

      // Scaled multiplication with evaluated matrix/vector
      {
         test_ = "Scaled multiplication with evaluated matrix/vector (OP/s)";

         try {
            dres_ = ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            sres_ = ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_ = ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            sres_ = ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }


      //=====================================================================================
      // Scaled multiplication with addition assignment (s*OP)
      //=====================================================================================

      // Scaled multiplication with addition assignment with the given matrix/vector
      {
         test_ = "Scaled multiplication with addition assignment with the given matrix/vector (s*OP)";

         try {
            dres_   += scalar * ( lhs_ * rhs_ );
            sres_   += scalar * ( lhs_ * rhs_ );
            refres_ += scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   += scalar * ( olhs_ * rhs_ );
            sres_   += scalar * ( olhs_ * rhs_ );
            refres_ += scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }

      // Scaled multiplication with addition assignment with evaluated matrix/vector
      {
         test_ = "Scaled multiplication with addition assignment with evaluated matrix/vector (s*OP)";

         try {
            dres_   += scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            sres_   += scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            refres_ += scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   += scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            sres_   += scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            refres_ += scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }


      //=====================================================================================
      // Scaled multiplication with addition assignment (OP*s)
      //=====================================================================================

      // Scaled multiplication with addition assignment with the given matrix/vector
      {
         test_ = "Scaled multiplication with addition assignment with the given matrix/vector (OP*s)";

         try {
            dres_   += ( lhs_ * rhs_ ) * scalar;
            sres_   += ( lhs_ * rhs_ ) * scalar;
            refres_ += ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   += ( olhs_ * rhs_ ) * scalar;
            sres_   += ( olhs_ * rhs_ ) * scalar;
            refres_ += ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }

      // Scaled multiplication with addition assignment with evaluated matrix/vector
      {
         test_ = "Scaled multiplication with addition assignment with evaluated matrix/vector (OP*s)";

         try {
            dres_   += ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            sres_   += ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            refres_ += ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   += ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            sres_   += ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            refres_ += ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }


      //=====================================================================================
      // Scaled multiplication with addition assignment (OP/s)
      //=====================================================================================

      // Scaled multiplication with addition assignment with the given matrix/vector
      {
         test_ = "Scaled multiplication with addition assignment with the given matrix/vector (OP/s)";

         try {
            dres_   += ( lhs_ * rhs_ ) / scalar;
            sres_   += ( lhs_ * rhs_ ) / scalar;
            refres_ += ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   += ( olhs_ * rhs_ ) / scalar;
            sres_   += ( olhs_ * rhs_ ) / scalar;
            refres_ += ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }

      // Scaled multiplication with addition assignment with evaluated matrix/vector
      {
         test_ = "Scaled multiplication with addition assignment with evaluated matrix/vector (OP/s)";

         try {
            dres_   += ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            sres_   += ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            refres_ += ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   += ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            sres_   += ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            refres_ += ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }


      //=====================================================================================
      // Scaled multiplication with subtraction assignment (s*OP)
      //=====================================================================================

      // Scaled multiplication with subtraction assignment with the given matrix/vector
      {
         test_ = "Scaled multiplication with subtraction assignment with the given matrix/vector (s*OP)";

         try {
            dres_   -= scalar * ( lhs_ * rhs_ );
            sres_   -= scalar * ( lhs_ * rhs_ );
            refres_ -= scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   -= scalar * ( olhs_ * rhs_ );
            sres_   -= scalar * ( olhs_ * rhs_ );
            refres_ -= scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }

      // Scaled multiplication with subtraction assignment with evaluated matrix/vector
      {
         test_ = "Scaled multiplication with subtraction assignment with evaluated matrix/vector (s*OP)";

         try {
            dres_   -= scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            sres_   -= scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            refres_ -= scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   -= scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            sres_   -= scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            refres_ -= scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }


      //=====================================================================================
      // Scaled multiplication with subtraction assignment (OP*s)
      //=====================================================================================

      // Scaled multiplication with subtraction assignment with the given matrix/vector
      {
         test_ = "Scaled multiplication with subtraction assignment with the given matrix/vector (OP*s)";

         try {
            dres_   -= ( lhs_ * rhs_ ) * scalar;
            sres_   -= ( lhs_ * rhs_ ) * scalar;
            refres_ -= ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   -= ( olhs_ * rhs_ ) * scalar;
            sres_   -= ( olhs_ * rhs_ ) * scalar;
            refres_ -= ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }

      // Scaled multiplication with subtraction assignment with evaluated matrix/vector
      {
         test_ = "Scaled multiplication with subtraction assignment with evaluated matrix/vector (OP*s)";

         try {
            dres_   -= ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            sres_   -= ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            refres_ -= ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   -= ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            sres_   -= ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            refres_ -= ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }


      //=====================================================================================
      // Scaled multiplication with subtraction assignment (OP/s)
      //=====================================================================================

      // Scaled multiplication with subtraction assignment with the given matrix/vector
      {
         test_ = "Scaled multiplication with subtraction assignment with the given matrix/vector (OP/s)";

         try {
            dres_   -= ( lhs_ * rhs_ ) / scalar;
            sres_   -= ( lhs_ * rhs_ ) / scalar;
            refres_ -= ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   -= ( olhs_ * rhs_ ) / scalar;
            sres_   -= ( olhs_ * rhs_ ) / scalar;
            refres_ -= ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }

      // Scaled multiplication with subtraction assignment with evaluated matrix/vector
      {
         test_ = "Scaled multiplication with subtraction assignment with evaluated matrix/vector (OP/s)";

         try {
            dres_   -= ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            sres_   -= ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            refres_ -= ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   -= ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            sres_   -= ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            refres_ -= ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }


      //=====================================================================================
      // Scaled multiplication with multiplication assignment (s*OP)
      //=====================================================================================

      // Scaled multiplication with multiplication assignment with the given matrix/vector
      {
         test_ = "Scaled multiplication with multiplication assignment with the given matrix/vector (s*OP)";

         try {
            dres_   *= scalar * ( lhs_ * rhs_ );
            sres_   *= scalar * ( lhs_ * rhs_ );
            refres_ *= scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   *= scalar * ( olhs_ * rhs_ );
            sres_   *= scalar * ( olhs_ * rhs_ );
            refres_ *= scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }

      // Scaled multiplication with multiplication assignment with evaluated matrix/vector
      {
         test_ = "Scaled multiplication with multiplication assignment with evaluated matrix/vector (s*OP)";

         try {
            dres_   *= scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            sres_   *= scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            refres_ *= scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   *= scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            sres_   *= scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            refres_ *= scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }


      //=====================================================================================
      // Scaled multiplication with multiplication assignment (OP*s)
      //=====================================================================================

      // Scaled multiplication with multiplication assignment with the given matrix/vector
      {
         test_ = "Scaled multiplication with multiplication assignment with the given matrix/vector (OP*s)";

         try {
            dres_   *= ( lhs_ * rhs_ ) * scalar;
            sres_   *= ( lhs_ * rhs_ ) * scalar;
            refres_ *= ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   *= ( olhs_ * rhs_ ) * scalar;
            sres_   *= ( olhs_ * rhs_ ) * scalar;
            refres_ *= ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }

      // Scaled multiplication with multiplication assignment with evaluated matrix/vector
      {
         test_ = "Scaled multiplication with multiplication assignment with evaluated matrix/vector (OP*s)";

         try {
            dres_   *= ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            sres_   *= ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            refres_ *= ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   *= ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            sres_   *= ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            refres_ *= ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }


      //=====================================================================================
      // Scaled multiplication with multiplication assignment (OP/s)
      //=====================================================================================

      // Scaled multiplication with multiplication assignment with the given matrix/vector
      {
         test_ = "Scaled multiplication with multiplication assignment with the given matrix/vector (OP/s)";

         try {
            dres_   *= ( lhs_ * rhs_ ) / scalar;
            sres_   *= ( lhs_ * rhs_ ) / scalar;
            refres_ *= ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   *= ( olhs_ * rhs_ ) / scalar;
            sres_   *= ( olhs_ * rhs_ ) / scalar;
            refres_ *= ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }

      // Scaled multiplication with multiplication assignment with evaluated matrix/vector
      {
         test_ = "Scaled multiplication with multiplication assignment with evaluated matrix/vector (OP/s)";

         try {
            dres_   *= ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            sres_   *= ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            refres_ *= ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   *= ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            sres_   *= ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            refres_ *= ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the transpose sparse matrix/dense vector multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the transpose matrix/vector multiplication with plain assignment,
// addition assignment, subtraction assignment, and multiplication assignment. In case any
// error resulting from the multiplication or the subsequent assignment is detected, a
// \a std::runtime_error exception is thrown.
*/
template< typename MT    // Type of the left-hand side sparse matrix
        , typename VT >  // Type of the right-hand side dense vector
void SMatDVecMult<MT,VT>::testTransposeOperation()
{
#if BLAZETEST_MATHTEST_TEST_TRANSPOSE_OPERATION
   if( BLAZETEST_MATHTEST_TEST_TRANSPOSE_OPERATION > 1 )
   {
      //=====================================================================================
      // Transpose multiplication
      //=====================================================================================

      // Transpose multiplication with the given matrix/vector
      {
         test_ = "Transpose multiplication with the given matrix/vector";

         try {
            tdres_   = trans( lhs_ * rhs_ );
            tsres_   = trans( lhs_ * rhs_ );
            trefres_ = trans( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkTransposeResults<MT>();

         try {
            tdres_  = trans( olhs_ * rhs_ );
            tsres_  = trans( olhs_ * rhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkTransposeResults<TMT>();
      }

      // Transpose multiplication with evaluated matrix/vector
      {
         test_ = "Transpose multiplication with evaluated matrix/vector";

         try {
            tdres_ = trans( eval( lhs_ ) * eval( rhs_ ) );
            tsres_ = trans( eval( lhs_ ) * eval( rhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkTransposeResults<MT>();

         try {
            tdres_ = trans( eval( olhs_ ) * eval( rhs_ ) );
            tsres_ = trans( eval( olhs_ ) * eval( rhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkTransposeResults<TMT>();
      }


      //=====================================================================================
      // Transpose multiplication with addition assignment
      //=====================================================================================

      // Transpose multiplication with addition assignment with the given matrix/vector
      {
         test_ = "Transpose multiplication with addition assignment with the given matrix/vector";

         try {
            tdres_   += trans( lhs_ * rhs_ );
            tsres_   += trans( lhs_ * rhs_ );
            trefres_ += trans( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkTransposeResults<MT>();

         try {
            tdres_   += trans( olhs_ * rhs_ );
            tsres_   += trans( olhs_ * rhs_ );
            trefres_ += trans( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkTransposeResults<TMT>();
      }

      // Transpose multiplication with addition assignment with evaluated matrix/vector
      {
         test_ = "Transpose multiplication with addition assignment with evaluated matrix/vector";

         try {
            tdres_   += trans( eval( lhs_ ) * eval( rhs_ ) );
            tsres_   += trans( eval( lhs_ ) * eval( rhs_ ) );
            trefres_ += trans( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkTransposeResults<MT>();

         try {
            tdres_   += trans( eval( olhs_ ) * eval( rhs_ ) );
            tsres_   += trans( eval( olhs_ ) * eval( rhs_ ) );
            trefres_ += trans( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkTransposeResults<TMT>();
      }


      //=====================================================================================
      // Transpose multiplication with subtraction assignment
      //=====================================================================================

      // Transpose multiplication with subtraction assignment with the given matrix/vector
      {
         test_ = "Transpose multiplication with subtraction assignment with the given matrix/vector";

         try {
            tdres_   -= trans( lhs_ * rhs_ );
            tsres_   -= trans( lhs_ * rhs_ );
            trefres_ -= trans( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkTransposeResults<MT>();

         try {
            tdres_   -= trans( olhs_ * rhs_ );
            tsres_   -= trans( olhs_ * rhs_ );
            trefres_ -= trans( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkTransposeResults<TMT>();
      }

      // Transpose multiplication with subtraction assignment with evaluated matrix/vector
      {
         test_ = "Transpose multiplication with subtraction assignment with evaluated matrix/vector";

         try {
            tdres_   -= trans( eval( lhs_ ) * eval( rhs_ ) );
            tsres_   -= trans( eval( lhs_ ) * eval( rhs_ ) );
            trefres_ -= trans( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkTransposeResults<MT>();

         try {
            tdres_   -= trans( eval( olhs_ ) * eval( rhs_ ) );
            tsres_   -= trans( eval( olhs_ ) * eval( rhs_ ) );
            trefres_ -= trans( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkTransposeResults<TMT>();
      }


      //=====================================================================================
      // Transpose multiplication with multiplication assignment
      //=====================================================================================

      // Transpose multiplication with multiplication assignment with the given matrix/vector
      {
         test_ = "Transpose multiplication with multiplication assignment with the given matrix/vector";

         try {
            tdres_   *= trans( lhs_ * rhs_ );
            tsres_   *= trans( lhs_ * rhs_ );
            trefres_ *= trans( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkTransposeResults<MT>();

         try {
            tdres_   *= trans( olhs_ * rhs_ );
            tsres_   *= trans( olhs_ * rhs_ );
            trefres_ *= trans( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkTransposeResults<TMT>();
      }

      // Transpose multiplication with multiplication assignment with evaluated matrix/vector
      {
         test_ = "Transpose multiplication with multiplication assignment with evaluated matrix/vector";

         try {
            tdres_   *= trans( eval( lhs_ ) * eval( rhs_ ) );
            tsres_   *= trans( eval( lhs_ ) * eval( rhs_ ) );
            trefres_ *= trans( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkTransposeResults<MT>();

         try {
            tdres_   *= trans( eval( olhs_ ) * eval( rhs_ ) );
            tsres_   *= trans( eval( olhs_ ) * eval( rhs_ ) );
            trefres_ *= trans( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkTransposeResults<TMT>();
      }
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the abs sparse matrix/dense vector multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the abs matrix/vector multiplication with plain assignment, addition
// assignment, subtraction assignment, and multiplication assignment. In case any error
// resulting from the multiplication or the subsequent assignment is detected, a
// \a std::runtime_error exception is thrown.
*/
template< typename MT    // Type of the left-hand side sparse matrix
        , typename VT >  // Type of the right-hand side dense vector
void SMatDVecMult<MT,VT>::testAbsOperation()
{
#if BLAZETEST_MATHTEST_TEST_ABS_OPERATION
   if( BLAZETEST_MATHTEST_TEST_ABS_OPERATION > 1 )
   {
      //=====================================================================================
      // Abs multiplication
      //=====================================================================================

      // Abs multiplication with the given matrix/vector
      {
         test_ = "Abs multiplication with the given matrix/vector";

         try {
            dres_   = abs( lhs_ * rhs_ );
            sres_   = abs( lhs_ * rhs_ );
            refres_ = abs( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_ = abs( olhs_ * rhs_ );
            sres_ = abs( olhs_ * rhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }

      // Abs multiplication with evaluated matrix/vector
      {
         test_ = "Abs multiplication with evaluated matrix/vector";

         try {
            dres_  = abs( eval( lhs_ ) * eval( rhs_ ) );
            sres_  = abs( eval( lhs_ ) * eval( rhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_ = abs( eval( olhs_ ) * eval( rhs_ ) );
            sres_ = abs( eval( olhs_ ) * eval( rhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }


      //=====================================================================================
      // Abs multiplication with addition assignment
      //=====================================================================================

      // Abs multiplication with addition assignment with the given matrix/vector
      {
         test_ = "Abs multiplication with addition assignment with the given matrix/vector";

         try {
            dres_   += abs( lhs_ * rhs_ );
            sres_   += abs( lhs_ * rhs_ );
            refres_ += abs( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   += abs( olhs_ * rhs_ );
            sres_   += abs( olhs_ * rhs_ );
            refres_ += abs( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }

      // Abs multiplication with addition assignment with evaluated matrix/vector
      {
         test_ = "Abs multiplication with addition assignment with evaluated matrix/vector";

         try {
            dres_   += abs( eval( lhs_ ) * eval( rhs_ ) );
            sres_   += abs( eval( lhs_ ) * eval( rhs_ ) );
            refres_ += abs( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   += abs( eval( olhs_ ) * eval( rhs_ ) );
            sres_   += abs( eval( olhs_ ) * eval( rhs_ ) );
            refres_ += abs( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed addition assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }


      //=====================================================================================
      // Abs multiplication with subtraction assignment
      //=====================================================================================

      // Abs multiplication with subtraction assignment with the given matrix/vector
      {
         test_ = "Abs multiplication with subtraction assignment with the given matrix/vector";

         try {
            dres_   -= abs( lhs_ * rhs_ );
            sres_   -= abs( lhs_ * rhs_ );
            refres_ -= abs( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   -= abs( olhs_ * rhs_ );
            sres_   -= abs( olhs_ * rhs_ );
            refres_ -= abs( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }

      // Abs multiplication with subtraction assignment with evaluated matrix/vector
      {
         test_ = "Abs multiplication with subtraction assignment with evaluated matrix/vector";

         try {
            dres_   -= abs( eval( lhs_ ) * eval( rhs_ ) );
            sres_   -= abs( eval( lhs_ ) * eval( rhs_ ) );
            refres_ -= abs( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   -= abs( eval( olhs_ ) * eval( rhs_ ) );
            sres_   -= abs( eval( olhs_ ) * eval( rhs_ ) );
            refres_ -= abs( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed subtraction assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }


      //=====================================================================================
      // Abs multiplication with multiplication assignment
      //=====================================================================================

      // Abs multiplication with multiplication assignment with the given matrix/vector
      {
         test_ = "Abs multiplication with multiplication assignment with the given matrix/vector";

         try {
            dres_   *= abs( lhs_ * rhs_ );
            sres_   *= abs( lhs_ * rhs_ );
            refres_ *= abs( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   *= abs( olhs_ * rhs_ );
            sres_   *= abs( olhs_ * rhs_ );
            refres_ *= abs( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
      }

      // Abs multiplication with multiplication assignment with evaluated matrix/vector
      {
         test_ = "Abs multiplication with multiplication assignment with evaluated matrix/vector";

         try {
            dres_   *= abs( eval( lhs_ ) * eval( rhs_ ) );
            sres_   *= abs( eval( lhs_ ) * eval( rhs_ ) );
            refres_ *= abs( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side row-major sparse matrix type:\n"
                << "     " << typeid( MT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT>();

         try {
            dres_   *= abs( eval( olhs_ ) * eval( rhs_ ) );
            sres_   *= abs( eval( olhs_ ) * eval( rhs_ ) );
            refres_ *= abs( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed multiplication assignment operation\n"
                << " Details:\n"
                << "   Left-hand side column-major sparse matrix type:\n"
                << "     " << typeid( TMT ).name() << "\n"
                << "   Right-hand side dense vector type:\n"
                << "     " << typeid( VT ).name() << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<TMT>();
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
// This function is called after each test case to check and compare the computed results.
// The template argument \a LT indicates the types of the left-hand side operand used for
// the computations.
*/
template< typename MT    // Type of the left-hand side sparse matrix
        , typename VT >  // Type of the right-hand side dense vector
template< typename LT >  // Type of the left-hand side operand
void SMatDVecMult<MT,VT>::checkResults()
{
   using blaze::IsRowMajorMatrix;

   if( !isEqual( dres_, refres_ ) ) {
      std::ostringstream oss;
      oss.precision( 20 );
      oss << " Test : " << test_ << "\n"
          << " Error: Incorrect dense result detected\n"
          << " Details:\n"
          << "   Left-hand side " << ( IsRowMajorMatrix<LT>::value ? ( "row-major" ) : ( "column-major" ) ) << " sparse matrix type:\n"
          << "     " << typeid( LT ).name() << "\n"
          << "   Right-hand side dense vector type:\n"
          << "     " << typeid( VT ).name() << "\n"
          << "   Result:\n" << dres_ << "\n"
          << "   Expected result:\n" << refres_ << "\n";
      throw std::runtime_error( oss.str() );
   }

   if( !isEqual( sres_, refres_ ) ) {
      std::ostringstream oss;
      oss.precision( 20 );
      oss << " Test : " << test_ << "\n"
          << " Error: Incorrect sparse result detected\n"
          << " Details:\n"
          << "   Left-hand side " << ( IsRowMajorMatrix<LT>::value ? ( "row-major" ) : ( "column-major" ) ) << " sparse matrix type:\n"
          << "     " << typeid( LT ).name() << "\n"
          << "   Right-hand side dense vector type:\n"
          << "     " << typeid( VT ).name() << "\n"
          << "   Result:\n" << sres_ << "\n"
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
// results. The template argument \a LT indicates the types of the left-hand side operand
// used for the computations.
*/
template< typename MT    // Type of the left-hand side sparse matrix
        , typename VT >  // Type of the right-hand side dense vector
template< typename LT >  // Type of the left-hand side operand
void SMatDVecMult<MT,VT>::checkTransposeResults()
{
   using blaze::IsRowMajorMatrix;

   if( !isEqual( tdres_, trefres_ ) ) {
      std::ostringstream oss;
      oss.precision( 20 );
      oss << " Test : " << test_ << "\n"
          << " Error: Incorrect dense result detected\n"
          << " Details:\n"
          << "   Left-hand side " << ( IsRowMajorMatrix<LT>::value ? ( "row-major" ) : ( "column-major" ) ) << " sparse matrix type:\n"
          << "     " << typeid( LT ).name() << "\n"
          << "   Right-hand side dense vector type:\n"
          << "     " << typeid( VT ).name() << "\n"
          << "   Transpose result:\n" << tdres_ << "\n"
          << "   Expected transpose result:\n" << trefres_ << "\n";
      throw std::runtime_error( oss.str() );
   }

   if( !isEqual( tsres_, trefres_ ) ) {
      std::ostringstream oss;
      oss.precision( 20 );
      oss << " Test : " << test_ << "\n"
          << " Error: Incorrect sparse result detected\n"
          << " Details:\n"
          << "   Left-hand side " << ( IsRowMajorMatrix<LT>::value ? ( "row-major" ) : ( "column-major" ) ) << " sparse matrix type:\n"
          << "     " << typeid( LT ).name() << "\n"
          << "   Right-hand side dense vector type:\n"
          << "     " << typeid( VT ).name() << "\n"
          << "   Transpose result:\n" << tsres_ << "\n"
          << "   Expected transpose result:\n" << trefres_ << "\n";
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
/*!\brief Testing the matrix/vector multiplication between two specific types.
//
// \param creator1 The creator for the left-hand side matrix.
// \param creator2 The creator for the right-hand side vector.
// \return void
*/
template< typename MT    // Type of the left-hand side sparse matrix
        , typename VT >  // Type of the right-hand side dense vector
void runTest( const Creator<MT>& creator1, const Creator<VT>& creator2 )
{
   for( size_t rep=0; rep<repetitions; ++rep ) {
      SMatDVecMult<MT,VT>( creator1, creator2 );
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
/*!\brief Macro for the execution of a sparse matrix/dense vector multiplication test case.
*/
#define RUN_SMATDVECMULT_TEST( C1, C2 ) \
   blazetest::mathtest::smatdvecmult::runTest( C1, C2 )
/*! \endcond */
//*************************************************************************************************

} // namespace smatdvecmult

} // namespace mathtest

} // namespace blazetest

#endif
