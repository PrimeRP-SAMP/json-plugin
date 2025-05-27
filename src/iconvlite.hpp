#pragma once

#include <unordered_map>
#include <string>
#include <string_view>
#include <array>

namespace iconvlite {
constexpr std::array<uint32_t, 128> cp1251_to_unicode = {
    0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021,
    0x20AC, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F,
    0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
    0x0000, 0x2122, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F,
    0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7,
    0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407,
    0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7,
    0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457,
    0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
    0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
    0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
    0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
    0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
    0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
    0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
    0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F
};

const std::unordered_map<uint32_t, unsigned char> unicode_to_cp1251 = [] {
  std::unordered_map<uint32_t, unsigned char> m;
  for (unsigned char i = 0; i < 0x80; ++i) {
    m[i] = i; // ASCII
  }
  for (size_t i = 0; i < cp1251_to_unicode.size(); ++i) {
    if (cp1251_to_unicode[i]) {
      m[cp1251_to_unicode[i]] = static_cast<unsigned char>(i + 0x80);
    }
  }
  return m;
}();

inline std::string cp2utf(const std::string_view &s) {
  std::string result;
  result.reserve(s.size() * 2);

  for (unsigned char ch : s) {
    if (ch < 0x80) {
      result.push_back(ch);
    } else {
      uint32_t code = cp1251_to_unicode[ch - 0x80];
      if (code < 0x800) {
        result.push_back(static_cast<char>(0xC0 | (code >> 6)));
        result.push_back(static_cast<char>(0x80 | (code & 0x3F)));
      } else {
        result.push_back(static_cast<char>(0xE0 | (code >> 12)));
        result.push_back(static_cast<char>(0x80 | ((code >> 6) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | (code & 0x3F)));
      }
    }
  }

  return result;
}

inline std::string utf2cp(const std::string_view &s) {
  std::string result;
  result.reserve(s.size());

  for (size_t i = 0, slen = s.size(); i < slen;) {
    uint32_t code = 0;
    const auto c = static_cast<unsigned char>(s[i]);

    if (c < 0x80) {
      code = c;
      ++i;
    } else if ((c & 0xE0) == 0xC0 && i + 1 < s.size()) {
      const auto c1 = static_cast<unsigned char>(s[i + 1]);
      if ((c1 & 0xC0) != 0x80) {
        result.push_back('?');
        ++i;
        continue;
      }
      code = ((c & 0x1F) << 6) | (c1 & 0x3F);
      i += 2;
    } else if ((c & 0xF0) == 0xE0 && i + 2 < s.size()) {
      const auto c1 = static_cast<unsigned char>(s[i + 1]);
      const auto c2 = static_cast<unsigned char>(s[i + 2]);
      if ((c1 & 0xC0) != 0x80 || (c2 & 0xC0) != 0x80) {
        result.push_back('?');
        ++i;
        continue;
      }
      code = ((c & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);
      i += 3;
    } else {
      result.push_back('?');
      ++i;
      continue;
    }

    const auto it = unicode_to_cp1251.find(code);
    result.push_back(it != unicode_to_cp1251.cend() ? static_cast<char>(it->second) : '?');
  }

  return result;
}
}
