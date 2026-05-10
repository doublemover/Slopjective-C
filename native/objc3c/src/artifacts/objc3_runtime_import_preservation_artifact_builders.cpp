#include "artifacts/objc3_runtime_import_preservation_artifact_builders.h"

#include <cstddef>
#include <sstream>

#include "io/objc3_json.h"
#include "support/objc3_runtime_metadata_record_set.h"

namespace objc3::artifacts::frontend::runtime_import_preservation {
namespace {

using objc3::io::EscapeJsonString;

#include "artifacts/objc3_runtime_import_preservation_artifact_builders_surface_contract.inc"
#include "artifacts/objc3_runtime_import_preservation_artifact_builders_frontend_closure_counts.inc"

}  // namespace

#include "artifacts/objc3_runtime_import_preservation_artifact_builders_surface_artifact.inc"
#include "artifacts/objc3_runtime_import_preservation_artifact_builders_frontend_closure_replay_key.inc"
#include "artifacts/objc3_runtime_import_preservation_artifact_builders_frontend_closure_summary.inc"

}  // namespace objc3::artifacts::frontend::runtime_import_preservation
