#pragma once

#include <cstddef>
#include <string>

#include "pipeline/objc3_frontend_types.h"
#include "support/objc3_runtime_metadata_record_set.h"

namespace objc3c::pipeline {

void PreserveImportedRuntimeMetadataSourceRecordInventory(
    Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary,
    const Objc3RuntimeMetadataSourceRecordSet &records);

bool ValidateImportedRuntimeFrontendClosureInventory(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary,
    const Objc3RuntimeMetadataSourceRecordSet &records,
    std::size_t metadata_reference_count,
    std::string &error);

bool ValidateImportedRuntimeFrontendClosureHardCutoverBoundary(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary,
    std::string &error);

}  // namespace objc3c::pipeline
