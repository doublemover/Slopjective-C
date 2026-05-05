#include "sema/objc3_semantic_passes.h"
#include "sema/objc3_static_analysis.h"
#include "sema/objc3_type_form_scaffold.h"
#include "pipeline/objc3_frontend_types.h"

#include "diag/objc3_diag_utils.h"
#include "support/objc3_method_family.h"
#include "support/objc3_value_type_names.h"

#include <algorithm>
#include <cctype>
#include <limits>
#include <map>
#include <optional>
#include <sstream>
#include <type_traits>
#include <unordered_set>
#include <vector>

static std::string JoinStringVector(const std::vector<std::string> &items,
                                    const std::string &separator) {
  std::ostringstream out;
  for (std::size_t index = 0; index < items.size(); ++index) {
    if (index != 0u) {
      out << separator;
    }
    out << items[index];
  }
  return out.str();
}

static bool IsObjCReferenceAliasValueType(ValueType type);
struct Objc3ProtocolSemanticDefinition;

enum class SemanticOwnershipKind {
  None,
  Retained,
  Weak,
  Unowned,
};

struct SemanticTypeInfo {
  ValueType type = ValueType::Unknown;
  Objc3SemanticCanonicalType canonical_type;
  bool is_vector = false;
  std::string vector_base_spelling;
  unsigned vector_lane_count = 1;
  bool is_callable = false;
  std::vector<ValueType> callable_param_types;
  ValueType callable_return_type = ValueType::Unknown;
  bool callable_block_runtime_handle_candidate = false;
  bool callable_block_runtime_handle_has_byref_capture = false;
  bool callable_block_runtime_handle_has_owned_object_capture = false;
  std::string callable_block_runtime_first_byref_capture_name;
  std::string callable_block_runtime_first_owned_capture_name;
  SemanticOwnershipKind ownership_kind = SemanticOwnershipKind::None;
  bool has_nullability_suffix = false;
  bool is_refined_nonnull_reference = false;
  std::string object_pointer_type_name;
};

using SemanticScope = std::unordered_map<std::string, SemanticTypeInfo>;

static Objc3SemanticCanonicalType BuildCanonicalSemanticTypeFromParam(
    const FuncParam &param);
static Objc3SemanticCanonicalType BuildCanonicalSemanticTypeFromFunctionReturn(
    const FunctionDecl &fn);
static Objc3SemanticCanonicalType BuildCanonicalSemanticTypeFromMethodReturn(
    const Objc3MethodDecl &method);
static Objc3SemanticCanonicalType BuildCanonicalSemanticTypeFromProperty(
    const Objc3PropertyDecl &property);

struct OwnershipResourceMoveBindingState {
  bool cleanup_owned = false;
  bool moved = false;
  unsigned move_line = 1;
  unsigned move_column = 1;
};

using OwnershipResourceMoveScope =
    std::unordered_map<std::string, OwnershipResourceMoveBindingState>;

static OwnershipResourceMoveBindingState *LookupOwnershipResourceMoveBinding(
    std::vector<OwnershipResourceMoveScope> &scopes, const std::string &name) {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    auto found = it->find(name);
    if (found != it->end()) {
      return &found->second;
    }
  }
  return nullptr;
}

static const OwnershipResourceMoveBindingState *LookupOwnershipResourceMoveBinding(
    const std::vector<OwnershipResourceMoveScope> &scopes,
    const std::string &name) {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    auto found = it->find(name);
    if (found != it->end()) {
      return &found->second;
    }
  }
  return nullptr;
}

static SemanticTypeInfo ScopeLookupType(
    const std::vector<SemanticScope> &scopes, const std::string &name);

static SemanticTypeInfo MakeScalarSemanticType(ValueType type) {
  SemanticTypeInfo info;
  info.type = type;
  info.canonical_type.value_type = type;
  info.canonical_type.kind =
      IsObjCReferenceAliasValueType(type)
          ? Objc3SemanticCanonicalTypeKind::Object
          : Objc3SemanticCanonicalTypeKind::Scalar;
  info.canonical_type.canonical_spelling = objc3c::support::ValueTypeName(type);
  if (IsObjCReferenceAliasValueType(type)) {
    info.ownership_kind = SemanticOwnershipKind::Retained;
  }
  return info;
}

static SemanticTypeInfo MakeVectorSemanticType(ValueType base_type, const std::string &base_spelling,
                                               unsigned lane_count) {
  SemanticTypeInfo info;
  info.type = base_type;
  info.canonical_type.value_type = base_type;
  info.canonical_type.kind = Objc3SemanticCanonicalTypeKind::Vector;
  info.canonical_type.is_vector = true;
  info.canonical_type.vector_base_spelling = base_spelling;
  info.canonical_type.vector_lane_count = lane_count;
  info.canonical_type.canonical_spelling =
      "vector<" + base_spelling + "," + std::to_string(lane_count) + ">";
  info.is_vector = true;
  info.vector_base_spelling = base_spelling;
  info.vector_lane_count = lane_count;
  return info;
}

static SemanticTypeInfo MakeSemanticTypeFromParam(const FuncParam &param) {
  if (param.vector_spelling) {
    return MakeVectorSemanticType(param.type, param.vector_base_spelling, param.vector_lane_count);
  }
  SemanticTypeInfo info = MakeScalarSemanticType(param.type);
  info.canonical_type = BuildCanonicalSemanticTypeFromParam(param);
  if (param.object_pointer_type_spelling) {
    info.object_pointer_type_name = param.object_pointer_type_name;
  }
  if (IsObjCReferenceAliasValueType(param.type)) {
    if (param.ownership_is_weak_reference) {
      info.ownership_kind = SemanticOwnershipKind::Weak;
    } else if (param.ownership_is_unowned_reference) {
      info.ownership_kind = SemanticOwnershipKind::Unowned;
    } else {
      info.ownership_kind = SemanticOwnershipKind::Retained;
    }
    info.has_nullability_suffix = !param.nullability_suffix_tokens.empty();
  }
  return info;
}

static SemanticTypeInfo MakeSemanticTypeFromFunctionReturn(const FunctionDecl &fn) {
  if (fn.return_vector_spelling) {
    return MakeVectorSemanticType(fn.return_type, fn.return_vector_base_spelling, fn.return_vector_lane_count);
  }
  SemanticTypeInfo info = MakeScalarSemanticType(fn.return_type);
  info.canonical_type = BuildCanonicalSemanticTypeFromFunctionReturn(fn);
  if (fn.return_object_pointer_type_spelling) {
    info.object_pointer_type_name = fn.return_object_pointer_type_name;
  }
  if (IsObjCReferenceAliasValueType(fn.return_type)) {
    if (fn.return_ownership_is_weak_reference) {
      info.ownership_kind = SemanticOwnershipKind::Weak;
    } else if (fn.return_ownership_is_unowned_reference) {
      info.ownership_kind = SemanticOwnershipKind::Unowned;
    } else {
      info.ownership_kind = SemanticOwnershipKind::Retained;
    }
    info.has_nullability_suffix = !fn.return_nullability_suffix_tokens.empty();
  }
  return info;
}

static SemanticTypeInfo MakeSemanticTypeFromMethodReturn(const Objc3MethodDecl &method) {
  if (method.return_vector_spelling) {
    return MakeVectorSemanticType(method.return_type, method.return_vector_base_spelling, method.return_vector_lane_count);
  }
  SemanticTypeInfo info = MakeScalarSemanticType(method.return_type);
  info.canonical_type = BuildCanonicalSemanticTypeFromMethodReturn(method);
  if (method.return_object_pointer_type_spelling) {
    info.object_pointer_type_name = method.return_object_pointer_type_name;
  }
  if (IsObjCReferenceAliasValueType(method.return_type)) {
    if (method.return_ownership_is_weak_reference) {
      info.ownership_kind = SemanticOwnershipKind::Weak;
    } else if (method.return_ownership_is_unowned_reference) {
      info.ownership_kind = SemanticOwnershipKind::Unowned;
    } else {
      info.ownership_kind = SemanticOwnershipKind::Retained;
    }
    info.has_nullability_suffix = !method.return_nullability_suffix_tokens.empty();
  }
  return info;
}

static SemanticTypeInfo MakeSemanticTypeFromFunctionInfoParam(const FunctionInfo &fn, std::size_t index) {
  if (index >= fn.param_types.size()) {
    return MakeScalarSemanticType(ValueType::Unknown);
  }

  if (index < fn.param_is_vector.size() && fn.param_is_vector[index]) {
    const std::string base_spelling =
        index < fn.param_vector_base_spelling.size() ? fn.param_vector_base_spelling[index] : "";
    const unsigned lane_count = index < fn.param_vector_lane_count.size() ? fn.param_vector_lane_count[index] : 1u;
    return MakeVectorSemanticType(fn.param_types[index], base_spelling, lane_count);
  }
  SemanticTypeInfo info = MakeScalarSemanticType(fn.param_types[index]);
  if (index < fn.param_canonical_types.size()) {
    info.canonical_type = fn.param_canonical_types[index];
  }
  if (IsObjCReferenceAliasValueType(fn.param_types[index])) {
    if (index < fn.param_ownership_is_weak_reference.size() &&
        fn.param_ownership_is_weak_reference[index]) {
      info.ownership_kind = SemanticOwnershipKind::Weak;
    } else if (index < fn.param_ownership_is_unowned_reference.size() &&
               fn.param_ownership_is_unowned_reference[index]) {
      info.ownership_kind = SemanticOwnershipKind::Unowned;
    } else {
      info.ownership_kind = SemanticOwnershipKind::Retained;
    }
    info.has_nullability_suffix =
        index < fn.param_has_nullability_suffix.size() &&
        fn.param_has_nullability_suffix[index];
  }
  return info;
}

static SemanticTypeInfo MakeSemanticTypeFromFunctionInfoReturn(const FunctionInfo &fn) {
  if (fn.return_is_vector) {
    return MakeVectorSemanticType(fn.return_type, fn.return_vector_base_spelling, fn.return_vector_lane_count);
  }
  SemanticTypeInfo info = MakeScalarSemanticType(fn.return_type);
  info.canonical_type = fn.return_canonical_type;
  if (IsObjCReferenceAliasValueType(fn.return_type)) {
    if (fn.return_ownership_is_weak_reference) {
      info.ownership_kind = SemanticOwnershipKind::Weak;
    } else if (fn.return_ownership_is_unowned_reference) {
      info.ownership_kind = SemanticOwnershipKind::Unowned;
    } else {
      info.ownership_kind = SemanticOwnershipKind::Retained;
    }
    info.has_nullability_suffix = fn.return_has_nullability_suffix;
  }
  return info;
}

static SemanticTypeInfo MakeSemanticTypeFromMethodInfoParam(const Objc3MethodInfo &method,
                                                            std::size_t index) {
  if (index >= method.param_types.size()) {
    return MakeScalarSemanticType(ValueType::Unknown);
  }
  if (index < method.param_is_vector.size() && method.param_is_vector[index]) {
    const std::string base_spelling =
        index < method.param_vector_base_spelling.size()
            ? method.param_vector_base_spelling[index]
            : "";
    const unsigned lane_count =
        index < method.param_vector_lane_count.size()
            ? method.param_vector_lane_count[index]
            : 1u;
    return MakeVectorSemanticType(method.param_types[index], base_spelling,
                                  lane_count);
  }
  SemanticTypeInfo info = MakeScalarSemanticType(method.param_types[index]);
  if (index < method.param_canonical_types.size()) {
    info.canonical_type = method.param_canonical_types[index];
  }
  if (IsObjCReferenceAliasValueType(method.param_types[index])) {
    if (index < method.param_ownership_is_weak_reference.size() &&
        method.param_ownership_is_weak_reference[index]) {
      info.ownership_kind = SemanticOwnershipKind::Weak;
    } else if (index < method.param_ownership_is_unowned_reference.size() &&
               method.param_ownership_is_unowned_reference[index]) {
      info.ownership_kind = SemanticOwnershipKind::Unowned;
    } else {
      info.ownership_kind = SemanticOwnershipKind::Retained;
    }
    info.has_nullability_suffix =
        index < method.param_has_nullability_suffix.size() &&
        method.param_has_nullability_suffix[index];
  }
  return info;
}

static SemanticTypeInfo MakeSemanticTypeFromMethodInfoReturn(
    const Objc3MethodInfo &method) {
  if (method.return_is_vector) {
    return MakeVectorSemanticType(method.return_type,
                                  method.return_vector_base_spelling,
                                  method.return_vector_lane_count);
  }
  SemanticTypeInfo info = MakeScalarSemanticType(method.return_type);
  info.canonical_type = method.return_canonical_type;
  if (IsObjCReferenceAliasValueType(method.return_type)) {
    if (method.return_ownership_is_weak_reference) {
      info.ownership_kind = SemanticOwnershipKind::Weak;
    } else if (method.return_ownership_is_unowned_reference) {
      info.ownership_kind = SemanticOwnershipKind::Unowned;
    } else {
      info.ownership_kind = SemanticOwnershipKind::Retained;
    }
    info.has_nullability_suffix = method.return_has_nullability_suffix;
  }
  return info;
}

static SemanticTypeInfo MakeSemanticTypeFromPropertyInfo(
    const Objc3PropertyInfo &property) {
  if (property.is_vector) {
    return MakeVectorSemanticType(property.type, property.vector_base_spelling,
                                  property.vector_lane_count);
  }
  SemanticTypeInfo info = MakeScalarSemanticType(property.type);
  info.canonical_type = property.canonical_type;
  if (IsObjCReferenceAliasValueType(property.type)) {
    if (property.ownership_is_weak_reference) {
      info.ownership_kind = SemanticOwnershipKind::Weak;
    } else if (property.ownership_is_unowned_reference) {
      info.ownership_kind = SemanticOwnershipKind::Unowned;
    } else {
      info.ownership_kind = SemanticOwnershipKind::Retained;
    }
    info.has_nullability_suffix = property.has_nullability_suffix;
  }
  return info;
}

static SemanticTypeInfo MakeSemanticTypeFromGlobal(ValueType type) {
  return MakeScalarSemanticType(type);
}

static SemanticTypeInfo MakeCallableSemanticType(std::vector<ValueType> param_types,
                                                 ValueType return_type) {
  SemanticTypeInfo info;
  info.type = ValueType::Function;
  info.canonical_type.value_type = ValueType::Function;
  info.canonical_type.kind = Objc3SemanticCanonicalTypeKind::Block;
  info.canonical_type.canonical_spelling = "block";
  info.canonical_type.replay_key = "value=Function;kind=block";
  info.is_callable = true;
  info.callable_param_types = std::move(param_types);
  info.callable_return_type = return_type;
  return info;
}

static SemanticTypeInfo MakeCallableSemanticTypeFromBlockLiteral(const Expr &expr) {
  std::vector<ValueType> param_types = expr.block_parameter_types_source_order;
  if (param_types.size() < expr.block_parameter_count) {
    param_types.resize(expr.block_parameter_count, ValueType::Unknown);
  }
  SemanticTypeInfo info =
      MakeCallableSemanticType(std::move(param_types), ValueType::Unknown);
  info.callable_block_runtime_handle_candidate =
      expr.block_escape_shape_promotes_to_heap_candidate;
  info.callable_block_runtime_handle_has_byref_capture =
      expr.block_byref_capture_count > 0u;
  info.callable_block_runtime_handle_has_owned_object_capture =
      expr.block_runtime_owned_object_capture_count > 0u;
  if (!expr.block_byref_capture_names_lexicographic.empty()) {
    info.callable_block_runtime_first_byref_capture_name =
        expr.block_byref_capture_names_lexicographic.front();
  }
  if (!expr.block_runtime_owned_object_capture_names_lexicographic.empty()) {
    info.callable_block_runtime_first_owned_capture_name =
        expr.block_runtime_owned_object_capture_names_lexicographic.front();
  }
  return info;
}

static bool IsUnknownSemanticType(const SemanticTypeInfo &info) {
  return !info.is_vector && info.type == ValueType::Unknown;
}

static bool IsCallableSemanticType(const SemanticTypeInfo &info) {
  return !info.is_vector && info.type == ValueType::Function && info.is_callable;
}

static bool IsScalarSemanticType(const SemanticTypeInfo &info) {
  return !info.is_vector;
}

static bool IsScalarBoolCompatibleType(const SemanticTypeInfo &info) {
  return !info.is_vector && (info.type == ValueType::Bool || info.type == ValueType::I32);
}

static bool IsObjCReferenceAliasValueType(ValueType type) {
  switch (type) {
    case ValueType::ObjCId:
    case ValueType::ObjCClass:
    case ValueType::ObjCSel:
    case ValueType::ObjCProtocol:
    case ValueType::ObjCInstancetype:
    case ValueType::ObjCObjectPtr:
      return true;
    default:
      return false;
  }
}

static bool IsScalarI32CompatibleType(const SemanticTypeInfo &info) {
  return !info.is_vector && (info.type == ValueType::I32 || IsObjCReferenceAliasValueType(info.type));
}

static bool AreScalarI32AliasCompatible(const SemanticTypeInfo &lhs, const SemanticTypeInfo &rhs) {
  return IsScalarI32CompatibleType(lhs) && IsScalarI32CompatibleType(rhs);
}

static bool IsCanonicalObjc3TypeFormScaffoldReady() {
  static const Objc3TypeFormScaffoldSummary summary = BuildObjc3TypeFormScaffoldSummary();
  return IsReadyObjc3TypeFormScaffoldSummary(summary);
}

static bool IsObjCReferenceValueType(ValueType type) {
  if (!IsCanonicalObjc3TypeFormScaffoldReady()) {
    return false;
  }
  return IsObjc3CanonicalReferenceTypeForm(type);
}

static bool IsObjCReferenceSemanticType(const SemanticTypeInfo &info) {
  return !info.is_vector &&
         (IsObjCReferenceValueType(info.type) ||
          IsObjCReferenceAliasValueType(info.type));
}

static bool AreObjCReferenceTypesAssignmentCompatible(
    const SemanticTypeInfo &target, const SemanticTypeInfo &value,
    const Objc3SemanticIntegrationSurface *surface = nullptr);
static std::optional<bool> AreGenericSpecializationsVarianceAssignmentCompatible(
    const SemanticTypeInfo &target, const SemanticTypeInfo &value,
    const Objc3SemanticIntegrationSurface *surface);

static bool IsNullableObjCReferenceSemanticType(const SemanticTypeInfo &info) {
  return IsObjCReferenceSemanticType(info) &&
         !info.is_refined_nonnull_reference &&
         (info.canonical_type.nullability ==
              Objc3SemanticCanonicalNullability::Nullable ||
          info.canonical_type.nullability ==
              Objc3SemanticCanonicalNullability::ImplicitlyUnwrapped ||
          (info.has_nullability_suffix &&
           info.canonical_type.nullability ==
               Objc3SemanticCanonicalNullability::Unspecified));
}

static bool IsNonnullDestinationObjCReferenceSemanticType(
    const SemanticTypeInfo &info) {
  return IsObjCReferenceSemanticType(info) &&
         (info.canonical_type.nullability ==
              Objc3SemanticCanonicalNullability::Nonnull ||
          info.canonical_type.nullability ==
              Objc3SemanticCanonicalNullability::ImplicitlyUnwrapped ||
          info.canonical_type.nullability ==
              Objc3SemanticCanonicalNullability::NullResettable);
}

static bool IsUnsafeNullableToNonnullFlow(const SemanticTypeInfo &target,
                                          const SemanticTypeInfo &value,
                                          const Objc3SemanticIntegrationSurface *surface = nullptr) {
  return IsNonnullDestinationObjCReferenceSemanticType(target) &&
         IsNullableObjCReferenceSemanticType(value) &&
         AreObjCReferenceTypesAssignmentCompatible(target, value, surface);
}

static bool IsVoidSemanticType(const SemanticTypeInfo &info) {
  return !info.is_vector && info.type == ValueType::Void;
}

static SemanticTypeInfo MakeNonnullRefinedSemanticType(
    const SemanticTypeInfo &info) {
  SemanticTypeInfo refined = info;
  if (IsObjCReferenceSemanticType(refined)) {
    refined.has_nullability_suffix = false;
    refined.is_refined_nonnull_reference = true;
    refined.canonical_type.nullability =
        Objc3SemanticCanonicalNullability::Nonnull;
  }
  return refined;
}

static bool IsOwnedObjCReferenceSemanticType(const SemanticTypeInfo &info) {
  return IsObjCReferenceSemanticType(info) &&
         info.ownership_kind == SemanticOwnershipKind::Retained;
}

static bool IsWeakObjCReferenceSemanticType(const SemanticTypeInfo &info) {
  return IsObjCReferenceSemanticType(info) &&
         info.ownership_kind == SemanticOwnershipKind::Weak;
}

static bool IsUnownedObjCReferenceSemanticType(const SemanticTypeInfo &info) {
  return IsObjCReferenceSemanticType(info) &&
         info.ownership_kind == SemanticOwnershipKind::Unowned;
}

static bool IsMessageCompatibleType(const SemanticTypeInfo &info) {
  if (!IsCanonicalObjc3TypeFormScaffoldReady()) {
    return false;
  }
  return !info.is_vector && IsObjc3CanonicalMessageSendTypeForm(info.type);
}

static bool AreObjCReferenceTypesAssignmentCompatible(const SemanticTypeInfo &target,
                                                      const SemanticTypeInfo &value,
                                                      const Objc3SemanticIntegrationSurface *surface) {
  if (!IsCanonicalObjc3TypeFormScaffoldReady()) {
    return false;
  }
  if (!IsObjCReferenceSemanticType(target) || !IsObjCReferenceSemanticType(value)) {
    return false;
  }
  if (const std::optional<bool> variance_compatibility =
          AreGenericSpecializationsVarianceAssignmentCompatible(target, value,
                                                               surface)) {
    return *variance_compatibility;
  }
  if (target.type == value.type) {
    return true;
  }
  // Canonical ObjC bridge-top forms are assignment-compatible by design.
  if (IsObjc3CanonicalBridgeTopReferenceTypeForm(target.type) ||
      IsObjc3CanonicalBridgeTopReferenceTypeForm(value.type)) {
    return true;
  }
  return false;
}

static std::string BuildBootstrapLegalityFailureContractReplayKey(
    const Objc3BootstrapLegalityFailureContractSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id << ";surface_path=" << summary.surface_path
      << ";duplicate_registration_policy="
      << summary.duplicate_registration_policy
      << ";image_registration_order_invariant="
      << summary.image_registration_order_invariant
      << ";failure_mode=" << summary.failure_mode
      << ";restart_lifecycle_model=" << summary.restart_lifecycle_model
      << ";replay_order_model=" << summary.replay_order_model
      << ";image_local_init_reset_model="
      << summary.image_local_init_reset_model
      << ";catalog_retention_model=" << summary.catalog_retention_model
      << ";runtime_state_snapshot_symbol="
      << summary.runtime_state_snapshot_symbol;
  return out.str();
}

static Objc3BootstrapLegalityFailureContractSummary
BuildBootstrapLegalityFailureContractSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3BootstrapLegalityFailureContractSummary summary;
  // semantic migration anchor: canonical-mode legacy literal diagnostics stays
  // truthful only if the live fail-closed legacy-literal rejection surface is
  // preserved through the compatibility/strictness summary exported here.
  summary.fail_closed = true;
  summary.duplicate_registration_policy_frozen = true;
  summary.image_order_invariant_frozen = true;
  summary.bootstrap_rejection_frozen = true;
  summary.restart_boundary_frozen = true;
  summary.semantic_diagnostics_required = true;
  summary.ready_for_lowering_and_runtime =
      surface.interface_implementation_summary.deterministic;
  if (summary.ready_for_lowering_and_runtime) {
    summary.replay_key = BuildBootstrapLegalityFailureContractReplayKey(summary);
  }
  if (!IsReadyObjc3BootstrapLegalityFailureContractSummary(summary)) {
    summary.failure_reason =
        "bootstrap legality duplicate/order/failure contract summary is incomplete";
  }
  return summary;
}

static std::string BuildBootstrapLegalitySemanticsReplayKey(
    const Objc3BootstrapLegalitySemanticsSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";bootstrap_legality_failure_contract_id="
      << summary.bootstrap_legality_failure_contract_id
      << ";bootstrap_semantics_contract_id="
      << summary.bootstrap_semantics_contract_id
      << ";surface_path=" << summary.surface_path
      << ";duplicate_registration_policy="
      << summary.duplicate_registration_policy
      << ";image_registration_order_invariant="
      << summary.image_registration_order_invariant
      << ";cross_image_legality_model=" << summary.cross_image_legality_model
      << ";semantic_diagnostic_model=" << summary.semantic_diagnostic_model
      << ";bootstrap_legality_failure_replay_key="
      << summary.bootstrap_legality_failure_replay_key;
  return out.str();
}

static Objc3BootstrapLegalitySemanticsSummary
BuildBootstrapLegalitySemanticsSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3BootstrapLegalitySemanticsSummary summary;
  summary.fail_closed = true;
  summary.bootstrap_legality_failure_contract_ready =
      IsReadyObjc3BootstrapLegalityFailureContractSummary(
          surface.bootstrap_legality_failure_contract_summary);
  if (summary.bootstrap_legality_failure_contract_ready) {
    summary.duplicate_registration_policy =
        surface.bootstrap_legality_failure_contract_summary
            .duplicate_registration_policy;
    summary.image_registration_order_invariant =
        surface.bootstrap_legality_failure_contract_summary
            .image_registration_order_invariant;
    summary.bootstrap_legality_failure_replay_key =
        surface.bootstrap_legality_failure_contract_summary.replay_key;
  }
  summary.duplicate_registration_semantics_landed =
      summary.bootstrap_legality_failure_contract_ready;
  summary.image_order_semantics_landed =
      summary.bootstrap_legality_failure_contract_ready;
  summary.cross_image_legality_semantics_landed =
      summary.bootstrap_legality_failure_contract_ready;
  summary.semantic_diagnostics_landed =
      summary.bootstrap_legality_failure_contract_ready &&
      surface.bootstrap_legality_failure_contract_summary
          .semantic_diagnostics_required;
  summary.ready_for_lowering_and_runtime =
      summary.bootstrap_legality_failure_contract_ready &&
      surface.bootstrap_legality_failure_contract_summary
          .ready_for_lowering_and_runtime;
  if (summary.ready_for_lowering_and_runtime) {
    summary.replay_key = BuildBootstrapLegalitySemanticsReplayKey(summary);
  }
  if (!IsReadyObjc3BootstrapLegalitySemanticsSummary(summary)) {
    summary.failure_reason =
        "bootstrap legality duplicate/order semantics summary is incomplete";
  }
  return summary;
}

static std::string BuildBootstrapFailureRestartSemanticsReplayKey(
    const Objc3BootstrapFailureRestartSemanticsSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";bootstrap_legality_semantics_contract_id="
      << summary.bootstrap_legality_semantics_contract_id
      << ";bootstrap_reset_contract_id=" << summary.bootstrap_reset_contract_id
      << ";bootstrap_semantics_contract_id="
      << summary.bootstrap_semantics_contract_id
      << ";surface_path=" << summary.surface_path
      << ";failure_mode=" << summary.failure_mode
      << ";restart_lifecycle_model=" << summary.restart_lifecycle_model
      << ";replay_order_model=" << summary.replay_order_model
      << ";image_local_init_reset_model="
      << summary.image_local_init_reset_model
      << ";catalog_retention_model=" << summary.catalog_retention_model
      << ";unsupported_topology_model=" << summary.unsupported_topology_model
      << ";bootstrap_legality_semantics_replay_key="
      << summary.bootstrap_legality_semantics_replay_key;
  return out.str();
}

static Objc3BootstrapFailureRestartSemanticsSummary
BuildBootstrapFailureRestartSemanticsSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3BootstrapFailureRestartSemanticsSummary summary;
  summary.fail_closed = true;
  summary.bootstrap_legality_semantics_ready =
      IsReadyObjc3BootstrapLegalitySemanticsSummary(
          surface.bootstrap_legality_semantics_summary);
  if (summary.bootstrap_legality_semantics_ready) {
    summary.failure_mode =
        surface.bootstrap_legality_failure_contract_summary.failure_mode;
    summary.restart_lifecycle_model =
        surface.bootstrap_legality_failure_contract_summary
            .restart_lifecycle_model;
    summary.replay_order_model =
        surface.bootstrap_legality_failure_contract_summary.replay_order_model;
    summary.image_local_init_reset_model =
        surface.bootstrap_legality_failure_contract_summary
            .image_local_init_reset_model;
    summary.catalog_retention_model =
        surface.bootstrap_legality_failure_contract_summary
            .catalog_retention_model;
    summary.bootstrap_legality_semantics_replay_key =
        surface.bootstrap_legality_semantics_summary.replay_key;
  }
  summary.failure_mode_semantics_landed =
      summary.bootstrap_legality_semantics_ready;
  summary.restart_semantics_landed = summary.bootstrap_legality_semantics_ready;
  summary.replay_semantics_landed = summary.bootstrap_legality_semantics_ready;
  summary.unsupported_topology_semantics_landed =
      summary.bootstrap_legality_semantics_ready;
  summary.deterministic_recovery_semantics_landed =
      summary.bootstrap_legality_semantics_ready;
  summary.ready_for_lowering_and_runtime =
      summary.bootstrap_legality_semantics_ready &&
      surface.bootstrap_legality_semantics_summary.ready_for_lowering_and_runtime;
  if (summary.ready_for_lowering_and_runtime) {
    summary.replay_key =
        BuildBootstrapFailureRestartSemanticsReplayKey(summary);
  }
  if (!IsReadyObjc3BootstrapFailureRestartSemanticsSummary(summary)) {
    summary.failure_reason =
        "bootstrap failure-mode and restart semantics summary is incomplete";
  }
  return summary;
}

static std::string BuildCompatibilityStrictnessClaimSemanticsReplayKey(
    const Objc3CompatibilityStrictnessClaimSemanticsSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";inventory_contract_id="
      << summary.runnable_feature_claim_inventory_contract_id
      << ";truth_surface_contract_id="
      << summary.feature_claim_truth_surface_contract_id
      << ";surface_path=" << summary.surface_path
      << ";semantic_model=" << summary.semantic_model
      << ";downgrade_model=" << summary.downgrade_model
      << ";rejection_model=" << summary.rejection_model
      << ";canonical_interface_truth_model="
      << summary.canonical_interface_truth_model
      << ";separate_compilation_macro_truth_model="
      << summary.separate_compilation_macro_truth_model
      << ";canonical_interface_payload_mode="
      << summary.canonical_interface_payload_mode
      << ";language_profile=" << summary.effective_language_profile
      << ";valid_language_profiles="
      << summary.valid_language_profile_count
      << ";live_selection_surfaces=" << summary.live_selection_surface_count
      << ";valid_selection_combinations="
      << summary.valid_selection_combination_count
      << ";runnable_claims=" << summary.runnable_feature_claim_count
      << ";downgraded_source_only_claims="
      << summary.downgraded_source_only_claim_count
      << ";rejected_feature_claims="
      << summary.rejected_unsupported_feature_claim_count
      << ";live_unsupported_feature_families="
      << summary.live_unsupported_feature_family_count
      << ";live_unsupported_feature_sites="
      << summary.live_unsupported_feature_site_count
      << ";live_unsupported_feature_diagnostics="
      << summary.live_unsupported_feature_diagnostic_count
      << ";throws_source_rejections="
      << summary.throws_source_rejection_site_count
      << ";blocks_source_rejections="
      << summary.blocks_source_rejection_site_count
      << ";arc_source_rejections="
      << summary.arc_source_rejection_site_count
      << ";rejected_selection_surfaces="
      << summary.rejected_selection_surface_count
      << ";suppressed_macro_claims="
      << summary.suppressed_macro_claim_count
      << ";suppressed_macro_claim_ids=";
  for (std::size_t index = 0; index < summary.suppressed_macro_claim_ids.size();
       ++index) {
    if (index > 0u) {
      out << ",";
    }
    out << summary.suppressed_macro_claim_ids[index];
  }
  return out.str();
}

struct Objc3UnsupportedFeatureClaimEnforcementStats {
  std::size_t live_unsupported_feature_family_count = 0;
  std::size_t live_unsupported_feature_site_count = 0;
  std::size_t live_unsupported_feature_diagnostic_count = 0;
  std::size_t throws_source_rejection_site_count = 0;
  std::size_t blocks_source_rejection_site_count = 0;
  std::size_t arc_source_rejection_site_count = 0;
};

struct Objc3UnsupportedFeatureClaimContext {
  std::size_t owned_runtime_backed_object_property_sites = 0;
  bool has_owned_runtime_backed_object_storage = false;
  bool allow_source_only_block_literals = false;
  bool allow_source_only_defer_statements = false;
  bool allow_source_only_error_runtime_surface = false;
  bool arc_mode_enabled = false;
};

static bool IsRuntimeBackedObjectProperty(const Objc3PropertyDecl &property) {
  return !property.vector_spelling &&
         (property.id_spelling || property.class_spelling ||
          property.instancetype_spelling ||
          property.object_pointer_type_spelling);
}

static bool IsOwnedRuntimeBackedObjectProperty(const Objc3PropertyDecl &property) {
  return IsRuntimeBackedObjectProperty(property) &&
         (property.is_copy || property.is_strong ||
          property.ownership_qualifier_spelling == "__strong");
}

static Objc3UnsupportedFeatureClaimContext
BuildUnsupportedFeatureClaimContext(const Objc3Program &ast,
                                    bool allow_source_only_block_literals,
                                    bool allow_source_only_defer_statements,
                                    bool allow_source_only_error_runtime_surface,
                                    bool arc_mode_enabled) {
  Objc3UnsupportedFeatureClaimContext context;
  std::unordered_set<std::string> seen_owned_storage_sites;
  const auto record_properties = [&](const auto &owner_name,
                                     const auto &properties) {
    for (const auto &property : properties) {
      if (!IsOwnedRuntimeBackedObjectProperty(property)) {
        continue;
      }
      const std::string key = owner_name + "::" + property.name;
      if (!seen_owned_storage_sites.emplace(key).second) {
        continue;
      }
      ++context.owned_runtime_backed_object_property_sites;
    }
  };

  for (const auto &interface_decl : ast.interfaces) {
    record_properties(interface_decl.name, interface_decl.properties);
  }
  for (const auto &implementation_decl : ast.implementations) {
    record_properties(implementation_decl.name, implementation_decl.properties);
  }
  context.has_owned_runtime_backed_object_storage =
      context.owned_runtime_backed_object_property_sites > 0u;
  context.allow_source_only_block_literals = allow_source_only_block_literals;
  context.allow_source_only_defer_statements = allow_source_only_defer_statements;
  context.allow_source_only_error_runtime_surface =
      allow_source_only_error_runtime_surface;
  context.arc_mode_enabled = arc_mode_enabled;
  return context;
}

static Objc3CompatibilityStrictnessClaimSemanticsSummary
BuildCompatibilityStrictnessClaimSemanticsSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface,
    const Objc3UnsupportedFeatureClaimEnforcementStats
        &unsupported_feature_enforcement) {
  Objc3CompatibilityStrictnessClaimSemanticsSummary summary;
  summary.effective_language_profile = "canonical";
  summary.valid_language_profile_count =
      kObjc3CompatibilityStrictnessClaimValidLanguageProfileCount;
  summary.live_selection_surface_count =
      kObjc3CompatibilityStrictnessClaimLiveSelectionSurfaceCount;
  summary.valid_selection_combination_count =
      kObjc3CompatibilityStrictnessClaimValidSelectionCombinationCount;
  summary.runnable_feature_claim_count =
      kObjc3CompatibilityStrictnessClaimRunnableFeatureCount;
  summary.downgraded_source_only_claim_count =
      kObjc3CompatibilityStrictnessClaimSourceOnlyFeatureCount;
  summary.rejected_unsupported_feature_claim_count =
      kObjc3CompatibilityStrictnessClaimRejectedFeatureCount;
  summary.rejected_selection_surface_count =
      kObjc3CompatibilityStrictnessClaimRejectedSelectionSurfaceCount;
  summary.suppressed_macro_claim_count =
      kObjc3CompatibilityStrictnessClaimSuppressedMacroClaimCount;
  summary.live_unsupported_feature_family_count =
      unsupported_feature_enforcement.live_unsupported_feature_family_count;
  summary.live_unsupported_feature_site_count =
      unsupported_feature_enforcement.live_unsupported_feature_site_count;
  summary.live_unsupported_feature_diagnostic_count =
      unsupported_feature_enforcement.live_unsupported_feature_diagnostic_count;
  summary.throws_source_rejection_site_count =
      unsupported_feature_enforcement.throws_source_rejection_site_count;
  summary.blocks_source_rejection_site_count =
      unsupported_feature_enforcement.blocks_source_rejection_site_count;
  summary.arc_source_rejection_site_count =
      unsupported_feature_enforcement.arc_source_rejection_site_count;
  summary.suppressed_macro_claim_ids = {
      kObjc3SuppressedMacroClaimStrictnessLevel,
      kObjc3SuppressedMacroClaimConcurrencyMode,
      kObjc3SuppressedMacroClaimConcurrencyStrict,
  };
  summary.fail_closed = true;
  summary.language_profile_semantics_landed = true;
  summary.canonical_literal_rejection_semantics_landed = true;
  summary.source_only_claim_downgrade_semantics_landed = true;
  summary.unsupported_feature_claim_rejection_semantics_landed = true;
  summary.live_unsupported_feature_source_rejection_landed = true;
  summary.strictness_selection_rejection_semantics_landed = true;
  summary.feature_macro_claim_suppression_semantics_landed = true;
  summary.canonical_interface_truth_semantics_landed = true;
  summary.separate_compilation_macro_truth_semantics_landed = true;
  summary.selected_configuration_valid = true;
  summary.selected_configuration_downgraded = false;
  summary.selected_configuration_rejected = false;
  // semantic truth anchor: the current native toolchain still has
  // no standalone textual-interface payload, so canonical interface truth is
  // fail-closed by explicitly publishing that absence and the suppressed macro
  // claim set any future separate-compilation surface must preserve.
  summary.ready_for_lowering_and_runtime =
      surface.interface_implementation_summary.deterministic &&
      unsupported_feature_enforcement.live_unsupported_feature_site_count == 0u &&
      unsupported_feature_enforcement.live_unsupported_feature_diagnostic_count ==
          unsupported_feature_enforcement.live_unsupported_feature_site_count;
  if (summary.ready_for_lowering_and_runtime) {
    summary.replay_key =
        BuildCompatibilityStrictnessClaimSemanticsReplayKey(summary);
  }
  if (!IsReadyObjc3CompatibilityStrictnessClaimSemanticsSummary(summary)) {
    summary.failure_reason =
        "compatibility/strictness/claim semantics summary is incomplete";
  }
  return summary;
}

static bool IsSameSemanticType(const SemanticTypeInfo &lhs, const SemanticTypeInfo &rhs) {
  if (lhs.is_vector != rhs.is_vector) {
    return false;
  }
  if (lhs.type != rhs.type) {
    return false;
  }
  if (lhs.is_callable != rhs.is_callable) {
    return false;
  }
  if (!lhs.is_vector) {
    if (lhs.is_callable) {
      return lhs.callable_param_types == rhs.callable_param_types &&
             lhs.callable_return_type == rhs.callable_return_type;
    }
    return true;
  }
  return lhs.vector_lane_count == rhs.vector_lane_count && lhs.vector_base_spelling == rhs.vector_base_spelling;
}

static bool IsEscapingBlockRuntimeHandleCompatible(
    const SemanticTypeInfo &expected, const SemanticTypeInfo &value) {
  if (!IsCallableSemanticType(value) || !IsScalarSemanticType(expected)) {
    return false;
  }
  return expected.type == ValueType::I32 ||
         expected.type == ValueType::ObjCId ||
         expected.type == ValueType::ObjCObjectPtr;
}

static void DiagnoseEscapingBlockRuntimeHandleCaptureLegality(
    unsigned line, unsigned column, const SemanticTypeInfo &value,
    std::vector<std::string> &diagnostics) {
  (void)line;
  (void)column;
  (void)diagnostics;
  if (!value.callable_block_runtime_handle_candidate) {
    return;
  }
}

static std::string SemanticTypeName(const SemanticTypeInfo &info) {
  if (!info.is_vector) {
    if (info.is_callable) {
      std::ostringstream out;
      out << "block(";
      for (std::size_t index = 0; index < info.callable_param_types.size(); ++index) {
        if (index > 0u) {
          out << ", ";
        }
        out << objc3c::support::ValueTypeName(info.callable_param_types[index]);
      }
      out << ") -> " << objc3c::support::ValueTypeName(info.callable_return_type);
      return out.str();
    }
    return objc3c::support::ValueTypeName(info.type);
  }
  const std::string base =
      info.vector_base_spelling.empty() ? std::string(objc3c::support::ValueTypeName(info.type))
                                        : info.vector_base_spelling;
  return base + "x" + std::to_string(info.vector_lane_count);
}

static unsigned OwnershipQualifierLine(
    const std::vector<Objc3SemaTokenMetadata> &tokens,
    unsigned default_line) {
  return tokens.empty() ? default_line : tokens.front().line;
}

static unsigned OwnershipQualifierColumn(
    const std::vector<Objc3SemaTokenMetadata> &tokens,
    unsigned default_column) {
  return tokens.empty() ? default_column : tokens.front().column;
}

static void RecordUnsupportedFeatureClaimDiagnostic(
    std::size_t &site_counter,
    unsigned line,
    unsigned column,
    const std::string &message,
    std::vector<std::string> &diagnostics,
    Objc3UnsupportedFeatureClaimEnforcementStats &stats) {
  // unsupported-feature enforcement anchor: accepted
  // advanced surfaces fail closed here before lowering/runtime publication.
  ++site_counter;
  ++stats.live_unsupported_feature_site_count;
  ++stats.live_unsupported_feature_diagnostic_count;
  diagnostics.push_back(MakeDiag(line, column, "O3S221", message));
}

static void DiagnoseUnsupportedFeatureClaimsInExpr(
    const Expr *expr,
    std::vector<std::string> &diagnostics,
    Objc3UnsupportedFeatureClaimEnforcementStats &stats,
    const Objc3UnsupportedFeatureClaimContext &context);

static bool BlockLiteralUsesRunnableC003Subset(const Expr &expr) {
  // byref-cell/copy-helper/dispose-helper anchor: the runnable block
  // slice now admits non-escaping byref and ownership-sensitive captures at the
  // unsupported-feature gate as soon as the source model is normalized. The
  // gate intentionally remains parser/source-truth driven so later sema and
  // lowering passes can consume the real byref/helper lowering path without
  // depending on post-sema runtime counters. Heap-promotion remains deferred
  // to later lowering/runtime checks instead of this source admission step.
  return expr.block_literal_is_normalized &&
         expr.block_source_model_is_normalized &&
         expr.block_parameter_count <= 4u &&
         expr.block_source_storage_annotations_are_normalized;
}

static bool BlockLiteralUsesRunnableC002Subset(const Expr &expr) {
  // executable-block-object/invoke-thunk anchor: preserve the
  // original runnable-block gate name as a compatibility surface for the C002
  // readiness contract while C003 widens the admitted non-escaping subset.
  return BlockLiteralUsesRunnableC003Subset(expr);
}

static bool ExprContainsBlockLiteral(const Expr *expr);

static bool StmtContainsBlockLiteral(const Stmt *stmt) {
  if (stmt == nullptr) {
    return false;
  }

  switch (stmt->kind) {
    case Stmt::Kind::Let:
      return stmt->let_stmt != nullptr &&
             ExprContainsBlockLiteral(stmt->let_stmt->value.get());
    case Stmt::Kind::Assign:
      return stmt->assign_stmt != nullptr &&
             ExprContainsBlockLiteral(stmt->assign_stmt->value.get());
    case Stmt::Kind::Return:
      return stmt->return_stmt != nullptr &&
             ExprContainsBlockLiteral(stmt->return_stmt->value.get());
    case Stmt::Kind::If:
      if (stmt->if_stmt == nullptr) {
        return false;
      }
      if (ExprContainsBlockLiteral(stmt->if_stmt->condition.get())) {
        return true;
      }
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        if (StmtContainsBlockLiteral(then_stmt.get())) {
          return true;
        }
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        if (StmtContainsBlockLiteral(else_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt == nullptr) {
        return false;
      }
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        if (StmtContainsBlockLiteral(body_stmt.get())) {
          return true;
        }
      }
      return ExprContainsBlockLiteral(stmt->do_while_stmt->condition.get());
    case Stmt::Kind::For:
      if (stmt->for_stmt == nullptr) {
        return false;
      }
      if (ExprContainsBlockLiteral(stmt->for_stmt->init.value.get()) ||
          ExprContainsBlockLiteral(stmt->for_stmt->condition.get()) ||
          ExprContainsBlockLiteral(stmt->for_stmt->step.value.get())) {
        return true;
      }
      for (const auto &body_stmt : stmt->for_stmt->body) {
        if (StmtContainsBlockLiteral(body_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt == nullptr) {
        return false;
      }
      if (ExprContainsBlockLiteral(stmt->switch_stmt->condition.get())) {
        return true;
      }
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          if (StmtContainsBlockLiteral(case_stmt.get())) {
            return true;
          }
        }
      }
      return false;
    case Stmt::Kind::While:
      if (stmt->while_stmt == nullptr) {
        return false;
      }
      if (ExprContainsBlockLiteral(stmt->while_stmt->condition.get())) {
        return true;
      }
      for (const auto &body_stmt : stmt->while_stmt->body) {
        if (StmtContainsBlockLiteral(body_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
      if (stmt->block_stmt == nullptr) {
        return false;
      }
      for (const auto &body_stmt : stmt->block_stmt->body) {
        if (StmtContainsBlockLiteral(body_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::Expr:
      return stmt->expr_stmt != nullptr &&
             ExprContainsBlockLiteral(stmt->expr_stmt->value.get());
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return false;
  }
  return false;
}

static bool ExprContainsBlockLiteral(const Expr *expr) {
  if (expr == nullptr) {
    return false;
  }
  if (expr->kind == Expr::Kind::BlockLiteral) {
    return true;
  }

  if (ExprContainsBlockLiteral(expr->left.get()) ||
      ExprContainsBlockLiteral(expr->right.get()) ||
      ExprContainsBlockLiteral(expr->third.get()) ||
      ExprContainsBlockLiteral(expr->receiver.get())) {
    return true;
  }
  for (const auto &arg : expr->args) {
    if (ExprContainsBlockLiteral(arg.get())) {
      return true;
    }
  }
  return false;
}

static bool FunctionBodyContainsBlockLiteral(const FunctionDecl &fn) {
  for (const auto &stmt : fn.body) {
    if (StmtContainsBlockLiteral(stmt.get())) {
      return true;
    }
  }
  return false;
}

static bool MethodBodyContainsBlockLiteral(const Objc3MethodDecl &method) {
  for (const auto &stmt : method.body) {
    if (StmtContainsBlockLiteral(stmt.get())) {
      return true;
    }
  }
  return false;
}

static void DiagnoseUnsupportedFeatureClaimsInStmt(
    const Stmt *stmt,
    std::vector<std::string> &diagnostics,
    Objc3UnsupportedFeatureClaimEnforcementStats &stats,
    const Objc3UnsupportedFeatureClaimContext &context) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      DiagnoseUnsupportedFeatureClaimsInExpr(
          stmt->let_stmt->value.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      DiagnoseUnsupportedFeatureClaimsInExpr(
          stmt->assign_stmt->value.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      DiagnoseUnsupportedFeatureClaimsInExpr(
          stmt->return_stmt->value.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    DiagnoseUnsupportedFeatureClaimsInExpr(
        stmt->if_stmt->condition.get(), diagnostics, stats, context);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      DiagnoseUnsupportedFeatureClaimsInStmt(
          then_stmt.get(), diagnostics, stats, context);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      DiagnoseUnsupportedFeatureClaimsInStmt(
          else_stmt.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      DiagnoseUnsupportedFeatureClaimsInStmt(
          body_stmt.get(), diagnostics, stats, context);
    }
    DiagnoseUnsupportedFeatureClaimsInExpr(
        stmt->do_while_stmt->condition.get(), diagnostics, stats, context);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    DiagnoseUnsupportedFeatureClaimsInExpr(
        stmt->for_stmt->init.value.get(), diagnostics, stats, context);
    DiagnoseUnsupportedFeatureClaimsInExpr(
        stmt->for_stmt->condition.get(), diagnostics, stats, context);
    DiagnoseUnsupportedFeatureClaimsInExpr(
        stmt->for_stmt->step.value.get(), diagnostics, stats, context);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      DiagnoseUnsupportedFeatureClaimsInStmt(
          body_stmt.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    DiagnoseUnsupportedFeatureClaimsInExpr(
        stmt->switch_stmt->condition.get(), diagnostics, stats, context);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        DiagnoseUnsupportedFeatureClaimsInStmt(
            case_stmt.get(), diagnostics, stats, context);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    DiagnoseUnsupportedFeatureClaimsInExpr(
        stmt->while_stmt->condition.get(), diagnostics, stats, context);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      DiagnoseUnsupportedFeatureClaimsInStmt(
          body_stmt.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::Block:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    if (stmt->block_stmt->is_autoreleasepool_scope) {
      // autoreleasepool/destruction-order semantic expansion anchor:
      // lane-B identified the ownership-sensitive destruction-order surface so
      // later runtime work would not need to recover it from source.
      // runtime-memory-management implementation anchor: native mode
      // now accepts `@autoreleasepool` blocks and relies on emitted runtime
      // push/pop helpers plus live refcount/weak-table support instead of the
      // earlier fail-closed diagnostic path.
      // ownership-runtime-gate freeze anchor: lane-E now freezes
      // this accepted autoreleasepool slice as part of the supported ownership
      // runtime baseline and leaves ARC/block/public-ABI widening as explicit
      // non-goals for later milestones.
      // ownership-smoke closeout anchor: the runnable closeout matrix
      // consumes this admitted ownership-sensitive autoreleasepool slice
      // unchanged and may not widen it during the earlier tranche closeout.
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      DiagnoseUnsupportedFeatureClaimsInStmt(
          body_stmt.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::Defer:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    if (!context.allow_source_only_defer_statements) {
      diagnostics.push_back(MakeDiag(
          stmt->line,
          stmt->column,
          "O3S221",
          "unsupported feature claim: defer statements are not yet runnable in Objective-C 3 native mode"));
      ++stats.live_unsupported_feature_site_count;
      ++stats.live_unsupported_feature_diagnostic_count;
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      DiagnoseUnsupportedFeatureClaimsInStmt(
          body_stmt.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      DiagnoseUnsupportedFeatureClaimsInExpr(
          stmt->expr_stmt->value.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

static void DiagnoseUnsupportedFeatureClaimsInExpr(
    const Expr *expr,
    std::vector<std::string> &diagnostics,
    Objc3UnsupportedFeatureClaimEnforcementStats &stats,
    const Objc3UnsupportedFeatureClaimContext &context) {
  if (expr == nullptr) {
    return;
  }

  switch (expr->kind) {
  // executable-block-source-closure anchor: once block literals
  // parse successfully they still fail closed here until the next runtime step and later
  // runtime issues make them runnable.
  case Expr::Kind::BlockLiteral:
    // block-source-model-completion anchor: source-only frontend
    // projection runs may admit block literals through sema so manifest-only
    // tooling can prove the completed source model, while native emit paths
    // continue to fail closed until later lowering/runtime work lands.
    // block-runtime-semantic-rules freeze anchor: current semantic
    // behavior is intentionally split between source-only admission and native
    // fail-closed rejection, with no runnable block object semantics implied.
    if (context.allow_source_only_block_literals ||
        BlockLiteralUsesRunnableC002Subset(*expr)) {
      return;
    }
    RecordUnsupportedFeatureClaimDiagnostic(
        stats.blocks_source_rejection_site_count,
        expr->line,
        expr->column,
        "unsupported feature claim: block literals are not yet runnable in Objective-C 3 native mode",
        diagnostics,
        stats);
    return;
  case Expr::Kind::Call:
    for (const auto &arg : expr->args) {
      DiagnoseUnsupportedFeatureClaimsInExpr(arg.get(), diagnostics, stats, context);
    }
    return;
  case Expr::Kind::MessageSend:
    DiagnoseUnsupportedFeatureClaimsInExpr(
        expr->receiver.get(), diagnostics, stats, context);
    for (const auto &arg : expr->args) {
      DiagnoseUnsupportedFeatureClaimsInExpr(arg.get(), diagnostics, stats, context);
    }
    return;
  case Expr::Kind::Binary:
    DiagnoseUnsupportedFeatureClaimsInExpr(
        expr->left.get(), diagnostics, stats, context);
    DiagnoseUnsupportedFeatureClaimsInExpr(
        expr->right.get(), diagnostics, stats, context);
    return;
  case Expr::Kind::Conditional:
    DiagnoseUnsupportedFeatureClaimsInExpr(
        expr->left.get(), diagnostics, stats, context);
    DiagnoseUnsupportedFeatureClaimsInExpr(
        expr->right.get(), diagnostics, stats, context);
    DiagnoseUnsupportedFeatureClaimsInExpr(
        expr->third.get(), diagnostics, stats, context);
    return;
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::NilLiteral:
  case Expr::Kind::Number:
  default:
    return;
  }
}
