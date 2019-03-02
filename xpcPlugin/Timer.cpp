// Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
// National Aeronautics and Space Administration. All Rights Reserved.

#include "Timer.h"

namespace XPC {
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
        th.join();
    }
}
