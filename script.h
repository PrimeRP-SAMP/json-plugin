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
#include "iconvlite.hpp"

class script : public ptl::AbstractScript<script> {
public:
  call_result_t       JSON_Parse(const std::string buffer, node_ptr_t *node_id);
  call_result_t       JSON_ParseFile(const std::filesystem::path filename, node_ptr_t *node_id);
  call_result_t       JSON_SaveFile(const std::filesystem::path filename, const node_ptr_t node_id, const cell indent);
  call_result_t       JSON_Stringify(const node_ptr_t node_id, cell *out, const cell out_size, const cell indent);
  node_type_t         JSON_NodeType(const node_ptr_t node_id);

  template            <typename T>
  node_ptr_result_t   internal_JSON_ConstructNode(T value);
  node_ptr_result_t   JSON_Null();
  node_ptr_result_t   JSON_Bool(const bool value);
  node_ptr_result_t   JSON_Int(const cell value);
  node_ptr_result_t   JSON_Float(const float value);
  node_ptr_result_t   JSON_String(const std::string value);
  node_ptr_result_t   JSON_Object(const cell *params);
  node_ptr_result_t   JSON_Array(cell *params);

  node_ptr_result_t   JSON_Append(const node_ptr_t first_node, const node_ptr_t second_node);
  node_ptr_result_t   JSON_Merge(const node_ptr_t first_node, const node_ptr_t second_node);

  template            <typename T>
  call_result_t       internal_JSON_SetValue(node_ptr_t node_id, const std::string key, const T value);
  call_result_t       JSON_SetNull(node_ptr_t node_id, const std::string key);
  call_result_t       JSON_SetBool(node_ptr_t node_id, const std::string key, const bool value);
  call_result_t       JSON_SetInt(node_ptr_t node_id, const std::string key, const cell value);
  call_result_t       JSON_SetFloat(node_ptr_t node_id, const std::string key, const float value);
  call_result_t       JSON_SetString(node_ptr_t node_id, const std::string key, const std::string value);
  call_result_t       JSON_SetObject(node_ptr_t node_id, const std::string key, const node_ptr_t value_node);
  call_result_t       JSON_SetArray(node_ptr_t node_id, const std::string key, const node_ptr_t value_node);

  call_result_t       JSON_GetBool(node_ptr_t node_id, const std::string key, bool *out);
  call_result_t       JSON_GetInt(node_ptr_t node_id, const std::string key, cell *out);
  call_result_t       JSON_GetFloat(node_ptr_t node_id, const std::string key, float *out);
  call_result_t       JSON_GetString(node_ptr_t node_id, const std::string key, cell *out, cell out_size);
  call_result_t       JSON_GetObject(node_ptr_t node_id, const std::string key, node_ptr_t *out);
  call_result_t       JSON_GetArray(node_ptr_t node_id, const std::string key, node_ptr_t *out);

  node_type_t         JSON_GetType(node_ptr_t node_id, const std::string key);

  call_result_t       JSON_ArrayLength(node_ptr_t node_id, cell *out);
  call_result_t       JSON_ArrayObject(node_ptr_t node_id, cell index, node_ptr_t *out);
  call_result_t       JSON_ArrayIterate(node_ptr_t node_id, cell *index, node_ptr_t *out);
  call_result_t       JSON_ArrayAppend(node_ptr_t node_id, const std::string key, node_ptr_t value_node);
  call_result_t       JSON_ArrayRemove(node_ptr_t node_id, const std::string key, node_ptr_t value_node);
  call_result_t       JSON_ArrayRemoveIndex(node_ptr_t node_id, const std::string key, cell index);
  call_result_t       JSON_ArrayClear(node_ptr_t node_id, const std::string key);
  call_result_t       JSON_Keys(node_ptr_t node_id, cell index, char *out, cell out_size);
  call_result_t       JSON_Remove(node_ptr_t node_id, const std::string key);

  call_result_t       JSON_GetNodeBool(node_ptr_t node_id, bool *out);
  call_result_t       JSON_GetNodeInt(node_ptr_t node_id, cell *out);
  call_result_t       JSON_GetNodeFloat(node_ptr_t node_id, float *out);
  call_result_t       JSON_GetNodeString(node_ptr_t node_id, cell *out, cell out_size);

  call_result_t       JSON_Cleanup(node_ptr_t node_id);
};
