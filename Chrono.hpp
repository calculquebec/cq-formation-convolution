/*!
 * \file Chrono.hpp
 * \brief Class definition for a portable chronograph (stopwatch).
 * \author Marc Parizeau, Laboratoire de vision et systemes numeriques, Universite Laval
 * $Revision: 1.0 $
 * $Date: 2014/01/15 $
 */

#ifndef Chrono_hpp_
#define Chrono_hpp_
#include <chrono>

/*! \brief Chronograph.
\author Marc Parizeau, Laboratoire de vision et systemes numeriques, Universite Laval

This class incapsulates a portable chronograph (stopwatch) class. It is implemented
using the standard C++11 chrono classes. It offers a simple interface that provides 
basic pause/resume/reset functionalities.
*/
class Chrono 
{
public:
    /*! \brief Construct chronograph.

    A chronograph is initialized with a null count value. By default, it starts running 
    automatically. It will initially be paused if argument iAutoStart is set to false.
    */
    Chrono(bool iAutoStart=true) {
        mCount = 0;
        if(iAutoStart) {
            mIsRunning = true;
            mLast = std::chrono::high_resolution_clock::now();
        } else {
            mIsRunning = false;
        }
    }

    /*! \brief Return the current chronograph count value.

    The elapsesd time is returned as a double precision floating point 
    number of seconds.
    */
    inline double get(void) {
        if(mIsRunning) {
            auto lNow = std::chrono::high_resolution_clock::now();
            mCount += (double) (lNow-mLast).count() / std::chrono::high_resolution_clock::period::den;
            mLast = lNow;
        }
        return mCount;
    }

    /*! \brief Return the chronograph resolution.

    The platform dependent resolution is returned as a double precision 
    floating point number of seconds.
    */
    inline double getRes(void) const {
        return 1./std::chrono::high_resolution_clock::period::den;
    }

    /*! \brief Pause the chronograph.

    The chronograph stops counting. Does nothing if the chronograph is 
    already paused.
    */
    inline void pause(void) {
        if(mIsRunning) {
            mIsRunning = false;
            auto lNow = std::chrono::high_resolution_clock::now();
            mCount += (double) (lNow-mLast).count() / std::chrono::high_resolution_clock::period::den;
        }
    }

    /*! \brief Reset the chronograph.

    Resets the chronograph count value to zero. By default, this method
    does not affect the state of the chronograph, in the sense that it will
    keep on running if it was previously running. However, it will pause if
    argument iAutoStop is true.
    */
    inline void reset(bool iAutoStop=false) {
        mCount = 0;
        if(iAutoStop) {
            mIsRunning = false;
        } else if(mIsRunning) {
            mLast = std::chrono::high_resolution_clock::now();
        }
    }

    /*! \brief Resume the chronograph.

    Resumes the execution of the chronograph function. Does nothing if the chronograph 
    is already running.
    */
    inline void resume(void) {
        if(!mIsRunning) {
            mIsRunning = true;
            mLast = std::chrono::high_resolution_clock::now();
        }
    }

private:
    typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;

    double mCount;   //! count of elapsed seconds
    bool mIsRunning; //! is chronograph currently running?
    TimePoint mLast; //! last measured time point 
    
};

#endif // Chrono_hpp_
