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
#define ASSERT_NODE_EXISTS(x) if ((x) == nullptr) { Log("%s: %d: error: node not exists", __FUNCTION__, __LINE__); return JSON_CALL_NODE_NOT_EXISTS_ERR; }

call_result_t script::JSON_Parse(const std::string buffer, node_ptr_t *node) {
  ASSERT_NODE_EXISTS(node);
  try {
    *node = new nlohmann::json(nlohmann::json::parse(buffer));
    return JSON_CALL_NO_ERR;
  } catch (const std::exception &e) {
    LOG_EXCEPTION(e);
    return JSON_CALL_PARSER_ERR;
  }
}

call_result_t script::JSON_ParseFile(const std::filesystem::path filename, node_ptr_t *node) {
  ASSERT_NODE_EXISTS(node);
  try {
    if (!exists(filename) || !is_regular_file(filename)) {
      return JSON_CALL_NO_SUCH_FILE_ERR;
    }
    std::ifstream f(filename);
    *node = new nlohmann::json(nlohmann::json::parse(f));
    return JSON_CALL_NO_ERR;
  } catch (const std::exception &e) {
    LOG_EXCEPTION(e);
    return JSON_CALL_PARSER_ERR;
  }
}

call_result_t script::JSON_SaveFile(const std::filesystem::path filename, const node_ptr_t node, const cell indent) {
  ASSERT_NODE_EXISTS(node);
  try {
    auto parent_path = filename.parent_path();
    if (!parent_path.empty()) {
      if (!exists(parent_path)) {
        create_directories(filename.parent_path());
      } else if (!is_directory(filename.parent_path())) {
        return JSON_CALL_NO_SUCH_DIR_ERR;
      }
    }
    std::ofstream o(filename, std::ofstream::out | std::ofstream::trunc);
    o.clear();
    o << node->dump(indent) << std::endl;
    return JSON_CALL_NO_ERR;
  } catch (const std::exception &e) {
    LOG_EXCEPTION(e);
    return JSON_CALL_PARSER_ERR;
  }
}

call_result_t script::JSON_Stringify(const node_ptr_t node, cell *out, const cell out_size, const cell indent) {
  ASSERT_NODE_EXISTS(node);
  try {
    auto str = node->dump(indent);
    SetString(out, str, out_size);
    return JSON_CALL_NO_ERR;
  } catch (const std::exception &e) {
    LOG_EXCEPTION(e);
    return JSON_CALL_UNKNOWN_ERR;
  }
}

node_type_t script::JSON_NodeType(const node_ptr_t node) {
  ASSERT_NODE_EXISTS(node);
  using value_t = nlohmann::json::value_t;
  switch (node->type()) {
  case value_t::null:return JSON_NODE_NULL;
  case value_t::object:return JSON_NODE_OBJECT;
  case value_t::array:return JSON_NODE_ARRAY;
  case value_t::string:return JSON_NODE_STRING;
  case value_t::boolean:return JSON_NODE_BOOLEAN;
    /*case value_t::binary:
      return "binary";
    case value_t::discarded:
      return "discarded";*/
  case value_t::number_integer:
  case value_t::number_unsigned:
  case value_t::number_float:
  default:return JSON_NODE_NUMBER;
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
    return JSON_CALL_WRONG_TYPE_ERR;
  }
  if (second_node->type() != first_node->type()) {
    PLUGIN_LOG("Second array type does not equal to first one");
    return JSON_CALL_WRONG_TYPE_ERR;
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
call_result_t script::internal_JSON_SetValue(node_ptr_t node, const std::string key, const T value) {
  ASSERT_NODE_EXISTS(node);
  (*node)[key] = value;
  return JSON_CALL_NO_ERR;
}

call_result_t script::JSON_SetNull(node_ptr_t node, const std::string key) {
  return internal_JSON_SetValue(node, key, nullptr);
}

call_result_t script::JSON_SetBool(node_ptr_t node, const std::string key, bool value) {
  return internal_JSON_SetValue(node, key, value);
}

call_result_t script::JSON_SetInt(node_ptr_t node, const std::string key, cell value) {
  return internal_JSON_SetValue(node, key, value);
}

call_result_t script::JSON_SetFloat(node_ptr_t node, const std::string key, float value) {
  return internal_JSON_SetValue(node, key, value);
}

call_result_t script::JSON_SetString(node_ptr_t node, const std::string key, std::string value) {
  return internal_JSON_SetValue(node, key, cp2utf(value));
}

call_result_t script::JSON_SetObject(node_ptr_t node, const std::string key, node_ptr_t value_node) {
  ASSERT_NODE_EXISTS(value_node);
  return internal_JSON_SetValue(node, key, *value_node);
}

call_result_t script::JSON_SetArray(node_ptr_t node, const std::string key, node_ptr_t value_node) {
  ASSERT_NODE_EXISTS(value_node);
  return internal_JSON_SetValue(node, key, *value_node);
}

call_result_t script::JSON_GetBool(node_ptr_t node, const std::string key, bool *out) {
  ASSERT_NODE_EXISTS(node);
  if (!node->contains(key)) {
    PLUGIN_LOG("Node not exists");
    return JSON_CALL_NODE_NOT_EXISTS_ERR;
  }
  auto &subnode = (*node)[key];
  if (!subnode.is_boolean()) {
    PLUGIN_LOG("Array item '%s' type does not equal to required one", key.c_str());
    return JSON_CALL_WRONG_TYPE_ERR;
  }
  *out = subnode;
  return JSON_CALL_NO_ERR;
}

call_result_t script::JSON_GetInt(node_ptr_t node, const std::string key, cell *out) {
  ASSERT_NODE_EXISTS(node);
  if (!node->contains(key)) {
    PLUGIN_LOG("Node does not have item by key '%s'", key.c_str());
    return JSON_CALL_NODE_NOT_EXISTS_ERR;
  }
  auto &subnode = (*node)[key];
  if (!subnode.is_number_integer()) {
    PLUGIN_LOG("Array item '%s' type does not equal to required one", key.c_str());
    return JSON_CALL_WRONG_TYPE_ERR;
  }
  *out = subnode;
  return JSON_CALL_NO_ERR;
}

call_result_t script::JSON_GetFloat(node_ptr_t node, const std::string key, float *out) {
  ASSERT_NODE_EXISTS(node);
  if (!node->contains(key)) {
    PLUGIN_LOG("Node does not have item by key '%s'", key.c_str());
    return JSON_CALL_NODE_NOT_EXISTS_ERR;
  }
  auto &subnode = (*node)[key];
  if (!subnode.is_number_float()) {
    PLUGIN_LOG("Array item '%s' type does not equal to required one", key.c_str());
    return JSON_CALL_WRONG_TYPE_ERR;
  }
  *out = subnode;
  return JSON_CALL_NO_ERR;
}

call_result_t script::JSON_GetString(node_ptr_t node, const std::string key, cell *out, cell out_size) {
  ASSERT_NODE_EXISTS(node);
  if (!node->contains(key)) {
    PLUGIN_LOG("Node does not have item by key '%s'", key.c_str());
    return JSON_CALL_NODE_NOT_EXISTS_ERR;
  }
  auto &subnode = (*node)[key];
  if (!subnode.is_string()) {
    PLUGIN_LOG("Array item '%s' type does not equal to required one", key.c_str());
    return JSON_CALL_WRONG_TYPE_ERR;
  }
  auto str = utf2cp(subnode);
  if (!str.has_value())
    return JSON_CALL_NO_RETURN_STRING_ERR;
  SetString(out, str.value(), out_size);
  return JSON_CALL_NO_ERR;
}

call_result_t script::JSON_GetObject(node_ptr_t node, const std::string key, node_ptr_t *out) {
  ASSERT_NODE_EXISTS(node);
  if (!node->contains(key)) {
    PLUGIN_LOG("Node does not have item by key '%s'", key.c_str());
    return JSON_CALL_NODE_NOT_EXISTS_ERR;
  }
//  TODO: This check seems to be useless?
//  if (!(*node)[key].is_object()) {
//    PLUGIN_LOG("Array item '%s' type does not equal to required one", key.c_str());
//    return JSON_CALL_WRONG_TYPE_ERR;
//  }
  *out = reinterpret_cast<node_ptr_t>(new nlohmann::json((*node)[key]));
  return JSON_CALL_NO_ERR;
}

call_result_t script::JSON_GetArray(node_ptr_t node, const std::string key, node_ptr_t *out) {
  ASSERT_NODE_EXISTS(node);
  if (!node->contains(key)) {
    PLUGIN_LOG("Node does not have item by key '%s'", key.c_str());
    return JSON_CALL_NODE_NOT_EXISTS_ERR;
  }
  auto &subnode = (*node)[key];
  if (!subnode.is_array()) {
    PLUGIN_LOG("Array item '%s' type does not equal to required one", key.c_str());
    return JSON_CALL_WRONG_TYPE_ERR;
  }
  *out = reinterpret_cast<node_ptr_t>(new nlohmann::json(subnode));
  return JSON_CALL_NO_ERR;
}

node_type_t script::JSON_GetType(node_ptr_t node, const std::string key) {
  ASSERT_NODE_EXISTS(node);
  if (!node->contains(key)) {
    PLUGIN_LOG("Node does not have item by key '%s'", key.c_str());
    return JSON_CALL_NODE_NOT_EXISTS_ERR;
  }
  return JSON_NodeType(reinterpret_cast<node_ptr_t>(&((*node)[key])));
}

call_result_t script::JSON_ArrayLength(node_ptr_t node, cell *out) {
  ASSERT_NODE_EXISTS(node);
  if (!node->is_array()) {
    PLUGIN_LOG("Node type does not equal to required one");
    return JSON_CALL_WRONG_TYPE_ERR;
  }
  *out = node->size();
  return JSON_CALL_NO_ERR;
}

/**
 * This function is going to:
 * 1. Pick nlohmann::json from node by index
 * 2. Allocate JsonNode from nlohmann::json and push ptr to out
 * It may be useful together with JSON_NodeType, JSON_GetNode* to pick value from native JsonNode
 */
call_result_t script::JSON_ArrayObject(node_ptr_t node, cell index, node_ptr_t *out) {
  ASSERT_NODE_EXISTS(node);
  if (!node->is_array()) {
    PLUGIN_LOG("Node type does not equal to required one");
    return JSON_CALL_WRONG_TYPE_ERR;
  }
  if (node->size() <= index) {
    return JSON_CALL_NODE_NOT_EXISTS_ERR;
  }
  *out = reinterpret_cast<node_ptr_t>(new nlohmann::json((*node)[index]));
  return JSON_CALL_NO_ERR;
}

call_result_t script::JSON_ArrayIterate(node_ptr_t node, cell *index, node_ptr_t *out) {
  ASSERT_NODE_EXISTS(node);
  if (!node->is_array()) {
    PLUGIN_LOG("Node type does not equal to required one");
    return JSON_CALL_WRONG_TYPE_ERR;
  }
  if (node->size() <= *index) {
    return JSON_CALL_NODE_NOT_EXISTS_ERR;
  }
  // TODO: Does it have to be there? Maybe AMX handles such destructors by itself?
  if (*out != nullptr)
    delete (*out);
  *out = new nlohmann::json((*node)[*index]);
  *index += 1;
  return JSON_CALL_NO_ERR;
}

call_result_t script::JSON_ArrayAppend(node_ptr_t node, const std::string key, node_ptr_t value_node) {
  ASSERT_NODE_EXISTS(node);
  ASSERT_NODE_EXISTS(value_node);
  if (!node->is_object()) {
    PLUGIN_LOG("Node type does not equal to required one");
    return JSON_CALL_WRONG_TYPE_ERR;
  }
  auto &subnode = (*node)[key];
  if (!subnode.is_array()) {
    PLUGIN_LOG("Subnode type does not equal to required one");
    return JSON_CALL_WRONG_TYPE_ERR;
  }
  subnode.emplace_back(*value_node);
  return JSON_CALL_NO_ERR;
}

call_result_t script::JSON_ArrayRemove(node_ptr_t node, const std::string key, node_ptr_t value_node) {
  ASSERT_NODE_EXISTS(node);
  ASSERT_NODE_EXISTS(value_node);
  if (!node->is_object()) {
    PLUGIN_LOG("Node type does not equal to required one");
    return JSON_CALL_WRONG_TYPE_ERR;
  }
  auto &subnode = (*node)[key];
  if (!subnode.is_array()) {
    PLUGIN_LOG("Subnode type does not equal to required one");
    return JSON_CALL_WRONG_TYPE_ERR;
  }
  for (auto ptr = subnode.cbegin(); ptr != subnode.end();) {
    if (*ptr == *value_node) {
      ptr = subnode.erase(ptr);
    } else {
      ++ptr;
    }
  }
  return JSON_CALL_NO_ERR;
}

call_result_t script::JSON_ArrayRemoveIndex(node_ptr_t node, const std::string key, cell index) {
  ASSERT_NODE_EXISTS(node);
  try {
    if (!node->is_object()) {
      PLUGIN_LOG("Node type does not equal to required one");
      return JSON_CALL_WRONG_TYPE_ERR;
    }
    if (!node->contains(key)) {
      PLUGIN_LOG("Node does not have item by key '%s'", key.c_str());
      return JSON_CALL_NODE_NOT_EXISTS_ERR;
    }
    auto &subnode = (*node)[key];
    if (!subnode.is_array()) {
      PLUGIN_LOG("Subnode type does not equal to required one");
      return JSON_CALL_WRONG_TYPE_ERR;
    }
    if (subnode.size() < index) {
      PLUGIN_LOG("Node does not have item by index %d", index);
      return JSON_CALL_NODE_NOT_EXISTS_ERR;
    }
    subnode.erase(index);
    return JSON_CALL_NO_ERR;
  }
  catch (const std::exception &e) {
    return JSON_CALL_UNKNOWN_ERR;
  }
}

call_result_t script::JSON_ArrayClear(node_ptr_t node, const std::string key) {
  ASSERT_NODE_EXISTS(node);
  if (!node->is_object()) {
    PLUGIN_LOG("Node type does not equal to required one");
    return JSON_CALL_WRONG_TYPE_ERR;
  }
  if (!node->contains(key)) {
    PLUGIN_LOG("Node does not have item by key '%s'", key.c_str());
    return JSON_CALL_NODE_NOT_EXISTS_ERR;
  }
  auto &subnode = (*node)[key];
//  TODO: Original plugin has check to node[key] type, should it be there?
//  if (!subnode.is_array()) {
//    PLUGIN_LOG("Subnode type does not equal to required one");
//    return JSON_CALL_WRONG_TYPE_ERR;
//  }
  subnode.clear();
  return JSON_CALL_NO_ERR;
}

call_result_t script::JSON_Remove(node_ptr_t node, const std::string key) {
  ASSERT_NODE_EXISTS(node);
  if (!node->is_object()) {
    PLUGIN_LOG("Node type does not equal to required one");
    return JSON_CALL_WRONG_TYPE_ERR;
  }
  node->erase(key);
  return JSON_CALL_NO_ERR;
}

call_result_t script::JSON_GetNodeBool(node_ptr_t node, bool *out) {
  ASSERT_NODE_EXISTS(node);
  if (!node->is_boolean()) {
    PLUGIN_LOG("Node type does not equal to required one");
    return JSON_CALL_WRONG_TYPE_ERR;
  }
  *out = *node;
  return JSON_CALL_NO_ERR;
}

call_result_t script::JSON_GetNodeInt(node_ptr_t node, cell *out) {
  ASSERT_NODE_EXISTS(node);
  if (!node->is_number_integer()) {
    PLUGIN_LOG("Node type does not equal to required one");
    return JSON_CALL_WRONG_TYPE_ERR;
  }
  *out = *node;
  return JSON_CALL_NO_ERR;
}

call_result_t script::JSON_GetNodeFloat(node_ptr_t node, float *out) {
  ASSERT_NODE_EXISTS(node);
  if (!node->is_number_float()) {
    PLUGIN_LOG("Node type does not equal to required one");
    return JSON_CALL_WRONG_TYPE_ERR;
  }
  *out = *node;
  return JSON_CALL_NO_ERR;
}

call_result_t script::JSON_GetNodeString(node_ptr_t node, cell *out, cell out_size) {
  ASSERT_NODE_EXISTS(node);
  if (!node->is_string()) {
    PLUGIN_LOG("Node type does not equal to required one");
    return JSON_CALL_WRONG_TYPE_ERR;
  }
  SetString(out, *node, out_size);
  return JSON_CALL_NO_ERR;
}

call_result_t script::JSON_Cleanup(node_ptr_t node) {
  // Silently return because node may be not initialized
  if (node == nullptr)
    return JSON_CALL_NODE_NOT_EXISTS_ERR;
  delete node;
  return JSON_CALL_NO_ERR;
}
