//
//  Timer.hpp
//  Thread
//
//  Created by Jan Chaloupecky on 16.12.18.
//  Copyright © 2018 Jan Chaloupecky. All rights reserved.
//

#ifndef Timer_hpp
#define Timer_hpp

#include <stdio.h>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

class Timer
{
    
    bool running = false;

public:
    thread th;
    typedef std::function<void(void)> Callback;

    void start(const chrono::milliseconds, const Callback &callback);

    void stop();
};

#endif /* Timer_hpp */
