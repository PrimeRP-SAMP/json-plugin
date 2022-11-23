// MIT License

// Copyright (c) 2022 Northn

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "common.h"
#include "script.h"

struct native_param : script::NativeParam {
//  operator size_t() { return static_cast<size_t>(raw_value); }
//  operator size_t*() { return reinterpret_cast<size_t*>(raw_value); }

  operator bool() { return static_cast<bool>(raw_value); }
  operator bool*() { return reinterpret_cast<bool*>(script.GetPhysAddr(raw_value)); }

  operator char*() { return reinterpret_cast<char*>(script.GetPhysAddr(raw_value)); }
  operator node_ptr_t() { return reinterpret_cast<node_ptr_t>(raw_value); }

  operator node_ptr_t*() {
    if (raw_value == JSON_INVALID_NODE)
      return nullptr;
    return reinterpret_cast<node_ptr_t*>(script.GetPhysAddr(raw_value));
  }

  operator std::filesystem::path() { return script.GetString(raw_value); }
};
