#include "Utils.h"

#include <chrono>


int Utils::GetTimeStampSeconds()
{
	return std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()).time_since_epoch().count();
}

