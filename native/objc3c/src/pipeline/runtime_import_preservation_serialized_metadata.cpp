#include "pipeline/runtime_import_preservation_owners.h"

#include <string>
#include <utility>

#include "pipeline/runtime_import_frontend_closure_boundary.h"
#include "pipeline/runtime_import_record_parsing.h"
#include "support/objc3_runtime_metadata_record_set.h"

namespace objc3c::pipeline::runtime_import_preservation {

bool PopulateSerializedRuntimeMetadataReuse(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  Objc3RuntimeMetadataSourceRecordSet local_runtime_metadata_source_records;
  if (!ParseRuntimeMetadataSourceRecordSet(root, "runtime_owned_declarations",
                                           local_runtime_metadata_source_records,
                                           error)) {
    return false;
  }
  PreserveImportedRuntimeMetadataSourceRecordInventory(
      surface.frontend_closure_summary, local_runtime_metadata_source_records);

  const RuntimeImportJsonValue *references_value =
      FindMember(root, "metadata_references");
  if (references_value == nullptr) {
    error = "missing JSON array member 'metadata_references'";
    return false;
  }
  const RuntimeImportJsonValue::Array *references_array =
      AsArray(*references_value);
  if (references_array == nullptr) {
    error = "JSON member 'metadata_references' must be an array";
    return false;
  }

  surface.frontend_closure_summary.superclass_reference_count = 0;
  surface.frontend_closure_summary.protocol_reference_count = 0;
  surface.frontend_closure_summary.property_accessor_reference_count = 0;
  surface.frontend_closure_summary.property_ivar_binding_reference_count = 0;
  surface.frontend_closure_summary.method_selector_reference_count = 0;
  for (const RuntimeImportJsonValue &reference_value : *references_array) {
    const RuntimeImportJsonValue::Object *reference_object =
        AsObject(reference_value);
    if (reference_object == nullptr) {
      error = "metadata_references must contain objects";
      return false;
    }
    std::string reference_kind;
    if (!ReadStringMember(*reference_object, "reference_kind", reference_kind,
                          error)) {
      return false;
    }
    if (reference_kind == "class-superclass") {
      ++surface.frontend_closure_summary.superclass_reference_count;
    } else if (reference_kind == "class-adopted-protocol" ||
               reference_kind == "protocol-inherited-protocol" ||
               reference_kind == "category-adopted-protocol") {
      ++surface.frontend_closure_summary.protocol_reference_count;
    } else if (reference_kind == "property-getter-selector" ||
               reference_kind == "property-setter-selector") {
      ++surface.frontend_closure_summary.property_accessor_reference_count;
    } else if (reference_kind == "property-ivar-binding") {
      ++surface.frontend_closure_summary.property_ivar_binding_reference_count;
    } else if (reference_kind == "method-selector") {
      ++surface.frontend_closure_summary.method_selector_reference_count;
    }
  }

  if (!ValidateImportedRuntimeFrontendClosureInventory(
          surface.frontend_closure_summary,
          local_runtime_metadata_source_records, references_array->size(),
          error) ||
      !ValidateImportedRuntimeFrontendClosureHardCutoverBoundary(
          surface.frontend_closure_summary, error)) {
    return false;
  }
  if (!ParseSerializedRuntimeMetadataReusePayload(root, surface, error)) {
    return false;
  }
  if (!surface.uses_serialized_runtime_metadata_payload) {
    surface.runtime_metadata_source_records =
        std::move(local_runtime_metadata_source_records);
  }
  return true;
}

}  // namespace objc3c::pipeline::runtime_import_preservation
