#ifndef __MEMORY_MGR_H__
#define __MEMORY_MGR_H__

#include "MemoryPool.h"


class MemoryMgr
{
private:
	MemoryMgr();
	MemoryMgr(const MemoryMgr&) = delete;
	MemoryMgr& operator=(MemoryMgr&) = delete;
	~MemoryMgr();

public:
	static MemoryMgr& Instance();

	//申请内存，释放内存。
	void* AllocMemory(int size);
	void FreeMemory(void * ptr);

private:
	MemoryPool * m_MemoryPool_8;
	MemoryPool * m_MemoryPool_16;
	MemoryPool * m_MemoryPool_32;
	MemoryPool * m_MemoryPool_64;
	MemoryPool * m_MemoryPool_128;
	MemoryPool * m_MemoryPool_256;
	MemoryPool * m_MemoryPool_512;
	MemoryPool * m_MemoryPool_1024;
	MemoryPool * m_MemoryPool_2048;
	MemoryPool * m_MemoryPool_Other;

};

#endif // !__MEMORY_MGR_H__