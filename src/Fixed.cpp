#include "stdafx.h"

#include "Fixed.h"

#define WHOLE(x) (x >> 6)
#define FRAC(x) (x & 0x3f)
#define SIGN(x) ((x & 0x80000000) ? -1 : 1)

Fixed FixedAdd(Fixed a, Fixed b)
{
    return a + b;
}

Fixed FixedSub(Fixed a, Fixed b)
{
    return a - b;
}

Fixed FixedMul(Fixed a, Fixed b)
{
    return static_cast<Fixed>((static_cast<int64_t>(a) * static_cast<int64_t>(b)) >> 6);
}

Fixed FixedDiv(Fixed a, Fixed b)
{
    return static_cast<Fixed>((static_cast<int64_t>(a) << 6) / b);
}

string FixedFormat(Fixed a)
{
    stringstream ss;
    ss << (a / 64.0f);
    return ss.str();
}
