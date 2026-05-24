#pragma once

#include <string>

#include "ast/objc3_ast_core.h"
#include "ir/objc3_ir_value_optional_carrier_model.h"

const char *LLVMScalarType(ValueType type);
unsigned LLVMScalarAlignment(ValueType type);
const char *LLVMLocalStorageType(ValueType type);
unsigned LLVMLocalStorageAlignment(ValueType type);
const char *LLVMScalarTypeForValueOptionalCarrier(
    ValueType type,
    const Objc3IRValueOptionalCarrierMetadata &carrier);
unsigned LLVMScalarAlignmentForValueOptionalCarrier(
    ValueType type,
    const Objc3IRValueOptionalCarrierMetadata &carrier);
const char *LLVMLocalStorageTypeForValueOptionalCarrier(
    ValueType type, Objc3IRValueOptionalCarrierKind carrier);
unsigned LLVMLocalStorageAlignmentForValueOptionalCarrier(
    ValueType type, Objc3IRValueOptionalCarrierKind carrier);
const char *LLVMZeroValueForValueOptionalCarrier(
    ValueType type, Objc3IRValueOptionalCarrierKind carrier);
ValueType RuntimeMetadataValueType(const std::string &type_name);
