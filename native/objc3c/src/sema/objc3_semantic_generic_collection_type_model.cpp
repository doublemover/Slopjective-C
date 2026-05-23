#include "sema/objc3_semantic_generic_collection_type_model.h"

#include <algorithm>
#include <cstddef>
#include <sstream>

#include "sema/objc3_semantic_generic_argument_relations.h"

namespace {

bool HasNestedGenericShape(const std::string &argument) {
  return argument.find('<') != std::string::npos ||
         argument.find('>') != std::string::npos;
}

std::string JoinTypeArguments(const std::vector<std::string> &arguments) {
  std::ostringstream out;
  for (std::size_t index = 0; index < arguments.size(); ++index) {
    if (index > 0u) {
      out << ",";
    }
    out << arguments[index];
  }
  return out.str();
}

bool IsSupportedCollectionElementType(const std::string &type_identity) {
  return type_identity == "i32" || type_identity == "bool" ||
         type_identity == "id" || type_identity == "object" ||
         type_identity == "TextHandle";
}

bool IsSupportedCollectionKeyType(const std::string &type_identity) {
  return type_identity == "i32" || type_identity == "bool" ||
         type_identity == "TextHandle";
}

std::string BuildDescriptorIdentity(
    Objc3GenericCollectionKind kind,
    const std::vector<std::string> &arguments_source_order) {
  std::ostringstream out;
  out << "objc3.collections.descriptor:"
      << Objc3GenericCollectionKindName(kind) << "<"
      << JoinTypeArguments(arguments_source_order) << ">";
  return out.str();
}

void RejectCollectionModel(Objc3GenericCollectionTypeModel &model,
                           const std::string &code,
                           const std::string &reason) {
  model.diagnostic_code = code;
  model.diagnostic_reason = reason;
  model.deterministic = true;
}

void PopulateCollectionShape(Objc3GenericCollectionTypeModel &model) {
  switch (model.kind) {
    case Objc3GenericCollectionKind::Array:
      model.mutability = Objc3GenericCollectionMutability::Immutable;
      model.element_type_identity = model.type_arguments_source_order[0];
      model.iterator_element_type_identity = model.element_type_identity;
      break;
    case Objc3GenericCollectionKind::MutableArray:
      model.mutability = Objc3GenericCollectionMutability::Mutable;
      model.element_type_identity = model.type_arguments_source_order[0];
      model.iterator_element_type_identity = model.element_type_identity;
      break;
    case Objc3GenericCollectionKind::Slice:
      model.mutability = Objc3GenericCollectionMutability::ImmutableView;
      model.element_type_identity = model.type_arguments_source_order[0];
      model.iterator_element_type_identity = model.element_type_identity;
      break;
    case Objc3GenericCollectionKind::Set:
      model.mutability = Objc3GenericCollectionMutability::Immutable;
      model.element_type_identity = model.type_arguments_source_order[0];
      model.iterator_element_type_identity = model.element_type_identity;
      break;
    case Objc3GenericCollectionKind::MutableSet:
      model.mutability = Objc3GenericCollectionMutability::Mutable;
      model.element_type_identity = model.type_arguments_source_order[0];
      model.iterator_element_type_identity = model.element_type_identity;
      break;
    case Objc3GenericCollectionKind::Iterator:
      model.mutability = Objc3GenericCollectionMutability::Immutable;
      model.element_type_identity = model.type_arguments_source_order[0];
      model.iterator_element_type_identity = model.element_type_identity;
      break;
    case Objc3GenericCollectionKind::Map:
      model.mutability = Objc3GenericCollectionMutability::Immutable;
      model.key_type_identity = model.type_arguments_source_order[0];
      model.value_type_identity = model.type_arguments_source_order[1];
      model.iterator_element_type_identity =
          "MapEntry<" + model.key_type_identity + "," +
          model.value_type_identity + ">";
      break;
    case Objc3GenericCollectionKind::MutableMap:
      model.mutability = Objc3GenericCollectionMutability::Mutable;
      model.key_type_identity = model.type_arguments_source_order[0];
      model.value_type_identity = model.type_arguments_source_order[1];
      model.iterator_element_type_identity =
          "MapEntry<" + model.key_type_identity + "," +
          model.value_type_identity + ">";
      break;
    case Objc3GenericCollectionKind::MapIterator:
      model.mutability = Objc3GenericCollectionMutability::Immutable;
      model.key_type_identity = model.type_arguments_source_order[0];
      model.value_type_identity = model.type_arguments_source_order[1];
      model.iterator_element_type_identity =
          "MapEntry<" + model.key_type_identity + "," +
          model.value_type_identity + ">";
      break;
    case Objc3GenericCollectionKind::None:
      break;
  }
}

bool HasExpectedArgumentCount(Objc3GenericCollectionKind kind,
                              std::size_t argument_count) {
  switch (kind) {
    case Objc3GenericCollectionKind::Array:
    case Objc3GenericCollectionKind::MutableArray:
    case Objc3GenericCollectionKind::Slice:
    case Objc3GenericCollectionKind::Set:
    case Objc3GenericCollectionKind::MutableSet:
    case Objc3GenericCollectionKind::Iterator:
      return argument_count == 1u;
    case Objc3GenericCollectionKind::Map:
    case Objc3GenericCollectionKind::MutableMap:
    case Objc3GenericCollectionKind::MapIterator:
      return argument_count == 2u;
    case Objc3GenericCollectionKind::None:
      return false;
  }
  return false;
}

}  // namespace

Objc3GenericCollectionKind Objc3GenericCollectionKindFromSpelling(
    const std::string &type_name) {
  if (type_name == "Array") {
    return Objc3GenericCollectionKind::Array;
  }
  if (type_name == "MutableArray") {
    return Objc3GenericCollectionKind::MutableArray;
  }
  if (type_name == "Slice") {
    return Objc3GenericCollectionKind::Slice;
  }
  if (type_name == "Map") {
    return Objc3GenericCollectionKind::Map;
  }
  if (type_name == "MutableMap") {
    return Objc3GenericCollectionKind::MutableMap;
  }
  if (type_name == "Set") {
    return Objc3GenericCollectionKind::Set;
  }
  if (type_name == "MutableSet") {
    return Objc3GenericCollectionKind::MutableSet;
  }
  if (type_name == "Iterator") {
    return Objc3GenericCollectionKind::Iterator;
  }
  if (type_name == "MapIterator") {
    return Objc3GenericCollectionKind::MapIterator;
  }
  return Objc3GenericCollectionKind::None;
}

const char *Objc3GenericCollectionKindName(Objc3GenericCollectionKind kind) {
  switch (kind) {
    case Objc3GenericCollectionKind::Array:
      return "Array";
    case Objc3GenericCollectionKind::MutableArray:
      return "MutableArray";
    case Objc3GenericCollectionKind::Slice:
      return "Slice";
    case Objc3GenericCollectionKind::Map:
      return "Map";
    case Objc3GenericCollectionKind::MutableMap:
      return "MutableMap";
    case Objc3GenericCollectionKind::Set:
      return "Set";
    case Objc3GenericCollectionKind::MutableSet:
      return "MutableSet";
    case Objc3GenericCollectionKind::Iterator:
      return "Iterator";
    case Objc3GenericCollectionKind::MapIterator:
      return "MapIterator";
    case Objc3GenericCollectionKind::None:
      return "None";
  }
  return "None";
}

bool IsObjc3GenericCollectionKind(Objc3GenericCollectionKind kind) {
  return kind != Objc3GenericCollectionKind::None;
}

Objc3GenericCollectionTypeModel BuildObjc3GenericCollectionTypeModel(
    const std::string &type_name,
    const std::vector<std::string> &arguments_source_order) {
  Objc3GenericCollectionTypeModel model;
  model.kind = Objc3GenericCollectionKindFromSpelling(type_name);
  if (!IsObjc3GenericCollectionKind(model.kind)) {
    return model;
  }

  model.public_spelling = std::string(Objc3GenericCollectionKindName(model.kind)) +
                          "<" + JoinTypeArguments(arguments_source_order) + ">";
  model.type_arguments_source_order.reserve(arguments_source_order.size());
  for (const std::string &argument : arguments_source_order) {
    model.type_arguments_source_order.push_back(
        NormalizeGenericArgumentTypeSpelling(argument));
  }

  if (!HasExpectedArgumentCount(model.kind,
                                model.type_arguments_source_order.size())) {
    RejectCollectionModel(model, "O3S206",
                          "missing or extra generic collection type arguments");
    return model;
  }
  if (std::any_of(model.type_arguments_source_order.begin(),
                  model.type_arguments_source_order.end(),
                  HasNestedGenericShape)) {
    RejectCollectionModel(model, "O3S206",
                          "nested generic collection shape is not supported");
    return model;
  }

  PopulateCollectionShape(model);
  if (!model.key_type_identity.empty() &&
      !IsSupportedCollectionKeyType(model.key_type_identity)) {
    RejectCollectionModel(model, "O3S206",
                          "unsupported generic collection key type");
    return model;
  }
  if (!model.element_type_identity.empty() &&
      !IsSupportedCollectionElementType(model.element_type_identity)) {
    RejectCollectionModel(model, "O3S206",
                          "unsupported generic collection element type");
    return model;
  }
  if (!model.value_type_identity.empty() &&
      !IsSupportedCollectionElementType(model.value_type_identity)) {
    RejectCollectionModel(model, "O3S206",
                          "unsupported generic collection value type");
    return model;
  }

  model.runtime_descriptor_identity =
      BuildDescriptorIdentity(model.kind, model.type_arguments_source_order);
  model.abi_identity = "objc3.collections.abi:" +
                       std::string(Objc3GenericCollectionKindName(model.kind)) +
                       ":" + JoinTypeArguments(model.type_arguments_source_order);
  model.deterministic = !model.runtime_descriptor_identity.empty() &&
                        !model.abi_identity.empty() &&
                        model.variance ==
                            Objc3GenericCollectionVariance::Invariant &&
                        model.descriptor_identity_required &&
                        model.value_semantics_are_handle_backed &&
                        model.ownership_is_runtime_descriptor_bound;
  return model;
}

Objc3GenericCollectionTypeModel BuildObjc3GenericCollectionTypeModel(
    const Objc3SemanticCanonicalType &type) {
  if (!type.is_objc_named_object_pointer || !type.has_generic_suffix) {
    return {};
  }
  return BuildObjc3GenericCollectionTypeModel(
      type.object_pointer_type_name, type.generic_arguments_source_order);
}

bool IsReadyObjc3GenericCollectionTypeModel(
    const Objc3GenericCollectionTypeModel &model) {
  return IsObjc3GenericCollectionKind(model.kind) && model.deterministic &&
         model.diagnostic_code.empty() &&
         model.variance == Objc3GenericCollectionVariance::Invariant &&
         !model.public_spelling.empty() &&
         !model.runtime_descriptor_identity.empty() &&
         !model.abi_identity.empty() &&
         model.value_semantics_are_handle_backed &&
         model.ownership_is_runtime_descriptor_bound &&
         model.descriptor_identity_required;
}

bool AreSameObjc3GenericCollectionTypeModel(
    const Objc3GenericCollectionTypeModel &lhs,
    const Objc3GenericCollectionTypeModel &rhs) {
  return IsReadyObjc3GenericCollectionTypeModel(lhs) &&
         IsReadyObjc3GenericCollectionTypeModel(rhs) && lhs.kind == rhs.kind &&
         lhs.type_arguments_source_order == rhs.type_arguments_source_order &&
         lhs.mutability == rhs.mutability &&
         lhs.runtime_descriptor_identity == rhs.runtime_descriptor_identity &&
         lhs.abi_identity == rhs.abi_identity;
}

bool AreObjc3GenericCollectionTypeModelsAssignmentCompatible(
    const Objc3GenericCollectionTypeModel &target,
    const Objc3GenericCollectionTypeModel &value) {
  if (!IsReadyObjc3GenericCollectionTypeModel(target) ||
      !IsReadyObjc3GenericCollectionTypeModel(value)) {
    return false;
  }
  return AreSameObjc3GenericCollectionTypeModel(target, value);
}
