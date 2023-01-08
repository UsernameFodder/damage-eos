// Reimplementation of the various mathematical utilities the game uses for damage calculation

#ifndef MATHUTIL_HPP_
#define MATHUTIL_HPP_

#include <cstdint>

using std::int16_t;
using std::int32_t;
using std::int64_t;
using std::int8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::uint8_t;

// 32-bit signed, binary fixed-point number used by the game for many math operations.
// The lower 8 bits are the fraction bits.
// The actual game just uses raw 32-bit integers, but defining a class makes things more convenient
// for reimplementation
class Fx32 {
    uint32_t raw;
    friend class Fx64;

  public:
    static const Fx32 CONST_NEG0_5;
    static const Fx32 CONST_0_25;
    static const Fx32 CONST_0_5;
    static const Fx32 CONST_153_DIV_256;
    static const Fx32 CONST_1_DIV_SQRT2;
    static const Fx32 CONST_0_75;
    static const Fx32 CONST_0_8;
    static const Fx32 CONST_1_2;
    static const Fx32 CONST_1_25;
    static const Fx32 CONST_85_DIV_64;
    static const Fx32 CONST_1_4;
    static const Fx32 CONST_1_5;
    static const Fx32 CONST_1_7;

    Fx32() { raw = 0; }
    Fx32(uint32_t ipart, uint8_t fpart) { raw = (ipart << 8) | fpart; }
    Fx32(const Fx32& fx32) { raw = fx32.raw; }
    Fx32(uint32_t x) { raw = x << 8; }
    Fx32(int32_t x) : Fx32(static_cast<uint32_t>(x)) {}
    bool is_negative() const;
    // Not used in the damage calc; for debug use only
    uint32_t get_raw() const { return raw; }
    // Truncate to just the integer part.
    // In the original binary, this is just an arithmetic right-shift by 8
    int32_t trunc() const;
    int32_t ceil() const;
    double val() const; // Only for use with displaying output

    Fx32& operator+=(const Fx32& rhs);
    friend Fx32 operator+(Fx32 lhs, const Fx32& rhs);
    Fx32& operator*=(const Fx32& rhs);
    friend Fx32 operator*(Fx32 lhs, const Fx32& rhs);
    Fx32& operator/=(const Fx32& rhs);
    friend Fx32 operator/(Fx32 lhs, const Fx32& rhs);

    friend bool operator<(const Fx32& lhs, const Fx32& rhs);
    friend bool operator>(const Fx32& lhs, const Fx32& rhs);
};

// 64-bit signed, binary fixed-point number used by the game for many math operations in the
// damage calcalation routines specifically. The lower 16 bits are the fraction bits.
// The actual game represents these as a pair of 32-bit integers, one for the high bits and one
// for the low bits
class Fx64 {
    uint64_t raw;

  public:
    static const Fx64 CONST_0_5;
    static const Fx64 CONST_0_75;
    static const Fx64 CONST_1_5;

    Fx64() { raw = 0; }
    Fx64(uint32_t high, uint32_t low) { raw = (static_cast<uint64_t>(high) << 32) | low; }
    Fx64(const Fx64& fx64) { raw = fx64.raw; }
    // pmdsky-debug: FixedPoint32To64 ([NA] 0x2001CD4)
    Fx64(Fx32 fx32) {
        raw = static_cast<uint64_t>(fx32.raw) << 8;
        if (fx32.is_negative()) {
            // sign extend
            raw |= (uint64_t(0xFFFFFF) << 40);
        }
    }
    // pmdsky-debug: IntToFixedPoint64 ([NA] 0x2001C80)
    Fx64(uint32_t x) {
        raw = static_cast<uint64_t>(x) << 16;
        // This is a bug in the game, which deals with negative numbers incorrectly
        if (x & 0x8000) {
            raw |= (uint64_t(0xFFFF) << 48);
        }
    }
    Fx64(int32_t x) : Fx64(static_cast<uint32_t>(x)) {}
    // Not used in the damage calc; for debug use only
    uint64_t get_raw() const { return raw; }
    bool is_negative() const;
    int32_t round() const;
    double val() const; // Only for use with displaying output, NOT FULLY PRECISE

    Fx64& operator+=(const Fx64& rhs);
    friend Fx64 operator+(Fx64 lhs, const Fx64& rhs);
    Fx64& operator*=(const Fx64& rhs);
    friend Fx64 operator*(Fx64 lhs, const Fx64& rhs);
    Fx64& operator/=(const Fx64& rhs);
    friend Fx64 operator/(Fx64 lhs, const Fx64& rhs);

    friend bool operator<(const Fx64& lhs, const Fx64& rhs);
    friend bool operator==(const Fx64& lhs, const Fx64& rhs);
    friend bool operator!=(const Fx64& lhs, const Fx64& rhs);
};

// A 32-bit decimal fixed-point number, used by the game for belly.
// This class matches what the game does, with a 16-bit integer for the integer part and another
// 16-bit integer for thousandths.
class DecFx16_16 {
    int16_t ipart;
    uint16_t thousandths;

  public:
    DecFx16_16(int16_t _ipart = 0, uint16_t _thousandths = 0)
        : ipart(_ipart), thousandths(_thousandths) {}
    // Not used in the damage calc; for debug use only
    int16_t get_ipart() const { return ipart; }
    // Not used in the damage calc; for debug use only
    uint16_t get_thousandths() const { return thousandths; }
    int32_t ceil() const;
};

// Signed integer overflow is implementation-dependent, so we can't rely on it.
// The actual game doesn't need this and just relies on the behavior of the ARM architecture
int32_t u32_to_i32(uint32_t x);
// Arithmetic shift right. Sign-bit extension is implementation-dependent, so we can't rely on it.
// The actual game doesn't need this, and uses ARM's asr instruction directly.
uint32_t asr(uint32_t x, uint8_t shift);

// Returns x/4, truncated towards 0.
inline int32_t div4_trunc(int32_t x) { return u32_to_i32(asr((x + (asr(x, 1) >> 30)), 2)); }

Fx64 clamped_ln(int32_t x);

#endif
