#pragma once

#include "common.h"

namespace minstl
{
    template<typename T>
    class vector
    {
        T *_array;
        int _capacity;

    public:
        vector(): this(4){ }

        vector(int capacity) 
            : _capacity(capacity)
        {
            if(capacity <= 0)
            {

            }
        }
    };
}