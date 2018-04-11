/*
 * Timer.h
 *
 *  Created on: Mar 28, 2018
 *      Author: bob
 */

#ifndef SRC_BENCHMARK_INCLUDE_BENCHMARK_TIMER_H_
#define SRC_BENCHMARK_INCLUDE_BENCHMARK_TIMER_H_


#ifdef WIN32

    #include <windows.h>

    double get_time() {
        LARGE_INTEGER t, f;
        QueryPerformanceCounter(&t);
        QueryPerformanceFrequency(&f);
        return (double)t.QuadPart/(double)f.QuadPart;
    }

#else

    #include <sys/time.h>
    #include <sys/resource.h>

    double get_time() {
        struct timeval t;
        gettimeofday(&t, NULL);
        return t.tv_sec + t.tv_usec*1e-6;
    }

#endif


static double timer_time;


double timer_elapsed() {
    const double currTime = get_time();
    const double diff = currTime - timer_time;
    timer_time = currTime;
    return diff;
}


#endif /* SRC_BENCHMARK_INCLUDE_BENCHMARK_TIMER_H_ */
