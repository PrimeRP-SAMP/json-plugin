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

#include "json_watcher.h"
#include "plugin.h"

call_result_t json_watcher::start(const std::filesystem::path &filename) {
  if (files.contains(filename.string()))
    return JSON_CALL_WATCHER_EXISTS_ERR;
  files.insert(std::make_pair(filename.string(), watcher_entry(filename)));
  return JSON_CALL_NO_ERR;
}

call_result_t json_watcher::stop(const std::filesystem::path &filename) {
  if (!files.contains(filename.string()))
    return JSON_CALL_NO_SUCH_WATCHER_ERR;
  files.erase(filename.string());
  return JSON_CALL_NO_ERR;
}

JsonWatcherFileState json_watcher::get_file_state(const std::filesystem::path &filename, watcher_entry &entry) {
  auto is_exists = exists(filename);
  auto last_edit = is_exists ? last_write_time(filename) : std::chrono::file_clock::now();
  if (entry.is_exists != is_exists) {
    entry.is_exists = is_exists;
    entry.last_edit = last_edit;
    return is_exists ? JSON_WATCHER_FILE_CREATED : JSON_WATCHER_FILE_ERASED;
  } else if (is_exists && last_edit > entry.last_edit) {
    entry.last_edit = last_edit;
    return JSON_WATCHER_FILE_MODIFIED;
  }
  return JSON_WATCHER_FILE_MAX;
}

void json_watcher::process(script *scr) {
  static auto last_check = std::chrono::steady_clock::now();
  if (files.empty())
    return;
  auto now = std::chrono::steady_clock::now();
  if (std::chrono::duration_cast<std::chrono::seconds>(now - last_check) < kIntervalMs)
    return;
  last_check = now;
  for (auto &[filename, entry] : files) {
    if (auto state = get_file_state(filename, entry); state != JSON_WATCHER_FILE_MAX) {
      if (!scr->json_watcher_handler(filename, state)) {
        return;
      }
    }
  }
}

json_watcher::watcher_entry::watcher_entry(const std::filesystem::path &path)
    : is_exists(exists(path)), last_edit(is_exists ? last_write_time(path) : std::chrono::file_clock::now()) {}
