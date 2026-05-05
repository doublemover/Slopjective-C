        "objc3c.tooling.corpus.sharding.release.evidence.packaging.v1";
inline constexpr const char
    *kObjc3ToolingCorpusShardingReleaseEvidencePackagingDependencyContractId =
        "objc3c.tooling.feature.aware.conformance.report.emission.v1";
inline constexpr const char
    *kObjc3ToolingCorpusShardingReleaseEvidencePackagingSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_tooling_corpus_sharding_release_evidence_packaging";
inline constexpr const char
    *kObjc3ToolingCorpusShardingReleaseEvidencePackagingPayloadModel =
        "versioned-conformance-report-now-embeds-tooling-corpus-shard-and-release-evidence-packaging-without-introducing-a-parallel-report-format";
inline constexpr const char
    *kObjc3ToolingCorpusShardingReleaseEvidencePackagingAuthorityModel =
        "tooling-release-evidence-packaging-remains-bounded-to-emitted-report-payloads-checklist-refs-and-stable-conformance-bucket-manifests";
inline constexpr const char *kObjc3RuntimeCapabilityModuleFormatVersion =
    "objc3c-runtime-metadata-v1";
// metadata-emission gate anchor: lane-E now freezes the upstream
// object-emission evidence contract over A002/B003/C006/D003 so later closeout
// work must fail closed if the source-to-section matrix, object-format policy,
// binary inspection corpus, or archive/static-link discovery proof drifts.
inline constexpr const char *kObjc3RuntimeMetadataEmissionGateContractId =
    "objc3c.runtime.metadata.emission.gate.v1";
inline constexpr const char *kObjc3RuntimeMetadataEmissionGateEvidenceModel =
    "source-sema-ir-runtime-summary-chain";
inline constexpr const char *kObjc3RuntimeMetadataEmissionGateFailureModel =
    "fail-closed-on-upstream-summary-drift";
// cross-lane object-emission closeout anchor: lane-E now freezes one
// integrated closeout over the E001 summary chain plus fresh native object
// probes so later startup-registration work can trust the same emitted objects
// on the class/category/message-send paths.
inline constexpr const char *kObjc3RuntimeMetadataObjectEmissionCloseoutContractId =
    "objc3c.runtime.cross.lane.object.emission.closeout.v1";
inline constexpr const char *kObjc3RuntimeMetadataObjectEmissionCloseoutEvidenceModel =
    "integrated-summary-plus-native-object-emission-probes";
inline constexpr const char *kObjc3RuntimeMetadataObjectEmissionCloseoutFailureModel =
    "fail-closed-on-summary-or-integrated-probe-drift";
// manifest/object/IR truth gate anchor: this binds the compiler sidecar
// manifest, emitted LLVM IR, native object, runtime registration artifacts,
// and release-claim sidecars into one deterministic evidence boundary.
inline constexpr const char *kObjc3ManifestObjectIrTruthGateContractId =
    "objc3c.manifest.object.ir.truth.gate.v1";
inline constexpr const char *kObjc3ManifestObjectIrTruthGateEvidenceModel =
    "manifest-ir-object-registration-and-conformance-sidecars-form-one-regenerated-truth-set";
inline constexpr const char *kObjc3ManifestObjectIrTruthGateManifestModel =
    "module.manifest.json-publishes-the-semantic-lowering-runtime-metadata-and-replay-key-source-of-truth";
inline constexpr const char *kObjc3ManifestObjectIrTruthGateIrModel =
    "module.ll-republishes-the-same-contract-boundaries-and-runtime-registration-roots-as-reviewable-ir-evidence";
inline constexpr const char *kObjc3ManifestObjectIrTruthGateObjectModel =
    "module.obj-materializes-the-same-objc3-runtime-sections-symbols-and-registration-roots-observed-in-ir";
inline constexpr const char *kObjc3ManifestObjectIrTruthGateClaimModel =
    "versioned-conformance-and-runtime-capability-sidecars-are-bound-to-the-same-replay-key-and-remain-narrower-than-evidence";
inline constexpr const char *kObjc3ManifestObjectIrTruthGateFailureModel =
    "missing-artifact-hash-drift-object-section-drift-or-unsupported-negative-emission-fails-closed";
// normalized layout policy anchor: semantic finalization of runtime
// metadata ordering, visibility, relocation, and retention now flows through
// one lowering-owned normalized policy packet before the IR emitter materializes
// globals. The emitter consumes the normalized plan directly instead of
// hardcoding family order or relocation semantics ad hoc.
inline constexpr const char *kObjc3RuntimeMetadataLayoutPolicyContractId =
    "objc3c.runtime.metadata.layout.policy.v1";
inline constexpr std::size_t kObjc3RuntimeMetadataLayoutPolicyFamilyCount = 5u;
inline constexpr const char *kObjc3RuntimeMetadataLayoutPolicyClassFamily =
    "class";
inline constexpr const char *kObjc3RuntimeMetadataLayoutPolicyProtocolFamily =
    "protocol";
inline constexpr const char *kObjc3RuntimeMetadataLayoutPolicyCategoryFamily =
    "category";
inline constexpr const char *kObjc3RuntimeMetadataLayoutPolicyPropertyFamily =
    "property";
inline constexpr const char *kObjc3RuntimeMetadataLayoutPolicyIvarFamily =
    "ivar";
inline constexpr const char *kObjc3AtomicMemoryOrderRelaxed = "relaxed";
inline constexpr const char *kObjc3AtomicMemoryOrderAcquire = "acquire";
inline constexpr const char *kObjc3AtomicMemoryOrderRelease = "release";
inline constexpr const char *kObjc3AtomicMemoryOrderAcqRel = "acq_rel";
inline constexpr const char *kObjc3AtomicMemoryOrderSeqCst = "seq_cst";
inline constexpr const char *kObjc3SimdVectorLaneContract = "2,4,8,16";
inline constexpr const char *kObjc3SimdVectorBaseI32 = "i32";
inline constexpr const char *kObjc3SimdVectorBaseBool = "bool";
inline constexpr const char *kObjc3MethodLookupOverrideConflictLaneContract =
    "objc3c.method.lookup.override.conflict.v1";
inline constexpr const char *kObjc3PropertySynthesisIvarBindingLaneContract =
    "objc3c.property.synthesis.ivar.binding.v1";
inline constexpr const char *kObjc3IdClassSelObjectPointerTypecheckLaneContract =
    "objc3c.id.class.sel.object.pointer.typecheck.v1";
inline constexpr const char *kObjc3MessageSendSelectorLoweringLaneContract =
    "objc3c.message.send.selector.lowering.v1";
inline constexpr const char *kObjc3DispatchAbiMarshallingLaneContract =
    "objc3c.dispatch.abi.marshalling.v1";
inline constexpr const char *kObjc3NilReceiverSemanticsFoldabilityLaneContract =
    "objc3c.nil.receiver.semantics.foldability.v1";
inline constexpr const char *kObjc3SuperDispatchMethodFamilyLaneContract =
    "objc3c.super.dispatch.method.family.v1";
inline constexpr const char *kObjc3RuntimeLinkHostLinkLaneContract =
    "objc3c.runtime.dispatch.host.link.v1";
inline constexpr const char *kObjc3OwnershipQualifierLoweringLaneContract =
    "objc3c.ownership.qualifier.lowering.v1";
inline constexpr const char *kObjc3RetainReleaseOperationLoweringLaneContract =
    "objc3c.retain.release.operation.lowering.v1";
inline constexpr const char *kObjc3AutoreleasePoolScopeLoweringLaneContract =
    "objc3c.autoreleasepool.scope.lowering.v1";
inline constexpr const char *kObjc3WeakUnownedSemanticsLoweringLaneContract =
    "objc3c.weak.unowned.semantics.lowering.v1";
inline constexpr const char *kObjc3ArcDiagnosticsFixitLoweringLaneContract =
    "objc3c.arc.diagnostics.fixit.lowering.v1";
inline constexpr const char *kObjc3BlockLiteralCaptureLoweringLaneContract =
    "objc3c.block.literal.capture.lowering.v1";
inline constexpr const char *kObjc3BlockAbiInvokeTrampolineLoweringLaneContract =
    "objc3c.block.abi.invoke.trampoline.lowering.v1";
inline constexpr const char *kObjc3BlockStorageEscapeLoweringLaneContract =
    "objc3c.block.storage.escape.lowering.v1";
inline constexpr const char *kObjc3BlockCopyDisposeLoweringLaneContract =
    "objc3c.block.copy.dispose.lowering.v1";
inline constexpr const char *kObjc3BlockDeterminismPerfBaselineLoweringLaneContract =
    "objc3c.block.determinism.perf.baseline.lowering.v1";
inline constexpr const char *kObjc3BlockSourceModelCompletionLaneContract =
    "objc3c.block.source.model.v1";
inline constexpr const char *kObjc3BlockSourceStorageAnnotationLaneContract =
    "objc3c.block.source.storage.annotations.v1";
// capture-legality/escape/invocation implementation anchor: lane-B
// keeps the same runtime-summary lane contract while source-only sema grows
// live capture-resolution, truthful escape classification, and local callable
// invocation typing before runnable block lowering lands.
inline constexpr const char *kObjc3BlockRuntimeSemanticRulesLaneContract =
    "objc3c.block.runtime.semantic.rules.v1";
inline constexpr const char
    *kObjc3ExecutableBlockLoweringAbiArtifactBoundaryLaneContract =
        "objc3c.block.lowering.abi.artifact.boundary.v1";
inline constexpr const char *kObjc3ExecutableBlockObjectInvokeThunkLoweringLaneContract =
    "objc3c.block.object.invoke.thunk.lowering.v1";
inline constexpr const char *kObjc3ExecutableBlockByrefHelperLoweringLaneContract =
    "objc3c.block.byref.helper.lowering.v1";
inline constexpr const char *kObjc3ExecutableBlockEscapeRuntimeHookLoweringLaneContract =
    "objc3c.block.escape.runtime.hook.lowering.v1";
inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringLaneContract =
    "objc3c.type_system.optional.keypath.lowering.v1";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringLaneContract =
    "objc3c.control_flow.control.flow.safety.lowering.v1";
inline constexpr const char *kObjc3LightweightGenericsConstraintLoweringLaneContract =
    "objc3c.lightweight.generics.constraint.lowering.v1";
inline constexpr const char *kObjc3NullabilityFlowWarningPrecisionLoweringLaneContract =
    "objc3c.nullability.flow.warning.precision.lowering.v1";
inline constexpr const char *kObjc3ProtocolQualifiedObjectTypeLoweringLaneContract =
    "objc3c.protocol.qualified.object.type.lowering.v1";
inline constexpr const char *kObjc3VarianceBridgeCastLoweringLaneContract =
    "objc3c.variance.bridge.cast.lowering.v1";
inline constexpr const char *kObjc3GenericMetadataAbiLoweringLaneContract =
    "objc3c.generic.metadata.abi.lowering.v1";
inline constexpr const char *kObjc3ModuleImportGraphLoweringLaneContract =
    "objc3c.module.import.graph.lowering.v1";
inline constexpr const char *kObjc3NamespaceCollisionShadowingLoweringLaneContract =
    "objc3c.namespace.collision.shadowing.lowering.v1";
inline constexpr const char *kObjc3PublicPrivateApiPartitionLoweringLaneContract =
    "objc3c.public.private.api.partition.lowering.v1";
inline constexpr const char *kObjc3IncrementalModuleCacheInvalidationLoweringLaneContract =
    "objc3c.incremental.module.cache.invalidation.lowering.v1";
inline constexpr const char *kObjc3CrossModuleConformanceLoweringLaneContract =
    "objc3c.cross.module.conformance.lowering.v1";
inline constexpr const char *kObjc3ThrowsPropagationLoweringLaneContract =
    "objc3c.throws.propagation.lowering.v1";
inline constexpr const char *kObjc3ResultLikeLoweringLaneContract =
    "objc3c.result.like.lowering.v1";
inline constexpr const char *kObjc3NSErrorBridgingLoweringLaneContract =
    "objc3c.ns.error.bridging.lowering.v1";
inline constexpr const char *kObjc3UnwindCleanupLoweringLaneContract =
    "objc3c.unwind.cleanup.lowering.v1";
inline constexpr const char *kObjc3ErrorDiagnosticsRecoveryLoweringLaneContract =
    "objc3c.error.diagnostics.recovery.lowering.v1";
inline constexpr const char *kObjc3AsyncContinuationLoweringLaneContract =
    "objc3c.async.continuation.lowering.v1";
inline constexpr const char
    *kObjc3AwaitLoweringSuspensionStateLoweringLaneContract =
        "objc3c.await.lowering.suspension.state.lowering.v1";
// freeze anchor: these lane contracts become the explicit Part 7
// continuation ABI / await suspension lowering boundary once frontend artifacts
// publish the replay-stable handoff packet into emitted manifests and IR.
// implementation anchor: the currently supported runnable lowering
// slice is narrower than the lane-contract names imply. Async entry points and
// await-marked expressions lower through the existing direct-call/object path
// only for the non-suspending happy path, while continuation allocation,
// suspend/resume, and executor scheduling remain deferred.
// integration anchor: the same supported async slice composes with
// existing autoreleasepool-scope and defer-cleanup lowering rather than a
// separate suspension cleanup runtime. Later work still has to widen that
// into real suspension-frame cleanup and executor resume behavior.
inline constexpr const char *kObjc3ActorIsolationSendabilityLoweringLaneContract =
    "objc3c.actor.isolation.sendability.lowering.v1";
// lowering-freeze anchor: Part 7 actor lowering now freezes one
// dedicated emitted contract for actor metadata carriage, isolation-thunk
// planning, and hop-artifact planning. Live thunk bodies and runtime entrypoints
// remain later C002/C003 work, but this packet is the canonical lowering
// handoff for actor-focused IR metadata and manifest truth.
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataContractId =
    "objc3c.concurrency.actor.lowering.and.metadata.contract.v1";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_concurrency_actor_lowering_and_metadata_contract";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataModel =
    "actor-member-semantic-and-hazard-packets-now-lower-through-one-deterministic-actor-metadata-isolation-thunk-and-hop-artifact-contract";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataDeferredModel =
    "live-actor-thunk-bodies-mailbox-runtime-entrypoints-and-runnable-cross-actor-scheduling-remain-later-actor-lowering-and-replay-runtime-work";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataLaneContract =
    "objc3c.actor.lowering.metadata.contract.v1";
// lowering-freeze anchor: Part 8 now freezes one explicit emitted
// lowering contract that carries cleanup/resource ownership counts, borrowed
// boundary counts, and retainable-family callable counts into manifests and IR
// metadata without overclaiming live runtime cleanup or borrowed lifetime
// execution. Runtime integration must widen this exact contract family rather than inventing
// a second system-extension lowering boundary.
// lowering-implementation anchor: live Part 8 lowering now consumes
// this frozen contract directly for native helper emission and object proof.
// Stack/local cleanup-resource capture lowering is implemented; actual escaping
// move-capture promotion stays fail-closed until later runtime ownership
// transfer work widens the same boundary explicitly.
inline constexpr const char *kObjc3OwnershipSystemExtensionLoweringContractId =
    "objc3c.ownership.system.extension.lowering.contract.v1";
inline constexpr const char *kObjc3OwnershipSystemExtensionLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_ownership_system_extension_lowering_contract";
inline constexpr const char *kObjc3OwnershipSystemExtensionLoweringModel =
    "cleanup-resource-borrowed-and-retainable-family-sema-packets-now-feed-one-deterministic-ownership-lowering-contract-for-manifest-and-ir-carriage";
inline constexpr const char *kObjc3OwnershipSystemExtensionLoweringDeferredModel =
    "live-cleanup-runtime-carriers-borrowed-lifetime-enforcement-and-runnable-retainable-family-runtime-interop-remain-later-system-extension-runtime-work";
inline constexpr const char *kObjc3OwnershipSystemExtensionLoweringLaneContract =
    "objc3c.ownership.system.extension.lowering.contract.v1";
// lowering-freeze anchor: Part 9 now freezes one explicit emitted
// lowering contract for direct-call candidates, final/sealed dispatch-boundary
// metadata, and replay-stable callable/container dynamism intent carriage. Live
// selector-bypass/direct-call rewrites and runnable dispatch-boundary behavior
// remain later lane-C and lane-D work.
inline constexpr const char *kObjc3DispatchDispatchControlLoweringContractId =
    "objc3c.dispatch.dispatch.control.lowering.contract.v1";
inline constexpr const char *kObjc3DispatchDispatchControlLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_dispatch_dispatch_control_lowering_contract";
inline constexpr const char *kObjc3DispatchDispatchControlLoweringModel =
    "dispatch-direct-call-candidates-final-sealed-boundaries-and-dynamism-intent-metadata-now-feed-one-deterministic-lowering-contract-for-manifest-and-ir-carriage";
inline constexpr const char *kObjc3DispatchDispatchControlLoweringDeferredModel =
    "live-direct-call-selector-bypass-runtime-dispatch-boundary-realization-and-runnable-metadata-consumption-remain-later-dispatch-control-runtime-work";
inline constexpr const char *kObjc3DispatchDispatchControlLoweringLaneContract =
    "objc3c.dispatch.dispatch.control.lowering.contract.v1";
// expansion/lowering freeze anchor: lane-C freezes one deterministic
// Part 10 lowering packet over derived selector inventory, macro replay
// visibility, and synthesized property metadata carriage. Runnable derive body
// emission, macro execution, and property-behavior runtime hooks remain later
// work.
inline constexpr const char *kObjc3MetaprogrammingExpansionLoweringContractId =
    "objc3c.metaprogramming.expansion.lowering.contract.v1";
inline constexpr const char *kObjc3MetaprogrammingExpansionLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_metaprogramming_expansion_and_lowering_contract";
inline constexpr const char *kObjc3MetaprogrammingExpansionLoweringModel =
    "metaprogramming-derived-selector-inventory-macro-replay-visibility-and-synthesized-property-metadata-now-feed-one-deterministic-lowering-contract-for-manifest-and-ir-carriage";
inline constexpr const char *kObjc3MetaprogrammingExpansionLoweringDeferredModel =
    "runnable-derive-body-emission-macro-execution-and-property-behavior-runtime-materialization-remain-later-expansion-runtime-work";
inline constexpr const char *kObjc3MetaprogrammingExpansionLoweringLaneContract =
    "objc3c.metaprogramming.expansion.lowering.contract.v1";
inline constexpr const char *kObjc3MetaprogrammingSynthesizedArtifactEmissionContractId =
    "objc3c.metaprogramming.synthesized.ast.ir.emission.v1";
inline constexpr const char *kObjc3MetaprogrammingSynthesizedArtifactEmissionSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_metaprogramming_synthesized_ast_and_ir_emission";
inline constexpr const char *kObjc3MetaprogrammingSynthesizedArtifactEmissionModel =
    "supported-derive-macro-and-property-behavior-sites-now-materialize-deterministic-synthesized-ir-artifacts-and-runtime-visible-method-bodies";
inline constexpr const char
    *kObjc3MetaprogrammingSynthesizedArtifactEmissionDeferredModel =
        "cross-module-preservation-expansion-host-execution-and-cached-macro-toolchain-integration-remain-deferred-to-later-runtime-work";
inline constexpr const char *kObjc3MetaprogrammingSynthesizedArtifactEmissionLaneContract =
    "objc3c.metaprogramming.synthesized.ast.ir.emission.v1";
// preservation anchor: lane-C extends the local C002 synthesis win by
// preserving derive/macro/property-behavior replay facts, interface-facing
// artifact splits, and deterministic import-surface summaries across module
// boundaries before lane-D runtime execution work begins.
inline constexpr const char
    *kObjc3MetaprogrammingModuleInterfaceReplayPreservationContractId =
        "objc3c.metaprogramming.module.interface.replay.preservation.v1";
inline constexpr const char
    *kObjc3MetaprogrammingModuleInterfaceReplayPreservationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_metaprogramming_module_interface_and_replay_preservation";
inline constexpr const char
    *kObjc3MetaprogrammingModuleInterfaceReplayPreservationImportArtifactMemberName =
        "objc_metaprogramming_module_interface_and_replay_preservation";
inline constexpr const char
    *kObjc3MetaprogrammingModuleInterfaceReplayPreservationSourceModel =
        "runtime-import-surface-artifacts-preserve-metaprogramming-derived-method-macro-and-property-behavior-replay-facts-for-separate-compilation-and-interface-inspection";
inline constexpr const char
    *kObjc3MetaprogrammingModuleInterfaceReplayPreservationModel =
        "provider-and-consumer-import-surfaces-preserve-metaprogramming-synthesized-emission-counts-replay-keys-and-interface-vs-implementation-property-behavior-splits-beyond-local-ir-object-emission";
inline constexpr const char
    *kObjc3MetaprogrammingModuleInterfaceReplayPreservationFailClosedModel =
        "missing-or-drifted-metaprogramming-preservation-packets-disable-cross-module-metaprogramming-preservation-claims";
// host/runtime-boundary freeze anchor: lane-D freezes the truthful
// Part 10 runtime boundary around the existing packaged runtime archive and
// private property-accessor helpers while keeping macro host execution and
// runtime package loading explicitly disabled and fail-closed.
inline constexpr const char *kObjc3MetaprogrammingExpansionHostRuntimeBoundaryContractId =
    "objc3c.metaprogramming.expansion.host.runtime.boundary.v1";
inline constexpr const char
    *kObjc3MetaprogrammingExpansionHostRuntimeBoundarySourceContractId =
        "objc3c.metaprogramming.module.interface.replay.preservation.v1";
inline constexpr const char *kObjc3MetaprogrammingExpansionHostRuntimeBoundaryHostModel =
    "macro-host-execution-process-launch-and-runtime-package-loading-remain-disabled-and-fail-closed";
inline constexpr const char
    *kObjc3MetaprogrammingExpansionHostRuntimeBoundaryPropertyRuntimeModel =
        "supported-property-behavior-lowering-reuses-existing-private-runtime-property-accessor-layout-and-current-property-hooks";
inline constexpr const char
    *kObjc3MetaprogrammingExpansionHostRuntimeBoundaryPackagingModel =
        "native-driver-packaging-still-hands-off-metaprogramming-runtime-support-through-artifacts-lib-objc3_runtime-lib-and-runtime-registration-manifests";
inline constexpr const char
    *kObjc3MetaprogrammingExpansionHostRuntimeBoundaryFailClosedModel =
        "no-live-macro-expansion-host-or-runtime-package-loader-is-claimed-yet";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationContractId =
        "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSourceContractId =
        "objc3c.metaprogramming.expansion.host.runtime.boundary.v1";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_metaprogramming_macro_host_process_and_cache_runtime_integration";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationImportArtifactMemberName =
        "objc_metaprogramming_macro_host_process_and_cache_runtime_integration";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationHostExecutableRelativePath =
        "artifacts/bin/objc3c-frontend-c-api-runner.exe";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationCacheRootRelativePath =
        "tmp/artifacts/objc3c-native/cache/metaprogramming";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationHostModel =
        "native-driver-launches-objc3c-frontend-c-api-runner-for-supported-metaprogramming-expansion-cache-materialization";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationToolchainModel =
        "frontend-runner-executes-with-manifest-enabled-and-ir-object-emission-disabled-for-deterministic-cache-materialization";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationCacheModel =
        "cache-entry-path-is-derived-from-a-stable-fnv1a64-key-over-the-metaprogramming-replay-surface-and-reused-on-subsequent-runs";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationFailClosedModel =
        "missing-runner-corrupt-cache-or-import-surface-drift-disables-metaprogramming-host-process-cache-claims";
// lowering-freeze anchor: Part 11 now freezes one explicit emitted
// lowering contract for foreign callable ABI carriage, ownership/error bridge
// interaction counts, Swift-facing concurrency metadata, and interface/import
// preservation facts across FFI boundaries. Live bridge helper emission and
// runnable foreign-call execution remain later lane-C and lane-D work.
inline constexpr const char *kObjc3InteropInteropLoweringContractId =
    "objc3c.interop.interop.lowering.and.abi.contract.v1";
inline constexpr const char *kObjc3InteropInteropLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_interop_interop_lowering_and_abi_contract";
inline constexpr const char *kObjc3InteropInteropLoweringModel =
    "interop-foreign-callable-sema-runtime-parity-cpp-interaction-swift-isolation-and-interface-preservation-packets-now-feed-one-deterministic-lowering-contract-for-manifest-and-ir-carriage";
inline constexpr const char *kObjc3InteropInteropLoweringDeferredModel =
    "live-ffi-call-lowering-ownership-bridge-helper-emission-error-runtime-integration-and-cross-module-runtime-consumption-remain-later-interop-runtime-work";
inline constexpr const char *kObjc3InteropInteropLoweringLaneContract =
    "objc3c.interop.interop.lowering.abi.contract.v1";
// lowering-implementation anchor: lane-C now materializes a second
// Part 11 packet over the actual callable-lowering slice, proving that foreign,
// C++-annotated, and Swift-facing call boundaries emit deterministic ownership
// and lifetime bridge operations in IR rather than stopping at the frozen C001
// contract surface.
inline constexpr const char *kObjc3InteropForeignCallLifetimeLoweringContractId =
    "objc3c.interop.foreign.call.and.lifetime.lowering.v1";
inline constexpr const char *kObjc3InteropForeignCallLifetimeLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_interop_foreign_call_and_lifetime_lowering";
inline constexpr const char *kObjc3InteropForeignCallLifetimeLoweringModel =
    "foreign-calls-and-cpp-swift-facing-free-functions-now-lower-through-one-deterministic-interop-call-boundary-that-preserves-ownership-lifetime-and-annotation-facts-in-manifest-and-ir";
inline constexpr const char
    *kObjc3InteropForeignCallLifetimeLoweringDeferredModel =
        "cross-module-runtime-consumption-live-foreign-linking-and-runnable-host-language-integration-remain-later-interop-closeout-work";
inline constexpr const char
    *kObjc3InteropForeignCallLifetimeLoweringDependencyContractId =
        kObjc3InteropInteropLoweringContractId;
// preservation-expansion anchor: lane-C now publishes one explicit
// replay/import packet proving Part 11 callable and annotation metadata survive
// separate compilation through the runtime-import-surface artifact and emitted
// IR payloads instead of remaining local-only facts from A003/C002.
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationContractId =
        "objc3c.interop.ffi.metadata.interface.preservation.v1";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_interop_ffi_metadata_and_interface_preservation";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationImportArtifactMemberName =
        "objc_interop_ffi_metadata_and_interface_preservation";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationSourceContractId =
        kObjc3InteropForeignCallLifetimeLoweringContractId;
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationPreservationModel =
        "provider-runtime-import-surfaces-now-preserve-interop-callable-and-annotation-metadata-beyond-local-manifest-ir-and-object-emission";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationSourceModel =
        "local-lane-c-interop-lowering-and-lane-a-preservation-packets-feed-one-runtime-import-surface-summary-for-separate-compilation-replay";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationFailClosedModel =
        "missing-import-surface-packet-drifted-replay-keys-or-non-deterministic-provider-preservation-disables-cross-module-interop-preservation-claims";
// packaging/toolchain freeze anchor: lane-D now freezes the truthful
// Part 11 bridge-packaging/toolchain boundary around the existing packaged
// runtime archive, registration-manifest and cross-module link-plan topology,
// and operator-visible interop evidence artifacts while header/module/bridge
// generation remain explicitly deferred to the next runtime step.
inline constexpr const char
    *kObjc3InteropBridgePackagingToolchainContractId =
        "objc3c.interop.bridge.packaging.and.toolchain.contract.v1";
inline constexpr const char
    *kObjc3InteropBridgePackagingToolchainSourceContractId =
        kObjc3InteropFfiMetadataInterfacePreservationContractId;
inline constexpr const char
    *kObjc3InteropBridgePackagingToolchainPreservationContractId =
        "objc3c.interop.foreign.surface.interface.preservation.v1";
inline constexpr const char
    *kObjc3InteropBridgePackagingToolchainPackagingModel =
        "runtime-registration-manifests-runtime-import-surfaces-cross-module-link-plans-and-linker-response-sidecars-are-the-current-toolchain-visible-interop-packaging-topology";
inline constexpr const char
    *kObjc3InteropBridgePackagingToolchainEvidenceModel =
        "operator-visible-interop-evidence-is-published-through-the-packaged-runtime-archive-registration-manifest-cross-module-link-plan-and-ir-summary";
inline constexpr const char
    *kObjc3InteropBridgePackagingToolchainFailClosedModel =
        "header-module-and-bridge-generation-remain-unclaimed-until-next-runtime-phase";
inline constexpr const char
    *kObjc3InteropHeaderModuleBridgeGenerationLoweringContractId =
        "objc3c.interop.header.module.and.bridge.generation.v1";
inline constexpr const char
    *kObjc3InteropHeaderModuleBridgeGenerationLoweringSourceContractId =
        "objc3c.interop.bridge.packaging.and.toolchain.contract.v1";
inline constexpr const char
    *kObjc3InteropHeaderModuleBridgeGenerationLoweringPreservationContractId =
        "objc3c.interop.ffi.metadata.interface.preservation.v1";
inline constexpr const char
    *kObjc3InteropHeaderModuleBridgeGenerationLoweringPackagingModel =
        "compiler-emits-deterministic-interop-bridge-header-modulemap-and-bridge-json-artifacts-and-preserves-them-through-runtime-import-surfaces-and-cross-module-link-plans";
inline constexpr const char
    *kObjc3InteropHeaderModuleBridgeGenerationLoweringFailClosedModel =
        "missing-generated-bridge-artifacts-or-drifted-import-surface-paths-disable-live-interop-bridge-generation-claims";
// preservation anchor: lane-C now preserves the direct/final/sealed
// intent introduced by C002 through runtime metadata source records, emitted
// runtime-import-surface artifacts, and replay-stable frontend metadata instead
// of restricting those facts to local IR/object payloads only.
inline constexpr const char
    *kObjc3DispatchDispatchMetadataInterfacePreservationContractId =
        "objc3c.dispatch.dispatch.metadata.interface.preservation.v1";
inline constexpr const char
    *kObjc3DispatchDispatchMetadataInterfacePreservationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_dispatch_dispatch_metadata_and_interface_preservation";
inline constexpr const char
    *kObjc3DispatchDispatchMetadataInterfacePreservationImportArtifactMemberName =
        "objc_dispatch_dispatch_metadata_and_interface_preservation";
inline constexpr const char
    *kObjc3DispatchDispatchMetadataInterfacePreservationSourceModel =
        "runtime-metadata-source-records-and-runtime-import-surface-artifacts-preserve-direct-final-sealed-intent-for-separate-compilation-and-interface-replay";
inline constexpr const char
    *kObjc3DispatchDispatchMetadataInterfacePreservationModel =
        "provider-and-consumer-runtime-import-surface-artifacts-preserve-direct-final-sealed-dispatch-intent-beyond-local-ir-object-emission";
inline constexpr const char
    *kObjc3DispatchDispatchMetadataInterfacePreservationFailClosedModel =
        "missing-or-drifted-dispatch-intent-preservation-packets-disable-cross-module-dispatch-preservation-claims";
// ABI/artifact completion anchor: keep the frozen Part 8 lowering
// contract from C001 as the single lowering boundary, but publish one
// dedicated ABI/replay packet above it for borrowed-return contracts and
// retainable-family callable inventories so lane-D runtime work can consume a
// stable artifact surface instead of re-deriving those call-boundary facts.
inline constexpr const char *kObjc3OwnershipBorrowedRetainableAbiCompletionContractId =
    "objc3c.ownership.borrowed.retainable.family.abi.completion.v1";
inline constexpr const char *kObjc3OwnershipBorrowedRetainableAbiCompletionSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_ownership_borrowed_pointer_and_retainable_family_abi_completion";
inline constexpr const char *kObjc3OwnershipBorrowedRetainableAbiCompletionLaneContract =
    "objc3c.ownership.borrowed.retainable.family.abi.completion.v1";
// runtime/helper-freeze anchor: lane-D does not invent a second
// Part 8 runtime subsystem. Cleanup execution, resource invalidation, and
// retainable-family helper integration freeze the already-live private ARC /
// autorelease / snapshot helper slice plus the existing packaged runtime
// archive path. Borrowed lifetime runtime enforcement and escaping
// cleanup/resource ownership transfer remain later work.
inline constexpr const char *kObjc3OwnershipSystemHelperRuntimeContractId =
    "objc3c.ownership.system.helper.runtime.contract.v1";
inline constexpr const char *kObjc3OwnershipSystemHelperRuntimeSourceModel =
    "cleanup-resource-invalidation-and-retainable-family-runtime-proof-reuses-existing-private-arc-autorelease-and-snapshot-helpers";
inline constexpr const char *kObjc3OwnershipSystemHelperRuntimeAbiModel =
    "private-retain-release-autorelease-autoreleasepool-and-testing-snapshot-helper-cluster";
inline constexpr const char *kObjc3OwnershipSystemHelperRuntimePackagingModel =
    "same-packaged-runtime-archive-no-public-runtime-header-widening-and-no-new-ownership-import-surface";
inline constexpr const char *kObjc3OwnershipSystemHelperRuntimeFailClosedModel =
    "borrowed-lifetime-runtime-enforcement-and-escaping-resource-ownership-transfer-remain-deferred";
// live runtime-integration anchor: the supported Part 8 cleanup /
// retainable-family slice now proves actual linked execution through the same
// private helper cluster frozen in D001 plus emitted scope-exit cleanup calls.
inline constexpr const char *kObjc3OwnershipLiveCleanupRetainableIntegrationContractId =
    "objc3c.ownership.live.cleanup.retainable.runtime.integration.v1";
inline constexpr const char *kObjc3OwnershipLiveCleanupRetainableIntegrationSourceModel =
    "supported-ownership-cleanup-resource-and-retainable-family-sites-now-link-and-execute-through-emitted-cleanup-calls-and-the-private-helper-cluster";
inline constexpr const char *kObjc3OwnershipLiveCleanupRetainableIntegrationExecutionModel =
    "linked-native-probes-execute-lifo-cleanup-resource-invalidation-and-retainable-family-helper-traffic-on-the-supported-slice";
inline constexpr const char *kObjc3OwnershipLiveCleanupRetainableIntegrationPackagingModel =
    "linked-module-object-plus-existing-runtime-support-archive-no-new-runtime-package-surface";
inline constexpr const char *kObjc3OwnershipLiveCleanupRetainableIntegrationFailClosedModel =
    "borrowed-lifetime-runtime-enforcement-and-escaping-resource-ownership-transfer-remain-deferred";
inline constexpr const char *kObjc3TaskRuntimeInteropCancellationLoweringLaneContract =
    "objc3c.task.runtime.interop.cancellation.lowering.v1";
inline constexpr const char *kObjc3ConcurrencyReplayRaceGuardLoweringLaneContract =
    "objc3c.concurrency.replay.race.guard.lowering.v1";
// lowering-freeze anchor: these existing Part 7 concurrency lane
// contracts now become the explicit task-runtime lowering handoff for task
// creation, executor hops, cancellation polling, and task-group artifacts.
// Later lane-C work must widen the same contract family with native
// spawn/hop/cancel entrypoints and task-group ABI completion rather than
// inventing a second lowering boundary.
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeLoweringContractId =
    "objc3c.concurrency.task.runtime.lowering.contract.v1";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeLoweringSurfacePath =
    "frontend.pipeline.semantic_surface.objc_concurrency_task_runtime_lowering_contract";
// ABI/artifact completion anchor: the supported Part 7 helper-backed
// lowering slice now also publishes a dedicated runtime-ABI packet and IR
// boundary so later lane-D runtime freezes consume a stable artifact surface
// instead of rediscovering helper names ad hoc.
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeAbiCompletionContractId =
    "objc3c.concurrency.task.runtime.abi.completion.v1";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeAbiCompletionSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_concurrency_task_group_and_runtime_abi_completion";
inline constexpr const char *kObjc3UnsafePointerExtensionLoweringLaneContract =
    "objc3c.unsafe.pointer.extension.gating.lowering.v1";
inline constexpr const char *kObjc3InlineAsmIntrinsicGovernanceLoweringLaneContract =
    "objc3c.inline.asm.intrinsic.governance.lowering.v1";

enum class Objc3AtomicMemoryOrder : std::uint8_t {
  Relaxed = 0,
  Acquire = 1,
  Release = 2,
  AcqRel = 3,
  SeqCst = 4,
};

struct Objc3LoweringContract {
  std::size_t max_message_send_args = kObjc3RuntimeDispatchDefaultArgs;
  std::string runtime_dispatch_symbol = kObjc3RuntimeDispatchSymbol;
};

struct Objc3LoweringIRBoundary {
  std::size_t runtime_dispatch_arg_slots = kObjc3RuntimeDispatchDefaultArgs;
  std::string runtime_dispatch_symbol = kObjc3RuntimeDispatchSymbol;
  std::string selector_global_ordering = kObjc3SelectorGlobalOrdering;
};

struct Objc3RuntimeMetadataLayoutPolicyFamilyInput {
  std::string kind;
  std::string section_name;
  std::string aggregate_symbol_name;
  std::size_t descriptor_count = 0;
};

struct Objc3RuntimeMetadataLayoutPolicyInput {
  std::string abi_contract_id;
  std::string scaffold_contract_id;
  bool section_boundary_ready = false;
  bool runtime_export_ready = false;
  bool scaffold_emitted = false;
  bool scaffold_fail_closed = false;
  bool uses_llvm_used = false;
  bool image_info_emitted = false;
  std::string image_info_symbol;
  std::string image_info_section;
  std::string descriptor_symbol_prefix;
  std::string descriptor_linkage;
  std::string aggregate_linkage;
  std::string metadata_visibility;
  std::string retention_root;
  std::size_t total_retained_global_count = 0;
  std::array<Objc3RuntimeMetadataLayoutPolicyFamilyInput,
             kObjc3RuntimeMetadataLayoutPolicyFamilyCount>
      families;
};

struct Objc3RuntimeMetadataLayoutPolicyFamily {
  std::string kind;
  std::string logical_section_name;
  std::string emitted_section_name;
  std::string aggregate_symbol_name;
  std::size_t descriptor_count = 0;
};

struct Objc3RuntimeMetadataLayoutPolicy {
  std::string contract_id = kObjc3RuntimeMetadataLayoutPolicyContractId;
  std::string abi_contract_id;
  std::string scaffold_contract_id;
  std::string family_ordering_model =
      kObjc3RuntimeMetadataLayoutFamilyOrderingModel;
  std::string descriptor_ordering_model =
      kObjc3RuntimeMetadataDescriptorOrderingModel;
  std::string aggregate_relocation_policy =
      kObjc3RuntimeMetadataAggregateRelocationPolicy;
  std::string comdat_policy = kObjc3RuntimeMetadataComdatPolicy;
  std::string visibility_spelling_policy =
      kObjc3RuntimeMetadataVisibilitySpellingPolicy;
  std::string retention_ordering_model =
      kObjc3RuntimeMetadataRetentionOrderingModel;
  std::string object_format_policy_model =
      kObjc3RuntimeMetadataObjectFormatPolicyModel;
  std::string object_format_surface_contract_id =
      kObjc3RuntimeMetadataObjectFormatSurfaceContractId;
  std::string object_format;
  std::string section_spelling_model;
  std::string retention_anchor_model;
  std::string image_info_symbol;
  std::string logical_image_info_section;
  std::string emitted_image_info_section;
  std::string descriptor_symbol_prefix;
  std::string descriptor_linkage;
  std::string aggregate_linkage;
  std::string metadata_visibility;
  std::string retention_root;
  std::size_t total_retained_global_count = 0;
  bool ready = false;
  bool fail_closed = false;
  std::array<Objc3RuntimeMetadataLayoutPolicyFamily,
             kObjc3RuntimeMetadataLayoutPolicyFamilyCount>
      families;
  std::string failure_reason;
};

struct Objc3MethodLookupOverrideConflictContract {
  std::size_t method_lookup_sites = 0;
  std::size_t method_lookup_hits = 0;
  std::size_t method_lookup_misses = 0;
  std::size_t override_lookup_sites = 0;
  std::size_t override_lookup_hits = 0;
  std::size_t override_lookup_misses = 0;
  std::size_t override_conflicts = 0;
  std::size_t unresolved_base_interfaces = 0;
  bool deterministic = true;
};

struct Objc3PropertySynthesisIvarBindingContract {
  std::size_t property_synthesis_sites = 0;
  std::size_t property_synthesis_explicit_ivar_bindings = 0;
  std::size_t property_synthesis_default_ivar_bindings = 0;
  std::size_t interface_owned_property_synthesis_sites = 0;
  std::size_t implementation_property_redeclaration_sites = 0;
  std::size_t ivar_binding_sites = 0;
  std::size_t ivar_binding_resolved = 0;
  std::size_t ivar_binding_missing = 0;
  std::size_t ivar_binding_conflicts = 0;
  bool deterministic = true;
};

struct Objc3IdClassSelObjectPointerTypecheckContract {
  std::size_t id_typecheck_sites = 0;
  std::size_t class_typecheck_sites = 0;
  std::size_t sel_typecheck_sites = 0;
  std::size_t object_pointer_typecheck_sites = 0;
  std::size_t total_typecheck_sites = 0;
  bool deterministic = true;
};

struct Objc3DispatchSurfaceClassificationContract {
  std::size_t instance_dispatch_sites = 0;
  std::size_t class_dispatch_sites = 0;
  std::size_t super_dispatch_sites = 0;
  std::size_t direct_dispatch_sites = 0;
  std::size_t dynamic_dispatch_sites = 0;
  std::string instance_entrypoint_family = kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily;
  std::string class_entrypoint_family = kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily;
  std::string super_entrypoint_family = kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily;
  std::string direct_entrypoint_family = kObjc3DispatchSurfaceDirectDispatchBinding;
  std::string dynamic_entrypoint_family = kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily;
  bool deterministic = true;
};

struct Objc3MessageSendSelectorLoweringContract {
  std::size_t message_send_sites = 0;
  std::size_t unary_selector_sites = 0;
  std::size_t keyword_selector_sites = 0;
  std::size_t selector_piece_sites = 0;
  std::size_t argument_expression_sites = 0;
  std::size_t receiver_expression_sites = 0;
  std::size_t selector_literal_entries = 0;
  std::size_t selector_literal_characters = 0;
  bool deterministic = true;
};

struct Objc3DispatchAbiMarshallingContract {
  std::size_t message_send_sites = 0;
  std::size_t receiver_slots_marshaled = 0;
  std::size_t selector_slots_marshaled = 0;
  std::size_t argument_value_slots_marshaled = 0;
  std::size_t argument_padding_slots_marshaled = 0;
  std::size_t argument_total_slots_marshaled = 0;
  std::size_t total_marshaled_slots = 0;
  std::size_t runtime_dispatch_arg_slots = kObjc3RuntimeDispatchDefaultArgs;
  bool deterministic = true;
};

struct Objc3NilReceiverSemanticsFoldabilityContract {
  std::size_t message_send_sites = 0;
  std::size_t receiver_nil_literal_sites = 0;
  std::size_t nil_receiver_semantics_enabled_sites = 0;
  std::size_t nil_receiver_foldable_sites = 0;
  std::size_t nil_receiver_runtime_dispatch_required_sites = 0;
  std::size_t non_nil_receiver_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3SuperDispatchMethodFamilyContract {
  std::size_t message_send_sites = 0;
  std::size_t receiver_super_identifier_sites = 0;
  std::size_t super_dispatch_enabled_sites = 0;
  std::size_t super_dispatch_requires_class_context_sites = 0;
  std::size_t method_family_init_sites = 0;
  std::size_t method_family_copy_sites = 0;
  std::size_t method_family_mutable_copy_sites = 0;
  std::size_t method_family_new_sites = 0;
  std::size_t method_family_none_sites = 0;
  std::size_t method_family_returns_retained_result_sites = 0;
  std::size_t method_family_returns_related_result_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3RuntimeLinkHostLinkContract {
  std::size_t message_send_sites = 0;
  std::size_t runtime_link_required_sites = 0;
  std::size_t runtime_link_elided_sites = 0;
  std::size_t runtime_dispatch_arg_slots = kObjc3RuntimeDispatchDefaultArgs;
  std::size_t runtime_dispatch_declaration_parameter_count = 0;
  std::size_t contract_violation_sites = 0;
  std::string runtime_dispatch_symbol = kObjc3RuntimeDispatchSymbol;
  bool default_runtime_dispatch_symbol_binding = true;
  bool deterministic = true;
};

struct Objc3RuntimeDispatchLoweringAbiContract {
  std::size_t message_send_sites = 0;
  std::size_t fixed_argument_slot_count = kObjc3RuntimeDispatchDefaultArgs;
  std::size_t runtime_dispatch_parameter_count = 0;
  std::string lowering_boundary_model =
      kObjc3RuntimeDispatchLoweringAbiBoundaryModel;
  std::string canonical_runtime_dispatch_symbol =
      kObjc3RuntimeDispatchLoweringCanonicalEntrypointSymbol;
  std::string compatibility_runtime_dispatch_symbol =
      kObjc3RuntimeDispatchLoweringCompatibilityEntrypointSymbol;
  std::string default_lowering_target_symbol = kObjc3RuntimeDispatchSymbol;
  std::string selector_lookup_symbol =
      kObjc3RuntimeDispatchLoweringSelectorLookupSymbol;
  std::string selector_handle_type =
      kObjc3RuntimeDispatchLoweringSelectorHandleType;
  std::string receiver_abi_type = kObjc3RuntimeDispatchLoweringReceiverAbiType;
  std::string selector_abi_type = kObjc3RuntimeDispatchLoweringSelectorAbiType;
  std::string argument_abi_type = kObjc3RuntimeDispatchLoweringArgumentAbiType;
  std::string result_abi_type = kObjc3RuntimeDispatchLoweringResultAbiType;
  std::string selector_operand_model =
      kObjc3RuntimeDispatchLoweringSelectorOperandModel;
  std::string selector_handle_model =
      kObjc3RuntimeDispatchLoweringSelectorHandleModel;
  std::string argument_padding_model =
      kObjc3RuntimeDispatchLoweringArgumentPaddingModel;
  std::string default_lowering_target_model =
      kObjc3RuntimeDispatchLoweringDefaultTargetModel;
  std::string compatibility_bridge_role_model =
      kObjc3RuntimeDispatchLoweringCompatibilityRoleModel;
  std::string deferred_cases_model =
      kObjc3RuntimeDispatchLoweringDeferredCasesModel;
  bool fail_closed = true;
  bool deterministic = true;
};

struct Objc3OwnershipQualifierLoweringContract {
  std::size_t ownership_qualifier_sites = 0;
  std::size_t invalid_ownership_qualifier_sites = 0;
  std::size_t object_pointer_type_annotation_sites = 0;
  bool deterministic = true;
};

struct Objc3RetainReleaseOperationLoweringContract {
  std::size_t ownership_qualified_sites = 0;
  std::size_t retain_insertion_sites = 0;
  std::size_t release_insertion_sites = 0;
  std::size_t autorelease_insertion_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3AutoreleasePoolScopeLoweringContract {
  std::size_t scope_sites = 0;
  std::size_t scope_symbolized_sites = 0;
  unsigned max_scope_depth = 0;
  std::size_t scope_entry_transition_sites = 0;
  std::size_t scope_exit_transition_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3WeakUnownedSemanticsLoweringContract {
  std::size_t ownership_candidate_sites = 0;
  std::size_t weak_reference_sites = 0;
  std::size_t unowned_reference_sites = 0;
  std::size_t unowned_safe_reference_sites = 0;
  std::size_t weak_unowned_conflict_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ArcDiagnosticsFixitLoweringContract {
  std::size_t ownership_arc_diagnostic_candidate_sites = 0;
  std::size_t ownership_arc_fixit_available_sites = 0;
  std::size_t ownership_arc_profiled_sites = 0;
  std::size_t ownership_arc_weak_unowned_conflict_diagnostic_sites = 0;
  std::size_t ownership_arc_empty_fixit_hint_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockLiteralCaptureLoweringContract {
  std::size_t block_literal_sites = 0;
  std::size_t block_parameter_entries = 0;
  std::size_t block_capture_entries = 0;
  std::size_t block_body_statement_entries = 0;
  std::size_t block_empty_capture_sites = 0;
  std::size_t block_nondeterministic_capture_sites = 0;
  std::size_t block_non_normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockSourceModelCompletionContract {
  std::size_t block_literal_sites = 0;
  std::size_t signature_entries_total = 0;
  std::size_t explicit_typed_parameter_entries_total = 0;
  std::size_t implicit_parameter_entries_total = 0;
  std::size_t capture_inventory_entries_total = 0;
  std::size_t byvalue_readonly_capture_entries_total = 0;
  std::size_t invoke_surface_entries_total = 0;
  std::size_t non_normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockSourceStorageAnnotationContract {
  std::size_t block_literal_sites = 0;
  std::size_t capture_entries_total = 0;
  std::size_t mutated_capture_entries_total = 0;
  std::size_t byref_capture_entries_total = 0;
  std::size_t copy_helper_intent_sites = 0;
  std::size_t dispose_helper_intent_sites = 0;
  std::size_t heap_candidate_sites = 0;
  std::size_t expression_sites = 0;
  std::size_t global_initializer_sites = 0;
  std::size_t binding_initializer_sites = 0;
  std::size_t assignment_value_sites = 0;
  std::size_t return_value_sites = 0;
  std::size_t call_argument_sites = 0;
  std::size_t message_argument_sites = 0;
  std::size_t non_normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockAbiInvokeTrampolineLoweringContract {
  std::size_t block_literal_sites = 0;
  std::size_t invoke_argument_slots_total = 0;
  std::size_t capture_word_count_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t descriptor_symbolized_sites = 0;
  std::size_t invoke_trampoline_symbolized_sites = 0;
  std::size_t missing_invoke_trampoline_sites = 0;
  std::size_t non_normalized_layout_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockStorageEscapeLoweringContract {
  std::size_t block_literal_sites = 0;
  std::size_t mutable_capture_count_total = 0;
  std::size_t byref_slot_count_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t requires_byref_cells_sites = 0;
  std::size_t escape_analysis_enabled_sites = 0;
  std::size_t escape_to_heap_sites = 0;
  std::size_t escape_profile_normalized_sites = 0;
  std::size_t byref_layout_symbolized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockCopyDisposeLoweringContract {
  std::size_t block_literal_sites = 0;
  std::size_t mutable_capture_count_total = 0;
  std::size_t byref_slot_count_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t copy_helper_required_sites = 0;
  std::size_t dispose_helper_required_sites = 0;
  std::size_t profile_normalized_sites = 0;
  std::size_t copy_helper_symbolized_sites = 0;
  std::size_t dispose_helper_symbolized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockDeterminismPerfBaselineLoweringContract {
  std::size_t block_literal_sites = 0;
  std::size_t baseline_weight_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t deterministic_capture_sites = 0;
  std::size_t heavy_tier_sites = 0;
  std::size_t normalized_profile_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3TypeSystemOptionalKeypathLoweringContract {
  std::size_t optional_binding_sites = 0;
  std::size_t optional_binding_clause_sites = 0;
  std::size_t optional_send_sites = 0;
  std::size_t nil_coalescing_sites = 0;
  std::size_t typed_keypath_literal_sites = 0;
  std::size_t typed_keypath_self_root_sites = 0;
  std::size_t typed_keypath_class_root_sites = 0;
  std::size_t live_optional_lowering_sites = 0;
  std::size_t single_evaluation_nil_short_circuit_sites = 0;
  std::size_t live_typed_keypath_artifact_sites = 0;
  std::size_t deferred_typed_keypath_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ControlFlowControlFlowSafetyLoweringContract {
  std::size_t guard_statement_sites = 0;
  std::size_t guard_clause_sites = 0;
  std::size_t match_statement_sites = 0;
  std::size_t defer_statement_sites = 0;
  std::size_t live_guard_short_circuit_sites = 0;
  std::size_t live_match_dispatch_sites = 0;
  std::size_t live_defer_cleanup_sites = 0;
  std::size_t fail_closed_guard_short_circuit_sites = 0;
  std::size_t fail_closed_match_dispatch_sites = 0;
  std::size_t fail_closed_defer_cleanup_sites = 0;
  std::size_t deterministic_fail_closed_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3LightweightGenericsConstraintLoweringContract {
  std::size_t generic_constraint_sites = 0;
  std::size_t generic_suffix_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t terminated_generic_suffix_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_constraint_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3NullabilityFlowWarningPrecisionLoweringContract {
  std::size_t nullability_flow_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t nullability_suffix_sites = 0;
  std::size_t nullable_suffix_sites = 0;
  std::size_t nonnull_suffix_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ProtocolQualifiedObjectTypeLoweringContract {
  std::size_t protocol_qualified_object_type_sites = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t terminated_protocol_composition_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_protocol_composition_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3VarianceBridgeCastLoweringContract {
  std::size_t variance_bridge_cast_sites = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t ownership_qualifier_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3GenericMetadataAbiLoweringContract {
  std::size_t generic_metadata_abi_sites = 0;
  std::size_t generic_suffix_sites = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t ownership_qualifier_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ModuleImportGraphLoweringContract {
  std::size_t module_import_graph_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3NamespaceCollisionShadowingLoweringContract {
  std::size_t namespace_collision_shadowing_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3PublicPrivateApiPartitionLoweringContract {
  std::size_t public_private_api_partition_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3IncrementalModuleCacheInvalidationLoweringContract {
  std::size_t incremental_module_cache_invalidation_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t cache_invalidation_candidate_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3CrossModuleConformanceLoweringContract {
  std::size_t cross_module_conformance_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t cache_invalidation_candidate_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ThrowsPropagationLoweringContract {
  std::size_t throws_propagation_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t cache_invalidation_candidate_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ResultLikeLoweringContract {
  std::size_t result_like_sites = 0;
  std::size_t result_success_sites = 0;
  std::size_t result_failure_sites = 0;
  std::size_t result_branch_sites = 0;
  std::size_t result_payload_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t branch_merge_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3NSErrorBridgingLoweringContract {
  std::size_t ns_error_bridging_sites = 0;
  std::size_t ns_error_parameter_sites = 0;
  std::size_t ns_error_out_parameter_sites = 0;
  std::size_t ns_error_bridge_path_sites = 0;
  std::size_t failable_call_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t bridge_boundary_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3UnwindCleanupLoweringContract {
  std::size_t unwind_cleanup_sites = 0;
  std::size_t unwind_edge_sites = 0;
  std::size_t cleanup_scope_sites = 0;
  std::size_t cleanup_emit_sites = 0;
  std::size_t landing_pad_sites = 0;
  std::size_t cleanup_resume_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ErrorDiagnosticsRecoveryLoweringContract {
  std::size_t error_diagnostic_sites = 0;
  std::size_t parser_diagnostic_sites = 0;
  std::size_t semantic_diagnostic_sites = 0;
  std::size_t fixit_hint_sites = 0;
  std::size_t recovery_candidate_sites = 0;
  std::size_t recovery_applied_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3AsyncContinuationLoweringContract {
  std::size_t async_continuation_sites = 0;
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t continuation_allocation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3AwaitLoweringSuspensionStateLoweringContract {
  std::size_t await_suspension_sites = 0;
  std::size_t await_keyword_sites = 0;
  std::size_t await_suspension_point_sites = 0;
  std::size_t await_resume_sites = 0;
  std::size_t await_state_machine_sites = 0;
  std::size_t await_continuation_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ActorIsolationSendabilityLoweringContract {
  std::size_t actor_isolation_sites = 0;
  std::size_t sendability_check_sites = 0;
  std::size_t cross_actor_hop_sites = 0;
  std::size_t non_sendable_capture_sites = 0;
  std::size_t sendable_transfer_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ActorLoweringMetadataContract {
  std::size_t actor_interface_sites = 0;
  std::size_t actor_method_sites = 0;
  std::size_t actor_metadata_record_sites = 0;
  std::size_t nonisolated_entry_sites = 0;
  std::size_t executor_affinity_sites = 0;
  std::size_t actor_hop_artifact_sites = 0;
  std::size_t actor_isolation_thunk_sites = 0;
  std::size_t replay_proof_dependency_sites = 0;
  std::size_t race_guard_dependency_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3OwnershipSystemExtensionLoweringContract {
  std::size_t cleanup_hook_sites = 0;
  std::size_t resource_local_sites = 0;
  std::size_t cleanup_owned_local_sites = 0;
  std::size_t resource_move_capture_sites = 0;
  std::size_t borrowed_parameter_sites = 0;
  std::size_t borrowed_return_callable_sites = 0;
  std::size_t borrowed_escape_candidate_sites = 0;
  std::size_t explicit_capture_item_sites = 0;
  std::size_t retainable_family_callable_sites = 0;
  std::size_t retainable_family_operation_callable_sites = 0;
  std::size_t retainable_family_alias_callable_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3DispatchDispatchControlLoweringContract {
  std::size_t direct_call_candidate_sites = 0;
  std::size_t direct_members_defaulted_sites = 0;
  std::size_t dynamic_opt_out_sites = 0;
  std::size_t final_container_sites = 0;
  std::size_t sealed_container_sites = 0;
  std::size_t override_legality_sites = 0;
  std::size_t metadata_preserved_callable_sites = 0;
  std::size_t metadata_preserved_container_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3MetaprogrammingExpansionLoweringContract {
  std::size_t derive_inventory_sites = 0;
  std::size_t derived_selector_artifact_sites = 0;
  std::size_t macro_replay_visible_sites = 0;
  std::size_t property_behavior_sites = 0;
  std::size_t synthesized_binding_sites = 0;
  std::size_t synthesized_getter_sites = 0;
  std::size_t synthesized_setter_sites = 0;
  std::size_t replay_visible_metadata_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3InteropInteropLoweringContract {
  std::size_t foreign_callable_sites = 0;
  std::size_t c_foreign_callable_sites = 0;
  std::size_t objc_runtime_parity_callable_sites = 0;
  std::size_t ownership_bridge_callable_sites = 0;
  std::size_t error_surface_sites = 0;
  std::size_t async_boundary_sites = 0;
  std::size_t swift_concurrency_metadata_sites = 0;
  std::size_t interface_preserved_foreign_callable_sites = 0;
  std::size_t interface_preserved_metadata_annotation_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3InteropForeignCallLifetimeLoweringContract {
  std::size_t foreign_callable_sites = 0;
  std::size_t c_foreign_callable_sites = 0;
  std::size_t objc_runtime_parity_callable_sites = 0;
  std::size_t ownership_bridge_sites = 0;
  std::size_t lifetime_bridge_sites = 0;
  std::size_t metadata_preservation_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3InteropFfiMetadataInterfacePreservationContract {
  std::size_t local_foreign_callable_count = 0;
  std::size_t local_metadata_preservation_sites = 0;
  std::size_t local_interface_annotation_sites = 0;
  std::size_t imported_module_count = 0;
  std::size_t imported_foreign_callable_count = 0;
  std::size_t imported_metadata_preservation_sites = 0;
  std::size_t imported_interface_annotation_sites = 0;
  bool runtime_import_artifact_ready = false;
  bool separate_compilation_preservation_ready = false;
  bool deterministic = false;
};

struct Objc3InteropBridgePackagingToolchainContract {
  bool packaging_topology_ready = false;
  bool operator_visible_evidence_ready = false;
  bool header_generation_ready = false;
  bool module_generation_ready = false;
  bool bridge_generation_ready = false;
  bool deterministic = false;
};

struct Objc3MetaprogrammingSynthesizedArtifactEmissionContract {
  std::size_t derive_inventory_sites = 0;
  std::size_t emitted_derive_method_sites = 0;
  std::size_t emitted_macro_artifact_sites = 0;
  std::size_t emitted_property_behavior_artifact_sites = 0;
  std::size_t emitted_global_artifact_sites = 0;
  std::size_t emitted_runtime_method_list_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3TaskRuntimeInteropCancellationLoweringContract {
  std::size_t task_runtime_sites = 0;
  std::size_t task_runtime_interop_sites = 0;
  std::size_t cancellation_probe_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t runtime_resume_sites = 0;
  std::size_t runtime_cancel_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ConcurrencyReplayRaceGuardLoweringContract {
  std::size_t concurrency_replay_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t deterministic_schedule_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3UnsafePointerExtensionLoweringContract {
  std::size_t unsafe_pointer_extension_sites = 0;
  std::size_t unsafe_keyword_sites = 0;
  std::size_t pointer_arithmetic_sites = 0;
  std::size_t raw_pointer_type_sites = 0;
  std::size_t unsafe_operation_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3InlineAsmIntrinsicGovernanceLoweringContract {
  std::size_t inline_asm_intrinsic_sites = 0;
  std::size_t inline_asm_sites = 0;
  std::size_t intrinsic_sites = 0;
  std::size_t governed_intrinsic_sites = 0;
  std::size_t privileged_intrinsic_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

bool IsValidRuntimeDispatchSymbol(const std::string &symbol);
bool TryNormalizeObjc3LoweringContract(const Objc3LoweringContract &input,
                                       Objc3LoweringContract &normalized,
                                       std::string &error);
bool TryBuildObjc3LoweringIRBoundary(const Objc3LoweringContract &input,
                                     Objc3LoweringIRBoundary &boundary,
                                     std::string &error);
std::string Objc3LoweringIRBoundaryReplayKey(const Objc3LoweringIRBoundary &boundary);
bool UsesCanonicalObjc3RuntimeDispatchEntrypoint(
    const std::string &dispatch_surface_family);
bool RequiresFailClosedObjc3RuntimeDispatchError(
    const std::string &dispatch_surface_family);
const char *Objc3DispatchSurfaceRuntimeEntrypointSymbol(
    const std::string &dispatch_surface_family);
std::string Objc3RuntimeDispatchDeclarationReplayKey(const Objc3LoweringIRBoundary &boundary);
bool TryBuildObjc3RuntimeMetadataLayoutPolicy(
    const Objc3RuntimeMetadataLayoutPolicyInput &input,
    Objc3RuntimeMetadataLayoutPolicy &policy, std::string &error);
bool IsReadyObjc3RuntimeMetadataLayoutPolicy(
    const Objc3RuntimeMetadataLayoutPolicy &policy);
std::string Objc3RuntimeMetadataLayoutPolicyReplayKey(
    const Objc3RuntimeMetadataLayoutPolicy &policy);
std::string Objc3RuntimeMetadataSectionEmissionBoundarySummary();
std::string Objc3RuntimeMetadataClassMetaclassEmissionSummary();
std::string Objc3RuntimeMetadataProtocolCategoryEmissionSummary();
std::string Objc3RuntimeMetadataMemberTableEmissionSummary();
std::string Objc3RuntimeMetadataSelectorStringPoolEmissionSummary();
std::string Objc3ExecutableObjectArtifactLoweringSummary();
std::string Objc3ExecutablePropertyAccessorLayoutLoweringSummary();
std::string Objc3ExecutableIvarLayoutEmissionSummary();
std::string Objc3ExecutableSynthesizedAccessorPropertyLoweringSummary();
std::string Objc3RuntimePropertyLayoutConsumptionSummary();
std::string Objc3RuntimeInstanceAllocationLayoutSupportSummary();
std::string Objc3RuntimePropertyMetadataReflectionSummary();
std::string Objc3RuntimeBackedObjectOwnershipAttributeSurfaceSummary();
std::string Objc3RetainableObjectSemanticRulesFreezeSummary();
std::string Objc3RuntimeBackedStorageOwnershipLegalitySummary();
std::string Objc3RuntimeBackedAutoreleasepoolDestructionOrderSummary();
std::string Objc3OwnershipLoweringBaselineSummary();
std::string Objc3OwnershipRuntimeHookEmissionSummary();
std::string Objc3RuntimeMemoryManagementApiSummary();
std::string Objc3RuntimeMemoryManagementImplementationSummary();
std::string Objc3OwnershipRuntimeGateSummary();
std::string Objc3ExecutableBlockSourceClosureSummary();
std::string Objc3ExecutableBlockSourceModelCompletionSummary();
std::string Objc3ExecutableBlockSourceStorageAnnotationSummary();
std::string Objc3ExecutableBlockRuntimeSemanticRulesSummary();
std::string Objc3ExecutableBlockLoweringAbiArtifactBoundarySummary();
std::string Objc3ExecutableBlockObjectInvokeThunkLoweringSummary();
std::string Objc3ExecutableBlockByrefHelperLoweringSummary();
std::string Objc3ExecutableBlockEscapeRuntimeHookLoweringSummary();
std::string Objc3RuntimeBlockApiObjectLayoutSummary();
std::string Objc3RuntimeBlockAllocationCopyDisposeInvokeSupportSummary();
std::string Objc3RuntimeBlockByrefForwardingHeapPromotionInteropSummary();
std::string Objc3RunnableBlockRuntimeGateSummary();
std::string Objc3RunnableBlockExecutionMatrixSummary();
std::string Objc3RuntimeBackedSemanticsClosureSummary();
// Part 3 lowering freeze anchor: native lowering now
