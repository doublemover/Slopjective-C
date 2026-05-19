#pragma once

#include "pipeline/readiness/objc3_typed_sema_lowering_readiness.h"

void PopulateObjc3TypedSemaLoweringReadinessSurface(
    Objc3ParseLoweringReadinessSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3TypedSemaToLoweringContractSurface &typed_sema_to_lowering_contract_surface);

void ResolveObjc3TypedSemaLoweringBoundaryReadiness(
    Objc3ParseLoweringReadinessSurface &surface,
    const Objc3TypedSemaToLoweringContractSurface &typed_sema_to_lowering_contract_surface,
    const Objc3FrontendOptions &options);

bool IsObjc3TypedSemaCoreFeatureExpansionReady(
    const Objc3ParseLoweringReadinessSurface &surface);

void PopulateObjc3TypedSemaLoweringReadinessAlignments(
    Objc3TypedSemaLoweringReadinessRecord &record,
    const Objc3ParseLoweringReadinessSurface &surface,
    const Objc3TypedSemaToLoweringContractSurface &typed_sema_to_lowering_contract_surface);

bool IsObjc3TypedSemaCoreFeatureReady(
    const Objc3ParseLoweringReadinessSurface &surface,
    const Objc3TypedSemaLoweringReadinessRecord &record,
    bool typed_core_feature_expansion_ready);

bool IsObjc3TypedSemaSemanticHandoffDeterministic(
    const Objc3ParseLoweringReadinessSurface &surface);
