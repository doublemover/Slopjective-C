#pragma once

#include <string>
#include <vector>

#include "ast/objc3_ast_core.h"

struct Objc3BlockParameterSourceModel {
  std::string name;
  std::string type_spelling = "implicit-unspecified";
  bool explicit_type = false;
};

std::string BuildBlockParameterSignatureEntry(
    const Objc3BlockParameterSourceModel &parameter);
std::vector<std::string> BuildBlockParameterSignatureEntriesLexicographic(
    const std::vector<Objc3BlockParameterSourceModel> &parameters);
std::vector<ValueType> BuildBlockParameterTypesSourceOrder(
    const std::vector<Objc3BlockParameterSourceModel> &parameters);
std::string BuildBlockSignatureProfile(
    const std::vector<Objc3BlockParameterSourceModel> &parameters);
