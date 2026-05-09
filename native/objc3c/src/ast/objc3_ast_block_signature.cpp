#include "ast/objc3_ast_block_signature.h"

#include <algorithm>
#include <sstream>
#include <utility>

#include "ast/objc3_ast_ordering.h"

std::string BuildBlockParameterSignatureEntry(
    const Objc3BlockParameterSourceModel &parameter) {
  std::ostringstream out;
  out << "name=" << parameter.name
      << ";type=" << parameter.type_spelling
      << ";explicit_type=" << (parameter.explicit_type ? "true" : "false");
  return out.str();
}

std::vector<std::string> BuildBlockParameterSignatureEntriesLexicographic(
    const std::vector<Objc3BlockParameterSourceModel> &parameters) {
  std::vector<std::string> entries;
  entries.reserve(parameters.size());
  for (const auto &parameter : parameters) {
    entries.push_back(BuildBlockParameterSignatureEntry(parameter));
  }
  return Objc3AstSortedUniqueStrings(std::move(entries));
}

std::vector<ValueType> BuildBlockParameterTypesSourceOrder(
    const std::vector<Objc3BlockParameterSourceModel> &parameters) {
  std::vector<ValueType> types;
  types.reserve(parameters.size());
  for (const auto &parameter : parameters) {
    if (!parameter.explicit_type) {
      types.push_back(ValueType::Unknown);
      continue;
    }
    if (parameter.type_spelling == "i32") {
      types.push_back(ValueType::I32);
      continue;
    }
    if (parameter.type_spelling == "bool") {
      types.push_back(ValueType::Bool);
      continue;
    }
    if (parameter.type_spelling == "void") {
      types.push_back(ValueType::Void);
      continue;
    }
    types.push_back(ValueType::Unknown);
  }
  return types;
}

std::string BuildBlockSignatureProfile(
    const std::vector<Objc3BlockParameterSourceModel> &parameters) {
  const std::size_t explicit_typed_parameter_count =
      static_cast<std::size_t>(std::count_if(
          parameters.begin(), parameters.end(),
          [](const Objc3BlockParameterSourceModel &parameter) {
            return parameter.explicit_type;
          }));
  const std::size_t implicit_parameter_count =
      parameters.size() - explicit_typed_parameter_count;
  std::ostringstream out;
  out << "block-signature:parameters=" << parameters.size()
      << ";explicit-typed=" << explicit_typed_parameter_count
      << ";implicit=" << implicit_parameter_count
      << ";return-surface=body-inferred";
  return out.str();
}
