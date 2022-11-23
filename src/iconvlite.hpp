#pragma once

#include <optional>
#include <string>
#include <string_view>

constexpr unsigned table[128] = {
    0x82D0, 0x83D0, 0x9A80E2, 0x93D1, 0x9E80E2, 0xA680E2, 0xA080E2, 0xA180E2,
    0xAC82E2, 0xB080E2, 0x89D0, 0xB980E2, 0x8AD0, 0x8CD0, 0x8BD0, 0x8FD0,
    0x92D1, 0x9880E2, 0x9980E2, 0x9C80E2, 0x9D80E2, 0xA280E2, 0x9380E2, 0x9480E2,
    0, 0xA284E2, 0x99D1, 0xBA80E2, 0x9AD1, 0x9CD1, 0x9BD1, 0x9FD1,
    0xA0C2, 0x8ED0, 0x9ED1, 0x88D0, 0xA4C2, 0x90D2, 0xA6C2, 0xA7C2,
    0x81D0, 0xA9C2, 0x84D0, 0xABC2, 0xACC2, 0xADC2, 0xAEC2, 0x87D0,
    0xB0C2, 0xB1C2, 0x86D0, 0x96D1, 0x91D2, 0xB5C2, 0xB6C2, 0xB7C2,
    0x91D1, 0x9684E2, 0x94D1, 0xBBC2, 0x98D1, 0x85D0, 0x95D1, 0x97D1,
    0x90D0, 0x91D0, 0x92D0, 0x93D0, 0x94D0, 0x95D0, 0x96D0, 0x97D0,
    0x98D0, 0x99D0, 0x9AD0, 0x9BD0, 0x9CD0, 0x9DD0, 0x9ED0, 0x9FD0,
    0xA0D0, 0xA1D0, 0xA2D0, 0xA3D0, 0xA4D0, 0xA5D0, 0xA6D0, 0xA7D0,
    0xA8D0, 0xA9D0, 0xAAD0, 0xABD0, 0xACD0, 0xADD0, 0xAED0, 0xAFD0,
    0xB0D0, 0xB1D0, 0xB2D0, 0xB3D0, 0xB4D0, 0xB5D0, 0xB6D0, 0xB7D0,
    0xB8D0, 0xB9D0, 0xBAD0, 0xBBD0, 0xBCD0, 0xBDD0, 0xBED0, 0xBFD0,
    0x80D1, 0x81D1, 0x82D1, 0x83D1, 0x84D1, 0x85D1, 0x86D1, 0x87D1,
    0x88D1, 0x89D1, 0x8AD1, 0x8BD1, 0x8CD1, 0x8DD1, 0x8ED1, 0x8FD1
};

inline std::string cp2utf(std::string_view s) {
    std::string ns;
    ns.reserve(s.size() * 4);
    for (const char k : s) {
      auto i = static_cast<unsigned char>(k);
        if (i & 0x80) {
            int v = table[(0x7f & i)];
            if (v) {
                ns.push_back(static_cast<char>(v & 0xFF));
                ns.push_back(static_cast<char>((v >> 8) & 0xFF));
                if (v >>= 16)
                    ns.push_back(static_cast<char>(v & 0xFF));
            }
        }
        else {
            ns.push_back(static_cast<char>(i));
        }
    }
    return ns;
}

inline std::optional<std::string> utf2cp(std::string_view s) {
    std::string out;
    out.reserve(s.size());
    for (auto i = 0u; i < s.size() && s[i] != 0; ++i) {
        auto prefix = static_cast<unsigned char>(s[i]);
        if ((prefix & 0x80) == 0) {
            out.push_back(static_cast<char>(prefix));
        }
        else if ((~prefix) & 0x20) {
            auto suffix = static_cast<unsigned char>(s[i + 1]);

            const int first5bit = (prefix & 0b11111) << 6;
            const int sec6bit = suffix & 0b111111;
            const int unicode_char = first5bit + sec6bit;

            if (unicode_char >= 0x410 && unicode_char <= 0x44F) {
                out.push_back(static_cast<char>(unicode_char - 0x350));
            }
            else if (unicode_char >= 0x80 && unicode_char <= 0xFF) {
                out.push_back(static_cast<char>(unicode_char));
            }
            else if (unicode_char >= 0x402 && unicode_char <= 0x403) {
                out.push_back(static_cast<char>(unicode_char - 0x382));
            }
            else {
                unsigned utf_8 = (suffix << 8) | (prefix);
                auto k = 0;
                for (const unsigned c : table) {
                    if (c == utf_8) {
                        out.push_back(static_cast<char>(k));
                        break;
                    }
                    ++k;
                }
            }
            ++i;
        }
        else if ((~prefix) & 0x10) {
            auto middle = static_cast<unsigned char>(s[i + 1]);
            auto suffix = static_cast<unsigned char>(s[i + 2]);

            const int first4bit = (prefix & 0b1111) << 12;
            const int sec6bit = (middle & 0b111111) << 6;
            const int third6bit = (suffix & 0b111111);
            const int unicode_char = first4bit + sec6bit + third6bit;

            unsigned utf_8 = (suffix << 16) | (middle << 8) | (prefix);
            auto k = 0;
            for (const unsigned c : table) {
                if (c == utf_8) {
                    out.push_back(static_cast<char>(k));
                    break;
                }
                ++k;
            }
            i += 2;
        }
        else {
            return std::nullopt;
        }
    }
    return out;
}