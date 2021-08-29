#include "MemoryPool.h"

#include <iostream>


MemoryPool::MemoryPool(int maxSize, int count)
	:m_BlockMaxSize(maxSize)
{
	if (maxSize == 0 && count == 0)
	{
		m_pMemory = nullptr;
		m_pHead = nullptr;
		return;
	}
	int blockAllSize = maxSize + sizeof(MemoryPoolHead);
	m_pMemory = (char*)std::malloc(blockAllSize*count);
	m_pHead = (MemoryPoolHead *)m_pMemory;
	for (int i = 0; i < count; ++i)
	{
		MemoryPoolHead * pBlock = (MemoryPoolHead *)(m_pMemory + (blockAllSize*i));
		pBlock->Next = (MemoryPoolHead *)(((char*)pBlock) + blockAllSize);
		pBlock->Pool = this;
		pBlock->IsBlock = true;
	}
	MemoryPoolHead * lastBlock = (MemoryPoolHead *)(m_pMemory + (blockAllSize*(count - 1)));
	lastBlock->Next = nullptr;
}

MemoryPool::~MemoryPool()
{
	if (m_pMemory != nullptr)
	{
		std::free(m_pMemory);
	}
}

void * MemoryPool::AllocMemory()
{
	{
		std::unique_lock<std::mutex> lock(m_Mutex);
		if (m_pHead != nullptr)
		{
			MemoryPoolHead * retBlock = m_pHead;
			m_pHead = m_pHead->Next;
			return ((char*)retBlock)+sizeof(MemoryPoolHead);
		}
	}
	return AllocMemory(m_BlockMaxSize);
}

void * MemoryPool::AllocMemory(size_t size)
{
	std::cout << "申请内容：" << size << std::endl;
	//需要新申请。
	int blockAllSize = size + sizeof(MemoryPoolHead);
	MemoryPoolHead *  newBlock = (MemoryPoolHead*)std::malloc(blockAllSize);
	newBlock->Next = nullptr;
	newBlock->Pool = this;
	newBlock->IsBlock = false;
	return ((char*)newBlock) + sizeof(MemoryPoolHead);
}

void MemoryPool::FreeMemory(void * ptr)
{
	MemoryPoolHead * oldBlock = (MemoryPoolHead *)(((char*)ptr) - sizeof(MemoryPoolHead));
	if (oldBlock->IsBlock)
	{
		std::unique_lock<std::mutex> lock(m_Mutex);
		oldBlock->Next = m_pHead;
		m_pHead = oldBlock;
		return;
	}
	std::free(oldBlock);
}

void * MemoryPool::operator new(std::size_t size)
{
	return std::malloc(size);
}

void * MemoryPool::operator new[](std::size_t size)
{
	return std::malloc(size);
}

void MemoryPool::operator delete(void * ptr)
{
	std::free(ptr);
}

void MemoryPool::operator delete[](void * ptr)
{
	std::free(ptr);
}
