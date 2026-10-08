#include "sema/objc3_semantic_passes.h"
#include "ast/objc3_ast_constant_evaluator.h"
#include "sema/objc3_semantic_collection_helpers.h"
#include "sema/objc3_semantic_context.h"
#include "sema/objc3_semantic_feature_claims.h"
#include "sema/objc3_semantic_generic_collection_type_model.h"
#include "sema/objc3_semantic_generic_argument_relations.h"
#include "sema/objc3_semantic_interop_surface_helpers.h"
#include "sema/objc3_semantic_match_exhaustiveness.h"
#include "sema/objc3_semantic_protocol_composition_parser.h"
#include "sema/objc3_semantic_signature_compatibility.h"
#include "sema/objc3_semantic_symbol_lookup.h"
#include "sema/objc3_semantic_type_factory.h"
#include "sema/objc3_semantic_type_predicates.h"
#include "sema/objc3_semantic_type_helpers.h"
#include "sema/objc3_semantic_type_relations.h"
#include "sema/objc3_static_analysis.h"
#include "sema/objc3_type_form_scaffold.h"
#include "pipeline/objc3_frontend_types.h"

#include "diag/objc3_diag_utils.h"
#include "support/objc3_concurrency_symbol_profiles.h"
#include "support/objc3_method_family.h"
#include "support/objc3_property_storage_profile_helpers.h"
#include "support/objc3_receiver_profile_helpers.h"
#include "support/objc3_string_predicates.h"
#include "support/objc3_type_profile_helpers.h"
#include "support/objc3_value_type_names.h"

#include <algorithm>
#include <limits>
#include <map>
#include <optional>
#include <sstream>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using objc3c::support::IsObjCReferenceAliasValueType;

struct Objc3ProtocolSemanticDefinition;

static Objc3SemanticCanonicalType BuildCanonicalSemanticTypeFromParam(
    const FuncParam &param,
    const std::unordered_set<std::string> *generic_interface_names = nullptr);
static Objc3SemanticCanonicalType BuildCanonicalSemanticTypeFromFunctionReturn(
    const FunctionDecl &fn,
    const std::unordered_set<std::string> *generic_interface_names = nullptr);
static Objc3SemanticCanonicalType BuildCanonicalSemanticTypeFromMethodReturn(
    const Objc3MethodDecl &method,
    const std::unordered_set<std::string> *generic_interface_names = nullptr);
static Objc3SemanticCanonicalType BuildCanonicalSemanticTypeFromProperty(
    const Objc3PropertyDecl &property);

#include "sema/objc3_semantic_passes_generic_callable_contracts.inc"

#include "sema/objc3_semantic_passes_type_baseline_semantic_type_accounting.inc"
#include "sema/objc3_semantic_passes_type_baseline_bootstrap_publication.inc"
#include "sema/objc3_semantic_passes_feature_claim_strictness_publication.inc"
#include "sema/objc3_semantic_passes_feature_claim_unsupported_validation.inc"
