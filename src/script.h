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
  /**
   * @brief Parses JSON buffer
   * @param buffer Buffer to parse
   * @param node Output node
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_PARSER_ERROR on parser error
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if no output node was provided
   */
  call_result_t       JSON_Parse(const std::string buffer, node_ptr_t *node);
  /**
   * @brief Parses JSON file
   * @param filename Name of file to parse
   * @param node Output node
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_PARSER_ERROR on parser error
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if no output node was provided
   *            JSON_CALL_NO_SUCH_FILE_ERROR if file not exists
   */
  call_result_t       JSON_ParseFile(const std::filesystem::path filename, node_ptr_t *node);
  /**
   * @brief Saves JSON node to file
   * @param filename Name of file to save in
   * @param node Node to save
   * @param indent Count of spaces for tabulation. Default: -1
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_UNKNOWN_ERROR on any exception
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if no node was provided
   *            JSON_CALL_NO_SUCH_DIRECTORY_ERROR if output path (not a file) does not exist
   */
  call_result_t       JSON_SaveFile(const std::filesystem::path filename, const node_ptr_t node, const cell indent);
  /**
   * @brief Converts JSON Node to string
   * @param node Node to convert
   * @param out Output buffer
   * @param out_size Output buffer size
   * @param indent Count of spaces for tabulation. Default: -1
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_PARSER_ERROR on parser error
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if no output node was provided
   */
  call_result_t       JSON_Stringify(const node_ptr_t node, cell *out, const cell out_size, const cell indent);
  /**
   * @brief Returns node type
   * @param node Node to check type of
   * @return    JsonNodeType on success
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if node was not provided
   */
  node_type_t         JSON_NodeType(const node_ptr_t node);

  template            <typename T>
  node_ptr_result_t   internal_JSON_ConstructNode(T value);

  /**
   * @brief Constructs null node
   * @return JsonNode
   */
  node_ptr_result_t   JSON_Null();
  /**
   * @brief Constructs boolean node
   * @param value Value to set
   * @return JsonNode
   */
  node_ptr_result_t   JSON_Bool(const bool value);
  /**
   * @brief Constructs int node
   * @param value Value to set
   * @return JsonNode
   */
  node_ptr_result_t   JSON_Int(const cell value);
  /**
   * @brief Constructs float node
   * @param value Value to set
   * @return JsonNode
   */
  node_ptr_result_t   JSON_Float(const float value);
  /**
   * @brief Contructs string node
   * @param value Value to set
   * @return JsonNode
   */
  node_ptr_result_t   JSON_String(const std::string value);
  /**
   * @brief Contructs object node
   * @param params List of <key, JsonNode> to set
   * @return JsonNode
   */
  node_ptr_result_t   JSON_Object(const cell *params);
  /**
   * @brief Contructs array node
   * @param params List of JsonNode to set
   * @return JsonNode
   */
  node_ptr_result_t   JSON_Array(cell *params);

  /** TODO: Complete */
  node_ptr_result_t   JSON_Append(const node_ptr_t first_node, const node_ptr_t second_node);
  /** TODO: Complete */
  node_ptr_result_t   JSON_Merge(const node_ptr_t first_node, const node_ptr_t second_node);

  template            <typename T>
  call_result_t       internal_JSON_SetValue(node_ptr_t node, const std::string key, const T value);
  /**
   * @brief Sets null to JsonNode[key]
   * @param node Parent node
   * @param key Key of JsonNode in object
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if no node was provided
   */
  call_result_t       JSON_SetNull(node_ptr_t node, const std::string key);
  /**
   * @brief Sets boolean to JsonNode[key]
   * @param node Parent node
   * @param key Key of JsonNode in object
   * @param value Value to set
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if no node was provided
   */
  call_result_t       JSON_SetBool(node_ptr_t node, const std::string key, const bool value);
  /**
   * @brief Sets integer to JsonNode[key]
   * @param node Parent node
   * @param key Key of JsonNode in object
   * @param value Value to set
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if no node was provided
   */
  call_result_t       JSON_SetInt(node_ptr_t node, const std::string key, const cell value);
  /**
   * @brief Sets float to JsonNode[key]
   * @param node Parent node
   * @param key Key of JsonNode in object
   * @param value Value to set
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if no node was provided
   */
  call_result_t       JSON_SetFloat(node_ptr_t node, const std::string key, const float value);
  /**
   * @brief Sets string to JsonNode[key]
   * @param node Parent node
   * @param key Key of JsonNode in object
   * @param value Value to set
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if no node was provided
   */
  call_result_t       JSON_SetString(node_ptr_t node, const std::string key, const std::string value);
  /**
   * @brief Sets boolean to JsonNode[key]
   * @param node Parent node
   * @param key Key of JsonNode in object
   * @param value_node Node to set
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if first/second node was not provided
   */
  call_result_t       JSON_SetObject(node_ptr_t node, const std::string key, const node_ptr_t value_node);
  /**
   * @brief Sets boolean to JsonNode[key]
   * @param node Parent node
   * @param key Key of JsonNode in object
   * @param value_node Node to set
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if first/second node was not provided
   */
  call_result_t       JSON_SetArray(node_ptr_t node, const std::string key, const node_ptr_t value_node);

  /**
   * @brief Gets boolean value of JsonNode within JsonNode by key
   * @param node Parent node
   * @param key Key of JsonNode in object
   * @param out Output value
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if node was not provided or there is no node by provided key
   */
  call_result_t       JSON_GetBool(node_ptr_t node, const std::string key, bool *out);
  /**
   * @brief Gets integer value of JsonNode within JsonNode by key
   * @param node Parent node
   * @param key Key of JsonNode in object
   * @param out Output value
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if node was not provided or there is no node by provided key
   */
  call_result_t       JSON_GetInt(node_ptr_t node, const std::string key, cell *out);
  /**
   * @brief Gets float value of JsonNode within JsonNode by key
   * @param node Parent node
   * @param key Key of JsonNode in object
   * @param out Output value
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if node was not provided or there is no node by provided key
   */
  call_result_t       JSON_GetFloat(node_ptr_t node, const std::string key, float *out);
  /**
   * @brief Gets string value of JsonNode within JsonNode by key
   * @param node Parent node
   * @param key Key of JsonNode in object
   * @param out Output buffer
   * @param out_size Output buffer size
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if node was not provided or there is no node by provided key
   */
  call_result_t       JSON_GetString(node_ptr_t node, const std::string key, cell *out, cell out_size);
  /**
   * @brief Gets value of JsonNode within JsonNode by key
   * @param node Parent node
   * @param key Key of JsonNode in object
   * @param out Output value
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if node was not provided or there is no node by provided key
   */
  call_result_t       JSON_GetObject(node_ptr_t node, const std::string key, node_ptr_t *out);
  /**
   * @brief Gets value of JsonNode within JsonNode by key
   * @param node Parent node
   * @param key Key of JsonNode in object
   * @param out Output value
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if node was not provided or there is no node by provided key
   */
  call_result_t       JSON_GetArray(node_ptr_t node, const std::string key, node_ptr_t *out);

  /**
   * @brief Gets type of JsonNode from object by provided key
   * @param node Parent node
   * @param key Key of JsonNode in object
   * @return    JsonNodeType on success
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if node was not provided or there is no node by provided key
   */
  node_type_t         JSON_GetType(node_ptr_t node, const std::string key);

  /**
   * @brief Gets size of JsonNode
   * @param node Node to get size of
   * @param out Output value
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if node was not provided or there is no node by provided key
   */
  call_result_t       JSON_ArrayLength(node_ptr_t node, cell *out);
  /**
   * @brief Returns native JsonNode from JSON array by index. Native JsonNode value can be accessed by various functions
   * @param node Parent node
   * @param index Index of item within array
   * @param out Output JsonNode
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_WRONG_TYPE_ERROR if parent node is not an array
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if node was not provided or there is no node by provided index
   */
  call_result_t       JSON_ArrayObject(node_ptr_t node, cell index, node_ptr_t *out);
  /**
   * @brief Iterates an array and pushes next item index and current item into out
   * @param node An array to iterate
   * @param index Index of next item
   * @param out Output JsonNode
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_WRONG_TYPE_ERROR if parent node is not an array
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if there is no any item within array anymore
   */
  call_result_t       JSON_ArrayIterate(node_ptr_t node, cell *index, node_ptr_t *out);
  /**
   * @brief Appends any given JsonNode to an existing JsonNode array
   * @param node Parent array to add to (object)
   * @param key Key of subnode to add in (array)
   * @param value_node Node to add
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_WRONG_TYPE_ERROR if parent node is not an object or subnode is not an array
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if there is no any item within array anymore
   */
  call_result_t       JSON_ArrayAppend(node_ptr_t node, const std::string key, node_ptr_t value_node);
  /**
   * @brief Removes item(s) from array within parent node if they equal to value_node
   * @param node Parent node (object)
   * @param key Key of array within parent object
   * @param value_node Similar node to compare by
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_WRONG_TYPE_ERROR if parent node or subnode is not an array
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if node was not provided or there is no node by provided key
   */
  call_result_t       JSON_ArrayRemove(node_ptr_t node, const std::string key, node_ptr_t value_node);
  /**
   * @brief Removes item from array by specified key within parent node by index
   * @param node Parent node (object)
   * @param key Key of array within parent object
   * @param index Index of item within array
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_WRONG_TYPE_ERROR if parent node or subnode is not an array
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if node was not provided or there is no node by provided key/index
   *            JSON_CALL_UNKNOWN_ERROR on any unhandled exception
   */
  call_result_t       JSON_ArrayRemoveIndex(node_ptr_t node, const std::string key, cell index);
  /**
   * @brief Clears an array within parent object by specified key
   * @param node Parent node (object)
   * @param key Key of array within parent object
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_WRONG_TYPE_ERROR if parent node or subnode is not an array
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if node was not provided or there is no node by provided key/index
   */
  call_result_t       JSON_ArrayClear(node_ptr_t node, const std::string key);
  call_result_t       JSON_Keys(node_ptr_t node, cell index, char *out, cell out_size);
  /**
   * @brief Removes an item from object by specified key
   * @param node Parent node (object)
   * @param key Key of item within object
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_WRONG_TYPE_ERROR if parent node or subnode is not an array
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if node was not provided or there is no node by provided key/index
   */
  call_result_t       JSON_Remove(node_ptr_t node, const std::string key);

  /**
   * @brief Gets a boolean value of native JsonNode
   * @param node Node
   * @param out Output value
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_WRONG_TYPE_ERROR if parent node or subnode is not an array
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if node was not provided
   */
  call_result_t       JSON_GetNodeBool(node_ptr_t node, bool *out);
  /**
   * @brief Gets a integer value of native JsonNode
   * @param node Node
   * @param out Output value
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_WRONG_TYPE_ERROR if parent node or subnode is not an array
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if node was not provided
   */
  call_result_t       JSON_GetNodeInt(node_ptr_t node, cell *out);
  /**
   * @brief Gets a float value of native JsonNode
   * @param node Node
   * @param out Output value
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_WRONG_TYPE_ERROR if parent node or subnode is not an array
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if node was not provided
   */
  call_result_t       JSON_GetNodeFloat(node_ptr_t node, float *out);
  /**
   * @brief Gets a string value of native JsonNode
   * @param node Node
   * @param out Output buffer
   * @param out_size Size of output buffer
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_WRONG_TYPE_ERROR if parent node or subnode is not an array
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if node was not provided
   */
  call_result_t       JSON_GetNodeString(node_ptr_t node, cell *out, cell out_size);

  /**
   * @brief ONLY FOR INTERNAL USAGE! Destroys allocated JsonNode.
   * @param node Node
   * @return    JSON_CALL_NO_ERROR on success
   *            JSON_CALL_NODE_NOT_EXISTS_ERROR if node was not provided
   */
  call_result_t       JSON_Cleanup(node_ptr_t node);
};
