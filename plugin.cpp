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

#include "plugin.h"

#define REGISTER_NATIVE(name) RegisterNative<&script::name>(#name)
#define REGISTER_NATIVE_EXPANDED(name) RegisterNative<&script::name, false>(#name)

bool plugin::OnLoad() {
  REGISTER_NATIVE(JSON_Parse);
  REGISTER_NATIVE(JSON_ParseFile);
  REGISTER_NATIVE(JSON_SaveFile);
  REGISTER_NATIVE(JSON_Stringify);
  REGISTER_NATIVE(JSON_NodeType);

  REGISTER_NATIVE(JSON_Null);
  REGISTER_NATIVE(JSON_Bool);
  REGISTER_NATIVE(JSON_Int);
  REGISTER_NATIVE(JSON_Float);
  REGISTER_NATIVE(JSON_String);
  REGISTER_NATIVE_EXPANDED(JSON_Object);
  REGISTER_NATIVE_EXPANDED(JSON_Array);

  REGISTER_NATIVE(JSON_Append);
//  REGISTER_NATIVE(JSON_Merge);

  REGISTER_NATIVE(JSON_SetNull);
  REGISTER_NATIVE(JSON_SetBool);
  REGISTER_NATIVE(JSON_SetInt);
  REGISTER_NATIVE(JSON_SetFloat);
  REGISTER_NATIVE(JSON_SetString);
  REGISTER_NATIVE(JSON_SetObject);
  REGISTER_NATIVE(JSON_SetArray);

  REGISTER_NATIVE(JSON_GetBool);
  REGISTER_NATIVE(JSON_GetInt);
  REGISTER_NATIVE(JSON_GetFloat);
  REGISTER_NATIVE(JSON_GetString);
  REGISTER_NATIVE(JSON_GetObject);
  REGISTER_NATIVE(JSON_GetArray);

  REGISTER_NATIVE(JSON_GetType);

  REGISTER_NATIVE(JSON_ArrayLength);
  REGISTER_NATIVE(JSON_ArrayObject);
  REGISTER_NATIVE(JSON_ArrayIterate);
  REGISTER_NATIVE(JSON_ArrayAppend);
  REGISTER_NATIVE(JSON_ArrayRemove);
  REGISTER_NATIVE(JSON_ArrayRemoveIndex);
  REGISTER_NATIVE(JSON_ArrayClear);
  //REGISTER_NATIVE(JSON_Keys);
  REGISTER_NATIVE(JSON_Remove);

  REGISTER_NATIVE(JSON_GetNodeBool);
  REGISTER_NATIVE(JSON_GetNodeInt);
  REGISTER_NATIVE(JSON_GetNodeFloat);
  REGISTER_NATIVE(JSON_GetNodeString);

  //REGISTER_NATIVE(JSON_ToggleGC);
  REGISTER_NATIVE(JSON_Cleanup);
  //REGISTER_NATIVE(JSON_CountNodes);

  Log("Plugin is loaded");

  return true;
}
#undef REGISTER_NATIVE

const char *plugin::Name() {
  return "YAPJ";
}

int plugin::Version() {
  return JSON_VERSION;
}
