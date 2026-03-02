#pragma once

#include "sema/objc3_sema_pass_manager_contract.h"

void MarkObjc3SemaPassExecuted(Objc3SemaPassFlowSummary &summary, Objc3SemaPassId pass);

void FinalizeObjc3SemaPassFlowSummary(
    Objc3SemaPassFlowSummary &summary,
    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff,
    bool diagnostics_after_pass_monotonic,
    bool pass_order_matches_contract,
    bool deterministic_semantic_diagnostics,
    bool deterministic_type_metadata_handoff);
