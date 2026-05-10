#pragma once

#include <cstddef>
#include <string>
#include <vector>

struct Objc3Program;
struct Objc3RuntimeMetadataSourceRecordSet;
struct Objc3RuntimeSupportLibraryLinkWiringSummary;

namespace objc3::artifacts::frontend {

#include "artifacts/objc3_frontend_type_system_contract_records_constants.inc"

#include "artifacts/objc3_frontend_type_system_contract_records_lowering_contracts.inc"

#include "artifacts/objc3_frontend_type_system_contract_records_parity_surface.inc"

#include "artifacts/objc3_frontend_type_system_contract_records_semantic_metadata.inc"

#include "artifacts/objc3_frontend_type_system_contract_records_inventories.inc"

#include "artifacts/objc3_frontend_type_system_contract_records_api.inc"

}  // namespace objc3::artifacts::frontend
