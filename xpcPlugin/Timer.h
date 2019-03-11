// Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
// National Aeronautics and Space Administration. All Rights Reserved.

#ifndef Timer_hpp
#define Timer_hpp

#include <stdio.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <functional>

namespace XPC {
    class Timer
    {
        
        bool running = false;
        
    public:
        std::thread th;
        typedef std::function<void(void)> Callback;
        
        void start(const std::chrono::milliseconds, const Callback &callback);
        
        void stop();
    };
}

#endif /* Timer_hpp */
