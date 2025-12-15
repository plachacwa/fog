#include "region.h"
void* Region::alloc(size_t size) {
	if (!currentMemory || usedMemory + size > blockSize)
		allocBlock(std::max(size, blockSize));
	void* ptr = currentMemory + usedMemory;
	usedMemory += size;
	return ptr;
};

void Region::allocBlock(size_t size) {
	currentMemory = static_cast<char*>(std::malloc(size));
	blocks.push_back(currentMemory);
	usedMemory = 0;
};

Region::~Region() {
	for (auto block : blocks) std::free(block);
};