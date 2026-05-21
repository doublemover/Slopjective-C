#include "sema/objc3_semantic_type_relations.h"

#include <cstddef>
#include <sstream>
#include <utility>

#include "sema/objc3_semantic_generic_collection_type_model.h"
#include "sema/objc3_semantic_type_predicates.h"
#include "support/objc3_value_type_names.h"

bool IsSameSemanticType(const SemanticTypeInfo &lhs, const SemanticTypeInfo &rhs) {
  if (lhs.is_vector != rhs.is_vector) {
    return false;
  }
  if (lhs.type != rhs.type) {
    return false;
  }
  if (lhs.is_callable != rhs.is_callable) {
    return false;
  }
  if (lhs.is_generic_collection != rhs.is_generic_collection) {
    return false;
  }
  if (lhs.is_generic_collection) {
    return AreSameObjc3GenericCollectionTypeModel(
        lhs.generic_collection_model, rhs.generic_collection_model);
  }
  if (!lhs.is_vector) {
    if (lhs.is_callable) {
      return lhs.callable_param_types == rhs.callable_param_types &&
             lhs.callable_return_type == rhs.callable_return_type;
    }
    return true;
  }
  return lhs.vector_lane_count == rhs.vector_lane_count &&
         lhs.vector_base_spelling == rhs.vector_base_spelling;
}

bool IsEscapingBlockRuntimeHandleCompatible(
    const SemanticTypeInfo &expected,
    const SemanticTypeInfo &value) {
  if (!IsCallableSemanticType(value) || !IsScalarSemanticType(expected)) {
    return false;
  }
  return expected.type == ValueType::I32 ||
         expected.type == ValueType::ObjCId ||
         expected.type == ValueType::ObjCObjectPtr;
}

std::string SemanticTypeName(const SemanticTypeInfo &info) {
  if (!info.is_vector) {
    if (info.is_generic_collection) {
      return info.generic_collection_model.public_spelling;
    }
    if (info.is_callable) {
      std::ostringstream out;
      out << "block(";
      for (std::size_t index = 0; index < info.callable_param_types.size(); ++index) {
        if (index > 0u) {
          out << ", ";
        }
        out << objc3c::support::ValueTypeName(info.callable_param_types[index]);
      }
      out << ") -> " << objc3c::support::ValueTypeName(info.callable_return_type);
      return out.str();
    }
    return objc3c::support::ValueTypeName(info.type);
  }
  const std::string base =
      info.vector_base_spelling.empty()
          ? std::string(objc3c::support::ValueTypeName(info.type))
          : info.vector_base_spelling;
  return base + "x" + std::to_string(info.vector_lane_count);
}
