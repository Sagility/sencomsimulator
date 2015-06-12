// config.h
#pragma once



#define ENERGYSIM_DLL_EXPORT __declspec(dllexport)
#define ENERGYSIM_DLL_IMPORT __declspec(dllimport)

// Should be defined when MyDLL is built.
#ifdef ENERGYSIM_DLL_EXPORTS
  #define ENERGYSIM_DLL_PUBLIC ENERGYSIM_DLL_EXPORT
#else
  #define ENERGYSIM_DLL_PUBLIC ENERGYSIM_DLL_IMPORT
#endif

#define ENERGYSIM_DLL_PRIVATE

#ifdef __cplusplus
  #define ENERGYSIM_DLL_FUNCTION extern "C"
#else
  #define ENERGYSIM_DLL_FUNCTION extern
#endif


#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>

namespace EnergySim {

	class ENERGYSIM_DLL_PUBLIC DateTime{
	public:
		// Get current date/time, format is DD_MM_YYYY_HH_mm_ss
		static const std::string currentDateTime()
		{

			struct tm timeinfo;
			char       buf[80];
			time_t     now = time(0);
#ifdef STDC

			tstruct = *localtime(&now);

			// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
			// for more information about date/time format
			strftime(buf, sizeof(buf), "%d-%m-%Y.%X", &timeinfo);
#else
			localtime_s(&timeinfo, &now);
			strftime(buf, sizeof(buf), "%d_%m_%Y_%H_%M_%S", &timeinfo);

#endif
			return buf;
		};
	};
}