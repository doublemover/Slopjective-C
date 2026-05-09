#pragma once

#include "sema/objc3_sema_contract.h"

bool IsEquivalentSelectorNormalizationSummary(const Objc3SelectorNormalizationSummary &lhs,
                                              const Objc3SelectorNormalizationSummary &rhs);
bool IsEquivalentPropertyAttributeSummary(const Objc3PropertyAttributeSummary &lhs,
                                          const Objc3PropertyAttributeSummary &rhs);
bool IsEquivalentTypeAnnotationSurfaceSummary(const Objc3TypeAnnotationSurfaceSummary &lhs,
                                              const Objc3TypeAnnotationSurfaceSummary &rhs);
bool IsEquivalentLightweightGenericConstraintSummary(
    const Objc3LightweightGenericConstraintSummary &lhs,
    const Objc3LightweightGenericConstraintSummary &rhs);
bool IsEquivalentNullabilityFlowWarningPrecisionSummary(
    const Objc3NullabilityFlowWarningPrecisionSummary &lhs,
    const Objc3NullabilityFlowWarningPrecisionSummary &rhs);
bool IsEquivalentProtocolQualifiedObjectTypeSummary(
    const Objc3ProtocolQualifiedObjectTypeSummary &lhs,
    const Objc3ProtocolQualifiedObjectTypeSummary &rhs);
bool IsEquivalentVarianceBridgeCastSummary(const Objc3VarianceBridgeCastSummary &lhs,
                                           const Objc3VarianceBridgeCastSummary &rhs);
bool IsEquivalentGenericMetadataAbiSummary(const Objc3GenericMetadataAbiSummary &lhs,
                                           const Objc3GenericMetadataAbiSummary &rhs);
bool IsEquivalentModuleImportGraphSummary(const Objc3ModuleImportGraphSummary &lhs,
                                          const Objc3ModuleImportGraphSummary &rhs);
bool IsEquivalentNamespaceCollisionShadowingSummary(
    const Objc3NamespaceCollisionShadowingSummary &lhs,
    const Objc3NamespaceCollisionShadowingSummary &rhs);
bool IsEquivalentPublicPrivateApiPartitionSummary(
    const Objc3PublicPrivateApiPartitionSummary &lhs,
    const Objc3PublicPrivateApiPartitionSummary &rhs);
bool IsEquivalentIncrementalModuleCacheInvalidationSummary(
    const Objc3IncrementalModuleCacheInvalidationSummary &lhs,
    const Objc3IncrementalModuleCacheInvalidationSummary &rhs);
bool IsEquivalentCrossModuleConformanceSummary(const Objc3CrossModuleConformanceSummary &lhs,
                                               const Objc3CrossModuleConformanceSummary &rhs);
bool IsEquivalentThrowsPropagationSummary(const Objc3ThrowsPropagationSummary &lhs,
                                          const Objc3ThrowsPropagationSummary &rhs);
bool IsEquivalentAsyncContinuationSummary(const Objc3AsyncContinuationSummary &lhs,
                                          const Objc3AsyncContinuationSummary &rhs);
bool IsEquivalentActorIsolationSendabilitySummary(
    const Objc3ActorIsolationSendabilitySummary &lhs,
    const Objc3ActorIsolationSendabilitySummary &rhs);
bool IsEquivalentTaskRuntimeCancellationSummary(const Objc3TaskRuntimeCancellationSummary &lhs,
                                                const Objc3TaskRuntimeCancellationSummary &rhs);
