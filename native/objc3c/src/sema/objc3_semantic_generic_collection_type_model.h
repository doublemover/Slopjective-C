#pragma once

#include <string>
#include <vector>

#include "sema/model/generic_collection_type_model.h"

Objc3GenericCollectionKind Objc3GenericCollectionKindFromSpelling(
    const std::string &type_name);
const char *Objc3GenericCollectionKindName(Objc3GenericCollectionKind kind);
bool IsObjc3GenericCollectionKind(Objc3GenericCollectionKind kind);

Objc3GenericCollectionTypeModel BuildObjc3GenericCollectionTypeModel(
    const std::string &type_name,
    const std::vector<std::string> &arguments_source_order);
Objc3GenericCollectionTypeModel BuildObjc3GenericCollectionTypeModel(
    const Objc3SemanticCanonicalType &type);

bool IsReadyObjc3GenericCollectionTypeModel(
    const Objc3GenericCollectionTypeModel &model);
bool AreSameObjc3GenericCollectionTypeModel(
    const Objc3GenericCollectionTypeModel &lhs,
    const Objc3GenericCollectionTypeModel &rhs);
bool AreObjc3GenericCollectionTypeModelsAssignmentCompatible(
    const Objc3GenericCollectionTypeModel &target,
    const Objc3GenericCollectionTypeModel &value);
