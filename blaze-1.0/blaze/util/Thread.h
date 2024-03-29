//=================================================================================================
/*!
//  \file blaze/util/Thread.h
//  \brief Header file for the Thread class
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

#ifndef _BLAZE_UTIL_THREAD_H_
#define _BLAZE_UTIL_THREAD_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <boost/scoped_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <blaze/util/NonCopyable.h>


namespace blaze {

//=================================================================================================
//
//  ::blaze NAMESPACE FORWARD DECLARATIONS
//
//=================================================================================================

class ThreadPool;




//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Implementation of a single thread of execution.
// \ingroup threads
//
// \section thread_general General
//
// The Thread class represents a thread of execution for the parallel execution of concurrent
// tasks. Each Thread object incorporates a single thread of execution, or Not-a-Thread, and
// at most one Thread object incorporates a given thread of execution since it is not possible
// to copy a Thread. The implementation of the Thread class is based on the implementation of
// the boost library. For more information about boost threads, see the current documentation
// at the boost homepage: www.boost.org.
//
//
// \section thread_setup Creating individual threads
//
// The Blaze library provides the functionality to create individual threads for specific tasks,
// or to create thread pools for the execution of a larger number of tasks (see the ThreadPool
// class description). The following example demonstrates the setup of individual threads to
// handle specific tasks. In this example, a function without arguments and a functor with two
// arguments are executed in parallel by two distinct threads:

   \code
   // Definition of a function with no arguments that returns void
   void function0() { ... }

   // Definition of a functor (function object) taking two arguments and returning void
   struct Functor2
   {
      void operator()( int a, int b ) { ... }
   };

   int main()
   {
      // Creating a new thread executing the zero argument function.
      blaze::Thread thread1( function0 );

      // Waiting for the thread to finish its task
      thread1.join();

      // After the thread has completed its tasks, it is not possible to reassign it a
      // new task. Therefore it is necessary to create a new thread for optional follow
      // up tasks.

      // Creating a new thread executing the binary functor.
      blaze::Thread thread2( Functor2(), 4, 6 );

      // Waiting for the second thread to finish its task
      thread2.join();
   }
   \endcode

// Note that the Thread class allows for up to five arguments for the given functions/functors.
// Also note that the two tasks are not executed in parallel since the join() function is used
// to wait for each thread's completion.
//
//
// \section thread_exception Throwing exceptions in a thread parallel environment
//
// It can happen that during the execution of a given task a thread encounters an erroneous
// situation and has to throw an exception. However, exceptions thrown in the usual way
// cannot be caught by a try-catch-block in the main thread of execution:

   \code
   // Definition of a function throwing a std::runtime_error during its execution
   void task()
   {
      ...
      throw std::runtime_error( ... );
      ...
   }

   // Creating a thread executing the throwing function. Although the setup, execution and
   // destruction of the thread are encapsuled inside a try-catch-block, the exception cannot
   // be caught and results in an abortion of the program.
   try {
      Thread thread( task );
      thread.join();
   }
   catch( ... )
   {
      ...
   }
   \endcode

// The only possible way to transport exceptions between threads is to use the according boost
// functionality demonstrated in the following example. Note that any function/functor passed
// to a thread is responsible to handle exceptions in this way!

   \code
   #include <boost/bind.hpp>
   #include <boost/exception_ptr.hpp>

   // Definition of a function that happens to throw an exception. In order to throw the
   // exception, boost::enable_current_exception() is used in combination with throw.
   void throwException()
   {
      ...
      throw boost::enable_current_exception( std::runtime_error( ... ) );
      ...
   }

   // Definition of a thread function. The try-catch-block catches the exception and uses the
   // boost::current_exception() function to get a boost::exception_ptr object.
   void task( boost::exception_ptr& error )
   {
      try {
         throwException();
         error = boost::exception_ptr();
      }
      catch( ... ) {
         error = boost::current_exception();
      }
   }

   // The function that start a thread of execution can pass along a boost::exception_ptr object
   // that is set in case of an exception. Note that boost::current_exception() captures the
   // original type of the exception object. The exception can be thrown again using the
   // boost::rethrow_exception() function.
   void work()
   {
      boost::exception_ptr error;

      Thread thread( boost::bind( task, boost::ref(error) ) );
      thread.join();

      if( error ) {
         std::cerr << " Exception during thread execution!\n\n";
         boost::rethrow_exception( error );
      }
   }
   \endcode
*/
class Thread : private NonCopyable
{
 private:
   //**Type definitions****************************************************************************
   typedef boost::scoped_ptr<boost::thread>  ThreadHandle;  //!< Handle for a single thread.
   //**********************************************************************************************

   //**Constructors********************************************************************************
   /*!\name Constructors */
   //@{
   explicit Thread( ThreadPool* pool );
   //@}
   //**********************************************************************************************

 public:
   //**Constructors********************************************************************************
   /*!\name Constructors */
   //@{
   template< typename Callable >
   explicit inline Thread( Callable func );

   template< typename Callable, typename A1 >
   explicit inline Thread( Callable func, A1 a1 );

   template< typename Callable, typename A1, typename A2 >
   explicit inline Thread( Callable func, A1 a1, A2 a2 );

   template< typename Callable, typename A1, typename A2, typename A3 >
   explicit inline Thread( Callable func, A1 a1, A2 a2, A3 a3 );

   template< typename Callable, typename A1, typename A2, typename A3, typename A4 >
   explicit inline Thread( Callable func, A1 a1, A2 a2, A3 a3, A4 a4 );

   template< typename Callable, typename A1, typename A2, typename A3, typename A4, typename A5 >
   explicit inline Thread( Callable func, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 );
   //@}
   //**********************************************************************************************

   //**Destructor**********************************************************************************
   /*!\name Destructor */
   //@{
   ~Thread();
   //@}
   //**********************************************************************************************

   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
   inline bool joinable() const;
   inline void join();
   //@}
   //**********************************************************************************************

 private:
   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
   inline bool hasTerminated() const;
   //@}
   //**********************************************************************************************

   //**Thread execution functions******************************************************************
   /*!\name Thread execution functions */
   //@{
   void run();
   //@}
   //**********************************************************************************************

   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   volatile bool terminated_;  //!< Thread termination flag.
                               /*!< This flag value is used by the managing thread
                                    pool to learn whether the thread has terminated
                                    its execution. */
   ThreadPool*   pool_;        //!< Handle to the managing thread pool.
   ThreadHandle  thread_;      //!< Handle to the thread of execution.
   //@}
   //**********************************************************************************************

   //**Friend declarations*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   friend class ThreadPool;
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
/*!\brief Starting a thread of execution on the given zero argument function/functor.
//
// \param func The given function/functor.
//
// This function creates a new thread of execution on the given function/functor. The given
// function/functor must be copyable, must be callable without arguments and must return void.
*/
template< typename Callable >  // Type of the function/functor
inline Thread::Thread( Callable func )
   : pool_  ( 0 )  // Handle to the managing thread pool
   , thread_( 0 )  // Handle to the thread of execution
{
   thread_.reset( new boost::thread( func ) );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Starting a thread of execution on the given unary function/functor.
//
// \param func The given function/functor.
// \param a1 The first argument.
//
// This function creates a new thread of execution on the given function/functor. The given
// function/functor must be copyable, must be callable with a single argument and must return
// void.
*/
template< typename Callable  // Type of the function/functor
        , typename A1 >      // Type of the first argument
inline Thread::Thread( Callable func, A1 a1 )
   : pool_  ( 0 )  // Handle to the managing thread pool
   , thread_( 0 )  // Handle to the thread of execution
{
   thread_.reset( new boost::thread( func, a1 ) );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Starting a thread of execution on the given binary function/functor.
//
// \param func The given function/functor.
// \param a1 The first argument.
// \param a2 The second argument.
//
// This function creates a new thread of execution on the given function/functor. The given
// function/functor must be copyable, must be callable with two arguments and must return
// void.
*/
template< typename Callable  // Type of the function/functor
        , typename A1        // Type of the first argument
        , typename A2 >      // Type of the second argument
inline Thread::Thread( Callable func, A1 a1, A2 a2 )
   : pool_  ( 0 )  // Handle to the managing thread pool
   , thread_( 0 )  // Handle to the thread of execution
{
   thread_.reset( new boost::thread( func, a1, a2 ) );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Starting a thread of execution on the given ternary function/functor.
//
// \param func The given function/functor.
// \param a1 The first argument.
// \param a2 The second argument.
// \param a3 The third argument.
//
// This function creates a new thread of execution on the given function/functor. The given
// function/functor must be copyable, must be callable with three arguments and must return
// void.
*/
template< typename Callable  // Type of the function/functor
        , typename A1        // Type of the first argument
        , typename A2        // Type of the second argument
        , typename A3 >      // Type of the third argument
inline Thread::Thread( Callable func, A1 a1, A2 a2, A3 a3 )
   : pool_  ( 0 )  // Handle to the managing thread pool
   , thread_( 0 )  // Handle to the thread of execution
{
   thread_.reset( new boost::thread( func, a1, a2, a3 ) );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Starting a thread of execution on the given four argument function/functor.
//
// \param func The given function/functor.
// \param a1 The first argument.
// \param a2 The second argument.
// \param a3 The third argument.
// \param a4 The fourth argument.
//
// This function creates a new thread of execution on the given function/functor. The given
// function/functor must be copyable, must be callable with four arguments and must return
// void.
*/
template< typename Callable  // Type of the function/functor
        , typename A1        // Type of the first argument
        , typename A2        // Type of the second argument
        , typename A3        // Type of the third argument
        , typename A4 >      // Type of the fourth argument
inline Thread::Thread( Callable func, A1 a1, A2 a2, A3 a3, A4 a4 )
   : pool_  ( 0 )  // Handle to the managing thread pool
   , thread_( 0 )  // Handle to the thread of execution
{
   thread_.reset( new boost::thread( func, a1, a2, a3, a4 ) );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Starting a thread of execution on the given five argument function/functor.
//
// \param func The given function/functor.
// \param a1 The first argument.
// \param a2 The second argument.
// \param a3 The third argument.
// \param a4 The fourth argument.
// \param a5 The firth argument.
//
// This function creates a new thread of execution on the given function/functor. The given
// function/functor must be copyable, must be callable with five arguments and must return
// void.
*/
template< typename Callable  // Type of the function/functor
        , typename A1        // Type of the first argument
        , typename A2        // Type of the second argument
        , typename A3        // Type of the third argument
        , typename A4        // Type of the fourth argument
        , typename A5 >      // Type of the fifth argument
inline Thread::Thread( Callable func, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 )
   : pool_  ( 0 )  // Handle to the managing thread pool
   , thread_( 0 )  // Handle to the thread of execution
{
   thread_.reset( new boost::thread( func, a1, a2, a3, a4, a5 ) );
}
//*************************************************************************************************




//=================================================================================================
//
//  UTILITY FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Returns whether this is a thread of execution.
//
// \return \a true if this is a thread of execution, \a false otherwise.
//
// This function returns whether this thread is still executing the given task or if it has
// already finished the job. In case the thread is still execution, the function returns
// \a true, else it returns \a false.
*/
inline bool Thread::joinable() const
{
   return thread_->joinable();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Waiting for a thread of execution to complete.
//
// \return void
//
// If the thread is still executing the given task, this function blocks until the thread's
// tasks is completed.
*/
inline void Thread::join()
{
   thread_->join();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns whether the thread has terminated its execution.
//
// \return \a true in case the thread is terminated, \a false otherwise.
//
// This function is used by the managing thread pool to learn whether the thread has finished
// its execution and can be destroyed.
*/
inline bool Thread::hasTerminated() const
{
   return terminated_;
}
//*************************************************************************************************

} // namespace blaze

#endif
