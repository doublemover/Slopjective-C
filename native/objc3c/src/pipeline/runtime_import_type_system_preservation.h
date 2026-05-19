#pragma once

#include <string>

#include "pipeline/runtime_import_json_helpers.h"

struct Objc3ImportedRuntimeModuleSurface;

namespace objc3c::pipeline {

bool PopulateImportedTypeSystemOptionalKeypathSurface(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error);

bool PopulateImportedTypeSystemGenericContractPreservation(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error);

bool PopulateImportedTypeSystemNullabilityContractPreservation(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error);

bool PopulateImportedTypeSystemProtocolContractPreservation(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error);

}  // namespace objc3c::pipeline
