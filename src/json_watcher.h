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

class json_watcher {
  static constexpr std::chrono::milliseconds kIntervalMs{1000};
  struct watcher_entry {
    bool is_exists;
    std::filesystem::file_time_type last_edit;

    watcher_entry(const std::filesystem::path &path);
  };
  std::unordered_map<std::string, watcher_entry> files;
public:
  call_result_t start(const std::filesystem::path &filename);
  call_result_t stop(const std::filesystem::path &filename);

  JsonWatcherFileState get_file_state(const std::filesystem::path &filename, watcher_entry &entry);
  void process(class script *scr);
};
