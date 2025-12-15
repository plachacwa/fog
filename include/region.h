#pragma once
#include <vector>
#include <cstdlib>

class Region {
	std::vector<void*> blocks;
	size_t blockSize = 1024;
	char* currentMemory = nullptr;
	size_t usedMemory = 0;
	
	public:
		void* alloc(size_t);
		
		template<class T, class ...Args>
		T* make(Args&& ...args) {
			void* memory = alloc(sizeof(T));
			return new(memory) T(std::forward<Args>(args)...);
		};
	
		~Region();
	private:
		void allocBlock(size_t);
};