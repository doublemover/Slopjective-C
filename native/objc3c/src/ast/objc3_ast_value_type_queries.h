#pragma once

#include "ast/objc3_ast_value_type.h"

const char *Objc3ValueTypeSpelling(ValueType type);
bool Objc3ValueTypeIsScalar(ValueType type);
bool Objc3ValueTypeIsObjectReference(ValueType type);
bool Objc3ValueTypeIsRuntimeMetadataReference(ValueType type);
