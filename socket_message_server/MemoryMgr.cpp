#include "MemoryMgr.h"

#include "Config.h"



MemoryMgr::MemoryMgr()
    : m_MemoryPool_8(new MemoryPool(8, CONFIG_MEMORY_POOL_8_COUNT))
    , m_MemoryPool_16(new MemoryPool(16, CONFIG_MEMORY_POOL_16_COUNT))
    , m_MemoryPool_32(new MemoryPool(32, CONFIG_MEMORY_POOL_32_COUNT))
    , m_MemoryPool_64(new MemoryPool(64, CONFIG_MEMORY_POOL_64_COUNT))
    , m_MemoryPool_128(new MemoryPool(128, CONFIG_MEMORY_POOL_128_COUNT))
    , m_MemoryPool_256(new MemoryPool(256, CONFIG_MEMORY_POOL_256_COUNT))
    , m_MemoryPool_512(new MemoryPool(512, CONFIG_MEMORY_POOL_512_COUNT))
    , m_MemoryPool_1024(new MemoryPool(1024, CONFIG_MEMORY_POOL_1024_COUNT))
    , m_MemoryPool_2048(new MemoryPool(2048, CONFIG_MEMORY_POOL_2048_COUNT))
	, m_MemoryPool_Other(new MemoryPool(0, 0))
{
}

MemoryMgr::~MemoryMgr()
{
	delete m_MemoryPool_8;
	delete m_MemoryPool_16;
	delete m_MemoryPool_32;
	delete m_MemoryPool_64;
	delete m_MemoryPool_128;
	delete m_MemoryPool_256;
	delete m_MemoryPool_512;
	delete m_MemoryPool_1024;
	delete m_MemoryPool_2048;
	delete m_MemoryPool_Other;
}

MemoryMgr & MemoryMgr::Instance()
{
	static MemoryMgr mgr;
	return mgr;
}

void * MemoryMgr::AllocMemory(int size)
{
	if (size <= 8) {
		return m_MemoryPool_8->AllocMemory();
	}
	if (size <= 16) {
		return m_MemoryPool_16->AllocMemory();
	}
	if (size <= 32) {
		return m_MemoryPool_32->AllocMemory();
	}
	if (size <= 64) {
		return m_MemoryPool_64->AllocMemory();
	}
	if (size <= 128) {
		return m_MemoryPool_128->AllocMemory();
	}
	if (size <= 256) {
		return m_MemoryPool_256->AllocMemory();
	}
	if (size <= 512) {
		return m_MemoryPool_512->AllocMemory();
	}
	if (size <= 1024) {
		return m_MemoryPool_1024->AllocMemory();
	}
	if (size <= 2048) {
		return m_MemoryPool_2048->AllocMemory();
	}
	return m_MemoryPool_Other->AllocMemory(size);
}

void MemoryMgr::FreeMemory(void * ptr)
{
	MemoryPoolHead * oldBlock = (MemoryPoolHead *)(((char*)ptr) - sizeof(MemoryPoolHead));
	oldBlock->Pool->FreeMemory(ptr);
	size_t t;
}
