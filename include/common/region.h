#pragma once
#include <vector>
#include <cstdlib>

class Region {
    std::vector<void*> blocks = {};
    size_t blockSize = 4096;
    char* currentMemory = nullptr;
    size_t usedMemory = 0;

public:
    void* alloc(size_t);

    template<class T, class ...Args>
    T* make(Args&& ...args) {
        void* memory = alloc(sizeof(T));
        return new(memory) T(std::forward<Args>(args)...);
    };
    template<class T>
    T* put(T&& obj) {
        void* memory = alloc(sizeof(T));
        return new(memory) T(std::forward<T>(obj));
    }

    ~Region();
private:
    void allocBlock(size_t);
};