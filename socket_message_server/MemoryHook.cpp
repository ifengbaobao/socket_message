#include "MemoryHook.h"

#ifdef __MEMORY_POOL__

#include "MemoryMgr.h"

#include <iostream>

void * operator new(std::size_t size) _GLIBCXX_THROW(bad_alloc)
{
	return MemoryMgr::Instance().AllocMemory(size);
}

void * operator new[](std::size_t size) _GLIBCXX_THROW(bad_alloc)
{
	return MemoryMgr::Instance().AllocMemory(size);
}

void operator delete(void * p) _GLIBCXX_USE_NOEXCEPT
{
	MemoryMgr::Instance().FreeMemory(p);
}

void operator delete[](void * p) _GLIBCXX_USE_NOEXCEPT
{
	MemoryMgr::Instance().FreeMemory(p);
}

#endif // __MEMORY_POOL__
