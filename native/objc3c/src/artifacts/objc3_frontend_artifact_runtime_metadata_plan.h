#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

struct Objc3FrontendArtifactRuntimeMetadataPlan {
  Objc3RuntimeMetadataSectionAbiFreezeSummary runtime_metadata_section_abi;
  Objc3RuntimeMetadataSectionPublicationSummary
      runtime_metadata_section_publication;
  Objc3RuntimeMetadataObjectInspectionHarnessSummary
      runtime_metadata_object_inspection;
  Objc3RuntimeMetadataSourceToSectionMatrixSummary
      runtime_metadata_source_to_section_matrix;
  Objc3ExecutableMetadataDebugProjectionSummary
      executable_metadata_debug_projection;
  Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
      executable_metadata_runtime_ingest_packaging_contract;
  std::string executable_metadata_runtime_ingest_binary_payload;
  Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary
      executable_metadata_runtime_ingest_binary_boundary;
};

Objc3FrontendArtifactRuntimeMetadataPlan
BuildObjc3FrontendArtifactRuntimeMetadataPlan(
    const Objc3FrontendPipelineResult &pipeline_result);
