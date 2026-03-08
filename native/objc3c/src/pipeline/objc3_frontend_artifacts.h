#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "pipeline/objc3_frontend_types.h"

struct Objc3FrontendArtifactBundle {
  Objc3FrontendDiagnosticsBus stage_diagnostics;
  Objc3ParseLoweringReadinessSurface parse_lowering_readiness_surface;
  std::vector<std::string> post_pipeline_diagnostics;
  std::vector<std::string> diagnostics;
  std::string manifest_json;
  std::string runtime_metadata_binary;
  Objc3RuntimeTranslationUnitRegistrationManifestSummary
      runtime_translation_unit_registration_manifest_summary;
  Objc3RuntimeBootstrapSemanticsSummary runtime_bootstrap_semantics_summary;
  std::string ir_text;
};

Objc3FrontendArtifactBundle BuildObjc3FrontendArtifacts(const std::filesystem::path &input_path,
                                                        const Objc3FrontendPipelineResult &pipeline_result,
                                                        const Objc3FrontendOptions &options);
