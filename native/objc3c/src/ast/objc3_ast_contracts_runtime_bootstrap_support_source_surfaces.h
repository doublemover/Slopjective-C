#pragma once

inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationSourceSurfaceContractId =
        "objc3c.runtime.bootstrap.registration.source.surface.v1";
inline constexpr const char
    *kObjc3RuntimeBootstrapLoweringRegistrationArtifactSurfaceContractId =
        "objc3c.runtime.bootstrap.lowering.registration.artifact.surface.v1";
inline constexpr const char
    *kObjc3RuntimeMultiImageStartupOrderingSourceSurfaceContractId =
        "objc3c.runtime.multi.image.startup.ordering.source.surface.v1";
inline constexpr const char
    *kObjc3RuntimeObjectModelRealizationSourceSurfaceContractId =
        "objc3c.runtime.object.model.realization.source.surface.v1";
inline constexpr const char
    *kObjc3RuntimeRealizationLoweringReflectionArtifactSurfaceContractId =
        "objc3c.runtime.realization.lowering.reflection.artifact.surface.v1";
inline constexpr const char
    *kObjc3RuntimeDispatchTableReflectionRecordLoweringSurfaceContractId =
        "objc3c.runtime.dispatch.table.reflection.record.lowering.surface.v1";
inline constexpr const char *kObjc3RuntimeObjectModelQueryBoundaryModel =
    "public-runtime-header-plus-private-testing-snapshots-freeze-the-object-model-lookup-and-reflection-query-surface-without-widening-the-public-abi";
inline constexpr const char
    *kObjc3RuntimeRealizationLookupReflectionImplementationModel =
        "runtime-owned-class-property-protocol-and-method-cache-query-snapshots-publish-coherent-last-query-state-and-live-object-model-counts";
inline constexpr const char *kObjc3RuntimeReflectionQuerySurfaceContractId =
    "objc3c.runtime.reflection.query.surface.v1";
inline constexpr const char *kObjc3RuntimeRealizationLookupSemanticsSurfaceContractId =
    "objc3c.runtime.realization.lookup.semantics.v1";
inline constexpr const char *kObjc3RuntimeClassMetaclassProtocolRealizationSurfaceContractId =
    "objc3c.runtime.class.metaclass.protocol.realization.v1";
inline constexpr const char *kObjc3RuntimeCategoryAttachmentMergedDispatchSurfaceContractId =
    "objc3c.runtime.category.attachment.merged.dispatch.surface.v1";
inline constexpr const char
    *kObjc3RuntimeReflectionVisibilityCoherenceDiagnosticsSurfaceContractId =
        "objc3c.runtime.reflection.visibility.coherence.diagnostics.surface.v1";
inline constexpr const char *kObjc3RuntimeUnifiedConcurrencySourceSurfaceContractId =
    "objc3c.runtime.unified.concurrency.source.surface.v1";
inline constexpr const char
    *kObjc3RuntimeAsyncTaskActorNormalizationCompletionSurfaceContractId =
        "objc3c.runtime.async.task.actor.normalization.completion.surface.v1";
inline constexpr const char
    *kObjc3RuntimeUnifiedConcurrencyLoweringMetadataSurfaceContractId =
        "objc3c.runtime.unified.concurrency.lowering.metadata.surface.v1";
inline constexpr const char
    *kObjc3RuntimeUnifiedConcurrencyRuntimeAbiSurfaceContractId =
        "objc3c.runtime.unified.concurrency.runtime.abi.surface.v1";
inline constexpr const char
    *kObjc3RuntimeUnifiedConcurrencyRuntimeAbiBoundaryModel =
        "private-async-task-and-actor-helper-entrypoints-plus-testing-snapshots-define-the-live-runtime-abi-without-widening-the-public-runtime-header";
inline constexpr const char
    *kObjc3RuntimeUnifiedConcurrencyContinuationRuntimeModel =
        "continuation-allocation-handoff-resume-cancel-and-testing-snapshots-stay-on-bootstrap-internal-runtime-entrypoints";
inline constexpr const char
    *kObjc3RuntimeUnifiedConcurrencyTaskRuntimeModel =
        "task-spawn-group-cancellation-executor-hop-and-testing-snapshots-stay-on-bootstrap-internal-runtime-entrypoints";
inline constexpr const char
    *kObjc3RuntimeUnifiedConcurrencyActorRuntimeModel =
        "actor-isolation-nonisolated-hop-replay-race-guard-mailbox-and-testing-snapshots-stay-on-bootstrap-internal-runtime-entrypoints";
inline constexpr const char
    *kObjc3RuntimeUnifiedConcurrencyRuntimeAbiFailClosedModel =
        "public-runtime-header-remains-registration-lookup-dispatch-only-until-deliberate-concurrency-runtime-abi-widening";
