#include "artifacts/objc3_frontend_metaprogramming_semantic_artifacts.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include "artifacts/objc3_frontend_artifact_semantic_contract_records.h"
#include "ast/objc3_ast_declarations.h"

namespace objc3::artifacts::frontend {
namespace {

#include "artifacts/objc3_frontend_metaprogramming_artifact_bundle_helpers.inc"

}  // namespace

#include "artifacts/objc3_frontend_metaprogramming_derived_method_bundles.inc"
#include "artifacts/objc3_frontend_metaprogramming_macro_artifact_bundles.inc"
#include "artifacts/objc3_frontend_metaprogramming_property_behavior_bundles.inc"

}  // namespace objc3::artifacts::frontend
