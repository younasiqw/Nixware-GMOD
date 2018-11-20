#pragma once
#include "interface.hpp"

class IBaseClientDLL {
public:
};

namespace globals
{
	inline IBaseClientDLL* client = nullptr;
}