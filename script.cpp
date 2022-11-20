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

#include "script.h"

#define PLUGIN_LOG(text, ...) Log("%s: %d: unknown error: " text, __FUNCTION__, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#define LOG_EXCEPTION(exc) Log("%s: %d: unknown exception: %s", __FUNCTION__, __LINE__, (exc).what())
#define ASSERT_NODE_EXISTS(x) if ((x) == nullptr) { Log("%s: %d: error: node not exists", __FUNCTION__, __LINE__); return kCallNodeNotExistsError; }

call_result_t script::JSON_Parse(const std::string buffer, node_ptr_t *node_id) {
  ASSERT_NODE_EXISTS(node_id);
  try {
    *node_id = new nlohmann::json(nlohmann::json::parse(buffer));
    return kCallNoError;
  } catch (const std::exception &e) {
    LOG_EXCEPTION(e);
    return kCallParserError;
  }
}

call_result_t script::JSON_ParseFile(const std::filesystem::path filename, node_ptr_t *node_id) {
  ASSERT_NODE_EXISTS(node_id);
  try {
    if (!exists(filename) || !is_regular_file(filename)) {
      return kCallNoFileError;
    }
    std::ifstream f(filename);
    *node_id = new nlohmann::json(nlohmann::json::parse(f));
    return kCallNoError;
  } catch (const std::exception &e) {
    LOG_EXCEPTION(e);
    return kCallParserError;
  }
}

call_result_t script::JSON_SaveFile(const std::filesystem::path filename, const node_ptr_t node_id, const cell indent) {
  ASSERT_NODE_EXISTS(node_id);
  try {
    auto parent_path = filename.parent_path();
    if (!parent_path.empty()) {
      if (!exists(parent_path)) {
        create_directories(filename.parent_path());
      } else if (!is_directory(filename.parent_path())) {
        return kCallNoSuchDirectoryError;
      }
    }
    std::ofstream o(filename, std::ofstream::out | std::ofstream::trunc);
    o.clear();
    o << node_id->dump(indent) << std::endl;
    return kCallNoError;
  } catch (const std::exception &e) {
    LOG_EXCEPTION(e);
    return kCallParserError;
  }
}

call_result_t script::JSON_Stringify(const node_ptr_t node_id, cell *out, const cell out_size, const cell indent) {
  ASSERT_NODE_EXISTS(node_id);
  try {
    auto str = node_id->dump(indent);
    SetString(out, str, out_size);
    return kCallNoError;
  } catch (const std::exception &e) {
    LOG_EXCEPTION(e);
    return kCallUnknownError;
  }
}

node_type_t script::JSON_NodeType(const node_ptr_t node_id) {
  ASSERT_NODE_EXISTS(node_id);
  using value_t = nlohmann::json::value_t;
  switch (node_id->type()) {
  case value_t::null:return kNodeTypeNull;
  case value_t::object:return kNodeTypeObject;
  case value_t::array:return kNodeTypeArray;
  case value_t::string:return kNodeTypeString;
  case value_t::boolean:return kNodeTypeBoolean;
    /*case value_t::binary:
      return "binary";
    case value_t::discarded:
      return "discarded";*/
  case value_t::number_integer:
  case value_t::number_unsigned:
  case value_t::number_float:
  default:return kNodeTypeNumber;
  }
}

template<typename T>
node_ptr_result_t script::internal_JSON_ConstructNode(T value) {
  return reinterpret_cast<node_ptr_result_t>(new nlohmann::json(value));
}

node_ptr_result_t script::JSON_Null() {
  return internal_JSON_ConstructNode(nullptr);
}

node_ptr_result_t script::JSON_Bool(const bool value) {
  return internal_JSON_ConstructNode(value);
}

node_ptr_result_t script::JSON_Int(const cell value) {
  return internal_JSON_ConstructNode(value);
}

node_ptr_result_t script::JSON_Float(const float value) {
  return internal_JSON_ConstructNode(value);
}

node_ptr_result_t script::JSON_String(const std::string value) {
  return internal_JSON_ConstructNode(cp2utf(value));
}

node_ptr_result_t script::JSON_Object(const cell *params) {
  if ((params[0] / 4) % 2 != 0 && params[0] != 0) {
    PLUGIN_LOG("Invalid variadic argument pattern passed: must be passed as pair");
    return 0;
  }
  auto obj = new nlohmann::json(nlohmann::json::object());
  size_t pairs = params[0] / sizeof(cell) / 2;
  for (size_t i = 0; i < pairs; ++i) {
    auto pair_ptr = params + (1 + (i * 2));
    auto key = GetString(*pair_ptr);
    auto item = *reinterpret_cast<node_ptr_t *>(GetPhysAddr(*(++pair_ptr)));
    if (item == nullptr)
      continue;
    (*obj)[key] = *item;
  }
  return reinterpret_cast<node_ptr_result_t>(obj);
}

node_ptr_result_t script::JSON_Array(cell *params) {
  auto arr = new nlohmann::json(nlohmann::json::array());
  for (size_t i = 1; i <= params[0] / sizeof(cell); ++i) {
    auto item = *reinterpret_cast<node_ptr_t *>(GetPhysAddr(params[i]));
    if (item == nullptr)
      continue;
    arr->emplace_back(*item);
  }
  return reinterpret_cast<node_ptr_result_t>(arr);
}

node_ptr_result_t script::JSON_Append(const node_ptr_t first_node, const node_ptr_t second_node) {
  // TODO: Returning errors from this method is ambiguous
  ASSERT_NODE_EXISTS(first_node);
  ASSERT_NODE_EXISTS(second_node);
  if (!first_node->is_object() && !first_node->is_array()) {
    PLUGIN_LOG("First array type does not equal to required one");
    return kCallWrongTypeError;
  }
  if (second_node->type() != first_node->type()) {
    PLUGIN_LOG("Second array type does not equal to first one");
    return kCallWrongTypeError;
  }
  auto copy_first_node = new nlohmann::json(*first_node);
  if (copy_first_node->is_object()) {
    copy_first_node->merge_patch(*second_node);
  } else {
    copy_first_node->insert(copy_first_node->end(), second_node->begin(), second_node->end());
  }
  return reinterpret_cast<node_ptr_result_t>(copy_first_node);
}

template<typename T>
call_result_t script::internal_JSON_SetValue(node_ptr_t node_id, const std::string key, const T value) {
  ASSERT_NODE_EXISTS(node_id);
  (*node_id)[key] = value;
  return kCallNoError;
}

call_result_t script::JSON_SetNull(node_ptr_t node_id, const std::string key) {
  return internal_JSON_SetValue(node_id, key, nullptr);
}

call_result_t script::JSON_SetBool(node_ptr_t node_id, const std::string key, bool value) {
  return internal_JSON_SetValue(node_id, key, value);
}

call_result_t script::JSON_SetInt(node_ptr_t node_id, const std::string key, cell value) {
  return internal_JSON_SetValue(node_id, key, value);
}

call_result_t script::JSON_SetFloat(node_ptr_t node_id, const std::string key, float value) {
  return internal_JSON_SetValue(node_id, key, value);
}

call_result_t script::JSON_SetString(node_ptr_t node_id, const std::string key, std::string value) {
  return internal_JSON_SetValue(node_id, key, cp2utf(value));
}

call_result_t script::JSON_SetObject(node_ptr_t node_id, const std::string key, node_ptr_t value_node) {
  ASSERT_NODE_EXISTS(value_node);
  return internal_JSON_SetValue(node_id, key, *value_node);
}

call_result_t script::JSON_SetArray(node_ptr_t node_id, const std::string key, node_ptr_t value_node) {
  ASSERT_NODE_EXISTS(value_node);
  return internal_JSON_SetValue(node_id, key, *value_node);
}

call_result_t script::JSON_GetBool(node_ptr_t node_id, const std::string key, bool *out) {
  ASSERT_NODE_EXISTS(node_id);
  if (!node_id->contains(key)) {
    PLUGIN_LOG("Node not exists");
    return kCallNodeNotExistsError;
  }
  if (!(*node_id)[key].is_boolean()) {
    PLUGIN_LOG("Array item '%s' type does not equal to required one", key.c_str());
    return kCallWrongTypeError;
  }
  *out = (*node_id)[key];
  return kCallNoError;
}

call_result_t script::JSON_GetInt(node_ptr_t node_id, const std::string key, cell *out) {
  ASSERT_NODE_EXISTS(node_id);
  if (!node_id->contains(key)) {
    PLUGIN_LOG("Node does not have item by key '%s'", key.c_str());
    return kCallNodeNotExistsError;
  }
  if (!(*node_id)[key].is_number_integer()) {
    PLUGIN_LOG("Array item '%s' type does not equal to required one", key.c_str());
    return kCallWrongTypeError;
  }
  *out = (*node_id)[key];
  return kCallNoError;
}

call_result_t script::JSON_GetFloat(node_ptr_t node_id, const std::string key, float *out) {
  ASSERT_NODE_EXISTS(node_id);
  if (!node_id->contains(key)) {
    PLUGIN_LOG("Node does not have item by key '%s'", key.c_str());
    return kCallNodeNotExistsError;
  }
  if (!(*node_id)[key].is_number_float()) {
    PLUGIN_LOG("Array item '%s' type does not equal to required one", key.c_str());
    return kCallWrongTypeError;
  }
  *out = (*node_id)[key];
  return kCallNoError;
}

call_result_t script::JSON_GetString(node_ptr_t node_id, const std::string key, cell *out, cell out_size) {
  ASSERT_NODE_EXISTS(node_id);
  if (!node_id->contains(key)) {
    PLUGIN_LOG("Node does not have item by key '%s'", key.c_str());
    return kCallNodeNotExistsError;
  }
  auto &node = (*node_id)[key];
  if (!node.is_string()) {
    PLUGIN_LOG("Array item '%s' type does not equal to required one", key.c_str());
    return kCallWrongTypeError;
  }
  auto str = utf2cp(node);
  if (!str.has_value())
    return kCallNoReturnStringError;
  SetString(out, str.value(), out_size);
  return kCallNoError;
}

call_result_t script::JSON_GetObject(node_ptr_t node_id, const std::string key, node_ptr_t *out) {
  ASSERT_NODE_EXISTS(node_id);
  if (!node_id->contains(key)) {
    PLUGIN_LOG("Node does not have item by key '%s'", key.c_str());
    return kCallNodeNotExistsError;
  }
//  TODO: This check seems to be useless?
//  if (!(*node_id)[key].is_object()) {
//    PLUGIN_LOG("Array item '%s' type does not equal to required one", key.c_str());
//    return kCallWrongTypeError;
//  }
  *out = reinterpret_cast<node_ptr_t>(new nlohmann::json((*node_id)[key]));
  return kCallNoError;
}

call_result_t script::JSON_GetArray(node_ptr_t node_id, const std::string key, node_ptr_t *out) {
  ASSERT_NODE_EXISTS(node_id);
  if (!node_id->contains(key)) {
    PLUGIN_LOG("Node does not have item by key '%s'", key.c_str());
    return kCallNodeNotExistsError;
  }
  if (!(*node_id)[key].is_array()) {
    PLUGIN_LOG("Array item '%s' type does not equal to required one", key.c_str());
    return kCallWrongTypeError;
  }
  *out = reinterpret_cast<node_ptr_t>(new nlohmann::json((*node_id)[key]));
  return kCallNoError;
}

node_type_t script::JSON_GetType(node_ptr_t node_id, const std::string key) {
  ASSERT_NODE_EXISTS(node_id);
  if (!node_id->contains(key)) {
    PLUGIN_LOG("Node does not have item by key '%s'", key.c_str());
    return kCallNodeNotExistsError;
  }
  return JSON_NodeType(reinterpret_cast<node_ptr_t>(&((*node_id)[key])));
}

call_result_t script::JSON_ArrayLength(node_ptr_t node_id, cell *out) {
  ASSERT_NODE_EXISTS(node_id);
  if (!node_id->is_array()) {
    PLUGIN_LOG("Node type does not equal to required one");
    return kCallWrongTypeError;
  }
  *out = node_id->size();
  return kCallNoError;
}

/**
 * This function is going to:
 * 1. Pick nlohmann::json from node by index
 * 2. Allocate JsonNode from nlohmann::json and push ptr to out
 * It may be useful together with JSON_NodeType, JSON_GetNode* to pick value from native JsonNode
 */
call_result_t script::JSON_ArrayObject(node_ptr_t node_id, cell index, node_ptr_t *out) {
  ASSERT_NODE_EXISTS(node_id);
  if (!node_id->is_array()) {
    PLUGIN_LOG("Node type does not equal to required one");
    return kCallWrongTypeError;
  }
  if (node_id->size() < index) {
    return kCallNodeNotExistsError;
  }
  *out = reinterpret_cast<node_ptr_t>(new nlohmann::json((*node_id)[index]));
  return kCallNoError;
}

call_result_t script::JSON_ArrayAppend(node_ptr_t node_id, const std::string key, node_ptr_t value_node) {
  ASSERT_NODE_EXISTS(node_id);
  ASSERT_NODE_EXISTS(value_node);
  if (!node_id->is_object()) {
    PLUGIN_LOG("Node type does not equal to required one");
    return kCallWrongTypeError;
  }
  auto &subnode = (*node_id)[key];
  if (!subnode.is_array()) {
    PLUGIN_LOG("Subnode type does not equal to required one");
    return kCallWrongTypeError;
  }
  subnode.emplace_back(*value_node);
  return kCallNoError;
}

call_result_t script::JSON_ArrayRemove(node_ptr_t node_id, const std::string key, node_ptr_t value_node) {
  ASSERT_NODE_EXISTS(node_id);
  ASSERT_NODE_EXISTS(value_node);
  if (!node_id->is_object()) {
    PLUGIN_LOG("Node type does not equal to required one");
    return kCallWrongTypeError;
  }
  auto &subnode = (*node_id)[key];
  if (!subnode.is_array()) {
    PLUGIN_LOG("Subnode type does not equal to required one");
    return kCallWrongTypeError;
  }
  for (auto ptr = subnode.cbegin(); ptr != subnode.end();) {
    if (*ptr == *value_node) {
      ptr = subnode.erase(ptr);
    } else {
      ++ptr;
    }
  }
  return kCallNoError;
}

call_result_t script::JSON_ArrayRemoveIndex(node_ptr_t node_id, const std::string key, cell index) {
  ASSERT_NODE_EXISTS(node_id);
  try {
    if (!node_id->contains(key)) {
      PLUGIN_LOG("Node does not have item by key '%s'", key.c_str());
      return kCallNodeNotExistsError;
    }
    auto &arr = (*node_id)[key];
    if (arr.size() < index) {
      PLUGIN_LOG("Node does not have item by index %d", index);
      return kCallNodeNotExistsError;
    }
    arr.erase(index);
    return kCallNoError;
  }
  catch (const std::exception &e) {
    return kCallUnknownError;
  }
}

call_result_t script::JSON_ArrayClear(node_ptr_t node_id, const std::string key) {
  ASSERT_NODE_EXISTS(node_id);
  if (!node_id->contains(key)) {
    PLUGIN_LOG("Node does not have item by key '%s'", key.c_str());
    return kCallNodeNotExistsError;
  }
  auto &subnode = (*node_id)[key];
//  TODO: Original plugin has check to node[key] type, should it be there?
//  if (!subnode.is_array()) {
//    PLUGIN_LOG("Subnode type does not equal to required one");
//    return kCallWrongTypeError;
//  }
  subnode.clear();
  return kCallNoError;
}

call_result_t script::JSON_Remove(node_ptr_t node_id, const std::string key) {
  ASSERT_NODE_EXISTS(node_id);
  if (!node_id->is_object()) {
    PLUGIN_LOG("Node type does not equal to required one");
    return kCallWrongTypeError;
  }
  node_id->erase(key);
  return kCallNoError;
}

call_result_t script::JSON_GetNodeBool(node_ptr_t node_id, bool *out) {
  ASSERT_NODE_EXISTS(node_id);
  if (!node_id->is_boolean()) {
    PLUGIN_LOG("Node type does not equal to required one");
    return kCallWrongTypeError;
  }
  *out = *node_id;
  return kCallNoError;
}

call_result_t script::JSON_GetNodeInt(node_ptr_t node_id, cell *out) {
  ASSERT_NODE_EXISTS(node_id);
  if (!node_id->is_number_integer()) {
    PLUGIN_LOG("Node type does not equal to required one");
    return kCallWrongTypeError;
  }
  *out = *node_id;
  return kCallNoError;
}

call_result_t script::JSON_GetNodeFloat(node_ptr_t node_id, float *out) {
  ASSERT_NODE_EXISTS(node_id);
  if (!node_id->is_number_float()) {
    PLUGIN_LOG("Node type does not equal to required one");
    return kCallWrongTypeError;
  }
  *out = *node_id;
  return kCallNoError;
}

call_result_t script::JSON_GetNodeString(node_ptr_t node_id, cell *out, cell out_size) {
  ASSERT_NODE_EXISTS(node_id);
  if (!node_id->is_string()) {
    PLUGIN_LOG("Node type does not equal to required one");
    return kCallWrongTypeError;
  }
  SetString(out, *node_id, out_size);
  return kCallNoError;
}

call_result_t script::JSON_Cleanup(node_ptr_t node_id) {
  ASSERT_NODE_EXISTS(node_id);
  delete node_id;
  return kCallNoError;
}
