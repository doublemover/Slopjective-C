#include "artifacts/objc3_frontend_runtime_registration_artifacts.h"

#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>

#include "ast/objc3_ast_contracts.h"

namespace objc3::artifacts::frontend {

#include "artifacts/objc3_frontend_runtime_registration_artifacts_metadata_surface_summaries.inc"
#include "artifacts/objc3_frontend_runtime_registration_artifacts_runtime_support_library.inc"
#include "artifacts/objc3_frontend_runtime_registration_artifacts_translation_unit_contract.inc"
#include "artifacts/objc3_frontend_runtime_registration_artifacts_translation_unit_manifest.inc"

}  // namespace objc3::artifacts::frontend
