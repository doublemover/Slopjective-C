#include "lower/contracts/runtime_metadata_emission_contracts.h"

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
      << ";category_attachment_model=" << kObjc3RuntimeCategoryAttachmentModel
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
