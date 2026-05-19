#include "io/objc3_runtime_metadata_discovery_document.h"

#include <sstream>

#include "io/objc3_process_internal.h"

std::string BuildObjc3RuntimeMetadataLinkerRetentionDiscoveryJson(
    const Objc3RuntimeMetadataLinkerRetentionArtifacts &artifacts) {
  std::ostringstream discovery;
  JsonObjectWriter discovery_document(discovery);
  discovery_document.StringField("contract_id",
                                 kObjc3RuntimeLinkerRetentionContractId);
  discovery_document.StringField("object_format", artifacts.object_format);
  discovery_document.StringField("object_artifact",
                                 artifacts.object_artifact_relative_path);
  discovery_document.StringField("linker_anchor_symbol",
                                 artifacts.linker_anchor_symbol);
  discovery_document.StringField("discovery_root_symbol",
                                 artifacts.discovery_root_symbol);
  discovery_document.StringField("linker_anchor_logical_section",
                                 artifacts.linker_anchor_logical_section);
  discovery_document.StringField("discovery_root_logical_section",
                                 artifacts.discovery_root_logical_section);
  discovery_document.StringField("linker_anchor_emitted_section",
                                 artifacts.linker_anchor_emitted_section);
  discovery_document.StringField("discovery_root_emitted_section",
                                 artifacts.discovery_root_emitted_section);
  discovery_document.StringField("linker_response_artifact_suffix",
                                 artifacts.linker_response_artifact_suffix);
  discovery_document.StringField("discovery_artifact_suffix",
                                 artifacts.discovery_artifact_suffix);
  discovery_document.StringField("translation_unit_identity_model",
                                 artifacts.translation_unit_identity_model);
  discovery_document.StringField("translation_unit_identity_key",
                                 artifacts.translation_unit_identity_key);
  discovery_document.StringArrayField("driver_linker_flags",
                                      {artifacts.driver_linker_flag});
  return FinishJsonObject(discovery_document, discovery);
}
