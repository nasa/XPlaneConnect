// Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
// National Aeronautics and Space Administration. All Rights Reserved.

#ifndef XPCPLUGIN_TIMER_H_
#define XPCPLUGIN_TIMER_H_

#include <stdio.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>

namespace XPC {
	class Timer
	{        
		std::atomic_flag running;
		std::thread th;
		
	public:

		using Callback = std::function<void(void)>;
		
		void start(const std::chrono::milliseconds, const Callback &callback);
		
		void stop();
	};
}

#endif /* Timer_hpp */
