#pragma once

#include <string>

#include "ast/objc3_ast_declarations.h"
#include "runtime/metadata/runtime_metadata_model.h"
#include "sema/objc3_sema_contract_core.h"
#include "sema/objc3_sema_contract_type_handoff.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] std::string BuildTypeSystemGenericContractPreservationJson(
    const Objc3SemanticTypeMetadataHandoff &handoff,
    const Objc3TypeSystemTypeSemanticModelSummary &semantic_summary);

[[nodiscard]] std::string BuildTypeSystemNullabilityContractPreservationJson(
    const Objc3TypeSystemTypeSemanticModelSummary &summary);

[[nodiscard]] std::string BuildTypeSystemProtocolContractPreservationJson(
    const Objc3Program &program,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_records,
    const Objc3TypeSystemTypeSemanticModelSummary &semantic_summary);

}  // namespace objc3::artifacts::frontend
