#include "parse/objc3_parser.h"

// source-closure anchors: unsupported 'try' expression,
// unsupported 'throw' statement, and unsupported 'do/catch' statement remain
// explicit fail-closed parser-owned diagnostics until the later runnable Part 6
// lowering/runtime tranches land.
#include "parse/objc3_ast_builder.h"
#include "parse/objc3_message_send_profiles.h"
#include "parse/objc3_parse_support.h"
#include "support/objc3_method_family.h"

#include <algorithm>
#include <cctype>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

namespace {

// source note: Part 12 does not admit new syntax here. Both the
// diagnostics/fix-it/migrator inventory and the migration/canonicalization
// completion packet aggregate existing Part 6 through Part 11 parser-owned
// surfaces and frontend summary packets.

using Token = Objc3LexToken;
using TokenKind = Objc3LexTokenKind;

using objc3c::parse::support::MakeDiag;
using objc3c::parse::support::ParseIntegerLiteralValue;
using objc3c::parse::BuildDispatchAbiMarshallingSymbol;
using objc3c::parse::BuildMessageSendFormSymbol;
using objc3c::parse::BuildMessageSendSelectorLoweringSymbol;
using objc3c::parse::BuildMethodFamilySemanticsSymbol;
using objc3c::parse::BuildNilReceiverFoldingSymbol;
using objc3c::parse::BuildOptionalSendSymbol;
using objc3c::parse::BuildRuntimeLinkHostLinkSymbol;
using objc3c::parse::BuildSuperDispatchSymbol;
using objc3c::parse::ComputeDispatchAbiArgumentPaddingSlots;
using objc3c::parse::IsSuperDispatchReceiver;
using objc3c::parse::kDispatchAbiMarshallingRuntimeArgSlots;
using objc3c::parse::kRuntimeLinkHostLinkDispatchSymbol;

static Objc3SemaTokenMetadata MakeSemaTokenMetadata(Objc3SemaTokenKind kind, const Token &token) {
  return MakeObjc3SemaTokenMetadata(kind, token.text, token.line, token.column);
}

static bool TryParseVectorTypeSpelling(const Token &type_token,
                                       ValueType &vector_type,
                                       std::string &vector_base_spelling,
                                       unsigned &vector_lane_count) {
  const std::string &text = type_token.text;
  const bool is_i32_vector = text.rfind("i32x", 0) == 0;
  const bool is_bool_vector = text.rfind("boolx", 0) == 0;
  if (!is_i32_vector && !is_bool_vector) {
    return false;
  }

  const std::size_t prefix_length = is_i32_vector ? 4u : 5u;
  if (text.size() <= prefix_length) {
    return false;
  }

  unsigned lane_count = 0;
  for (std::size_t i = prefix_length; i < text.size(); ++i) {
    const char c = text[i];
    if (!std::isdigit(static_cast<unsigned char>(c))) {
      return false;
    }
    lane_count = (lane_count * 10u) + static_cast<unsigned>(c - '0');
    if (lane_count > 1024u) {
      return false;
    }
  }

  if (lane_count != 2u && lane_count != 4u && lane_count != 8u && lane_count != 16u) {
    return false;
  }

  vector_type = is_i32_vector ? ValueType::I32 : ValueType::Bool;
  vector_base_spelling = is_i32_vector ? "i32" : "bool";
  vector_lane_count = lane_count;
  return true;
}

static std::string BuildNormalizedObjcSelector(const std::vector<Objc3MethodDecl::SelectorPiece> &pieces) {
  std::string normalized;
  for (const auto &piece : pieces) {
    normalized += piece.keyword;
    if (piece.has_parameter) {
      normalized += ":";
    }
  }
  return normalized;
}

static std::string BuildTypedKeyPathLiteralProfile(const std::string &root_name,
                                                   bool root_is_self,
                                                   const std::vector<std::string> &components) {
  std::ostringstream out;
  out << "typed-keypath:root=" << (root_is_self ? "self" : root_name)
      << ";components=";
  for (std::size_t i = 0; i < components.size(); ++i) {
    if (i != 0u) {
      out << ".";
    }
    out << components[i];
  }
  return out.str();
}

static std::string BuildAutoreleasePoolScopeSymbol(unsigned serial, unsigned depth) {
  std::ostringstream out;
  out << "autoreleasepool-scope:" << serial << ";depth=" << depth;
  return out.str();
}

static std::string DescribeCompatDiagnosticToken(const Token &token) {
  if (token.kind == TokenKind::Eof) {
    return "end of file";
  }
  if (token.kind == TokenKind::Identifier) {
    return "identifier '" + token.text + "'";
  }
  if (token.kind == TokenKind::Number) {
    return "number '" + token.text + "'";
  }
  if (token.text.empty()) {
    return "token";
  }
  return "token '" + token.text + "'";
}

static std::string BuildLightweightGenericConstraintProfile(
    bool object_pointer_type_spelling,
    bool has_generic_suffix,
    bool generic_suffix_terminated,
    bool has_pointer_declarator,
    const std::string &generic_suffix_text) {
  const bool generic_instantiation_valid =
      !has_generic_suffix || (generic_suffix_terminated && object_pointer_type_spelling);
  std::ostringstream out;
  out << "lightweight-generics:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";suffix-bytes=" << generic_suffix_text.size()
      << ";instantiation-valid=" << (generic_instantiation_valid ? "true" : "false");
  return out.str();
}

static bool IsLightweightGenericConstraintProfileNormalized(
    bool object_pointer_type_spelling,
    bool has_generic_suffix,
    bool generic_suffix_terminated) {
  if (!has_generic_suffix) {
    return true;
  }
  return generic_suffix_terminated && object_pointer_type_spelling;
}

static std::string BuildNullabilityFlowProfile(
    bool object_pointer_type_spelling,
    std::size_t nullability_suffix_count,
    bool has_pointer_declarator,
    bool has_generic_suffix,
    bool generic_suffix_terminated) {
  const bool flow_precision_valid =
      nullability_suffix_count == 0 || object_pointer_type_spelling;
  std::ostringstream out;
  out << "nullability-flow:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";suffix-count=" << nullability_suffix_count
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";generic-terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";flow-precision-valid=" << (flow_precision_valid ? "true" : "false");
  return out.str();
}

static bool IsNullabilityFlowProfileNormalized(
    bool object_pointer_type_spelling,
    std::size_t nullability_suffix_count) {
  if (nullability_suffix_count == 0) {
    return true;
  }
  return object_pointer_type_spelling;
}

static std::size_t CountMarkerOccurrences(const std::string &text, const std::string &marker) {
  if (marker.empty() || text.empty()) {
    return 0;
  }
  std::size_t count = 0;
  std::size_t offset = 0;
  while (true) {
    const std::size_t found = text.find(marker, offset);
    if (found == std::string::npos) {
      break;
    }
    ++count;
    offset = found + marker.size();
  }
  return count;
}

static std::size_t CountTopLevelGenericArgumentSlots(const std::string &generic_suffix_text) {
  if (generic_suffix_text.size() < 2) {
    return 0;
  }
  std::size_t begin = 0;
  std::size_t end = generic_suffix_text.size();
  if (generic_suffix_text.front() == '<' && generic_suffix_text.back() == '>') {
    begin = 1;
    end -= 1;
  }
  if (begin >= end) {
    return 0;
  }

  std::size_t slots = 1;
  std::size_t depth = 0;
  bool saw_non_whitespace = false;
  for (std::size_t i = begin; i < end; ++i) {
    const char c = generic_suffix_text[i];
    if (!std::isspace(static_cast<unsigned char>(c))) {
      saw_non_whitespace = true;
    }
    if (c == '<') {
      ++depth;
      continue;
    }
    if (c == '>') {
      if (depth > 0) {
        --depth;
      }
      continue;
    }
    if (c == ',' && depth == 0) {
      ++slots;
    }
  }

  return saw_non_whitespace ? slots : 0;
}

static std::string BuildVarianceBridgeCastProfile(
    bool object_pointer_type_spelling,
    bool has_generic_suffix,
    bool generic_suffix_terminated,
    bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &ownership_qualifier_spelling) {
  const std::size_t covariant_markers = CountMarkerOccurrences(generic_suffix_text, "__covariant");
  const std::size_t contravariant_markers = CountMarkerOccurrences(generic_suffix_text, "__contravariant");
  const std::size_t invariant_markers = CountMarkerOccurrences(generic_suffix_text, "__invariant");
  const std::size_t bridge_transfer_markers = CountMarkerOccurrences(generic_suffix_text, "__bridge_transfer");
  const std::size_t bridge_retained_markers = CountMarkerOccurrences(generic_suffix_text, "__bridge_retained");
  const std::size_t bridge_markers = CountMarkerOccurrences(generic_suffix_text, "__bridge") +
                                     CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge");
  const std::size_t bridge_transfer_total =
      bridge_transfer_markers + CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge_transfer");
  const std::size_t bridge_retained_total =
      bridge_retained_markers + CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge_retained");
  const bool variance_marked =
      covariant_markers + contravariant_markers + invariant_markers > 0;
  const bool bridge_marked = bridge_markers + bridge_transfer_total + bridge_retained_total > 0;
  const bool variance_safe = (covariant_markers == 0 || contravariant_markers == 0) &&
                             (covariant_markers + contravariant_markers <= 1);
  const bool bridge_cast_valid = bridge_transfer_total <= 1 && bridge_retained_total <= 1;
  const bool object_pointer_required_for_markers =
      !variance_marked && !bridge_marked ? true : object_pointer_type_spelling;

  std::ostringstream out;
  out << "variance-bridge-cast:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";covariant-markers=" << covariant_markers
      << ";contravariant-markers=" << contravariant_markers
      << ";invariant-markers=" << invariant_markers
      << ";bridge-markers=" << bridge_markers
      << ";bridge-transfer-markers=" << bridge_transfer_total
      << ";bridge-retained-markers=" << bridge_retained_total
      << ";variance-safe=" << (variance_safe ? "true" : "false")
      << ";bridge-cast-valid=" << (bridge_cast_valid ? "true" : "false")
      << ";marker-object-pointer-valid=" << (object_pointer_required_for_markers ? "true" : "false");
  return out.str();
}

static bool IsVarianceBridgeCastProfileNormalized(
    bool object_pointer_type_spelling,
    bool has_generic_suffix,
    bool generic_suffix_terminated,
    const std::string &generic_suffix_text,
    const std::string &ownership_qualifier_spelling) {
  const std::size_t covariant_markers = CountMarkerOccurrences(generic_suffix_text, "__covariant");
  const std::size_t contravariant_markers = CountMarkerOccurrences(generic_suffix_text, "__contravariant");
  const std::size_t invariant_markers = CountMarkerOccurrences(generic_suffix_text, "__invariant");
  const std::size_t bridge_transfer_markers =
      CountMarkerOccurrences(generic_suffix_text, "__bridge_transfer") +
      CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge_transfer");
  const std::size_t bridge_retained_markers =
      CountMarkerOccurrences(generic_suffix_text, "__bridge_retained") +
      CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge_retained");
  const bool variance_marked =
      covariant_markers + contravariant_markers + invariant_markers > 0;
  const bool bridge_marked =
      CountMarkerOccurrences(generic_suffix_text, "__bridge") +
          CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge") +
          bridge_transfer_markers + bridge_retained_markers >
      0;
  const bool variance_safe = (covariant_markers == 0 || contravariant_markers == 0) &&
                             (covariant_markers + contravariant_markers <= 1);
  const bool bridge_cast_valid = bridge_transfer_markers <= 1 && bridge_retained_markers <= 1;
  if (variance_marked && (!has_generic_suffix || !generic_suffix_terminated)) {
    return false;
  }
  if ((variance_marked || bridge_marked) && !object_pointer_type_spelling) {
    return false;
  }
  return variance_safe && bridge_cast_valid;
}

static std::string BuildGenericMetadataAbiProfile(
    bool object_pointer_type_spelling,
    bool has_generic_suffix,
    bool generic_suffix_terminated,
    bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &ownership_qualifier_spelling) {
  const std::size_t generic_argument_slots =
      has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text) : 0;
  const std::size_t variance_markers =
      CountMarkerOccurrences(generic_suffix_text, "__covariant") +
      CountMarkerOccurrences(generic_suffix_text, "__contravariant") +
      CountMarkerOccurrences(generic_suffix_text, "__invariant");
  const std::size_t bridge_markers =
      CountMarkerOccurrences(generic_suffix_text, "__bridge") +
      CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge");
  const bool metadata_emission_ready =
      has_generic_suffix && generic_suffix_terminated && object_pointer_type_spelling &&
      generic_argument_slots > 0;
  const bool abi_layout_stable = metadata_emission_ready &&
                                 (!has_pointer_declarator || object_pointer_type_spelling);

  std::ostringstream out;
  out << "generic-metadata-abi:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";generic-argument-slots=" << generic_argument_slots
      << ";variance-markers=" << variance_markers
      << ";bridge-markers=" << bridge_markers
      << ";metadata-emission-ready=" << (metadata_emission_ready ? "true" : "false")
      << ";abi-layout-stable=" << (abi_layout_stable ? "true" : "false");
  return out.str();
}

static bool IsGenericMetadataAbiProfileNormalized(
    bool object_pointer_type_spelling,
    bool has_generic_suffix,
    bool generic_suffix_terminated,
    bool has_pointer_declarator,
    const std::string &generic_suffix_text) {
  if (!has_generic_suffix) {
    return true;
  }

  const std::size_t generic_argument_slots =
      CountTopLevelGenericArgumentSlots(generic_suffix_text);
  if (!generic_suffix_terminated || !object_pointer_type_spelling ||
      generic_argument_slots == 0) {
    return false;
  }

  if (has_pointer_declarator && !object_pointer_type_spelling) {
    return false;
  }
  return true;
}

static std::size_t CountNamespaceSegments(const std::string &name) {
  if (name.empty()) {
    return 0;
  }
  std::size_t segments = 1;
  for (char c : name) {
    if (c == '.') {
      ++segments;
    }
  }
  return segments;
}

static std::string BuildModuleImportGraphProfile(
    bool object_pointer_type_spelling,
    bool has_generic_suffix,
    bool generic_suffix_terminated,
    bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name) {
  const std::size_t import_edge_candidates =
      has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text) : 0;
  const std::size_t module_segments = CountNamespaceSegments(object_pointer_type_name);
  const bool graph_well_formed =
      !has_generic_suffix ||
      (generic_suffix_terminated && object_pointer_type_spelling && import_edge_candidates > 0);
  const bool namespace_stable = module_segments <= 1 || object_pointer_type_spelling;

  std::ostringstream out;
  out << "module-import-graph:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";module-segments=" << module_segments
      << ";import-edge-candidates=" << import_edge_candidates
      << ";graph-well-formed=" << (graph_well_formed ? "true" : "false")
      << ";namespace-stable=" << (namespace_stable ? "true" : "false");
  return out.str();
}

static bool IsModuleImportGraphProfileNormalized(
    bool object_pointer_type_spelling,
    bool has_generic_suffix,
    bool generic_suffix_terminated,
    const std::string &generic_suffix_text) {
  if (!has_generic_suffix) {
    return true;
  }
  const std::size_t import_edge_candidates =
      CountTopLevelGenericArgumentSlots(generic_suffix_text);
  return generic_suffix_terminated &&
         object_pointer_type_spelling &&
         import_edge_candidates > 0;
}

static std::string BuildNamespaceCollisionShadowingProfile(
    bool object_pointer_type_spelling,
    bool has_generic_suffix,
    bool generic_suffix_terminated,
    bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name) {
  const std::size_t import_edge_candidates =
      has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text) : 0;
  const std::size_t namespace_segments = CountNamespaceSegments(object_pointer_type_name);
  const bool namespace_collision_risk = namespace_segments > 1 && import_edge_candidates > 0;
  const bool shadowing_risk = has_pointer_declarator && namespace_segments > 1;
  const bool diagnostics_ready =
      !namespace_collision_risk ||
      (generic_suffix_terminated && object_pointer_type_spelling);

  std::ostringstream out;
  out << "namespace-collision-shadowing:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";namespace-segments=" << namespace_segments
      << ";import-edge-candidates=" << import_edge_candidates
      << ";namespace-collision-risk=" << (namespace_collision_risk ? "true" : "false")
      << ";shadowing-risk=" << (shadowing_risk ? "true" : "false")
      << ";diagnostics-ready=" << (diagnostics_ready ? "true" : "false");
  return out.str();
}

static bool IsNamespaceCollisionShadowingProfileNormalized(
    bool object_pointer_type_spelling,
    bool has_generic_suffix,
    bool generic_suffix_terminated,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name) {
  const std::size_t import_edge_candidates =
      has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text) : 0;
  const std::size_t namespace_segments = CountNamespaceSegments(object_pointer_type_name);
  const bool namespace_collision_risk = namespace_segments > 1 && import_edge_candidates > 0;
  if (!namespace_collision_risk) {
    return true;
  }
  return generic_suffix_terminated &&
         object_pointer_type_spelling;
}

static std::string BuildPublicPrivateApiPartitionProfile(
    bool object_pointer_type_spelling,
    bool has_generic_suffix,
    bool generic_suffix_terminated,
    bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name) {
  const std::size_t import_edge_candidates =
      has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text)
                         : 0;
  const std::size_t namespace_segments =
      CountNamespaceSegments(object_pointer_type_name);
  const bool private_partition_required = namespace_segments > 1;
  const bool public_api_safe = !private_partition_required;
  const bool partition_ready = !private_partition_required ||
                               (generic_suffix_terminated &&
                                object_pointer_type_spelling);
  const bool pointer_partition_overlap =
      has_pointer_declarator && private_partition_required;

  std::ostringstream out;
  out << "public-private-api-partition:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";namespace-segments=" << namespace_segments
      << ";import-edge-candidates=" << import_edge_candidates
      << ";public-api-safe=" << (public_api_safe ? "true" : "false")
      << ";private-partition-required="
      << (private_partition_required ? "true" : "false")
      << ";partition-ready=" << (partition_ready ? "true" : "false")
      << ";pointer-partition-overlap="
      << (pointer_partition_overlap ? "true" : "false");
  return out.str();
}

static bool IsPublicPrivateApiPartitionProfileNormalized(
    bool object_pointer_type_spelling,
    bool has_generic_suffix,
    bool generic_suffix_terminated,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name) {
  const std::size_t import_edge_candidates =
      has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text)
                         : 0;
  const std::size_t namespace_segments =
      CountNamespaceSegments(object_pointer_type_name);
  const bool private_partition_required = namespace_segments > 1;
  if (!private_partition_required) {
    return true;
  }
  if (import_edge_candidates == 0) {
    return object_pointer_type_spelling;
  }
  return generic_suffix_terminated && object_pointer_type_spelling;
}

static std::string BuildIncrementalModuleCacheInvalidationProfile(
    bool object_pointer_type_spelling,
    bool has_generic_suffix,
    bool generic_suffix_terminated,
    bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name) {
  const std::size_t import_edge_candidates =
      has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text)
                         : 0;
  const std::size_t namespace_segments =
      CountNamespaceSegments(object_pointer_type_name);
  const bool cache_key_ready =
      object_pointer_type_spelling &&
      (!has_generic_suffix ||
       (generic_suffix_terminated && import_edge_candidates > 0));
  const bool cache_partitioned = namespace_segments > 1;
  const bool invalidation_on_shape_change =
      has_generic_suffix || has_pointer_declarator || cache_partitioned;
  const bool invalidation_ready =
      !invalidation_on_shape_change || cache_key_ready;

  std::ostringstream out;
  out << "incremental-module-cache-invalidation:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";namespace-segments=" << namespace_segments
      << ";import-edge-candidates=" << import_edge_candidates
      << ";cache-key-ready=" << (cache_key_ready ? "true" : "false")
      << ";cache-partitioned=" << (cache_partitioned ? "true" : "false")
      << ";invalidation-on-shape-change="
      << (invalidation_on_shape_change ? "true" : "false")
      << ";invalidation-ready=" << (invalidation_ready ? "true" : "false");
  return out.str();
}

static bool IsIncrementalModuleCacheInvalidationProfileNormalized(
    bool object_pointer_type_spelling,
    bool has_generic_suffix,
    bool generic_suffix_terminated,
    bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name) {
  const std::size_t import_edge_candidates =
      has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text)
                         : 0;
  const std::size_t namespace_segments =
      CountNamespaceSegments(object_pointer_type_name);
  if (namespace_segments > 1 && !object_pointer_type_spelling) {
    return false;
  }
  if (has_pointer_declarator && !object_pointer_type_spelling) {
    return false;
  }
  if (!has_generic_suffix) {
    return true;
  }
  return generic_suffix_terminated &&
         object_pointer_type_spelling &&
         import_edge_candidates > 0;
}

static std::string BuildCrossModuleConformanceProfile(
    bool object_pointer_type_spelling,
    bool has_generic_suffix,
    bool generic_suffix_terminated,
    bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name) {
  const std::size_t import_edge_candidates =
      has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text)
                         : 0;
  const std::size_t namespace_segments =
      CountNamespaceSegments(object_pointer_type_name);
  const bool cross_module_boundary_engaged =
      namespace_segments > 1 || has_generic_suffix;
  const bool conformance_surface_ready =
      object_pointer_type_spelling &&
      (!has_generic_suffix ||
       (generic_suffix_terminated && import_edge_candidates > 0));
  const bool boundary_shape_stable =
      !cross_module_boundary_engaged || conformance_surface_ready;
  const bool pointer_boundary_coupling =
      has_pointer_declarator && cross_module_boundary_engaged;
  const bool deterministic_handoff =
      boundary_shape_stable &&
      (!has_pointer_declarator || object_pointer_type_spelling);

  std::ostringstream out;
  out << "cross-module-conformance:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";namespace-segments=" << namespace_segments
      << ";import-edge-candidates=" << import_edge_candidates
      << ";cross-module-boundary-engaged="
      << (cross_module_boundary_engaged ? "true" : "false")
      << ";conformance-surface-ready="
      << (conformance_surface_ready ? "true" : "false")
      << ";boundary-shape-stable="
      << (boundary_shape_stable ? "true" : "false")
      << ";pointer-boundary-coupling="
      << (pointer_boundary_coupling ? "true" : "false")
      << ";deterministic-handoff="
      << (deterministic_handoff ? "true" : "false");
  return out.str();
}

static bool IsCrossModuleConformanceProfileNormalized(
    bool object_pointer_type_spelling,
    bool has_generic_suffix,
    bool generic_suffix_terminated,
    bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name) {
  const std::size_t import_edge_candidates =
      has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text)
                         : 0;
  const std::size_t namespace_segments =
      CountNamespaceSegments(object_pointer_type_name);
  if (has_pointer_declarator && !object_pointer_type_spelling) {
    return false;
  }
  if (namespace_segments <= 1 && !has_generic_suffix) {
    return true;
  }
  if (!object_pointer_type_spelling) {
    return false;
  }
  if (!has_generic_suffix) {
    return true;
  }
  return generic_suffix_terminated && import_edge_candidates > 0;
}

static std::string BuildThrowsDeclarationProfile(
    bool throws_declared,
    bool has_return_annotation,
    bool is_prototype,
    bool has_body,
    bool is_method_declaration,
    bool is_class_method,
    std::size_t parameter_count,
    std::size_t selector_piece_count) {
  const bool declaration_shape_valid =
      (is_prototype && !has_body) || (!is_prototype && has_body);
  const bool method_selector_surface_ready =
      !is_method_declaration || selector_piece_count > 0;
  const bool propagation_ready = declaration_shape_valid && method_selector_surface_ready;

  std::ostringstream out;
  out << "throws-declaration:declared=" << (throws_declared ? "true" : "false")
      << ";has-return-annotation=" << (has_return_annotation ? "true" : "false")
      << ";prototype=" << (is_prototype ? "true" : "false")
      << ";has-body=" << (has_body ? "true" : "false")
      << ";is-method-declaration=" << (is_method_declaration ? "true" : "false")
      << ";is-class-method=" << (is_class_method ? "true" : "false")
      << ";parameter-count=" << parameter_count
      << ";selector-piece-count=" << selector_piece_count
      << ";declaration-shape-valid=" << (declaration_shape_valid ? "true" : "false")
      << ";method-selector-surface-ready=" << (method_selector_surface_ready ? "true" : "false")
      << ";propagation-ready=" << (propagation_ready ? "true" : "false");
  return out.str();
}

static bool IsThrowsDeclarationProfileNormalized(
    bool is_prototype,
    bool has_body,
    bool is_method_declaration,
    std::size_t selector_piece_count) {
  const bool declaration_shape_valid =
      (is_prototype && !has_body) || (!is_prototype && has_body);
  if (!declaration_shape_valid) {
    return false;
  }
  if (!is_method_declaration) {
    return true;
  }
  return selector_piece_count > 0;
}

struct Objc3ResultLikeProfile {
  std::size_t result_like_sites = 0;
  std::size_t result_success_sites = 0;
  std::size_t result_failure_sites = 0;
  std::size_t result_branch_sites = 0;
  std::size_t result_payload_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t branch_merge_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic_result_like_lowering_handoff = false;
};

static std::string BuildResultLikeProfile(
    std::size_t result_like_sites,
    std::size_t result_success_sites,
    std::size_t result_failure_sites,
    std::size_t result_branch_sites,
    std::size_t result_payload_sites,
    std::size_t normalized_sites,
    std::size_t branch_merge_sites,
    std::size_t contract_violation_sites,
    bool deterministic_result_like_lowering_handoff) {
  std::ostringstream out;
  out << "result-like-lowering:result_like_sites=" << result_like_sites
      << ";result_success_sites=" << result_success_sites
      << ";result_failure_sites=" << result_failure_sites
      << ";result_branch_sites=" << result_branch_sites
      << ";result_payload_sites=" << result_payload_sites
      << ";normalized_sites=" << normalized_sites
      << ";branch_merge_sites=" << branch_merge_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_result_like_lowering_handoff="
      << (deterministic_result_like_lowering_handoff ? "true" : "false");
  return out.str();
}

static bool IsResultLikeProfileNormalized(
    std::size_t result_like_sites,
    std::size_t result_success_sites,
    std::size_t result_failure_sites,
    std::size_t result_branch_sites,
    std::size_t result_payload_sites,
    std::size_t normalized_sites,
    std::size_t branch_merge_sites,
    std::size_t contract_violation_sites) {
  if (result_success_sites + result_failure_sites != normalized_sites) {
    return false;
  }
  if (result_success_sites > result_like_sites || result_failure_sites > result_like_sites ||
      result_branch_sites > result_like_sites || result_payload_sites > result_like_sites) {
    return false;
  }
  if (normalized_sites + branch_merge_sites != result_like_sites) {
    return false;
  }
  return contract_violation_sites == 0;
}

static bool IsResultLikeFailureExpr(const Expr *expr) {
  if (expr == nullptr) {
    return false;
  }
  switch (expr->kind) {
  case Expr::Kind::NilLiteral:
    return true;
  case Expr::Kind::BoolLiteral:
    return !expr->bool_value;
  case Expr::Kind::Number:
    return expr->number == 0;
  case Expr::Kind::Identifier:
    return expr->ident == "err" || expr->ident == "error" || expr->ident == "failure";
  default:
    return false;
  }
}

static void CollectResultLikeExprProfile(const Expr *expr, Objc3ResultLikeProfile &profile) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::Binary:
    CollectResultLikeExprProfile(expr->left.get(), profile);
    CollectResultLikeExprProfile(expr->right.get(), profile);
    return;
  case Expr::Kind::Conditional:
    profile.result_like_sites += 1u;
    profile.result_branch_sites += 1u;
    profile.branch_merge_sites += 1u;
    CollectResultLikeExprProfile(expr->left.get(), profile);
    CollectResultLikeExprProfile(expr->right.get(), profile);
    CollectResultLikeExprProfile(expr->third.get(), profile);
    return;
  case Expr::Kind::Call:
    for (const auto &arg : expr->args) {
      CollectResultLikeExprProfile(arg.get(), profile);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectResultLikeExprProfile(expr->receiver.get(), profile);
    for (const auto &arg : expr->args) {
      CollectResultLikeExprProfile(arg.get(), profile);
    }
    return;
  case Expr::Kind::BlockLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::Number:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::NilLiteral:
  default:
    return;
  }
}

static void CollectResultLikeForClauseProfile(const ForClause &clause, Objc3ResultLikeProfile &profile) {
  CollectResultLikeExprProfile(clause.value.get(), profile);
}

static void CollectResultLikeStmtProfile(const Stmt *stmt, Objc3ResultLikeProfile &profile) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectResultLikeExprProfile(stmt->let_stmt->value.get(), profile);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectResultLikeExprProfile(stmt->assign_stmt->value.get(), profile);
    }
    return;
  case Stmt::Kind::Return:
    profile.result_like_sites += 1u;
    profile.normalized_sites += 1u;
    if (stmt->return_stmt != nullptr && stmt->return_stmt->value != nullptr) {
      profile.result_payload_sites += 1u;
      CollectResultLikeExprProfile(stmt->return_stmt->value.get(), profile);
      if (IsResultLikeFailureExpr(stmt->return_stmt->value.get())) {
        profile.result_failure_sites += 1u;
      } else {
        profile.result_success_sites += 1u;
      }
    } else {
      profile.result_success_sites += 1u;
    }
    return;
  case Stmt::Kind::If:
    profile.result_like_sites += 1u;
    profile.result_branch_sites += 1u;
    profile.branch_merge_sites += 1u;
    if (stmt->if_stmt != nullptr) {
      CollectResultLikeExprProfile(stmt->if_stmt->condition.get(), profile);
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        CollectResultLikeStmtProfile(then_stmt.get(), profile);
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        CollectResultLikeStmtProfile(else_stmt.get(), profile);
      }
    }
    return;
  case Stmt::Kind::DoWhile:
    profile.result_like_sites += 1u;
    profile.result_branch_sites += 1u;
    profile.branch_merge_sites += 1u;
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        CollectResultLikeStmtProfile(body_stmt.get(), profile);
      }
      CollectResultLikeExprProfile(stmt->do_while_stmt->condition.get(), profile);
    }
    return;
  case Stmt::Kind::For:
    profile.result_like_sites += 1u;
    profile.result_branch_sites += 1u;
    profile.branch_merge_sites += 1u;
    if (stmt->for_stmt != nullptr) {
      CollectResultLikeForClauseProfile(stmt->for_stmt->init, profile);
      CollectResultLikeExprProfile(stmt->for_stmt->condition.get(), profile);
      CollectResultLikeForClauseProfile(stmt->for_stmt->step, profile);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        CollectResultLikeStmtProfile(body_stmt.get(), profile);
      }
    }
    return;
  case Stmt::Kind::Switch:
    profile.result_like_sites += 1u;
    profile.result_branch_sites += 1u;
    profile.branch_merge_sites += 1u;
    if (stmt->switch_stmt != nullptr) {
      CollectResultLikeExprProfile(stmt->switch_stmt->condition.get(), profile);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          CollectResultLikeStmtProfile(case_stmt.get(), profile);
        }
      }
    }
    return;
  case Stmt::Kind::While:
    profile.result_like_sites += 1u;
    profile.result_branch_sites += 1u;
    profile.branch_merge_sites += 1u;
    if (stmt->while_stmt != nullptr) {
      CollectResultLikeExprProfile(stmt->while_stmt->condition.get(), profile);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        CollectResultLikeStmtProfile(body_stmt.get(), profile);
      }
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &body_stmt : stmt->block_stmt->body) {
        CollectResultLikeStmtProfile(body_stmt.get(), profile);
      }
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectResultLikeExprProfile(stmt->expr_stmt->value.get(), profile);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

static Objc3ResultLikeProfile BuildResultLikeProfileFromBody(const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3ResultLikeProfile profile;
  for (const auto &stmt : body) {
    CollectResultLikeStmtProfile(stmt.get(), profile);
  }

  if (profile.result_success_sites + profile.result_failure_sites != profile.normalized_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.result_success_sites > profile.result_like_sites ||
      profile.result_failure_sites > profile.result_like_sites ||
      profile.result_branch_sites > profile.result_like_sites ||
      profile.result_payload_sites > profile.result_like_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.branch_merge_sites != profile.result_like_sites) {
    profile.contract_violation_sites += 1u;
  }
  profile.deterministic_result_like_lowering_handoff = profile.contract_violation_sites == 0u;
  return profile;
}

static Objc3ResultLikeProfile BuildResultLikeProfileFromOpaqueBody(bool has_body) {
  Objc3ResultLikeProfile profile;
  if (has_body) {
    profile.result_like_sites = 1u;
    profile.result_branch_sites = 1u;
    profile.branch_merge_sites = 1u;
  }
  profile.deterministic_result_like_lowering_handoff = true;
  return profile;
}

struct Objc3NSErrorBridgingProfile {
  std::size_t ns_error_bridging_sites = 0;
  std::size_t ns_error_parameter_sites = 0;
  std::size_t ns_error_out_parameter_sites = 0;
  std::size_t ns_error_bridge_path_sites = 0;
  std::size_t failable_call_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t bridge_boundary_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic_ns_error_bridging_lowering_handoff = false;
};

static std::string BuildLowercaseProfileToken(std::string token) {
  std::transform(token.begin(), token.end(), token.begin(), [](unsigned char c) {
    return static_cast<char>(std::tolower(c));
  });
  return token;
}

static bool IsNSErrorTypeSpelling(const FuncParam &param) {
  if (!param.object_pointer_type_spelling) {
    return false;
  }
  return BuildLowercaseProfileToken(param.object_pointer_type_name) == "nserror";
}

static bool IsNSErrorOutParameterSite(const FuncParam &param) {
  if (!IsNSErrorTypeSpelling(param)) {
    return false;
  }
  const std::string lowered_name = BuildLowercaseProfileToken(param.name);
  return param.has_pointer_declarator || lowered_name.find("error") != std::string::npos;
}

static bool IsFailableCallSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("error") != std::string::npos || lowered.find("fail") != std::string::npos ||
         lowered.find("try") != std::string::npos;
}

static std::size_t CountFailableCallSitesInExpr(const Expr *expr) {
  if (expr == nullptr) {
    return 0;
  }
  switch (expr->kind) {
  case Expr::Kind::Call: {
    std::size_t count = IsFailableCallSymbol(expr->ident) ? 1u : 0u;
    for (const auto &arg : expr->args) {
      count += CountFailableCallSitesInExpr(arg.get());
    }
    return count;
  }
  case Expr::Kind::MessageSend: {
    std::size_t count = IsFailableCallSymbol(expr->selector) ? 1u : 0u;
    count += CountFailableCallSitesInExpr(expr->receiver.get());
    for (const auto &arg : expr->args) {
      count += CountFailableCallSitesInExpr(arg.get());
    }
    return count;
  }
  case Expr::Kind::Binary:
    return CountFailableCallSitesInExpr(expr->left.get()) + CountFailableCallSitesInExpr(expr->right.get());
  case Expr::Kind::Conditional:
    return CountFailableCallSitesInExpr(expr->left.get()) + CountFailableCallSitesInExpr(expr->right.get()) +
           CountFailableCallSitesInExpr(expr->third.get());
  case Expr::Kind::BlockLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::Number:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::NilLiteral:
  default:
    return 0;
  }
}

static std::size_t CountFailableCallSitesInForClause(const ForClause &clause) {
  return CountFailableCallSitesInExpr(clause.value.get());
}

static std::size_t CountFailableCallSitesInStmt(const Stmt *stmt) {
  if (stmt == nullptr) {
    return 0;
  }

  switch (stmt->kind) {
  case Stmt::Kind::Let:
    return stmt->let_stmt == nullptr ? 0u : CountFailableCallSitesInExpr(stmt->let_stmt->value.get());
  case Stmt::Kind::Assign:
    return stmt->assign_stmt == nullptr ? 0u : CountFailableCallSitesInExpr(stmt->assign_stmt->value.get());
  case Stmt::Kind::Return:
    return stmt->return_stmt == nullptr ? 0u : CountFailableCallSitesInExpr(stmt->return_stmt->value.get());
  case Stmt::Kind::If: {
    if (stmt->if_stmt == nullptr) {
      return 0;
    }
    std::size_t count = CountFailableCallSitesInExpr(stmt->if_stmt->condition.get());
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      count += CountFailableCallSitesInStmt(then_stmt.get());
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      count += CountFailableCallSitesInStmt(else_stmt.get());
    }
    return count;
  }
  case Stmt::Kind::DoWhile: {
    if (stmt->do_while_stmt == nullptr) {
      return 0;
    }
    std::size_t count = CountFailableCallSitesInExpr(stmt->do_while_stmt->condition.get());
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      count += CountFailableCallSitesInStmt(body_stmt.get());
    }
    return count;
  }
  case Stmt::Kind::For: {
    if (stmt->for_stmt == nullptr) {
      return 0;
    }
    std::size_t count = CountFailableCallSitesInForClause(stmt->for_stmt->init);
    count += CountFailableCallSitesInExpr(stmt->for_stmt->condition.get());
    count += CountFailableCallSitesInForClause(stmt->for_stmt->step);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      count += CountFailableCallSitesInStmt(body_stmt.get());
    }
    return count;
  }
  case Stmt::Kind::Switch: {
    if (stmt->switch_stmt == nullptr) {
      return 0;
    }
    std::size_t count = CountFailableCallSitesInExpr(stmt->switch_stmt->condition.get());
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        count += CountFailableCallSitesInStmt(case_stmt.get());
      }
    }
    return count;
  }
  case Stmt::Kind::While: {
    if (stmt->while_stmt == nullptr) {
      return 0;
    }
    std::size_t count = CountFailableCallSitesInExpr(stmt->while_stmt->condition.get());
    for (const auto &body_stmt : stmt->while_stmt->body) {
      count += CountFailableCallSitesInStmt(body_stmt.get());
    }
    return count;
  }
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer: {
    if (stmt->block_stmt == nullptr) {
      return 0;
    }
    std::size_t count = 0;
    for (const auto &body_stmt : stmt->block_stmt->body) {
      count += CountFailableCallSitesInStmt(body_stmt.get());
    }
    return count;
  }
  case Stmt::Kind::Expr:
    return stmt->expr_stmt == nullptr ? 0u : CountFailableCallSitesInExpr(stmt->expr_stmt->value.get());
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return 0;
  }
}

static std::size_t CountFailableCallSitesInBody(const std::vector<std::unique_ptr<Stmt>> &body) {
  std::size_t count = 0;
  for (const auto &stmt : body) {
    count += CountFailableCallSitesInStmt(stmt.get());
  }
  return count;
}

static std::string BuildNSErrorBridgingProfile(
    std::size_t ns_error_bridging_sites,
    std::size_t ns_error_parameter_sites,
    std::size_t ns_error_out_parameter_sites,
    std::size_t ns_error_bridge_path_sites,
    std::size_t failable_call_sites,
    std::size_t normalized_sites,
    std::size_t bridge_boundary_sites,
    std::size_t contract_violation_sites,
    bool deterministic_ns_error_bridging_lowering_handoff) {
  std::ostringstream out;
  out << "ns-error-bridging:ns_error_bridging_sites=" << ns_error_bridging_sites
      << ";ns_error_parameter_sites=" << ns_error_parameter_sites
      << ";ns_error_out_parameter_sites=" << ns_error_out_parameter_sites
      << ";ns_error_bridge_path_sites=" << ns_error_bridge_path_sites
      << ";failable_call_sites=" << failable_call_sites
      << ";normalized_sites=" << normalized_sites
      << ";bridge_boundary_sites=" << bridge_boundary_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_ns_error_bridging_lowering_handoff="
      << (deterministic_ns_error_bridging_lowering_handoff ? "true" : "false");
  return out.str();
}

static bool IsNSErrorBridgingProfileNormalized(
    std::size_t ns_error_bridging_sites,
    std::size_t ns_error_parameter_sites,
    std::size_t ns_error_out_parameter_sites,
    std::size_t ns_error_bridge_path_sites,
    std::size_t failable_call_sites,
    std::size_t normalized_sites,
    std::size_t bridge_boundary_sites,
    std::size_t contract_violation_sites) {
  if (ns_error_out_parameter_sites > ns_error_parameter_sites) {
    return false;
  }
  if (ns_error_bridge_path_sites > ns_error_out_parameter_sites ||
      ns_error_bridge_path_sites > failable_call_sites) {
    return false;
  }
  if (normalized_sites + bridge_boundary_sites != ns_error_bridging_sites) {
    return false;
  }
  if (ns_error_parameter_sites > ns_error_bridging_sites ||
      ns_error_out_parameter_sites > ns_error_bridging_sites ||
      ns_error_bridge_path_sites > ns_error_bridging_sites ||
      failable_call_sites > ns_error_bridging_sites ||
      normalized_sites > ns_error_bridging_sites ||
      bridge_boundary_sites > ns_error_bridging_sites) {
    return false;
  }
  return contract_violation_sites == 0;
}

static Objc3NSErrorBridgingProfile BuildNSErrorBridgingProfileFromParameters(
    const std::vector<FuncParam> &params,
    std::size_t raw_failable_call_sites) {
  Objc3NSErrorBridgingProfile profile;
  for (const auto &param : params) {
    if (IsNSErrorTypeSpelling(param)) {
      profile.ns_error_parameter_sites += 1u;
      if (IsNSErrorOutParameterSite(param)) {
        profile.ns_error_out_parameter_sites += 1u;
      }
    }
  }

  profile.ns_error_bridge_path_sites = std::min(profile.ns_error_out_parameter_sites, raw_failable_call_sites);
  profile.normalized_sites = profile.ns_error_parameter_sites + profile.ns_error_out_parameter_sites;
  profile.bridge_boundary_sites = profile.ns_error_bridge_path_sites;
  profile.ns_error_bridging_sites = profile.normalized_sites + profile.bridge_boundary_sites;
  profile.failable_call_sites = std::min(raw_failable_call_sites, profile.ns_error_bridging_sites);

  if (profile.ns_error_out_parameter_sites > profile.ns_error_parameter_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.ns_error_bridge_path_sites > profile.ns_error_out_parameter_sites ||
      profile.ns_error_bridge_path_sites > profile.failable_call_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.bridge_boundary_sites != profile.ns_error_bridging_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.ns_error_parameter_sites > profile.ns_error_bridging_sites ||
      profile.ns_error_out_parameter_sites > profile.ns_error_bridging_sites ||
      profile.ns_error_bridge_path_sites > profile.ns_error_bridging_sites ||
      profile.failable_call_sites > profile.ns_error_bridging_sites ||
      profile.normalized_sites > profile.ns_error_bridging_sites ||
      profile.bridge_boundary_sites > profile.ns_error_bridging_sites) {
    profile.contract_violation_sites += 1u;
  }

  profile.deterministic_ns_error_bridging_lowering_handoff = profile.contract_violation_sites == 0u;
