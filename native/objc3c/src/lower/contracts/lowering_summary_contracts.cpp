#include "lower/objc3_lowering_contract.h"
#include "lower/metadata/lowering_metadata_helpers.h"

#include "ast/objc3_ast.h"
#include "sema/objc3_sema_contract.h"

#include <sstream>
#include <string>

std::string Objc3RuntimeMetadataSectionEmissionBoundarySummary() {
  std::ostringstream out;
  // metadata section emission freeze anchor: lane-C begins from the
  // current real-section owner contract rather than from manifest-only
  // summaries. The boundary is explicit that zero payload bytes are
  // lowering-owned object-file records, not emitter-local placeholders.
  out << "contract=" << kObjc3RuntimeMetadataSectionEmissionContractId
      << ";owner_contract="
      << kObjc3RuntimeMetadataSectionEmissionOwnerContractId
      << ";owner_model=" << kObjc3RuntimeMetadataSectionEmissionOwnerModel
      << ";payload_model=" << kObjc3RuntimeMetadataSectionEmissionPayloadModel
      << ";inventory_model=" << kObjc3RuntimeMetadataSectionEmissionInventoryModel
      << ";image_info_payload_model="
      << kObjc3RuntimeMetadataSectionEmissionImageInfoPayloadModel
      << ";descriptor_payload_model="
      << kObjc3RuntimeMetadataSectionEmissionDescriptorPayloadModel
      << ";aggregate_payload_model="
      << kObjc3RuntimeMetadataSectionEmissionAggregatePayloadModel
      << ";non_goals=no-method-selector-string-pool-payloads";
  return out.str();
}

std::string Objc3RuntimeMetadataClassMetaclassEmissionSummary() {
  std::ostringstream out;
  // class/metaclass data emission anchor: lane-C now replaces the
  // class-family placeholder byte model with one real descriptor-bundle
  // payload. Each class descriptor bundle carries a class record, an inline
  // metaclass record, one shared class-name cstring, nullable superclass
  // bundle links, and method-list reference globals without claiming that real
  // method/property/ivar list payloads or selector/string pools already exist.
  out << "contract=" << kObjc3RuntimeClassMetaclassEmissionContractId
      << ";payload_model=" << kObjc3RuntimeClassMetaclassEmissionPayloadModel
      << ";name_model=" << kObjc3RuntimeClassMetaclassEmissionNameModel
      << ";super_link_model=" << kObjc3RuntimeClassMetaclassEmissionSuperLinkModel
      << ";method_list_reference_model="
      << kObjc3RuntimeClassMetaclassEmissionMethodListReferenceModel
      << ";non_goals=no-standalone-metaclass-section-or-selector-string-pool";
  return out.str();
}

std::string Objc3RuntimeMetadataProtocolCategoryEmissionSummary() {
  std::ostringstream out;
  // protocol/category data emission anchor: lane-C now replaces the
  // protocol/category family placeholder byte model with real descriptor
  // bundles, count-plus-descriptor protocol-reference lists, and
  // count-plus-owner-identity attachment lists without claiming that real
  // selector/string pools or standalone property/ivar payload sections exist.
  out << "contract=" << kObjc3RuntimeProtocolCategoryEmissionContractId
      << ";protocol_payload_model=" << kObjc3RuntimeProtocolEmissionPayloadModel
      << ";category_payload_model=" << kObjc3RuntimeCategoryEmissionPayloadModel
      << ";protocol_reference_model=" << kObjc3RuntimeProtocolReferenceModel
      << ";category_attachment_model="
      << kObjc3RuntimeCategoryAttachmentModel
      << ";non_goals=no-selector-string-pool-or-standalone-property-ivar-payloads";
  return out.str();
}

std::string Objc3RuntimeMetadataMemberTableEmissionSummary() {
  std::ostringstream out;
  // member-table data emission anchor: lane-C now adds real
  // owner-scoped method tables plus real property/ivar descriptor bytes while
  // preserving the previously frozen class/protocol/category descriptor
  // bundle shapes. Method-table grouping stays declaration-owner/class-kind
  // ordered, and selector/property/field strings remain inline cstrings rather
  // than opening selector/string-pool families yet.
  out << "contract=" << kObjc3RuntimeMemberTableEmissionContractId
      << ";method_list_payload_model="
      << kObjc3RuntimeMethodListEmissionPayloadModel
      << ";method_list_grouping_model="
      << kObjc3RuntimeMethodListEmissionGroupingModel
      << ";property_payload_model="
      << kObjc3RuntimePropertyDescriptorEmissionPayloadModel
      << ";ivar_payload_model="
      << kObjc3RuntimeIvarDescriptorEmissionPayloadModel
      << ";non_goals=no-selector-string-pool-or-runtime-registration";
  return out.str();
}

std::string Objc3RuntimeMetadataSelectorStringPoolEmissionSummary() {
  std::ostringstream out;
  // selector/string pool expansion anchor: lane-C now emits
  // canonical selector and string pool sections with stable ordinal aggregates
  // so runtime-facing payload lookup no longer depends on selector-only globals
  // being the only pooled surface. Existing descriptor bundles remain
  // shape-stable and keep their current inline cstring payloads in this issue.
  out << "contract=" << kObjc3RuntimeSelectorStringPoolEmissionContractId
      << ";selector_pool_payload_model="
      << kObjc3RuntimeSelectorPoolEmissionPayloadModel
      << ";string_pool_payload_model="
      << kObjc3RuntimeStringPoolEmissionPayloadModel
      << ";non_goals=no-runtime-registration-or-descriptor-pool-rewiring";
  return out.str();
}

std::string Objc3ExecutableObjectArtifactLoweringSummary() {
  std::ostringstream out;
  // executable object artifact lowering freeze anchor: lane-C begins
  // from the already-emitted method-list/class/category payload surface where
  // implementation-owned method entries may carry concrete LLVM body symbols
  // and realized object records consume owner-scoped method-list refs. The
  // freeze is explicit that parser/sema remain the source of identities and
  // legality while IR/object emission only binds those decisions into the
  // produced artifact.
  out << "contract=" << kObjc3ExecutableObjectArtifactLoweringContractId
      << ";method_body_binding_model="
      << kObjc3ExecutableObjectArtifactLoweringMethodBodyBindingModel
      << ";realization_record_model="
      << kObjc3ExecutableObjectArtifactLoweringRealizationRecordModel
      << ";method_entry_payload_model="
      << kObjc3ExecutableObjectArtifactLoweringMethodEntryPayloadModel
      << ";scope_model=" << kObjc3ExecutableObjectArtifactLoweringScopeModel
      << ";fail_closed_model="
      << kObjc3ExecutableObjectArtifactLoweringFailClosedModel
      << ";non_goals=no-new-descriptor-families-no-bootstrap-rebinding-no-protocol-executable-realization";
  return out.str();
}

std::string Objc3ExecutablePropertyAccessorLayoutLoweringSummary() {
  std::ostringstream out;
  // accessor/layout lowering freeze anchor: lane-C begins from the
  // already-emitted property/ivar descriptor surface and the sema-approved
  // source-model completion packet. This freeze is explicit that accessor
  // bodies are emitted on the live runtime-helper path while layout facts stay
  // source-owned and runtime consumption remains downstream; lowering
  // republishes the property table, ivar layout, and synthesized binding
  // handoff into emitted IR/object artifacts.
  out << "contract=" << kObjc3ExecutablePropertyAccessorLayoutLoweringContractId
      << ";property_table_model="
      << kObjc3ExecutablePropertyAccessorLayoutLoweringPropertyTableModel
      << ";ivar_layout_model="
      << kObjc3ExecutablePropertyAccessorLayoutLoweringIvarLayoutModel
      << ";accessor_binding_model="
      << kObjc3ExecutablePropertyAccessorLayoutLoweringAccessorBindingModel
      << ";scope_model="
      << kObjc3ExecutablePropertyAccessorLayoutLoweringScopeModel
      << ";fail_closed_model="
      << kObjc3ExecutablePropertyAccessorLayoutLoweringFailClosedModel
      << ";non_goals=no-layout-rederivation-no-reflective-property-registration";
  return out.str();
}

std::string Objc3ExecutableIvarLayoutEmissionSummary() {
  std::ostringstream out;
  // ivar offset/layout emission anchor: lane-C upgrades the frozen
  // C001 handoff into real object payloads. Lowering now materializes
  // sema-approved slot/size/alignment identities as retained offset globals,
  // per-owner layout tables, and ivar descriptor records, but runtime
  // allocation and synthesized accessor execution remain deferred to lane D/C003.
  out << "contract=" << kObjc3ExecutableIvarLayoutEmissionContractId
      << ";descriptor_model=" << kObjc3ExecutableIvarLayoutDescriptorModel
      << ";offset_global_model=" << kObjc3ExecutableIvarOffsetGlobalModel
      << ";layout_table_model=" << kObjc3ExecutableIvarLayoutTableModel
      << ";scope_model=" << kObjc3ExecutableIvarLayoutEmissionScopeModel
      << ";fail_closed_model="
      << kObjc3ExecutableIvarLayoutEmissionFailClosedModel
      << ";non_goals=no-runtime-instance-allocation-no-layout-rederivation-no-accessor-body-synthesis";
  return out.str();
}

std::string Objc3ExecutableSynthesizedAccessorPropertyLoweringSummary() {
  std::ostringstream out;
  // synthesized accessor/property lowering anchor: lane-C promotes
  // sema-approved effective property accessors into executable method entries
  // and direct runtime-helper-backed getter/setter bodies without reopening
  // source-driven layout recovery or reflective property registration.
  out << "contract="
      << kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId
      << ";source_model="
      << kObjc3ExecutableSynthesizedAccessorPropertyLoweringSourceModel
      << ";storage_model="
      << kObjc3ExecutableSynthesizedAccessorPropertyLoweringStorageModel
      << ";property_descriptor_model="
      << kObjc3ExecutableSynthesizedAccessorPropertyLoweringPropertyDescriptorModel
      << ";fail_closed_model="
      << kObjc3ExecutableSynthesizedAccessorPropertyLoweringFailClosedModel
      << ";non_goals=no-shared-storage-bypasses-no-source-layout-rederivation-no-runtime-property-registration";
  return out.str();
}

std::string Objc3RuntimePropertyLayoutConsumptionSummary() {
  std::ostringstream out;
  // runtime property/layout consumption freeze anchor: the current
  // runtime consumes emitted accessor implementation pointers and
  // property/layout attachment identities through the existing lookup/dispatch
  // ABI, hands alloc/new off to realized-layout-backed instance allocation,
  // and executes synthesized accessors against runtime-owned per-instance slot
  // storage selected by the active dispatch frame.
  out << "contract=" << kObjc3RuntimePropertyLayoutConsumptionContractId
      << ";descriptor_model="
      << kObjc3RuntimePropertyLayoutConsumptionDescriptorModel
      << ";allocator_model="
      << kObjc3RuntimePropertyLayoutConsumptionAllocatorModel
      << ";storage_model="
      << kObjc3RuntimePropertyLayoutConsumptionStorageModel
      << ";fail_closed_model="
      << kObjc3RuntimePropertyLayoutConsumptionFailClosedModel
      << ";non_goals=no-source-layout-rederivation-no-reflective-property-registration";
  return out.str();
}

std::string Objc3RuntimeInstanceAllocationLayoutSupportSummary() {
  std::ostringstream out;
  // instance-allocation-layout-runtime anchor: runtime now
  // materializes distinct instance identities from the realized class graph and
  // executes synthesized property access through per-instance slot storage
  // derived from emitted ivar offset/layout metadata rather than lane-C
  // storage globals.
  out << "contract=" << kObjc3RuntimeInstanceAllocationLayoutSupportContractId
      << ";descriptor_model="
      << kObjc3RuntimeInstanceAllocationLayoutSupportDescriptorModel
      << ";allocator_model="
      << kObjc3RuntimeInstanceAllocationLayoutSupportAllocatorModel
      << ";storage_model="
      << kObjc3RuntimeInstanceAllocationLayoutSupportStorageModel
      << ";fail_closed_model="
      << kObjc3RuntimeInstanceAllocationLayoutSupportFailClosedModel
      << ";non_goals=no-source-layout-rederivation-no-reflective-property-registration";
  return out.str();
}

std::string Objc3RuntimePropertyMetadataReflectionSummary() {
  std::ostringstream out;
  // property-metadata-reflection anchor: runtime now publishes a
  // private reflective helper surface over the realized property/accessor/layout
  // graph so tests and diagnostics can query live metadata without reopening
  // the public ABI or rederiving property facts from source.
  out << "contract=" << kObjc3RuntimePropertyMetadataReflectionContractId
      << ";registration_model="
      << kObjc3RuntimePropertyMetadataReflectionRegistrationModel
      << ";query_model=" << kObjc3RuntimePropertyMetadataReflectionQueryModel
      << ";fail_closed_model="
      << kObjc3RuntimePropertyMetadataReflectionFailClosedModel
      << ";non_goals=no-public-runtime-reflection-abi-no-source-recovery";
  return out.str();
}

std::string Objc3RuntimeBackedObjectOwnershipAttributeSurfaceSummary() {
  std::ostringstream out;
  // runtime-backed object ownership attribute surface anchor:
  // ownership-bearing property/member facts stop being manifest-only evidence
  // by flowing into the emitted property descriptor payload that the runtime
  // already consumes for property realization and reflection.
  out << "contract="
      << kObjc3RuntimeBackedObjectOwnershipAttributeSurfaceContractId
      << ";source_model="
      << kObjc3RuntimeBackedObjectOwnershipAttributeSourceModel
      << ";descriptor_model="
      << kObjc3RuntimeBackedObjectOwnershipAttributeDescriptorModel
      << ";runtime_model="
      << kObjc3RuntimeBackedObjectOwnershipAttributeRuntimeModel
      << ";fail_closed_model="
      << kObjc3RuntimeBackedObjectOwnershipAttributeFailClosedModel
      << ";non_goals=no-live-arc-hook-emission-no-source-recovery";
  return out.str();
}

std::string Objc3RetainableObjectSemanticRulesFreezeSummary() {
  std::ostringstream out;
  // retainable-object semantic-rule freeze anchor: runtime-backed
  // property/member ownership metadata and storage legality are now truthful
  // live sema-enforced surfaces, but retain/release legality,
  // autoreleasepool execution, and destruction-order behavior remain
  // summary-driven and fail-closed until the next runtime step lands.
  out << "contract=" << kObjc3RetainableObjectSemanticRulesFreezeContractId
      << ";semantic_model="
      << kObjc3RetainableObjectSemanticRulesSemanticModel
      << ";destruction_model="
      << kObjc3RetainableObjectSemanticRulesDestructionModel
      << ";failure_model="
      << kObjc3RetainableObjectSemanticRulesFailClosedModel;
  return out.str();
}

std::string Objc3RuntimeBackedStorageOwnershipLegalitySummary() {
  std::ostringstream out;
  // runtime-backed storage ownership legality anchor: explicit
  // ownership qualifiers on Objective-C object properties now participate in
  // live semantic legality. Weak and unsafe-unretained qualifiers must agree
  // with the concrete runtime-backed storage modifier family before metadata
  // emission proceeds.
  out << "contract=" << kObjc3RuntimeBackedStorageOwnershipLegalityContractId
      << ";owned_storage_model="
      << kObjc3RuntimeBackedStorageOwnershipOwnedStorageModel
      << ";weak_unowned_model="
      << kObjc3RuntimeBackedStorageOwnershipWeakUnownedModel
      << ";failure_model="
      << kObjc3RuntimeBackedStorageOwnershipFailClosedModel;
  return out.str();
}

std::string Objc3RuntimeBackedAutoreleasepoolDestructionOrderSummary() {
  std::ostringstream out;
  // autoreleasepool/destruction-order semantic expansion anchor:
  // autoreleasepool scopes still fail closed, but owned runtime-backed object storage now upgrades
  // that rejection into a deterministic destruction-order
  // edge diagnostic rather than leaving the ownership-sensitive case
  // indistinguishable from a plain autoreleasepool parse-only probe.
  out << "contract="
      << kObjc3RuntimeBackedAutoreleasepoolDestructionOrderContractId
      << ";autoreleasepool_model="
      << kObjc3RuntimeBackedAutoreleasepoolModel
      << ";destruction_model="
      << kObjc3RuntimeBackedDestructionOrderModel
      << ";failure_model="
      << kObjc3RuntimeBackedAutoreleasepoolDestructionOrderFailClosedModel;
  return out.str();
}

std::string Objc3OwnershipLoweringBaselineSummary() {
  std::ostringstream out;
  // ownership-lowering baseline freeze anchor: runtime-backed
  // ownership metadata and sema legality are already live, but retain/release,
  // autoreleasepool, and weak/unowned execution still stop at legacy lowering
  // summaries instead of emitting a summary-only-without-live-runtime-hook-emission
  // widening before the next runtime step.
  out << "contract=" << kObjc3OwnershipLoweringBaselineContractId
      << ";ownership_qualifier_model="
      << kObjc3OwnershipLoweringBaselineQualifierModel
      << ";runtime_hook_model="
      << kObjc3OwnershipLoweringBaselineRuntimeHookModel
      << ";autoreleasepool_model="
      << kObjc3OwnershipLoweringBaselineAutoreleasepoolModel
      << ";fail_closed_model="
      << kObjc3OwnershipLoweringBaselineFailClosedModel
      << ";ownership_qualifier_lane=" << kObjc3OwnershipQualifierLoweringLaneContract
      << ";retain_release_lane=" << kObjc3RetainReleaseOperationLoweringLaneContract
      << ";autoreleasepool_lane=" << kObjc3AutoreleasePoolScopeLoweringLaneContract
      << ";weak_unowned_lane=" << kObjc3WeakUnownedSemanticsLoweringLaneContract;
  return out.str();
}

std::string Objc3OwnershipRuntimeHookEmissionSummary() {
  std::ostringstream out;
  // runtime hook emission anchor: synthesized accessors now execute
  // through runtime-owned helper entrypoints that operate on the current
  // runtime dispatch frame and realized property layout, while preserving the
  // existing synthesized accessor descriptor/storage artifact surface from
  // 
  out << "contract=" << kObjc3OwnershipRuntimeHookEmissionContractId
      << ";accessor_model="
      << kObjc3OwnershipRuntimeHookEmissionAccessorModel
      << ";property_context_model="
      << kObjc3OwnershipRuntimeHookEmissionPropertyContextModel
      << ";autorelease_model="
      << kObjc3OwnershipRuntimeHookEmissionAutoreleaseModel
      << ";fail_closed_model="
      << kObjc3OwnershipRuntimeHookEmissionFailClosedModel
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";autorelease_symbol=" << kObjc3RuntimeAutoreleaseI32Symbol
      << ";read_property_symbol=" << kObjc3RuntimeReadCurrentPropertyI32Symbol
      << ";write_property_symbol=" << kObjc3RuntimeWriteCurrentPropertyI32Symbol
      << ";exchange_property_symbol="
      << kObjc3RuntimeExchangeCurrentPropertyI32Symbol
      << ";weak_load_symbol="
      << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
      << ";weak_store_symbol="
      << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol;
  return out.str();
}

std::string Objc3RuntimeMemoryManagementApiSummary() {
  std::ostringstream out;
  // runtime memory-management API freeze anchor: the public runtime
  // ABI still stops at registration/lookup/dispatch, while lowered ownership
  // helpers remain private bootstrap-internal entrypoints that runtime probes
  // and lowered IR may consume without widening the stable public header yet.
  out << "contract=" << kObjc3RuntimeMemoryManagementApiContractId
      << ";reference_model="
      << kObjc3RuntimeMemoryManagementApiReferenceModel
      << ";weak_model=" << kObjc3RuntimeMemoryManagementApiWeakModel
      << ";autoreleasepool_model="
      << kObjc3RuntimeMemoryManagementApiAutoreleasepoolModel
      << ";fail_closed_model="
      << kObjc3RuntimeMemoryManagementApiFailClosedModel
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";autorelease_symbol=" << kObjc3RuntimeAutoreleaseI32Symbol
      << ";read_property_symbol=" << kObjc3RuntimeReadCurrentPropertyI32Symbol
      << ";write_property_symbol=" << kObjc3RuntimeWriteCurrentPropertyI32Symbol
      << ";exchange_property_symbol="
      << kObjc3RuntimeExchangeCurrentPropertyI32Symbol
      << ";weak_load_symbol="
      << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
      << ";weak_store_symbol="
      << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol;
  return out.str();
}

std::string Objc3RuntimeMemoryManagementImplementationSummary() {
  std::ostringstream out;
  // runtime memory-management implementation anchor: runtime-backed
  // object execution now owns live refcount, weak-table, and autoreleasepool
  // behavior behind private helper entrypoints and emitted autoreleasepool
  // lowering rather than the older summary-only/fail-closed lane.
  out << "contract=" << kObjc3RuntimeMemoryManagementImplementationContractId
      << ";refcount_model="
      << kObjc3RuntimeMemoryManagementImplementationRefcountModel
      << ";weak_model="
      << kObjc3RuntimeMemoryManagementImplementationWeakModel
      << ";autoreleasepool_model="
      << kObjc3RuntimeMemoryManagementImplementationAutoreleasepoolModel
      << ";fail_closed_model="
      << kObjc3RuntimeMemoryManagementImplementationFailClosedModel
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";autorelease_symbol=" << kObjc3RuntimeAutoreleaseI32Symbol
      << ";push_autoreleasepool_symbol="
      << kObjc3RuntimePushAutoreleasepoolScopeSymbol
      << ";pop_autoreleasepool_symbol="
      << kObjc3RuntimePopAutoreleasepoolScopeSymbol
      << ";weak_load_symbol="
      << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
      << ";weak_store_symbol="
      << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol;
  return out.str();
}

std::string Objc3OwnershipRuntimeGateSummary() {
  std::ostringstream out;
  // ownership-runtime-gate freeze anchor: lane-E now freezes the
  // supported ownership runtime slice and its non-goals using the already-live
  // C002/D001/D002 implementation surfaces as the truthful evidence boundary.
  // ownership-smoke closeout anchor: the runnable smoke matrix
  // consumes this same gate summary unchanged for closeout.
  out << "contract=" << kObjc3OwnershipRuntimeGateContractId
      << ";supported_model=" << kObjc3OwnershipRuntimeGateSupportedModel
      << ";evidence_model=" << kObjc3OwnershipRuntimeGateEvidenceModel
      << ";non_goal_model=" << kObjc3OwnershipRuntimeGateNonGoalModel
      << ";fail_closed_model=" << kObjc3OwnershipRuntimeGateFailClosedModel
      << ";ownership_hook_contract="
      << kObjc3OwnershipRuntimeHookEmissionContractId
      << ";memory_api_contract="
      << kObjc3RuntimeMemoryManagementApiContractId
      << ";memory_implementation_contract="
      << kObjc3RuntimeMemoryManagementImplementationContractId;
  return out.str();
}

std::string Objc3ExecutableBlockSourceClosureSummary() {
  std::ostringstream out;
  // executable-block-source-closure freeze anchor: this summary is
  // intentionally truthful about the current boundary. Parser/AST/source
  // replay for block literals is live, while runnable lowering remains a
  // fail-closed non-goal until later work.
  out << "contract=" << Expr::kObjc3ExecutableBlockSourceClosureContractId
      << ";source_model=" << Expr::kObjc3ExecutableBlockSourceSurfaceModel
      << ";evidence_model=" << Expr::kObjc3ExecutableBlockSourceEvidenceModel
      << ";non_goal_model=" << Expr::kObjc3ExecutableBlockSourceNonGoalModel
      << ";fail_closed_model=" << Expr::kObjc3ExecutableBlockSourceFailureModel
      << ";capture_lane_contract=" << kObjc3BlockLiteralCaptureLoweringLaneContract;
  return out.str();
}

std::string Objc3ExecutableBlockSourceModelCompletionSummary() {
  std::ostringstream out;
  // block-source-model-completion anchor: lane-A now upgrades the
  // frozen block source closure into a deterministic parameter/capture/invoke
  // source model that source-only frontend runs may publish before runnable
  // lowering still fails closed on native emit paths.
  out << "contract="
      << Expr::kObjc3ExecutableBlockSourceModelCompletionContractId
      << ";signature_model=" << Expr::kObjc3ExecutableBlockSignatureModel
      << ";capture_inventory_model="
      << Expr::kObjc3ExecutableBlockCaptureInventoryModel
      << ";invoke_surface_model="
      << Expr::kObjc3ExecutableBlockInvokeSurfaceModel
      << ";evidence_model="
      << Expr::kObjc3ExecutableBlockSourceModelEvidenceModel
      << ";fail_closed_model="
      << Expr::kObjc3ExecutableBlockSourceModelFailureModel
      << ";lane_contract=" << kObjc3BlockSourceModelCompletionLaneContract;
  return out.str();
}

std::string Objc3ExecutableBlockSourceStorageAnnotationSummary() {
  std::ostringstream out;
  // block-source-storage-annotation anchor: lane-A now publishes a
  // truthful byref/helper/escape-shape source inventory without claiming that
  // runnable block lowering, helper emission, or heap promotion already exist.
  out << "contract="
      << Expr::kObjc3ExecutableBlockSourceStorageAnnotationContractId
      << ";byref_storage_model=" << Expr::kObjc3ExecutableBlockByrefStorageModel
      << ";helper_intent_model="
      << Expr::kObjc3ExecutableBlockHelperIntentModel
      << ";escape_shape_model="
      << Expr::kObjc3ExecutableBlockEscapeShapeModel
      << ";lane_contract="
      << kObjc3BlockSourceStorageAnnotationLaneContract;
  return out.str();
}

std::string Objc3ExecutableBlockRuntimeSemanticRulesSummary() {
  std::ostringstream out;
  // block-runtime-semantic-rules freeze anchor: lane-B now freezes
  // the current semantic split where source-only block admission is truthful,
  // deterministic capture/byref/helper/escape annotations exist, and native
  // emit paths still fail closed before runnable block semantics land.
  out << "contract="
      << Expr::kObjc3ExecutableBlockRuntimeSemanticRulesContractId
      << ";capture_legality_model="
      << Expr::kObjc3ExecutableBlockRuntimeCaptureLegalityModel
      << ";storage_class_model="
      << Expr::kObjc3ExecutableBlockRuntimeStorageClassModel
      << ";escape_behavior_model="
      << Expr::kObjc3ExecutableBlockRuntimeEscapeBehaviorModel
      << ";helper_generation_model="
      << Expr::kObjc3ExecutableBlockRuntimeHelperGenerationModel
      << ";invocation_model="
      << Expr::kObjc3ExecutableBlockRuntimeInvocationModel
      << ";fail_closed_model="
      << Expr::kObjc3ExecutableBlockRuntimeFailClosedModel
      << ";lane_contract=" << kObjc3BlockRuntimeSemanticRulesLaneContract;
  return out.str();
}

std::string Objc3ExecutableBlockLoweringAbiArtifactBoundarySummary() {
  std::ostringstream out;
  // block-lowering-ABI/artifact-boundary freeze anchor: lane-C now
  // freezes the truthful lowering boundary that later runnable block-object
  // emission must preserve. The current compiler publishes deterministic
  // capture/invoke/storage/copy-dispose lowering surfaces, but native emit
  // still fails closed before emitted block records, invoke thunks, byref
  // cells, or helper bodies exist.
  out << "contract="
      << Expr::kObjc3ExecutableBlockLoweringAbiArtifactBoundaryContractId
      << ";abi_model=" << Expr::kObjc3ExecutableBlockLoweringAbiModel
      << ";helper_symbol_policy="
      << Expr::kObjc3ExecutableBlockHelperSymbolPolicyModel
      << ";artifact_inventory_model="
      << Expr::kObjc3ExecutableBlockArtifactInventoryModel
      << ";fail_closed_model="
      << Expr::kObjc3ExecutableBlockLoweringFailClosedModel
      << ";non_goal_model="
      << Expr::kObjc3ExecutableBlockLoweringNonGoalModel
      << ";capture_lane_contract="
      << kObjc3BlockLiteralCaptureLoweringLaneContract
      << ";invoke_lane_contract="
      << kObjc3BlockAbiInvokeTrampolineLoweringLaneContract
      << ";storage_lane_contract="
      << kObjc3BlockStorageEscapeLoweringLaneContract
      << ";copy_dispose_lane_contract="
      << kObjc3BlockCopyDisposeLoweringLaneContract
      << ";lane_contract="
      << kObjc3ExecutableBlockLoweringAbiArtifactBoundaryLaneContract;
  return out.str();
}

std::string Objc3ExecutableBlockObjectInvokeThunkLoweringSummary() {
  std::ostringstream out;
  // executable-block-object/invoke-thunk implementation anchor:
  // lane-C now widens the frozen C001 boundary into one real runnable slice.
  // Native lowering emits stack block storage plus one internal invoke thunk
  // for direct local invocation when captures are readonly scalar values. Byref
  // cells, helper bodies, owned-object captures, and heap-promotion semantics
  // remain deferred to C003.
  out << "contract="
      << Expr::kObjc3ExecutableBlockObjectInvokeThunkLoweringContractId
      << ";boundary_contract="
      << Expr::kObjc3ExecutableBlockLoweringAbiArtifactBoundaryContractId
      << ";active_model="
      << Expr::kObjc3ExecutableBlockObjectInvokeThunkActiveModel
      << ";deferred_model="
      << Expr::kObjc3ExecutableBlockObjectInvokeThunkDeferredModel
      << ";execution_evidence_model="
      << Expr::kObjc3ExecutableBlockObjectInvokeThunkExecutionEvidenceModel
      << ";invoke_lane_contract="
      << kObjc3BlockAbiInvokeTrampolineLoweringLaneContract
      << ";storage_lane_contract="
      << kObjc3BlockStorageEscapeLoweringLaneContract
      << ";copy_dispose_lane_contract="
      << kObjc3BlockCopyDisposeLoweringLaneContract
      << ";lane_contract="
      << kObjc3ExecutableBlockObjectInvokeThunkLoweringLaneContract;
  return out.str();
}

std::string Objc3ExecutableBlockByrefHelperLoweringSummary() {
  std::ostringstream out;
  // byref-cell/copy-helper/dispose-helper implementation anchor:
  // lane-C now makes the non-escaping byref and owned-capture block slice
  // runnable by emitting stack byref-cell references plus helper bodies and
  // helper call sites. Heap-promotion and runtime-managed copy/dispose remain
  // intentionally deferred.
  out << "contract="
      << Expr::kObjc3ExecutableBlockByrefHelperLoweringContractId
      << ";previous_contract="
      << Expr::kObjc3ExecutableBlockObjectInvokeThunkLoweringContractId
      << ";active_model="
      << Expr::kObjc3ExecutableBlockByrefHelperLoweringActiveModel
      << ";deferred_model="
      << Expr::kObjc3ExecutableBlockByrefHelperLoweringDeferredModel
      << ";execution_evidence_model="
      << Expr::kObjc3ExecutableBlockByrefHelperLoweringExecutionEvidenceModel
      << ";storage_lane_contract="
      << kObjc3BlockStorageEscapeLoweringLaneContract
      << ";copy_dispose_lane_contract="
      << kObjc3BlockCopyDisposeLoweringLaneContract
      << ";runtime_semantic_lane_contract="
      << kObjc3BlockRuntimeSemanticRulesLaneContract
      << ";lane_contract="
      << kObjc3ExecutableBlockEscapeRuntimeHookLoweringLaneContract;
  return out.str();
}

std::string Objc3ExecutableBlockEscapeRuntimeHookLoweringSummary() {
  std::ostringstream out;
  // escaping-block runtime-hook implementation anchor: lane-C now
  // widens runnable native block lowering to escaping readonly-scalar block
  // values by emitting runtime heap-promotion and invoke hooks, while
  // ownership-sensitive escaping captures remain deferred to later lane-D
  // runtime work.
  out << "contract="
      << Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId
      << ";previous_contract="
      << Expr::kObjc3ExecutableBlockByrefHelperLoweringContractId
      << ";active_model="
      << Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringActiveModel
      << ";deferred_model="
      << Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringDeferredModel
      << ";execution_evidence_model="
      << Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringExecutionEvidenceModel
      << ";storage_lane_contract="
      << kObjc3BlockStorageEscapeLoweringLaneContract
      << ";copy_dispose_lane_contract="
      << kObjc3BlockCopyDisposeLoweringLaneContract
      << ";runtime_semantic_lane_contract="
      << kObjc3BlockRuntimeSemanticRulesLaneContract
      << ";lane_contract="
      << kObjc3ExecutableBlockEscapeRuntimeHookLoweringLaneContract;
  return out.str();
}

std::string Objc3RuntimeBlockApiObjectLayoutSummary() {
  std::ostringstream out;
  // block-runtime API/object-layout freeze anchor: the current
  // runtime helper surface is frozen as a private lowering/runtime contract
  // with opaque storage copies and i32 block handles; no public block-object
  // ABI or generalized heap-managed copy/dispose surface is implied yet.
  out << "contract=" << kObjc3RuntimeBlockApiObjectLayoutContractId
      << ";public_surface=stable-public-runtime-header-excludes-block-helper-entrypoints"
      << ";private_helper_surface=objc3_runtime_promote_block_i32-and-objc3_runtime_invoke_block_i32-remain-private-to-objc3_runtime_bootstrap_internal_h"
      << ";handle_type=i32"
      << ";promotion_abi=ptr-storage-plus-i64-size-plus-i32-pointer-capture-flag"
      << ";invoke_abi=i32-handle-plus-four-i32-arguments-returning-i32"
      << ";runtime_record_model=private-runtime-record-copies-emitted-block-storage-bytes-and-invoke-pointer"
      << ";object_layout_model=runtime-block-records-are-private-runtime-state-not-public-object-abi"
      << ";fail_closed_model=byref-forwarding-and-owned-capture-escaping-block-lifetimes-remain-deferred-until-the-next-runtime-hardening-phase"
      << ";non_goals=no-public-block-object-abi-no-generalized-runtime-copy-dispose-allocation-surface";
  return out.str();
}

std::string Objc3RuntimeBlockAllocationCopyDisposeInvokeSupportSummary() {
  std::ostringstream out;
  // block-runtime allocation/copy-dispose/invoke implementation
  // anchor: promoted runtime block records now preserve helper pointers and
  // aligned copied storage so pointer-capture block records can run copy,
  // invoke, and final-dispose behavior without claiming byref/ownership
  // interop is solved yet.
  out << "contract="
      << kObjc3RuntimeBlockAllocationCopyDisposeInvokeSupportContractId
      << ";previous_contract=" << kObjc3RuntimeBlockApiObjectLayoutContractId
      << ";allocation_model=runtime-block-records-copy-promoted-storage-into-aligned-word-buffers"
      << ";copy_dispose_model=pointer-capture-promotion-runs-copy-helper-and-final-release-runs-dispose-helper"
      << ";invoke_model=runtime-invoke-supports-readonly-scalar-and-pointer-capture-block-records"
      << ";handle_lifetime_model=i32-block-handles-participate-in-runtime-retain-release"
      << ";fail_closed_model=byref-forwarding-runtime-reentrant-helper-bodies-and-owned-capture-escape-interop-remain-deferred-until-next-runtime-phase"
      << ";non_goals=no-public-block-object-abi-no-generalized-public-runtime-helper-surface";
  return out.str();
}

std::string Objc3RuntimeBlockByrefForwardingHeapPromotionInteropSummary() {
  std::ostringstream out;
  // byref-forwarding/heap-promotion/ownership-interop
  // implementation anchor: escaping pointer-capture block promotion now
  // rewrites capture slots onto runtime-owned heap cells before helper
  // execution so byref mutation and owned-capture lifetime hooks survive after
  // the source frame returns.
  out << "contract="
      << kObjc3RuntimeBlockByrefForwardingHeapPromotionInteropContractId
      << ";previous_contract="
      << kObjc3RuntimeBlockAllocationCopyDisposeInvokeSupportContractId
      << ";forwarding_model=escaping-pointer-capture-slots-rewrite-to-runtime-owned-forwarding-cells"
      << ";heap_promotion_model=promotion-deep-copies-captured-i32-cells-before-helper-execution"
      << ";ownership_interop_model=copy-dispose-helpers-run-against-runtime-owned-cells-for-owned-captures"
      << ";invoke_model=escaped-byref-and-owned-capture-block-handles-invoke-after-source-frame-return"
      << ";fail_closed_model=no-public-block-abi-widening-and-no-outer-stack-cell-forwarding-bridge-yet"
      << ";non_goals=no-public-byref-layout-surface-no-generalized-foreign-abi-block-interoperability";
  return out.str();
}

std::string Objc3RunnableBlockRuntimeGateSummary() {
  std::ostringstream out;
  // runnable-block-runtime gate anchor: lane-E now freezes one
  // integrated proof boundary above the retained source, sema, lowering, and
  // runtime summaries so runnable block behavior is validated against the live
  // native path rather than metadata-only claims.
  out << "contract=" << Expr::kObjc3RunnableBlockRuntimeGateContractId
      << ";evidence_model="
      << Expr::kObjc3RunnableBlockRuntimeGateEvidenceModel
      << ";active_model="
      << Expr::kObjc3RunnableBlockRuntimeGateActiveModel
      << ";source_contract="
      << Expr::kObjc3ExecutableBlockSourceStorageAnnotationContractId
      << ";semantic_contract="
      << Expr::kObjc3ExecutableBlockOwnershipSemanticsImplementationContractId
      << ";lowering_contract="
      << Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId
      << ";runtime_contract="
      << kObjc3RuntimeBlockByrefForwardingHeapPromotionInteropContractId
      << ";non_goals="
      << Expr::kObjc3RunnableBlockRuntimeGateNonGoalModel
      << ";fail_closed_model="
      << Expr::kObjc3RunnableBlockRuntimeGateFailClosedModel
      << ";follow_on_surface=objc3c.runtime.block.runnablegate.closeout.v1";
  return out.str();
}

std::string Objc3RunnableBlockExecutionMatrixSummary() {
  std::ostringstream out;
  // runnable-block execution-matrix anchor: lane-E now closes the block-runtime tranche
  // with one truthful executable matrix over the retained source, sema,
  // lowering, runtime, and E001 gate surfaces. This repackages the already
  // supported block slice into an operator-facing closeout proof without
  // widening the public block ABI or helper boundary.
  out << "contract=" << Expr::kObjc3RunnableBlockExecutionMatrixContractId
      << ";evidence_model="
      << Expr::kObjc3RunnableBlockExecutionMatrixEvidenceModel
      << ";active_model="
      << Expr::kObjc3RunnableBlockExecutionMatrixActiveModel
      << ";source_contract="
      << Expr::kObjc3ExecutableBlockSourceStorageAnnotationContractId
      << ";semantic_contract="
      << Expr::kObjc3ExecutableBlockOwnershipSemanticsImplementationContractId
      << ";lowering_contract="
      << Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId
      << ";runtime_contract="
      << kObjc3RuntimeBlockByrefForwardingHeapPromotionInteropContractId
      << ";gate_contract="
      << Expr::kObjc3RunnableBlockRuntimeGateContractId
      << ";non_goals="
      << Expr::kObjc3RunnableBlockExecutionMatrixNonGoalModel
      << ";fail_closed_model="
      << Expr::kObjc3RunnableBlockExecutionMatrixFailClosedModel
      << ";follow_on_surface=objc3c.arc.executionmatrix.surface.v1";
  return out.str();
}

std::string Objc3RuntimeBackedSemanticsClosureSummary() {
  std::ostringstream out;
  // runtime-backed semantics closure anchor: this composite contract is the
  // reviewable boundary tying block, ARC, error, async/task, and actor lowering
  // to private runtime helpers plus durable fixture/report evidence.
  out << "contract=" << kObjc3RuntimeBackedSemanticsClosureContractId
      << ";surface_path=" << kObjc3RuntimeBackedSemanticsClosureSurfacePath
      << ";evidence_model=" << kObjc3RuntimeBackedSemanticsClosureEvidenceModel
      << ";block_model=" << kObjc3RuntimeBackedSemanticsClosureBlockModel
      << ";arc_model=" << kObjc3RuntimeBackedSemanticsClosureArcModel
      << ";error_model=" << kObjc3RuntimeBackedSemanticsClosureErrorModel
      << ";concurrency_model="
      << kObjc3RuntimeBackedSemanticsClosureConcurrencyModel
      << ";runtime_helper_model="
      << kObjc3RuntimeBackedSemanticsClosureRuntimeHelperModel
      << ";block_contract="
      << kObjc3RuntimeBlockByrefForwardingHeapPromotionInteropContractId
      << ";block_gate_contract=" << kObjc3RunnableBlockExecutionMatrixContractId
      << ";arc_contract=" << kObjc3RunnableArcCloseoutContractId
      << ";error_contract="
      << kObjc3ErrorHandlingLiveErrorRuntimeIntegrationContractId
      << ";continuation_contract="
      << kObjc3ConcurrencyLiveContinuationRuntimeIntegrationContractId
      << ";task_contract=" << kObjc3ConcurrencyTaskRuntimeHardeningContractId
      << ";runtime_helper_count=36"
      << ";runtime_helpers="
      << kObjc3RuntimeReadCurrentPropertyI32Symbol << ","
      << kObjc3RuntimeWriteCurrentPropertyI32Symbol << ","
      << kObjc3RuntimeExchangeCurrentPropertyI32Symbol << ","
      << kObjc3RuntimeStoreThrownErrorI32Symbol << ","
      << kObjc3RuntimeLoadThrownErrorI32Symbol << ","
      << kObjc3RuntimeBridgeStatusErrorI32Symbol << ","
      << kObjc3RuntimeBridgeNSErrorErrorI32Symbol << ","
      << kObjc3RuntimeCatchMatchesErrorI32Symbol << ","
      << kObjc3RuntimeAllocateAsyncContinuationI32Symbol << ","
      << kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol << ","
      << kObjc3RuntimeResumeAsyncContinuationI32Symbol << ","
      << kObjc3RuntimeSpawnTaskI32Symbol << ","
      << kObjc3RuntimeEnterTaskGroupScopeI32Symbol << ","
      << kObjc3RuntimeAddTaskGroupTaskI32Symbol << ","
      << kObjc3RuntimeWaitTaskGroupNextI32Symbol << ","
      << kObjc3RuntimeCancelTaskGroupI32Symbol << ","
      << kObjc3RuntimeTaskIsCancelledI32Symbol << ","
      << kObjc3RuntimeTaskOnCancelI32Symbol << ","
      << kObjc3RuntimeExecutorHopI32Symbol << ","
      << kObjc3RuntimeActorEnterIsolationThunkI32Symbol << ","
      << kObjc3RuntimeActorEnterNonisolatedI32Symbol << ","
      << kObjc3RuntimeActorHopToExecutorI32Symbol << ","
      << kObjc3RuntimeActorRecordReplayProofI32Symbol << ","
      << kObjc3RuntimeActorRecordRaceGuardI32Symbol << ","
      << kObjc3RuntimeActorBindExecutorI32Symbol << ","
      << kObjc3RuntimeActorMailboxEnqueueI32Symbol << ","
      << kObjc3RuntimeActorMailboxDrainNextI32Symbol << ","
      << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol << ","
      << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol << ","
      << kObjc3RuntimeRetainI32Symbol << ","
      << kObjc3RuntimeReleaseI32Symbol << ","
      << kObjc3RuntimeAutoreleaseI32Symbol << ","
      << kObjc3RuntimePromoteBlockI32Symbol << ","
      << kObjc3RuntimeInvokeBlockI32Symbol << ","
      << kObjc3RuntimePushAutoreleasepoolScopeSymbol << ","
      << kObjc3RuntimePopAutoreleasepoolScopeSymbol
      << ";fail_closed_model="
      << kObjc3RuntimeBackedSemanticsClosureFailClosedModel
      << ";follow_on_surface=objc3c.runtime.backed.semantics.closeout.v1";
  return out.str();
}

std::string Objc3TypeSystemOptionalKeypathLoweringSummary() {
  std::ostringstream out;
  // optional chaining lowering anchor: `?.member` now desugars onto
  // the same optional-send ABI and nil-short-circuit path already used by
  // bracketed optional sends, so the live lowering packet truthfully covers
  // optional-member access. The later lowering step now widens the same packet to cover
  // validated typed key-path descriptor emission and stable runtime handles
  // without claiming full key-path application/runtime evaluation yet.
  out << "contract_id=" << kObjc3TypeSystemOptionalKeypathLoweringContractId
      << ";optional_model="
      << kObjc3TypeSystemOptionalKeypathLoweringOptionalModel
      << ";typed_keypath_model="
      << kObjc3TypeSystemOptionalKeypathLoweringTypedKeypathModel
      << ";authority_model="
      << kObjc3TypeSystemOptionalKeypathLoweringAuthorityModel
      << ";fail_closed_model="
      << kObjc3TypeSystemOptionalKeypathLoweringFailClosedModel
      << ";lane_contract="
      << kObjc3TypeSystemOptionalKeypathLoweringLaneContract;
  return out.str();
}

std::string Objc3ControlFlowControlFlowSafetyLoweringSummary() {
  std::ostringstream out;
  out << "contract_id=" << kObjc3ControlFlowControlFlowSafetyLoweringContractId
      << ";surface_path="
      << kObjc3ControlFlowControlFlowSafetyLoweringSurfacePath
      << ";guard_model=" << kObjc3ControlFlowControlFlowSafetyLoweringGuardModel
      << ";match_model=" << kObjc3ControlFlowControlFlowSafetyLoweringMatchModel
      << ";defer_model=" << kObjc3ControlFlowControlFlowSafetyLoweringDeferModel
      << ";authority_model="
      << kObjc3ControlFlowControlFlowSafetyLoweringAuthorityModel
      << ";fail_closed_model="
      << kObjc3ControlFlowControlFlowSafetyLoweringFailClosedModel
      << ";lane_contract="
      << kObjc3ControlFlowControlFlowSafetyLoweringLaneContract;
  return out.str();
}

std::string Objc3TypeSystemOptionalKeypathRuntimeHelperContractSummary() {
  std::ostringstream out;
  // live-optional-send-and-keypath-runtime-support anchor: optional
  // sends stay on the public selector lookup/dispatch ABI while validated
  // single-component typed key-path handles now feed the private runtime
  // registry/testing helper surface without falsely claiming full
  // multi-component key-path evaluation.
  out << "contract_id=" << kObjc3TypeSystemOptionalKeypathRuntimeHelperContractId
      << ";surface_path=" << kObjc3TypeSystemOptionalKeypathRuntimeHelperSurfacePath
      << ";optional_model="
      << kObjc3TypeSystemOptionalKeypathRuntimeHelperOptionalModel
      << ";typed_keypath_model="
      << kObjc3TypeSystemOptionalKeypathRuntimeHelperTypedKeypathModel
      << ";diagnostic_model="
      << kObjc3TypeSystemOptionalKeypathRuntimeHelperDiagnosticModel
      << ";lookup_selector_symbol="
      << kObjc3RuntimeSupportLibraryLookupSelectorSymbol
      << ";dispatch_i32_symbol="
      << kObjc3RuntimeSupportLibraryDispatchI32Symbol
      << ";keypath_descriptor_section="
      << kObjc3RuntimeKeypathDescriptorLogicalSection
      << ";keypath_descriptor_aggregate=__objc3_sec_keypath_descriptors"
      << ";typed_keypath_runtime_execution_helper_landed=true";
  return out.str();
}

std::string Objc3ArcSourceModeBoundarySummary() {
  std::ostringstream out;
  // ARC source-surface/mode-boundary anchor: ownership qualifiers,
  // weak/unowned metadata, autoreleasepool profiling, and ARC fix-it surfaces
  // remain live in parser/sema/replay space, but the native driver still
  // rejects `-fobjc-arc` and executable ownership-qualified functions/methods
  // stay fail-closed until ARC automation begins in the next lowering step.
  out << "contract=" << Expr::kObjc3ArcSourceModeBoundaryContractId
      << ";source_model=" << Expr::kObjc3ArcSourceModeBoundarySourceModel
      << ";mode_model=" << Expr::kObjc3ArcSourceModeBoundaryModeModel
      << ";ownership_qualifier_lane="
      << kObjc3OwnershipQualifierLoweringLaneContract
      << ";retain_release_lane="
      << kObjc3RetainReleaseOperationLoweringLaneContract
      << ";autoreleasepool_lane="
      << kObjc3AutoreleasePoolScopeLoweringLaneContract
      << ";weak_unowned_lane="
      << kObjc3WeakUnownedSemanticsLoweringLaneContract
      << ";arc_fixit_lane="
      << kObjc3ArcDiagnosticsFixitLoweringLaneContract
      << ";non_goal_model="
      << Expr::kObjc3ArcSourceModeBoundaryNonGoalModel
      << ";fail_closed_model="
      << Expr::kObjc3ArcSourceModeBoundaryFailClosedModel
      << ";follow_on_surface=objc3c.arc.sourcemode.boundary.v1";
  return out.str();
}

std::string Objc3ArcModeHandlingSummary(bool arc_mode_enabled) {
  std::ostringstream out;
  // ARC mode-handling core implementation anchor: the native driver
  // now admits explicit ARC mode, threads it through frontend/sema/IR, and
  // keeps non-ARC ownership-qualified executable signatures fail-closed.
  out << "contract=" << Expr::kObjc3ArcModeHandlingContractId
      << ";source_model=" << Expr::kObjc3ArcModeHandlingSourceModel
      << ";mode_model=" << Expr::kObjc3ArcModeHandlingModeModel
      << ";arc_mode=" << (arc_mode_enabled ? "enabled" : "disabled")
      << ";ownership_qualifier_lane="
      << kObjc3OwnershipQualifierLoweringLaneContract
      << ";retain_release_lane="
      << kObjc3RetainReleaseOperationLoweringLaneContract
      << ";autoreleasepool_lane="
      << kObjc3AutoreleasePoolScopeLoweringLaneContract
      << ";weak_unowned_lane="
      << kObjc3WeakUnownedSemanticsLoweringLaneContract
      << ";arc_fixit_lane="
      << kObjc3ArcDiagnosticsFixitLoweringLaneContract
      << ";block_runtime_gate=" << Expr::kObjc3RunnableBlockRuntimeGateContractId
      << ";fail_closed_model=" << Expr::kObjc3ArcModeHandlingFailClosedModel
      << ";non_goal_model=" << Expr::kObjc3ArcModeHandlingNonGoalModel
      << ";follow_on_surface=objc3c.arc.modehandling.surface.v1";
  return out.str();
}

std::string Objc3ArcSemanticRulesSummary() {
  std::ostringstream out;
  // ARC semantic-rule freeze anchor: explicit ARC mode is now a real
  // admission boundary, but property ownership conflicts, atomic
  // ownership-aware storage, and broader ARC inference still fail closed until
  // later lane-B implementation issues land.
  out << "contract=" << Expr::kObjc3ArcSemanticRulesContractId
      << ";source_model=" << Expr::kObjc3ArcSemanticRulesSourceModel
      << ";semantic_model=" << Expr::kObjc3ArcSemanticRulesSemanticModel
      << ";weak_unowned_lane=" << kObjc3WeakUnownedSemanticsLoweringLaneContract
      << ";arc_fixit_lane=" << kObjc3ArcDiagnosticsFixitLoweringLaneContract
      << ";fail_closed_model=" << Expr::kObjc3ArcSemanticRulesFailClosedModel
      << ";non_goal_model=" << Expr::kObjc3ArcSemanticRulesNonGoalModel
      << ";follow_on_surface=objc3c.arc.semanticrules.surface.v1";
  return out.str();
}

std::string Objc3ArcInferenceLifetimeSummary() {
  std::ostringstream out;
  // ARC inference/lifetime implementation anchor: explicit ARC mode
  // now upgrades the supported runnable slice from explicit-only ownership
  // spelling to semantic strong-owned inference for unqualified object
  // parameters, returns, and property surfaces, while non-ARC remains a
  // zero-inference baseline and broader ARC cleanup/runtime interactions stay
  // deferred.
  out << "contract=" << Expr::kObjc3ArcInferenceLifetimeContractId
      << ";source_model=" << Expr::kObjc3ArcInferenceLifetimeSourceModel
      << ";semantic_model=" << Expr::kObjc3ArcInferenceLifetimeSemanticModel
      << ";arc_mode_contract=" << Expr::kObjc3ArcModeHandlingContractId
      << ";semantic_rules_contract=" << Expr::kObjc3ArcSemanticRulesContractId
      << ";retain_release_lane="
      << kObjc3RetainReleaseOperationLoweringLaneContract
      << ";block_escape_lane=" << kObjc3BlockStorageEscapeLoweringLaneContract
      << ";fail_closed_model="
      << Expr::kObjc3ArcInferenceLifetimeFailClosedModel
      << ";non_goal_model=" << Expr::kObjc3ArcInferenceLifetimeNonGoalModel
      << ";follow_on_surface=objc3c.arc.inferencelifetime.surface.v1";
  return out.str();
}
