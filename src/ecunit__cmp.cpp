#include <ecunit.h>

template<class T>
static inline bool ecunit__cmp(T a, T b, ecunit__operator_t op) {
    switch (op) {
    case ecunit__operator__eq:
        return !(a == b);
        break;
    case ecunit__operator__lt:
        return !(a < b);
        break;
    case ecunit__operator__le:
        return !(a <= b);
        break;
    case ecunit__operator__gt:
        return !(a > b);
        break;
    case ecunit__operator__ge:
        return !(a >= b);
        break;
    default:
        return true;
    }
}

extern "C" {

bool __ecunit__cmp_int(int a, int b, ecunit__operator_t op) {
    return ecunit__cmp<int>(a, b, op);
}

bool __ecunit__cmp_uint(unsigned int a, unsigned int b, ecunit__operator_t op) {
    return ecunit__cmp<unsigned int>(a, b, op);
}

}
