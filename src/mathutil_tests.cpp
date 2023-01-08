#include <catch2/catch_test_macros.hpp>
#include "mathutil.hpp"

TEST_CASE("Fx32 default constructor sets value to 0", "[Fx32]") { REQUIRE(Fx32().get_raw() == 0); }
TEST_CASE("Fx32 can be constructed from integer and fractional parts", "[Fx32]") {
    REQUIRE(Fx32(100, 200).get_raw() == 0x64C8);
    REQUIRE(Fx32(0x12345678, 0xAB).get_raw() == 0x345678AB);
}
TEST_CASE("Fx32 copy constructor works", "[Fx32]") {
    Fx32 x(100, 200);
    REQUIRE(Fx32(x).get_raw() == x.get_raw());
}
TEST_CASE("Fx32 can be constructed from integer", "[Fx32]") {
    REQUIRE(Fx32(10).get_raw() == 0xA00);
    REQUIRE(Fx32(-10).get_raw() == 0xFFFFF600);
}
TEST_CASE("Fx32 is_negative() works", "[Fx32]") {
    REQUIRE(!Fx32(1).is_negative());
    REQUIRE(!Fx32(0).is_negative());
    REQUIRE(Fx32(-1).is_negative());
}
TEST_CASE("Fx32 can be transformed into the represented value", "[Fx32]") {
    Fx32 x(50, 100);
    Fx32 y(100, 200);
    Fx32 z(1, 128);
    Fx32 a(0xFFFFCE, 100);
    Fx32 b(0xFFFF9C, 200);
    Fx32 c(0xFFFFFF, 128);

    SECTION("trunc() works") {
        REQUIRE(x.trunc() == 50);
        REQUIRE(y.trunc() == 100);
        REQUIRE(z.trunc() == 1);
        REQUIRE(a.trunc() == -50);
        REQUIRE(b.trunc() == -100);
        REQUIRE(c.trunc() == -1);
    }
    SECTION("ceil() works") {
        REQUIRE(x.ceil() == 51);
        REQUIRE(y.ceil() == 101);
        REQUIRE(z.ceil() == 2);
        REQUIRE(a.ceil() == -49);
        REQUIRE(b.ceil() == -99);
        REQUIRE(c.ceil() == 0);
    }
    SECTION("val() works") {
        REQUIRE(x.val() == 50.390625);
        REQUIRE(y.val() == 100.78125);
        REQUIRE(z.val() == 1.5);
        REQUIRE(a.val() == -49.609375);
        REQUIRE(b.val() == -99.21875);
        REQUIRE(c.val() == -0.5);
    }
}
TEST_CASE("Fx32 binary operators work", "[Fx32]") {
    Fx32 x(1, 0x80);
    Fx32 y(3);
    Fx32 z(0x7FFFFF, 0);
    Fx32 m(0x800000, 0);
    Fx32 n(0xFFFFFD, 0x80);

    SECTION("addition works") {
        REQUIRE((x + x).val() == 3);
        REQUIRE((x + y).val() == 4.5);
        REQUIRE((x + n).val() == -1);
        REQUIRE((y + n).val() == 0.5);
        REQUIRE((y + z).val() == -8388606);
        REQUIRE((m + n).val() == 8388605.5);
    }
    SECTION("multiplication works") {
        REQUIRE((x * y).val() == 4.5);
        REQUIRE((x * n).val() == -3.75);
        REQUIRE((x * z).val() == -4194305.5);
        REQUIRE((x * m).val() == 4194304);
        REQUIRE((n * n).val() == 6.25);
        REQUIRE((x * Fx32(0xFFFFFF, 0xFF)).val() == -0.0078125);
        REQUIRE((x * 0).val() == 0);
    }
    SECTION("division works") {
        REQUIRE((x / y).val() == 0.5);
        REQUIRE((y / x).val() == 2);
        REQUIRE((x / n).val() == -0.59765625);
        REQUIRE((z / x).val() == 5592404.6640625);
        REQUIRE((m / x).val() == -5592405.33203125);
        REQUIRE((n / n).val() == 1);
        REQUIRE((m / z).val() == -1);
        REQUIRE((z / m).val() == -0.99609375);
    }
    SECTION("comparison works") {
        REQUIRE(x > 0);
        REQUIRE_FALSE(x < 0);
        REQUIRE_FALSE(n > 0);
        REQUIRE(n < 0);
        REQUIRE(x < y);
        REQUIRE(y < z);
        REQUIRE(z > m);
        REQUIRE(m < n);
        REQUIRE(n < x);
    }
}
TEST_CASE("Fx32 predefined constants are correct", "[Fx32]") {
    REQUIRE(Fx32::CONST_NEG0_5.val() == -0.5);
    REQUIRE(Fx32::CONST_0_25.val() == 0.25);
    REQUIRE(Fx32::CONST_0_5.val() == 0.5);
    REQUIRE(Fx32::CONST_153_DIV_256.val() == 0.59765625);
    REQUIRE(Fx32::CONST_1_DIV_SQRT2.val() == 0.70703125);
    REQUIRE(Fx32::CONST_0_75.val() == 0.75);
    REQUIRE(Fx32::CONST_0_8.val() == 0.796875);
    REQUIRE(Fx32::CONST_1_2.val() == 1.19921875);
    REQUIRE(Fx32::CONST_1_25.val() == 1.25);
    REQUIRE(Fx32::CONST_85_DIV_64.val() == 1.328125);
    REQUIRE(Fx32::CONST_1_4.val() == 1.3984375);
    REQUIRE(Fx32::CONST_1_5.val() == 1.5);
    REQUIRE(Fx32::CONST_1_7.val() == 1.69921875);
}

TEST_CASE("Fx64 default constructor sets value to 0", "[Fx64]") { REQUIRE(Fx64().get_raw() == 0); }
TEST_CASE("Fx64 can be constructed from high and low parts", "[Fx64]") {
    REQUIRE(Fx64(100, 200).get_raw() == 0x64000000C8);
    REQUIRE(Fx64(0x12345678, 0x90ABCDEF).get_raw() == 0x1234567890ABCDEF);
}
TEST_CASE("Fx64 copy constructor works", "[Fx64]") {
    Fx64 x(100, 200);
    REQUIRE(Fx64(x).get_raw() == x.get_raw());
}
TEST_CASE("Fx64 can be constructed from Fx32", "[Fx64]") {
    REQUIRE(Fx64(Fx32(100, 200)).get_raw() == 0x64C800);
    REQUIRE(Fx64(Fx32(0xFFFFFFFF, 0xFF)).get_raw() == 0xFFFFFFFFFFFFFF00);
}
TEST_CASE("Fx64 can be constructed from integer", "[Fx64]") {
    REQUIRE(Fx64(10).get_raw() == 0xA0000);
    REQUIRE(Fx64(-10).get_raw() == 0xFFFFFFFFFFF60000);
}
TEST_CASE("Fx64 is_negative() works", "[Fx64]") {
    REQUIRE(!Fx64(1).is_negative());
    REQUIRE(!Fx64(0).is_negative());
    REQUIRE(Fx64(-1).is_negative());
}
TEST_CASE("Fx64 can be transformed into the represented value", "[Fx64]") {
    Fx64 a(1);
    Fx64 b(0, 0x14000);
    Fx64 c(0, 0x18000);
    Fx64 d(0, 0x1C000);
    Fx64 e(-1);
    Fx64 f(0xFFFFFFFF, 0xFFFF4000);
    Fx64 g(0xFFFFFFFF, 0xFFFF8000);
    Fx64 h(0xFFFFFFFF, 0xFFFFC000);

    SECTION("round() works") {
        REQUIRE(a.round() == 1);
        REQUIRE(b.round() == 1);
        REQUIRE(c.round() == 2);
        REQUIRE(d.round() == 2);
        REQUIRE(e.round() == -1);
        REQUIRE(f.round() == -1);
        REQUIRE(g.round() == 0);
        REQUIRE(h.round() == 0);
    }
    SECTION("val() works") {
        REQUIRE(a.val() == 1);
        REQUIRE(b.val() == 1.25);
        REQUIRE(c.val() == 1.5);
        REQUIRE(d.val() == 1.75);
        REQUIRE(e.val() == -1);
        REQUIRE(f.val() == -0.75);
        REQUIRE(g.val() == -0.5);
        REQUIRE(h.val() == -0.25);
    }
}
TEST_CASE("Fx64 binary operators work", "[Fx64]") {
    Fx64 x(0, 0x18000);
    Fx64 y(3);
    Fx64 z(0x7FFFFFFF, 0xFFFF0000);
    Fx64 m(0x80000000, 0x00000000);
    Fx64 n(0xFFFFFFFF, 0xFFFD8000);

    SECTION("addition works") {
        REQUIRE((x + x).val() == 3);
        REQUIRE((x + y).val() == 4.5);
        REQUIRE((x + n).val() == -1);
        REQUIRE((y + n).val() == 0.5);
        REQUIRE((y + z).val() == -140737488355326);
        REQUIRE((m + n).val() == 140737488355325.5);
    }
    SECTION("multiplication works") {
        REQUIRE((x * y).val() == 4.5);
        REQUIRE((x * n).val() == -3.75);
        REQUIRE((x * z).val() == -70368744177665.5);
        REQUIRE((x * m).val() == 70368744177664);
        REQUIRE((n * n).val() == 6.25);
        REQUIRE((x * Fx64(0xFFFFFFFF, 0xFFFFFFFF)).val() == -3.0517578125e-5);
        REQUIRE((x * 0).val() == 0);
    }
    SECTION("division works") {
        REQUIRE((x / y).val() == 0.5);
        REQUIRE((y / x).val() == 2);
        REQUIRE((x / n).val() == -0.5999908447265625);
        REQUIRE((z / x).val() == 93824992236884.67);
        REQUIRE((m / x).val() == -93824992236885.33);
        REQUIRE((n / n).val() == 1);
        REQUIRE((m / z).val() == -1);
        REQUIRE((z / m).val() == -0.9999847412109375);
        REQUIRE((x / 0).get_raw() == 0x7FFFFFFFFFFFFFFF);
    }
    SECTION("less than operator works") {
        REQUIRE(0 < x);
        REQUIRE_FALSE(x < 0);
        REQUIRE_FALSE(0 < n);
        REQUIRE(n < 0);
        REQUIRE(x < y);
        REQUIRE(y < z);
        REQUIRE(m < z);
        REQUIRE(m < n);
        REQUIRE(n < x);
    }
    SECTION("equality operators work") {
        REQUIRE(x == x);
        REQUIRE_FALSE(x == y);
        REQUIRE_FALSE(x != x);
        REQUIRE(x != y);
    }
}
TEST_CASE("Fx64 predefined constants are correct", "[Fx64]") {
    REQUIRE(Fx64::CONST_0_5.get_raw() == 0x8000);
    REQUIRE(Fx64::CONST_0_75.get_raw() == 0xC000);
    REQUIRE(Fx64::CONST_1_5.get_raw() == 0x18000);
}

TEST_CASE("DecFx16_16 can be constructed from its raw parts", "[DecFx16_16]") {
    DecFx16_16 x(99, 500);
    REQUIRE(x.get_ipart() == 99);
    REQUIRE(x.get_thousandths() == 500);
}
TEST_CASE("DecFx16_16::ceil() works", "[DecFx16_16]") {
    REQUIRE(DecFx16_16(99, 0).ceil() == 99);
    REQUIRE(DecFx16_16(99, 500).ceil() == 100);
    REQUIRE(DecFx16_16(99, 999).ceil() == 100);
}

TEST_CASE("u32_to_i32() works", "[functions]") {
    REQUIRE(u32_to_i32(0) == 0);
    REQUIRE(u32_to_i32(1) == 1);
    REQUIRE(u32_to_i32(0xFFFFFFF) == 0xFFFFFFF);
    REQUIRE(u32_to_i32(0x80000000) == std::numeric_limits<int32_t>::min());
    REQUIRE(u32_to_i32(0xFFFFFFFF) == -1);
}

TEST_CASE("asr() works", "[functions]") {
    REQUIRE(asr(0, 1) == 0);
    REQUIRE(asr(1, 1) == 0);
    REQUIRE(asr(0b1100, 1) == 0b110);
    REQUIRE(asr(1 << 8, 8) == 1);
    REQUIRE(asr(0xFFFFFFFF, 8) == 0xFFFFFFFF);
    REQUIRE(asr(0x80000001, 1) == (0b11 << 30));
    REQUIRE(asr(0x80000000 | 0b10, 1) == ((0b11 << 30) | 1));
    REQUIRE(asr(0x80000000, 7) == (0xFF << 24));
}

TEST_CASE("div4_trunc() works", "[functions]") {
    REQUIRE(div4_trunc(0) == 0);
    REQUIRE(div4_trunc(1) == 0);
    REQUIRE(div4_trunc(-1) == 0);
    REQUIRE(div4_trunc(2) == 0);
    REQUIRE(div4_trunc(-2) == 0);
    REQUIRE(div4_trunc(3) == 0);
    REQUIRE(div4_trunc(-3) == 0);
    REQUIRE(div4_trunc(4) == 1);
    REQUIRE(div4_trunc(-4) == -1);
}

TEST_CASE("clamped_ln() works", "[functions]") {
    REQUIRE(clamped_ln(-10) == clamped_ln(1));
    REQUIRE(clamped_ln(0) == clamped_ln(1));
    REQUIRE(clamped_ln(10000) == clamped_ln(2047));
    REQUIRE(clamped_ln(2) == Fx64{0, 0xB170});
}
