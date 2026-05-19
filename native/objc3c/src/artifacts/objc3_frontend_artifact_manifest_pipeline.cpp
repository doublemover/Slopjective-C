#include "artifacts/objc3_frontend_artifact_manifest_pipeline.h"

#include <ostream>

#include "artifacts/json/program_manifest_json.h"
#include "artifacts/json/runtime_metadata_manifest_json.h"
#include "artifacts/json/semantic_type_manifest_json.h"
#include "artifacts/objc3_frontend_artifacts.h"
#include "artifacts/objc3_frontend_parser_diagnostic_artifacts.h"

namespace objc3::artifacts::frontend {

#include "artifacts/objc3_frontend_artifact_manifest_pipeline_stage_fields.inc"
#include "artifacts/objc3_frontend_artifact_manifest_sema_pass_diagnostics.inc"
#include "artifacts/objc3_frontend_artifact_manifest_lowering_header.inc"
#include "artifacts/objc3_frontend_artifact_manifest_record_arrays.inc"

}  // namespace objc3::artifacts::frontend
