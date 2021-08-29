#include "Logger.hpp"


Logger * Logger::s_pInstance = nullptr;
std::mutex Logger::s_Mutex;
