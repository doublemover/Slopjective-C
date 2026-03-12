#include "lower/objc3_lowering_contract.h"

#include "ast/objc3_ast.h"
#include "sema/objc3_sema_contract.h"

#include <cctype>
#include <sstream>
#include <string>

namespace {

bool IsRuntimeDispatchSymbolStart(char c) {
  return std::isalpha(static_cast<unsigned char>(c)) != 0 || c == '_' || c == '$' || c == '.';
}

bool IsRuntimeDispatchSymbolBody(char c) {
  return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_' || c == '$' || c == '.';
}

const char *AtomicMemoryOrderToken(Objc3AtomicMemoryOrder order) {
  switch (order) {
    case Objc3AtomicMemoryOrder::Relaxed:
      return kObjc3AtomicMemoryOrderRelaxed;
    case Objc3AtomicMemoryOrder::Acquire:
      return kObjc3AtomicMemoryOrderAcquire;
    case Objc3AtomicMemoryOrder::Release:
      return kObjc3AtomicMemoryOrderRelease;
    case Objc3AtomicMemoryOrder::AcqRel:
      return kObjc3AtomicMemoryOrderAcqRel;
    case Objc3AtomicMemoryOrder::SeqCst:
    default:
      return kObjc3AtomicMemoryOrderSeqCst;
  }
}

bool IsSupportedVectorBaseSpelling(const std::string &base_spelling) {
  return base_spelling == kObjc3SimdVectorBaseI32 || base_spelling == kObjc3SimdVectorBaseBool;
}

constexpr std::array<const char *, kObjc3RuntimeMetadataLayoutPolicyFamilyCount>
    kCanonicalRuntimeMetadataFamilyOrder = {
        kObjc3RuntimeMetadataLayoutPolicyClassFamily,
        kObjc3RuntimeMetadataLayoutPolicyProtocolFamily,
        kObjc3RuntimeMetadataLayoutPolicyCategoryFamily,
        kObjc3RuntimeMetadataLayoutPolicyPropertyFamily,
        kObjc3RuntimeMetadataLayoutPolicyIvarFamily,
};

std::string VectorTypeSpelling(const std::string &base_spelling, unsigned lane_count) {
  return base_spelling + "x" + std::to_string(lane_count);
}

const char *BoolToken(bool value) { return value ? "true" : "false"; }

// M253-B003 object-format policy expansion anchor: lowering selects one
// supported host object format and derives emitted section spellings from the
// logical metadata ABI surface before IR emission begins.
const char *HostRuntimeMetadataObjectFormat() {
#if defined(_WIN32)
  return kObjc3RuntimeMetadataObjectFormatCoff;
#elif defined(__APPLE__)
  return kObjc3RuntimeMetadataObjectFormatMachO;
#else
  return kObjc3RuntimeMetadataObjectFormatElf;
#endif
}

const char *HostRuntimeMetadataSectionSpellingModel() {
#if defined(_WIN32)
  return kObjc3RuntimeMetadataSectionSpellingModelCoff;
#elif defined(__APPLE__)
  return kObjc3RuntimeMetadataSectionSpellingModelMachO;
#else
  return kObjc3RuntimeMetadataSectionSpellingModelElf;
#endif
}

const char *HostRuntimeMetadataRetentionAnchorModel() {
#if defined(_WIN32)
  return kObjc3RuntimeMetadataRetentionAnchorModelCoff;
#elif defined(__APPLE__)
  return kObjc3RuntimeMetadataRetentionAnchorModelMachO;
#else
  return kObjc3RuntimeMetadataRetentionAnchorModelElf;
#endif
}

bool IsSupportedRuntimeMetadataObjectFormat(const std::string &object_format) {
  return object_format == kObjc3RuntimeMetadataObjectFormatCoff ||
         object_format == kObjc3RuntimeMetadataObjectFormatElf ||
         object_format == kObjc3RuntimeMetadataObjectFormatMachO;
}

std::string MapRuntimeMetadataSectionForObjectFormat(
    const std::string &object_format, const std::string &logical_section) {
  if (logical_section.empty()) {
    return "";
  }
  if (object_format == kObjc3RuntimeMetadataObjectFormatCoff ||
      object_format == kObjc3RuntimeMetadataObjectFormatElf) {
    return logical_section;
  }
  if (object_format == kObjc3RuntimeMetadataObjectFormatMachO) {
    constexpr const char *kLogicalPrefix = "objc3.runtime.";
    const std::string logical_prefix = kLogicalPrefix;
    if (logical_section.rfind(logical_prefix, 0) != 0) {
      return "";
    }
    return "__DATA,__objc3_" + logical_section.substr(logical_prefix.size());
  }
  return "";
}

std::string BuildRuntimeMetadataDriverLinkerRetentionFlagForObjectFormat(
    const std::string &object_format, const std::string &symbol_name) {
  if (symbol_name.empty()) {
    return "";
  }
  if (object_format == kObjc3RuntimeMetadataObjectFormatCoff) {
    return std::string("-Wl,/include:") + symbol_name;
  }
  if (object_format == kObjc3RuntimeMetadataObjectFormatElf) {
    return std::string("-Wl,--undefined=") + symbol_name;
  }
  if (object_format == kObjc3RuntimeMetadataObjectFormatMachO) {
    return std::string("-Wl,-u,_") + symbol_name;
  }
  return "";
}

std::size_t CountRuntimeMetadataLayoutDescriptors(
    const std::array<Objc3RuntimeMetadataLayoutPolicyFamily,
                     kObjc3RuntimeMetadataLayoutPolicyFamilyCount> &families) {
  std::size_t total = 0;
  for (const auto &family : families) {
    total += family.descriptor_count;
  }
  return total;
}

}  // namespace

bool IsValidRuntimeDispatchSymbol(const std::string &symbol) {
  if (symbol.empty() || !IsRuntimeDispatchSymbolStart(symbol[0])) {
    return false;
  }
  for (std::size_t i = 1; i < symbol.size(); ++i) {
    if (!IsRuntimeDispatchSymbolBody(symbol[i])) {
      return false;
    }
  }
  return true;
}

bool TryNormalizeObjc3LoweringContract(const Objc3LoweringContract &input,
                                       Objc3LoweringContract &normalized,
                                       std::string &error) {
  if (input.max_message_send_args > kObjc3RuntimeDispatchMaxArgs) {
    error = "invalid lowering contract max_message_send_args: " + std::to_string(input.max_message_send_args) +
            " (expected <= " + std::to_string(kObjc3RuntimeDispatchMaxArgs) + ")";
    return false;
  }
  if (!IsValidRuntimeDispatchSymbol(input.runtime_dispatch_symbol)) {
    error = "invalid lowering contract runtime_dispatch_symbol (expected [A-Za-z_.$][A-Za-z0-9_.$]*): " +
            input.runtime_dispatch_symbol;
    return false;
  }
  normalized.max_message_send_args = input.max_message_send_args;
  normalized.runtime_dispatch_symbol = input.runtime_dispatch_symbol;
  return true;
}

bool TryBuildObjc3LoweringIRBoundary(const Objc3LoweringContract &input,
                                     Objc3LoweringIRBoundary &boundary,
                                     std::string &error) {
  Objc3LoweringContract normalized;
  if (!TryNormalizeObjc3LoweringContract(input, normalized, error)) {
    return false;
  }
  boundary.runtime_dispatch_arg_slots = normalized.max_message_send_args;
  boundary.runtime_dispatch_symbol = normalized.runtime_dispatch_symbol;
  boundary.selector_global_ordering = kObjc3SelectorGlobalOrdering;
  return true;
}

std::string Objc3LoweringIRBoundaryReplayKey(const Objc3LoweringIRBoundary &boundary) {
  // M253-A001 emitted metadata inventory freeze anchor: replay keys here
  // cover lowering/message-send ABI only. Runtime metadata section inventory
  // stays frozen in the frontend ABI/scaffold summaries and is not inferred
  // from lowering-boundary strings.
  // M253-A002 source-to-section matrix anchor: the node-to-section matrix
  // remains a frontend artifact summary rather than a lowering-boundary string.
  // M253-B001 layout/visibility policy anchor: replay keys may not infer or
  // rewrite metadata family ordering, descriptor ordinal ordering,
  // zero-sentinel-or-count-plus-pointer-vector relocation, the local-linkage/no-COMDAT policy,
  // explicit visibility spelling policy, or llvm.used retention order. Those
  // remain one frozen emitted policy surface until M253-B002 and M253-B003
  // extend them explicitly.
  return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +
         ";runtime_dispatch_arg_slots=" + std::to_string(boundary.runtime_dispatch_arg_slots) +
         ";selector_global_ordering=" + boundary.selector_global_ordering;
}

bool UsesCanonicalObjc3RuntimeDispatchEntrypoint(
    const std::string &dispatch_surface_family) {
  return dispatch_surface_family == kObjc3DispatchSurfaceInstanceFamily ||
         dispatch_surface_family == kObjc3DispatchSurfaceClassFamily ||
         dispatch_surface_family == kObjc3DispatchSurfaceSuperFamily ||
         dispatch_surface_family == kObjc3DispatchSurfaceDynamicFamily;
}

bool RequiresFailClosedObjc3RuntimeDispatchFallback(
    const std::string &dispatch_surface_family) {
  return dispatch_surface_family == kObjc3DispatchSurfaceDirectFamily;
}

const char *Objc3DispatchSurfaceRuntimeEntrypointSymbol(
    const std::string &dispatch_surface_family) {
  // M255-C004 live-dispatch cutover anchor: all supported live dispatch
  // surfaces now route through objc3_runtime_dispatch_i32, the exported
  // compatibility symbol remains a non-emitted alias/test surface, and
  // reserved direct-dispatch cases still fail closed before IR emission.
  return UsesCanonicalObjc3RuntimeDispatchEntrypoint(dispatch_surface_family)
             ? kObjc3RuntimeDispatchLoweringCanonicalEntrypointSymbol
             : kObjc3RuntimeDispatchLoweringCompatibilityEntrypointSymbol;
}

bool TryBuildObjc3RuntimeMetadataLayoutPolicy(
    const Objc3RuntimeMetadataLayoutPolicyInput &input,
    Objc3RuntimeMetadataLayoutPolicy &policy, std::string &error) {
  policy = Objc3RuntimeMetadataLayoutPolicy{};
  policy.abi_contract_id = input.abi_contract_id;
  policy.scaffold_contract_id = input.scaffold_contract_id;
  policy.object_format = HostRuntimeMetadataObjectFormat();
  policy.section_spelling_model = HostRuntimeMetadataSectionSpellingModel();
  policy.retention_anchor_model = HostRuntimeMetadataRetentionAnchorModel();
  policy.image_info_symbol = input.image_info_symbol;
  policy.logical_image_info_section = input.image_info_section;
  policy.emitted_image_info_section = MapRuntimeMetadataSectionForObjectFormat(
      policy.object_format, input.image_info_section);
  policy.descriptor_symbol_prefix = input.descriptor_symbol_prefix;
  policy.descriptor_linkage = input.descriptor_linkage;
  policy.aggregate_linkage = input.aggregate_linkage;
  policy.metadata_visibility = input.metadata_visibility;
  policy.retention_root = input.retention_root;
  policy.total_retained_global_count = input.total_retained_global_count;
  policy.fail_closed = input.scaffold_fail_closed;

  if (input.abi_contract_id.empty()) {
    error = "runtime metadata layout policy requires a non-empty ABI contract id";
    policy.failure_reason = error;
    return false;
  }
  if (input.scaffold_contract_id.empty()) {
    error =
        "runtime metadata layout policy requires a non-empty scaffold contract id";
    policy.failure_reason = error;
    return false;
  }
  if (!input.section_boundary_ready || !input.runtime_export_ready ||
      !input.scaffold_emitted || !input.scaffold_fail_closed ||
      !input.uses_llvm_used || !input.image_info_emitted) {
    error = "runtime metadata layout policy prerequisites are not ready";
    policy.failure_reason = error;
    return false;
  }
  if (input.image_info_symbol.empty() || input.image_info_section.empty() ||
      policy.emitted_image_info_section.empty()) {
    error =
        "runtime metadata layout policy requires image-info symbol and section";
    policy.failure_reason = error;
    return false;
  }
  if (!IsSupportedRuntimeMetadataObjectFormat(policy.object_format) ||
      policy.section_spelling_model.empty() ||
      policy.retention_anchor_model.empty()) {
    error =
        "runtime metadata layout policy requires a supported explicit object-format surface";
    policy.failure_reason = error;
    return false;
  }
  if (input.descriptor_symbol_prefix.empty()) {
    error =
        "runtime metadata layout policy requires a descriptor symbol prefix";
    policy.failure_reason = error;
    return false;
  }
  if (input.descriptor_linkage != "private") {
    error = "runtime metadata layout policy requires descriptor linkage private";
    policy.failure_reason = error;
    return false;
  }
  if (input.aggregate_linkage != "internal") {
    error = "runtime metadata layout policy requires aggregate linkage internal";
    policy.failure_reason = error;
    return false;
  }
  if (input.metadata_visibility != "hidden") {
    error =
        "runtime metadata layout policy requires metadata visibility hidden";
    policy.failure_reason = error;
    return false;
  }
  if (input.retention_root != "llvm.used") {
    error = "runtime metadata layout policy requires llvm.used retention root";
    policy.failure_reason = error;
    return false;
  }

  for (std::size_t i = 0; i < kCanonicalRuntimeMetadataFamilyOrder.size(); ++i) {
    const auto &family_input = input.families[i];
    auto &family = policy.families[i];
    family.kind = kCanonicalRuntimeMetadataFamilyOrder[i];
    family.logical_section_name = family_input.section_name;
    family.emitted_section_name = MapRuntimeMetadataSectionForObjectFormat(
        policy.object_format, family_input.section_name);
    family.aggregate_symbol_name = family_input.aggregate_symbol_name;
    family.descriptor_count = family_input.descriptor_count;

    if (family_input.kind != kCanonicalRuntimeMetadataFamilyOrder[i]) {
      error = "runtime metadata layout policy family order mismatch at index " +
              std::to_string(i) + ": expected " +
              kCanonicalRuntimeMetadataFamilyOrder[i] + " but saw " +
              family_input.kind;
      policy.failure_reason = error;
      return false;
    }
    if (family.logical_section_name.empty() ||
        family.emitted_section_name.empty() ||
        family.aggregate_symbol_name.empty()) {
      error =
          "runtime metadata layout policy requires non-empty family section and aggregate names";
      policy.failure_reason = error;
      return false;
    }
  }

  const std::size_t total_descriptor_count =
      CountRuntimeMetadataLayoutDescriptors(policy.families);
  if (input.total_retained_global_count != total_descriptor_count + 6u) {
    error =
        "runtime metadata layout policy retained-global count drifted from descriptor inventory";
    policy.failure_reason = error;
    return false;
  }

  policy.ready = true;
  policy.fail_closed = true;
  return true;
}

bool IsReadyObjc3RuntimeMetadataLayoutPolicy(
    const Objc3RuntimeMetadataLayoutPolicy &policy) {
  if (!policy.ready || !policy.fail_closed || policy.contract_id.empty() ||
      policy.abi_contract_id.empty() || policy.scaffold_contract_id.empty() ||
      policy.object_format_surface_contract_id.empty() ||
      !IsSupportedRuntimeMetadataObjectFormat(policy.object_format) ||
      policy.section_spelling_model.empty() ||
      policy.retention_anchor_model.empty() || policy.image_info_symbol.empty() ||
      policy.logical_image_info_section.empty() ||
      policy.emitted_image_info_section.empty() ||
      policy.descriptor_symbol_prefix.empty() ||
      policy.descriptor_linkage != "private" ||
      policy.aggregate_linkage != "internal" ||
      policy.metadata_visibility != "hidden" ||
      policy.retention_root != "llvm.used" ||
      policy.family_ordering_model !=
          kObjc3RuntimeMetadataLayoutFamilyOrderingModel ||
      policy.descriptor_ordering_model !=
          kObjc3RuntimeMetadataDescriptorOrderingModel ||
      policy.aggregate_relocation_policy !=
          kObjc3RuntimeMetadataAggregateRelocationPolicy ||
      policy.comdat_policy != kObjc3RuntimeMetadataComdatPolicy ||
      policy.visibility_spelling_policy !=
          kObjc3RuntimeMetadataVisibilitySpellingPolicy ||
      policy.retention_ordering_model !=
          kObjc3RuntimeMetadataRetentionOrderingModel ||
      policy.object_format_policy_model !=
          kObjc3RuntimeMetadataObjectFormatPolicyModel ||
      !policy.failure_reason.empty()) {
    return false;
  }

  for (std::size_t i = 0; i < kCanonicalRuntimeMetadataFamilyOrder.size(); ++i) {
    const auto &family = policy.families[i];
    if (family.kind != kCanonicalRuntimeMetadataFamilyOrder[i] ||
        family.logical_section_name.empty() ||
        family.emitted_section_name.empty() ||
        family.aggregate_symbol_name.empty()) {
      return false;
    }
  }

  return policy.total_retained_global_count ==
         CountRuntimeMetadataLayoutDescriptors(policy.families) + 6u;
}

std::string Objc3RuntimeMetadataLayoutPolicyReplayKey(
    const Objc3RuntimeMetadataLayoutPolicy &policy) {
  std::ostringstream out;
  // M253-B002 normalized layout policy anchor: replay proof now serializes the
  // canonical normalized metadata layout decision rather than relying on
  // emitter-local hardcoded family ordering or relocation semantics.
  // M253-B003 object-format policy expansion anchor: replay proof now also
  // serializes the explicit host-format surface, including emitted section
  // spellings and retention-anchor behavior.
  out << "contract=" << policy.contract_id
      << ";abi_contract=" << policy.abi_contract_id
      << ";scaffold_contract=" << policy.scaffold_contract_id
      << ";object_format_contract="
      << policy.object_format_surface_contract_id
      << ";ready=" << BoolToken(policy.ready)
      << ";fail_closed=" << BoolToken(policy.fail_closed)
      << ";family_order=" << policy.family_ordering_model
      << ";descriptor_order=" << policy.descriptor_ordering_model
      << ";aggregate_relocation=" << policy.aggregate_relocation_policy
      << ";comdat=" << policy.comdat_policy
      << ";visibility_spelling=" << policy.visibility_spelling_policy
      << ";retention_order=" << policy.retention_ordering_model
      << ";object_format_model=" << policy.object_format_policy_model
      << ";object_format=" << policy.object_format
      << ";section_spelling_model=" << policy.section_spelling_model
      << ";retention_anchor_model=" << policy.retention_anchor_model
      << ";image_info=" << policy.image_info_symbol << "@"
      << policy.logical_image_info_section
      << ";image_info_emitted=" << policy.image_info_symbol << "@"
      << policy.emitted_image_info_section
      << ";descriptor_linkage=" << policy.descriptor_linkage
      << ";aggregate_linkage=" << policy.aggregate_linkage
      << ";metadata_visibility=" << policy.metadata_visibility
      << ";retention_root=" << policy.retention_root
      << ";total_retained_globals=" << policy.total_retained_global_count;
  for (const auto &family : policy.families) {
    out << ";family=" << family.kind << "|" << family.logical_section_name
        << "|" << family.aggregate_symbol_name << "|" << family.descriptor_count
        << ";family_emitted=" << family.kind << "|"
        << family.emitted_section_name << "|" << family.aggregate_symbol_name
        << "|" << family.descriptor_count;
  }
  if (!policy.failure_reason.empty()) {
    out << ";failure=" << policy.failure_reason;
  }
  return out.str();
}

std::string Objc3RuntimeMetadataSectionEmissionBoundarySummary() {
  std::ostringstream out;
  // M253-C001 metadata section emission freeze anchor: lane-C begins from the
  // current real-section scaffold state rather than from manifest-only
  // summaries. The boundary is explicit that payload bytes are still
  // placeholders until later implementation issues replace them.
  out << "contract=" << kObjc3RuntimeMetadataSectionEmissionContractId
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
  // M253-C002 class/metaclass data emission anchor: lane-C now replaces the
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
  // M253-C003 protocol/category data emission anchor: lane-C now replaces the
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
  // M253-C004 member-table data emission anchor: lane-C now adds real
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
  // M253-C005 selector/string pool expansion anchor: lane-C now emits
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
  // M256-C001 executable object artifact lowering freeze anchor: lane-C begins
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
  // M257-C001 accessor/layout lowering freeze anchor: lane-C begins from the
  // already-emitted property/ivar descriptor surface and the sema-approved
  // source-model completion packet. This freeze is explicit that accessor
  // bodies and runtime storage/layout realization remain deferred; lowering
  // only republishes the property table, ivar layout, and synthesized binding
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
      << ";non_goals=no-synthesized-accessor-body-emission-no-runtime-storage-allocation-no-instance-layout-realization";
  return out.str();
}

std::string Objc3ExecutableIvarLayoutEmissionSummary() {
  std::ostringstream out;
  // M257-C002 ivar offset/layout emission anchor: lane-C upgrades the frozen
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
  // M257-C003 synthesized accessor/property lowering anchor: lane-C promotes
  // sema-approved effective property accessors into executable method entries
  // and deterministic storage globals without reopening true runtime instance
  // allocation or reflective property registration.
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
      << ";non_goals=no-runtime-instance-allocation-no-runtime-property-registration";
  return out.str();
}

std::string Objc3RuntimePropertyLayoutConsumptionSummary() {
  std::ostringstream out;
  // M257-D001 runtime property/layout consumption freeze anchor: the current
  // runtime consumes emitted accessor implementation pointers and
  // property/layout attachment identities through the existing lookup/dispatch
  // ABI, but alloc/new still collapse onto one canonical realized instance
  // identity per class and synthesized accessors still execute against the
  // lane-C storage globals until D002 introduces true instance slots.
  out << "contract=" << kObjc3RuntimePropertyLayoutConsumptionContractId
      << ";descriptor_model="
      << kObjc3RuntimePropertyLayoutConsumptionDescriptorModel
      << ";allocator_model="
      << kObjc3RuntimePropertyLayoutConsumptionAllocatorModel
      << ";storage_model="
      << kObjc3RuntimePropertyLayoutConsumptionStorageModel
      << ";fail_closed_model="
      << kObjc3RuntimePropertyLayoutConsumptionFailClosedModel
      << ";non_goals=no-true-instance-allocation-no-per-instance-slot-storage-no-reflective-property-registration";
  return out.str();
}

std::string Objc3RuntimeInstanceAllocationLayoutSupportSummary() {
  std::ostringstream out;
  // M257-D002 instance-allocation-layout-runtime anchor: runtime now
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
  // M257-D003 property-metadata-reflection anchor: runtime now publishes a
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
  // M260-A002 runtime-backed object ownership attribute surface anchor:
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
  // M260-B001 retainable-object semantic-rule freeze anchor: runtime-backed
  // property/member ownership metadata is now the truthful live surface, but
  // retain/release legality, autoreleasepool execution, and destruction-order
  // behavior remain summary-driven and fail-closed until M260-B002+ land.
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
  // M260-B002 runtime-backed storage ownership legality anchor: explicit
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
  // M260-B003 autoreleasepool/destruction-order semantic expansion anchor:
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
  // M260-C001 ownership-lowering baseline freeze anchor: runtime-backed
  // ownership metadata and sema legality are already live, but retain/release,
  // autoreleasepool, and weak/unowned execution still stop at legacy lowering
  // summaries instead of emitting a summary-only-without-live-runtime-hook-emission
  // widening before M260-C002.
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
  // M260-C002 runtime hook emission anchor: synthesized accessors now execute
  // through runtime-owned helper entrypoints that operate on the current
  // runtime dispatch frame and realized property layout, while preserving the
  // existing synthesized accessor descriptor/storage artifact surface from
  // M257-C003.
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
  // M260-D001 runtime memory-management API freeze anchor: the public runtime
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
  // M260-D002 runtime memory-management implementation anchor: runtime-backed
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
  // M260-E001 ownership-runtime-gate freeze anchor: lane-E now freezes the
  // supported ownership runtime slice and its non-goals using the already-live
  // C002/D001/D002 implementation surfaces as the truthful evidence boundary.
  // M260-E002 ownership-smoke closeout anchor: the runnable smoke matrix
  // consumes this same gate summary unchanged for M260 closeout.
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
  // M261-A001 executable-block-source-closure freeze anchor: this summary is
  // intentionally truthful about the current boundary. Parser/AST/source
  // replay for block literals is live, while runnable lowering remains a
  // fail-closed non-goal until later M261 issues.
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
  // M261-A002 block-source-model-completion anchor: lane-A now upgrades the
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
  // M261-A003 block-source-storage-annotation anchor: lane-A now publishes a
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
  // M261-B001 block-runtime-semantic-rules freeze anchor: lane-B now freezes
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
  // M261-C001 block-lowering-ABI/artifact-boundary freeze anchor: lane-C now
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
  // M261-C002 executable-block-object/invoke-thunk implementation anchor:
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
  // M261-C003 byref-cell/copy-helper/dispose-helper implementation anchor:
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
  // M261-C004 escaping-block runtime-hook implementation anchor: lane-C now
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
  // M261-D001 block-runtime API/object-layout freeze anchor: the current
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
      << ";fail_closed_model=byref-forwarding-and-owned-capture-escaping-block-lifetimes-remain-deferred-until-m261-d002-and-m261-d003"
      << ";non_goals=no-public-block-object-abi-no-generalized-runtime-copy-dispose-allocation-surface";
  return out.str();
}

std::string Objc3RuntimeBlockAllocationCopyDisposeInvokeSupportSummary() {
  std::ostringstream out;
  // M261-D002 block-runtime allocation/copy-dispose/invoke implementation
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
      << ";fail_closed_model=byref-forwarding-runtime-reentrant-helper-bodies-and-owned-capture-escape-interop-remain-deferred-until-m261-d003"
      << ";non_goals=no-public-block-object-abi-no-generalized-public-runtime-helper-surface";
  return out.str();
}

std::string Objc3RuntimeBlockByrefForwardingHeapPromotionInteropSummary() {
  std::ostringstream out;
  // M261-D003 byref-forwarding/heap-promotion/ownership-interop
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
  // M261-E001 runnable-block-runtime gate anchor: lane-E now freezes one
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
      << ";next_issue=M261-E002";
  return out.str();
}

std::string Objc3RunnableBlockExecutionMatrixSummary() {
  std::ostringstream out;
  // M261-E002 runnable-block execution-matrix anchor: lane-E now closes M261
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
      << ";next_issue=M262-A001";
  return out.str();
}

std::string Objc3ArcSourceModeBoundarySummary() {
  std::ostringstream out;
  // M262-A001 ARC source-surface/mode-boundary anchor: ownership qualifiers,
  // weak/unowned metadata, autoreleasepool profiling, and ARC fix-it surfaces
  // remain live in parser/sema/replay space, but the native driver still
  // rejects `-fobjc-arc` and executable ownership-qualified functions/methods
  // stay fail-closed until ARC automation begins in M262-A002.
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
      << ";next_issue=M262-A002";
  return out.str();
}

std::string Objc3ArcModeHandlingSummary(bool arc_mode_enabled) {
  std::ostringstream out;
  // M262-A002 ARC mode-handling core implementation anchor: the native driver
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
      << ";next_issue=M262-B001";
  return out.str();
}

std::string Objc3ArcSemanticRulesSummary() {
  std::ostringstream out;
  // M262-B001 ARC semantic-rule freeze anchor: explicit ARC mode is now a real
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
      << ";next_issue=M262-B002";
  return out.str();
}

std::string Objc3ArcInferenceLifetimeSummary() {
  std::ostringstream out;
  // M262-B002 ARC inference/lifetime implementation anchor: explicit ARC mode
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
      << ";next_issue=M262-B003";
  return out.str();
}

std::string Objc3ArcInteractionSemanticsSummary() {
  std::ostringstream out;
  // M262-B003 ARC interaction-semantics expansion anchor: explicit ARC mode
  // now carries one truthful semantic packet over weak/non-owning property
  // and block interactions, explicit autorelease returns, and synthesized
  // property accessor ownership packets for the supported runnable slice,
  // while generalized ARC cleanup and broader automation remain deferred.
  out << "contract=" << Expr::kObjc3ArcInteractionSemanticsContractId
      << ";source_model=" << Expr::kObjc3ArcInteractionSemanticsSourceModel
      << ";semantic_model=" << Expr::kObjc3ArcInteractionSemanticsSemanticModel
      << ";arc_inference_contract=" << Expr::kObjc3ArcInferenceLifetimeContractId
      << ";weak_unowned_lane=" << kObjc3WeakUnownedSemanticsLoweringLaneContract
      << ";retain_release_lane="
      << kObjc3RetainReleaseOperationLoweringLaneContract
      << ";autoreleasepool_lane="
      << kObjc3AutoreleasePoolScopeLoweringLaneContract
      << ";block_escape_lane=" << kObjc3BlockStorageEscapeLoweringLaneContract
      << ";synthesized_accessor_contract="
      << kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId
      << ";fail_closed_model="
      << Expr::kObjc3ArcInteractionSemanticsFailClosedModel
      << ";non_goal_model="
      << Expr::kObjc3ArcInteractionSemanticsNonGoalModel
      << ";next_issue=M262-C001";
  return out.str();
}

std::string Objc3ArcLoweringAbiCleanupModelSummary() {
  std::ostringstream out;
  // M262-C001 ARC lowering ABI/cleanup freeze anchor: lane-C now freezes the
  // current lowering boundary as the combination of semantic ARC packets,
  // unwind-cleanup accounting, and private runtime helper entrypoints, while
  // generalized cleanup scheduling and helper-placement automation remain
  // deferred to the later lane-C implementation issues.
  out << "contract=" << kObjc3ArcLoweringAbiCleanupModelContractId
      << ";source_model=" << kObjc3ArcLoweringAbiCleanupModelSourceModel
      << ";abi_model=" << kObjc3ArcLoweringAbiCleanupModelAbiModel
      << ";cleanup_model=" << kObjc3ArcLoweringAbiCleanupModelCleanupModel
      << ";arc_mode_contract=" << Expr::kObjc3ArcModeHandlingContractId
      << ";arc_semantic_rules_contract=" << Expr::kObjc3ArcSemanticRulesContractId
      << ";arc_inference_contract=" << Expr::kObjc3ArcInferenceLifetimeContractId
      << ";arc_interaction_contract=" << Expr::kObjc3ArcInteractionSemanticsContractId
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";autorelease_symbol=" << kObjc3RuntimeAutoreleaseI32Symbol
      << ";weak_load_symbol=" << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
      << ";weak_store_symbol=" << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
      << ";autoreleasepool_push_symbol="
      << kObjc3RuntimePushAutoreleasepoolScopeSymbol
      << ";autoreleasepool_pop_symbol="
      << kObjc3RuntimePopAutoreleasepoolScopeSymbol
      << ";retain_release_lane="
      << kObjc3RetainReleaseOperationLoweringLaneContract
      << ";weak_unowned_lane=" << kObjc3WeakUnownedSemanticsLoweringLaneContract
      << ";autoreleasepool_lane="
      << kObjc3AutoreleasePoolScopeLoweringLaneContract
      << ";unwind_cleanup_lane=" << kObjc3UnwindCleanupLoweringLaneContract
      << ";fail_closed_model="
      << kObjc3ArcLoweringAbiCleanupModelFailClosedModel
      << ";non_goal_model="
      << kObjc3ArcLoweringAbiCleanupModelNonGoalModel
      << ";next_issue=M262-C002";
  return out.str();
}

std::string Objc3ArcAutomaticInsertionSummary() {
  std::ostringstream out;
  // M262-C002 ARC automatic-insertion anchor: lane-C now consumes the
  // existing ARC semantic insertion flags for the supported runnable slice so
  // ordinary function and method lowering materializes retain/release/
  // autorelease helper calls instead of publishing summary-only intent.
  out << "contract=" << kObjc3ArcAutomaticInsertionContractId
      << ";source_model=" << kObjc3ArcAutomaticInsertionSourceModel
      << ";lowering_model=" << kObjc3ArcAutomaticInsertionLoweringModel
      << ";arc_mode_contract=" << Expr::kObjc3ArcModeHandlingContractId
      << ";arc_inference_contract=" << Expr::kObjc3ArcInferenceLifetimeContractId
      << ";arc_interaction_contract=" << Expr::kObjc3ArcInteractionSemanticsContractId
      << ";arc_cleanup_contract=" << kObjc3ArcLoweringAbiCleanupModelContractId
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";autorelease_symbol=" << kObjc3RuntimeAutoreleaseI32Symbol
      << ";fail_closed_model=" << kObjc3ArcAutomaticInsertionFailureModel
      << ";non_goal_model=" << kObjc3ArcAutomaticInsertionNonGoalModel
      << ";next_issue=M262-C003";
  return out.str();
}

std::string Objc3ArcCleanupWeakLifetimeHooksSummary() {
  std::ostringstream out;
  // M262-C003 ARC cleanup/weak/lifetime lowering anchor: lane-C now widens the
  // supported ARC slice with scope-aware cleanup emission, the retained weak
  // current-property helper path, and deterministic block-capture lifetime
  // cleanup without claiming generalized weak-local or exception-driven ARC.
  out << "contract=" << kObjc3ArcCleanupWeakLifetimeHooksContractId
      << ";source_model=" << kObjc3ArcCleanupWeakLifetimeHooksSourceModel
      << ";lowering_model=" << kObjc3ArcCleanupWeakLifetimeHooksLoweringModel
      << ";arc_mode_contract=" << Expr::kObjc3ArcModeHandlingContractId
      << ";arc_interaction_contract="
      << Expr::kObjc3ArcInteractionSemanticsContractId
      << ";arc_cleanup_contract=" << kObjc3ArcLoweringAbiCleanupModelContractId
      << ";arc_insertion_contract=" << kObjc3ArcAutomaticInsertionContractId
      << ";weak_load_symbol=" << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
      << ";weak_store_symbol="
      << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";fail_closed_model="
      << kObjc3ArcCleanupWeakLifetimeHooksFailureModel
      << ";non_goal_model=" << kObjc3ArcCleanupWeakLifetimeHooksNonGoalModel
      << ";next_issue=M262-C004";
  return out.str();
}

std::string Objc3ExecutableMethodBodyBindingSummary() {
  std::ostringstream out;
  // M256-C002 executable method-body binding implementation anchor: lane-C
  // upgrades the frozen C001 surface into a fail-closed runtime capability by
  // requiring every implementation-owned executable method entry to bind to
  // exactly one concrete LLVM definition symbol before the object artifact is
  // accepted.
  out << "contract=" << kObjc3ExecutableMethodBodyBindingContractId
      << ";source_model=" << kObjc3ExecutableMethodBodyBindingSourceModel
      << ";runtime_model=" << kObjc3ExecutableMethodBodyBindingRuntimeModel
      << ";fail_closed_model="
      << kObjc3ExecutableMethodBodyBindingFailClosedModel
      << ";scope_model=" << kObjc3ExecutableObjectArtifactLoweringScopeModel;
  return out.str();
}

std::string Objc3ExecutableRealizationRecordsSummary() {
  std::ostringstream out;
  // M256-C003 executable realization-record expansion anchor: emitted
  // class/protocol/category records now preserve the owner and graph edges that
  // D-lane runtime realization will consume directly. Parser/sema still own the
  // legality and canonical identities; lowering only serializes that closure
  // into stable artifact layouts.
  out << "contract=" << kObjc3ExecutableRealizationRecordsContractId
      << ";class_record_model="
      << kObjc3ExecutableRealizationClassRecordModel
      << ";protocol_record_model="
      << kObjc3ExecutableRealizationProtocolRecordModel
      << ";category_record_model="
      << kObjc3ExecutableRealizationCategoryRecordModel
      << ";fail_closed_model="
      << kObjc3ExecutableRealizationFailClosedModel
      << ";scope_model=" << kObjc3ExecutableObjectArtifactLoweringScopeModel;
  return out.str();
}

std::string Objc3RuntimeClassRealizationSummary() {
  std::ostringstream out;
  // M256-D001 class-realization-runtime freeze anchor: the current runtime
  // consumes emitted realization records directly, walks the class/metaclass
  // graph deterministically, attaches preferred category implementation
  // records after bundle selection, and uses protocol records only as
  // declaration-aware negative lookup evidence. Property/ivar storage and
  // executable protocol bodies remain outside this runtime boundary.
  out << "contract=" << kObjc3RuntimeClassRealizationContractId
      << ";class_realization_model=" << kObjc3RuntimeClassRealizationModel
      << ";metaclass_graph_model=" << kObjc3RuntimeMetaclassGraphModel
      << ";category_attachment_model="
      << kObjc3RuntimeClassRealizationCategoryAttachmentModel
      << ";protocol_check_model=" << kObjc3RuntimeProtocolCheckModel
      << ";fail_closed_model="
      << kObjc3RuntimeClassRealizationFailClosedModel
      << ";non_goals=no-property-storage-no-ivar-layout-no-protocol-body-dispatch";
  return out.str();
}

std::string Objc3RuntimeMetaclassGraphRootClassSummary() {
  std::ostringstream out;
  // M256-D002 metaclass-graph-root-class anchor: runtime now publishes a
  // realized class graph keyed by stable receiver base identities, preserves
  // root classes as explicit nodes with null superclass links, and keeps
  // known-class/class-self dispatch on the same metaclass graph without
  // widening the public runtime ABI. Allocation, instance storage, and
  // protocol-body execution remain outside this boundary.
  out << "contract=" << kObjc3RuntimeMetaclassGraphRootClassContractId
      << ";realized_class_graph_model="
      << kObjc3RuntimeRealizedClassGraphModel
      << ";root_class_baseline_model="
      << kObjc3RuntimeRootClassBaselineModel
      << ";fail_closed_model="
      << kObjc3RuntimeRealizedClassGraphFailClosedModel
      << ";non_goals=no-allocation-no-instance-storage-no-protocol-body-dispatch";
  return out.str();
}

std::string Objc3RuntimeCategoryAttachmentProtocolConformanceSummary() {
  std::ostringstream out;
  // M256-D003 category-attachment-protocol-conformance anchor: runtime-owned
  // realized class nodes now retain preferred category attachments and answer
  // protocol conformance queries from emitted class/category protocol refs
  // without rediscovering source legality or widening the public ABI.
  out << "contract="
      << kObjc3RuntimeCategoryAttachmentProtocolConformanceContractId
      << ";category_attachment_model="
      << kObjc3RuntimeCategoryAttachmentRealizedGraphModel
      << ";protocol_conformance_query_model="
      << kObjc3RuntimeProtocolConformanceQueryModel
      << ";fail_closed_model="
      << kObjc3RuntimeAttachmentConformanceFailClosedModel
      << ";non_goals=no-allocation-no-property-storage-no-cross-image-attachment";
  return out.str();
}

std::string Objc3RuntimeCanonicalRunnableObjectSampleSupportSummary() {
  std::ostringstream out;
  // M256-D004 canonical-runnable-object-sample anchor: runtime-owned builtin
  // alloc/new/init resolution now closes the smallest truthful executable
  // object sample while metadata-rich object-model behavior stays proven
  // through paired library/probe evidence instead of pretending the runtime
  // export gate is already open for every executable sample shape.
  out << "contract="
      << kObjc3RuntimeCanonicalRunnableObjectSampleSupportContractId
      << ";execution_model="
      << kObjc3RuntimeCanonicalRunnableObjectExecutionModel
      << ";probe_split_model="
      << kObjc3RuntimeCanonicalRunnableObjectProbeSplitModel
      << ";fail_closed_model="
      << kObjc3RuntimeCanonicalRunnableObjectFailClosedModel
      << ";non_goals=no-property-storage-no-ivar-layout-no-metadata-heavy-executable-export-bypass";
  return out.str();
}

std::string Objc3RuntimeMetadataBinaryInspectionHarnessSummary() {
  std::ostringstream out;
  // M253-C006 binary inspection harness expansion anchor: lane-C now proves
  // emitted metadata sections structurally through one shared llvm-readobj and
  // llvm-objdump corpus. The positive corpus covers scaffold-only, class-heavy,
  // category-heavy, and selector-pool-heavy objects, while the negative corpus
  // remains fail-closed when semantic validation prevents object emission.
  out << "contract=" << kObjc3RuntimeBinaryInspectionHarnessContractId
      << ";positive_corpus_model="
      << kObjc3RuntimeBinaryInspectionPositiveCorpusModel
      << ";negative_corpus_model="
      << kObjc3RuntimeBinaryInspectionNegativeCorpusModel
      << ";section_inventory_command="
      << kObjc3RuntimeBinaryInspectionSectionCommand
      << ";symbol_inventory_command="
      << kObjc3RuntimeBinaryInspectionSymbolCommand
      << ";non_goals=no-new-metadata-families-or-runtime-registration";
  return out.str();
}

std::string Objc3RuntimeMetadataObjectPackagingRetentionSummary() {
  std::ostringstream out;
  // M253-D001 object-packaging/retention freeze anchor: lane-D now freezes the
  // current produced-object handoff as module.obj plus retained aggregate
  // symbols. Later archive/link/startup-registration work must preserve this
  // boundary and may not silently replace llvm.used or aggregate symbol roots.
  out << "contract=" << kObjc3RuntimeObjectPackagingRetentionContractId
      << ";boundary_model="
      << kObjc3RuntimeObjectPackagingRetentionBoundaryModel
      << ";retention_anchor_model="
      << kObjc3RuntimeObjectPackagingRetentionAnchorModel
      << ";object_artifact=" << kObjc3RuntimeObjectPackagingRetentionArtifact
      << ";aggregate_symbol_prefix="
      << kObjc3RuntimeObjectPackagingRetentionSymbolPrefix
      << ";non_goals=no-archive-packaging-link-registration-or-startup-bootstrap";
  return out.str();
}

std::string Objc3RuntimeMetadataLinkerRetentionSummary() {
  std::ostringstream out;
  // M253-D002 linker-retention/dead-strip resistance anchor: lane-D adds one
  // real public linker anchor and one public discovery root, together with a
  // driver response-file payload that can force-retain the archived object that
  // owns the metadata sections. Multi-archive and multi-TU edge cases remain
  // deferred until M253-D003.
  out << "contract=" << kObjc3RuntimeLinkerRetentionContractId
      << ";anchor_model=" << kObjc3RuntimeLinkerRetentionAnchorModel
      << ";discovery_model=" << kObjc3RuntimeLinkerDiscoveryModel
      << ";linker_anchor_logical_section="
      << kObjc3RuntimeLinkerAnchorLogicalSection
      << ";discovery_root_logical_section="
      << kObjc3RuntimeLinkerDiscoveryRootLogicalSection
      << ";linker_response_artifact_suffix="
      << kObjc3RuntimeLinkerResponseArtifactSuffix
      << ";discovery_artifact_suffix="
      << kObjc3RuntimeLinkerDiscoveryArtifactSuffix
      << ";coff_flag_model=" << kObjc3RuntimeLinkerRetentionCoffFlagModel
      << ";elf_flag_model=" << kObjc3RuntimeLinkerRetentionElfFlagModel
      << ";mach_o_flag_model=" << kObjc3RuntimeLinkerRetentionMachOFlagModel
      << ";non_goals=no-multi-archive-fan-in-or-cross-translation-unit-anchor-merging";
  return out.str();
}

std::string Objc3RuntimeMetadataArchiveStaticLinkDiscoverySummary() {
  std::ostringstream out;
  // M253-D003 archive/static-link discovery anchor: lane-D extends the D002
  // object-level discovery path with translation-unit-stable public anchors and
  // one merged discovery/response artifact pair that downstream archive/static
  // link orchestration can consume deterministically across multiple TUs.
  out << "contract=" << kObjc3RuntimeArchiveStaticLinkDiscoveryContractId
      << ";anchor_seed_model="
      << kObjc3RuntimeArchiveStaticLinkAnchorSeedModel
      << ";translation_unit_identity_model="
      << kObjc3RuntimeArchiveStaticLinkTranslationUnitIdentityModel
      << ";merge_model=" << kObjc3RuntimeArchiveStaticLinkMergeModel
      << ";merged_linker_response_artifact_suffix="
      << kObjc3RuntimeMergedLinkerResponseArtifactSuffix
      << ";merged_discovery_artifact_suffix="
      << kObjc3RuntimeMergedDiscoveryArtifactSuffix
      << ";non_goals=no-runtime-registration-or-startup-bootstrap";
  return out.str();
}

std::string Objc3RuntimeBootstrapLoweringBoundarySummary() {
  std::ostringstream out;
  // M263-C001 constructor-root/init-array lowering freeze anchor: the live
  // lowering path already materializes ctor-root/global_ctors/init-stub/
  // registration-table/image-local-init globals. This summary is the
  // canonical replay-stable description of that current boundary and the
  // registration-descriptor handoff it consumes.
  out << "contract=" << kObjc3RuntimeBootstrapLoweringContractId
      << ";boundary_model=" << kObjc3RuntimeBootstrapLoweringBoundaryModel
      << ";registration_descriptor_handoff_contract_id="
      << kObjc3RuntimeBootstrapRegistrationDescriptorHandoffContractId
      << ";registration_descriptor_artifact="
      << kObjc3RuntimeBootstrapRegistrationDescriptorArtifact
      << ";registration_descriptor_handoff_model="
      << kObjc3RuntimeBootstrapRegistrationDescriptorHandoffModel
      << ";constructor_root_symbol="
      << kObjc3RuntimeTranslationUnitRegistrationConstructorRootSymbol
      << ";init_stub_symbol_prefix="
      << kObjc3RuntimeTranslationUnitRegistrationInitStubSymbolPrefix
      << ";registration_table_symbol_prefix="
      << kObjc3RuntimeBootstrapRegistrationTableSymbolPrefix
      << ";registration_entrypoint_symbol="
      << kObjc3RuntimeTranslationUnitRegistrationEntryPointSymbol
      << ";global_ctor_list_model="
      << kObjc3RuntimeBootstrapGlobalCtorListModel
      << ";constructor_root_emission_state="
      << kObjc3RuntimeBootstrapConstructorRootEmissionState
      << ";init_stub_emission_state="
      << kObjc3RuntimeBootstrapInitStubEmissionState
      << ";registration_table_emission_state="
      << kObjc3RuntimeBootstrapRegistrationTableEmissionState
      << ";non_goals=no-multi-image-root-fanout-no-runtime-replay-partitioning-no-late-linker-synthesis";
  return out.str();
}

std::string Objc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringSummary() {
  std::ostringstream out;
  // M263-C002 registration-descriptor/image-root lowering anchor: this
  // summary freezes the first live binary lowering surface for the source
  // identifiers from M263-A002. The IR/object path now materializes dedicated
  // registration-descriptor and image-root globals in their own sections
  // rather than treating those identifiers as sidecar-only metadata.
  out << "contract="
      << kObjc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringContractId
      << ";lowering_model="
      << kObjc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringModel
      << ";registration_descriptor_logical_section="
      << kObjc3RuntimeBootstrapRegistrationDescriptorLogicalSection
      << ";image_root_logical_section="
      << kObjc3RuntimeBootstrapImageRootLogicalSection
      << ";registration_descriptor_symbol_prefix="
      << kObjc3RuntimeBootstrapRegistrationDescriptorSymbolPrefix
      << ";image_root_symbol_prefix="
      << kObjc3RuntimeBootstrapImageRootSymbolPrefix
      << ";registration_descriptor_payload_model="
      << kObjc3RuntimeBootstrapRegistrationDescriptorPayloadModel
      << ";image_root_payload_model="
      << kObjc3RuntimeBootstrapImageRootPayloadModel
      << ";non_goals=no-cross-translation-unit-root-deduplication-or-runtime-fanout-merge";
  return out.str();
}

std::string Objc3RuntimeBootstrapArchiveStaticLinkReplayCorpusSummary() {
  std::ostringstream out;
  // M263-C003 archive/static-link bootstrap replay corpus anchor: this
  // summary binds the M253-D003 retained archive/static-link discovery path to
  // the live M263-B003 bootstrap reset/replay runtime so validation can prove
  // real startup registration and replay behavior over linked archives.
  out << "contract="
      << kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusContractId
      << ";corpus_model="
      << kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusModel
      << ";binary_proof_model="
      << kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusBinaryProofModel
      << ";archive_static_link_discovery_contract_id="
      << kObjc3RuntimeArchiveStaticLinkDiscoveryContractId
      << ";bootstrap_failure_restart_contract_id="
      << kObjc3BootstrapFailureRestartSemanticsContractId
      << ";registration_descriptor_lowering_contract_id="
      << kObjc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringContractId
      << ";replay_registered_images_symbol="
      << kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol
      << ";reset_replay_state_snapshot_symbol="
      << kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol
      << ";non_goals=no-new-bootstrap-runtime-entrypoints-or-linker-merge-models";
  return out.str();
}

std::string Objc3RuntimeMetadataEmissionGateSummary() {
  std::ostringstream out;
  // M253-E001 metadata-emission gate anchor: lane-E freezes one fail-closed
  // evidence chain over the implemented A002/B003/C006/D003 summaries before
  // cross-lane closeout begins, so later work must preserve the same source
  // matrix, object-format policy, binary inspection corpus, and archive/static
  // link discovery proofs instead of redefining the gate ad hoc.
  out << "contract=" << kObjc3RuntimeMetadataEmissionGateContractId
      << ";evidence_model=" << kObjc3RuntimeMetadataEmissionGateEvidenceModel
      << ";failure_model=" << kObjc3RuntimeMetadataEmissionGateFailureModel
      << ";non_goals=no-new-emission-families-or-runtime-registration";
  return out.str();
}

std::string Objc3RuntimeMetadataObjectEmissionCloseoutSummary() {
  std::ostringstream out;
  // M253-E002 cross-lane object-emission closeout anchor: lane-E extends the
  // E001 summary chain with fresh integrated native object probes so class,
  // category, and message-send outputs all prove the same source graph,
  // object-format policy, binary inspection, and linker/discovery continuity
  // before later startup registration work begins.
  out << "contract=" << kObjc3RuntimeMetadataObjectEmissionCloseoutContractId
      << ";evidence_model="
      << kObjc3RuntimeMetadataObjectEmissionCloseoutEvidenceModel
      << ";failure_model="
      << kObjc3RuntimeMetadataObjectEmissionCloseoutFailureModel
      << ";non_goals=no-startup-registration-or-runtime-bootstrap";
  return out.str();
}

std::string Objc3RuntimeMetadataSectionForObjectFormat(
    const std::string &object_format, const std::string &logical_section) {
  return MapRuntimeMetadataSectionForObjectFormat(object_format, logical_section);
}

std::string Objc3RuntimeMetadataDriverLinkerRetentionFlagForObjectFormat(
    const std::string &object_format, const std::string &symbol_name) {
  return BuildRuntimeMetadataDriverLinkerRetentionFlagForObjectFormat(
      object_format, symbol_name);
}

std::string Objc3RuntimeMetadataHostSectionForLogicalName(
    const std::string &logical_section) {
  return MapRuntimeMetadataSectionForObjectFormat(HostRuntimeMetadataObjectFormat(),
                                                  logical_section);
}

std::string Objc3RuntimeDispatchDeclarationReplayKey(const Objc3LoweringIRBoundary &boundary) {
  std::ostringstream out;
  out << "declare i32 @" << boundary.runtime_dispatch_symbol << "(i32, ptr";
  for (std::size_t i = 0; i < boundary.runtime_dispatch_arg_slots; ++i) {
    out << ", i32";
  }
  out << ")";
  return out.str();
}

bool TryGetCompoundAssignmentBinaryOpcode(const std::string &op, std::string &opcode) {
  if (op == "+=") {
    opcode = "add";
    return true;
  }
  if (op == "-=") {
    opcode = "sub";
    return true;
  }
  if (op == "*=") {
    opcode = "mul";
    return true;
  }
  if (op == "/=") {
    opcode = "sdiv";
    return true;
  }
  if (op == "%=") {
    opcode = "srem";
    return true;
  }
  if (op == "&=") {
    opcode = "and";
    return true;
  }
  if (op == "|=") {
    opcode = "or";
    return true;
  }
  if (op == "^=") {
    opcode = "xor";
    return true;
  }
  if (op == "<<=") {
    opcode = "shl";
    return true;
  }
  if (op == ">>=") {
    opcode = "ashr";
    return true;
  }
  return false;
}

bool TryParseObjc3AtomicMemoryOrder(const std::string &token, Objc3AtomicMemoryOrder &order) {
  if (token == kObjc3AtomicMemoryOrderRelaxed) {
    order = Objc3AtomicMemoryOrder::Relaxed;
    return true;
  }
  if (token == kObjc3AtomicMemoryOrderAcquire) {
    order = Objc3AtomicMemoryOrder::Acquire;
    return true;
  }
  if (token == kObjc3AtomicMemoryOrderRelease) {
    order = Objc3AtomicMemoryOrder::Release;
    return true;
  }
  if (token == kObjc3AtomicMemoryOrderAcqRel || token == "acquire_release") {
    order = Objc3AtomicMemoryOrder::AcqRel;
    return true;
  }
  if (token == kObjc3AtomicMemoryOrderSeqCst) {
    order = Objc3AtomicMemoryOrder::SeqCst;
    return true;
  }
  return false;
}

const char *Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder order) {
  switch (order) {
    case Objc3AtomicMemoryOrder::Relaxed:
      return "monotonic";
    case Objc3AtomicMemoryOrder::Acquire:
      return "acquire";
    case Objc3AtomicMemoryOrder::Release:
      return "release";
    case Objc3AtomicMemoryOrder::AcqRel:
      return "acq_rel";
    case Objc3AtomicMemoryOrder::SeqCst:
    default:
      return "seq_cst";
  }
}

std::string Objc3AtomicMemoryOrderMappingReplayKey() {
  return std::string(AtomicMemoryOrderToken(Objc3AtomicMemoryOrder::Relaxed)) + "=" +
         Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder::Relaxed) + ";" +
         std::string(AtomicMemoryOrderToken(Objc3AtomicMemoryOrder::Acquire)) + "=" +
         Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder::Acquire) + ";" +
         std::string(AtomicMemoryOrderToken(Objc3AtomicMemoryOrder::Release)) + "=" +
         Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder::Release) + ";" +
         std::string(AtomicMemoryOrderToken(Objc3AtomicMemoryOrder::AcqRel)) + "=" +
         Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder::AcqRel) + ";" +
         std::string(AtomicMemoryOrderToken(Objc3AtomicMemoryOrder::SeqCst)) + "=" +
         Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder::SeqCst);
}

bool IsSupportedObjc3SimdVectorLaneCount(unsigned lane_count) {
  return lane_count == 2u || lane_count == 4u || lane_count == 8u || lane_count == 16u;
}

bool TryBuildObjc3SimdVectorLLVMType(const std::string &base_spelling, unsigned lane_count, std::string &llvm_type) {
  if (!IsSupportedVectorBaseSpelling(base_spelling) || !IsSupportedObjc3SimdVectorLaneCount(lane_count)) {
    return false;
  }

  llvm_type = "<" + std::to_string(lane_count) + " x " +
              (base_spelling == kObjc3SimdVectorBaseBool ? "i1" : "i32") + ">";
  return true;
}

std::string Objc3SimdVectorTypeLoweringReplayKey() {
  std::string replay_key;
  const std::string base_spellings[2] = {kObjc3SimdVectorBaseI32, kObjc3SimdVectorBaseBool};
  const unsigned lane_counts[4] = {2u, 4u, 8u, 16u};
  bool first = true;

  for (const std::string &base_spelling : base_spellings) {
    for (const unsigned lane_count : lane_counts) {
      std::string llvm_type;
      if (!TryBuildObjc3SimdVectorLLVMType(base_spelling, lane_count, llvm_type)) {
        continue;
      }
      if (!first) {
        replay_key += ";";
      }
      replay_key += VectorTypeSpelling(base_spelling, lane_count) + "=" + llvm_type;
      first = false;
    }
  }

  replay_key += ";lane_contract=";
  replay_key += kObjc3SimdVectorLaneContract;
  return replay_key;
}

bool IsValidObjc3MethodLookupOverrideConflictContract(const Objc3MethodLookupOverrideConflictContract &contract) {
  if (contract.method_lookup_hits > contract.method_lookup_sites ||
      contract.method_lookup_misses > contract.method_lookup_sites ||
      contract.method_lookup_hits + contract.method_lookup_misses != contract.method_lookup_sites) {
    return false;
  }
  if (contract.override_lookup_hits > contract.override_lookup_sites ||
      contract.override_lookup_misses > contract.override_lookup_sites ||
      contract.override_lookup_hits + contract.override_lookup_misses != contract.override_lookup_sites) {
    return false;
  }
  if (contract.override_conflicts > contract.override_lookup_hits) {
    return false;
  }
  if (contract.unresolved_base_interfaces > contract.override_lookup_misses) {
    return false;
  }
  return true;
}

std::string Objc3MethodLookupOverrideConflictReplayKey(const Objc3MethodLookupOverrideConflictContract &contract) {
  return std::string("method_lookup_sites=") + std::to_string(contract.method_lookup_sites) +
         ";method_lookup_hits=" + std::to_string(contract.method_lookup_hits) +
         ";method_lookup_misses=" + std::to_string(contract.method_lookup_misses) +
         ";override_lookup_sites=" + std::to_string(contract.override_lookup_sites) +
         ";override_lookup_hits=" + std::to_string(contract.override_lookup_hits) +
         ";override_lookup_misses=" + std::to_string(contract.override_lookup_misses) +
         ";override_conflicts=" + std::to_string(contract.override_conflicts) +
         ";unresolved_base_interfaces=" + std::to_string(contract.unresolved_base_interfaces) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3MethodLookupOverrideConflictLaneContract;
}

Objc3PropertySynthesisIvarBindingContract Objc3DefaultPropertySynthesisIvarBindingContract(
    std::size_t property_synthesis_sites,
    bool deterministic) {
  Objc3PropertySynthesisIvarBindingContract contract;
  contract.property_synthesis_sites = property_synthesis_sites;
  contract.property_synthesis_explicit_ivar_bindings = 0;
  contract.property_synthesis_default_ivar_bindings = property_synthesis_sites;
  contract.ivar_binding_sites = property_synthesis_sites;
  contract.ivar_binding_resolved = property_synthesis_sites;
  contract.ivar_binding_missing = 0;
  contract.ivar_binding_conflicts = 0;
  contract.deterministic = deterministic;
  return contract;
}

bool IsValidObjc3PropertySynthesisIvarBindingContract(
    const Objc3PropertySynthesisIvarBindingContract &contract) {
  if (contract.property_synthesis_explicit_ivar_bindings +
              contract.property_synthesis_default_ivar_bindings !=
          contract.property_synthesis_sites ||
      contract.property_synthesis_explicit_ivar_bindings > contract.property_synthesis_sites ||
      contract.property_synthesis_default_ivar_bindings > contract.property_synthesis_sites) {
    return false;
  }
  if (contract.ivar_binding_sites != contract.property_synthesis_sites) {
    return false;
  }
  if (contract.ivar_binding_resolved > contract.ivar_binding_sites ||
      contract.ivar_binding_missing > contract.ivar_binding_sites ||
      contract.ivar_binding_conflicts > contract.ivar_binding_sites ||
      contract.ivar_binding_resolved + contract.ivar_binding_missing + contract.ivar_binding_conflicts !=
          contract.ivar_binding_sites) {
    return false;
  }
  return true;
}

std::string Objc3PropertySynthesisIvarBindingReplayKey(
    const Objc3PropertySynthesisIvarBindingContract &contract) {
  return std::string("property_synthesis_sites=") + std::to_string(contract.property_synthesis_sites) +
         ";property_synthesis_explicit_ivar_bindings=" +
         std::to_string(contract.property_synthesis_explicit_ivar_bindings) +
         ";property_synthesis_default_ivar_bindings=" +
         std::to_string(contract.property_synthesis_default_ivar_bindings) +
         ";ivar_binding_sites=" + std::to_string(contract.ivar_binding_sites) +
         ";ivar_binding_resolved=" + std::to_string(contract.ivar_binding_resolved) +
         ";ivar_binding_missing=" + std::to_string(contract.ivar_binding_missing) +
         ";ivar_binding_conflicts=" + std::to_string(contract.ivar_binding_conflicts) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3PropertySynthesisIvarBindingLaneContract;
}

bool IsValidObjc3IdClassSelObjectPointerTypecheckContract(
    const Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  const std::size_t computed_total = contract.id_typecheck_sites +
                                     contract.class_typecheck_sites +
                                     contract.sel_typecheck_sites +
                                     contract.object_pointer_typecheck_sites;
  return contract.total_typecheck_sites == computed_total;
}

std::string Objc3IdClassSelObjectPointerTypecheckReplayKey(
    const Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  return std::string("id_typecheck_sites=") + std::to_string(contract.id_typecheck_sites) +
         ";class_typecheck_sites=" + std::to_string(contract.class_typecheck_sites) +
         ";sel_typecheck_sites=" + std::to_string(contract.sel_typecheck_sites) +
         ";object_pointer_typecheck_sites=" + std::to_string(contract.object_pointer_typecheck_sites) +
         ";total_typecheck_sites=" + std::to_string(contract.total_typecheck_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3IdClassSelObjectPointerTypecheckLaneContract;
}

bool IsValidObjc3DispatchSurfaceClassificationContract(
    const Objc3DispatchSurfaceClassificationContract &contract) {
  const bool live_runtime_bindings_ok =
      contract.instance_entrypoint_family == kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily &&
      contract.class_entrypoint_family == kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily &&
      contract.super_entrypoint_family == kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily &&
      contract.dynamic_entrypoint_family == kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily;
  return live_runtime_bindings_ok &&
         contract.direct_entrypoint_family == kObjc3DispatchSurfaceDirectDispatchBinding;
}

std::string Objc3DispatchSurfaceClassificationReplayKey(
    const Objc3DispatchSurfaceClassificationContract &contract) {
  return std::string("instance_dispatch_sites=") +
             std::to_string(contract.instance_dispatch_sites) +
         ";class_dispatch_sites=" + std::to_string(contract.class_dispatch_sites) +
         ";super_dispatch_sites=" + std::to_string(contract.super_dispatch_sites) +
         ";direct_dispatch_sites=" + std::to_string(contract.direct_dispatch_sites) +
         ";dynamic_dispatch_sites=" + std::to_string(contract.dynamic_dispatch_sites) +
         ";instance_entrypoint_family=" + contract.instance_entrypoint_family +
         ";class_entrypoint_family=" + contract.class_entrypoint_family +
         ";super_entrypoint_family=" + contract.super_entrypoint_family +
         ";direct_entrypoint_family=" + contract.direct_entrypoint_family +
         ";dynamic_entrypoint_family=" + contract.dynamic_entrypoint_family +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";contract_id=" + kObjc3DispatchSurfaceClassificationContractId;
}

bool IsValidObjc3MessageSendSelectorLoweringContract(
    const Objc3MessageSendSelectorLoweringContract &contract) {
  if (contract.unary_selector_sites + contract.keyword_selector_sites != contract.message_send_sites) {
    return false;
  }
  if (contract.receiver_expression_sites != contract.message_send_sites) {
    return false;
  }
  if (contract.selector_piece_sites < contract.message_send_sites) {
    return false;
  }
  if (contract.argument_expression_sites < contract.keyword_selector_sites) {
    return false;
  }
  if (contract.selector_literal_entries > contract.message_send_sites) {
    return false;
  }
  if (contract.selector_literal_entries == 0 && contract.selector_literal_characters != 0) {
    return false;
  }
  return true;
}

std::string Objc3MessageSendSelectorLoweringReplayKey(
    const Objc3MessageSendSelectorLoweringContract &contract) {
  return std::string("message_send_sites=") + std::to_string(contract.message_send_sites) +
         ";unary_selector_sites=" + std::to_string(contract.unary_selector_sites) +
         ";keyword_selector_sites=" + std::to_string(contract.keyword_selector_sites) +
         ";selector_piece_sites=" + std::to_string(contract.selector_piece_sites) +
         ";argument_expression_sites=" + std::to_string(contract.argument_expression_sites) +
         ";receiver_expression_sites=" + std::to_string(contract.receiver_expression_sites) +
         ";selector_literal_entries=" + std::to_string(contract.selector_literal_entries) +
         ";selector_literal_characters=" + std::to_string(contract.selector_literal_characters) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3MessageSendSelectorLoweringLaneContract;
}

bool IsValidObjc3DispatchAbiMarshallingContract(
    const Objc3DispatchAbiMarshallingContract &contract) {
  const std::size_t expected_argument_total =
      contract.message_send_sites * contract.runtime_dispatch_arg_slots;
  if (contract.receiver_slots_marshaled != contract.message_send_sites ||
      contract.selector_slots_marshaled != contract.message_send_sites) {
    return false;
  }
  if (contract.argument_total_slots_marshaled != expected_argument_total) {
    return false;
  }
  if (contract.argument_value_slots_marshaled > contract.argument_total_slots_marshaled) {
    return false;
  }
  if (contract.argument_padding_slots_marshaled + contract.argument_value_slots_marshaled !=
      contract.argument_total_slots_marshaled) {
    return false;
  }
  const std::size_t expected_total = contract.receiver_slots_marshaled +
                                     contract.selector_slots_marshaled +
                                     contract.argument_total_slots_marshaled;
  return contract.total_marshaled_slots == expected_total;
}

std::string Objc3DispatchAbiMarshallingReplayKey(
    const Objc3DispatchAbiMarshallingContract &contract) {
  return std::string("message_send_sites=") + std::to_string(contract.message_send_sites) +
         ";receiver_slots_marshaled=" + std::to_string(contract.receiver_slots_marshaled) +
         ";selector_slots_marshaled=" + std::to_string(contract.selector_slots_marshaled) +
         ";argument_value_slots_marshaled=" + std::to_string(contract.argument_value_slots_marshaled) +
         ";argument_padding_slots_marshaled=" + std::to_string(contract.argument_padding_slots_marshaled) +
         ";argument_total_slots_marshaled=" + std::to_string(contract.argument_total_slots_marshaled) +
         ";total_marshaled_slots=" + std::to_string(contract.total_marshaled_slots) +
         ";runtime_dispatch_arg_slots=" + std::to_string(contract.runtime_dispatch_arg_slots) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3DispatchAbiMarshallingLaneContract;
}

bool IsValidObjc3NilReceiverSemanticsFoldabilityContract(
    const Objc3NilReceiverSemanticsFoldabilityContract &contract) {
  if (contract.receiver_nil_literal_sites != contract.nil_receiver_semantics_enabled_sites) {
    return false;
  }
  if (contract.nil_receiver_foldable_sites > contract.nil_receiver_semantics_enabled_sites) {
    return false;
  }
  if (contract.nil_receiver_runtime_dispatch_required_sites + contract.nil_receiver_foldable_sites !=
      contract.message_send_sites) {
    return false;
  }
  if (contract.nil_receiver_semantics_enabled_sites + contract.non_nil_receiver_sites !=
      contract.message_send_sites) {
    return false;
  }
  return contract.contract_violation_sites <= contract.message_send_sites;
}

std::string Objc3NilReceiverSemanticsFoldabilityReplayKey(
    const Objc3NilReceiverSemanticsFoldabilityContract &contract) {
  return std::string("message_send_sites=") + std::to_string(contract.message_send_sites) +
         ";receiver_nil_literal_sites=" + std::to_string(contract.receiver_nil_literal_sites) +
         ";nil_receiver_semantics_enabled_sites=" + std::to_string(contract.nil_receiver_semantics_enabled_sites) +
         ";nil_receiver_foldable_sites=" + std::to_string(contract.nil_receiver_foldable_sites) +
         ";nil_receiver_runtime_dispatch_required_sites=" +
         std::to_string(contract.nil_receiver_runtime_dispatch_required_sites) +
         ";non_nil_receiver_sites=" + std::to_string(contract.non_nil_receiver_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3NilReceiverSemanticsFoldabilityLaneContract;
}

bool IsValidObjc3SuperDispatchMethodFamilyContract(
    const Objc3SuperDispatchMethodFamilyContract &contract) {
  if (contract.receiver_super_identifier_sites != contract.super_dispatch_enabled_sites) {
    return false;
  }
  if (contract.super_dispatch_requires_class_context_sites != contract.super_dispatch_enabled_sites) {
    return false;
  }
  if (contract.method_family_init_sites + contract.method_family_copy_sites +
              contract.method_family_mutable_copy_sites + contract.method_family_new_sites +
              contract.method_family_none_sites !=
          contract.message_send_sites) {
    return false;
  }
  if (contract.method_family_returns_related_result_sites > contract.method_family_init_sites) {
    return false;
  }
  if (contract.method_family_returns_retained_result_sites > contract.message_send_sites) {
    return false;
  }
  return contract.contract_violation_sites <= contract.message_send_sites;
}

std::string Objc3SuperDispatchMethodFamilyReplayKey(
    const Objc3SuperDispatchMethodFamilyContract &contract) {
  return std::string("message_send_sites=") + std::to_string(contract.message_send_sites) +
         ";receiver_super_identifier_sites=" + std::to_string(contract.receiver_super_identifier_sites) +
         ";super_dispatch_enabled_sites=" + std::to_string(contract.super_dispatch_enabled_sites) +
         ";super_dispatch_requires_class_context_sites=" +
         std::to_string(contract.super_dispatch_requires_class_context_sites) +
         ";method_family_init_sites=" + std::to_string(contract.method_family_init_sites) +
         ";method_family_copy_sites=" + std::to_string(contract.method_family_copy_sites) +
         ";method_family_mutable_copy_sites=" + std::to_string(contract.method_family_mutable_copy_sites) +
         ";method_family_new_sites=" + std::to_string(contract.method_family_new_sites) +
         ";method_family_none_sites=" + std::to_string(contract.method_family_none_sites) +
         ";method_family_returns_retained_result_sites=" +
         std::to_string(contract.method_family_returns_retained_result_sites) +
         ";method_family_returns_related_result_sites=" +
         std::to_string(contract.method_family_returns_related_result_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3SuperDispatchMethodFamilyLaneContract;
}

bool IsValidObjc3RuntimeShimHostLinkContract(
    const Objc3RuntimeShimHostLinkContract &contract) {
  if (!IsValidRuntimeDispatchSymbol(contract.runtime_dispatch_symbol)) {
    return false;
  }
  if (contract.runtime_dispatch_arg_slots > kObjc3RuntimeDispatchMaxArgs) {
    return false;
  }
  if (contract.runtime_shim_required_sites > contract.message_send_sites) {
    return false;
  }
  if (contract.runtime_shim_required_sites + contract.runtime_shim_elided_sites !=
      contract.message_send_sites) {
    return false;
  }
  if (contract.runtime_dispatch_declaration_parameter_count !=
      contract.runtime_dispatch_arg_slots + 2u) {
    return false;
  }
  if (contract.default_runtime_dispatch_symbol_binding !=
      (contract.runtime_dispatch_symbol == kObjc3RuntimeDispatchSymbol)) {
    return false;
  }
  return contract.contract_violation_sites <= contract.message_send_sites;
}

std::string Objc3RuntimeShimHostLinkReplayKey(
    const Objc3RuntimeShimHostLinkContract &contract) {
  return std::string("message_send_sites=") + std::to_string(contract.message_send_sites) +
         ";runtime_shim_required_sites=" + std::to_string(contract.runtime_shim_required_sites) +
         ";runtime_shim_elided_sites=" + std::to_string(contract.runtime_shim_elided_sites) +
         ";runtime_dispatch_arg_slots=" + std::to_string(contract.runtime_dispatch_arg_slots) +
         ";runtime_dispatch_declaration_parameter_count=" +
         std::to_string(contract.runtime_dispatch_declaration_parameter_count) +
         ";runtime_dispatch_symbol=" + contract.runtime_dispatch_symbol +
         ";default_runtime_dispatch_symbol_binding=" +
         BoolToken(contract.default_runtime_dispatch_symbol_binding) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3RuntimeShimHostLinkLaneContract;
}

// M255-C001 dispatch lowering ABI freeze anchor: lane-C publishes the
// compiler-to-runtime cutover boundary here without changing executable call
// emission yet. Validation must keep the canonical runtime symbol, selector
// lookup/handle surface, i32 receiver/result ABI, and fixed four-slot padding
// model synchronized with the compatibility-bridge default target.
bool IsValidObjc3RuntimeDispatchLoweringAbiContract(
    const Objc3RuntimeDispatchLoweringAbiContract &contract) {
  if (contract.fixed_argument_slot_count > kObjc3RuntimeDispatchMaxArgs) {
    return false;
  }
  if (contract.runtime_dispatch_parameter_count !=
      contract.fixed_argument_slot_count + 2u) {
    return false;
  }
  if (contract.lowering_boundary_model !=
      kObjc3RuntimeDispatchLoweringAbiBoundaryModel) {
    return false;
  }
  if (contract.canonical_runtime_dispatch_symbol !=
      kObjc3RuntimeDispatchLoweringCanonicalEntrypointSymbol) {
    return false;
  }
  if (contract.compatibility_runtime_dispatch_symbol !=
      kObjc3RuntimeDispatchLoweringCompatibilityEntrypointSymbol) {
    return false;
  }
  if (!IsValidRuntimeDispatchSymbol(contract.default_lowering_target_symbol)) {
    return false;
  }
  if (contract.default_lowering_target_symbol !=
      contract.canonical_runtime_dispatch_symbol) {
    return false;
  }
  if (contract.selector_lookup_symbol !=
      kObjc3RuntimeDispatchLoweringSelectorLookupSymbol) {
    return false;
  }
  if (contract.selector_handle_type !=
      kObjc3RuntimeDispatchLoweringSelectorHandleType) {
    return false;
  }
  if (contract.receiver_abi_type != kObjc3RuntimeDispatchLoweringReceiverAbiType ||
      contract.selector_abi_type != kObjc3RuntimeDispatchLoweringSelectorAbiType ||
      contract.argument_abi_type != kObjc3RuntimeDispatchLoweringArgumentAbiType ||
      contract.result_abi_type != kObjc3RuntimeDispatchLoweringResultAbiType) {
    return false;
  }
  if (contract.selector_operand_model !=
          kObjc3RuntimeDispatchLoweringSelectorOperandModel ||
      contract.selector_handle_model !=
          kObjc3RuntimeDispatchLoweringSelectorHandleModel ||
      contract.argument_padding_model !=
          kObjc3RuntimeDispatchLoweringArgumentPaddingModel ||
      contract.default_lowering_target_model !=
          kObjc3RuntimeDispatchLiveCutoverDefaultTargetModel ||
      contract.compatibility_bridge_role_model !=
          kObjc3RuntimeDispatchLiveCutoverCompatibilityModel ||
      contract.deferred_cases_model !=
          kObjc3RuntimeDispatchLiveCutoverDeferredCasesModel) {
    return false;
  }
  return contract.fail_closed;
}

std::string Objc3RuntimeDispatchLoweringAbiReplayKey(
    const Objc3RuntimeDispatchLoweringAbiContract &contract) {
  return std::string("message_send_sites=") +
         std::to_string(contract.message_send_sites) +
         ";fixed_argument_slot_count=" +
         std::to_string(contract.fixed_argument_slot_count) +
         ";runtime_dispatch_parameter_count=" +
         std::to_string(contract.runtime_dispatch_parameter_count) +
         ";lowering_boundary_model=" + contract.lowering_boundary_model +
         ";canonical_runtime_dispatch_symbol=" +
         contract.canonical_runtime_dispatch_symbol +
         ";compatibility_runtime_dispatch_symbol=" +
         contract.compatibility_runtime_dispatch_symbol +
         ";default_lowering_target_symbol=" +
         contract.default_lowering_target_symbol +
         ";selector_lookup_symbol=" + contract.selector_lookup_symbol +
         ";selector_handle_type=" + contract.selector_handle_type +
         ";receiver_abi_type=" + contract.receiver_abi_type +
         ";selector_abi_type=" + contract.selector_abi_type +
         ";argument_abi_type=" + contract.argument_abi_type +
         ";result_abi_type=" + contract.result_abi_type +
         ";selector_operand_model=" + contract.selector_operand_model +
         ";selector_handle_model=" + contract.selector_handle_model +
         ";argument_padding_model=" + contract.argument_padding_model +
         ";default_lowering_target_model=" +
         contract.default_lowering_target_model +
         ";compatibility_bridge_role_model=" +
         contract.compatibility_bridge_role_model +
         ";deferred_cases_model=" + contract.deferred_cases_model +
         ";fail_closed=" + BoolToken(contract.fail_closed) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";contract_id=" + kObjc3RuntimeDispatchLoweringAbiContractId;
}

std::string Objc3RuntimeDispatchLoweringAbiBoundarySummary(
    const Objc3RuntimeDispatchLoweringAbiContract &contract) {
  return std::string("contract=") + kObjc3RuntimeDispatchLoweringAbiContractId +
         ";canonical_runtime_dispatch_symbol=" +
         contract.canonical_runtime_dispatch_symbol +
         ";compatibility_runtime_dispatch_symbol=" +
         contract.compatibility_runtime_dispatch_symbol +
         ";default_lowering_target_symbol=" +
         contract.default_lowering_target_symbol +
         ";selector_lookup_symbol=" + contract.selector_lookup_symbol +
         ";selector_handle_type=" + contract.selector_handle_type +
         ";receiver_abi_type=" + contract.receiver_abi_type +
         ";argument_abi_type=" + contract.argument_abi_type +
         ";fixed_argument_slot_count=" +
         std::to_string(contract.fixed_argument_slot_count) +
         ";result_abi_type=" + contract.result_abi_type +
         ";default_lowering_target_model=" +
         contract.default_lowering_target_model +
         ";deferred_cases_model=" + contract.deferred_cases_model;
}

bool IsValidObjc3OwnershipQualifierLoweringContract(
    const Objc3OwnershipQualifierLoweringContract &contract) {
  // M260-A001 runtime-backed-object-ownership freeze anchor: these legacy
  // ownership replay surfaces remain part of the truthful runtime-backed
  // object ownership boundary until live ARC runtime behavior lands later in
  // M260. They are preserved summary lanes, not proof that executable
  // function/method ownership qualifiers are already runnable.
  // M260-B001 retainable-object semantic-rule freeze anchor: ownership
  // qualifier replay stays authoritative for fail-closed executable storage
  // legality until live runtime-backed retain/release rules land.
  return contract.invalid_ownership_qualifier_sites <= contract.ownership_qualifier_sites &&
         contract.ownership_qualifier_sites <= contract.object_pointer_type_annotation_sites;
}

std::string Objc3OwnershipQualifierLoweringReplayKey(
    const Objc3OwnershipQualifierLoweringContract &contract) {
  return std::string("ownership_qualifier_sites=") + std::to_string(contract.ownership_qualifier_sites) +
         ";invalid_ownership_qualifier_sites=" + std::to_string(contract.invalid_ownership_qualifier_sites) +
         ";object_pointer_type_annotation_sites=" + std::to_string(contract.object_pointer_type_annotation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3OwnershipQualifierLoweringLaneContract;
}

bool IsValidObjc3RetainReleaseOperationLoweringContract(
    const Objc3RetainReleaseOperationLoweringContract &contract) {
  const std::size_t qualified_or_violation = contract.ownership_qualified_sites + contract.contract_violation_sites;
  return contract.retain_insertion_sites <= qualified_or_violation &&
         contract.release_insertion_sites <= qualified_or_violation &&
         contract.autorelease_insertion_sites <= qualified_or_violation;
}

std::string Objc3RetainReleaseOperationLoweringReplayKey(
    const Objc3RetainReleaseOperationLoweringContract &contract) {
  return std::string("ownership_qualified_sites=") + std::to_string(contract.ownership_qualified_sites) +
         ";retain_insertion_sites=" + std::to_string(contract.retain_insertion_sites) +
         ";release_insertion_sites=" + std::to_string(contract.release_insertion_sites) +
         ";autorelease_insertion_sites=" + std::to_string(contract.autorelease_insertion_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3RetainReleaseOperationLoweringLaneContract;
}

bool IsValidObjc3AutoreleasePoolScopeLoweringContract(
    const Objc3AutoreleasePoolScopeLoweringContract &contract) {
  return contract.scope_symbolized_sites <= contract.scope_sites &&
         contract.contract_violation_sites <= contract.scope_sites &&
         contract.scope_entry_transition_sites == contract.scope_sites &&
         contract.scope_exit_transition_sites == contract.scope_sites &&
         (contract.scope_sites > 0u || contract.max_scope_depth == 0u) &&
         contract.max_scope_depth <= static_cast<unsigned>(contract.scope_sites);
}

std::string Objc3AutoreleasePoolScopeLoweringReplayKey(
    const Objc3AutoreleasePoolScopeLoweringContract &contract) {
  return std::string("scope_sites=") + std::to_string(contract.scope_sites) +
         ";scope_symbolized_sites=" + std::to_string(contract.scope_symbolized_sites) +
         ";max_scope_depth=" + std::to_string(contract.max_scope_depth) +
         ";scope_entry_transition_sites=" + std::to_string(contract.scope_entry_transition_sites) +
         ";scope_exit_transition_sites=" + std::to_string(contract.scope_exit_transition_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3AutoreleasePoolScopeLoweringLaneContract;
}

bool IsValidObjc3WeakUnownedSemanticsLoweringContract(
    const Objc3WeakUnownedSemanticsLoweringContract &contract) {
  return contract.weak_reference_sites <= contract.ownership_candidate_sites &&
         contract.unowned_reference_sites <= contract.ownership_candidate_sites &&
         contract.unowned_safe_reference_sites <= contract.unowned_reference_sites &&
         contract.weak_unowned_conflict_sites <= contract.ownership_candidate_sites &&
         contract.contract_violation_sites <=
             contract.ownership_candidate_sites + contract.weak_unowned_conflict_sites;
}

std::string Objc3WeakUnownedSemanticsLoweringReplayKey(
    const Objc3WeakUnownedSemanticsLoweringContract &contract) {
  return std::string("ownership_candidate_sites=") + std::to_string(contract.ownership_candidate_sites) +
         ";weak_reference_sites=" + std::to_string(contract.weak_reference_sites) +
         ";unowned_reference_sites=" + std::to_string(contract.unowned_reference_sites) +
         ";unowned_safe_reference_sites=" + std::to_string(contract.unowned_safe_reference_sites) +
         ";weak_unowned_conflict_sites=" + std::to_string(contract.weak_unowned_conflict_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3WeakUnownedSemanticsLoweringLaneContract;
}

bool IsValidObjc3ArcDiagnosticsFixitLoweringContract(
    const Objc3ArcDiagnosticsFixitLoweringContract &contract) {
  return contract.ownership_arc_fixit_available_sites <=
             contract.ownership_arc_diagnostic_candidate_sites + contract.contract_violation_sites &&
         contract.ownership_arc_profiled_sites <=
             contract.ownership_arc_diagnostic_candidate_sites + contract.contract_violation_sites &&
         contract.ownership_arc_weak_unowned_conflict_diagnostic_sites <=
             contract.ownership_arc_diagnostic_candidate_sites + contract.contract_violation_sites &&
         contract.ownership_arc_empty_fixit_hint_sites <=
             contract.ownership_arc_fixit_available_sites + contract.contract_violation_sites;
}

std::string Objc3ArcDiagnosticsFixitLoweringReplayKey(
    const Objc3ArcDiagnosticsFixitLoweringContract &contract) {
  return std::string("ownership_arc_diagnostic_candidate_sites=") +
             std::to_string(contract.ownership_arc_diagnostic_candidate_sites) +
         ";ownership_arc_fixit_available_sites=" +
             std::to_string(contract.ownership_arc_fixit_available_sites) +
         ";ownership_arc_profiled_sites=" + std::to_string(contract.ownership_arc_profiled_sites) +
         ";ownership_arc_weak_unowned_conflict_diagnostic_sites=" +
             std::to_string(contract.ownership_arc_weak_unowned_conflict_diagnostic_sites) +
         ";ownership_arc_empty_fixit_hint_sites=" +
             std::to_string(contract.ownership_arc_empty_fixit_hint_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3ArcDiagnosticsFixitLoweringLaneContract;
}

bool IsValidObjc3BlockSourceModelCompletionContract(
    const Objc3BlockSourceModelCompletionContract &contract) {
  const bool explicit_parameter_count_valid =
      contract.explicit_typed_parameter_entries_total <=
      contract.signature_entries_total;
  const bool implicit_parameter_count_valid =
      contract.implicit_parameter_entries_total <=
      contract.signature_entries_total;
  const bool readonly_capture_count_valid =
      contract.byvalue_readonly_capture_entries_total <=
      contract.capture_inventory_entries_total;
  const bool invoke_surface_count_valid =
      contract.invoke_surface_entries_total >=
      contract.block_literal_sites * 2u;
  const bool non_normalized_sites_valid =
      contract.non_normalized_sites <= contract.block_literal_sites;
  return explicit_parameter_count_valid &&
         implicit_parameter_count_valid &&
         readonly_capture_count_valid &&
         invoke_surface_count_valid &&
         non_normalized_sites_valid;
}

std::string Objc3BlockSourceModelCompletionReplayKey(
    const Objc3BlockSourceModelCompletionContract &contract) {
  return "signature_entries=" +
         std::to_string(contract.signature_entries_total) +
         ";explicit_typed_parameters=" +
         std::to_string(contract.explicit_typed_parameter_entries_total) +
         ";capture_inventory_entries=" +
         std::to_string(contract.capture_inventory_entries_total) +
         ";byvalue_readonly_captures=" +
         std::to_string(contract.byvalue_readonly_capture_entries_total) +
         ";invoke_surface_entries=" +
         std::to_string(contract.invoke_surface_entries_total) +
         ";deterministic=" + (contract.deterministic ? "true" : "false") +
         ";lane_contract=" + kObjc3BlockSourceModelCompletionLaneContract;
}

bool IsValidObjc3BlockSourceStorageAnnotationContract(
    const Objc3BlockSourceStorageAnnotationContract &contract) {
  const std::size_t classified_sites =
      contract.expression_sites +
      contract.global_initializer_sites +
      contract.binding_initializer_sites +
      contract.assignment_value_sites +
      contract.return_value_sites +
      contract.call_argument_sites +
      contract.message_argument_sites;
  if (classified_sites != contract.block_literal_sites ||
      contract.non_normalized_sites > contract.block_literal_sites ||
      contract.contract_violation_sites > contract.block_literal_sites ||
      contract.copy_helper_intent_sites > contract.block_literal_sites ||
      contract.dispose_helper_intent_sites > contract.block_literal_sites ||
      contract.heap_candidate_sites > contract.block_literal_sites ||
      contract.mutated_capture_entries_total > contract.capture_entries_total ||
      contract.byref_capture_entries_total > contract.mutated_capture_entries_total) {
    return false;
  }
  if (contract.copy_helper_intent_sites != contract.dispose_helper_intent_sites) {
    return false;
  }
  if (contract.block_literal_sites == 0) {
    return contract.capture_entries_total == 0 &&
           contract.mutated_capture_entries_total == 0 &&
           contract.byref_capture_entries_total == 0;
  }
  if (contract.heap_candidate_sites !=
      contract.global_initializer_sites +
          contract.binding_initializer_sites +
          contract.assignment_value_sites +
          contract.return_value_sites +
          contract.call_argument_sites +
          contract.message_argument_sites) {
    return false;
  }
  if ((contract.non_normalized_sites > 0 ||
       contract.contract_violation_sites > 0) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3BlockSourceStorageAnnotationReplayKey(
    const Objc3BlockSourceStorageAnnotationContract &contract) {
  return "block_literal_sites=" +
         std::to_string(contract.block_literal_sites) +
         ";capture_entries_total=" +
         std::to_string(contract.capture_entries_total) +
         ";mutated_capture_entries_total=" +
         std::to_string(contract.mutated_capture_entries_total) +
         ";byref_capture_entries_total=" +
         std::to_string(contract.byref_capture_entries_total) +
         ";copy_helper_intent_sites=" +
         std::to_string(contract.copy_helper_intent_sites) +
         ";dispose_helper_intent_sites=" +
         std::to_string(contract.dispose_helper_intent_sites) +
         ";heap_candidate_sites=" +
         std::to_string(contract.heap_candidate_sites) +
         ";expression_sites=" +
         std::to_string(contract.expression_sites) +
         ";global_initializer_sites=" +
         std::to_string(contract.global_initializer_sites) +
         ";binding_initializer_sites=" +
         std::to_string(contract.binding_initializer_sites) +
         ";assignment_value_sites=" +
         std::to_string(contract.assignment_value_sites) +
         ";return_value_sites=" +
         std::to_string(contract.return_value_sites) +
         ";call_argument_sites=" +
         std::to_string(contract.call_argument_sites) +
         ";message_argument_sites=" +
         std::to_string(contract.message_argument_sites) +
         ";deterministic=" + (contract.deterministic ? "true" : "false") +
         ";lane_contract=" +
         kObjc3BlockSourceStorageAnnotationLaneContract;
}

bool IsValidObjc3BlockLiteralCaptureLoweringContract(
    const Objc3BlockLiteralCaptureLoweringContract &contract) {
  if (contract.block_empty_capture_sites > contract.block_literal_sites ||
      contract.block_nondeterministic_capture_sites > contract.block_literal_sites ||
      contract.block_non_normalized_sites > contract.block_literal_sites ||
      contract.contract_violation_sites > contract.block_literal_sites) {
    return false;
  }
  if (contract.block_literal_sites == 0) {
    return contract.block_parameter_entries == 0 && contract.block_capture_entries == 0 &&
           contract.block_body_statement_entries == 0;
  }
  if (contract.block_nondeterministic_capture_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3BlockLiteralCaptureLoweringReplayKey(
    const Objc3BlockLiteralCaptureLoweringContract &contract) {
  return std::string("block_literal_sites=") + std::to_string(contract.block_literal_sites) +
         ";block_parameter_entries=" + std::to_string(contract.block_parameter_entries) +
         ";block_capture_entries=" + std::to_string(contract.block_capture_entries) +
         ";block_body_statement_entries=" + std::to_string(contract.block_body_statement_entries) +
         ";block_empty_capture_sites=" + std::to_string(contract.block_empty_capture_sites) +
         ";block_nondeterministic_capture_sites=" +
             std::to_string(contract.block_nondeterministic_capture_sites) +
         ";block_non_normalized_sites=" + std::to_string(contract.block_non_normalized_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3BlockLiteralCaptureLoweringLaneContract;
}

bool IsValidObjc3BlockAbiInvokeTrampolineLoweringContract(
    const Objc3BlockAbiInvokeTrampolineLoweringContract &contract) {
  if (contract.descriptor_symbolized_sites > contract.block_literal_sites ||
      contract.invoke_trampoline_symbolized_sites > contract.block_literal_sites ||
      contract.missing_invoke_trampoline_sites > contract.block_literal_sites ||
      contract.non_normalized_layout_sites > contract.block_literal_sites ||
      contract.contract_violation_sites > contract.block_literal_sites) {
    return false;
  }
  if (contract.block_literal_sites == 0) {
    return contract.invoke_argument_slots_total == 0 &&
           contract.capture_word_count_total == 0 &&
           contract.parameter_entries_total == 0 &&
           contract.capture_entries_total == 0 &&
           contract.body_statement_entries_total == 0;
  }
  if (contract.invoke_trampoline_symbolized_sites + contract.missing_invoke_trampoline_sites !=
      contract.block_literal_sites) {
    return false;
  }
  if (contract.invoke_argument_slots_total != contract.parameter_entries_total ||
      contract.capture_word_count_total != contract.capture_entries_total) {
    return false;
  }
  if ((contract.missing_invoke_trampoline_sites > 0 || contract.non_normalized_layout_sites > 0) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3BlockAbiInvokeTrampolineLoweringReplayKey(
    const Objc3BlockAbiInvokeTrampolineLoweringContract &contract) {
  return std::string("block_literal_sites=") + std::to_string(contract.block_literal_sites) +
         ";invoke_argument_slots_total=" + std::to_string(contract.invoke_argument_slots_total) +
         ";capture_word_count_total=" + std::to_string(contract.capture_word_count_total) +
         ";parameter_entries_total=" + std::to_string(contract.parameter_entries_total) +
         ";capture_entries_total=" + std::to_string(contract.capture_entries_total) +
         ";body_statement_entries_total=" + std::to_string(contract.body_statement_entries_total) +
         ";descriptor_symbolized_sites=" + std::to_string(contract.descriptor_symbolized_sites) +
         ";invoke_trampoline_symbolized_sites=" +
             std::to_string(contract.invoke_trampoline_symbolized_sites) +
         ";missing_invoke_trampoline_sites=" + std::to_string(contract.missing_invoke_trampoline_sites) +
         ";non_normalized_layout_sites=" + std::to_string(contract.non_normalized_layout_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3BlockAbiInvokeTrampolineLoweringLaneContract;
}

bool IsValidObjc3BlockStorageEscapeLoweringContract(
    const Objc3BlockStorageEscapeLoweringContract &contract) {
  // M261-B002 capture-legality/escape/invocation implementation anchor:
  // truthful escape classification no longer pretends that every capture is
  // mutated or lowered through byref storage. Mutable/byref counts are now a
  // bounded subset of the total capture inventory while source-only native
  // fail-closed behavior remains unchanged.
  if (contract.requires_byref_cells_sites > contract.block_literal_sites ||
      contract.escape_analysis_enabled_sites > contract.block_literal_sites ||
      contract.escape_to_heap_sites > contract.block_literal_sites ||
      contract.escape_profile_normalized_sites > contract.block_literal_sites ||
      contract.byref_layout_symbolized_sites > contract.block_literal_sites ||
      contract.contract_violation_sites > contract.block_literal_sites) {
    return false;
  }
  if (contract.block_literal_sites == 0) {
    return contract.mutable_capture_count_total == 0 &&
           contract.byref_slot_count_total == 0 &&
           contract.parameter_entries_total == 0 &&
           contract.capture_entries_total == 0 &&
           contract.body_statement_entries_total == 0;
  }
  if (contract.mutable_capture_count_total > contract.capture_entries_total ||
      contract.byref_slot_count_total > contract.mutable_capture_count_total ||
      contract.escape_analysis_enabled_sites != contract.block_literal_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 || contract.escape_profile_normalized_sites !=
                                            contract.block_literal_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3BlockStorageEscapeLoweringReplayKey(
    const Objc3BlockStorageEscapeLoweringContract &contract) {
  return std::string("block_literal_sites=") + std::to_string(contract.block_literal_sites) +
         ";mutable_capture_count_total=" + std::to_string(contract.mutable_capture_count_total) +
         ";byref_slot_count_total=" + std::to_string(contract.byref_slot_count_total) +
         ";parameter_entries_total=" + std::to_string(contract.parameter_entries_total) +
         ";capture_entries_total=" + std::to_string(contract.capture_entries_total) +
         ";body_statement_entries_total=" + std::to_string(contract.body_statement_entries_total) +
         ";requires_byref_cells_sites=" + std::to_string(contract.requires_byref_cells_sites) +
         ";escape_analysis_enabled_sites=" + std::to_string(contract.escape_analysis_enabled_sites) +
         ";escape_to_heap_sites=" + std::to_string(contract.escape_to_heap_sites) +
         ";escape_profile_normalized_sites=" + std::to_string(contract.escape_profile_normalized_sites) +
         ";byref_layout_symbolized_sites=" + std::to_string(contract.byref_layout_symbolized_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3BlockStorageEscapeLoweringLaneContract;
}

bool IsValidObjc3BlockCopyDisposeLoweringContract(
    const Objc3BlockCopyDisposeLoweringContract &contract) {
  // M261-B002 capture-legality/escape/invocation implementation anchor:
  // copy/dispose helper intent is now driven by truthful mutable/byref
  // capture counts rather than by the older synthetic all-captures-need-
  // helpers model.
  // M261-B003 byref/copy-dispose/object-ownership anchor: helper eligibility
  // may now be promoted by owned object captures even when byref slot totals remain zero,
  // so this contract intentionally avoids pinning helper counts directly to
  // byref totals.
  if (contract.copy_helper_required_sites > contract.block_literal_sites ||
      contract.dispose_helper_required_sites > contract.block_literal_sites ||
      contract.profile_normalized_sites > contract.block_literal_sites ||
      contract.copy_helper_symbolized_sites > contract.block_literal_sites ||
      contract.dispose_helper_symbolized_sites > contract.block_literal_sites ||
      contract.contract_violation_sites > contract.block_literal_sites) {
    return false;
  }
  if (contract.block_literal_sites == 0) {
    return contract.mutable_capture_count_total == 0 &&
           contract.byref_slot_count_total == 0 &&
           contract.parameter_entries_total == 0 &&
           contract.capture_entries_total == 0 &&
           contract.body_statement_entries_total == 0;
  }
  if (contract.mutable_capture_count_total > contract.capture_entries_total ||
      contract.byref_slot_count_total > contract.mutable_capture_count_total ||
      contract.copy_helper_required_sites != contract.dispose_helper_required_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 || contract.profile_normalized_sites !=
                                            contract.block_literal_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3BlockCopyDisposeLoweringReplayKey(
    const Objc3BlockCopyDisposeLoweringContract &contract) {
  return std::string("block_literal_sites=") + std::to_string(contract.block_literal_sites) +
         ";mutable_capture_count_total=" + std::to_string(contract.mutable_capture_count_total) +
         ";byref_slot_count_total=" + std::to_string(contract.byref_slot_count_total) +
         ";parameter_entries_total=" + std::to_string(contract.parameter_entries_total) +
         ";capture_entries_total=" + std::to_string(contract.capture_entries_total) +
         ";body_statement_entries_total=" + std::to_string(contract.body_statement_entries_total) +
         ";copy_helper_required_sites=" + std::to_string(contract.copy_helper_required_sites) +
         ";dispose_helper_required_sites=" + std::to_string(contract.dispose_helper_required_sites) +
         ";profile_normalized_sites=" + std::to_string(contract.profile_normalized_sites) +
         ";copy_helper_symbolized_sites=" + std::to_string(contract.copy_helper_symbolized_sites) +
         ";dispose_helper_symbolized_sites=" + std::to_string(contract.dispose_helper_symbolized_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3BlockCopyDisposeLoweringLaneContract;
}

bool IsValidObjc3BlockDeterminismPerfBaselineLoweringContract(
    const Objc3BlockDeterminismPerfBaselineLoweringContract &contract) {
  if (contract.deterministic_capture_sites > contract.block_literal_sites ||
      contract.heavy_tier_sites > contract.block_literal_sites ||
      contract.normalized_profile_sites > contract.block_literal_sites ||
      contract.contract_violation_sites > contract.block_literal_sites) {
    return false;
  }
  if (contract.block_literal_sites == 0) {
    return contract.baseline_weight_total == 0 &&
           contract.parameter_entries_total == 0 &&
           contract.capture_entries_total == 0 &&
           contract.body_statement_entries_total == 0;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_profile_sites != contract.block_literal_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3BlockDeterminismPerfBaselineLoweringReplayKey(
    const Objc3BlockDeterminismPerfBaselineLoweringContract &contract) {
  return std::string("block_literal_sites=") + std::to_string(contract.block_literal_sites) +
         ";baseline_weight_total=" + std::to_string(contract.baseline_weight_total) +
         ";parameter_entries_total=" + std::to_string(contract.parameter_entries_total) +
         ";capture_entries_total=" + std::to_string(contract.capture_entries_total) +
         ";body_statement_entries_total=" + std::to_string(contract.body_statement_entries_total) +
         ";deterministic_capture_sites=" + std::to_string(contract.deterministic_capture_sites) +
         ";heavy_tier_sites=" + std::to_string(contract.heavy_tier_sites) +
         ";normalized_profile_sites=" + std::to_string(contract.normalized_profile_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3BlockDeterminismPerfBaselineLoweringLaneContract;
}

bool IsValidObjc3LightweightGenericsConstraintLoweringContract(
    const Objc3LightweightGenericsConstraintLoweringContract &contract) {
  if (contract.generic_suffix_sites > contract.generic_constraint_sites ||
      contract.object_pointer_type_sites > contract.generic_constraint_sites ||
      contract.terminated_generic_suffix_sites > contract.generic_suffix_sites ||
      contract.pointer_declarator_sites > contract.generic_constraint_sites ||
      contract.normalized_constraint_sites > contract.generic_constraint_sites ||
      contract.contract_violation_sites > contract.generic_constraint_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_constraint_sites != contract.generic_constraint_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3LightweightGenericsConstraintLoweringReplayKey(
    const Objc3LightweightGenericsConstraintLoweringContract &contract) {
  return std::string("generic_constraint_sites=") + std::to_string(contract.generic_constraint_sites) +
         ";generic_suffix_sites=" + std::to_string(contract.generic_suffix_sites) +
         ";object_pointer_type_sites=" + std::to_string(contract.object_pointer_type_sites) +
         ";terminated_generic_suffix_sites=" + std::to_string(contract.terminated_generic_suffix_sites) +
         ";pointer_declarator_sites=" + std::to_string(contract.pointer_declarator_sites) +
         ";normalized_constraint_sites=" + std::to_string(contract.normalized_constraint_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3LightweightGenericsConstraintLoweringLaneContract;
}

bool IsValidObjc3NullabilityFlowWarningPrecisionLoweringContract(
    const Objc3NullabilityFlowWarningPrecisionLoweringContract &contract) {
  if (contract.nullability_suffix_sites > contract.nullability_flow_sites ||
      contract.nullable_suffix_sites > contract.nullability_suffix_sites ||
      contract.nonnull_suffix_sites > contract.nullability_suffix_sites ||
      contract.object_pointer_type_sites < contract.nullability_suffix_sites ||
      contract.normalized_sites > contract.nullability_flow_sites ||
      contract.contract_violation_sites > contract.nullability_flow_sites) {
    return false;
  }
  if (contract.nullability_suffix_sites !=
      contract.nullable_suffix_sites + contract.nonnull_suffix_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.nullability_flow_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3NullabilityFlowWarningPrecisionLoweringReplayKey(
    const Objc3NullabilityFlowWarningPrecisionLoweringContract &contract) {
  return std::string("nullability_flow_sites=") + std::to_string(contract.nullability_flow_sites) +
         ";object_pointer_type_sites=" + std::to_string(contract.object_pointer_type_sites) +
         ";nullability_suffix_sites=" + std::to_string(contract.nullability_suffix_sites) +
         ";nullable_suffix_sites=" + std::to_string(contract.nullable_suffix_sites) +
         ";nonnull_suffix_sites=" + std::to_string(contract.nonnull_suffix_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3NullabilityFlowWarningPrecisionLoweringLaneContract;
}

bool IsValidObjc3ProtocolQualifiedObjectTypeLoweringContract(
    const Objc3ProtocolQualifiedObjectTypeLoweringContract &contract) {
  if (contract.terminated_protocol_composition_sites > contract.protocol_composition_sites ||
      contract.normalized_protocol_composition_sites > contract.protocol_qualified_object_type_sites ||
      contract.contract_violation_sites > contract.protocol_qualified_object_type_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_protocol_composition_sites != contract.protocol_qualified_object_type_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ProtocolQualifiedObjectTypeLoweringReplayKey(
    const Objc3ProtocolQualifiedObjectTypeLoweringContract &contract) {
  return std::string("protocol_qualified_object_type_sites=") +
         std::to_string(contract.protocol_qualified_object_type_sites) +
         ";protocol_composition_sites=" + std::to_string(contract.protocol_composition_sites) +
         ";object_pointer_type_sites=" + std::to_string(contract.object_pointer_type_sites) +
         ";terminated_protocol_composition_sites=" + std::to_string(contract.terminated_protocol_composition_sites) +
         ";pointer_declarator_sites=" + std::to_string(contract.pointer_declarator_sites) +
         ";normalized_protocol_composition_sites=" +
         std::to_string(contract.normalized_protocol_composition_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3ProtocolQualifiedObjectTypeLoweringLaneContract;
}

bool IsValidObjc3VarianceBridgeCastLoweringContract(
    const Objc3VarianceBridgeCastLoweringContract &contract) {
  if (contract.protocol_composition_sites > contract.variance_bridge_cast_sites ||
      contract.ownership_qualifier_sites > contract.variance_bridge_cast_sites ||
      contract.object_pointer_type_sites < contract.protocol_composition_sites ||
      contract.pointer_declarator_sites > contract.variance_bridge_cast_sites ||
      contract.normalized_sites > contract.variance_bridge_cast_sites ||
      contract.contract_violation_sites > contract.variance_bridge_cast_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.variance_bridge_cast_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3VarianceBridgeCastLoweringReplayKey(
    const Objc3VarianceBridgeCastLoweringContract &contract) {
  return std::string("variance_bridge_cast_sites=") + std::to_string(contract.variance_bridge_cast_sites) +
         ";protocol_composition_sites=" + std::to_string(contract.protocol_composition_sites) +
         ";ownership_qualifier_sites=" + std::to_string(contract.ownership_qualifier_sites) +
         ";object_pointer_type_sites=" + std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" + std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3VarianceBridgeCastLoweringLaneContract;
}

bool IsValidObjc3GenericMetadataAbiLoweringContract(
    const Objc3GenericMetadataAbiLoweringContract &contract) {
  if (contract.generic_suffix_sites > contract.generic_metadata_abi_sites ||
      contract.protocol_composition_sites > contract.generic_metadata_abi_sites ||
      contract.ownership_qualifier_sites > contract.generic_metadata_abi_sites ||
      contract.object_pointer_type_sites < contract.protocol_composition_sites ||
      contract.pointer_declarator_sites > contract.generic_metadata_abi_sites ||
      contract.normalized_sites > contract.generic_metadata_abi_sites ||
      contract.contract_violation_sites > contract.generic_metadata_abi_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.generic_metadata_abi_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3GenericMetadataAbiLoweringReplayKey(
    const Objc3GenericMetadataAbiLoweringContract &contract) {
  return std::string("generic_metadata_abi_sites=") + std::to_string(contract.generic_metadata_abi_sites) +
         ";generic_suffix_sites=" + std::to_string(contract.generic_suffix_sites) +
         ";protocol_composition_sites=" + std::to_string(contract.protocol_composition_sites) +
         ";ownership_qualifier_sites=" + std::to_string(contract.ownership_qualifier_sites) +
         ";object_pointer_type_sites=" + std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" + std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3GenericMetadataAbiLoweringLaneContract;
}

bool IsValidObjc3ModuleImportGraphLoweringContract(
    const Objc3ModuleImportGraphLoweringContract &contract) {
  if (contract.import_edge_candidate_sites > contract.module_import_graph_sites ||
      contract.namespace_segment_sites > contract.module_import_graph_sites ||
      contract.object_pointer_type_sites < contract.import_edge_candidate_sites ||
      contract.pointer_declarator_sites > contract.module_import_graph_sites ||
      contract.normalized_sites > contract.module_import_graph_sites ||
      contract.contract_violation_sites > contract.module_import_graph_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.module_import_graph_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ModuleImportGraphLoweringReplayKey(
    const Objc3ModuleImportGraphLoweringContract &contract) {
  return std::string("module_import_graph_sites=") + std::to_string(contract.module_import_graph_sites) +
         ";import_edge_candidate_sites=" + std::to_string(contract.import_edge_candidate_sites) +
         ";namespace_segment_sites=" + std::to_string(contract.namespace_segment_sites) +
         ";object_pointer_type_sites=" + std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" + std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3ModuleImportGraphLoweringLaneContract;
}

bool IsValidObjc3NamespaceCollisionShadowingLoweringContract(
    const Objc3NamespaceCollisionShadowingLoweringContract &contract) {
  if (contract.namespace_segment_sites > contract.namespace_collision_shadowing_sites ||
      contract.import_edge_candidate_sites > contract.namespace_collision_shadowing_sites ||
      contract.object_pointer_type_sites < contract.import_edge_candidate_sites ||
      contract.pointer_declarator_sites > contract.namespace_collision_shadowing_sites ||
      contract.normalized_sites > contract.namespace_collision_shadowing_sites ||
      contract.contract_violation_sites > contract.namespace_collision_shadowing_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.namespace_collision_shadowing_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3NamespaceCollisionShadowingLoweringReplayKey(
    const Objc3NamespaceCollisionShadowingLoweringContract &contract) {
  return std::string("namespace_collision_shadowing_sites=") +
             std::to_string(contract.namespace_collision_shadowing_sites) +
         ";namespace_segment_sites=" + std::to_string(contract.namespace_segment_sites) +
         ";import_edge_candidate_sites=" +
             std::to_string(contract.import_edge_candidate_sites) +
         ";object_pointer_type_sites=" +
             std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" +
             std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";contract_violation_sites=" +
             std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3NamespaceCollisionShadowingLoweringLaneContract;
}

bool IsValidObjc3PublicPrivateApiPartitionLoweringContract(
    const Objc3PublicPrivateApiPartitionLoweringContract &contract) {
  if (contract.namespace_segment_sites >
          contract.public_private_api_partition_sites ||
      contract.import_edge_candidate_sites >
          contract.public_private_api_partition_sites ||
      contract.object_pointer_type_sites <
          contract.import_edge_candidate_sites ||
      contract.pointer_declarator_sites >
          contract.public_private_api_partition_sites ||
      contract.normalized_sites > contract.public_private_api_partition_sites ||
      contract.contract_violation_sites >
          contract.public_private_api_partition_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.public_private_api_partition_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3PublicPrivateApiPartitionLoweringReplayKey(
    const Objc3PublicPrivateApiPartitionLoweringContract &contract) {
  return std::string("public_private_api_partition_sites=") +
             std::to_string(contract.public_private_api_partition_sites) +
         ";namespace_segment_sites=" +
         std::to_string(contract.namespace_segment_sites) +
         ";import_edge_candidate_sites=" +
         std::to_string(contract.import_edge_candidate_sites) +
         ";object_pointer_type_sites=" +
         std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" +
         std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3PublicPrivateApiPartitionLoweringLaneContract;
}

bool IsValidObjc3IncrementalModuleCacheInvalidationLoweringContract(
    const Objc3IncrementalModuleCacheInvalidationLoweringContract &contract) {
  if (contract.namespace_segment_sites >
          contract.incremental_module_cache_invalidation_sites ||
      contract.import_edge_candidate_sites >
          contract.incremental_module_cache_invalidation_sites ||
      contract.object_pointer_type_sites < contract.import_edge_candidate_sites ||
      contract.pointer_declarator_sites >
          contract.incremental_module_cache_invalidation_sites ||
      contract.normalized_sites >
          contract.incremental_module_cache_invalidation_sites ||
      contract.cache_invalidation_candidate_sites >
          contract.incremental_module_cache_invalidation_sites ||
      contract.contract_violation_sites >
          contract.incremental_module_cache_invalidation_sites) {
    return false;
  }
  if (contract.normalized_sites +
          contract.cache_invalidation_candidate_sites >
      contract.incremental_module_cache_invalidation_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites !=
           contract.incremental_module_cache_invalidation_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3IncrementalModuleCacheInvalidationLoweringReplayKey(
    const Objc3IncrementalModuleCacheInvalidationLoweringContract &contract) {
  return std::string("incremental_module_cache_invalidation_sites=") +
             std::to_string(contract.incremental_module_cache_invalidation_sites) +
         ";namespace_segment_sites=" +
         std::to_string(contract.namespace_segment_sites) +
         ";import_edge_candidate_sites=" +
         std::to_string(contract.import_edge_candidate_sites) +
         ";object_pointer_type_sites=" +
         std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" +
         std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";cache_invalidation_candidate_sites=" +
         std::to_string(contract.cache_invalidation_candidate_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3IncrementalModuleCacheInvalidationLoweringLaneContract;
}

bool IsValidObjc3CrossModuleConformanceLoweringContract(
    const Objc3CrossModuleConformanceLoweringContract &contract) {
  if (contract.namespace_segment_sites >
          contract.cross_module_conformance_sites ||
      contract.import_edge_candidate_sites >
          contract.cross_module_conformance_sites ||
      contract.object_pointer_type_sites < contract.import_edge_candidate_sites ||
      contract.pointer_declarator_sites >
          contract.cross_module_conformance_sites ||
      contract.normalized_sites > contract.cross_module_conformance_sites ||
      contract.cache_invalidation_candidate_sites >
          contract.cross_module_conformance_sites ||
      contract.contract_violation_sites >
          contract.cross_module_conformance_sites) {
    return false;
  }
  if (contract.normalized_sites +
          contract.cache_invalidation_candidate_sites >
      contract.cross_module_conformance_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.cross_module_conformance_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3CrossModuleConformanceLoweringReplayKey(
    const Objc3CrossModuleConformanceLoweringContract &contract) {
  return std::string("cross_module_conformance_sites=") +
             std::to_string(contract.cross_module_conformance_sites) +
         ";namespace_segment_sites=" +
         std::to_string(contract.namespace_segment_sites) +
         ";import_edge_candidate_sites=" +
         std::to_string(contract.import_edge_candidate_sites) +
         ";object_pointer_type_sites=" +
         std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" +
         std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";cache_invalidation_candidate_sites=" +
         std::to_string(contract.cache_invalidation_candidate_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3CrossModuleConformanceLoweringLaneContract;
}

bool IsValidObjc3ThrowsPropagationLoweringContract(
    const Objc3ThrowsPropagationLoweringContract &contract) {
  if (contract.namespace_segment_sites > contract.throws_propagation_sites ||
      contract.import_edge_candidate_sites > contract.throws_propagation_sites ||
      contract.object_pointer_type_sites < contract.import_edge_candidate_sites ||
      contract.pointer_declarator_sites > contract.throws_propagation_sites ||
      contract.normalized_sites > contract.throws_propagation_sites ||
      contract.cache_invalidation_candidate_sites >
          contract.throws_propagation_sites ||
      contract.contract_violation_sites > contract.throws_propagation_sites) {
    return false;
  }
  if (contract.normalized_sites +
          contract.cache_invalidation_candidate_sites >
      contract.throws_propagation_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.throws_propagation_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ThrowsPropagationLoweringReplayKey(
    const Objc3ThrowsPropagationLoweringContract &contract) {
  return std::string("throws_propagation_sites=") +
             std::to_string(contract.throws_propagation_sites) +
         ";namespace_segment_sites=" +
         std::to_string(contract.namespace_segment_sites) +
         ";import_edge_candidate_sites=" +
         std::to_string(contract.import_edge_candidate_sites) +
         ";object_pointer_type_sites=" +
         std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" +
         std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";cache_invalidation_candidate_sites=" +
         std::to_string(contract.cache_invalidation_candidate_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3ThrowsPropagationLoweringLaneContract;
}

bool IsValidObjc3ResultLikeLoweringContract(
    const Objc3ResultLikeLoweringContract &contract) {
  if (contract.result_success_sites > contract.result_like_sites ||
      contract.result_failure_sites > contract.result_like_sites ||
      contract.result_branch_sites > contract.result_like_sites ||
      contract.result_payload_sites > contract.result_like_sites ||
      contract.normalized_sites > contract.result_like_sites ||
      contract.branch_merge_sites > contract.result_like_sites ||
      contract.contract_violation_sites > contract.result_like_sites) {
    return false;
  }
  if (contract.normalized_sites + contract.branch_merge_sites !=
      contract.result_like_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ResultLikeLoweringReplayKey(
    const Objc3ResultLikeLoweringContract &contract) {
  return std::string("result_like_sites=") +
             std::to_string(contract.result_like_sites) +
         ";result_success_sites=" +
         std::to_string(contract.result_success_sites) +
         ";result_failure_sites=" +
         std::to_string(contract.result_failure_sites) +
         ";result_branch_sites=" +
         std::to_string(contract.result_branch_sites) +
         ";result_payload_sites=" +
         std::to_string(contract.result_payload_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";branch_merge_sites=" + std::to_string(contract.branch_merge_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3ResultLikeLoweringLaneContract;
}

bool IsValidObjc3NSErrorBridgingLoweringContract(
    const Objc3NSErrorBridgingLoweringContract &contract) {
  if (contract.ns_error_parameter_sites > contract.ns_error_bridging_sites ||
      contract.ns_error_out_parameter_sites > contract.ns_error_parameter_sites ||
      contract.ns_error_bridge_path_sites > contract.ns_error_out_parameter_sites ||
      contract.ns_error_bridge_path_sites > contract.failable_call_sites ||
      contract.failable_call_sites > contract.ns_error_bridging_sites ||
      contract.normalized_sites > contract.ns_error_bridging_sites ||
      contract.bridge_boundary_sites > contract.ns_error_bridging_sites ||
      contract.contract_violation_sites > contract.ns_error_bridging_sites) {
    return false;
  }
  if (contract.normalized_sites + contract.bridge_boundary_sites !=
      contract.ns_error_bridging_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3NSErrorBridgingLoweringReplayKey(
    const Objc3NSErrorBridgingLoweringContract &contract) {
  return std::string("ns_error_bridging_sites=") +
             std::to_string(contract.ns_error_bridging_sites) +
         ";ns_error_parameter_sites=" +
         std::to_string(contract.ns_error_parameter_sites) +
         ";ns_error_out_parameter_sites=" +
         std::to_string(contract.ns_error_out_parameter_sites) +
         ";ns_error_bridge_path_sites=" +
         std::to_string(contract.ns_error_bridge_path_sites) +
         ";failable_call_sites=" +
         std::to_string(contract.failable_call_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";bridge_boundary_sites=" + std::to_string(contract.bridge_boundary_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3NSErrorBridgingLoweringLaneContract;
}

bool IsValidObjc3UnwindCleanupLoweringContract(
    const Objc3UnwindCleanupLoweringContract &contract) {
  if (contract.unwind_edge_sites > contract.unwind_cleanup_sites ||
      contract.cleanup_scope_sites > contract.unwind_cleanup_sites ||
      contract.cleanup_emit_sites > contract.cleanup_scope_sites ||
      contract.landing_pad_sites > contract.unwind_cleanup_sites ||
      contract.cleanup_resume_sites > contract.unwind_cleanup_sites ||
      contract.normalized_sites > contract.unwind_cleanup_sites ||
      contract.guard_blocked_sites > contract.unwind_cleanup_sites ||
      contract.contract_violation_sites > contract.unwind_cleanup_sites) {
    return false;
  }
  if (contract.landing_pad_sites + contract.cleanup_resume_sites >
      contract.unwind_cleanup_sites) {
    return false;
  }
  if (contract.normalized_sites + contract.guard_blocked_sites !=
      contract.unwind_cleanup_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3UnwindCleanupLoweringReplayKey(
    const Objc3UnwindCleanupLoweringContract &contract) {
  return std::string("unwind_cleanup_sites=") +
             std::to_string(contract.unwind_cleanup_sites) +
         ";unwind_edge_sites=" + std::to_string(contract.unwind_edge_sites) +
         ";cleanup_scope_sites=" +
         std::to_string(contract.cleanup_scope_sites) +
         ";cleanup_emit_sites=" + std::to_string(contract.cleanup_emit_sites) +
         ";landing_pad_sites=" + std::to_string(contract.landing_pad_sites) +
         ";cleanup_resume_sites=" +
         std::to_string(contract.cleanup_resume_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";guard_blocked_sites=" +
         std::to_string(contract.guard_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3UnwindCleanupLoweringLaneContract;
}

bool IsValidObjc3ErrorDiagnosticsRecoveryLoweringContract(
    const Objc3ErrorDiagnosticsRecoveryLoweringContract &contract) {
  if (contract.parser_diagnostic_sites > contract.error_diagnostic_sites ||
      contract.semantic_diagnostic_sites > contract.error_diagnostic_sites ||
      contract.fixit_hint_sites > contract.error_diagnostic_sites ||
      contract.recovery_candidate_sites > contract.error_diagnostic_sites ||
      contract.recovery_applied_sites > contract.recovery_candidate_sites ||
      contract.normalized_sites > contract.error_diagnostic_sites ||
      contract.guard_blocked_sites > contract.error_diagnostic_sites ||
      contract.contract_violation_sites > contract.error_diagnostic_sites) {
    return false;
  }
  if (contract.parser_diagnostic_sites + contract.semantic_diagnostic_sites >
      contract.error_diagnostic_sites) {
    return false;
  }
  if (contract.normalized_sites + contract.guard_blocked_sites !=
      contract.error_diagnostic_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ErrorDiagnosticsRecoveryLoweringReplayKey(
    const Objc3ErrorDiagnosticsRecoveryLoweringContract &contract) {
  return std::string("error_diagnostic_sites=") +
             std::to_string(contract.error_diagnostic_sites) +
         ";parser_diagnostic_sites=" +
         std::to_string(contract.parser_diagnostic_sites) +
         ";semantic_diagnostic_sites=" +
         std::to_string(contract.semantic_diagnostic_sites) +
         ";fixit_hint_sites=" + std::to_string(contract.fixit_hint_sites) +
         ";recovery_candidate_sites=" +
         std::to_string(contract.recovery_candidate_sites) +
         ";recovery_applied_sites=" +
         std::to_string(contract.recovery_applied_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";guard_blocked_sites=" + std::to_string(contract.guard_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3ErrorDiagnosticsRecoveryLoweringLaneContract;
}

bool IsValidObjc3AsyncContinuationLoweringContract(
    const Objc3AsyncContinuationLoweringContract &contract) {
  if (contract.async_keyword_sites > contract.async_continuation_sites ||
      contract.async_function_sites > contract.async_continuation_sites ||
      contract.continuation_allocation_sites > contract.async_continuation_sites ||
      contract.continuation_resume_sites > contract.async_continuation_sites ||
      contract.continuation_suspend_sites > contract.async_continuation_sites ||
      contract.async_state_machine_sites > contract.async_continuation_sites ||
      contract.normalized_sites > contract.async_continuation_sites ||
      contract.gate_blocked_sites > contract.async_continuation_sites ||
      contract.contract_violation_sites > contract.async_continuation_sites) {
    return false;
  }
  if (contract.normalized_sites + contract.gate_blocked_sites !=
      contract.async_continuation_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3AsyncContinuationLoweringReplayKey(
    const Objc3AsyncContinuationLoweringContract &contract) {
  return std::string("async_continuation_sites=") +
             std::to_string(contract.async_continuation_sites) +
         ";async_keyword_sites=" + std::to_string(contract.async_keyword_sites) +
         ";async_function_sites=" + std::to_string(contract.async_function_sites) +
         ";continuation_allocation_sites=" +
         std::to_string(contract.continuation_allocation_sites) +
         ";continuation_resume_sites=" +
         std::to_string(contract.continuation_resume_sites) +
         ";continuation_suspend_sites=" +
         std::to_string(contract.continuation_suspend_sites) +
         ";async_state_machine_sites=" +
         std::to_string(contract.async_state_machine_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";gate_blocked_sites=" + std::to_string(contract.gate_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3AsyncContinuationLoweringLaneContract;
}

bool IsValidObjc3AwaitLoweringSuspensionStateLoweringContract(
    const Objc3AwaitLoweringSuspensionStateLoweringContract &contract) {
  if (contract.await_keyword_sites > contract.await_suspension_sites ||
      contract.await_suspension_point_sites > contract.await_suspension_sites ||
      contract.await_resume_sites > contract.await_suspension_point_sites ||
      contract.await_state_machine_sites > contract.await_suspension_point_sites ||
      contract.await_continuation_sites >
          contract.await_suspension_point_sites ||
      contract.normalized_sites > contract.await_suspension_sites ||
      contract.gate_blocked_sites > contract.await_suspension_sites ||
      contract.contract_violation_sites > contract.await_suspension_sites) {
    return false;
  }
  if (contract.normalized_sites + contract.gate_blocked_sites !=
      contract.await_suspension_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3AwaitLoweringSuspensionStateLoweringReplayKey(
    const Objc3AwaitLoweringSuspensionStateLoweringContract &contract) {
  return std::string("await_suspension_sites=") +
             std::to_string(contract.await_suspension_sites) +
         ";await_keyword_sites=" + std::to_string(contract.await_keyword_sites) +
         ";await_suspension_point_sites=" +
         std::to_string(contract.await_suspension_point_sites) +
         ";await_resume_sites=" + std::to_string(contract.await_resume_sites) +
         ";await_state_machine_sites=" +
         std::to_string(contract.await_state_machine_sites) +
         ";await_continuation_sites=" +
         std::to_string(contract.await_continuation_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";gate_blocked_sites=" + std::to_string(contract.gate_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3AwaitLoweringSuspensionStateLoweringLaneContract;
}

bool IsValidObjc3ActorIsolationSendabilityLoweringContract(
    const Objc3ActorIsolationSendabilityLoweringContract &contract) {
  if (contract.sendability_check_sites > contract.actor_isolation_sites ||
      contract.cross_actor_hop_sites > contract.actor_isolation_sites ||
      contract.non_sendable_capture_sites > contract.sendability_check_sites ||
      contract.sendable_transfer_sites > contract.sendability_check_sites ||
      contract.isolation_boundary_sites > contract.actor_isolation_sites ||
      contract.guard_blocked_sites > contract.actor_isolation_sites ||
      contract.contract_violation_sites > contract.actor_isolation_sites) {
    return false;
  }
  if (contract.isolation_boundary_sites + contract.guard_blocked_sites !=
      contract.actor_isolation_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ActorIsolationSendabilityLoweringReplayKey(
    const Objc3ActorIsolationSendabilityLoweringContract &contract) {
  return std::string("actor_isolation_sites=") +
             std::to_string(contract.actor_isolation_sites) +
         ";sendability_check_sites=" +
         std::to_string(contract.sendability_check_sites) +
         ";cross_actor_hop_sites=" +
         std::to_string(contract.cross_actor_hop_sites) +
         ";non_sendable_capture_sites=" +
         std::to_string(contract.non_sendable_capture_sites) +
         ";sendable_transfer_sites=" +
         std::to_string(contract.sendable_transfer_sites) +
         ";isolation_boundary_sites=" +
         std::to_string(contract.isolation_boundary_sites) +
         ";guard_blocked_sites=" + std::to_string(contract.guard_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3ActorIsolationSendabilityLoweringLaneContract;
}

bool IsValidObjc3TaskRuntimeInteropCancellationLoweringContract(
    const Objc3TaskRuntimeInteropCancellationLoweringContract &contract) {
  if (contract.task_runtime_interop_sites > contract.task_runtime_sites ||
      contract.cancellation_probe_sites > contract.task_runtime_sites ||
      contract.cancellation_handler_sites > contract.task_runtime_sites ||
      contract.runtime_resume_sites > contract.task_runtime_sites ||
      contract.runtime_cancel_sites > contract.task_runtime_sites ||
      contract.normalized_sites > contract.task_runtime_sites ||
      contract.guard_blocked_sites > contract.task_runtime_sites ||
      contract.contract_violation_sites > contract.task_runtime_sites) {
    return false;
  }
  if (contract.runtime_resume_sites + contract.runtime_cancel_sites >
      contract.task_runtime_sites) {
    return false;
  }
  if (contract.normalized_sites + contract.guard_blocked_sites !=
      contract.task_runtime_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3TaskRuntimeInteropCancellationLoweringReplayKey(
    const Objc3TaskRuntimeInteropCancellationLoweringContract &contract) {
  return std::string("task_runtime_sites=") +
             std::to_string(contract.task_runtime_sites) +
         ";task_runtime_interop_sites=" +
         std::to_string(contract.task_runtime_interop_sites) +
         ";cancellation_probe_sites=" +
         std::to_string(contract.cancellation_probe_sites) +
         ";cancellation_handler_sites=" +
         std::to_string(contract.cancellation_handler_sites) +
         ";runtime_resume_sites=" +
         std::to_string(contract.runtime_resume_sites) +
         ";runtime_cancel_sites=" +
         std::to_string(contract.runtime_cancel_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";guard_blocked_sites=" + std::to_string(contract.guard_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3TaskRuntimeInteropCancellationLoweringLaneContract;
}

bool IsValidObjc3ConcurrencyReplayRaceGuardLoweringContract(
    const Objc3ConcurrencyReplayRaceGuardLoweringContract &contract) {
  if (contract.replay_proof_sites > contract.concurrency_replay_sites ||
      contract.race_guard_sites > contract.concurrency_replay_sites ||
      contract.task_handoff_sites > contract.concurrency_replay_sites ||
      contract.actor_isolation_sites > contract.concurrency_replay_sites ||
      contract.deterministic_schedule_sites > contract.concurrency_replay_sites ||
      contract.guard_blocked_sites > contract.concurrency_replay_sites ||
      contract.contract_violation_sites > contract.concurrency_replay_sites) {
    return false;
  }
  if (contract.deterministic_schedule_sites + contract.guard_blocked_sites !=
      contract.concurrency_replay_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ConcurrencyReplayRaceGuardLoweringReplayKey(
    const Objc3ConcurrencyReplayRaceGuardLoweringContract &contract) {
  return std::string("concurrency_replay_sites=") +
             std::to_string(contract.concurrency_replay_sites) +
         ";replay_proof_sites=" + std::to_string(contract.replay_proof_sites) +
         ";race_guard_sites=" + std::to_string(contract.race_guard_sites) +
         ";task_handoff_sites=" + std::to_string(contract.task_handoff_sites) +
         ";actor_isolation_sites=" +
         std::to_string(contract.actor_isolation_sites) +
         ";deterministic_schedule_sites=" +
         std::to_string(contract.deterministic_schedule_sites) +
         ";guard_blocked_sites=" +
         std::to_string(contract.guard_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3ConcurrencyReplayRaceGuardLoweringLaneContract;
}

bool IsValidObjc3UnsafePointerExtensionLoweringContract(
    const Objc3UnsafePointerExtensionLoweringContract &contract) {
  if (contract.unsafe_keyword_sites > contract.unsafe_pointer_extension_sites ||
      contract.pointer_arithmetic_sites >
          contract.unsafe_pointer_extension_sites ||
      contract.raw_pointer_type_sites > contract.unsafe_pointer_extension_sites ||
      contract.unsafe_operation_sites > contract.unsafe_pointer_extension_sites ||
      contract.normalized_sites > contract.unsafe_pointer_extension_sites ||
      contract.gate_blocked_sites > contract.unsafe_pointer_extension_sites ||
      contract.contract_violation_sites > contract.unsafe_pointer_extension_sites) {
    return false;
  }
  if (contract.normalized_sites + contract.gate_blocked_sites !=
      contract.unsafe_pointer_extension_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3UnsafePointerExtensionLoweringReplayKey(
    const Objc3UnsafePointerExtensionLoweringContract &contract) {
  return std::string("unsafe_pointer_extension_sites=") +
             std::to_string(contract.unsafe_pointer_extension_sites) +
         ";unsafe_keyword_sites=" +
         std::to_string(contract.unsafe_keyword_sites) +
         ";pointer_arithmetic_sites=" +
         std::to_string(contract.pointer_arithmetic_sites) +
         ";raw_pointer_type_sites=" +
         std::to_string(contract.raw_pointer_type_sites) +
         ";unsafe_operation_sites=" +
         std::to_string(contract.unsafe_operation_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";gate_blocked_sites=" + std::to_string(contract.gate_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3UnsafePointerExtensionLoweringLaneContract;
}

bool IsValidObjc3InlineAsmIntrinsicGovernanceLoweringContract(
    const Objc3InlineAsmIntrinsicGovernanceLoweringContract &contract) {
  if (contract.inline_asm_sites > contract.inline_asm_intrinsic_sites ||
      contract.intrinsic_sites > contract.inline_asm_intrinsic_sites ||
      contract.governed_intrinsic_sites > contract.intrinsic_sites ||
      contract.privileged_intrinsic_sites > contract.governed_intrinsic_sites ||
      contract.normalized_sites > contract.inline_asm_intrinsic_sites ||
      contract.gate_blocked_sites > contract.inline_asm_intrinsic_sites ||
      contract.contract_violation_sites > contract.inline_asm_intrinsic_sites) {
    return false;
  }
  if (contract.normalized_sites + contract.gate_blocked_sites !=
      contract.inline_asm_intrinsic_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3InlineAsmIntrinsicGovernanceLoweringReplayKey(
    const Objc3InlineAsmIntrinsicGovernanceLoweringContract &contract) {
  return std::string("inline_asm_intrinsic_sites=") +
             std::to_string(contract.inline_asm_intrinsic_sites) +
         ";inline_asm_sites=" + std::to_string(contract.inline_asm_sites) +
         ";intrinsic_sites=" + std::to_string(contract.intrinsic_sites) +
         ";governed_intrinsic_sites=" +
         std::to_string(contract.governed_intrinsic_sites) +
         ";privileged_intrinsic_sites=" +
         std::to_string(contract.privileged_intrinsic_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";gate_blocked_sites=" + std::to_string(contract.gate_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3InlineAsmIntrinsicGovernanceLoweringLaneContract;
}
