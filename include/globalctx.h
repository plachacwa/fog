#pragma once
#include "region.h"

class GlobalContext {
    Region storage;

    public:
        GlobalContext();

        template <class T, class ...Args>
        T* add(Args&& ...args) { return storage.make(std::forward<Args>(args)...); };
};
