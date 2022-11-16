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

class pool {
  struct node_entry {
    bool gc;
    nlohmann::json value;

    template <typename T>
    node_entry(T value, bool gc = false);
  };
  std::unordered_map<node_ptr_t, node_entry> nodes{};
  node_ptr_t current_node = 0x80000000;

  template <typename T>
  node_ptr_t alloc(T value);
  nlohmann::json &get(node_ptr_t id);
  nlohmann::json take(node_ptr_t id);
  bool has(node_ptr_t id);
  void set_gc(node_ptr_t id, bool enabled);
  void collect(node_ptr_t id, bool forced = false);
};
