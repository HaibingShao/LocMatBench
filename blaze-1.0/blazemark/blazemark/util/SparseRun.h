//=================================================================================================
/*!
//  \file blazemark/util/SparseRun.h
//  \brief Header file for the SparseRun class
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

#ifndef _BLAZEMARK_UTIL_SPARSERUN_H_
#define _BLAZEMARK_UTIL_SPARSERUN_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <iomanip>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <blaze/math/Functions.h>
#include <blaze/math/Infinity.h>
#include <blaze/math/shims/Equal.h>
#include <blaze/util/UnsignedValue.h>
#include <blazemark/system/Types.h>


namespace blazemark {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Data structure for the parameters of a benchmark run with sparse vectors and/or matrices.
//
// This auxiliary data structure represents the necessary parameters for a benchmark run with
// sparse vectors and/or matrices.
*/
class SparseRun
{
 private:
   //**Constructors********************************************************************************
   /*!\name Constructors */
   //@{
   explicit inline SparseRun();
   //@}
   //**********************************************************************************************

 public:
   //**Constructors********************************************************************************
   /*!\name Constructors */
   //@{
   explicit inline SparseRun( size_t size, size_t nonzeros );
   explicit inline SparseRun( size_t size, size_t nonzeros, size_t steps );
   // No explicitly declared copy constructor.
   //@}
   //**********************************************************************************************

   //**Destructor**********************************************************************************
   // No explicitly declared destructor.
   //**********************************************************************************************

   //**Copy assignment operator********************************************************************
   // No explicitly declared copy assignment operator.
   //**********************************************************************************************

   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
   inline size_t getSize           () const;
   inline size_t getNonZeros       () const;
   inline float  getFillingDegree  () const;
   inline size_t getSteps          () const;
   inline double getClikeResult    () const;
   inline double getClassicResult  () const;
   inline double getBlazeResult    () const;
   inline double getBoostResult    () const;
   inline double getBlitzResult    () const;
   inline double getGMMResult      () const;
   inline double getArmadilloResult() const;
   inline double getMTLResult      () const;
   inline double getEigenResult    () const;

   inline void   setSize    ( size_t newSize     );
   inline void   setNonZeros( size_t newNonZeros );
   inline void   setSteps   ( size_t newSteps    );
   inline void   setClikeResult    ( double result );
   inline void   setClassicResult  ( double result );
   inline void   setBlazeResult    ( double result );
   inline void   setBoostResult    ( double result );
   inline void   setBlitzResult    ( double result );
   inline void   setGMMResult      ( double result );
   inline void   setArmadilloResult( double result );
   inline void   setMTLResult      ( double result );
   inline void   setEigenResult    ( double result );
   //@}
   //**********************************************************************************************

 private:
   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   size_t size_;       //!< The target size of the sparse vectors/matrices.
                       /*!< In case of a sparse vector, this value directly corresponds to the size
                            of the vector, in case of a sparse matrix \a size_ corresponds to the
                            number of rows and columns. */
   size_t nonzeros_;   //!< The number of non-zero elements in the sparse vectors/matrices.
                       /*!< In case of a sparse vector, this value directly corresponds to the
                            number of sparse elements. In case of a sparse matrix, the value
                            specifies the number of non-zero elements per row. */
   size_t steps_;      //!< The number of steps for the benchmark run.
                       /*!< The (composite) arithmetic operation of each benchmark is run several
                            times to guarantee reasonable runtimes. \a steps_ corresponds to the
                            number of performed iterations. */
   double clike_;      //!< Benchmark result of the C-like implementation.
   double classic_;    //!< Benchmark result of classic C++ operator overloading.
   double blaze_;      //!< Benchmark result of the Blaze library.
   double boost_;      //!< Benchmark result of the Boost uBLAS library.
   double blitz_;      //!< Benchmark result of the Blitz++ library.
   double gmm_;        //!< Benchmark result of the GMM++ library.
   double armadillo_;  //!< Benchmark result of the Armadillo library.
   double mtl_;        //!< Benchmark result of the MTL4 library.
   double eigen_;      //!< Benchmark result of the Eigen3 library.
   //@}
   //**********************************************************************************************

   //**Friend declarations*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   template< typename > friend class Parser;
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
/*!\brief Default constructor for the SparseRun class.
//
// The default constructor in exclusively accessible for the blazemark::Parser class.
*/
inline SparseRun::SparseRun()
   : size_     ( 0   )  // The target size of the sparse vectors/matrices
   , nonzeros_ ( 0   )  // The number of non-zero elements in the sparse vectors/matrices.
   , steps_    ( 0   )  // The number of steps for the benchmark run
   , clike_    ( 0.0 )  // Benchmark result of the C-like implementation
   , classic_  ( 0.0 )  // Benchmark result of the classic C++ implementation
   , blaze_    ( 0.0 )  // Benchmark result of the Blaze library
   , boost_    ( 0.0 )  // Benchmark result of the Boost uBLAS library
   , blitz_    ( 0.0 )  // Benchmark result of the Blitz++ library
   , gmm_      ( 0.0 )  // Benchmark result of the GMM++ library
   , armadillo_( 0.0 )  // Benchmark result of the Armadillo library
   , mtl_      ( 0.0 )  // Benchmark result of the MTL4 library
   , eigen_    ( 0.0 )  // Benchmark result of the Eigen3 library
{}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Two-argument constructor for the SparseRun class.
//
// \param size The size of the sparse vector or matrix \f$ [1..\infty) \f$.
// \param nonzeros The number of non-zero elements in the sparse vectors/matrices \f$ [1..size] \f$.
// \exception std::invalid_argument Invalid size parameter.
// \exception std::invalid_argument Invalid number of non-zero elements.
//
// This constructor creates a sparse run with a specified target size for the sparse vectors
// and/or matrices and a specified number of non-zero elements. The number of steps will
// automatically be evaluated to guarantuee an approximate runtime of blazemark::runtime
// seconds (see for the 'blazemark/config/Config.h' file for more details).
*/
inline SparseRun::SparseRun( size_t size, size_t nonzeros )
   : size_     ( size     )  // The target size of the sparse vectors/matrices
   , nonzeros_ ( nonzeros )  // The number of non-zero elements in the sparse vectors/matrices.
   , steps_    ( 0        )  // The number of steps for the benchmark run
   , clike_    ( 0.0      )  // Benchmark result of the C-like implementation
   , classic_  ( 0.0      )  // Benchmark result of the classic C++ implementation
   , blaze_    ( 0.0      )  // Benchmark result of the Blaze library
   , boost_    ( 0.0      )  // Benchmark result of the Boost uBLAS library
   , blitz_    ( 0.0      )  // Benchmark result of the Blitz++ library
   , gmm_      ( 0.0      )  // Benchmark result of the GMM++ library
   , armadillo_( 0.0      )  // Benchmark result of the Armadillo library
   , mtl_      ( 0.0      )  // Benchmark result of the MTL4 library
   , eigen_    ( 0.0      )  // Benchmark result of the Eigen3 library
{
   // Checking the target size for the sparse vectors/matrices
   if( size_ == size_t(0) )
      throw std::invalid_argument( "Invalid size parameter" );

   // Checking the number of non-zero elements
   if( nonzeros_ == size_t(0) || nonzeros_ > size_ )
      throw std::invalid_argument( "Invalid number of non-zero elements" );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Three-argument constructor for the SparseRun class.
//
// \param size The size of the sparse vector or matrix \f$ [1..\infty) \f$.
// \param nonzeros The number of non-zero elements in the sparse vectors/matrices \f$ [1..size] \f$.
// \param steps The number of steps for the benchmark \f$ [1..\infty) \f$.
// \exception std::invalid_argument Invalid size parameter.
// \exception std::invalid_argument Invalid number of non-zero elements.
//
// This constructor creates a sparse run with a specified target size for the sparse vectors
// and/or matrcies, a specfied number of non-zero elements, and a specified number of steps
// for the benchmark. In case \a steps is zero, the number of steps will automatically be
// evaluated to guarantee an approximate runtime of blazemark::runtime seconds (see for
// the 'blazemark/config/Config.h' file for more details).
*/
inline SparseRun::SparseRun( size_t size, size_t nonzeros, size_t steps )
   : size_     ( size     )  // The target size of the sparse vectors/matrices
   , nonzeros_ ( nonzeros )  // The number of non-zero elements in the sparse vectors/matrices
   , steps_    ( steps    )  // The number of steps for the benchmark run
   , clike_    ( 0.0      )  // Benchmark result of the C-like implementation
   , classic_  ( 0.0      )  // Benchmark result of the classic C++ implementation
   , blaze_    ( 0.0      )  // Benchmark result of the Blaze library
   , boost_    ( 0.0      )  // Benchmark result of the Boost uBLAS library
   , blitz_    ( 0.0      )  // Benchmark result of the Blitz++ library
   , gmm_      ( 0.0      )  // Benchmark result of the GMM++ library
   , armadillo_( 0.0      )  // Benchmark result of the Armadillo library
   , mtl_      ( 0.0      )  // Benchmark result of the MTL4 library
   , eigen_    ( 0.0      )  // Benchmark result of the Eigen3 library
{
   // Checking the target size for the sparse vectors/matrices
   if( size_ == size_t(0) )
      throw std::invalid_argument( "Invalid size parameter" );

   // Checking the number of non-zero elements
   if( nonzeros_ == size_t(0) || nonzeros_ > size_ )
      throw std::invalid_argument( "Invalid number of non-zero elements" );
}
//*************************************************************************************************




//=================================================================================================
//
//  UTILITY FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Returns the target size of the sparse vectors/matrices of the benchmark run.
//
// \return The target size of the vectors/matrices.
*/
inline size_t SparseRun::getSize() const
{
   return size_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the number of non-zero elements of the sparse vectors/matrices of the benchmark run.
//
// \return The number of non-zero elements of the vectors/matrices.
*/
inline size_t SparseRun::getNonZeros() const
{
   return nonzeros_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the filling degree of the sparse vectors/matrices of the benchmark run.
//
// \return The filling degree of the vectors/matrices in percent.
*/
inline float SparseRun::getFillingDegree() const
{
   return float(nonzeros_) / float(size_) * 100.0F;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the number of steps of the benchmark run.
//
// \return The number of steps of the benchmark run.
*/
inline size_t SparseRun::getSteps() const
{
   return steps_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the benchmark result of the C-like implementation.
//
// \return The result of the C-like implementation.
*/
inline double SparseRun::getClikeResult() const
{
   return clike_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the benchmark result of the classic C++ implementation.
//
// \return The result of the classic C++ implementation.
*/
inline double SparseRun::getClassicResult() const
{
   return classic_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the benchmark result of the Blaze library.
//
// \return The result of the Blaze library.
*/
inline double SparseRun::getBlazeResult() const
{
   return blaze_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the benchmark result of the Boost uBLAS library.
//
// \return The result of the Boost uBLAS library.
*/
inline double SparseRun::getBoostResult() const
{
   return boost_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the benchmark result of the Blitz++ library.
//
// \return The result of the Blitz++ library.
*/
inline double SparseRun::getBlitzResult() const
{
   return blitz_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the benchmark result of the GMM++ library.
//
// \return The result of the GMM++ library.
*/
inline double SparseRun::getGMMResult() const
{
   return gmm_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the benchmark result of the Armadillo library.
//
// \return The result of the Armadillo library.
*/
inline double SparseRun::getArmadilloResult() const
{
   return armadillo_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the benchmark result of the MTL4 library.
//
// \return The result of the MTL4 library.
*/
inline double SparseRun::getMTLResult() const
{
   return mtl_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the benchmark result of the Eigen3 library.
//
// \return The result of the Eigen3 library.
*/
inline double SparseRun::getEigenResult() const
{
   return eigen_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Setting the target size of the vectors/matrices of the benchmark run.
//
// \param newSize The new target size of the vectors/matrices of the benchmark run.
// \return void
// \exception std::invalid_argument Invalid size parameter.
*/
inline void SparseRun::setSize( size_t newSize )
{
   if( newSize == size_t(0) )
      throw std::invalid_argument( "Invalid size parameter" );
   size_ = newSize;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Setting the number of non-zero elements of the vectors/matrices of the benchmark run.
//
// \param newNonZeros The new number of non-zero elements of the vectors/matrices of the benchmark run.
// \return void
// \exception std::invalid_argument Invalid number of non-zero elements.
*/
inline void SparseRun::setNonZeros( size_t newNonZeros )
{
   if( newNonZeros == size_t(0) )
      throw std::invalid_argument( "Invalid number of non-zero elements" );
   nonzeros_ = newNonZeros;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Setting the number of steps of the benchmark run.
//
// \param newSteps The new number of steps of the benchmark run.
// \return void
*/
inline void SparseRun::setSteps( size_t newSteps )
{
   steps_ = newSteps;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Setting the benchmark result of the C-like implementation.
//
// \param result The result of the C-like implementation.
// \return void
// \exception std::invalid_argument Invalid result value.
*/
inline void SparseRun::setClikeResult( double result )
{
   if( result < 0.0 )
      throw std::invalid_argument( "Invalid result value" );
   clike_ = result;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Setting the benchmark result of the classic C++ implementation.
//
// \param result The result of the classic C++ implementation.
// \return void
// \exception std::invalid_argument Invalid result value.
*/
inline void SparseRun::setClassicResult( double result )
{
   if( result < 0.0 )
      throw std::invalid_argument( "Invalid result value" );
   classic_ = result;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Setting the benchmark result of the Blaze library.
//
// \param result The result of the Blaze library.
// \return void
// \exception std::invalid_argument Invalid result value.
*/
inline void SparseRun::setBlazeResult( double result )
{
   if( result < 0.0 )
      throw std::invalid_argument( "Invalid result value" );
   blaze_ = result;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Setting the benchmark result of the Boost uBLAS library.
//
// \param result The result of the Boost uBLAS library.
// \return void
// \exception std::invalid_argument Invalid result value.
*/
inline void SparseRun::setBoostResult( double result )
{
   if( result < 0.0 )
      throw std::invalid_argument( "Invalid result value" );
   boost_ = result;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Setting the benchmark result of the Blitz++ library.
//
// \param result The result of the Blitz++ library.
// \return void
// \exception std::invalid_argument Invalid result value.
*/
inline void SparseRun::setBlitzResult( double result )
{
   if( result < 0.0 )
      throw std::invalid_argument( "Invalid result value" );
   blitz_ = result;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Setting the benchmark result of the GMM++ library.
//
// \param result The result of the GMM++ library.
// \return void
// \exception std::invalid_argument Invalid result value.
*/
inline void SparseRun::setGMMResult( double result )
{
   if( result < 0.0 )
      throw std::invalid_argument( "Invalid result value" );
   gmm_ = result;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Setting the benchmark result of the Armadillo library.
//
// \param result The result of the Armadillo library.
// \return void
// \exception std::invalid_argument Invalid result value.
*/
inline void SparseRun::setArmadilloResult( double result )
{
   if( result < 0.0 )
      throw std::invalid_argument( "Invalid result value" );
   armadillo_ = result;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Setting the benchmark result of the MTL4 library.
//
// \param result The result of the MTL4 library.
// \return void
// \exception std::invalid_argument Invalid result value.
*/
inline void SparseRun::setMTLResult( double result )
{
   if( result < 0.0 )
      throw std::invalid_argument( "Invalid result value" );
   mtl_ = result;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Setting the benchmark result of the Eigen3 library.
//
// \param result The result of the Eigen3 library.
// \return void
// \exception std::invalid_argument Invalid result value.
*/
inline void SparseRun::setEigenResult( double result )
{
   if( result < 0.0 )
      throw std::invalid_argument( "Invalid result value" );
   eigen_ = result;
}
//*************************************************************************************************




//=================================================================================================
//
//  GLOBAL OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Less-than comparison between two SparseRun objects.
//
// \param lhs The left-hand side SparseRun object.
// \param rhs The right-hand side SparseRun object.
// \return \a true if the left value is less than the right value, \a false if not.
//
// SparseRun objects are primarily sorted according to the nonzeros value and secondarily
// according to the size value: In case the nonzeros value of the left-hand side SparseRun
// object is smaller or in case both nonzeros values are equal and the size value of the
// left-hand side is smaller the function returns \a true. Otherwise the function returns
// \a false.
*/
inline bool operator<( const SparseRun& lhs, const SparseRun& rhs )
{
   return ( lhs.getFillingDegree() < rhs.getFillingDegree() ) ||
          ( ::blaze::equal( lhs.getFillingDegree(), rhs.getFillingDegree() ) && lhs.getSize() < rhs.getSize() );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Global output operator for the SparseRun class.
//
// \param os Reference to the output stream.
// \param run Reference to a SparseRun object.
// \return The output stream.
*/
inline std::ostream& operator<<( std::ostream& os, const SparseRun& run )
{
   const std::ios::fmtflags flags( os.flags() );

   os << std::left << "   N=" << run.getSize() << ", fill level=" << run.getFillingDegree() << "%, steps=" << run.getSteps() << "\n";

   const double clike    ( run.getClikeResult()     );
   const double classic  ( run.getClassicResult()   );
   const double blaze    ( run.getBlazeResult()     );
   const double boost    ( run.getBoostResult()     );
   const double blitz    ( run.getBlitzResult()     );
   const double gmm      ( run.getGMMResult()       );
   const double armadillo( run.getArmadilloResult() );
   const double mtl      ( run.getMTLResult()       );
   const double eigen    ( run.getEigenResult()     );

   double minTime = ::blaze::inf;

   if( clike     != 0.0 ) minTime = ::blaze::min( minTime, clike     );
   if( classic   != 0.0 ) minTime = ::blaze::min( minTime, classic   );
   if( blaze     != 0.0 ) minTime = ::blaze::min( minTime, blaze     );
   if( boost     != 0.0 ) minTime = ::blaze::min( minTime, boost     );
   if( blitz     != 0.0 ) minTime = ::blaze::min( minTime, blitz     );
   if( gmm       != 0.0 ) minTime = ::blaze::min( minTime, gmm       );
   if( armadillo != 0.0 ) minTime = ::blaze::min( minTime, armadillo );
   if( mtl       != 0.0 ) minTime = ::blaze::min( minTime, mtl       );
   if( eigen     != 0.0 ) minTime = ::blaze::min( minTime, eigen     );

   if( clike     != 0.0 ) os << "     C-like      = " << std::setw(8) << ( clike     / minTime ) << " (" << clike     << ")\n";
   if( classic   != 0.0 ) os << "     Classic     = " << std::setw(8) << ( classic   / minTime ) << " (" << classic   << ")\n";
   if( blaze     != 0.0 ) os << "     Blaze       = " << std::setw(8) << ( blaze     / minTime ) << " (" << blaze     << ")\n";
   if( boost     != 0.0 ) os << "     Boost uBLAS = " << std::setw(8) << ( boost     / minTime ) << " (" << boost     << ")\n";
   if( blitz     != 0.0 ) os << "     Blitz++     = " << std::setw(8) << ( blitz     / minTime ) << " (" << blitz     << ")\n";
   if( gmm       != 0.0 ) os << "     GMM++       = " << std::setw(8) << ( gmm       / minTime ) << " (" << gmm       << ")\n";
   if( armadillo != 0.0 ) os << "     Armadillo   = " << std::setw(8) << ( armadillo / minTime ) << " (" << armadillo << ")\n";
   if( mtl       != 0.0 ) os << "     MTL         = " << std::setw(8) << ( mtl       / minTime ) << " (" << mtl       << ")\n";
   if( eigen     != 0.0 ) os << "     Eigen       = " << std::setw(8) << ( eigen     / minTime ) << " (" << eigen     << ")\n";

   os << std::flush;

   os.flags( flags );
   return os;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Global input operator for the SparseRun class.
//
// \param is Reference to the input stream.
// \param run Reference to a SparseRun object.
// \return The input stream.
//
// The input operator guarantees that this object is not changed in the case of an input error.
// Only values suitable for the according built-in unsigned integral data type \a T are allowed.
// Otherwise, the input stream's position is returned to its previous position and the
// \a std::istream::failbit is set.
*/
inline std::istream& operator>>( std::istream& is, SparseRun& run )
{
   char c1, c2, c3;
   ::blaze::UnsignedValue<size_t> size, nonzeros, steps;
   const std::istream::pos_type pos( is.tellg() );

   if( !(is >> c1 >> size >> c2 >> nonzeros >> c3) ||
       c1 != '(' || size == 0 || c2 != ',' || nonzeros == 0 ||
       ( c3 != ')' && ( c3 != ',' || ( !(is >> steps >> c1) || c1 != ')' || steps == 0 ) ) ) )
   {
      is.clear();
      is.seekg( pos );
      is.setstate( std::istream::failbit );
      return is;
   }

   run.setSize( size );
   run.setNonZeros( nonzeros );
   run.setSteps( steps );

   return is;
}
//*************************************************************************************************

} // namespace blazemark

#endif
