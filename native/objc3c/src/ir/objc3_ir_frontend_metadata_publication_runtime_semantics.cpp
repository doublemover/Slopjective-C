#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics.h"

#include <cstddef>
#include <sstream>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRDispatchOwnershipMetadataNodes(
    const Objc3IRFrontendMetadata &metadata,
    std::size_t synthesized_property_accessor_count, std::ostringstream &out) {
  out << "!66 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_surface_classification_instance_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_surface_classification_class_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_surface_classification_super_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_surface_classification_direct_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_surface_classification_dynamic_sites)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.dispatch_surface_classification_instance_entrypoint_family)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.dispatch_surface_classification_class_entrypoint_family)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.dispatch_surface_classification_super_entrypoint_family)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.dispatch_surface_classification_direct_entrypoint_family)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.dispatch_surface_classification_dynamic_entrypoint_family)
      << "\", i1 "
      << (metadata.deterministic_dispatch_surface_classification_handoff ? 1 : 0)
      << "}\n";
  out << "!67 = !{!\""
      << EscapeCStringLiteral(metadata.executable_ivar_layout_emission_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(metadata.executable_ivar_layout_descriptor_model)
      << "\", !\""
      << EscapeCStringLiteral(metadata.executable_ivar_offset_global_model)
      << "\", !\""
      << EscapeCStringLiteral(metadata.executable_ivar_layout_table_model)
      << "\", i1 " << (metadata.executable_ivar_layout_emission_ready ? 1 : 0)
      << ", i1 "
      << (metadata.executable_ivar_layout_emission_fail_closed ? 1 : 0)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.executable_ivar_offset_global_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.executable_ivar_layout_table_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.executable_ivar_layout_owner_entries)
      << ", !\""
      << EscapeCStringLiteral(metadata.executable_ivar_layout_emission_replay_key)
      << "\"}\n";
  out << "!68 = !{!\""
      << EscapeCStringLiteral(
             kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ExecutableSynthesizedAccessorPropertyLoweringSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ExecutableSynthesizedAccessorPropertyLoweringStorageModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ExecutableSynthesizedAccessorPropertyLoweringPropertyDescriptorModel)
      << "\", i64 "
      << static_cast<unsigned long long>(synthesized_property_accessor_count)
      << "}\n";
  out << "!69 = !{!\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionAccessorModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionPropertyContextModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionAutoreleaseModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionFailClosedModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeReadCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeWriteCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeExchangeCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
      << "\", i64 "
      << static_cast<unsigned long long>(synthesized_property_accessor_count)
      << "}\n";
  out << "!70 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiReferenceModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiWeakModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiAutoreleasepoolModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiFailClosedModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeReadCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeWriteCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeExchangeCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
      << "\", i64 "
      << static_cast<unsigned long long>(synthesized_property_accessor_count)
      << "}\n";
  out << "!71 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementImplementationContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeMemoryManagementImplementationRefcountModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementImplementationWeakModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeMemoryManagementImplementationAutoreleasepoolModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeMemoryManagementImplementationFailClosedModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
      << "\", i64 "
      << static_cast<unsigned long long>(synthesized_property_accessor_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.autoreleasepool_scope_lowering_scope_sites)
      << "}\n";
  out << "!72 = !{!\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateSupportedModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateEvidenceModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateNonGoalModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateFailClosedModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeMemoryManagementImplementationContractId)
      << "\"}\n";
}

void EmitObjc3IRBlockArcMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!73 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateContractId)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateEvidenceModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateActiveModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateNonGoalModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateFailClosedModel)
      << "\", !\""
      << EscapeCStringLiteral(
             Expr::kObjc3ExecutableBlockSourceStorageAnnotationContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             Expr::kObjc3ExecutableBlockOwnershipSemanticsImplementationContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeBlockByrefForwardingHeapPromotionInteropContractId)
      << "\"}\n";
  out << "!74 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockExecutionMatrixContractId)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockExecutionMatrixEvidenceModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockExecutionMatrixActiveModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockExecutionMatrixNonGoalModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockExecutionMatrixFailClosedModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateContractId)
      << "\"}\n";
  out << "!75 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundaryContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundarySourceModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundaryModeModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3OwnershipQualifierLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RetainReleaseOperationLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3AutoreleasePoolScopeLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3WeakUnownedSemanticsLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcDiagnosticsFixitLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundaryNonGoalModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundaryFailClosedModel)
      << "\"}\n";
  out << "!76 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingSourceModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingModeModel)
      << "\", !\"" << EscapeCStringLiteral(metadata.arc_mode)
      << "\", !\"" << EscapeCStringLiteral(kObjc3OwnershipQualifierLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RetainReleaseOperationLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3AutoreleasePoolScopeLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3WeakUnownedSemanticsLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcDiagnosticsFixitLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingFailClosedModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingNonGoalModel)
      << "\"}\n";
  out << "!77 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesSourceModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesSemanticModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3WeakUnownedSemanticsLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcDiagnosticsFixitLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesFailClosedModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesNonGoalModel)
      << "\"}\n";
  out << "!78 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeSourceModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeSemanticModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RetainReleaseOperationLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3BlockStorageEscapeLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeFailClosedModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeNonGoalModel)
      << "\"}\n";
  out << "!79 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsSourceModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsSemanticModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3WeakUnownedSemanticsLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RetainReleaseOperationLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3AutoreleasePoolScopeLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3BlockStorageEscapeLoweringLaneContract)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsFailClosedModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsNonGoalModel)
      << "\"}\n";
  out << "!80 = !{!\""
      << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionSourceModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionLoweringModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcLoweringAbiCleanupModelContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionFailureModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionNonGoalModel)
      << "\"}\n";
  out << "!81 = !{!\""
      << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksSourceModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksLoweringModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcLoweringAbiCleanupModelContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksFailureModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksNonGoalModel)
      << "\"}\n";
  out << "!82 = !{!\""
      << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringSourceModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePromoteBlockI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeInvokeBlockI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringFailureModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringNonGoalModel)
      << "\"}\n";
  out << "!83 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceReferenceModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceWeakModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceAutoreleasepoolModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReadCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeWriteCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeExchangeCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceFailClosedModel)
      << "\"}\n";
  out << "!84 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportDependencyModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportWeakModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeArcHelperRuntimeSupportAutoreleaseReturnModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportExecutionModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportFailClosedModel)
      << "\"}\n";
  out << "!85 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationDependencyModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationCoverageModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationValidationModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReadCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeWriteCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeExchangeCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationFailClosedModel)
      << "\"}\n";
  out << "!86 = !{!\""
      << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateEvidenceModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateActiveModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateNonGoalModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcModeHandlingContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcInteractionSemanticsContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateFailClosedModel)
      << "\"}\n";
}

void EmitObjc3IRErrorHandlingMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!87 = !{!\""
      << EscapeCStringLiteral(kObjc3ErrorHandlingThrowsAbiPropagationLoweringContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ErrorHandlingThrowsAbiPropagationLoweringSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ErrorHandlingThrowsAbiPropagationLoweringAbiModel)
      << "\", !\""
      << EscapeCStringLiteral(metadata.lowering_throws_propagation_replay_key)
      << "\", !\""
      << EscapeCStringLiteral(metadata.lowering_result_like_replay_key)
      << "\", !\""
      << EscapeCStringLiteral(metadata.lowering_ns_error_bridging_replay_key)
      << "\", !\""
      << EscapeCStringLiteral(metadata.lowering_unwind_cleanup_replay_key)
      << "\", i1 "
      << (metadata.deterministic_throws_propagation_lowering_handoff ? 1 : 0)
      << ", i1 "
      << (metadata.deterministic_result_like_lowering_handoff ? 1 : 0)
      << ", i1 "
      << (metadata.deterministic_ns_error_bridging_lowering_handoff ? 1 : 0)
      << ", i1 "
      << (metadata.deterministic_unwind_cleanup_lowering_handoff ? 1 : 0)
      << ", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingThrowsAbiPropagationLoweringFailClosedModel)
      << "\"}\n";
  out << "!88 = !{!\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingResultAndBridgingArtifactReplayContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingResultAndBridgingArtifactReplaySourceModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingResultAndBridgingArtifactReplayModel)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.lowering_error_handling_result_and_bridging_artifact_replay_key)
      << "\", i64 "
      << static_cast<unsigned long long>(
             metadata.imported_error_handling_result_and_bridging_artifact_modules)
      << ", i1 "
      << (metadata.error_handling_result_and_bridging_binary_artifact_replay_ready
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .error_handling_result_and_bridging_runtime_import_artifact_ready
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .error_handling_result_and_bridging_separate_compilation_replay_ready
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .deterministic_error_handling_result_and_bridging_artifact_replay_handoff
              ? 1
              : 0)
      << ", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingResultAndBridgingArtifactReplayFailClosedModel)
      << "\"}\n";
  out << "!89 = !{!\""
      << EscapeCStringLiteral(kObjc3ErrorHandlingErrorRuntimeBridgeHelperContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ErrorHandlingErrorRuntimeBridgeHelperSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ErrorHandlingErrorRuntimeBridgeHelperAbiModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeStoreThrownErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeLoadThrownErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBridgeStatusErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBridgeNSErrorErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeCatchMatchesErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingErrorRuntimeBridgeHelperFailClosedModel)
      << "\"}\n";
  out << "!90 = !{!\""
      << EscapeCStringLiteral(kObjc3ErrorHandlingLiveErrorRuntimeIntegrationContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingLiveErrorRuntimeIntegrationSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingLiveErrorRuntimeIntegrationExecutionModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingLiveErrorRuntimeIntegrationPackagingModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeStoreThrownErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeLoadThrownErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBridgeStatusErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeCatchMatchesErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingLiveErrorRuntimeIntegrationFailClosedModel)
      << "\"}\n";
}
