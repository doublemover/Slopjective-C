#include "artifacts/objc3_frontend_dispatch_semantic_artifacts.h"

#include <algorithm>
#include <cstddef>
#include <string>

#include "artifacts/objc3_frontend_artifact_dispatch_contract_constants.h"

namespace objc3::artifacts::frontend {
namespace {

#include "objc3_frontend_dispatch_semantic_record_collection.inc"
#include "objc3_frontend_dispatch_semantic_strict_type_evidence.inc"

}  // namespace

#include "objc3_frontend_dispatch_semantic_control_lowering.inc"
#include "objc3_frontend_dispatch_semantic_type_surface_contracts.inc"
#include "objc3_frontend_dispatch_semantic_readiness_runtime_contracts.inc"

}  // namespace objc3::artifacts::frontend
