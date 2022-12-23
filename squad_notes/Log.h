#pragma once

#include <iostream>
#include "extension/Singleton.h"

class Log : public Singleton<Log> {
public:
	Log() = default;

	void Logger(std::string logMsg);
};