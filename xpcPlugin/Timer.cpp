//
//  Timer.cpp
//  Thread
//
//  Created by Jan Chaloupecky on 16.12.18.
//  Copyright © 2018 Jan Chaloupecky. All rights reserved.
//

#include "Timer.h"

void Timer::start(const chrono::milliseconds interval, const Callback &callback) {
    {
        running = true;
        th = thread([=]()
                    {
                        while (running == true) {
                            this_thread::sleep_for(interval);
                            callback();
                        }
                    });
    }
}

void Timer::stop() {
    running = false;
}
