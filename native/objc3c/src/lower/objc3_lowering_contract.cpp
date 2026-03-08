#include "lower/objc3_lowering_contract.h"

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

bool IsValidObjc3OwnershipQualifierLoweringContract(
    const Objc3OwnershipQualifierLoweringContract &contract) {
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
  if (contract.mutable_capture_count_total != contract.capture_entries_total ||
      contract.byref_slot_count_total != contract.capture_entries_total ||
      contract.escape_analysis_enabled_sites != contract.block_literal_sites ||
      contract.requires_byref_cells_sites != contract.escape_to_heap_sites) {
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
  if (contract.mutable_capture_count_total != contract.capture_entries_total ||
      contract.byref_slot_count_total != contract.capture_entries_total ||
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
