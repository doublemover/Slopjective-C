#include "artifacts/objc3_frontend_interop_semantic_artifacts.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "artifacts/objc3_frontend_artifact_semantic_contract_records.h"
#include "ast/objc3_ast_declarations.h"
#include "pipeline/objc3_frontend_types.h"
#include "pipeline/objc3_runtime_import_surface.h"

namespace objc3::artifacts::frontend {

#include "artifacts/objc3_frontend_interop_interface_preservation_inventory.inc"
#include "artifacts/objc3_frontend_interop_interface_preservation_fields.inc"
#include "artifacts/objc3_frontend_interop_interface_preservation_replay_readiness.inc"
#include "artifacts/objc3_frontend_interop_interface_preservation_artifact_emission.inc"

}  // namespace objc3::artifacts::frontend
