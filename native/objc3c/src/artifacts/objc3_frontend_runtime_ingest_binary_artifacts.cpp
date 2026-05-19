#include "artifacts/objc3_frontend_runtime_ingest_binary_artifacts.h"

#include <cstdint>
#include <sstream>
#include <string>

#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"
#include "ast/objc3_ast_contracts.h"
#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {

using objc3::io::EscapeJsonString;

#include "artifacts/objc3_frontend_runtime_ingest_binary_artifacts_binary_metadata.inc"
#include "artifacts/objc3_frontend_runtime_ingest_binary_artifacts_ingest_record.inc"
#include "artifacts/objc3_frontend_runtime_ingest_binary_artifacts_artifact_emission.inc"
#include "artifacts/objc3_frontend_runtime_ingest_binary_artifacts_replay_readiness.inc"

}  // namespace objc3::artifacts::frontend
