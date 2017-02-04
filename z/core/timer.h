/**
 * File:            timer.h
 * Namespace:       z::core
 * Description:     Timer class. Because this class uses the
 *                  chrono library, it requires AT LEAST C++11.
 *
 *
 * Author:          Zachary Westerman
 * Email:           zacharywesterman@yahoo.com
 * Last modified:   3 Feb. 2017
**/

#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <chrono>

namespace z
{
    namespace core
    {
        class timer
        {
            typedef std::chrono::high_resolution_clock high_resolution_clock;
            typedef std::chrono::microseconds _microseconds;
            typedef std::chrono::milliseconds _milliseconds;
            typedef std::chrono::seconds _seconds;

        private:
            high_resolution_clock::time_point start;

        public:
            explicit timer (bool run = false)
            {
                if (run)
                    reset();
            }

            void reset()
            {
                start = high_resolution_clock::now();
            }


            double micros() const
            {
                return (double)(std::chrono::duration_cast<_microseconds>
                        (high_resolution_clock::now() - start)).count();
            }

            double millis() const
            {
                return (double)(std::chrono::duration_cast<_milliseconds>
                        (high_resolution_clock::now() - start)).count();
            }

            double seconds() const
            {
                return (double)(std::chrono::duration_cast<_seconds>
                        (high_resolution_clock::now() - start)).count();
            }
        };
    }
}

#endif // TIMER_H_INCLUDED