// Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
// National Aeronautics and Space Administration. All Rights Reserved.

#include "Timer.h"
using namespace std;

namespace XPC {
	void Timer::start(const chrono::milliseconds interval, const Callback &callback) {
		{
			running.test_and_set();
			th = thread([=]()
						{
							while (running.test_and_set()) {
								this_thread::sleep_for(interval);
								callback();
							}
						});
		}
	}
	
	void Timer::stop() {
		running.clear();
		if(th.joinable()) {
			th.join();
		}
	}
}
