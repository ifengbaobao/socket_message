#ifndef __MEMORY_HOOK_H__
#define __MEMORY_HOOK_H__

#ifdef __MEMORY_POOL__



#include <new>

void* operator new(std::size_t) _GLIBCXX_THROW(bad_alloc);
void* operator new[](std::size_t) _GLIBCXX_THROW(bad_alloc);
void operator delete(void*)_GLIBCXX_USE_NOEXCEPT;
void operator delete[](void*) _GLIBCXX_USE_NOEXCEPT;

#endif // __MEMORY_POOL__

#endif // !__MEMORY_HOOK_H__


