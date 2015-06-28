#pragma once

#undef TRUE
#undef FALSE

#define TRUE true
#define FALSE false

namespace minstl {

    class Bool
    {
        char value;
    public:
        Bool() : value(0) { }
        Bool(int v) : value(!!v) { }
        Bool(const Bool &other) : value(other.value) { }
    
        Bool &operator=(const Bool &other)
        {
            this->value = other.value;
            return *this;
        }
    
        Bool &operator=(int value)
        {
            this->value = !!value;
            return *this;
        }
    
        operator int() { return int(value); }
    
        operator int() const { return int(value); }
    };

}
#if 0
#undef true
#undef false

#define true bool(1)
#define false bool(0)
#endif