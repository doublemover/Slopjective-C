#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "contracts/objc3_diagnostic_owner_contract.h"
#include "sema/objc3_sema_canonical_literal_contract.h"
#include "sema/objc3_sema_closeout_readiness_contract.h"
#include "sema/objc3_sema_contract.h"
#include "sema/objc3_sema_pass_flow_core_contract.h"
#include "sema/objc3_sema_pass_flow_diagnostics_contract.h"
#include "sema/objc3_sema_pass_manager_publication_contract.h"

#include "sema/objc3_sema_pass_manager_type_metadata_records.h"

#include "sema/objc3_sema_pass_manager_closeout_surface_records.h"

#include "sema/objc3_sema_pass_manager_semantic_summary_records.h"

#include "sema/objc3_parser_sema_conformance_records.h"

#include "sema/objc3_parser_sema_readiness_records.h"

#include "sema/objc3_parser_sema_publication_records.h"

#include "sema/objc3_sema_pass_manager_semantic_publication_records.h"

#include "sema/objc3_sema_pass_manager_parity_validation_records.h"


struct Objc3SemaParityContractSurface {
#include "sema/objc3_sema_parity_contract_surface_record_fields.inc"
#include "sema/objc3_sema_parity_contract_surface_module_flow_fields.inc"
#include "sema/objc3_sema_parity_contract_surface_concurrency_error_fields.inc"
#include "sema/objc3_sema_parity_contract_surface_symbol_dispatch_arc_fields.inc"
#include "sema/objc3_sema_parity_contract_surface_determinism_fields.inc"
};
