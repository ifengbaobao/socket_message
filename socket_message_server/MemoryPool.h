#ifndef __MEMORY_POOL_H__
#define __MEMORY_POOL_H__

#include <mutex>

class MemoryPool;

struct MemoryPoolHead
{
	MemoryPoolHead* Next;
	MemoryPool * Pool;
	bool IsBlock;
};

class MemoryPool
{
public:
	//size 每个块的大小。count，一共多少个块。
	MemoryPool(int maxSize,int count);
	MemoryPool(const MemoryPool &) = delete;
	MemoryPool * operator=(MemoryPool &) = delete;
	~MemoryPool();
	
	//申请内存，释放内存。
	void* AllocMemory();
	void* AllocMemory(size_t size);
	void FreeMemory(void * ptr);

	void* operator new(std::size_t size);
	void* operator new[](std::size_t size);
	void operator delete(void* ptr);
	void operator delete[](void* ptr);

private:
	char * m_pMemory;
	MemoryPoolHead * m_pHead;
	int m_BlockMaxSize;
	std::mutex m_Mutex;

};


#endif // !__MEMORY_POOL_H__
