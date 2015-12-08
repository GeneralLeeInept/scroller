#pragma once

// 1.25.6 fixed point format
// 1 sign bit
// 25 bit whole
// 6 bit fractional
// 6 bit fractional because tiles are 64 x 64
typedef Sint32 Fixed;

Fixed FixedAdd(Fixed a, Fixed b);
Fixed FixedSub(Fixed a, Fixed b);
Fixed FixedMul(Fixed a, Fixed b);
Fixed FixedDiv(Fixed a, Fixed b);
string FixedFormat(Fixed a);