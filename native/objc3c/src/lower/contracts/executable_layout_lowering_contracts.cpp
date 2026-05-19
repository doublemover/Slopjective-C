#include "lower/contracts/executable_layout_lowering_contracts.h"

#include <sstream>
#include <string>

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
      << ";non_goals=no-storage-global-retired-routes-or-sidecar-body-proof-no-source-layout-rederivation-no-runtime-property-registration";
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
      << ";storage_model=" << kObjc3RuntimePropertyLayoutConsumptionStorageModel
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
