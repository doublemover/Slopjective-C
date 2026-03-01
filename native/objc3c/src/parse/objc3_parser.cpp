#include "parse/objc3_parser.h"
#include "parse/objc3_ast_builder.h"

#include <algorithm>
#include <cerrno>
#include <cctype>
#include <cstdlib>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

namespace {

using Token = Objc3LexToken;
using TokenKind = Objc3LexTokenKind;

static bool IsHexDigit(char c) {
  return std::isxdigit(static_cast<unsigned char>(c)) != 0;
}

static bool IsBinaryDigit(char c) {
  return c == '0' || c == '1';
}

static bool IsOctalDigit(char c) {
  return c >= '0' && c <= '7';
}

static bool IsDigitSeparator(char c) {
  return c == '_';
}

static bool IsDigitForBase(char c, int base) {
  switch (base) {
  case 2:
    return IsBinaryDigit(c);
  case 8:
    return IsOctalDigit(c);
  case 10:
    return std::isdigit(static_cast<unsigned char>(c)) != 0;
  case 16:
    return IsHexDigit(c);
  default:
    return false;
  }
}

static bool NormalizeIntegerDigits(const std::string &digits, int base, std::string &normalized) {
  normalized.clear();
  if (digits.empty()) {
    return false;
  }

  bool previous_was_digit = false;
  for (std::size_t i = 0; i < digits.size(); ++i) {
    const char c = digits[i];
    if (IsDigitSeparator(c)) {
      if (!previous_was_digit || i + 1 >= digits.size() || !IsDigitForBase(digits[i + 1], base)) {
        return false;
      }
      previous_was_digit = false;
      continue;
    }

    if (!IsDigitForBase(c, base)) {
      return false;
    }
    normalized.push_back(c);
    previous_was_digit = true;
  }

  return !normalized.empty() && previous_was_digit;
}

static bool ParseIntegerLiteralValue(const std::string &text, int &value) {
  if (text.empty()) {
    return false;
  }

  int base = 10;
  std::string digit_text = text;
  if (text.size() > 2 && text[0] == '0' && (text[1] == 'b' || text[1] == 'B')) {
    base = 2;
    digit_text = text.substr(2);
  } else if (text.size() > 2 && text[0] == '0' && (text[1] == 'o' || text[1] == 'O')) {
    base = 8;
    digit_text = text.substr(2);
  } else if (text.size() > 2 && text[0] == '0' && (text[1] == 'x' || text[1] == 'X')) {
    base = 16;
    digit_text = text.substr(2);
  }

  std::string normalized_digits;
  if (!NormalizeIntegerDigits(digit_text, base, normalized_digits)) {
    return false;
  }

  errno = 0;
  char *end = nullptr;
  const char *raw = normalized_digits.c_str();
  const long parsed = std::strtol(raw, &end, base);
  if (end == raw || end == nullptr || *end != '\0' || errno == ERANGE ||
      parsed < std::numeric_limits<int>::min() || parsed > std::numeric_limits<int>::max()) {
    return false;
  }

  value = static_cast<int>(parsed);
  return true;
}

static std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message) {
  std::ostringstream out;
  out << "error:" << line << ":" << column << ": " << message << " [" << code << "]";
  return out.str();
}

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

static std::string BuildMessageSendFormSymbol(Expr::MessageSendForm form) {
  switch (form) {
  case Expr::MessageSendForm::Unary:
    return "message-send-form:unary";
  case Expr::MessageSendForm::Keyword:
    return "message-send-form:keyword";
  case Expr::MessageSendForm::None:
  default:
    return "message-send-form:none";
  }
}

static std::string BuildMessageSendSelectorLoweringSymbol(
    const std::vector<Expr::MessageSendSelectorPiece> &pieces) {
  std::string normalized_selector;
  for (const auto &piece : pieces) {
    normalized_selector += piece.keyword;
    if (piece.has_argument) {
      normalized_selector += ":";
    }
  }
  return "selector-lowering:" + normalized_selector;
}

static std::string BuildAutoreleasePoolScopeSymbol(unsigned serial, unsigned depth) {
  std::ostringstream out;
  out << "autoreleasepool-scope:" << serial << ";depth=" << depth;
  return out.str();
}

constexpr unsigned kDispatchAbiMarshallingRuntimeArgSlots = 4u;
constexpr const char *kRuntimeShimHostLinkDispatchSymbol = "objc3_msgsend_i32";

static unsigned ComputeDispatchAbiArgumentPaddingSlots(std::size_t argument_count,
                                                       unsigned runtime_arg_slots) {
  if (runtime_arg_slots == 0u) {
    return 0u;
  }
  const std::size_t remainder = argument_count % runtime_arg_slots;
  if (remainder == 0u) {
    return 0u;
  }
  return static_cast<unsigned>(runtime_arg_slots - static_cast<unsigned>(remainder));
}

static std::string BuildDispatchAbiMarshallingSymbol(unsigned receiver_slots,
                                                     unsigned selector_slots,
                                                     unsigned argument_value_slots,
                                                     unsigned argument_padding_slots,
                                                     unsigned argument_total_slots,
                                                     unsigned total_slots,
                                                     unsigned runtime_arg_slots) {
  std::ostringstream out;
  out << "dispatch-abi-marshalling:recv=" << receiver_slots << ";sel=" << selector_slots
      << ";arg-values=" << argument_value_slots << ";arg-padding=" << argument_padding_slots
      << ";arg-total=" << argument_total_slots << ";total=" << total_slots
      << ";runtime-slots=" << runtime_arg_slots;
  return out.str();
}

static std::string BuildNilReceiverFoldingSymbol(bool nil_receiver_foldable,
                                                 bool requires_runtime_dispatch,
                                                 Expr::MessageSendForm form) {
  std::ostringstream out;
  out << "nil-receiver:foldable=" << (nil_receiver_foldable ? "true" : "false")
      << ";runtime-dispatch=" << (requires_runtime_dispatch ? "required" : "elided")
      << ";form=";
  switch (form) {
  case Expr::MessageSendForm::Unary:
    out << "unary";
    break;
  case Expr::MessageSendForm::Keyword:
    out << "keyword";
    break;
  case Expr::MessageSendForm::None:
  default:
    out << "none";
    break;
  }
  return out.str();
}

static bool IsSuperDispatchReceiver(const Expr &receiver) {
  return receiver.kind == Expr::Kind::Identifier && receiver.ident == "super";
}

static std::string ClassifyMethodFamilyFromSelector(const std::string &selector) {
  if (selector.rfind("mutableCopy", 0) == 0) {
    return "mutableCopy";
  }
  if (selector.rfind("copy", 0) == 0) {
    return "copy";
  }
  if (selector.rfind("init", 0) == 0) {
    return "init";
  }
  if (selector.rfind("new", 0) == 0) {
    return "new";
  }
  return "none";
}

static std::string BuildSuperDispatchSymbol(bool super_dispatch_enabled,
                                            bool super_dispatch_requires_class_context,
                                            Expr::MessageSendForm form) {
  std::ostringstream out;
  out << "super-dispatch:enabled=" << (super_dispatch_enabled ? "true" : "false")
      << ";class-context=" << (super_dispatch_requires_class_context ? "required" : "not-required")
      << ";form=";
  switch (form) {
  case Expr::MessageSendForm::Unary:
    out << "unary";
    break;
  case Expr::MessageSendForm::Keyword:
    out << "keyword";
    break;
  case Expr::MessageSendForm::None:
  default:
    out << "none";
    break;
  }
  return out.str();
}

static std::string BuildMethodFamilySemanticsSymbol(const std::string &method_family_name,
                                                    bool returns_retained_result,
                                                    bool returns_related_result) {
  std::ostringstream out;
  out << "method-family:name=" << method_family_name
      << ";returns-retained=" << (returns_retained_result ? "true" : "false")
      << ";returns-related=" << (returns_related_result ? "true" : "false");
  return out.str();
}

static std::string BuildRuntimeShimHostLinkSymbol(bool runtime_shim_required,
                                                  bool runtime_shim_elided,
                                                  unsigned runtime_dispatch_arg_slots,
                                                  unsigned runtime_dispatch_declaration_parameter_count,
                                                  const std::string &runtime_dispatch_symbol,
                                                  Expr::MessageSendForm form) {
  std::ostringstream out;
  out << "runtime-shim-host-link:required=" << (runtime_shim_required ? "true" : "false")
      << ";elided=" << (runtime_shim_elided ? "true" : "false")
      << ";runtime-slots=" << runtime_dispatch_arg_slots
      << ";decl-params=" << runtime_dispatch_declaration_parameter_count
      << ";symbol=" << runtime_dispatch_symbol
      << ";form=";
  switch (form) {
  case Expr::MessageSendForm::Unary:
    out << "unary";
    break;
  case Expr::MessageSendForm::Keyword:
    out << "keyword";
    break;
  case Expr::MessageSendForm::None:
  default:
    out << "none";
    break;
  }
  return out.str();
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
  case Stmt::Kind::Block: {
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
  return profile;
}

static Objc3NSErrorBridgingProfile BuildNSErrorBridgingProfileFromFunction(const FunctionDecl &fn) {
  return BuildNSErrorBridgingProfileFromParameters(fn.params, CountFailableCallSitesInBody(fn.body));
}

static Objc3NSErrorBridgingProfile BuildNSErrorBridgingProfileFromOpaqueBody(const Objc3MethodDecl &method) {
  std::size_t raw_failable_call_sites = 0;
  if (method.has_body) {
    for (const auto &param : method.params) {
      if (IsNSErrorOutParameterSite(param)) {
        raw_failable_call_sites = 1u;
        break;
      }
    }
  }
  return BuildNSErrorBridgingProfileFromParameters(method.params, raw_failable_call_sites);
}

static bool IsUnsafeOwnershipQualifierSpelling(const std::string &spelling) {
  return spelling == "__unsafe_unretained";
}

static std::size_t CountRawPointerTypeSites(const std::vector<FuncParam> &params,
                                            bool has_return_pointer_declarator) {
  std::size_t sites = has_return_pointer_declarator ? 1u : 0u;
  for (const auto &param : params) {
    if (param.has_pointer_declarator) {
      sites += 1u;
    }
  }
  return sites;
}

static std::size_t CountUnsafeKeywordSites(const std::vector<FuncParam> &params,
                                           const std::string &return_ownership_qualifier_spelling) {
  std::size_t sites = IsUnsafeOwnershipQualifierSpelling(return_ownership_qualifier_spelling) ? 1u : 0u;
  for (const auto &param : params) {
    if (IsUnsafeOwnershipQualifierSpelling(param.ownership_qualifier_spelling)) {
      sites += 1u;
    }
  }
  return sites;
}

static bool IsPointerArithmeticMutationOperator(const std::string &op) {
  return op == "+=" || op == "-=" || op == "++" || op == "--";
}

static void CollectPointerArithmeticExprSites(const Expr *expr, std::size_t &sites) {
  if (expr == nullptr) {
    return;
  }

  switch (expr->kind) {
  case Expr::Kind::Binary:
    if (expr->op == "+" || expr->op == "-") {
      sites += 1u;
    }
    CollectPointerArithmeticExprSites(expr->left.get(), sites);
    CollectPointerArithmeticExprSites(expr->right.get(), sites);
    return;
  case Expr::Kind::Conditional:
    CollectPointerArithmeticExprSites(expr->left.get(), sites);
    CollectPointerArithmeticExprSites(expr->right.get(), sites);
    CollectPointerArithmeticExprSites(expr->third.get(), sites);
    return;
  case Expr::Kind::Call:
    for (const auto &arg : expr->args) {
      CollectPointerArithmeticExprSites(arg.get(), sites);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectPointerArithmeticExprSites(expr->receiver.get(), sites);
    for (const auto &arg : expr->args) {
      CollectPointerArithmeticExprSites(arg.get(), sites);
    }
    return;
  case Expr::Kind::BlockLiteral:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::NilLiteral:
  case Expr::Kind::Number:
  default:
    return;
  }
}

static void CollectPointerArithmeticForClauseSites(const ForClause &clause, std::size_t &sites) {
  if (clause.kind == ForClause::Kind::Assign && IsPointerArithmeticMutationOperator(clause.op)) {
    sites += 1u;
  }
  CollectPointerArithmeticExprSites(clause.value.get(), sites);
}

static void CollectPointerArithmeticStmtSites(const Stmt *stmt, std::size_t &sites) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectPointerArithmeticExprSites(stmt->let_stmt->value.get(), sites);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      if (IsPointerArithmeticMutationOperator(stmt->assign_stmt->op)) {
        sites += 1u;
      }
      CollectPointerArithmeticExprSites(stmt->assign_stmt->value.get(), sites);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectPointerArithmeticExprSites(stmt->return_stmt->value.get(), sites);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    CollectPointerArithmeticExprSites(stmt->if_stmt->condition.get(), sites);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      CollectPointerArithmeticStmtSites(then_stmt.get(), sites);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      CollectPointerArithmeticStmtSites(else_stmt.get(), sites);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      CollectPointerArithmeticStmtSites(body_stmt.get(), sites);
    }
    CollectPointerArithmeticExprSites(stmt->do_while_stmt->condition.get(), sites);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    CollectPointerArithmeticForClauseSites(stmt->for_stmt->init, sites);
    CollectPointerArithmeticExprSites(stmt->for_stmt->condition.get(), sites);
    CollectPointerArithmeticForClauseSites(stmt->for_stmt->step, sites);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      CollectPointerArithmeticStmtSites(body_stmt.get(), sites);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    CollectPointerArithmeticExprSites(stmt->switch_stmt->condition.get(), sites);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        CollectPointerArithmeticStmtSites(case_stmt.get(), sites);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    CollectPointerArithmeticExprSites(stmt->while_stmt->condition.get(), sites);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      CollectPointerArithmeticStmtSites(body_stmt.get(), sites);
    }
    return;
  case Stmt::Kind::Block:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      CollectPointerArithmeticStmtSites(body_stmt.get(), sites);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectPointerArithmeticExprSites(stmt->expr_stmt->value.get(), sites);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

static std::size_t CountPointerArithmeticSitesInBody(const std::vector<std::unique_ptr<Stmt>> &body) {
  std::size_t sites = 0;
  for (const auto &stmt : body) {
    CollectPointerArithmeticStmtSites(stmt.get(), sites);
  }
  return sites;
}

struct Objc3UnsafePointerExtensionProfile {
  std::size_t unsafe_pointer_extension_sites = 0;
  std::size_t unsafe_keyword_sites = 0;
  std::size_t pointer_arithmetic_sites = 0;
  std::size_t raw_pointer_type_sites = 0;
  std::size_t unsafe_operation_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic_unsafe_pointer_extension_handoff = false;
};

static std::string BuildUnsafePointerExtensionProfile(
    std::size_t unsafe_pointer_extension_sites,
    std::size_t unsafe_keyword_sites,
    std::size_t pointer_arithmetic_sites,
    std::size_t raw_pointer_type_sites,
    std::size_t unsafe_operation_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_unsafe_pointer_extension_handoff) {
  std::ostringstream out;
  out << "unsafe-pointer-extension:unsafe_pointer_extension_sites="
      << unsafe_pointer_extension_sites
      << ";unsafe_keyword_sites=" << unsafe_keyword_sites
      << ";pointer_arithmetic_sites=" << pointer_arithmetic_sites
      << ";raw_pointer_type_sites=" << raw_pointer_type_sites
      << ";unsafe_operation_sites=" << unsafe_operation_sites
      << ";normalized_sites=" << normalized_sites
      << ";gate_blocked_sites=" << gate_blocked_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_unsafe_pointer_extension_handoff="
      << (deterministic_unsafe_pointer_extension_handoff ? "true" : "false");
  return out.str();
}

static bool IsUnsafePointerExtensionProfileNormalized(
    std::size_t unsafe_pointer_extension_sites,
    std::size_t unsafe_keyword_sites,
    std::size_t pointer_arithmetic_sites,
    std::size_t raw_pointer_type_sites,
    std::size_t unsafe_operation_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites) {
  if (unsafe_keyword_sites > unsafe_pointer_extension_sites ||
      pointer_arithmetic_sites > unsafe_pointer_extension_sites ||
      raw_pointer_type_sites > unsafe_pointer_extension_sites ||
      unsafe_operation_sites > unsafe_pointer_extension_sites ||
      normalized_sites > unsafe_pointer_extension_sites ||
      gate_blocked_sites > unsafe_pointer_extension_sites) {
    return false;
  }
  if (normalized_sites + gate_blocked_sites != unsafe_pointer_extension_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}

static Objc3UnsafePointerExtensionProfile BuildUnsafePointerExtensionProfileFromCounts(
    std::size_t unsafe_keyword_sites,
    std::size_t pointer_arithmetic_sites,
    std::size_t raw_pointer_type_sites) {
  Objc3UnsafePointerExtensionProfile profile;
  profile.unsafe_keyword_sites = unsafe_keyword_sites;
  profile.pointer_arithmetic_sites = pointer_arithmetic_sites;
  profile.raw_pointer_type_sites = raw_pointer_type_sites;
  profile.unsafe_operation_sites = pointer_arithmetic_sites + raw_pointer_type_sites;
  profile.unsafe_pointer_extension_sites =
      profile.unsafe_keyword_sites + profile.pointer_arithmetic_sites + profile.raw_pointer_type_sites;

  const bool gate_open = unsafe_keyword_sites > 0u;
  profile.gate_blocked_sites =
      gate_open ? 0u : (profile.pointer_arithmetic_sites + profile.raw_pointer_type_sites);
  profile.normalized_sites = profile.unsafe_pointer_extension_sites - profile.gate_blocked_sites;

  if (profile.unsafe_keyword_sites > profile.unsafe_pointer_extension_sites ||
      profile.pointer_arithmetic_sites > profile.unsafe_pointer_extension_sites ||
      profile.raw_pointer_type_sites > profile.unsafe_pointer_extension_sites ||
      profile.unsafe_operation_sites > profile.unsafe_pointer_extension_sites ||
      profile.normalized_sites > profile.unsafe_pointer_extension_sites ||
      profile.gate_blocked_sites > profile.unsafe_pointer_extension_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.gate_blocked_sites != profile.unsafe_pointer_extension_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (!gate_open && profile.normalized_sites != profile.unsafe_keyword_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (gate_open && profile.gate_blocked_sites != 0u) {
    profile.contract_violation_sites += 1u;
  }

  profile.deterministic_unsafe_pointer_extension_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

static Objc3UnsafePointerExtensionProfile BuildUnsafePointerExtensionProfileFromFunction(
    const FunctionDecl &fn) {
  const std::size_t unsafe_keyword_sites =
      CountUnsafeKeywordSites(fn.params, fn.return_ownership_qualifier_spelling);
  const std::size_t raw_pointer_type_sites =
      CountRawPointerTypeSites(fn.params, fn.has_return_pointer_declarator);
  const std::size_t pointer_arithmetic_sites = CountPointerArithmeticSitesInBody(fn.body);
  return BuildUnsafePointerExtensionProfileFromCounts(
      unsafe_keyword_sites, pointer_arithmetic_sites, raw_pointer_type_sites);
}

static Objc3UnsafePointerExtensionProfile BuildUnsafePointerExtensionProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  const std::size_t unsafe_keyword_sites =
      CountUnsafeKeywordSites(method.params, method.return_ownership_qualifier_spelling);
  const std::size_t raw_pointer_type_sites =
      CountRawPointerTypeSites(method.params, method.has_return_pointer_declarator);
  const std::size_t pointer_arithmetic_sites =
      method.has_body && raw_pointer_type_sites > 0u ? 1u : 0u;
  return BuildUnsafePointerExtensionProfileFromCounts(
      unsafe_keyword_sites, pointer_arithmetic_sites, raw_pointer_type_sites);
}

static bool IsInlineAsmCallSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered == "asm" || lowered == "__asm" || lowered == "__asm__" ||
         lowered.rfind("asm_", 0) == 0 || lowered.rfind("__asm_", 0) == 0 ||
         lowered.find("inline_asm") != std::string::npos;
}

static bool IsIntrinsicCallSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.rfind("__builtin_", 0) == 0 || lowered.rfind("llvm.", 0) == 0 ||
         lowered.rfind("llvm_", 0) == 0 || lowered.find("intrinsic") != std::string::npos;
}

static bool IsPrivilegedIntrinsicCallSymbol(const std::string &symbol) {
  if (!IsIntrinsicCallSymbol(symbol)) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("privileged") != std::string::npos ||
         lowered.find("unsafe") != std::string::npos ||
         lowered.find("syscall") != std::string::npos ||
         lowered.rfind("__builtin_ia32_", 0) == 0 ||
         lowered.rfind("__builtin_arm_", 0) == 0;
}

struct Objc3InlineAsmIntrinsicSiteCounts {
  std::size_t inline_asm_sites = 0;
  std::size_t intrinsic_sites = 0;
  std::size_t governed_intrinsic_sites = 0;
  std::size_t privileged_intrinsic_sites = 0;
};

static void CollectInlineAsmIntrinsicSitesFromSymbol(
    const std::string &symbol,
    Objc3InlineAsmIntrinsicSiteCounts &counts) {
  if (IsInlineAsmCallSymbol(symbol)) {
    counts.inline_asm_sites += 1u;
  }
  if (IsIntrinsicCallSymbol(symbol)) {
    counts.intrinsic_sites += 1u;
    counts.governed_intrinsic_sites += 1u;
    if (IsPrivilegedIntrinsicCallSymbol(symbol)) {
      counts.privileged_intrinsic_sites += 1u;
    }
  }
}

static void CollectInlineAsmIntrinsicExprSites(
    const Expr *expr,
    Objc3InlineAsmIntrinsicSiteCounts &counts) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::Call:
    CollectInlineAsmIntrinsicSitesFromSymbol(expr->ident, counts);
    for (const auto &arg : expr->args) {
      CollectInlineAsmIntrinsicExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectInlineAsmIntrinsicSitesFromSymbol(expr->selector, counts);
    CollectInlineAsmIntrinsicExprSites(expr->receiver.get(), counts);
    for (const auto &arg : expr->args) {
      CollectInlineAsmIntrinsicExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::Binary:
    CollectInlineAsmIntrinsicExprSites(expr->left.get(), counts);
    CollectInlineAsmIntrinsicExprSites(expr->right.get(), counts);
    return;
  case Expr::Kind::Conditional:
    CollectInlineAsmIntrinsicExprSites(expr->left.get(), counts);
    CollectInlineAsmIntrinsicExprSites(expr->right.get(), counts);
    CollectInlineAsmIntrinsicExprSites(expr->third.get(), counts);
    return;
  case Expr::Kind::BlockLiteral:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::NilLiteral:
  case Expr::Kind::Number:
  default:
    return;
  }
}

static void CollectInlineAsmIntrinsicForClauseSites(
    const ForClause &clause,
    Objc3InlineAsmIntrinsicSiteCounts &counts) {
  CollectInlineAsmIntrinsicExprSites(clause.value.get(), counts);
}

static void CollectInlineAsmIntrinsicStmtSites(
    const Stmt *stmt,
    Objc3InlineAsmIntrinsicSiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectInlineAsmIntrinsicExprSites(stmt->let_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectInlineAsmIntrinsicExprSites(stmt->assign_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectInlineAsmIntrinsicExprSites(stmt->return_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    CollectInlineAsmIntrinsicExprSites(stmt->if_stmt->condition.get(), counts);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      CollectInlineAsmIntrinsicStmtSites(then_stmt.get(), counts);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      CollectInlineAsmIntrinsicStmtSites(else_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      CollectInlineAsmIntrinsicStmtSites(body_stmt.get(), counts);
    }
    CollectInlineAsmIntrinsicExprSites(stmt->do_while_stmt->condition.get(), counts);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    CollectInlineAsmIntrinsicForClauseSites(stmt->for_stmt->init, counts);
    CollectInlineAsmIntrinsicExprSites(stmt->for_stmt->condition.get(), counts);
    CollectInlineAsmIntrinsicForClauseSites(stmt->for_stmt->step, counts);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      CollectInlineAsmIntrinsicStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    CollectInlineAsmIntrinsicExprSites(stmt->switch_stmt->condition.get(), counts);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        CollectInlineAsmIntrinsicStmtSites(case_stmt.get(), counts);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    CollectInlineAsmIntrinsicExprSites(stmt->while_stmt->condition.get(), counts);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      CollectInlineAsmIntrinsicStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Block:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      CollectInlineAsmIntrinsicStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectInlineAsmIntrinsicExprSites(stmt->expr_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

static Objc3InlineAsmIntrinsicSiteCounts CountInlineAsmIntrinsicSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3InlineAsmIntrinsicSiteCounts counts;
  for (const auto &stmt : body) {
    CollectInlineAsmIntrinsicStmtSites(stmt.get(), counts);
  }
  return counts;
}

struct Objc3InlineAsmIntrinsicGovernanceProfile {
  std::size_t inline_asm_intrinsic_sites = 0;
  std::size_t inline_asm_sites = 0;
  std::size_t intrinsic_sites = 0;
  std::size_t governed_intrinsic_sites = 0;
  std::size_t privileged_intrinsic_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic_inline_asm_intrinsic_governance_handoff = false;
};

static std::string BuildInlineAsmIntrinsicGovernanceProfile(
    std::size_t inline_asm_intrinsic_sites,
    std::size_t inline_asm_sites,
    std::size_t intrinsic_sites,
    std::size_t governed_intrinsic_sites,
    std::size_t privileged_intrinsic_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_inline_asm_intrinsic_governance_handoff) {
  std::ostringstream out;
  out << "inline-asm-intrinsic-governance:inline_asm_intrinsic_sites="
      << inline_asm_intrinsic_sites
      << ";inline_asm_sites=" << inline_asm_sites
      << ";intrinsic_sites=" << intrinsic_sites
      << ";governed_intrinsic_sites=" << governed_intrinsic_sites
      << ";privileged_intrinsic_sites=" << privileged_intrinsic_sites
      << ";normalized_sites=" << normalized_sites
      << ";gate_blocked_sites=" << gate_blocked_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_inline_asm_intrinsic_governance_handoff="
      << (deterministic_inline_asm_intrinsic_governance_handoff ? "true" : "false");
  return out.str();
}

static bool IsInlineAsmIntrinsicGovernanceProfileNormalized(
    std::size_t inline_asm_intrinsic_sites,
    std::size_t inline_asm_sites,
    std::size_t intrinsic_sites,
    std::size_t governed_intrinsic_sites,
    std::size_t privileged_intrinsic_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites) {
  if (inline_asm_sites > inline_asm_intrinsic_sites ||
      intrinsic_sites > inline_asm_intrinsic_sites ||
      governed_intrinsic_sites > intrinsic_sites ||
      privileged_intrinsic_sites > governed_intrinsic_sites ||
      normalized_sites > inline_asm_intrinsic_sites ||
      gate_blocked_sites > inline_asm_intrinsic_sites ||
      contract_violation_sites > inline_asm_intrinsic_sites) {
    return false;
  }
  if (normalized_sites + gate_blocked_sites != inline_asm_intrinsic_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}

static Objc3InlineAsmIntrinsicGovernanceProfile BuildInlineAsmIntrinsicGovernanceProfileFromCounts(
    std::size_t inline_asm_sites,
    std::size_t intrinsic_sites,
    std::size_t governed_intrinsic_sites,
    std::size_t privileged_intrinsic_sites) {
  Objc3InlineAsmIntrinsicGovernanceProfile profile;
  profile.inline_asm_sites = inline_asm_sites;
  profile.intrinsic_sites = intrinsic_sites;
  profile.governed_intrinsic_sites = governed_intrinsic_sites;
  profile.privileged_intrinsic_sites = privileged_intrinsic_sites;
  profile.inline_asm_intrinsic_sites = profile.inline_asm_sites + profile.intrinsic_sites;
  profile.gate_blocked_sites = profile.privileged_intrinsic_sites;
  if (profile.gate_blocked_sites > profile.inline_asm_intrinsic_sites) {
    profile.normalized_sites = 0u;
  } else {
    profile.normalized_sites =
        profile.inline_asm_intrinsic_sites - profile.gate_blocked_sites;
  }

  if (profile.inline_asm_sites > profile.inline_asm_intrinsic_sites ||
      profile.intrinsic_sites > profile.inline_asm_intrinsic_sites ||
      profile.governed_intrinsic_sites > profile.intrinsic_sites ||
      profile.privileged_intrinsic_sites > profile.governed_intrinsic_sites ||
      profile.normalized_sites > profile.inline_asm_intrinsic_sites ||
      profile.gate_blocked_sites > profile.inline_asm_intrinsic_sites ||
      profile.contract_violation_sites > profile.inline_asm_intrinsic_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.gate_blocked_sites != profile.inline_asm_intrinsic_sites) {
    profile.contract_violation_sites += 1u;
  }

  profile.deterministic_inline_asm_intrinsic_governance_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

static Objc3InlineAsmIntrinsicGovernanceProfile BuildInlineAsmIntrinsicGovernanceProfileFromFunction(
    const FunctionDecl &fn) {
  const Objc3InlineAsmIntrinsicSiteCounts counts = CountInlineAsmIntrinsicSitesInBody(fn.body);
  return BuildInlineAsmIntrinsicGovernanceProfileFromCounts(
      counts.inline_asm_sites,
      counts.intrinsic_sites,
      counts.governed_intrinsic_sites,
      counts.privileged_intrinsic_sites);
}

static Objc3InlineAsmIntrinsicGovernanceProfile BuildInlineAsmIntrinsicGovernanceProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3InlineAsmIntrinsicSiteCounts counts;
  if (method.has_body) {
    CollectInlineAsmIntrinsicSitesFromSymbol(method.selector, counts);
  }
  return BuildInlineAsmIntrinsicGovernanceProfileFromCounts(
      counts.inline_asm_sites,
      counts.intrinsic_sites,
      counts.governed_intrinsic_sites,
      counts.privileged_intrinsic_sites);
}

static bool IsAsyncKeywordSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered == "async" || lowered.find("async_") != std::string::npos;
}

static bool IsAsyncFunctionSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("async_fn") != std::string::npos ||
         lowered.find("future") != std::string::npos ||
         lowered.find("task") != std::string::npos;
}

static bool IsContinuationAllocationSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("continuation_alloc") != std::string::npos ||
         lowered.find("make_continuation") != std::string::npos ||
         lowered.find("continuation_new") != std::string::npos;
}

static bool IsContinuationResumeSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("continuation_resume") != std::string::npos ||
         lowered.find("resume_continuation") != std::string::npos ||
         lowered.find("resume") != std::string::npos;
}

static bool IsContinuationSuspendSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("continuation_suspend") != std::string::npos ||
         lowered.find("suspend_continuation") != std::string::npos ||
         lowered.find("suspend") != std::string::npos;
}

static bool IsAsyncStateMachineSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("state_machine") != std::string::npos ||
         lowered.find("poll") != std::string::npos ||
         lowered.find("waker") != std::string::npos;
}

struct Objc3AsyncContinuationSiteCounts {
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t continuation_allocation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
};

static void CollectAsyncContinuationSitesFromSymbol(
    const std::string &symbol,
    Objc3AsyncContinuationSiteCounts &counts) {
  if (IsAsyncKeywordSymbol(symbol)) {
    counts.async_keyword_sites += 1u;
  }
  if (IsAsyncFunctionSymbol(symbol)) {
    counts.async_function_sites += 1u;
  }
  if (IsContinuationAllocationSymbol(symbol)) {
    counts.continuation_allocation_sites += 1u;
  }
  if (IsContinuationResumeSymbol(symbol)) {
    counts.continuation_resume_sites += 1u;
  }
  if (IsContinuationSuspendSymbol(symbol)) {
    counts.continuation_suspend_sites += 1u;
  }
  if (IsAsyncStateMachineSymbol(symbol)) {
    counts.async_state_machine_sites += 1u;
  }
}

static void CollectAsyncContinuationExprSites(
    const Expr *expr,
    Objc3AsyncContinuationSiteCounts &counts) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::Call:
    CollectAsyncContinuationSitesFromSymbol(expr->ident, counts);
    for (const auto &arg : expr->args) {
      CollectAsyncContinuationExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectAsyncContinuationSitesFromSymbol(expr->selector, counts);
    CollectAsyncContinuationExprSites(expr->receiver.get(), counts);
    for (const auto &arg : expr->args) {
      CollectAsyncContinuationExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::Binary:
    CollectAsyncContinuationExprSites(expr->left.get(), counts);
    CollectAsyncContinuationExprSites(expr->right.get(), counts);
    return;
  case Expr::Kind::Conditional:
    CollectAsyncContinuationExprSites(expr->left.get(), counts);
    CollectAsyncContinuationExprSites(expr->right.get(), counts);
    CollectAsyncContinuationExprSites(expr->third.get(), counts);
    return;
  case Expr::Kind::BlockLiteral:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::NilLiteral:
  case Expr::Kind::Number:
  default:
    return;
  }
}

static void CollectAsyncContinuationForClauseSites(
    const ForClause &clause,
    Objc3AsyncContinuationSiteCounts &counts) {
  CollectAsyncContinuationExprSites(clause.value.get(), counts);
}

static void CollectAsyncContinuationStmtSites(
    const Stmt *stmt,
    Objc3AsyncContinuationSiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectAsyncContinuationExprSites(stmt->let_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectAsyncContinuationExprSites(stmt->assign_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectAsyncContinuationExprSites(stmt->return_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    CollectAsyncContinuationExprSites(stmt->if_stmt->condition.get(), counts);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      CollectAsyncContinuationStmtSites(then_stmt.get(), counts);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      CollectAsyncContinuationStmtSites(else_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      CollectAsyncContinuationStmtSites(body_stmt.get(), counts);
    }
    CollectAsyncContinuationExprSites(stmt->do_while_stmt->condition.get(), counts);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    CollectAsyncContinuationForClauseSites(stmt->for_stmt->init, counts);
    CollectAsyncContinuationExprSites(stmt->for_stmt->condition.get(), counts);
    CollectAsyncContinuationForClauseSites(stmt->for_stmt->step, counts);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      CollectAsyncContinuationStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    CollectAsyncContinuationExprSites(stmt->switch_stmt->condition.get(), counts);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        CollectAsyncContinuationStmtSites(case_stmt.get(), counts);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    CollectAsyncContinuationExprSites(stmt->while_stmt->condition.get(), counts);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      CollectAsyncContinuationStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Block:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      CollectAsyncContinuationStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectAsyncContinuationExprSites(stmt->expr_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

static Objc3AsyncContinuationSiteCounts CountAsyncContinuationSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3AsyncContinuationSiteCounts counts;
  for (const auto &stmt : body) {
    CollectAsyncContinuationStmtSites(stmt.get(), counts);
  }
  return counts;
}

struct Objc3AsyncContinuationProfile {
  std::size_t async_continuation_sites = 0;
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t continuation_allocation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic_async_continuation_handoff = false;
};

static std::string BuildAsyncContinuationProfile(
    std::size_t async_continuation_sites,
    std::size_t async_keyword_sites,
    std::size_t async_function_sites,
    std::size_t continuation_allocation_sites,
    std::size_t continuation_resume_sites,
    std::size_t continuation_suspend_sites,
    std::size_t async_state_machine_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_async_continuation_handoff) {
  std::ostringstream out;
  out << "async-continuation:async_continuation_sites=" << async_continuation_sites
      << ";async_keyword_sites=" << async_keyword_sites
      << ";async_function_sites=" << async_function_sites
      << ";continuation_allocation_sites=" << continuation_allocation_sites
      << ";continuation_resume_sites=" << continuation_resume_sites
      << ";continuation_suspend_sites=" << continuation_suspend_sites
      << ";async_state_machine_sites=" << async_state_machine_sites
      << ";normalized_sites=" << normalized_sites
      << ";gate_blocked_sites=" << gate_blocked_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_async_continuation_handoff="
      << (deterministic_async_continuation_handoff ? "true" : "false");
  return out.str();
}

static bool IsAsyncContinuationProfileNormalized(
    std::size_t async_continuation_sites,
    std::size_t async_keyword_sites,
    std::size_t async_function_sites,
    std::size_t continuation_allocation_sites,
    std::size_t continuation_resume_sites,
    std::size_t continuation_suspend_sites,
    std::size_t async_state_machine_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites) {
  if (async_keyword_sites > async_continuation_sites ||
      async_function_sites > async_continuation_sites ||
      continuation_allocation_sites > async_continuation_sites ||
      continuation_resume_sites > async_continuation_sites ||
      continuation_suspend_sites > async_continuation_sites ||
      async_state_machine_sites > async_continuation_sites ||
      normalized_sites > async_continuation_sites ||
      gate_blocked_sites > async_continuation_sites ||
      contract_violation_sites > async_continuation_sites) {
    return false;
  }
  if (normalized_sites + gate_blocked_sites != async_continuation_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}

static Objc3AsyncContinuationProfile BuildAsyncContinuationProfileFromCounts(
    std::size_t async_keyword_sites,
    std::size_t async_function_sites,
    std::size_t continuation_allocation_sites,
    std::size_t continuation_resume_sites,
    std::size_t continuation_suspend_sites,
    std::size_t async_state_machine_sites) {
  Objc3AsyncContinuationProfile profile;
  profile.async_keyword_sites = async_keyword_sites;
  profile.async_function_sites = async_function_sites;
  profile.continuation_allocation_sites = continuation_allocation_sites;
  profile.continuation_resume_sites = continuation_resume_sites;
  profile.continuation_suspend_sites = continuation_suspend_sites;
  profile.async_state_machine_sites = async_state_machine_sites;
  profile.async_continuation_sites = profile.async_keyword_sites;
  if (profile.async_continuation_sites >
      std::numeric_limits<std::size_t>::max() - profile.async_function_sites) {
    profile.async_continuation_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.async_continuation_sites += profile.async_function_sites;
  }
  if (profile.async_continuation_sites >
      std::numeric_limits<std::size_t>::max() - profile.continuation_allocation_sites) {
    profile.async_continuation_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.async_continuation_sites += profile.continuation_allocation_sites;
  }
  profile.gate_blocked_sites =
      std::min(profile.async_continuation_sites,
               std::min(profile.continuation_resume_sites, profile.continuation_suspend_sites));
  profile.normalized_sites = profile.async_continuation_sites - profile.gate_blocked_sites;
  if (profile.async_keyword_sites > profile.async_continuation_sites ||
      profile.async_function_sites > profile.async_continuation_sites ||
      profile.continuation_allocation_sites > profile.async_continuation_sites ||
      profile.continuation_resume_sites > profile.async_continuation_sites ||
      profile.continuation_suspend_sites > profile.async_continuation_sites ||
      profile.async_state_machine_sites > profile.async_continuation_sites ||
      profile.normalized_sites > profile.async_continuation_sites ||
      profile.gate_blocked_sites > profile.async_continuation_sites ||
      profile.contract_violation_sites > profile.async_continuation_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.gate_blocked_sites != profile.async_continuation_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.contract_violation_sites > profile.async_continuation_sites) {
    profile.contract_violation_sites = profile.async_continuation_sites;
  }
  profile.deterministic_async_continuation_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

static Objc3AsyncContinuationProfile BuildAsyncContinuationProfileFromFunction(
    const FunctionDecl &fn) {
  const Objc3AsyncContinuationSiteCounts counts =
      CountAsyncContinuationSitesInBody(fn.body);
  return BuildAsyncContinuationProfileFromCounts(
      counts.async_keyword_sites,
      counts.async_function_sites,
      counts.continuation_allocation_sites,
      counts.continuation_resume_sites,
      counts.continuation_suspend_sites,
      counts.async_state_machine_sites);
}

static Objc3AsyncContinuationProfile BuildAsyncContinuationProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3AsyncContinuationSiteCounts counts;
  if (method.has_body) {
    CollectAsyncContinuationSitesFromSymbol(method.selector, counts);
  }
  return BuildAsyncContinuationProfileFromCounts(
      counts.async_keyword_sites,
      counts.async_function_sites,
      counts.continuation_allocation_sites,
      counts.continuation_resume_sites,
      counts.continuation_suspend_sites,
      counts.async_state_machine_sites);
}

static bool IsAwaitKeywordSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered == "await" || lowered.find("await_") != std::string::npos;
}

static bool IsAwaitSuspensionPointSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("suspend") != std::string::npos ||
         lowered.find("yield") != std::string::npos;
}

static bool IsAwaitResumeSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("resume") != std::string::npos ||
         lowered.find("wakeup") != std::string::npos;
}

static bool IsAwaitStateMachineSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("state") != std::string::npos ||
         lowered.find("continuation") != std::string::npos ||
         lowered.find("poll") != std::string::npos;
}

static bool IsAwaitContinuationSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("continuation") != std::string::npos ||
         lowered.find("future") != std::string::npos ||
         lowered.find("promise") != std::string::npos;
}

struct Objc3AwaitSuspensionSiteCounts {
  std::size_t await_keyword_sites = 0;
  std::size_t await_suspension_point_sites = 0;
  std::size_t await_resume_sites = 0;
  std::size_t await_state_machine_sites = 0;
  std::size_t await_continuation_sites = 0;
};

static void CollectAwaitSuspensionSitesFromSymbol(
    const std::string &symbol,
    Objc3AwaitSuspensionSiteCounts &counts) {
  if (IsAwaitKeywordSymbol(symbol)) {
    counts.await_keyword_sites += 1u;
  }
  if (IsAwaitSuspensionPointSymbol(symbol)) {
    counts.await_suspension_point_sites += 1u;
  }
  if (IsAwaitResumeSymbol(symbol)) {
    counts.await_resume_sites += 1u;
  }
  if (IsAwaitStateMachineSymbol(symbol)) {
    counts.await_state_machine_sites += 1u;
  }
  if (IsAwaitContinuationSymbol(symbol)) {
    counts.await_continuation_sites += 1u;
  }
}

static void CollectAwaitSuspensionExprSites(
    const Expr *expr,
    Objc3AwaitSuspensionSiteCounts &counts) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::Call:
    CollectAwaitSuspensionSitesFromSymbol(expr->ident, counts);
    for (const auto &arg : expr->args) {
      CollectAwaitSuspensionExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectAwaitSuspensionSitesFromSymbol(expr->selector, counts);
    CollectAwaitSuspensionExprSites(expr->receiver.get(), counts);
    for (const auto &arg : expr->args) {
      CollectAwaitSuspensionExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::Binary:
    CollectAwaitSuspensionExprSites(expr->left.get(), counts);
    CollectAwaitSuspensionExprSites(expr->right.get(), counts);
    return;
  case Expr::Kind::Conditional:
    CollectAwaitSuspensionExprSites(expr->left.get(), counts);
    CollectAwaitSuspensionExprSites(expr->right.get(), counts);
    CollectAwaitSuspensionExprSites(expr->third.get(), counts);
    return;
  case Expr::Kind::BlockLiteral:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::NilLiteral:
  case Expr::Kind::Number:
  default:
    return;
  }
}

static void CollectAwaitSuspensionForClauseSites(
    const ForClause &clause,
    Objc3AwaitSuspensionSiteCounts &counts) {
  CollectAwaitSuspensionExprSites(clause.value.get(), counts);
}

static void CollectAwaitSuspensionStmtSites(
    const Stmt *stmt,
    Objc3AwaitSuspensionSiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectAwaitSuspensionExprSites(stmt->let_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectAwaitSuspensionExprSites(stmt->assign_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectAwaitSuspensionExprSites(stmt->return_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    CollectAwaitSuspensionExprSites(stmt->if_stmt->condition.get(), counts);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      CollectAwaitSuspensionStmtSites(then_stmt.get(), counts);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      CollectAwaitSuspensionStmtSites(else_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      CollectAwaitSuspensionStmtSites(body_stmt.get(), counts);
    }
    CollectAwaitSuspensionExprSites(stmt->do_while_stmt->condition.get(), counts);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    CollectAwaitSuspensionForClauseSites(stmt->for_stmt->init, counts);
    CollectAwaitSuspensionExprSites(stmt->for_stmt->condition.get(), counts);
    CollectAwaitSuspensionForClauseSites(stmt->for_stmt->step, counts);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      CollectAwaitSuspensionStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    CollectAwaitSuspensionExprSites(stmt->switch_stmt->condition.get(), counts);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        CollectAwaitSuspensionStmtSites(case_stmt.get(), counts);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    CollectAwaitSuspensionExprSites(stmt->while_stmt->condition.get(), counts);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      CollectAwaitSuspensionStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Block:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      CollectAwaitSuspensionStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectAwaitSuspensionExprSites(stmt->expr_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

static Objc3AwaitSuspensionSiteCounts CountAwaitSuspensionSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3AwaitSuspensionSiteCounts counts;
  for (const auto &stmt : body) {
    CollectAwaitSuspensionStmtSites(stmt.get(), counts);
  }
  return counts;
}

struct Objc3AwaitSuspensionProfile {
  std::size_t await_suspension_sites = 0;
  std::size_t await_keyword_sites = 0;
  std::size_t await_suspension_point_sites = 0;
  std::size_t await_resume_sites = 0;
  std::size_t await_state_machine_sites = 0;
  std::size_t await_continuation_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic_await_suspension_handoff = false;
};

static std::string BuildAwaitSuspensionProfile(
    std::size_t await_suspension_sites,
    std::size_t await_keyword_sites,
    std::size_t await_suspension_point_sites,
    std::size_t await_resume_sites,
    std::size_t await_state_machine_sites,
    std::size_t await_continuation_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_await_suspension_handoff) {
  std::ostringstream out;
  out << "await-suspension:await_suspension_sites=" << await_suspension_sites
      << ";await_keyword_sites=" << await_keyword_sites
      << ";await_suspension_point_sites=" << await_suspension_point_sites
      << ";await_resume_sites=" << await_resume_sites
      << ";await_state_machine_sites=" << await_state_machine_sites
      << ";await_continuation_sites=" << await_continuation_sites
      << ";normalized_sites=" << normalized_sites
      << ";gate_blocked_sites=" << gate_blocked_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_await_suspension_handoff="
      << (deterministic_await_suspension_handoff ? "true" : "false");
  return out.str();
}

static bool IsAwaitSuspensionProfileNormalized(
    std::size_t await_suspension_sites,
    std::size_t await_keyword_sites,
    std::size_t await_suspension_point_sites,
    std::size_t await_resume_sites,
    std::size_t await_state_machine_sites,
    std::size_t await_continuation_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites) {
  if (await_keyword_sites > await_suspension_sites ||
      await_suspension_point_sites > await_suspension_sites ||
      await_resume_sites > await_suspension_sites ||
      await_state_machine_sites > await_suspension_sites ||
      await_continuation_sites > await_suspension_sites ||
      normalized_sites > await_suspension_sites ||
      gate_blocked_sites > await_suspension_sites ||
      contract_violation_sites > await_suspension_sites) {
    return false;
  }
  if (normalized_sites + gate_blocked_sites != await_suspension_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}

static Objc3AwaitSuspensionProfile BuildAwaitSuspensionProfileFromCounts(
    std::size_t await_keyword_sites,
    std::size_t await_suspension_point_sites,
    std::size_t await_resume_sites,
    std::size_t await_state_machine_sites,
    std::size_t await_continuation_sites) {
  Objc3AwaitSuspensionProfile profile;
  profile.await_keyword_sites = await_keyword_sites;
  profile.await_suspension_point_sites = await_suspension_point_sites;
  profile.await_resume_sites = await_resume_sites;
  profile.await_state_machine_sites = await_state_machine_sites;
  profile.await_continuation_sites = await_continuation_sites;
  profile.await_suspension_sites = profile.await_keyword_sites;
  if (profile.await_suspension_sites >
      std::numeric_limits<std::size_t>::max() - profile.await_suspension_point_sites) {
    profile.await_suspension_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.await_suspension_sites += profile.await_suspension_point_sites;
  }
  if (profile.await_suspension_sites >
      std::numeric_limits<std::size_t>::max() - profile.await_continuation_sites) {
    profile.await_suspension_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.await_suspension_sites += profile.await_continuation_sites;
  }
  profile.gate_blocked_sites =
      std::min(profile.await_suspension_sites,
               std::min(profile.await_resume_sites, profile.await_state_machine_sites));
  profile.normalized_sites = profile.await_suspension_sites - profile.gate_blocked_sites;
  if (profile.await_keyword_sites > profile.await_suspension_sites ||
      profile.await_suspension_point_sites > profile.await_suspension_sites ||
      profile.await_resume_sites > profile.await_suspension_sites ||
      profile.await_state_machine_sites > profile.await_suspension_sites ||
      profile.await_continuation_sites > profile.await_suspension_sites ||
      profile.normalized_sites > profile.await_suspension_sites ||
      profile.gate_blocked_sites > profile.await_suspension_sites ||
      profile.contract_violation_sites > profile.await_suspension_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.gate_blocked_sites != profile.await_suspension_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.contract_violation_sites > profile.await_suspension_sites) {
    profile.contract_violation_sites = profile.await_suspension_sites;
  }
  profile.deterministic_await_suspension_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

static Objc3AwaitSuspensionProfile BuildAwaitSuspensionProfileFromFunction(
    const FunctionDecl &fn) {
  const Objc3AwaitSuspensionSiteCounts counts = CountAwaitSuspensionSitesInBody(fn.body);
  return BuildAwaitSuspensionProfileFromCounts(
      counts.await_keyword_sites,
      counts.await_suspension_point_sites,
      counts.await_resume_sites,
      counts.await_state_machine_sites,
      counts.await_continuation_sites);
}

static Objc3AwaitSuspensionProfile BuildAwaitSuspensionProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3AwaitSuspensionSiteCounts counts;
  if (method.has_body) {
    CollectAwaitSuspensionSitesFromSymbol(method.selector, counts);
  }
  return BuildAwaitSuspensionProfileFromCounts(
      counts.await_keyword_sites,
      counts.await_suspension_point_sites,
      counts.await_resume_sites,
      counts.await_state_machine_sites,
      counts.await_continuation_sites);
}

static bool IsActorIsolationDeclSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("actor") != std::string::npos ||
         lowered.find("isolated") != std::string::npos ||
         lowered.find("isolation") != std::string::npos;
}

static bool IsActorHopSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("hop_to") != std::string::npos ||
         lowered.find("enqueue") != std::string::npos ||
         lowered.find("executor") != std::string::npos;
}

static bool IsSendableAnnotationSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("sendable") != std::string::npos ||
         lowered.find("sendability") != std::string::npos;
}

static bool IsNonSendableCrossingSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("non_sendable") != std::string::npos ||
         lowered.find("unsafe_sendable") != std::string::npos ||
         lowered.find("cross_actor") != std::string::npos;
}

struct Objc3ActorIsolationSendabilitySiteCounts {
  std::size_t actor_isolation_decl_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendable_annotation_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
};

static void CollectActorIsolationSendabilitySitesFromSymbol(
    const std::string &symbol,
    Objc3ActorIsolationSendabilitySiteCounts &counts) {
  if (IsActorIsolationDeclSymbol(symbol)) {
    counts.actor_isolation_decl_sites += 1u;
  }
  if (IsActorHopSymbol(symbol)) {
    counts.actor_hop_sites += 1u;
  }
  if (IsSendableAnnotationSymbol(symbol)) {
    counts.sendable_annotation_sites += 1u;
  }
  if (IsNonSendableCrossingSymbol(symbol)) {
    counts.non_sendable_crossing_sites += 1u;
  }
}

static void CollectActorIsolationSendabilityExprSites(
    const Expr *expr,
    Objc3ActorIsolationSendabilitySiteCounts &counts) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::Call:
    CollectActorIsolationSendabilitySitesFromSymbol(expr->ident, counts);
    for (const auto &arg : expr->args) {
      CollectActorIsolationSendabilityExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectActorIsolationSendabilitySitesFromSymbol(expr->selector, counts);
    CollectActorIsolationSendabilityExprSites(expr->receiver.get(), counts);
    for (const auto &arg : expr->args) {
      CollectActorIsolationSendabilityExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::Binary:
    CollectActorIsolationSendabilityExprSites(expr->left.get(), counts);
    CollectActorIsolationSendabilityExprSites(expr->right.get(), counts);
    return;
  case Expr::Kind::Conditional:
    CollectActorIsolationSendabilityExprSites(expr->left.get(), counts);
    CollectActorIsolationSendabilityExprSites(expr->right.get(), counts);
    CollectActorIsolationSendabilityExprSites(expr->third.get(), counts);
    return;
  case Expr::Kind::BlockLiteral:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::NilLiteral:
  case Expr::Kind::Number:
  default:
    return;
  }
}

static void CollectActorIsolationSendabilityForClauseSites(
    const ForClause &clause,
    Objc3ActorIsolationSendabilitySiteCounts &counts) {
  CollectActorIsolationSendabilityExprSites(clause.value.get(), counts);
}

static void CollectActorIsolationSendabilityStmtSites(
    const Stmt *stmt,
    Objc3ActorIsolationSendabilitySiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectActorIsolationSendabilityExprSites(stmt->let_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectActorIsolationSendabilityExprSites(stmt->assign_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectActorIsolationSendabilityExprSites(stmt->return_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    CollectActorIsolationSendabilityExprSites(stmt->if_stmt->condition.get(), counts);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      CollectActorIsolationSendabilityStmtSites(then_stmt.get(), counts);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      CollectActorIsolationSendabilityStmtSites(else_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      CollectActorIsolationSendabilityStmtSites(body_stmt.get(), counts);
    }
    CollectActorIsolationSendabilityExprSites(
        stmt->do_while_stmt->condition.get(),
        counts);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    CollectActorIsolationSendabilityForClauseSites(stmt->for_stmt->init, counts);
    CollectActorIsolationSendabilityExprSites(stmt->for_stmt->condition.get(), counts);
    CollectActorIsolationSendabilityForClauseSites(stmt->for_stmt->step, counts);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      CollectActorIsolationSendabilityStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    CollectActorIsolationSendabilityExprSites(stmt->switch_stmt->condition.get(), counts);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        CollectActorIsolationSendabilityStmtSites(case_stmt.get(), counts);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    CollectActorIsolationSendabilityExprSites(stmt->while_stmt->condition.get(), counts);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      CollectActorIsolationSendabilityStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Block:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      CollectActorIsolationSendabilityStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectActorIsolationSendabilityExprSites(stmt->expr_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

static Objc3ActorIsolationSendabilitySiteCounts CountActorIsolationSendabilitySitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3ActorIsolationSendabilitySiteCounts counts;
  for (const auto &stmt : body) {
    CollectActorIsolationSendabilityStmtSites(stmt.get(), counts);
  }
  return counts;
}

struct Objc3ActorIsolationSendabilityProfile {
  std::size_t actor_isolation_sendability_sites = 0;
  std::size_t actor_isolation_decl_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendable_annotation_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic_actor_isolation_sendability_handoff = false;
};

static std::string BuildActorIsolationSendabilityProfile(
    std::size_t actor_isolation_sendability_sites,
    std::size_t actor_isolation_decl_sites,
    std::size_t actor_hop_sites,
    std::size_t sendable_annotation_sites,
    std::size_t non_sendable_crossing_sites,
    std::size_t isolation_boundary_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_actor_isolation_sendability_handoff) {
  std::ostringstream out;
  out << "actor-isolation-sendability:actor_isolation_sendability_sites="
      << actor_isolation_sendability_sites
      << ";actor_isolation_decl_sites=" << actor_isolation_decl_sites
      << ";actor_hop_sites=" << actor_hop_sites
      << ";sendable_annotation_sites=" << sendable_annotation_sites
      << ";non_sendable_crossing_sites=" << non_sendable_crossing_sites
      << ";isolation_boundary_sites=" << isolation_boundary_sites
      << ";normalized_sites=" << normalized_sites
      << ";gate_blocked_sites=" << gate_blocked_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_actor_isolation_sendability_handoff="
      << (deterministic_actor_isolation_sendability_handoff ? "true" : "false");
  return out.str();
}

static bool IsActorIsolationSendabilityProfileNormalized(
    std::size_t actor_isolation_sendability_sites,
    std::size_t actor_isolation_decl_sites,
    std::size_t actor_hop_sites,
    std::size_t sendable_annotation_sites,
    std::size_t non_sendable_crossing_sites,
    std::size_t isolation_boundary_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites) {
  if (actor_isolation_decl_sites > actor_isolation_sendability_sites ||
      actor_hop_sites > actor_isolation_sendability_sites ||
      sendable_annotation_sites > actor_isolation_sendability_sites ||
      non_sendable_crossing_sites > actor_isolation_sendability_sites ||
      isolation_boundary_sites > actor_isolation_sendability_sites ||
      normalized_sites > actor_isolation_sendability_sites ||
      gate_blocked_sites > actor_isolation_sendability_sites ||
      contract_violation_sites > actor_isolation_sendability_sites) {
    return false;
  }
  if (normalized_sites + gate_blocked_sites != actor_isolation_sendability_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}

static Objc3ActorIsolationSendabilityProfile BuildActorIsolationSendabilityProfileFromCounts(
    std::size_t actor_isolation_decl_sites,
    std::size_t actor_hop_sites,
    std::size_t sendable_annotation_sites,
    std::size_t non_sendable_crossing_sites) {
  Objc3ActorIsolationSendabilityProfile profile;
  profile.actor_isolation_decl_sites = actor_isolation_decl_sites;
  profile.actor_hop_sites = actor_hop_sites;
  profile.sendable_annotation_sites = sendable_annotation_sites;
  profile.non_sendable_crossing_sites = non_sendable_crossing_sites;
  profile.actor_isolation_sendability_sites = profile.actor_isolation_decl_sites;
  if (profile.actor_isolation_sendability_sites >
      std::numeric_limits<std::size_t>::max() - profile.actor_hop_sites) {
    profile.actor_isolation_sendability_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.actor_isolation_sendability_sites += profile.actor_hop_sites;
  }
  if (profile.actor_isolation_sendability_sites >
      std::numeric_limits<std::size_t>::max() - profile.sendable_annotation_sites) {
    profile.actor_isolation_sendability_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.actor_isolation_sendability_sites += profile.sendable_annotation_sites;
  }
  profile.isolation_boundary_sites =
      std::min(profile.actor_isolation_decl_sites, profile.actor_hop_sites);
  profile.gate_blocked_sites =
      std::min(profile.actor_isolation_sendability_sites, profile.non_sendable_crossing_sites);
  profile.normalized_sites =
      profile.actor_isolation_sendability_sites - profile.gate_blocked_sites;
  if (profile.actor_isolation_decl_sites > profile.actor_isolation_sendability_sites ||
      profile.actor_hop_sites > profile.actor_isolation_sendability_sites ||
      profile.sendable_annotation_sites > profile.actor_isolation_sendability_sites ||
      profile.non_sendable_crossing_sites > profile.actor_isolation_sendability_sites ||
      profile.isolation_boundary_sites > profile.actor_isolation_sendability_sites ||
      profile.normalized_sites > profile.actor_isolation_sendability_sites ||
      profile.gate_blocked_sites > profile.actor_isolation_sendability_sites ||
      profile.contract_violation_sites > profile.actor_isolation_sendability_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.gate_blocked_sites !=
      profile.actor_isolation_sendability_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.contract_violation_sites > profile.actor_isolation_sendability_sites) {
    profile.contract_violation_sites = profile.actor_isolation_sendability_sites;
  }
  profile.deterministic_actor_isolation_sendability_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

static Objc3ActorIsolationSendabilityProfile BuildActorIsolationSendabilityProfileFromFunction(
    const FunctionDecl &fn) {
  const Objc3ActorIsolationSendabilitySiteCounts counts =
      CountActorIsolationSendabilitySitesInBody(fn.body);
  return BuildActorIsolationSendabilityProfileFromCounts(
      counts.actor_isolation_decl_sites,
      counts.actor_hop_sites,
      counts.sendable_annotation_sites,
      counts.non_sendable_crossing_sites);
}

static Objc3ActorIsolationSendabilityProfile BuildActorIsolationSendabilityProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3ActorIsolationSendabilitySiteCounts counts;
  if (method.has_body) {
    CollectActorIsolationSendabilitySitesFromSymbol(method.selector, counts);
  }
  return BuildActorIsolationSendabilityProfileFromCounts(
      counts.actor_isolation_decl_sites,
      counts.actor_hop_sites,
      counts.sendable_annotation_sites,
      counts.non_sendable_crossing_sites);
}

static bool IsTaskRuntimeHookSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("task_runtime") != std::string::npos ||
         lowered.find("runtime_task") != std::string::npos ||
         lowered.find("executor") != std::string::npos;
}

static bool IsCancellationCheckSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("cancel") != std::string::npos ||
         lowered.find("is_cancelled") != std::string::npos ||
         lowered.find("cancellation") != std::string::npos;
}

static bool IsCancellationHandlerSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("cancel_handler") != std::string::npos ||
         lowered.find("with_cancellation_handler") != std::string::npos ||
         lowered.find("on_cancel") != std::string::npos;
}

static bool IsSuspensionPointSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("await") != std::string::npos ||
         lowered.find("suspend") != std::string::npos ||
         lowered.find("yield") != std::string::npos;
}

struct Objc3TaskRuntimeCancellationSiteCounts {
  std::size_t runtime_hook_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t suspension_point_sites = 0;
};

static void CollectTaskRuntimeCancellationSitesFromSymbol(
    const std::string &symbol,
    Objc3TaskRuntimeCancellationSiteCounts &counts) {
  if (IsTaskRuntimeHookSymbol(symbol)) {
    counts.runtime_hook_sites += 1u;
  }
  if (IsCancellationCheckSymbol(symbol)) {
    counts.cancellation_check_sites += 1u;
  }
  if (IsCancellationHandlerSymbol(symbol)) {
    counts.cancellation_handler_sites += 1u;
  }
  if (IsSuspensionPointSymbol(symbol)) {
    counts.suspension_point_sites += 1u;
  }
}

static void CollectTaskRuntimeCancellationExprSites(
    const Expr *expr,
    Objc3TaskRuntimeCancellationSiteCounts &counts) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::Call:
    CollectTaskRuntimeCancellationSitesFromSymbol(expr->ident, counts);
    for (const auto &arg : expr->args) {
      CollectTaskRuntimeCancellationExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectTaskRuntimeCancellationSitesFromSymbol(expr->selector, counts);
    CollectTaskRuntimeCancellationExprSites(expr->receiver.get(), counts);
    for (const auto &arg : expr->args) {
      CollectTaskRuntimeCancellationExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::Binary:
    CollectTaskRuntimeCancellationExprSites(expr->left.get(), counts);
    CollectTaskRuntimeCancellationExprSites(expr->right.get(), counts);
    return;
  case Expr::Kind::Conditional:
    CollectTaskRuntimeCancellationExprSites(expr->left.get(), counts);
    CollectTaskRuntimeCancellationExprSites(expr->right.get(), counts);
    CollectTaskRuntimeCancellationExprSites(expr->third.get(), counts);
    return;
  case Expr::Kind::BlockLiteral:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::NilLiteral:
  case Expr::Kind::Number:
  default:
    return;
  }
}

static void CollectTaskRuntimeCancellationForClauseSites(
    const ForClause &clause,
    Objc3TaskRuntimeCancellationSiteCounts &counts) {
  CollectTaskRuntimeCancellationExprSites(clause.value.get(), counts);
}

static void CollectTaskRuntimeCancellationStmtSites(
    const Stmt *stmt,
    Objc3TaskRuntimeCancellationSiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectTaskRuntimeCancellationExprSites(stmt->let_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectTaskRuntimeCancellationExprSites(stmt->assign_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectTaskRuntimeCancellationExprSites(stmt->return_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    CollectTaskRuntimeCancellationExprSites(stmt->if_stmt->condition.get(), counts);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      CollectTaskRuntimeCancellationStmtSites(then_stmt.get(), counts);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      CollectTaskRuntimeCancellationStmtSites(else_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      CollectTaskRuntimeCancellationStmtSites(body_stmt.get(), counts);
    }
    CollectTaskRuntimeCancellationExprSites(
        stmt->do_while_stmt->condition.get(),
        counts);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    CollectTaskRuntimeCancellationForClauseSites(stmt->for_stmt->init, counts);
    CollectTaskRuntimeCancellationExprSites(stmt->for_stmt->condition.get(), counts);
    CollectTaskRuntimeCancellationForClauseSites(stmt->for_stmt->step, counts);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      CollectTaskRuntimeCancellationStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    CollectTaskRuntimeCancellationExprSites(stmt->switch_stmt->condition.get(), counts);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        CollectTaskRuntimeCancellationStmtSites(case_stmt.get(), counts);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    CollectTaskRuntimeCancellationExprSites(stmt->while_stmt->condition.get(), counts);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      CollectTaskRuntimeCancellationStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Block:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      CollectTaskRuntimeCancellationStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectTaskRuntimeCancellationExprSites(stmt->expr_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

static Objc3TaskRuntimeCancellationSiteCounts CountTaskRuntimeCancellationSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3TaskRuntimeCancellationSiteCounts counts;
  for (const auto &stmt : body) {
    CollectTaskRuntimeCancellationStmtSites(stmt.get(), counts);
  }
  return counts;
}

struct Objc3TaskRuntimeCancellationProfile {
  std::size_t task_runtime_interop_sites = 0;
  std::size_t runtime_hook_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t suspension_point_sites = 0;
  std::size_t cancellation_propagation_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic_task_runtime_cancellation_handoff = false;
};

static std::string BuildTaskRuntimeCancellationProfile(
    std::size_t task_runtime_interop_sites,
    std::size_t runtime_hook_sites,
    std::size_t cancellation_check_sites,
    std::size_t cancellation_handler_sites,
    std::size_t suspension_point_sites,
    std::size_t cancellation_propagation_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_task_runtime_cancellation_handoff) {
  std::ostringstream out;
  out << "task-runtime-cancellation:task_runtime_interop_sites="
      << task_runtime_interop_sites
      << ";runtime_hook_sites=" << runtime_hook_sites
      << ";cancellation_check_sites=" << cancellation_check_sites
      << ";cancellation_handler_sites=" << cancellation_handler_sites
      << ";suspension_point_sites=" << suspension_point_sites
      << ";cancellation_propagation_sites=" << cancellation_propagation_sites
      << ";normalized_sites=" << normalized_sites
      << ";gate_blocked_sites=" << gate_blocked_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_task_runtime_cancellation_handoff="
      << (deterministic_task_runtime_cancellation_handoff ? "true" : "false");
  return out.str();
}

static bool IsTaskRuntimeCancellationProfileNormalized(
    std::size_t task_runtime_interop_sites,
    std::size_t runtime_hook_sites,
    std::size_t cancellation_check_sites,
    std::size_t cancellation_handler_sites,
    std::size_t suspension_point_sites,
    std::size_t cancellation_propagation_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites) {
  if (runtime_hook_sites > task_runtime_interop_sites ||
      cancellation_check_sites > task_runtime_interop_sites ||
      cancellation_handler_sites > task_runtime_interop_sites ||
      suspension_point_sites > task_runtime_interop_sites ||
      cancellation_propagation_sites > cancellation_check_sites ||
      normalized_sites > task_runtime_interop_sites ||
      gate_blocked_sites > task_runtime_interop_sites ||
      contract_violation_sites > task_runtime_interop_sites) {
    return false;
  }
  if (normalized_sites + gate_blocked_sites != task_runtime_interop_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}

static Objc3TaskRuntimeCancellationProfile BuildTaskRuntimeCancellationProfileFromCounts(
    std::size_t runtime_hook_sites,
    std::size_t cancellation_check_sites,
    std::size_t cancellation_handler_sites,
    std::size_t suspension_point_sites) {
  Objc3TaskRuntimeCancellationProfile profile;
  profile.runtime_hook_sites = runtime_hook_sites;
  profile.cancellation_check_sites = cancellation_check_sites;
  profile.cancellation_handler_sites = cancellation_handler_sites;
  profile.suspension_point_sites = suspension_point_sites;
  profile.cancellation_propagation_sites =
      std::min(profile.cancellation_handler_sites, profile.cancellation_check_sites);
  profile.task_runtime_interop_sites = profile.runtime_hook_sites;
  if (profile.task_runtime_interop_sites >
      std::numeric_limits<std::size_t>::max() - profile.cancellation_check_sites) {
    profile.task_runtime_interop_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.task_runtime_interop_sites += profile.cancellation_check_sites;
  }
  if (profile.task_runtime_interop_sites >
      std::numeric_limits<std::size_t>::max() - profile.suspension_point_sites) {
    profile.task_runtime_interop_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.task_runtime_interop_sites += profile.suspension_point_sites;
  }
  profile.gate_blocked_sites = profile.cancellation_propagation_sites;
  if (profile.gate_blocked_sites > profile.task_runtime_interop_sites) {
    profile.gate_blocked_sites = profile.task_runtime_interop_sites;
    profile.contract_violation_sites += 1u;
  }
  profile.normalized_sites =
      profile.task_runtime_interop_sites - profile.gate_blocked_sites;
  if (profile.runtime_hook_sites > profile.task_runtime_interop_sites ||
      profile.cancellation_check_sites > profile.task_runtime_interop_sites ||
      profile.cancellation_handler_sites > profile.task_runtime_interop_sites ||
      profile.suspension_point_sites > profile.task_runtime_interop_sites ||
      profile.cancellation_propagation_sites > profile.cancellation_check_sites ||
      profile.normalized_sites > profile.task_runtime_interop_sites ||
      profile.gate_blocked_sites > profile.task_runtime_interop_sites ||
      profile.contract_violation_sites > profile.task_runtime_interop_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.gate_blocked_sites !=
      profile.task_runtime_interop_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.contract_violation_sites > profile.task_runtime_interop_sites) {
    profile.contract_violation_sites = profile.task_runtime_interop_sites;
  }
  profile.deterministic_task_runtime_cancellation_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

static Objc3TaskRuntimeCancellationProfile BuildTaskRuntimeCancellationProfileFromFunction(
    const FunctionDecl &fn) {
  const Objc3TaskRuntimeCancellationSiteCounts counts =
      CountTaskRuntimeCancellationSitesInBody(fn.body);
  return BuildTaskRuntimeCancellationProfileFromCounts(
      counts.runtime_hook_sites,
      counts.cancellation_check_sites,
      counts.cancellation_handler_sites,
      counts.suspension_point_sites);
}

static Objc3TaskRuntimeCancellationProfile BuildTaskRuntimeCancellationProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3TaskRuntimeCancellationSiteCounts counts;
  if (method.has_body) {
    CollectTaskRuntimeCancellationSitesFromSymbol(method.selector, counts);
  }
  return BuildTaskRuntimeCancellationProfileFromCounts(
      counts.runtime_hook_sites,
      counts.cancellation_check_sites,
      counts.cancellation_handler_sites,
      counts.suspension_point_sites);
}

static bool IsConcurrencyReplaySymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("replay") != std::string::npos ||
         lowered.find("resume") != std::string::npos ||
         lowered.find("retry") != std::string::npos;
}

static bool IsReplayProofSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("proof") != std::string::npos ||
         lowered.find("deterministic") != std::string::npos ||
         lowered.find("stable") != std::string::npos;
}

static bool IsRaceGuardSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("race") != std::string::npos ||
         lowered.find("guard") != std::string::npos ||
         lowered.find("lock") != std::string::npos;
}

static bool IsTaskHandoffSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("handoff") != std::string::npos ||
         lowered.find("await") != std::string::npos ||
         lowered.find("task") != std::string::npos;
}

static bool IsActorIsolationSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("actor") != std::string::npos ||
         lowered.find("isolation") != std::string::npos ||
         lowered.find("isolated") != std::string::npos;
}

struct Objc3ConcurrencyReplayRaceGuardSiteCounts {
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
};

static void CollectConcurrencyReplayRaceGuardSitesFromSymbol(
    const std::string &symbol,
    Objc3ConcurrencyReplayRaceGuardSiteCounts &counts) {
  if (IsReplayProofSymbol(symbol)) {
    counts.replay_proof_sites += 1u;
  }
  if (IsRaceGuardSymbol(symbol)) {
    counts.race_guard_sites += 1u;
  }
  if (IsTaskHandoffSymbol(symbol)) {
    counts.task_handoff_sites += 1u;
  }
  if (IsActorIsolationSymbol(symbol)) {
    counts.actor_isolation_sites += 1u;
  }
  if (IsConcurrencyReplaySymbol(symbol) && counts.replay_proof_sites == 0u) {
    counts.replay_proof_sites += 1u;
  }
}

static void CollectConcurrencyReplayRaceGuardExprSites(
    const Expr *expr,
    Objc3ConcurrencyReplayRaceGuardSiteCounts &counts) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::Call:
    CollectConcurrencyReplayRaceGuardSitesFromSymbol(expr->ident, counts);
    for (const auto &arg : expr->args) {
      CollectConcurrencyReplayRaceGuardExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectConcurrencyReplayRaceGuardSitesFromSymbol(expr->selector, counts);
    CollectConcurrencyReplayRaceGuardExprSites(expr->receiver.get(), counts);
    for (const auto &arg : expr->args) {
      CollectConcurrencyReplayRaceGuardExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::Binary:
    CollectConcurrencyReplayRaceGuardExprSites(expr->left.get(), counts);
    CollectConcurrencyReplayRaceGuardExprSites(expr->right.get(), counts);
    return;
  case Expr::Kind::Conditional:
    CollectConcurrencyReplayRaceGuardExprSites(expr->left.get(), counts);
    CollectConcurrencyReplayRaceGuardExprSites(expr->right.get(), counts);
    CollectConcurrencyReplayRaceGuardExprSites(expr->third.get(), counts);
    return;
  case Expr::Kind::BlockLiteral:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::NilLiteral:
  case Expr::Kind::Number:
  default:
    return;
  }
}

static void CollectConcurrencyReplayRaceGuardForClauseSites(
    const ForClause &clause,
    Objc3ConcurrencyReplayRaceGuardSiteCounts &counts) {
  CollectConcurrencyReplayRaceGuardExprSites(clause.value.get(), counts);
}

static void CollectConcurrencyReplayRaceGuardStmtSites(
    const Stmt *stmt,
    Objc3ConcurrencyReplayRaceGuardSiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectConcurrencyReplayRaceGuardExprSites(stmt->let_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectConcurrencyReplayRaceGuardExprSites(stmt->assign_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectConcurrencyReplayRaceGuardExprSites(stmt->return_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    CollectConcurrencyReplayRaceGuardExprSites(stmt->if_stmt->condition.get(), counts);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      CollectConcurrencyReplayRaceGuardStmtSites(then_stmt.get(), counts);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      CollectConcurrencyReplayRaceGuardStmtSites(else_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      CollectConcurrencyReplayRaceGuardStmtSites(body_stmt.get(), counts);
    }
    CollectConcurrencyReplayRaceGuardExprSites(
        stmt->do_while_stmt->condition.get(),
        counts);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    CollectConcurrencyReplayRaceGuardForClauseSites(stmt->for_stmt->init, counts);
    CollectConcurrencyReplayRaceGuardExprSites(stmt->for_stmt->condition.get(), counts);
    CollectConcurrencyReplayRaceGuardForClauseSites(stmt->for_stmt->step, counts);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      CollectConcurrencyReplayRaceGuardStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    CollectConcurrencyReplayRaceGuardExprSites(stmt->switch_stmt->condition.get(), counts);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        CollectConcurrencyReplayRaceGuardStmtSites(case_stmt.get(), counts);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    CollectConcurrencyReplayRaceGuardExprSites(stmt->while_stmt->condition.get(), counts);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      CollectConcurrencyReplayRaceGuardStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Block:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      CollectConcurrencyReplayRaceGuardStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectConcurrencyReplayRaceGuardExprSites(stmt->expr_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

static Objc3ConcurrencyReplayRaceGuardSiteCounts CountConcurrencyReplayRaceGuardSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3ConcurrencyReplayRaceGuardSiteCounts counts;
  for (const auto &stmt : body) {
    CollectConcurrencyReplayRaceGuardStmtSites(stmt.get(), counts);
  }
  return counts;
}

struct Objc3ConcurrencyReplayRaceGuardProfile {
  std::size_t concurrency_replay_race_guard_sites = 0;
  std::size_t concurrency_replay_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t deterministic_schedule_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic_concurrency_replay_race_guard_handoff = false;
};

static std::string BuildConcurrencyReplayRaceGuardProfile(
    std::size_t concurrency_replay_race_guard_sites,
    std::size_t concurrency_replay_sites,
    std::size_t replay_proof_sites,
    std::size_t race_guard_sites,
    std::size_t task_handoff_sites,
    std::size_t actor_isolation_sites,
    std::size_t deterministic_schedule_sites,
    std::size_t guard_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_concurrency_replay_race_guard_handoff) {
  std::ostringstream out;
  out << "concurrency-replay-race-guard:concurrency_replay_race_guard_sites="
      << concurrency_replay_race_guard_sites
      << ";concurrency_replay_sites=" << concurrency_replay_sites
      << ";replay_proof_sites=" << replay_proof_sites
      << ";race_guard_sites=" << race_guard_sites
      << ";task_handoff_sites=" << task_handoff_sites
      << ";actor_isolation_sites=" << actor_isolation_sites
      << ";deterministic_schedule_sites=" << deterministic_schedule_sites
      << ";guard_blocked_sites=" << guard_blocked_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_concurrency_replay_race_guard_handoff="
      << (deterministic_concurrency_replay_race_guard_handoff ? "true" : "false");
  return out.str();
}

static bool IsConcurrencyReplayRaceGuardProfileNormalized(
    std::size_t concurrency_replay_race_guard_sites,
    std::size_t concurrency_replay_sites,
    std::size_t replay_proof_sites,
    std::size_t race_guard_sites,
    std::size_t task_handoff_sites,
    std::size_t actor_isolation_sites,
    std::size_t deterministic_schedule_sites,
    std::size_t guard_blocked_sites,
    std::size_t contract_violation_sites) {
  if (concurrency_replay_race_guard_sites != concurrency_replay_sites ||
      replay_proof_sites > concurrency_replay_sites ||
      race_guard_sites > concurrency_replay_sites ||
      task_handoff_sites > concurrency_replay_sites ||
      actor_isolation_sites > concurrency_replay_sites ||
      deterministic_schedule_sites > concurrency_replay_sites ||
      guard_blocked_sites > concurrency_replay_sites ||
      contract_violation_sites > concurrency_replay_sites) {
    return false;
  }
  if (deterministic_schedule_sites + guard_blocked_sites != concurrency_replay_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}

static Objc3ConcurrencyReplayRaceGuardProfile BuildConcurrencyReplayRaceGuardProfileFromCounts(
    std::size_t replay_proof_sites,
    std::size_t race_guard_sites,
    std::size_t task_handoff_sites,
    std::size_t actor_isolation_sites) {
  Objc3ConcurrencyReplayRaceGuardProfile profile;
  profile.replay_proof_sites = replay_proof_sites;
  profile.race_guard_sites = race_guard_sites;
  profile.task_handoff_sites = task_handoff_sites;
  profile.actor_isolation_sites = actor_isolation_sites;
  if (profile.replay_proof_sites >
      std::numeric_limits<std::size_t>::max() - profile.task_handoff_sites) {
    profile.concurrency_replay_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.concurrency_replay_sites =
        profile.replay_proof_sites + profile.task_handoff_sites;
  }
  profile.concurrency_replay_race_guard_sites = profile.concurrency_replay_sites;
  profile.guard_blocked_sites =
      std::min(profile.concurrency_replay_sites, profile.race_guard_sites / 2u);
  profile.deterministic_schedule_sites =
      profile.concurrency_replay_sites - profile.guard_blocked_sites;
  if (profile.concurrency_replay_race_guard_sites != profile.concurrency_replay_sites ||
      profile.replay_proof_sites > profile.concurrency_replay_sites ||
      profile.race_guard_sites > profile.concurrency_replay_sites ||
      profile.task_handoff_sites > profile.concurrency_replay_sites ||
      profile.actor_isolation_sites > profile.concurrency_replay_sites ||
      profile.deterministic_schedule_sites > profile.concurrency_replay_sites ||
      profile.guard_blocked_sites > profile.concurrency_replay_sites ||
      profile.contract_violation_sites > profile.concurrency_replay_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.deterministic_schedule_sites + profile.guard_blocked_sites !=
      profile.concurrency_replay_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.contract_violation_sites > profile.concurrency_replay_sites) {
    profile.contract_violation_sites = profile.concurrency_replay_sites;
  }
  profile.deterministic_concurrency_replay_race_guard_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

static Objc3ConcurrencyReplayRaceGuardProfile BuildConcurrencyReplayRaceGuardProfileFromFunction(
    const FunctionDecl &fn) {
  const Objc3ConcurrencyReplayRaceGuardSiteCounts counts =
      CountConcurrencyReplayRaceGuardSitesInBody(fn.body);
  return BuildConcurrencyReplayRaceGuardProfileFromCounts(
      counts.replay_proof_sites,
      counts.race_guard_sites,
      counts.task_handoff_sites,
      counts.actor_isolation_sites);
}

static Objc3ConcurrencyReplayRaceGuardProfile BuildConcurrencyReplayRaceGuardProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3ConcurrencyReplayRaceGuardSiteCounts counts;
  if (method.has_body) {
    CollectConcurrencyReplayRaceGuardSitesFromSymbol(method.selector, counts);
  }
  return BuildConcurrencyReplayRaceGuardProfileFromCounts(
      counts.replay_proof_sites,
      counts.race_guard_sites,
      counts.task_handoff_sites,
      counts.actor_isolation_sites);
}

static std::string BuildProtocolQualifiedObjectTypeProfile(
    bool object_pointer_type_spelling,
    bool has_generic_suffix,
    bool generic_suffix_terminated,
    bool has_pointer_declarator,
    const std::string &generic_suffix_text) {
  const bool protocol_composition_valid =
      !has_generic_suffix || (generic_suffix_terminated && object_pointer_type_spelling);
  std::ostringstream out;
  out << "protocol-qualified-object-type:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-protocol-composition=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";composition-bytes=" << generic_suffix_text.size()
      << ";composition-valid=" << (protocol_composition_valid ? "true" : "false");
  return out.str();
}

static bool IsProtocolQualifiedObjectTypeProfileNormalized(
    bool object_pointer_type_spelling,
    bool has_generic_suffix,
    bool generic_suffix_terminated) {
  if (!has_generic_suffix) {
    return true;
  }
  return generic_suffix_terminated && object_pointer_type_spelling;
}

static std::string BuildBlockLiteralCaptureProfile(const std::vector<std::string> &capture_names_lexicographic) {
  if (capture_names_lexicographic.empty()) {
    return "block-captures:none";
  }
  std::ostringstream out;
  out << "block-captures:";
  for (std::size_t i = 0; i < capture_names_lexicographic.size(); ++i) {
    out << capture_names_lexicographic[i];
    if (i + 1u != capture_names_lexicographic.size()) {
      out << ",";
    }
  }
  return out.str();
}

static std::string BuildBlockLiteralAbiLayoutProfile(std::size_t parameter_count,
                                                     std::size_t capture_count,
                                                     std::size_t body_statement_count) {
  std::ostringstream out;
  out << "block-abi-layout:invoke-arg-slots=" << parameter_count
      << ";capture-words=" << capture_count
      << ";body-statements=" << body_statement_count;
  return out.str();
}

static std::string BuildBlockLiteralAbiDescriptorSymbol(unsigned line,
                                                        unsigned column,
                                                        std::size_t parameter_count,
                                                        std::size_t capture_count) {
  std::ostringstream out;
  out << "__objc3_block_desc_" << line << "_" << column
      << "_p" << parameter_count
      << "_c" << capture_count;
  return out.str();
}

static std::string BuildBlockLiteralInvokeTrampolineSymbol(unsigned line,
                                                           unsigned column,
                                                           std::size_t parameter_count,
                                                           std::size_t capture_count) {
  std::ostringstream out;
  out << "__objc3_block_invoke_" << line << "_" << column
      << "_p" << parameter_count
      << "_c" << capture_count;
  return out.str();
}

static std::string BuildBlockStorageEscapeProfile(std::size_t mutable_capture_count,
                                                  std::size_t byref_slot_count,
                                                  bool escape_to_heap,
                                                  std::size_t body_statement_count) {
  std::ostringstream out;
  out << "block-storage:mutable-captures=" << mutable_capture_count
      << ";byref-slots=" << byref_slot_count
      << ";escape=" << (escape_to_heap ? "heap" : "stack")
      << ";body-statements=" << body_statement_count;
  return out.str();
}

static std::string BuildBlockStorageByrefLayoutSymbol(unsigned line,
                                                      unsigned column,
                                                      std::size_t mutable_capture_count,
                                                      std::size_t byref_slot_count,
                                                      bool escape_to_heap) {
  std::ostringstream out;
  out << "__objc3_block_byref_layout_" << line << "_" << column
      << "_m" << mutable_capture_count
      << "_b" << byref_slot_count
      << "_" << (escape_to_heap ? "heap" : "stack");
  return out.str();
}

static std::string BuildBlockCopyDisposeProfile(std::size_t mutable_capture_count,
                                                std::size_t byref_slot_count,
                                                bool escape_to_heap,
                                                std::size_t body_statement_count) {
  std::ostringstream out;
  out << "block-copy-dispose:copy-helper=" << (mutable_capture_count > 0u ? "enabled" : "elided")
      << ";dispose-helper=" << (byref_slot_count > 0u ? "enabled" : "elided")
      << ";escape=" << (escape_to_heap ? "heap" : "stack")
      << ";body-statements=" << body_statement_count;
  return out.str();
}

static std::string BuildBlockCopyHelperSymbol(unsigned line,
                                              unsigned column,
                                              std::size_t mutable_capture_count,
                                              std::size_t byref_slot_count,
                                              bool escape_to_heap) {
  std::ostringstream out;
  out << "__objc3_block_copy_helper_" << line << "_" << column
      << "_m" << mutable_capture_count
      << "_b" << byref_slot_count
      << "_" << (escape_to_heap ? "heap" : "stack");
  return out.str();
}

static std::string BuildBlockDisposeHelperSymbol(unsigned line,
                                                 unsigned column,
                                                 std::size_t mutable_capture_count,
                                                 std::size_t byref_slot_count,
                                                 bool escape_to_heap) {
  std::ostringstream out;
  out << "__objc3_block_dispose_helper_" << line << "_" << column
      << "_m" << mutable_capture_count
      << "_b" << byref_slot_count
      << "_" << (escape_to_heap ? "heap" : "stack");
  return out.str();
}

static std::size_t BuildBlockDeterminismPerfBaselineWeight(std::size_t parameter_count,
                                                           std::size_t capture_count,
                                                           std::size_t body_statement_count,
                                                           bool copy_helper_required,
                                                           bool dispose_helper_required) {
  std::size_t weight = parameter_count * 2u + capture_count * 8u + body_statement_count * 4u;
  if (copy_helper_required) {
    weight += 6u;
  }
  if (dispose_helper_required) {
    weight += 6u;
  }
  return weight;
}

static std::string BuildBlockDeterminismPerfBaselineProfile(std::size_t parameter_count,
                                                            std::size_t capture_count,
                                                            std::size_t body_statement_count,
                                                            bool copy_helper_required,
                                                            bool dispose_helper_required,
                                                            bool deterministic_capture_set,
                                                            bool copy_dispose_profile_is_normalized,
                                                            std::size_t baseline_weight) {
  const char *tier = baseline_weight <= 24u ? "light" : (baseline_weight <= 64u ? "medium" : "heavy");
  std::ostringstream out;
  out << "block-det-perf-baseline:params=" << parameter_count
      << ";captures=" << capture_count
      << ";body-statements=" << body_statement_count
      << ";copy-helper=" << (copy_helper_required ? "enabled" : "elided")
      << ";dispose-helper=" << (dispose_helper_required ? "enabled" : "elided")
      << ";deterministic-captures=" << (deterministic_capture_set ? "true" : "false")
      << ";normalized=" << (copy_dispose_profile_is_normalized ? "true" : "false")
      << ";weight=" << baseline_weight
      << ";tier=" << tier;
  return out.str();
}

static std::vector<std::string> BuildScopePathLexicographic(std::string owner_symbol,
                                                             std::string entry_symbol) {
  std::vector<std::string> path;
  if (!owner_symbol.empty()) {
    path.push_back(std::move(owner_symbol));
  }
  if (!entry_symbol.empty()) {
    path.push_back(std::move(entry_symbol));
  }
  std::sort(path.begin(), path.end());
  path.erase(std::unique(path.begin(), path.end()), path.end());
  return path;
}

static std::string BuildObjcContainerScopeOwner(const std::string &container_kind,
                                                const std::string &name,
                                                bool has_category,
                                                const std::string &category_name) {
  std::string owner = container_kind + ":" + name;
  if (has_category) {
    owner += "(" + category_name + ")";
  }
  return owner;
}

static std::string BuildObjcMethodScopePathSymbol(const Objc3MethodDecl &method) {
  return (method.is_class_method ? "class_method:" : "instance_method:") + method.selector;
}

static std::string BuildObjcPropertyScopePathSymbol(const Objc3PropertyDecl &property) {
  return "property:" + property.name;
}

static std::string BuildObjcPropertySynthesisSymbol(const Objc3PropertyDecl &property) {
  return "property_synthesis:" + property.name;
}

static std::string BuildObjcIvarBindingSymbol(const Objc3PropertyDecl &property) {
  return "ivar_binding:_" + property.name;
}

static std::string BuildObjcTypecheckParamFamilySymbol(const FuncParam &param) {
  if (param.id_spelling) {
    return "id";
  }
  if (param.class_spelling) {
    return "Class";
  }
  if (param.sel_spelling) {
    return "SEL";
  }
  if (param.object_pointer_type_spelling) {
    return "object-pointer:" + param.object_pointer_type_name;
  }
  return "";
}

static std::string BuildObjcTypecheckReturnFamilySymbol(const FunctionDecl &fn) {
  if (fn.return_id_spelling) {
    return "id";
  }
  if (fn.return_class_spelling) {
    return "Class";
  }
  if (fn.return_sel_spelling) {
    return "SEL";
  }
  if (fn.return_object_pointer_type_spelling) {
    return "object-pointer:" + fn.return_object_pointer_type_name;
  }
  return "";
}

static bool IsOwnershipQualifierSpelling(const std::string &text) {
  return text == "__strong" || text == "__weak" || text == "__autoreleasing" ||
         text == "__unsafe_unretained";
}

static std::string BuildOwnershipQualifierSymbol(const std::string &spelling, bool is_return_type) {
  if (spelling.empty()) {
    return "";
  }
  return std::string(is_return_type ? "return-ownership-qualifier:" : "ownership-qualifier:") + spelling;
}

struct Objc3OwnershipOperationProfile {
  bool insert_retain = false;
  bool insert_release = false;
  bool insert_autorelease = false;
  std::string profile;
};

struct Objc3WeakUnownedLifetimeProfile {
  bool is_weak_reference = false;
  bool is_unowned_reference = false;
  bool is_unowned_safe_reference = false;
  std::string lifetime_profile;
  std::string runtime_hook_profile;
};

struct Objc3ArcDiagnosticFixitProfile {
  bool diagnostic_candidate = false;
  bool fixit_available = false;
  std::string diagnostic_profile;
  std::string fixit_hint;
};

static Objc3OwnershipOperationProfile BuildParamOwnershipOperationProfile(const std::string &spelling) {
  Objc3OwnershipOperationProfile profile;
  if (spelling == "__strong") {
    profile.insert_retain = true;
    profile.insert_release = true;
    profile.profile = "param-retain-release";
  } else if (spelling == "__weak") {
    profile.profile = "param-weak-side-table";
  } else if (spelling == "__autoreleasing") {
    profile.insert_autorelease = true;
    profile.profile = "param-autorelease-bridge";
  } else if (spelling == "__unsafe_unretained") {
    profile.profile = "param-unsafe-unretained";
  }
  return profile;
}

static Objc3OwnershipOperationProfile BuildReturnOwnershipOperationProfile(const std::string &spelling) {
  Objc3OwnershipOperationProfile profile;
  if (spelling == "__strong") {
    profile.insert_retain = true;
    profile.insert_release = true;
    profile.profile = "return-retain-release-transfer";
  } else if (spelling == "__weak") {
    profile.profile = "return-weak-load";
  } else if (spelling == "__autoreleasing") {
    profile.insert_autorelease = true;
    profile.profile = "return-autorelease-transfer";
  } else if (spelling == "__unsafe_unretained") {
    profile.profile = "return-unsafe-unretained";
  }
  return profile;
}

static Objc3WeakUnownedLifetimeProfile BuildWeakUnownedLifetimeProfile(const std::string &spelling,
                                                                       bool prefer_safe_unowned) {
  Objc3WeakUnownedLifetimeProfile profile;
  if (spelling == "__weak") {
    profile.is_weak_reference = true;
    profile.lifetime_profile = "weak";
    profile.runtime_hook_profile = "objc-weak-side-table";
  } else if (spelling == "__unsafe_unretained") {
    profile.is_unowned_reference = true;
    profile.is_unowned_safe_reference = prefer_safe_unowned;
    profile.lifetime_profile = prefer_safe_unowned ? "unowned-safe" : "unowned-unsafe";
    profile.runtime_hook_profile = prefer_safe_unowned ? "objc-unowned-safe-guard"
                                                       : "objc-unowned-unsafe-direct";
  } else if (spelling == "__strong") {
    profile.lifetime_profile = "strong-owned";
  } else if (spelling == "__autoreleasing") {
    profile.lifetime_profile = "autoreleasing";
  }
  return profile;
}

static Objc3WeakUnownedLifetimeProfile BuildPropertyWeakUnownedLifetimeProfile(
    const Objc3PropertyDecl &property) {
  if (property.is_weak) {
    return BuildWeakUnownedLifetimeProfile("__weak", false);
  }
  if (property.is_unowned) {
    return BuildWeakUnownedLifetimeProfile("__unsafe_unretained", true);
  }
  if (!property.ownership_qualifier_spelling.empty()) {
    return BuildWeakUnownedLifetimeProfile(property.ownership_qualifier_spelling, false);
  }
  if (property.is_assign) {
    return BuildWeakUnownedLifetimeProfile("__unsafe_unretained", false);
  }
  return Objc3WeakUnownedLifetimeProfile{};
}

static Objc3ArcDiagnosticFixitProfile BuildArcDiagnosticFixitProfile(const std::string &spelling,
                                                                     bool is_return_type,
                                                                     bool is_property_type,
                                                                     bool weak_unowned_conflict) {
  Objc3ArcDiagnosticFixitProfile profile;
  if (weak_unowned_conflict) {
    profile.diagnostic_candidate = true;
    profile.fixit_available = true;
    profile.diagnostic_profile = "arc-weak-unowned-conflict";
    profile.fixit_hint = "remove-weak-or-unowned-attribute";
    return profile;
  }

  if (spelling == "__unsafe_unretained") {
    profile.diagnostic_candidate = true;
    profile.fixit_available = true;
    profile.diagnostic_profile = is_return_type ? "arc-return-unsafe-unretained" : "arc-unsafe-unretained";
    profile.fixit_hint = is_property_type ? "replace-with-weak-or-strong-attribute"
                                          : "replace-with-__weak-or-__strong";
    return profile;
  }

  if (spelling == "__autoreleasing") {
    profile.diagnostic_candidate = true;
    profile.fixit_available = true;
    profile.diagnostic_profile = is_return_type ? "arc-return-autoreleasing-transfer" : "arc-autoreleasing-misuse";
    profile.fixit_hint = is_return_type ? "replace-return-qualifier-with-__strong"
                                        : "replace-with-__strong-or-out-parameter";
    return profile;
  }

  if (is_return_type && spelling == "__weak") {
    profile.diagnostic_candidate = true;
    profile.fixit_available = true;
    profile.diagnostic_profile = "arc-return-weak-escape";
    profile.fixit_hint = "replace-return-qualifier-with-__strong";
  }
  return profile;
}

static std::vector<std::string> BuildSortedUniqueStrings(std::vector<std::string> values) {
  std::sort(values.begin(), values.end());
  values.erase(std::unique(values.begin(), values.end()), values.end());
  return values;
}

static std::vector<std::string> BuildProtocolSemanticLinkTargetsLexicographic(
    const std::vector<std::string> &protocol_names) {
  std::vector<std::string> targets;
  targets.reserve(protocol_names.size());
  for (const auto &name : protocol_names) {
    if (!name.empty()) {
      targets.push_back("protocol:" + name);
    }
  }
  return BuildSortedUniqueStrings(std::move(targets));
}

static std::string BuildObjcCategorySemanticLinkSymbol(const std::string &owner_name,
                                                       const std::string &category_name) {
  return "category:" + owner_name + "(" + category_name + ")";
}

static std::string BuildObjcMethodLookupSymbol(const Objc3MethodDecl &method) {
  return (method.is_class_method ? "class_lookup:" : "instance_lookup:") + method.selector;
}

static std::string BuildObjcMethodOverrideLookupSymbol(const Objc3MethodDecl &method) {
  return (method.is_class_method ? "class_override:" : "instance_override:") + method.selector;
}

static std::string BuildObjcMethodConflictLookupSymbol(const Objc3MethodDecl &method) {
  return (method.is_class_method ? "class_conflict:" : "instance_conflict:") + method.selector;
}

static std::vector<std::string> BuildObjcMethodLookupSymbolsLexicographic(
    const std::vector<Objc3MethodDecl> &methods) {
  std::vector<std::string> symbols;
  symbols.reserve(methods.size());
  for (const auto &method : methods) {
    if (!method.method_lookup_symbol.empty()) {
      symbols.push_back(method.method_lookup_symbol);
    }
  }
  return BuildSortedUniqueStrings(std::move(symbols));
}

static std::vector<std::string> BuildObjcMethodOverrideLookupSymbolsLexicographic(
    const std::vector<Objc3MethodDecl> &methods) {
  std::vector<std::string> symbols;
  symbols.reserve(methods.size());
  for (const auto &method : methods) {
    if (!method.override_lookup_symbol.empty()) {
      symbols.push_back(method.override_lookup_symbol);
    }
  }
  return BuildSortedUniqueStrings(std::move(symbols));
}

static std::vector<std::string> BuildObjcMethodConflictLookupSymbolsLexicographic(
    const std::vector<Objc3MethodDecl> &methods) {
  std::vector<std::string> symbols;
  symbols.reserve(methods.size());
  for (const auto &method : methods) {
    if (!method.conflict_lookup_symbol.empty()) {
      symbols.push_back(method.conflict_lookup_symbol);
    }
  }
  return BuildSortedUniqueStrings(std::move(symbols));
}

static std::vector<std::string> BuildObjcPropertySynthesisSymbolsLexicographic(
    const std::vector<Objc3PropertyDecl> &properties) {
  std::vector<std::string> symbols;
  symbols.reserve(properties.size());
  for (const auto &property : properties) {
    if (!property.property_synthesis_symbol.empty()) {
      symbols.push_back(property.property_synthesis_symbol);
    }
  }
  return BuildSortedUniqueStrings(std::move(symbols));
}

static std::vector<std::string> BuildObjcIvarBindingSymbolsLexicographic(
    const std::vector<Objc3PropertyDecl> &properties) {
  std::vector<std::string> symbols;
  symbols.reserve(properties.size());
  for (const auto &property : properties) {
    if (!property.ivar_binding_symbol.empty()) {
      symbols.push_back(property.ivar_binding_symbol);
    }
  }
  return BuildSortedUniqueStrings(std::move(symbols));
}

class Objc3Parser {
 public:
  explicit Objc3Parser(const std::vector<Token> &tokens) : tokens_(tokens) {}

  Objc3ParsedProgram Parse() {
    Objc3ParsedProgram program = ast_builder_.BeginProgram();
    while (!At(TokenKind::Eof)) {
      if (Match(TokenKind::KwModule)) {
        ParseModule(program);
      } else if (Match(TokenKind::KwLet)) {
        auto decl = ParseGlobalLet();
        if (decl != nullptr) {
          ast_builder_.AddGlobalDecl(program, std::move(*decl));
        }
      } else if (Match(TokenKind::KwAtInterface)) {
        auto decl = ParseObjcInterfaceDecl();
        if (decl != nullptr) {
          ast_builder_.AddInterfaceDecl(program, std::move(*decl));
        }
      } else if (Match(TokenKind::KwAtImplementation)) {
        auto decl = ParseObjcImplementationDecl();
        if (decl != nullptr) {
          ast_builder_.AddImplementationDecl(program, std::move(*decl));
        }
      } else if (Match(TokenKind::KwAtProtocol)) {
        auto decl = ParseObjcProtocolDecl();
        if (decl != nullptr) {
          ast_builder_.AddProtocolDecl(program, std::move(*decl));
        }
      } else if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwFn)) {
        ParseTopLevelFunctionDecl(program);
      } else {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P100",
                                        "unsupported Objective-C 3 statement"));
        SynchronizeTopLevel();
      }
    }
    return program;
  }

  std::vector<std::string> TakeDiagnostics() { return diagnostics_; }

 private:
  bool At(TokenKind kind) const { return tokens_[index_].kind == kind; }

  const Token &Peek() const { return tokens_[index_]; }

  const Token &Previous() const { return tokens_[index_ - 1]; }

  const Token &Advance() {
    if (!At(TokenKind::Eof)) {
      ++index_;
    }
    return Previous();
  }

  bool Match(TokenKind kind) {
    if (At(kind)) {
      Advance();
      return true;
    }
    return false;
  }

  void ParseTopLevelFunctionDecl(Objc3ParsedProgram &program) {
    bool is_pure = false;
    bool is_extern = false;
    std::optional<TokenKind> trailing_qualifier = std::nullopt;

    while (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      if (Match(TokenKind::KwPure)) {
        if (is_pure) {
          const Token &token = Previous();
          diagnostics_.push_back(
              MakeDiag(token.line, token.column, "O3P100", "duplicate 'pure' qualifier in function declaration"));
          SynchronizeTopLevel();
          return;
        }
        is_pure = true;
        trailing_qualifier = TokenKind::KwPure;
        continue;
      }

      if (Match(TokenKind::KwExtern)) {
        if (is_extern) {
          const Token &token = Previous();
          diagnostics_.push_back(
              MakeDiag(token.line, token.column, "O3P100", "duplicate 'extern' qualifier in function declaration"));
          SynchronizeTopLevel();
          return;
        }
        is_extern = true;
        trailing_qualifier = TokenKind::KwExtern;
      }
    }

    if (!Match(TokenKind::KwFn)) {
      const Token &token = Peek();
      const std::string message = (trailing_qualifier.has_value() && trailing_qualifier.value() == TokenKind::KwExtern)
                                      ? "expected 'fn' after 'extern'"
                                      : "expected 'fn' after 'pure'";
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P100", message));
      SynchronizeTopLevel();
      return;
    }

    auto fn = ParseFunction();
    if (fn == nullptr) {
      return;
    }

    fn->is_pure = is_pure;
    if (is_extern && !fn->is_prototype) {
      diagnostics_.push_back(MakeDiag(fn->line, fn->column, "O3P104", "missing ';' after extern function declaration"));
      return;
    }

    ast_builder_.AddFunctionDecl(program, std::move(*fn));
  }

  bool AtIdentifierColon() const {
    return At(TokenKind::Identifier) && (index_ + 1 < tokens_.size()) &&
           tokens_[index_ + 1].kind == TokenKind::Colon;
  }

  static bool IsAssignmentOperatorToken(TokenKind kind) {
    return kind == TokenKind::Equal || kind == TokenKind::PlusEqual || kind == TokenKind::MinusEqual ||
           kind == TokenKind::StarEqual || kind == TokenKind::SlashEqual || kind == TokenKind::PercentEqual ||
           kind == TokenKind::AmpersandEqual || kind == TokenKind::PipeEqual || kind == TokenKind::CaretEqual ||
           kind == TokenKind::LessLessEqual || kind == TokenKind::GreaterGreaterEqual;
  }

  static bool IsUpdateOperatorToken(TokenKind kind) {
    return kind == TokenKind::PlusPlus || kind == TokenKind::MinusMinus;
  }

  bool AtIdentifierAssignment() const {
    return At(TokenKind::Identifier) && (index_ + 1 < tokens_.size()) &&
           IsAssignmentOperatorToken(tokens_[index_ + 1].kind);
  }

  bool AtIdentifierUpdate() const {
    return At(TokenKind::Identifier) && (index_ + 1 < tokens_.size()) &&
           IsUpdateOperatorToken(tokens_[index_ + 1].kind);
  }

  bool AtPrefixUpdate() const {
    return IsUpdateOperatorToken(Peek().kind) && (index_ + 1 < tokens_.size()) &&
           tokens_[index_ + 1].kind == TokenKind::Identifier;
  }

  bool MatchAssignmentOperator(std::string &op) {
    if (Match(TokenKind::Equal)) {
      op = "=";
      return true;
    }
    if (Match(TokenKind::PlusEqual)) {
      op = "+=";
      return true;
    }
    if (Match(TokenKind::MinusEqual)) {
      op = "-=";
      return true;
    }
    if (Match(TokenKind::StarEqual)) {
      op = "*=";
      return true;
    }
    if (Match(TokenKind::SlashEqual)) {
      op = "/=";
      return true;
    }
    if (Match(TokenKind::PercentEqual)) {
      op = "%=";
      return true;
    }
    if (Match(TokenKind::AmpersandEqual)) {
      op = "&=";
      return true;
    }
    if (Match(TokenKind::PipeEqual)) {
      op = "|=";
      return true;
    }
    if (Match(TokenKind::CaretEqual)) {
      op = "^=";
      return true;
    }
    if (Match(TokenKind::LessLessEqual)) {
      op = "<<=";
      return true;
    }
    if (Match(TokenKind::GreaterGreaterEqual)) {
      op = ">>=";
      return true;
    }
    return false;
  }

  bool MatchUpdateOperator(std::string &op) {
    if (Match(TokenKind::PlusPlus)) {
      op = "++";
      return true;
    }
    if (Match(TokenKind::MinusMinus)) {
      op = "--";
      return true;
    }
    return false;
  }

  void ParseModule(Objc3ParsedProgram &program) {
    const Token &name_token = Peek();
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P101", "invalid module identifier"));
      SynchronizeTopLevel();
      return;
    }
    const std::string module_name = Advance().text;
    if (!Match(TokenKind::Semicolon)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P104", "missing ';' after module declaration"));
      SynchronizeTopLevel();
      return;
    }
    if (saw_module_declaration_) {
      diagnostics_.push_back(
          MakeDiag(name_token.line, name_token.column, "O3S200", "duplicate module '" + module_name + "'"));
      return;
    }
    saw_module_declaration_ = true;
    ast_builder_.SetModuleName(program, module_name);
  }

  std::unique_ptr<GlobalDecl> ParseGlobalLet() {
    auto decl = std::make_unique<GlobalDecl>();
    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(MakeDiag(name_token.line, name_token.column, "O3P101",
                                      "invalid declaration identifier"));
      SynchronizeTopLevel();
      return nullptr;
    }
    decl->name = Previous().text;
    decl->scope_owner_symbol = BuildObjcContainerScopeOwner("protocol", decl->name, false, "");
    decl->scope_path_lexicographic = BuildScopePathLexicographic(decl->scope_owner_symbol, "protocol:" + decl->name);
    decl->semantic_link_symbol = "protocol:" + decl->name;
    decl->line = Previous().line;
    decl->column = Previous().column;

    if (!Match(TokenKind::Equal)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P102", "missing '='"));
      SynchronizeTopLevel();
      return nullptr;
    }

    decl->value = ParseExpression();
    if (decl->value == nullptr) {
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!Match(TokenKind::Semicolon)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104",
                                      "missing ';' after declaration"));
      SynchronizeTopLevel();
      return nullptr;
    }
    return decl;
  }

  void CopyMethodReturnTypeFromFunctionDecl(const FunctionDecl &source, Objc3MethodDecl &target) {
    target.return_type = source.return_type;
    target.return_vector_spelling = source.return_vector_spelling;
    target.return_vector_base_spelling = source.return_vector_base_spelling;
    target.return_vector_lane_count = source.return_vector_lane_count;
    target.return_id_spelling = source.return_id_spelling;
    target.return_class_spelling = source.return_class_spelling;
    target.return_sel_spelling = source.return_sel_spelling;
    target.return_instancetype_spelling = source.return_instancetype_spelling;
    target.return_object_pointer_type_spelling = source.return_object_pointer_type_spelling;
    target.return_object_pointer_type_name = source.return_object_pointer_type_name;
    target.return_typecheck_family_symbol = source.return_typecheck_family_symbol;
    target.has_return_generic_suffix = source.has_return_generic_suffix;
    target.return_generic_suffix_terminated = source.return_generic_suffix_terminated;
    target.return_generic_suffix_text = source.return_generic_suffix_text;
    target.return_generic_line = source.return_generic_line;
    target.return_generic_column = source.return_generic_column;
    target.return_lightweight_generic_constraint_profile_is_normalized =
        source.return_lightweight_generic_constraint_profile_is_normalized;
    target.return_lightweight_generic_constraint_profile =
        source.return_lightweight_generic_constraint_profile;
    target.return_nullability_flow_profile_is_normalized =
        source.return_nullability_flow_profile_is_normalized;
    target.return_nullability_flow_profile =
        source.return_nullability_flow_profile;
    target.return_protocol_qualified_object_type_profile_is_normalized =
        source.return_protocol_qualified_object_type_profile_is_normalized;
    target.return_protocol_qualified_object_type_profile =
        source.return_protocol_qualified_object_type_profile;
    target.return_variance_bridge_cast_profile_is_normalized =
        source.return_variance_bridge_cast_profile_is_normalized;
    target.return_variance_bridge_cast_profile =
        source.return_variance_bridge_cast_profile;
    target.return_generic_metadata_abi_profile_is_normalized =
        source.return_generic_metadata_abi_profile_is_normalized;
    target.return_generic_metadata_abi_profile =
        source.return_generic_metadata_abi_profile;
    target.return_module_import_graph_profile_is_normalized =
        source.return_module_import_graph_profile_is_normalized;
    target.return_module_import_graph_profile =
        source.return_module_import_graph_profile;
    target.return_namespace_collision_shadowing_profile_is_normalized =
        source.return_namespace_collision_shadowing_profile_is_normalized;
    target.return_namespace_collision_shadowing_profile =
        source.return_namespace_collision_shadowing_profile;
    target.return_public_private_api_partition_profile_is_normalized =
        source.return_public_private_api_partition_profile_is_normalized;
    target.return_public_private_api_partition_profile =
        source.return_public_private_api_partition_profile;
    target.return_incremental_module_cache_invalidation_profile_is_normalized =
        source.return_incremental_module_cache_invalidation_profile_is_normalized;
    target.return_incremental_module_cache_invalidation_profile =
        source.return_incremental_module_cache_invalidation_profile;
    target.return_cross_module_conformance_profile_is_normalized =
        source.return_cross_module_conformance_profile_is_normalized;
    target.return_cross_module_conformance_profile =
        source.return_cross_module_conformance_profile;
    target.has_return_pointer_declarator = source.has_return_pointer_declarator;
    target.return_pointer_declarator_depth = source.return_pointer_declarator_depth;
    target.return_pointer_declarator_tokens = source.return_pointer_declarator_tokens;
    target.return_nullability_suffix_tokens = source.return_nullability_suffix_tokens;
    target.has_return_ownership_qualifier = source.has_return_ownership_qualifier;
    target.return_ownership_qualifier_spelling = source.return_ownership_qualifier_spelling;
    target.return_ownership_qualifier_symbol = source.return_ownership_qualifier_symbol;
    target.return_ownership_qualifier_tokens = source.return_ownership_qualifier_tokens;
    target.return_ownership_insert_retain = source.return_ownership_insert_retain;
    target.return_ownership_insert_release = source.return_ownership_insert_release;
    target.return_ownership_insert_autorelease = source.return_ownership_insert_autorelease;
    target.return_ownership_operation_profile = source.return_ownership_operation_profile;
    target.return_ownership_is_weak_reference = source.return_ownership_is_weak_reference;
    target.return_ownership_is_unowned_reference = source.return_ownership_is_unowned_reference;
    target.return_ownership_is_unowned_safe_reference = source.return_ownership_is_unowned_safe_reference;
    target.return_ownership_lifetime_profile = source.return_ownership_lifetime_profile;
    target.return_ownership_runtime_hook_profile = source.return_ownership_runtime_hook_profile;
    target.return_ownership_arc_diagnostic_candidate = source.return_ownership_arc_diagnostic_candidate;
    target.return_ownership_arc_fixit_available = source.return_ownership_arc_fixit_available;
    target.return_ownership_arc_diagnostic_profile = source.return_ownership_arc_diagnostic_profile;
    target.return_ownership_arc_fixit_hint = source.return_ownership_arc_fixit_hint;
    target.throws_declared = source.throws_declared;
    target.throws_declaration_profile_is_normalized =
        source.throws_declaration_profile_is_normalized;
    target.throws_declaration_profile = source.throws_declaration_profile;
    target.result_like_profile_is_normalized = source.result_like_profile_is_normalized;
    target.deterministic_result_like_lowering_handoff =
        source.deterministic_result_like_lowering_handoff;
    target.result_like_sites = source.result_like_sites;
    target.result_success_sites = source.result_success_sites;
    target.result_failure_sites = source.result_failure_sites;
    target.result_branch_sites = source.result_branch_sites;
    target.result_payload_sites = source.result_payload_sites;
    target.result_normalized_sites = source.result_normalized_sites;
    target.result_branch_merge_sites = source.result_branch_merge_sites;
    target.result_contract_violation_sites = source.result_contract_violation_sites;
    target.result_like_profile = source.result_like_profile;
    target.ns_error_bridging_profile_is_normalized = source.ns_error_bridging_profile_is_normalized;
    target.deterministic_ns_error_bridging_lowering_handoff =
        source.deterministic_ns_error_bridging_lowering_handoff;
    target.ns_error_bridging_sites = source.ns_error_bridging_sites;
    target.ns_error_parameter_sites = source.ns_error_parameter_sites;
    target.ns_error_out_parameter_sites = source.ns_error_out_parameter_sites;
    target.ns_error_bridge_path_sites = source.ns_error_bridge_path_sites;
    target.failable_call_sites = source.failable_call_sites;
    target.ns_error_bridging_normalized_sites = source.ns_error_bridging_normalized_sites;
    target.ns_error_bridge_boundary_sites = source.ns_error_bridge_boundary_sites;
    target.ns_error_bridging_contract_violation_sites = source.ns_error_bridging_contract_violation_sites;
    target.ns_error_bridging_profile = source.ns_error_bridging_profile;
    target.async_continuation_profile_is_normalized =
        source.async_continuation_profile_is_normalized;
    target.deterministic_async_continuation_handoff =
        source.deterministic_async_continuation_handoff;
    target.async_continuation_sites = source.async_continuation_sites;
    target.async_keyword_sites = source.async_keyword_sites;
    target.async_function_sites = source.async_function_sites;
    target.continuation_allocation_sites = source.continuation_allocation_sites;
    target.continuation_resume_sites = source.continuation_resume_sites;
    target.continuation_suspend_sites = source.continuation_suspend_sites;
    target.async_state_machine_sites = source.async_state_machine_sites;
    target.async_continuation_normalized_sites =
        source.async_continuation_normalized_sites;
    target.async_continuation_gate_blocked_sites =
        source.async_continuation_gate_blocked_sites;
    target.async_continuation_contract_violation_sites =
        source.async_continuation_contract_violation_sites;
    target.async_continuation_profile = source.async_continuation_profile;
    target.await_suspension_profile_is_normalized =
        source.await_suspension_profile_is_normalized;
    target.deterministic_await_suspension_handoff =
        source.deterministic_await_suspension_handoff;
    target.await_suspension_sites = source.await_suspension_sites;
    target.await_keyword_sites = source.await_keyword_sites;
    target.await_suspension_point_sites = source.await_suspension_point_sites;
    target.await_resume_sites = source.await_resume_sites;
    target.await_state_machine_sites = source.await_state_machine_sites;
    target.await_continuation_sites = source.await_continuation_sites;
    target.await_suspension_normalized_sites =
        source.await_suspension_normalized_sites;
    target.await_suspension_gate_blocked_sites =
        source.await_suspension_gate_blocked_sites;
    target.await_suspension_contract_violation_sites =
        source.await_suspension_contract_violation_sites;
    target.await_suspension_profile = source.await_suspension_profile;
    target.actor_isolation_sendability_profile_is_normalized =
        source.actor_isolation_sendability_profile_is_normalized;
    target.deterministic_actor_isolation_sendability_handoff =
        source.deterministic_actor_isolation_sendability_handoff;
    target.actor_isolation_sendability_sites =
        source.actor_isolation_sendability_sites;
    target.actor_isolation_decl_sites = source.actor_isolation_decl_sites;
    target.actor_hop_sites = source.actor_hop_sites;
    target.sendable_annotation_sites = source.sendable_annotation_sites;
    target.non_sendable_crossing_sites = source.non_sendable_crossing_sites;
    target.isolation_boundary_sites = source.isolation_boundary_sites;
    target.actor_isolation_sendability_normalized_sites =
        source.actor_isolation_sendability_normalized_sites;
    target.actor_isolation_sendability_gate_blocked_sites =
        source.actor_isolation_sendability_gate_blocked_sites;
    target.actor_isolation_sendability_contract_violation_sites =
        source.actor_isolation_sendability_contract_violation_sites;
    target.actor_isolation_sendability_profile =
        source.actor_isolation_sendability_profile;
    target.task_runtime_cancellation_profile_is_normalized =
        source.task_runtime_cancellation_profile_is_normalized;
    target.deterministic_task_runtime_cancellation_handoff =
        source.deterministic_task_runtime_cancellation_handoff;
    target.task_runtime_interop_sites = source.task_runtime_interop_sites;
    target.runtime_hook_sites = source.runtime_hook_sites;
    target.cancellation_check_sites = source.cancellation_check_sites;
    target.cancellation_handler_sites = source.cancellation_handler_sites;
    target.suspension_point_sites = source.suspension_point_sites;
    target.cancellation_propagation_sites =
        source.cancellation_propagation_sites;
    target.task_runtime_normalized_sites = source.task_runtime_normalized_sites;
    target.task_runtime_gate_blocked_sites = source.task_runtime_gate_blocked_sites;
    target.task_runtime_contract_violation_sites =
        source.task_runtime_contract_violation_sites;
    target.task_runtime_cancellation_profile =
        source.task_runtime_cancellation_profile;
    target.concurrency_replay_race_guard_profile_is_normalized =
        source.concurrency_replay_race_guard_profile_is_normalized;
    target.deterministic_concurrency_replay_race_guard_handoff =
        source.deterministic_concurrency_replay_race_guard_handoff;
    target.concurrency_replay_race_guard_sites =
        source.concurrency_replay_race_guard_sites;
    target.concurrency_replay_sites = source.concurrency_replay_sites;
    target.replay_proof_sites = source.replay_proof_sites;
    target.race_guard_sites = source.race_guard_sites;
    target.task_handoff_sites = source.task_handoff_sites;
    target.actor_isolation_sites = source.actor_isolation_sites;
    target.deterministic_schedule_sites = source.deterministic_schedule_sites;
    target.concurrency_replay_guard_blocked_sites =
        source.concurrency_replay_guard_blocked_sites;
    target.concurrency_replay_contract_violation_sites =
        source.concurrency_replay_contract_violation_sites;
    target.concurrency_replay_race_guard_profile =
        source.concurrency_replay_race_guard_profile;
    target.unsafe_pointer_extension_profile_is_normalized =
        source.unsafe_pointer_extension_profile_is_normalized;
    target.deterministic_unsafe_pointer_extension_handoff =
        source.deterministic_unsafe_pointer_extension_handoff;
    target.unsafe_pointer_extension_sites = source.unsafe_pointer_extension_sites;
    target.unsafe_keyword_sites = source.unsafe_keyword_sites;
    target.pointer_arithmetic_sites = source.pointer_arithmetic_sites;
    target.raw_pointer_type_sites = source.raw_pointer_type_sites;
    target.unsafe_operation_sites = source.unsafe_operation_sites;
    target.unsafe_pointer_extension_normalized_sites =
        source.unsafe_pointer_extension_normalized_sites;
    target.unsafe_pointer_extension_gate_blocked_sites =
        source.unsafe_pointer_extension_gate_blocked_sites;
    target.unsafe_pointer_extension_contract_violation_sites =
        source.unsafe_pointer_extension_contract_violation_sites;
    target.unsafe_pointer_extension_profile = source.unsafe_pointer_extension_profile;
    target.inline_asm_intrinsic_governance_profile_is_normalized =
        source.inline_asm_intrinsic_governance_profile_is_normalized;
    target.deterministic_inline_asm_intrinsic_governance_handoff =
        source.deterministic_inline_asm_intrinsic_governance_handoff;
    target.inline_asm_intrinsic_sites = source.inline_asm_intrinsic_sites;
    target.inline_asm_sites = source.inline_asm_sites;
    target.intrinsic_sites = source.intrinsic_sites;
    target.governed_intrinsic_sites = source.governed_intrinsic_sites;
    target.privileged_intrinsic_sites = source.privileged_intrinsic_sites;
    target.inline_asm_intrinsic_normalized_sites =
        source.inline_asm_intrinsic_normalized_sites;
    target.inline_asm_intrinsic_gate_blocked_sites =
        source.inline_asm_intrinsic_gate_blocked_sites;
    target.inline_asm_intrinsic_contract_violation_sites =
        source.inline_asm_intrinsic_contract_violation_sites;
    target.inline_asm_intrinsic_governance_profile =
        source.inline_asm_intrinsic_governance_profile;
  }

  void CopyPropertyTypeFromParam(const FuncParam &source, Objc3PropertyDecl &target) {
    target.type = source.type;
    target.vector_spelling = source.vector_spelling;
    target.vector_base_spelling = source.vector_base_spelling;
    target.vector_lane_count = source.vector_lane_count;
    target.id_spelling = source.id_spelling;
    target.class_spelling = source.class_spelling;
    target.sel_spelling = source.sel_spelling;
    target.instancetype_spelling = source.instancetype_spelling;
    target.object_pointer_type_spelling = source.object_pointer_type_spelling;
    target.object_pointer_type_name = source.object_pointer_type_name;
    target.typecheck_family_symbol = source.typecheck_family_symbol;
    target.has_generic_suffix = source.has_generic_suffix;
    target.generic_suffix_terminated = source.generic_suffix_terminated;
    target.generic_suffix_text = source.generic_suffix_text;
    target.generic_line = source.generic_line;
    target.generic_column = source.generic_column;
    target.lightweight_generic_constraint_profile_is_normalized =
        source.lightweight_generic_constraint_profile_is_normalized;
    target.lightweight_generic_constraint_profile =
        source.lightweight_generic_constraint_profile;
    target.nullability_flow_profile_is_normalized =
        source.nullability_flow_profile_is_normalized;
    target.nullability_flow_profile =
        source.nullability_flow_profile;
    target.protocol_qualified_object_type_profile_is_normalized =
        source.protocol_qualified_object_type_profile_is_normalized;
    target.protocol_qualified_object_type_profile =
        source.protocol_qualified_object_type_profile;
    target.variance_bridge_cast_profile_is_normalized =
        source.variance_bridge_cast_profile_is_normalized;
    target.variance_bridge_cast_profile =
        source.variance_bridge_cast_profile;
    target.generic_metadata_abi_profile_is_normalized =
        source.generic_metadata_abi_profile_is_normalized;
    target.generic_metadata_abi_profile =
        source.generic_metadata_abi_profile;
    target.module_import_graph_profile_is_normalized =
        source.module_import_graph_profile_is_normalized;
    target.module_import_graph_profile =
        source.module_import_graph_profile;
    target.namespace_collision_shadowing_profile_is_normalized =
        source.namespace_collision_shadowing_profile_is_normalized;
    target.namespace_collision_shadowing_profile =
        source.namespace_collision_shadowing_profile;
    target.public_private_api_partition_profile_is_normalized =
        source.public_private_api_partition_profile_is_normalized;
    target.public_private_api_partition_profile =
        source.public_private_api_partition_profile;
    target.incremental_module_cache_invalidation_profile_is_normalized =
        source.incremental_module_cache_invalidation_profile_is_normalized;
    target.incremental_module_cache_invalidation_profile =
        source.incremental_module_cache_invalidation_profile;
    target.cross_module_conformance_profile_is_normalized =
        source.cross_module_conformance_profile_is_normalized;
    target.cross_module_conformance_profile =
        source.cross_module_conformance_profile;
    target.has_pointer_declarator = source.has_pointer_declarator;
    target.pointer_declarator_depth = source.pointer_declarator_depth;
    target.pointer_declarator_tokens = source.pointer_declarator_tokens;
    target.nullability_suffix_tokens = source.nullability_suffix_tokens;
    target.has_ownership_qualifier = source.has_ownership_qualifier;
    target.ownership_qualifier_spelling = source.ownership_qualifier_spelling;
    target.ownership_qualifier_symbol = source.ownership_qualifier_symbol;
    target.ownership_qualifier_tokens = source.ownership_qualifier_tokens;
    target.ownership_insert_retain = source.ownership_insert_retain;
    target.ownership_insert_release = source.ownership_insert_release;
    target.ownership_insert_autorelease = source.ownership_insert_autorelease;
    target.ownership_operation_profile = source.ownership_operation_profile;
    target.ownership_is_weak_reference = source.ownership_is_weak_reference;
    target.ownership_is_unowned_reference = source.ownership_is_unowned_reference;
    target.ownership_is_unowned_safe_reference = source.ownership_is_unowned_safe_reference;
    target.ownership_lifetime_profile = source.ownership_lifetime_profile;
    target.ownership_runtime_hook_profile = source.ownership_runtime_hook_profile;
    target.ownership_arc_diagnostic_candidate = source.ownership_arc_diagnostic_candidate;
    target.ownership_arc_fixit_available = source.ownership_arc_fixit_available;
    target.ownership_arc_diagnostic_profile = source.ownership_arc_diagnostic_profile;
    target.ownership_arc_fixit_hint = source.ownership_arc_fixit_hint;
  }

  bool AtThrowsClauseKeyword() const {
    return At(TokenKind::Identifier) && Peek().text == "throws";
  }

  bool ParseOptionalThrowsClause(FunctionDecl &fn) {
    if (!AtThrowsClauseKeyword()) {
      return true;
    }
    const Token throws_token = Advance();
    if (fn.throws_declared) {
      diagnostics_.push_back(MakeDiag(throws_token.line, throws_token.column, "O3P181",
                                      "duplicate 'throws' declaration modifier"));
      return false;
    }
    fn.throws_declared = true;
    return true;
  }

  bool ParseOptionalThrowsClause(Objc3MethodDecl &method) {
    if (!AtThrowsClauseKeyword()) {
      return true;
    }
    const Token throws_token = Advance();
    if (method.throws_declared) {
      diagnostics_.push_back(MakeDiag(throws_token.line, throws_token.column, "O3P181",
                                      "duplicate 'throws' declaration modifier"));
      return false;
    }
    method.throws_declared = true;
    return true;
  }

  void FinalizeThrowsDeclarationProfile(FunctionDecl &fn, bool has_return_annotation) {
    fn.throws_declaration_profile = BuildThrowsDeclarationProfile(
        fn.throws_declared,
        has_return_annotation,
        fn.is_prototype,
        !fn.is_prototype,
        false,
        false,
        fn.params.size(),
        0u);
    fn.throws_declaration_profile_is_normalized = IsThrowsDeclarationProfileNormalized(
        fn.is_prototype,
        !fn.is_prototype,
        false,
        0u);
  }

  void FinalizeThrowsDeclarationProfile(Objc3MethodDecl &method) {
    method.throws_declaration_profile = BuildThrowsDeclarationProfile(
        method.throws_declared,
        true,
        !method.has_body,
        method.has_body,
        true,
        method.is_class_method,
        method.params.size(),
        method.selector_pieces.size());
    method.throws_declaration_profile_is_normalized = IsThrowsDeclarationProfileNormalized(
        !method.has_body,
        method.has_body,
        true,
        method.selector_pieces.size());
  }

  void FinalizeResultLikeProfile(FunctionDecl &fn) {
    const Objc3ResultLikeProfile profile = BuildResultLikeProfileFromBody(fn.body);
    fn.result_like_sites = profile.result_like_sites;
    fn.result_success_sites = profile.result_success_sites;
    fn.result_failure_sites = profile.result_failure_sites;
    fn.result_branch_sites = profile.result_branch_sites;
    fn.result_payload_sites = profile.result_payload_sites;
    fn.result_normalized_sites = profile.normalized_sites;
    fn.result_branch_merge_sites = profile.branch_merge_sites;
    fn.result_contract_violation_sites = profile.contract_violation_sites;
    fn.deterministic_result_like_lowering_handoff =
        profile.deterministic_result_like_lowering_handoff;
    fn.result_like_profile = BuildResultLikeProfile(
        fn.result_like_sites,
        fn.result_success_sites,
        fn.result_failure_sites,
        fn.result_branch_sites,
        fn.result_payload_sites,
        fn.result_normalized_sites,
        fn.result_branch_merge_sites,
        fn.result_contract_violation_sites,
        fn.deterministic_result_like_lowering_handoff);
    fn.result_like_profile_is_normalized = IsResultLikeProfileNormalized(
        fn.result_like_sites,
        fn.result_success_sites,
        fn.result_failure_sites,
        fn.result_branch_sites,
        fn.result_payload_sites,
        fn.result_normalized_sites,
        fn.result_branch_merge_sites,
        fn.result_contract_violation_sites);
  }

  void FinalizeResultLikeProfile(Objc3MethodDecl &method) {
    const Objc3ResultLikeProfile profile = BuildResultLikeProfileFromOpaqueBody(method.has_body);
    method.result_like_sites = profile.result_like_sites;
    method.result_success_sites = profile.result_success_sites;
    method.result_failure_sites = profile.result_failure_sites;
    method.result_branch_sites = profile.result_branch_sites;
    method.result_payload_sites = profile.result_payload_sites;
    method.result_normalized_sites = profile.normalized_sites;
    method.result_branch_merge_sites = profile.branch_merge_sites;
    method.result_contract_violation_sites = profile.contract_violation_sites;
    method.deterministic_result_like_lowering_handoff =
        profile.deterministic_result_like_lowering_handoff;
    method.result_like_profile = BuildResultLikeProfile(
        method.result_like_sites,
        method.result_success_sites,
        method.result_failure_sites,
        method.result_branch_sites,
        method.result_payload_sites,
        method.result_normalized_sites,
        method.result_branch_merge_sites,
        method.result_contract_violation_sites,
        method.deterministic_result_like_lowering_handoff);
    method.result_like_profile_is_normalized = IsResultLikeProfileNormalized(
        method.result_like_sites,
        method.result_success_sites,
        method.result_failure_sites,
        method.result_branch_sites,
        method.result_payload_sites,
        method.result_normalized_sites,
        method.result_branch_merge_sites,
        method.result_contract_violation_sites);
  }

  void FinalizeNSErrorBridgingProfile(FunctionDecl &fn) {
    const Objc3NSErrorBridgingProfile profile = BuildNSErrorBridgingProfileFromFunction(fn);
    fn.ns_error_bridging_sites = profile.ns_error_bridging_sites;
    fn.ns_error_parameter_sites = profile.ns_error_parameter_sites;
    fn.ns_error_out_parameter_sites = profile.ns_error_out_parameter_sites;
    fn.ns_error_bridge_path_sites = profile.ns_error_bridge_path_sites;
    fn.failable_call_sites = profile.failable_call_sites;
    fn.ns_error_bridging_normalized_sites = profile.normalized_sites;
    fn.ns_error_bridge_boundary_sites = profile.bridge_boundary_sites;
    fn.ns_error_bridging_contract_violation_sites = profile.contract_violation_sites;
    fn.deterministic_ns_error_bridging_lowering_handoff =
        profile.deterministic_ns_error_bridging_lowering_handoff;
    fn.ns_error_bridging_profile = BuildNSErrorBridgingProfile(
        fn.ns_error_bridging_sites,
        fn.ns_error_parameter_sites,
        fn.ns_error_out_parameter_sites,
        fn.ns_error_bridge_path_sites,
        fn.failable_call_sites,
        fn.ns_error_bridging_normalized_sites,
        fn.ns_error_bridge_boundary_sites,
        fn.ns_error_bridging_contract_violation_sites,
        fn.deterministic_ns_error_bridging_lowering_handoff);
    fn.ns_error_bridging_profile_is_normalized = IsNSErrorBridgingProfileNormalized(
        fn.ns_error_bridging_sites,
        fn.ns_error_parameter_sites,
        fn.ns_error_out_parameter_sites,
        fn.ns_error_bridge_path_sites,
        fn.failable_call_sites,
        fn.ns_error_bridging_normalized_sites,
        fn.ns_error_bridge_boundary_sites,
        fn.ns_error_bridging_contract_violation_sites);
  }

  void FinalizeNSErrorBridgingProfile(Objc3MethodDecl &method) {
    const Objc3NSErrorBridgingProfile profile = BuildNSErrorBridgingProfileFromOpaqueBody(method);
    method.ns_error_bridging_sites = profile.ns_error_bridging_sites;
    method.ns_error_parameter_sites = profile.ns_error_parameter_sites;
    method.ns_error_out_parameter_sites = profile.ns_error_out_parameter_sites;
    method.ns_error_bridge_path_sites = profile.ns_error_bridge_path_sites;
    method.failable_call_sites = profile.failable_call_sites;
    method.ns_error_bridging_normalized_sites = profile.normalized_sites;
    method.ns_error_bridge_boundary_sites = profile.bridge_boundary_sites;
    method.ns_error_bridging_contract_violation_sites = profile.contract_violation_sites;
    method.deterministic_ns_error_bridging_lowering_handoff =
        profile.deterministic_ns_error_bridging_lowering_handoff;
    method.ns_error_bridging_profile = BuildNSErrorBridgingProfile(
        method.ns_error_bridging_sites,
        method.ns_error_parameter_sites,
        method.ns_error_out_parameter_sites,
        method.ns_error_bridge_path_sites,
        method.failable_call_sites,
        method.ns_error_bridging_normalized_sites,
        method.ns_error_bridge_boundary_sites,
        method.ns_error_bridging_contract_violation_sites,
        method.deterministic_ns_error_bridging_lowering_handoff);
    method.ns_error_bridging_profile_is_normalized = IsNSErrorBridgingProfileNormalized(
        method.ns_error_bridging_sites,
        method.ns_error_parameter_sites,
        method.ns_error_out_parameter_sites,
        method.ns_error_bridge_path_sites,
        method.failable_call_sites,
        method.ns_error_bridging_normalized_sites,
        method.ns_error_bridge_boundary_sites,
        method.ns_error_bridging_contract_violation_sites);
  }

  void FinalizeAsyncContinuationProfile(FunctionDecl &fn) {
    const Objc3AsyncContinuationProfile profile =
        BuildAsyncContinuationProfileFromFunction(fn);
    fn.async_continuation_sites = profile.async_continuation_sites;
    fn.async_keyword_sites = profile.async_keyword_sites;
    fn.async_function_sites = profile.async_function_sites;
    fn.continuation_allocation_sites = profile.continuation_allocation_sites;
    fn.continuation_resume_sites = profile.continuation_resume_sites;
    fn.continuation_suspend_sites = profile.continuation_suspend_sites;
    fn.async_state_machine_sites = profile.async_state_machine_sites;
    fn.async_continuation_normalized_sites = profile.normalized_sites;
    fn.async_continuation_gate_blocked_sites = profile.gate_blocked_sites;
    fn.async_continuation_contract_violation_sites =
        profile.contract_violation_sites;
    fn.deterministic_async_continuation_handoff =
        profile.deterministic_async_continuation_handoff;
    fn.async_continuation_profile = BuildAsyncContinuationProfile(
        fn.async_continuation_sites,
        fn.async_keyword_sites,
        fn.async_function_sites,
        fn.continuation_allocation_sites,
        fn.continuation_resume_sites,
        fn.continuation_suspend_sites,
        fn.async_state_machine_sites,
        fn.async_continuation_normalized_sites,
        fn.async_continuation_gate_blocked_sites,
        fn.async_continuation_contract_violation_sites,
        fn.deterministic_async_continuation_handoff);
    fn.async_continuation_profile_is_normalized =
        IsAsyncContinuationProfileNormalized(
            fn.async_continuation_sites,
            fn.async_keyword_sites,
            fn.async_function_sites,
            fn.continuation_allocation_sites,
            fn.continuation_resume_sites,
            fn.continuation_suspend_sites,
            fn.async_state_machine_sites,
            fn.async_continuation_normalized_sites,
            fn.async_continuation_gate_blocked_sites,
            fn.async_continuation_contract_violation_sites);
  }

  void FinalizeAsyncContinuationProfile(Objc3MethodDecl &method) {
    const Objc3AsyncContinuationProfile profile =
        BuildAsyncContinuationProfileFromOpaqueBody(method);
    method.async_continuation_sites = profile.async_continuation_sites;
    method.async_keyword_sites = profile.async_keyword_sites;
    method.async_function_sites = profile.async_function_sites;
    method.continuation_allocation_sites = profile.continuation_allocation_sites;
    method.continuation_resume_sites = profile.continuation_resume_sites;
    method.continuation_suspend_sites = profile.continuation_suspend_sites;
    method.async_state_machine_sites = profile.async_state_machine_sites;
    method.async_continuation_normalized_sites = profile.normalized_sites;
    method.async_continuation_gate_blocked_sites = profile.gate_blocked_sites;
    method.async_continuation_contract_violation_sites =
        profile.contract_violation_sites;
    method.deterministic_async_continuation_handoff =
        profile.deterministic_async_continuation_handoff;
    method.async_continuation_profile = BuildAsyncContinuationProfile(
        method.async_continuation_sites,
        method.async_keyword_sites,
        method.async_function_sites,
        method.continuation_allocation_sites,
        method.continuation_resume_sites,
        method.continuation_suspend_sites,
        method.async_state_machine_sites,
        method.async_continuation_normalized_sites,
        method.async_continuation_gate_blocked_sites,
        method.async_continuation_contract_violation_sites,
        method.deterministic_async_continuation_handoff);
    method.async_continuation_profile_is_normalized =
        IsAsyncContinuationProfileNormalized(
            method.async_continuation_sites,
            method.async_keyword_sites,
            method.async_function_sites,
            method.continuation_allocation_sites,
            method.continuation_resume_sites,
            method.continuation_suspend_sites,
            method.async_state_machine_sites,
            method.async_continuation_normalized_sites,
            method.async_continuation_gate_blocked_sites,
            method.async_continuation_contract_violation_sites);
  }

  void FinalizeAwaitSuspensionProfile(FunctionDecl &fn) {
    const Objc3AwaitSuspensionProfile profile =
        BuildAwaitSuspensionProfileFromFunction(fn);
    fn.await_suspension_sites = profile.await_suspension_sites;
    fn.await_keyword_sites = profile.await_keyword_sites;
    fn.await_suspension_point_sites = profile.await_suspension_point_sites;
    fn.await_resume_sites = profile.await_resume_sites;
    fn.await_state_machine_sites = profile.await_state_machine_sites;
    fn.await_continuation_sites = profile.await_continuation_sites;
    fn.await_suspension_normalized_sites = profile.normalized_sites;
    fn.await_suspension_gate_blocked_sites = profile.gate_blocked_sites;
    fn.await_suspension_contract_violation_sites = profile.contract_violation_sites;
    fn.deterministic_await_suspension_handoff =
        profile.deterministic_await_suspension_handoff;
    fn.await_suspension_profile = BuildAwaitSuspensionProfile(
        fn.await_suspension_sites,
        fn.await_keyword_sites,
        fn.await_suspension_point_sites,
        fn.await_resume_sites,
        fn.await_state_machine_sites,
        fn.await_continuation_sites,
        fn.await_suspension_normalized_sites,
        fn.await_suspension_gate_blocked_sites,
        fn.await_suspension_contract_violation_sites,
        fn.deterministic_await_suspension_handoff);
    fn.await_suspension_profile_is_normalized = IsAwaitSuspensionProfileNormalized(
        fn.await_suspension_sites,
        fn.await_keyword_sites,
        fn.await_suspension_point_sites,
        fn.await_resume_sites,
        fn.await_state_machine_sites,
        fn.await_continuation_sites,
        fn.await_suspension_normalized_sites,
        fn.await_suspension_gate_blocked_sites,
        fn.await_suspension_contract_violation_sites);
  }

  void FinalizeAwaitSuspensionProfile(Objc3MethodDecl &method) {
    const Objc3AwaitSuspensionProfile profile =
        BuildAwaitSuspensionProfileFromOpaqueBody(method);
    method.await_suspension_sites = profile.await_suspension_sites;
    method.await_keyword_sites = profile.await_keyword_sites;
    method.await_suspension_point_sites = profile.await_suspension_point_sites;
    method.await_resume_sites = profile.await_resume_sites;
    method.await_state_machine_sites = profile.await_state_machine_sites;
    method.await_continuation_sites = profile.await_continuation_sites;
    method.await_suspension_normalized_sites = profile.normalized_sites;
    method.await_suspension_gate_blocked_sites = profile.gate_blocked_sites;
    method.await_suspension_contract_violation_sites = profile.contract_violation_sites;
    method.deterministic_await_suspension_handoff =
        profile.deterministic_await_suspension_handoff;
    method.await_suspension_profile = BuildAwaitSuspensionProfile(
        method.await_suspension_sites,
        method.await_keyword_sites,
        method.await_suspension_point_sites,
        method.await_resume_sites,
        method.await_state_machine_sites,
        method.await_continuation_sites,
        method.await_suspension_normalized_sites,
        method.await_suspension_gate_blocked_sites,
        method.await_suspension_contract_violation_sites,
        method.deterministic_await_suspension_handoff);
    method.await_suspension_profile_is_normalized = IsAwaitSuspensionProfileNormalized(
        method.await_suspension_sites,
        method.await_keyword_sites,
        method.await_suspension_point_sites,
        method.await_resume_sites,
        method.await_state_machine_sites,
        method.await_continuation_sites,
        method.await_suspension_normalized_sites,
        method.await_suspension_gate_blocked_sites,
        method.await_suspension_contract_violation_sites);
  }

  void FinalizeActorIsolationSendabilityProfile(FunctionDecl &fn) {
    const Objc3ActorIsolationSendabilityProfile profile =
        BuildActorIsolationSendabilityProfileFromFunction(fn);
    fn.actor_isolation_sendability_sites = profile.actor_isolation_sendability_sites;
    fn.actor_isolation_decl_sites = profile.actor_isolation_decl_sites;
    fn.actor_hop_sites = profile.actor_hop_sites;
    fn.sendable_annotation_sites = profile.sendable_annotation_sites;
    fn.non_sendable_crossing_sites = profile.non_sendable_crossing_sites;
    fn.isolation_boundary_sites = profile.isolation_boundary_sites;
    fn.actor_isolation_sendability_normalized_sites = profile.normalized_sites;
    fn.actor_isolation_sendability_gate_blocked_sites = profile.gate_blocked_sites;
    fn.actor_isolation_sendability_contract_violation_sites =
        profile.contract_violation_sites;
    fn.deterministic_actor_isolation_sendability_handoff =
        profile.deterministic_actor_isolation_sendability_handoff;
    fn.actor_isolation_sendability_profile = BuildActorIsolationSendabilityProfile(
        fn.actor_isolation_sendability_sites,
        fn.actor_isolation_decl_sites,
        fn.actor_hop_sites,
        fn.sendable_annotation_sites,
        fn.non_sendable_crossing_sites,
        fn.isolation_boundary_sites,
        fn.actor_isolation_sendability_normalized_sites,
        fn.actor_isolation_sendability_gate_blocked_sites,
        fn.actor_isolation_sendability_contract_violation_sites,
        fn.deterministic_actor_isolation_sendability_handoff);
    fn.actor_isolation_sendability_profile_is_normalized =
        IsActorIsolationSendabilityProfileNormalized(
            fn.actor_isolation_sendability_sites,
            fn.actor_isolation_decl_sites,
            fn.actor_hop_sites,
            fn.sendable_annotation_sites,
            fn.non_sendable_crossing_sites,
            fn.isolation_boundary_sites,
            fn.actor_isolation_sendability_normalized_sites,
            fn.actor_isolation_sendability_gate_blocked_sites,
            fn.actor_isolation_sendability_contract_violation_sites);
  }

  void FinalizeActorIsolationSendabilityProfile(Objc3MethodDecl &method) {
    const Objc3ActorIsolationSendabilityProfile profile =
        BuildActorIsolationSendabilityProfileFromOpaqueBody(method);
    method.actor_isolation_sendability_sites = profile.actor_isolation_sendability_sites;
    method.actor_isolation_decl_sites = profile.actor_isolation_decl_sites;
    method.actor_hop_sites = profile.actor_hop_sites;
    method.sendable_annotation_sites = profile.sendable_annotation_sites;
    method.non_sendable_crossing_sites = profile.non_sendable_crossing_sites;
    method.isolation_boundary_sites = profile.isolation_boundary_sites;
    method.actor_isolation_sendability_normalized_sites = profile.normalized_sites;
    method.actor_isolation_sendability_gate_blocked_sites = profile.gate_blocked_sites;
    method.actor_isolation_sendability_contract_violation_sites =
        profile.contract_violation_sites;
    method.deterministic_actor_isolation_sendability_handoff =
        profile.deterministic_actor_isolation_sendability_handoff;
    method.actor_isolation_sendability_profile = BuildActorIsolationSendabilityProfile(
        method.actor_isolation_sendability_sites,
        method.actor_isolation_decl_sites,
        method.actor_hop_sites,
        method.sendable_annotation_sites,
        method.non_sendable_crossing_sites,
        method.isolation_boundary_sites,
        method.actor_isolation_sendability_normalized_sites,
        method.actor_isolation_sendability_gate_blocked_sites,
        method.actor_isolation_sendability_contract_violation_sites,
        method.deterministic_actor_isolation_sendability_handoff);
    method.actor_isolation_sendability_profile_is_normalized =
        IsActorIsolationSendabilityProfileNormalized(
            method.actor_isolation_sendability_sites,
            method.actor_isolation_decl_sites,
            method.actor_hop_sites,
            method.sendable_annotation_sites,
            method.non_sendable_crossing_sites,
            method.isolation_boundary_sites,
            method.actor_isolation_sendability_normalized_sites,
            method.actor_isolation_sendability_gate_blocked_sites,
            method.actor_isolation_sendability_contract_violation_sites);
  }

  void FinalizeTaskRuntimeCancellationProfile(FunctionDecl &fn) {
    const Objc3TaskRuntimeCancellationProfile profile =
        BuildTaskRuntimeCancellationProfileFromFunction(fn);
    fn.task_runtime_interop_sites = profile.task_runtime_interop_sites;
    fn.runtime_hook_sites = profile.runtime_hook_sites;
    fn.cancellation_check_sites = profile.cancellation_check_sites;
    fn.cancellation_handler_sites = profile.cancellation_handler_sites;
    fn.suspension_point_sites = profile.suspension_point_sites;
    fn.cancellation_propagation_sites = profile.cancellation_propagation_sites;
    fn.task_runtime_normalized_sites = profile.normalized_sites;
    fn.task_runtime_gate_blocked_sites = profile.gate_blocked_sites;
    fn.task_runtime_contract_violation_sites = profile.contract_violation_sites;
    fn.deterministic_task_runtime_cancellation_handoff =
        profile.deterministic_task_runtime_cancellation_handoff;
    fn.task_runtime_cancellation_profile = BuildTaskRuntimeCancellationProfile(
        fn.task_runtime_interop_sites,
        fn.runtime_hook_sites,
        fn.cancellation_check_sites,
        fn.cancellation_handler_sites,
        fn.suspension_point_sites,
        fn.cancellation_propagation_sites,
        fn.task_runtime_normalized_sites,
        fn.task_runtime_gate_blocked_sites,
        fn.task_runtime_contract_violation_sites,
        fn.deterministic_task_runtime_cancellation_handoff);
    fn.task_runtime_cancellation_profile_is_normalized =
        IsTaskRuntimeCancellationProfileNormalized(
            fn.task_runtime_interop_sites,
            fn.runtime_hook_sites,
            fn.cancellation_check_sites,
            fn.cancellation_handler_sites,
            fn.suspension_point_sites,
            fn.cancellation_propagation_sites,
            fn.task_runtime_normalized_sites,
            fn.task_runtime_gate_blocked_sites,
            fn.task_runtime_contract_violation_sites);
  }

  void FinalizeTaskRuntimeCancellationProfile(Objc3MethodDecl &method) {
    const Objc3TaskRuntimeCancellationProfile profile =
        BuildTaskRuntimeCancellationProfileFromOpaqueBody(method);
    method.task_runtime_interop_sites = profile.task_runtime_interop_sites;
    method.runtime_hook_sites = profile.runtime_hook_sites;
    method.cancellation_check_sites = profile.cancellation_check_sites;
    method.cancellation_handler_sites = profile.cancellation_handler_sites;
    method.suspension_point_sites = profile.suspension_point_sites;
    method.cancellation_propagation_sites = profile.cancellation_propagation_sites;
    method.task_runtime_normalized_sites = profile.normalized_sites;
    method.task_runtime_gate_blocked_sites = profile.gate_blocked_sites;
    method.task_runtime_contract_violation_sites = profile.contract_violation_sites;
    method.deterministic_task_runtime_cancellation_handoff =
        profile.deterministic_task_runtime_cancellation_handoff;
    method.task_runtime_cancellation_profile = BuildTaskRuntimeCancellationProfile(
        method.task_runtime_interop_sites,
        method.runtime_hook_sites,
        method.cancellation_check_sites,
        method.cancellation_handler_sites,
        method.suspension_point_sites,
        method.cancellation_propagation_sites,
        method.task_runtime_normalized_sites,
        method.task_runtime_gate_blocked_sites,
        method.task_runtime_contract_violation_sites,
        method.deterministic_task_runtime_cancellation_handoff);
    method.task_runtime_cancellation_profile_is_normalized =
        IsTaskRuntimeCancellationProfileNormalized(
            method.task_runtime_interop_sites,
            method.runtime_hook_sites,
            method.cancellation_check_sites,
            method.cancellation_handler_sites,
            method.suspension_point_sites,
            method.cancellation_propagation_sites,
            method.task_runtime_normalized_sites,
            method.task_runtime_gate_blocked_sites,
            method.task_runtime_contract_violation_sites);
  }

  void FinalizeConcurrencyReplayRaceGuardProfile(FunctionDecl &fn) {
    const Objc3ConcurrencyReplayRaceGuardProfile profile =
        BuildConcurrencyReplayRaceGuardProfileFromFunction(fn);
    fn.concurrency_replay_race_guard_sites = profile.concurrency_replay_race_guard_sites;
    fn.concurrency_replay_sites = profile.concurrency_replay_sites;
    fn.replay_proof_sites = profile.replay_proof_sites;
    fn.race_guard_sites = profile.race_guard_sites;
    fn.task_handoff_sites = profile.task_handoff_sites;
    fn.actor_isolation_sites = profile.actor_isolation_sites;
    fn.deterministic_schedule_sites = profile.deterministic_schedule_sites;
    fn.concurrency_replay_guard_blocked_sites = profile.guard_blocked_sites;
    fn.concurrency_replay_contract_violation_sites =
        profile.contract_violation_sites;
    fn.deterministic_concurrency_replay_race_guard_handoff =
        profile.deterministic_concurrency_replay_race_guard_handoff;
    fn.concurrency_replay_race_guard_profile =
        BuildConcurrencyReplayRaceGuardProfile(
            fn.concurrency_replay_race_guard_sites,
            fn.concurrency_replay_sites,
            fn.replay_proof_sites,
            fn.race_guard_sites,
            fn.task_handoff_sites,
            fn.actor_isolation_sites,
            fn.deterministic_schedule_sites,
            fn.concurrency_replay_guard_blocked_sites,
            fn.concurrency_replay_contract_violation_sites,
            fn.deterministic_concurrency_replay_race_guard_handoff);
    fn.concurrency_replay_race_guard_profile_is_normalized =
        IsConcurrencyReplayRaceGuardProfileNormalized(
            fn.concurrency_replay_race_guard_sites,
            fn.concurrency_replay_sites,
            fn.replay_proof_sites,
            fn.race_guard_sites,
            fn.task_handoff_sites,
            fn.actor_isolation_sites,
            fn.deterministic_schedule_sites,
            fn.concurrency_replay_guard_blocked_sites,
            fn.concurrency_replay_contract_violation_sites);
  }

  void FinalizeConcurrencyReplayRaceGuardProfile(Objc3MethodDecl &method) {
    const Objc3ConcurrencyReplayRaceGuardProfile profile =
        BuildConcurrencyReplayRaceGuardProfileFromOpaqueBody(method);
    method.concurrency_replay_race_guard_sites =
        profile.concurrency_replay_race_guard_sites;
    method.concurrency_replay_sites = profile.concurrency_replay_sites;
    method.replay_proof_sites = profile.replay_proof_sites;
    method.race_guard_sites = profile.race_guard_sites;
    method.task_handoff_sites = profile.task_handoff_sites;
    method.actor_isolation_sites = profile.actor_isolation_sites;
    method.deterministic_schedule_sites = profile.deterministic_schedule_sites;
    method.concurrency_replay_guard_blocked_sites = profile.guard_blocked_sites;
    method.concurrency_replay_contract_violation_sites =
        profile.contract_violation_sites;
    method.deterministic_concurrency_replay_race_guard_handoff =
        profile.deterministic_concurrency_replay_race_guard_handoff;
    method.concurrency_replay_race_guard_profile =
        BuildConcurrencyReplayRaceGuardProfile(
            method.concurrency_replay_race_guard_sites,
            method.concurrency_replay_sites,
            method.replay_proof_sites,
            method.race_guard_sites,
            method.task_handoff_sites,
            method.actor_isolation_sites,
            method.deterministic_schedule_sites,
            method.concurrency_replay_guard_blocked_sites,
            method.concurrency_replay_contract_violation_sites,
            method.deterministic_concurrency_replay_race_guard_handoff);
    method.concurrency_replay_race_guard_profile_is_normalized =
        IsConcurrencyReplayRaceGuardProfileNormalized(
            method.concurrency_replay_race_guard_sites,
            method.concurrency_replay_sites,
            method.replay_proof_sites,
            method.race_guard_sites,
            method.task_handoff_sites,
            method.actor_isolation_sites,
            method.deterministic_schedule_sites,
            method.concurrency_replay_guard_blocked_sites,
            method.concurrency_replay_contract_violation_sites);
  }

  void FinalizeUnsafePointerExtensionProfile(FunctionDecl &fn) {
    const Objc3UnsafePointerExtensionProfile profile =
        BuildUnsafePointerExtensionProfileFromFunction(fn);
    fn.unsafe_pointer_extension_sites = profile.unsafe_pointer_extension_sites;
    fn.unsafe_keyword_sites = profile.unsafe_keyword_sites;
    fn.pointer_arithmetic_sites = profile.pointer_arithmetic_sites;
    fn.raw_pointer_type_sites = profile.raw_pointer_type_sites;
    fn.unsafe_operation_sites = profile.unsafe_operation_sites;
    fn.unsafe_pointer_extension_normalized_sites = profile.normalized_sites;
    fn.unsafe_pointer_extension_gate_blocked_sites = profile.gate_blocked_sites;
    fn.unsafe_pointer_extension_contract_violation_sites =
        profile.contract_violation_sites;
    fn.deterministic_unsafe_pointer_extension_handoff =
        profile.deterministic_unsafe_pointer_extension_handoff;
    fn.unsafe_pointer_extension_profile = BuildUnsafePointerExtensionProfile(
        fn.unsafe_pointer_extension_sites,
        fn.unsafe_keyword_sites,
        fn.pointer_arithmetic_sites,
        fn.raw_pointer_type_sites,
        fn.unsafe_operation_sites,
        fn.unsafe_pointer_extension_normalized_sites,
        fn.unsafe_pointer_extension_gate_blocked_sites,
        fn.unsafe_pointer_extension_contract_violation_sites,
        fn.deterministic_unsafe_pointer_extension_handoff);
    fn.unsafe_pointer_extension_profile_is_normalized =
        IsUnsafePointerExtensionProfileNormalized(
            fn.unsafe_pointer_extension_sites,
            fn.unsafe_keyword_sites,
            fn.pointer_arithmetic_sites,
            fn.raw_pointer_type_sites,
            fn.unsafe_operation_sites,
            fn.unsafe_pointer_extension_normalized_sites,
            fn.unsafe_pointer_extension_gate_blocked_sites,
            fn.unsafe_pointer_extension_contract_violation_sites);
  }

  void FinalizeUnsafePointerExtensionProfile(Objc3MethodDecl &method) {
    const Objc3UnsafePointerExtensionProfile profile =
        BuildUnsafePointerExtensionProfileFromOpaqueBody(method);
    method.unsafe_pointer_extension_sites = profile.unsafe_pointer_extension_sites;
    method.unsafe_keyword_sites = profile.unsafe_keyword_sites;
    method.pointer_arithmetic_sites = profile.pointer_arithmetic_sites;
    method.raw_pointer_type_sites = profile.raw_pointer_type_sites;
    method.unsafe_operation_sites = profile.unsafe_operation_sites;
    method.unsafe_pointer_extension_normalized_sites = profile.normalized_sites;
    method.unsafe_pointer_extension_gate_blocked_sites = profile.gate_blocked_sites;
    method.unsafe_pointer_extension_contract_violation_sites =
        profile.contract_violation_sites;
    method.deterministic_unsafe_pointer_extension_handoff =
        profile.deterministic_unsafe_pointer_extension_handoff;
    method.unsafe_pointer_extension_profile = BuildUnsafePointerExtensionProfile(
        method.unsafe_pointer_extension_sites,
        method.unsafe_keyword_sites,
        method.pointer_arithmetic_sites,
        method.raw_pointer_type_sites,
        method.unsafe_operation_sites,
        method.unsafe_pointer_extension_normalized_sites,
        method.unsafe_pointer_extension_gate_blocked_sites,
        method.unsafe_pointer_extension_contract_violation_sites,
        method.deterministic_unsafe_pointer_extension_handoff);
    method.unsafe_pointer_extension_profile_is_normalized =
        IsUnsafePointerExtensionProfileNormalized(
            method.unsafe_pointer_extension_sites,
            method.unsafe_keyword_sites,
            method.pointer_arithmetic_sites,
            method.raw_pointer_type_sites,
            method.unsafe_operation_sites,
            method.unsafe_pointer_extension_normalized_sites,
            method.unsafe_pointer_extension_gate_blocked_sites,
            method.unsafe_pointer_extension_contract_violation_sites);
  }

  void FinalizeInlineAsmIntrinsicGovernanceProfile(FunctionDecl &fn) {
    const Objc3InlineAsmIntrinsicGovernanceProfile profile =
        BuildInlineAsmIntrinsicGovernanceProfileFromFunction(fn);
    fn.inline_asm_intrinsic_sites = profile.inline_asm_intrinsic_sites;
    fn.inline_asm_sites = profile.inline_asm_sites;
    fn.intrinsic_sites = profile.intrinsic_sites;
    fn.governed_intrinsic_sites = profile.governed_intrinsic_sites;
    fn.privileged_intrinsic_sites = profile.privileged_intrinsic_sites;
    fn.inline_asm_intrinsic_normalized_sites = profile.normalized_sites;
    fn.inline_asm_intrinsic_gate_blocked_sites = profile.gate_blocked_sites;
    fn.inline_asm_intrinsic_contract_violation_sites =
        profile.contract_violation_sites;
    fn.deterministic_inline_asm_intrinsic_governance_handoff =
        profile.deterministic_inline_asm_intrinsic_governance_handoff;
    fn.inline_asm_intrinsic_governance_profile =
        BuildInlineAsmIntrinsicGovernanceProfile(
            fn.inline_asm_intrinsic_sites,
            fn.inline_asm_sites,
            fn.intrinsic_sites,
            fn.governed_intrinsic_sites,
            fn.privileged_intrinsic_sites,
            fn.inline_asm_intrinsic_normalized_sites,
            fn.inline_asm_intrinsic_gate_blocked_sites,
            fn.inline_asm_intrinsic_contract_violation_sites,
            fn.deterministic_inline_asm_intrinsic_governance_handoff);
    fn.inline_asm_intrinsic_governance_profile_is_normalized =
        IsInlineAsmIntrinsicGovernanceProfileNormalized(
            fn.inline_asm_intrinsic_sites,
            fn.inline_asm_sites,
            fn.intrinsic_sites,
            fn.governed_intrinsic_sites,
            fn.privileged_intrinsic_sites,
            fn.inline_asm_intrinsic_normalized_sites,
            fn.inline_asm_intrinsic_gate_blocked_sites,
            fn.inline_asm_intrinsic_contract_violation_sites);
  }

  void FinalizeInlineAsmIntrinsicGovernanceProfile(Objc3MethodDecl &method) {
    const Objc3InlineAsmIntrinsicGovernanceProfile profile =
        BuildInlineAsmIntrinsicGovernanceProfileFromOpaqueBody(method);
    method.inline_asm_intrinsic_sites = profile.inline_asm_intrinsic_sites;
    method.inline_asm_sites = profile.inline_asm_sites;
    method.intrinsic_sites = profile.intrinsic_sites;
    method.governed_intrinsic_sites = profile.governed_intrinsic_sites;
    method.privileged_intrinsic_sites = profile.privileged_intrinsic_sites;
    method.inline_asm_intrinsic_normalized_sites = profile.normalized_sites;
    method.inline_asm_intrinsic_gate_blocked_sites = profile.gate_blocked_sites;
    method.inline_asm_intrinsic_contract_violation_sites =
        profile.contract_violation_sites;
    method.deterministic_inline_asm_intrinsic_governance_handoff =
        profile.deterministic_inline_asm_intrinsic_governance_handoff;
    method.inline_asm_intrinsic_governance_profile =
        BuildInlineAsmIntrinsicGovernanceProfile(
            method.inline_asm_intrinsic_sites,
            method.inline_asm_sites,
            method.intrinsic_sites,
            method.governed_intrinsic_sites,
            method.privileged_intrinsic_sites,
            method.inline_asm_intrinsic_normalized_sites,
            method.inline_asm_intrinsic_gate_blocked_sites,
            method.inline_asm_intrinsic_contract_violation_sites,
            method.deterministic_inline_asm_intrinsic_governance_handoff);
    method.inline_asm_intrinsic_governance_profile_is_normalized =
        IsInlineAsmIntrinsicGovernanceProfileNormalized(
            method.inline_asm_intrinsic_sites,
            method.inline_asm_sites,
            method.intrinsic_sites,
            method.governed_intrinsic_sites,
            method.privileged_intrinsic_sites,
            method.inline_asm_intrinsic_normalized_sites,
            method.inline_asm_intrinsic_gate_blocked_sites,
            method.inline_asm_intrinsic_contract_violation_sites);
  }

  void AssignObjcMethodLookupOverrideConflictSymbols(Objc3MethodDecl &method,
                                                     const std::string &lookup_owner_symbol,
                                                     const std::string &override_owner_symbol) {
    method.method_lookup_symbol = lookup_owner_symbol + "::" + BuildObjcMethodLookupSymbol(method);
    method.override_lookup_symbol = override_owner_symbol + "::" + BuildObjcMethodOverrideLookupSymbol(method);
    method.conflict_lookup_symbol = BuildObjcMethodConflictLookupSymbol(method);
  }

  void FinalizeObjcMethodLookupOverrideConflictPackets(
      const std::vector<Objc3MethodDecl> &methods,
      std::vector<std::string> &method_lookup_symbols_lexicographic,
      std::vector<std::string> &override_lookup_symbols_lexicographic,
      std::vector<std::string> &conflict_lookup_symbols_lexicographic) {
    method_lookup_symbols_lexicographic = BuildObjcMethodLookupSymbolsLexicographic(methods);
    override_lookup_symbols_lexicographic = BuildObjcMethodOverrideLookupSymbolsLexicographic(methods);
    conflict_lookup_symbols_lexicographic = BuildObjcMethodConflictLookupSymbolsLexicographic(methods);
  }

  void AssignObjcPropertySynthesisIvarBindingSymbols(Objc3PropertyDecl &property,
                                                     const std::string &synthesis_owner_symbol) {
    property.property_synthesis_symbol = synthesis_owner_symbol + "::" + BuildObjcPropertySynthesisSymbol(property);
    property.ivar_binding_symbol = synthesis_owner_symbol + "::" + BuildObjcIvarBindingSymbol(property);
  }

  void FinalizeObjcPropertySynthesisIvarBindingPackets(
      const std::vector<Objc3PropertyDecl> &properties,
      std::vector<std::string> &property_synthesis_symbols_lexicographic,
      std::vector<std::string> &ivar_binding_symbols_lexicographic) {
    property_synthesis_symbols_lexicographic = BuildObjcPropertySynthesisSymbolsLexicographic(properties);
    ivar_binding_symbols_lexicographic = BuildObjcIvarBindingSymbolsLexicographic(properties);
  }

  void ConsumeBracedBodyTail() {
    int depth = 1;
    while (depth > 0 && !At(TokenKind::Eof)) {
      if (Match(TokenKind::LBrace)) {
        ++depth;
        continue;
      }
      if (Match(TokenKind::RBrace)) {
        --depth;
        continue;
      }
      Advance();
    }
  }

  bool ParseObjcMethodParameterClause(FuncParam &param) {
    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P106", "missing '(' before Objective-C method parameter type"));
      return false;
    }
    if (!ParseParameterType(param)) {
      return false;
    }
    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P109", "missing ')' after Objective-C method parameter type"));
      return false;
    }
    const Token &name = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          MakeDiag(name.line, name.column, "O3P101", "invalid Objective-C method parameter identifier"));
      return false;
    }
    param.name = Previous().text;
    param.line = Previous().line;
    param.column = Previous().column;
    return true;
  }

  bool ParseObjcMethodDecl(Objc3MethodDecl &method, bool allow_body) {
    if (Match(TokenKind::Minus)) {
      method.is_class_method = false;
    } else if (Match(TokenKind::Plus)) {
      method.is_class_method = true;
    } else {
      return false;
    }
    const Token method_marker = Previous();
    method.line = method_marker.line;
    method.column = method_marker.column;

    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P106", "missing '(' after Objective-C method marker"));
      return false;
    }

    FunctionDecl synthetic_fn;
    if (!ParseFunctionReturnType(synthetic_fn)) {
      return false;
    }
    CopyMethodReturnTypeFromFunctionDecl(synthetic_fn, method);

    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P109", "missing ')' after Objective-C method return type"));
      return false;
    }

    const Token &selector_head = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          MakeDiag(selector_head.line, selector_head.column, "O3P101", "invalid Objective-C selector identifier"));
      return false;
    }

    Objc3MethodDecl::SelectorPiece head_piece;
    head_piece.keyword = Previous().text;
    head_piece.line = Previous().line;
    head_piece.column = Previous().column;
    if (Match(TokenKind::Colon)) {
      head_piece.has_parameter = true;
      FuncParam first_param;
      if (!ParseObjcMethodParameterClause(first_param)) {
        return false;
      }
      head_piece.parameter_name = first_param.name;
      method.params.push_back(std::move(first_param));
      method.selector_pieces.push_back(std::move(head_piece));

      while (At(TokenKind::Identifier) && (index_ + 1 < tokens_.size()) && tokens_[index_ + 1].kind == TokenKind::Colon) {
        const Token keyword = Advance();
        (void)Match(TokenKind::Colon);
        Objc3MethodDecl::SelectorPiece keyword_piece;
        keyword_piece.keyword = keyword.text;
        keyword_piece.has_parameter = true;
        keyword_piece.line = keyword.line;
        keyword_piece.column = keyword.column;

        FuncParam keyword_param;
        if (!ParseObjcMethodParameterClause(keyword_param)) {
          return false;
        }
        keyword_piece.parameter_name = keyword_param.name;
        method.params.push_back(std::move(keyword_param));
        method.selector_pieces.push_back(std::move(keyword_piece));
      }
    } else {
      method.selector_pieces.push_back(std::move(head_piece));
    }

    method.selector = BuildNormalizedObjcSelector(method.selector_pieces);
    method.selector_is_normalized = true;

    if (!ParseOptionalThrowsClause(method)) {
      return false;
    }

    if (Match(TokenKind::Semicolon)) {
      method.has_body = false;
      FinalizeThrowsDeclarationProfile(method);
      FinalizeResultLikeProfile(method);
      FinalizeNSErrorBridgingProfile(method);
      FinalizeAsyncContinuationProfile(method);
      FinalizeAwaitSuspensionProfile(method);
      FinalizeActorIsolationSendabilityProfile(method);
      FinalizeTaskRuntimeCancellationProfile(method);
      FinalizeConcurrencyReplayRaceGuardProfile(method);
      FinalizeUnsafePointerExtensionProfile(method);
      FinalizeInlineAsmIntrinsicGovernanceProfile(method);
      return true;
    }

    if (!allow_body) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104",
                                      "missing ';' after Objective-C interface method declaration"));
      return false;
    }

    if (!Match(TokenKind::LBrace)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P110",
                                      "missing '{' or ';' after Objective-C implementation method declaration"));
      return false;
    }
    method.has_body = true;
    ConsumeBracedBodyTail();
    FinalizeThrowsDeclarationProfile(method);
    FinalizeResultLikeProfile(method);
    FinalizeNSErrorBridgingProfile(method);
    FinalizeAsyncContinuationProfile(method);
    FinalizeAwaitSuspensionProfile(method);
    FinalizeActorIsolationSendabilityProfile(method);
    FinalizeTaskRuntimeCancellationProfile(method);
    FinalizeConcurrencyReplayRaceGuardProfile(method);
    FinalizeUnsafePointerExtensionProfile(method);
    FinalizeInlineAsmIntrinsicGovernanceProfile(method);
    return true;
  }

  std::string ParseObjcPropertyAttributeValueText() {
    std::string value_text;
    while (!At(TokenKind::Eof) && !At(TokenKind::Comma) && !At(TokenKind::RParen)) {
      value_text += Advance().text;
    }
    return value_text;
  }

  bool ParseObjcPropertyAttributes(std::vector<Objc3PropertyAttributeDecl> &attributes) {
    if (!Match(TokenKind::LParen)) {
      return true;
    }

    while (true) {
      const Token &name_token = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(
            MakeDiag(name_token.line, name_token.column, "O3P101", "invalid Objective-C @property attribute"));
        return false;
      }

      Objc3PropertyAttributeDecl attribute;
      attribute.name = Previous().text;
      attribute.line = Previous().line;
      attribute.column = Previous().column;
      if (Match(TokenKind::Equal)) {
        attribute.has_value = true;
        attribute.value = ParseObjcPropertyAttributeValueText();
        if (attribute.value.empty()) {
          const Token &token = Peek();
          diagnostics_.push_back(
              MakeDiag(token.line, token.column, "O3P100", "missing Objective-C @property attribute value"));
          return false;
        }
      }
      attributes.push_back(std::move(attribute));

      if (Match(TokenKind::Comma)) {
        continue;
      }
      if (Match(TokenKind::RParen)) {
        return true;
      }

      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P109", "missing ')' after Objective-C @property attribute list"));
      return false;
    }
  }

  void ApplyObjcPropertyAttributes(Objc3PropertyDecl &property) {
    for (const auto &attribute : property.attributes) {
      if (attribute.name == "readonly") {
        property.is_readonly = true;
      } else if (attribute.name == "readwrite") {
        property.is_readwrite = true;
      } else if (attribute.name == "atomic") {
        property.is_atomic = true;
      } else if (attribute.name == "nonatomic") {
        property.is_nonatomic = true;
      } else if (attribute.name == "copy") {
        property.is_copy = true;
      } else if (attribute.name == "strong") {
        property.is_strong = true;
      } else if (attribute.name == "weak") {
        property.is_weak = true;
      } else if (attribute.name == "unowned") {
        property.is_unowned = true;
      } else if (attribute.name == "assign") {
        property.is_assign = true;
      } else if (attribute.name == "getter") {
        property.has_getter = true;
        property.getter_selector = attribute.value;
      } else if (attribute.name == "setter") {
        property.has_setter = true;
        property.setter_selector = attribute.value;
      }
    }
  }

  bool ParseObjcPropertyDecl(Objc3PropertyDecl &property) {
    if (!Match(TokenKind::KwAtProperty)) {
      return false;
    }
    const Token property_marker = Previous();
    property.line = property_marker.line;
    property.column = property_marker.column;

    if (!ParseObjcPropertyAttributes(property.attributes)) {
      return false;
    }

    FuncParam property_type;
    if (!ParseParameterType(property_type)) {
      return false;
    }
    CopyPropertyTypeFromParam(property_type, property);

    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          MakeDiag(name_token.line, name_token.column, "O3P101", "invalid Objective-C @property identifier"));
      return false;
    }
    property.name = Previous().text;

    if (!Match(TokenKind::Semicolon)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P104", "missing ';' after Objective-C @property declaration"));
      return false;
    }

    ApplyObjcPropertyAttributes(property);
    const Objc3WeakUnownedLifetimeProfile property_lifetime_profile =
        BuildPropertyWeakUnownedLifetimeProfile(property);
    property.ownership_is_weak_reference = property_lifetime_profile.is_weak_reference;
    property.ownership_is_unowned_reference = property_lifetime_profile.is_unowned_reference;
    property.ownership_is_unowned_safe_reference = property_lifetime_profile.is_unowned_safe_reference;
    property.ownership_lifetime_profile = property_lifetime_profile.lifetime_profile;
    property.ownership_runtime_hook_profile = property_lifetime_profile.runtime_hook_profile;
    property.has_weak_unowned_conflict = property.is_weak && property.is_unowned;
    const Objc3ArcDiagnosticFixitProfile property_arc_diagnostic_profile =
        BuildArcDiagnosticFixitProfile(
            property.ownership_qualifier_spelling,
            false,
            true,
            property.has_weak_unowned_conflict);
    property.ownership_arc_diagnostic_candidate = property_arc_diagnostic_profile.diagnostic_candidate;
    property.ownership_arc_fixit_available = property_arc_diagnostic_profile.fixit_available;
    property.ownership_arc_diagnostic_profile = property_arc_diagnostic_profile.diagnostic_profile;
    property.ownership_arc_fixit_hint = property_arc_diagnostic_profile.fixit_hint;
    return true;
  }

  void SynchronizeObjcContainer() {
    while (!At(TokenKind::Eof)) {
      if (At(TokenKind::KwAtEnd) || At(TokenKind::Minus) || At(TokenKind::Plus) || At(TokenKind::KwAtProperty)) {
        return;
      }
      if (At(TokenKind::KwAtInterface) || At(TokenKind::KwAtImplementation) || At(TokenKind::KwAtProtocol) ||
          At(TokenKind::KwModule) || At(TokenKind::KwLet) || At(TokenKind::KwFn) || At(TokenKind::KwPure) ||
          At(TokenKind::KwExtern)) {
        return;
      }
      if (Match(TokenKind::Semicolon)) {
        return;
      }
      if (Match(TokenKind::LBrace)) {
        ConsumeBracedBodyTail();
        continue;
      }
      Advance();
    }
  }

  bool ParseObjcProtocolCompositionClause(std::vector<std::string> &protocols) {
    if (!Match(TokenKind::Less)) {
      return true;
    }

    while (true) {
      const Token &protocol_token = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(MakeDiag(protocol_token.line, protocol_token.column, "O3P101",
                                        "invalid Objective-C protocol composition identifier"));
        return false;
      }
      protocols.push_back(Previous().text);

      if (Match(TokenKind::Comma)) {
        continue;
      }

      if (Match(TokenKind::Greater)) {
        return true;
      }

      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P112", "missing '>' after Objective-C protocol composition list"));
      return false;
    }
  }

  bool ParseObjcCategoryClause(std::string &category_name, bool &has_category) {
    if (!Match(TokenKind::LParen)) {
      return true;
    }
    has_category = true;
    if (Match(TokenKind::Identifier)) {
      category_name = Previous().text;
    }
    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P109", "missing ')' after Objective-C category name"));
      return false;
    }
    return true;
  }

  std::unique_ptr<Objc3ProtocolDecl> ParseObjcProtocolDecl() {
    auto decl = std::make_unique<Objc3ProtocolDecl>();
    decl->line = Previous().line;
    decl->column = Previous().column;

    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          MakeDiag(name_token.line, name_token.column, "O3P101", "invalid Objective-C protocol identifier"));
      SynchronizeTopLevel();
      return nullptr;
    }
    decl->name = Previous().text;

    if (!ParseObjcProtocolCompositionClause(decl->inherited_protocols)) {
      SynchronizeObjcContainer();
    }
    decl->inherited_protocols_lexicographic =
        BuildProtocolSemanticLinkTargetsLexicographic(decl->inherited_protocols);
    decl->semantic_link_symbol = "protocol:" + decl->name;
    decl->scope_owner_symbol = BuildObjcContainerScopeOwner("protocol", decl->name, false, "");
    decl->scope_path_lexicographic =
        BuildScopePathLexicographic(decl->scope_owner_symbol, decl->semantic_link_symbol);

    if (Match(TokenKind::Semicolon)) {
      decl->is_forward_declaration = true;
      return decl;
    }

    while (!At(TokenKind::KwAtEnd) && !At(TokenKind::Eof)) {
      if (At(TokenKind::KwAtProperty)) {
        Objc3PropertyDecl property;
        if (ParseObjcPropertyDecl(property)) {
          property.scope_owner_symbol = decl->scope_owner_symbol;
          property.scope_path_symbol = decl->scope_owner_symbol + "::" + BuildObjcPropertyScopePathSymbol(property);
          decl->properties.push_back(std::move(property));
          continue;
        }
        SynchronizeObjcContainer();
        continue;
      }
      if (!(At(TokenKind::Minus) || At(TokenKind::Plus))) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P100", "unsupported token inside @protocol declaration"));
        SynchronizeObjcContainer();
        continue;
      }

      Objc3MethodDecl method;
      if (ParseObjcMethodDecl(method, false)) {
        method.scope_owner_symbol = decl->scope_owner_symbol;
        method.scope_path_symbol = decl->scope_owner_symbol + "::" + BuildObjcMethodScopePathSymbol(method);
        AssignObjcMethodLookupOverrideConflictSymbols(method, decl->semantic_link_symbol, decl->semantic_link_symbol);
        decl->methods.push_back(std::move(method));
        continue;
      }
      SynchronizeObjcContainer();
    }

    FinalizeObjcMethodLookupOverrideConflictPackets(decl->methods,
                                                    decl->method_lookup_symbols_lexicographic,
                                                    decl->override_lookup_symbols_lexicographic,
                                                    decl->conflict_lookup_symbols_lexicographic);

    if (!Match(TokenKind::KwAtEnd)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P111", "missing '@end' after @protocol"));
      SynchronizeTopLevel();
      return nullptr;
    }
    return decl;
  }

  std::unique_ptr<Objc3InterfaceDecl> ParseObjcInterfaceDecl() {
    auto decl = std::make_unique<Objc3InterfaceDecl>();
    decl->line = Previous().line;
    decl->column = Previous().column;

    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          MakeDiag(name_token.line, name_token.column, "O3P101", "invalid Objective-C interface identifier"));
      SynchronizeTopLevel();
      return nullptr;
    }
    decl->name = Previous().text;

    if (Match(TokenKind::Colon)) {
      const Token &super_token = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(
            MakeDiag(super_token.line, super_token.column, "O3P101", "invalid Objective-C superclass identifier"));
        SynchronizeObjcContainer();
      } else {
        decl->super_name = Previous().text;
      }
    }

    if (!ParseObjcCategoryClause(decl->category_name, decl->has_category)) {
      SynchronizeObjcContainer();
    }

    if (!ParseObjcProtocolCompositionClause(decl->adopted_protocols)) {
      SynchronizeObjcContainer();
    }
    decl->adopted_protocols_lexicographic = BuildProtocolSemanticLinkTargetsLexicographic(decl->adopted_protocols);
    decl->semantic_link_symbol =
        BuildObjcContainerScopeOwner("interface", decl->name, decl->has_category, decl->category_name);
    if (!decl->super_name.empty()) {
      decl->semantic_link_super_symbol = "interface:" + decl->super_name;
    }
    if (decl->has_category) {
      decl->semantic_link_category_symbol = BuildObjcCategorySemanticLinkSymbol(decl->name, decl->category_name);
    }
    decl->scope_owner_symbol =
        BuildObjcContainerScopeOwner("interface", decl->name, decl->has_category, decl->category_name);
    decl->scope_path_lexicographic =
        BuildScopePathLexicographic(decl->scope_owner_symbol, "interface:" + decl->name);
    if (!decl->super_name.empty()) {
      decl->scope_path_lexicographic.push_back("super:" + decl->super_name);
      std::sort(decl->scope_path_lexicographic.begin(), decl->scope_path_lexicographic.end());
      decl->scope_path_lexicographic.erase(
          std::unique(decl->scope_path_lexicographic.begin(), decl->scope_path_lexicographic.end()),
          decl->scope_path_lexicographic.end());
    }

    while (!At(TokenKind::KwAtEnd) && !At(TokenKind::Eof)) {
      if (At(TokenKind::KwAtProperty)) {
        Objc3PropertyDecl property;
        if (ParseObjcPropertyDecl(property)) {
          property.scope_owner_symbol = decl->scope_owner_symbol;
          property.scope_path_symbol = decl->scope_owner_symbol + "::" + BuildObjcPropertyScopePathSymbol(property);
          AssignObjcPropertySynthesisIvarBindingSymbols(property, decl->semantic_link_symbol);
          decl->properties.push_back(std::move(property));
          continue;
        }
        SynchronizeObjcContainer();
        continue;
      }
      if (!(At(TokenKind::Minus) || At(TokenKind::Plus))) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P100", "unsupported token inside @interface declaration"));
        SynchronizeObjcContainer();
        continue;
      }

      Objc3MethodDecl method;
      if (ParseObjcMethodDecl(method, false)) {
        method.scope_owner_symbol = decl->scope_owner_symbol;
        method.scope_path_symbol = decl->scope_owner_symbol + "::" + BuildObjcMethodScopePathSymbol(method);
        const std::string override_owner_symbol =
            decl->semantic_link_super_symbol.empty() ? decl->semantic_link_symbol : decl->semantic_link_super_symbol;
        AssignObjcMethodLookupOverrideConflictSymbols(method, decl->semantic_link_symbol, override_owner_symbol);
        decl->methods.push_back(std::move(method));
        continue;
      }
      SynchronizeObjcContainer();
    }

    FinalizeObjcPropertySynthesisIvarBindingPackets(decl->properties,
                                                    decl->property_synthesis_symbols_lexicographic,
                                                    decl->ivar_binding_symbols_lexicographic);
    FinalizeObjcMethodLookupOverrideConflictPackets(decl->methods,
                                                    decl->method_lookup_symbols_lexicographic,
                                                    decl->override_lookup_symbols_lexicographic,
                                                    decl->conflict_lookup_symbols_lexicographic);

    if (!Match(TokenKind::KwAtEnd)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P111", "missing '@end' after @interface"));
      SynchronizeTopLevel();
      return nullptr;
    }
    return decl;
  }

  std::unique_ptr<Objc3ImplementationDecl> ParseObjcImplementationDecl() {
    auto decl = std::make_unique<Objc3ImplementationDecl>();
    decl->line = Previous().line;
    decl->column = Previous().column;

    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(
          MakeDiag(name_token.line, name_token.column, "O3P101", "invalid Objective-C implementation identifier"));
      SynchronizeTopLevel();
      return nullptr;
    }
    decl->name = Previous().text;

    if (!ParseObjcCategoryClause(decl->category_name, decl->has_category)) {
      SynchronizeObjcContainer();
    }
    decl->semantic_link_symbol =
        BuildObjcContainerScopeOwner("implementation", decl->name, decl->has_category, decl->category_name);
    decl->semantic_link_interface_symbol = BuildObjcContainerScopeOwner("interface", decl->name, false, "");
    if (decl->has_category) {
      decl->semantic_link_category_symbol = BuildObjcCategorySemanticLinkSymbol(decl->name, decl->category_name);
    }
    decl->scope_owner_symbol =
        BuildObjcContainerScopeOwner("implementation", decl->name, decl->has_category, decl->category_name);
    decl->scope_path_lexicographic =
        BuildScopePathLexicographic(decl->scope_owner_symbol, "implementation:" + decl->name);

    while (!At(TokenKind::KwAtEnd) && !At(TokenKind::Eof)) {
      if (At(TokenKind::KwAtProperty)) {
        Objc3PropertyDecl property;
        if (ParseObjcPropertyDecl(property)) {
          property.scope_owner_symbol = decl->scope_owner_symbol;
          property.scope_path_symbol = decl->scope_owner_symbol + "::" + BuildObjcPropertyScopePathSymbol(property);
          decl->properties.push_back(std::move(property));
          continue;
        }
        SynchronizeObjcContainer();
        continue;
      }
      if (!(At(TokenKind::Minus) || At(TokenKind::Plus))) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P100", "unsupported token inside @implementation declaration"));
        SynchronizeObjcContainer();
        continue;
      }

      Objc3MethodDecl method;
      if (ParseObjcMethodDecl(method, true)) {
        method.scope_owner_symbol = decl->scope_owner_symbol;
        method.scope_path_symbol = decl->scope_owner_symbol + "::" + BuildObjcMethodScopePathSymbol(method);
        AssignObjcMethodLookupOverrideConflictSymbols(
            method, decl->semantic_link_symbol, decl->semantic_link_interface_symbol);
        decl->methods.push_back(std::move(method));
        continue;
      }
      SynchronizeObjcContainer();
    }

    FinalizeObjcMethodLookupOverrideConflictPackets(decl->methods,
                                                    decl->method_lookup_symbols_lexicographic,
                                                    decl->override_lookup_symbols_lexicographic,
                                                    decl->conflict_lookup_symbols_lexicographic);

    if (!Match(TokenKind::KwAtEnd)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P111", "missing '@end' after @implementation"));
      SynchronizeTopLevel();
      return nullptr;
    }
    return decl;
  }

  std::unique_ptr<FunctionDecl> ParseFunction() {
    auto fn = std::make_unique<FunctionDecl>();
    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message = qualifier.kind == TokenKind::KwPure
                                      ? "unexpected qualifier 'pure' after 'fn'"
                                      : "unexpected qualifier 'extern' after 'fn'";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      SynchronizeTopLevel();
      return nullptr;
    }

    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(MakeDiag(name_token.line, name_token.column, "O3P101",
                                      "invalid function identifier"));
      SynchronizeTopLevel();
      return nullptr;
    }
    fn->name = Previous().text;
    fn->line = Previous().line;
    fn->column = Previous().column;
    fn->scope_owner_symbol = "global";
    fn->scope_path_lexicographic =
        BuildScopePathLexicographic(fn->scope_owner_symbol, "function:" + fn->name);

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message = qualifier.kind == TokenKind::KwPure
                                      ? "unexpected qualifier 'pure' after function name"
                                      : "unexpected qualifier 'extern' after function name";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after function name"));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!ParseFunctionParameters(*fn)) {
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after parameters"));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message = qualifier.kind == TokenKind::KwPure
                                      ? "unexpected qualifier 'pure' after parameter list"
                                      : "unexpected qualifier 'extern' after parameter list";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      SynchronizeTopLevel();
      return nullptr;
    }

    bool has_return_annotation = false;
    if (!ParseOptionalThrowsClause(*fn)) {
      SynchronizeTopLevel();
      return nullptr;
    }

    if (Match(TokenKind::Minus)) {
      const Token arrow_start = Previous();
      if (!Match(TokenKind::Greater)) {
        diagnostics_.push_back(
            MakeDiag(arrow_start.line, arrow_start.column, "O3P114", "missing '>' in function return annotation"));
        SynchronizeFunctionTail();
        return nullptr;
      }
      if (!ParseFunctionReturnType(*fn)) {
        SynchronizeFunctionTail();
        return nullptr;
      }
      has_return_annotation = true;
    }

    if (!ParseOptionalThrowsClause(*fn)) {
      SynchronizeTopLevel();
      return nullptr;
    }

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message = qualifier.kind == TokenKind::KwPure
                                      ? "unexpected qualifier 'pure' after function return annotation"
                                      : "unexpected qualifier 'extern' after function return annotation";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (Match(TokenKind::Semicolon)) {
      fn->is_prototype = true;
      FinalizeThrowsDeclarationProfile(*fn, has_return_annotation);
      FinalizeResultLikeProfile(*fn);
      FinalizeNSErrorBridgingProfile(*fn);
      FinalizeAsyncContinuationProfile(*fn);
      FinalizeAwaitSuspensionProfile(*fn);
      FinalizeActorIsolationSendabilityProfile(*fn);
      FinalizeTaskRuntimeCancellationProfile(*fn);
      FinalizeConcurrencyReplayRaceGuardProfile(*fn);
      FinalizeUnsafePointerExtensionProfile(*fn);
      FinalizeInlineAsmIntrinsicGovernanceProfile(*fn);
      return fn;
    }

    if (!At(TokenKind::LBrace)) {
      const Token &token = Peek();
      if (At(TokenKind::KwModule) || At(TokenKind::KwLet) || At(TokenKind::KwFn) || At(TokenKind::KwPure) ||
          At(TokenKind::KwExtern) || At(TokenKind::KwAtInterface) || At(TokenKind::KwAtImplementation) ||
          At(TokenKind::KwAtProtocol) || At(TokenKind::KwAtProperty) ||
          At(TokenKind::Eof)) {
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P104", "missing ';' after function prototype declaration"));
      } else {
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P110", "missing '{' to start block"));
      }
      SynchronizeTopLevel();
      return nullptr;
    }

    fn->body = ParseBlock();
    if (block_failed_) {
      block_failed_ = false;
      SynchronizeTopLevel();
      return nullptr;
    }
    FinalizeThrowsDeclarationProfile(*fn, has_return_annotation);
    FinalizeResultLikeProfile(*fn);
    FinalizeNSErrorBridgingProfile(*fn);
    FinalizeAsyncContinuationProfile(*fn);
    FinalizeAwaitSuspensionProfile(*fn);
    FinalizeActorIsolationSendabilityProfile(*fn);
    FinalizeTaskRuntimeCancellationProfile(*fn);
    FinalizeConcurrencyReplayRaceGuardProfile(*fn);
    FinalizeUnsafePointerExtensionProfile(*fn);
    FinalizeInlineAsmIntrinsicGovernanceProfile(*fn);
    return fn;
  }

  bool ParseFunctionParameters(FunctionDecl &fn) {
    if (At(TokenKind::RParen)) {
      return true;
    }

    while (true) {
      if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
        const Token qualifier = Advance();
        const std::string message = qualifier.kind == TokenKind::KwPure
                                        ? "unexpected qualifier 'pure' in parameter identifier position"
                                        : "unexpected qualifier 'extern' in parameter identifier position";
        diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
        return false;
      }

      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P101", "invalid parameter identifier"));
        return false;
      }

      FuncParam param;
      param.name = Advance().text;
      param.line = Previous().line;
      param.column = Previous().column;

      if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
        const Token qualifier = Advance();
        const std::string message = qualifier.kind == TokenKind::KwPure
                                        ? "unexpected qualifier 'pure' after parameter name"
                                        : "unexpected qualifier 'extern' after parameter name";
        diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
        return false;
      }

      if (!Match(TokenKind::Colon)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P107", "missing ':' after parameter name"));
        return false;
      }
      if (!ParseParameterType(param)) {
        return false;
      }

      if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
        const Token qualifier = Advance();
        const std::string message = qualifier.kind == TokenKind::KwPure
                                        ? "unexpected qualifier 'pure' after parameter type annotation"
                                        : "unexpected qualifier 'extern' after parameter type annotation";
        diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
        return false;
      }

      fn.params.push_back(std::move(param));
      if (!Match(TokenKind::Comma)) {
        return true;
      }
    }
  }

  bool ParseFunctionReturnType(FunctionDecl &fn) {
    fn.return_id_spelling = false;
    fn.return_class_spelling = false;
    fn.return_sel_spelling = false;
    fn.return_instancetype_spelling = false;
    fn.return_object_pointer_type_spelling = false;
    fn.return_object_pointer_type_name.clear();
    fn.return_typecheck_family_symbol.clear();
    fn.return_vector_spelling = false;
    fn.return_vector_base_spelling.clear();
    fn.return_vector_lane_count = 1;
    fn.has_return_generic_suffix = false;
    fn.return_generic_suffix_terminated = true;
    fn.return_generic_suffix_text.clear();
    fn.return_generic_line = 1;
    fn.return_generic_column = 1;
    fn.return_lightweight_generic_constraint_profile_is_normalized = false;
    fn.return_lightweight_generic_constraint_profile.clear();
    fn.return_nullability_flow_profile_is_normalized = false;
    fn.return_nullability_flow_profile.clear();
    fn.return_protocol_qualified_object_type_profile_is_normalized = false;
    fn.return_protocol_qualified_object_type_profile.clear();
    fn.return_variance_bridge_cast_profile_is_normalized = false;
    fn.return_variance_bridge_cast_profile.clear();
    fn.return_generic_metadata_abi_profile_is_normalized = false;
    fn.return_generic_metadata_abi_profile.clear();
    fn.return_module_import_graph_profile_is_normalized = false;
    fn.return_module_import_graph_profile.clear();
    fn.return_namespace_collision_shadowing_profile_is_normalized = false;
    fn.return_namespace_collision_shadowing_profile.clear();
    fn.return_public_private_api_partition_profile_is_normalized = false;
    fn.return_public_private_api_partition_profile.clear();
    fn.return_incremental_module_cache_invalidation_profile_is_normalized = false;
    fn.return_incremental_module_cache_invalidation_profile.clear();
    fn.return_cross_module_conformance_profile_is_normalized = false;
    fn.return_cross_module_conformance_profile.clear();
    fn.has_return_pointer_declarator = false;
    fn.return_pointer_declarator_depth = 0;
    fn.return_pointer_declarator_tokens.clear();
    fn.return_nullability_suffix_tokens.clear();
    fn.has_return_ownership_qualifier = false;
    fn.return_ownership_qualifier_spelling.clear();
    fn.return_ownership_qualifier_symbol.clear();
    fn.return_ownership_qualifier_tokens.clear();
    fn.return_ownership_insert_retain = false;
    fn.return_ownership_insert_release = false;
    fn.return_ownership_insert_autorelease = false;
    fn.return_ownership_operation_profile.clear();
    fn.return_ownership_is_weak_reference = false;
    fn.return_ownership_is_unowned_reference = false;
    fn.return_ownership_is_unowned_safe_reference = false;
    fn.return_ownership_lifetime_profile.clear();
    fn.return_ownership_runtime_hook_profile.clear();
    fn.return_ownership_arc_diagnostic_candidate = false;
    fn.return_ownership_arc_fixit_available = false;
    fn.return_ownership_arc_diagnostic_profile.clear();
    fn.return_ownership_arc_fixit_hint.clear();

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message = qualifier.kind == TokenKind::KwPure
                                      ? "unexpected qualifier 'pure' in function return type annotation"
                                      : "unexpected qualifier 'extern' in function return type annotation";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      return false;
    }

    while (At(TokenKind::Identifier) && IsOwnershipQualifierSpelling(Peek().text)) {
      const Token qualifier = Advance();
      fn.has_return_ownership_qualifier = true;
      fn.return_ownership_qualifier_spelling = qualifier.text;
      fn.return_ownership_qualifier_tokens.push_back(
          MakeSemaTokenMetadata(Objc3SemaTokenKind::OwnershipQualifier, qualifier));
    }

    if (Match(TokenKind::KwI32)) {
      fn.return_type = ValueType::I32;
    } else if (Match(TokenKind::KwBool)) {
      fn.return_type = ValueType::Bool;
    } else if (Match(TokenKind::KwBOOL)) {
      fn.return_type = ValueType::Bool;
    } else if (Match(TokenKind::KwNSInteger) || Match(TokenKind::KwNSUInteger)) {
      fn.return_type = ValueType::I32;
    } else if (Match(TokenKind::KwVoid)) {
      fn.return_type = ValueType::Void;
    } else if (Match(TokenKind::KwId)) {
      fn.return_type = ValueType::I32;
      fn.return_id_spelling = true;
    } else if (Match(TokenKind::KwClass)) {
      fn.return_type = ValueType::I32;
      fn.return_class_spelling = true;
    } else if (Match(TokenKind::KwSEL)) {
      fn.return_type = ValueType::I32;
      fn.return_sel_spelling = true;
    } else if (Match(TokenKind::KwProtocol)) {
      fn.return_type = ValueType::I32;
    } else if (Match(TokenKind::KwInstancetype)) {
      fn.return_type = ValueType::I32;
      fn.return_instancetype_spelling = true;
    } else {
      if (At(TokenKind::Identifier)) {
        const Token type_token = Advance();
        ValueType vector_type = ValueType::Unknown;
        std::string vector_base_spelling;
        unsigned vector_lane_count = 1;
        if (TryParseVectorTypeSpelling(type_token, vector_type, vector_base_spelling, vector_lane_count)) {
          fn.return_type = vector_type;
          fn.return_vector_spelling = true;
          fn.return_vector_base_spelling = vector_base_spelling;
          fn.return_vector_lane_count = vector_lane_count;
        } else {
          fn.return_type = ValueType::I32;
          fn.return_object_pointer_type_spelling = true;
          fn.return_object_pointer_type_name = type_token.text;
        }
      } else {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P114",
                     "expected function return type 'i32', 'bool', 'BOOL', 'NSInteger', 'NSUInteger', 'void', 'id', "
                     "'Class', 'SEL', 'Protocol', 'instancetype', object pointer spelling, ownership qualifiers "
                     "'__strong/__weak/__autoreleasing/__unsafe_unretained', or vector forms "
                     "'i32x2/i32x4/i32x8/i32x16' and 'boolx2/boolx4/boolx8/boolx16'"));
        return false;
      }
    }

    fn.return_typecheck_family_symbol = BuildObjcTypecheckReturnFamilySymbol(fn);

    bool parsed_generic_suffix = false;
    while (true) {
      if (At(TokenKind::Less) && !parsed_generic_suffix) {
        Match(TokenKind::Less);
        const Token &open = Previous();
        fn.has_return_generic_suffix = true;
        fn.return_generic_suffix_terminated = false;
        fn.return_generic_line = open.line;
        fn.return_generic_column = open.column;
        fn.return_generic_suffix_text = "<";
        int depth = 1;
        while (depth > 0 && !At(TokenKind::Eof)) {
          if (Match(TokenKind::Less)) {
            fn.return_generic_suffix_text += "<";
            ++depth;
            continue;
          }
          if (Match(TokenKind::Greater)) {
            fn.return_generic_suffix_text += ">";
            --depth;
            if (depth == 0) {
              fn.return_generic_suffix_terminated = true;
            }
            continue;
          }
          fn.return_generic_suffix_text += Advance().text;
        }
        if (!fn.return_generic_suffix_terminated) {
          diagnostics_.push_back(
              MakeDiag(fn.return_generic_line, fn.return_generic_column, "O3P114",
                       "unterminated generic function return type suffix"));
          return false;
        }
        parsed_generic_suffix = true;
        continue;
      }

      if (Match(TokenKind::Star)) {
        fn.has_return_pointer_declarator = true;
        fn.return_pointer_declarator_depth += 1;
        fn.return_pointer_declarator_tokens.push_back(
            MakeSemaTokenMetadata(Objc3SemaTokenKind::PointerDeclarator, Previous()));
        continue;
      }

      if (At(TokenKind::Question) || At(TokenKind::Bang)) {
        fn.return_nullability_suffix_tokens.push_back(
            MakeSemaTokenMetadata(Objc3SemaTokenKind::NullabilitySuffix, Advance()));
        continue;
      }

      if (At(TokenKind::Identifier) && IsOwnershipQualifierSpelling(Peek().text)) {
        const Token qualifier = Advance();
        fn.has_return_ownership_qualifier = true;
        fn.return_ownership_qualifier_spelling = qualifier.text;
        fn.return_ownership_qualifier_tokens.push_back(
            MakeSemaTokenMetadata(Objc3SemaTokenKind::OwnershipQualifier, qualifier));
        continue;
      }

      break;
    }

    fn.return_ownership_qualifier_symbol =
        BuildOwnershipQualifierSymbol(fn.return_ownership_qualifier_spelling, true);
    const Objc3OwnershipOperationProfile return_ownership_profile =
        BuildReturnOwnershipOperationProfile(fn.return_ownership_qualifier_spelling);
    fn.return_ownership_insert_retain = return_ownership_profile.insert_retain;
    fn.return_ownership_insert_release = return_ownership_profile.insert_release;
    fn.return_ownership_insert_autorelease = return_ownership_profile.insert_autorelease;
    fn.return_ownership_operation_profile = return_ownership_profile.profile;
    const Objc3WeakUnownedLifetimeProfile return_lifetime_profile =
        BuildWeakUnownedLifetimeProfile(fn.return_ownership_qualifier_spelling, false);
    fn.return_ownership_is_weak_reference = return_lifetime_profile.is_weak_reference;
    fn.return_ownership_is_unowned_reference = return_lifetime_profile.is_unowned_reference;
    fn.return_ownership_is_unowned_safe_reference = return_lifetime_profile.is_unowned_safe_reference;
    fn.return_ownership_lifetime_profile = return_lifetime_profile.lifetime_profile;
    fn.return_ownership_runtime_hook_profile = return_lifetime_profile.runtime_hook_profile;
    const Objc3ArcDiagnosticFixitProfile return_arc_diagnostic_profile =
        BuildArcDiagnosticFixitProfile(fn.return_ownership_qualifier_spelling, true, false, false);
    fn.return_ownership_arc_diagnostic_candidate = return_arc_diagnostic_profile.diagnostic_candidate;
    fn.return_ownership_arc_fixit_available = return_arc_diagnostic_profile.fixit_available;
    fn.return_ownership_arc_diagnostic_profile = return_arc_diagnostic_profile.diagnostic_profile;
    fn.return_ownership_arc_fixit_hint = return_arc_diagnostic_profile.fixit_hint;
    fn.return_lightweight_generic_constraint_profile =
        BuildLightweightGenericConstraintProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text);
    fn.return_lightweight_generic_constraint_profile_is_normalized =
        IsLightweightGenericConstraintProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated);
    fn.return_nullability_flow_profile =
        BuildNullabilityFlowProfile(
            fn.return_object_pointer_type_spelling,
            fn.return_nullability_suffix_tokens.size(),
            fn.has_return_pointer_declarator,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated);
    fn.return_nullability_flow_profile_is_normalized =
        IsNullabilityFlowProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.return_nullability_suffix_tokens.size());
    fn.return_protocol_qualified_object_type_profile =
        BuildProtocolQualifiedObjectTypeProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text);
    fn.return_protocol_qualified_object_type_profile_is_normalized =
        IsProtocolQualifiedObjectTypeProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated);
    fn.return_variance_bridge_cast_profile =
        BuildVarianceBridgeCastProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_ownership_qualifier_spelling);
    fn.return_variance_bridge_cast_profile_is_normalized =
        IsVarianceBridgeCastProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.return_generic_suffix_text,
            fn.return_ownership_qualifier_spelling);
    fn.return_generic_metadata_abi_profile =
        BuildGenericMetadataAbiProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_ownership_qualifier_spelling);
    fn.return_generic_metadata_abi_profile_is_normalized =
        IsGenericMetadataAbiProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text);
    fn.return_module_import_graph_profile =
        BuildModuleImportGraphProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_module_import_graph_profile_is_normalized =
        IsModuleImportGraphProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.return_generic_suffix_text);
    fn.return_namespace_collision_shadowing_profile =
        BuildNamespaceCollisionShadowingProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_namespace_collision_shadowing_profile_is_normalized =
        IsNamespaceCollisionShadowingProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_public_private_api_partition_profile =
        BuildPublicPrivateApiPartitionProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_public_private_api_partition_profile_is_normalized =
        IsPublicPrivateApiPartitionProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_incremental_module_cache_invalidation_profile =
        BuildIncrementalModuleCacheInvalidationProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_incremental_module_cache_invalidation_profile_is_normalized =
        IsIncrementalModuleCacheInvalidationProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_cross_module_conformance_profile =
        BuildCrossModuleConformanceProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_cross_module_conformance_profile_is_normalized =
        IsCrossModuleConformanceProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);

    return true;
  }

  bool ParseParameterType(FuncParam &param) {
    param.vector_spelling = false;
    param.vector_base_spelling.clear();
    param.vector_lane_count = 1;
    param.id_spelling = false;
    param.class_spelling = false;
    param.sel_spelling = false;
    param.instancetype_spelling = false;
    param.object_pointer_type_spelling = false;
    param.object_pointer_type_name.clear();
    param.typecheck_family_symbol.clear();
    param.has_generic_suffix = false;
    param.generic_suffix_terminated = true;
    param.generic_suffix_text.clear();
    param.generic_line = 1;
    param.generic_column = 1;
    param.lightweight_generic_constraint_profile_is_normalized = false;
    param.lightweight_generic_constraint_profile.clear();
    param.nullability_flow_profile_is_normalized = false;
    param.nullability_flow_profile.clear();
    param.protocol_qualified_object_type_profile_is_normalized = false;
    param.protocol_qualified_object_type_profile.clear();
    param.variance_bridge_cast_profile_is_normalized = false;
    param.variance_bridge_cast_profile.clear();
    param.generic_metadata_abi_profile_is_normalized = false;
    param.generic_metadata_abi_profile.clear();
    param.module_import_graph_profile_is_normalized = false;
    param.module_import_graph_profile.clear();
    param.namespace_collision_shadowing_profile_is_normalized = false;
    param.namespace_collision_shadowing_profile.clear();
    param.public_private_api_partition_profile_is_normalized = false;
    param.public_private_api_partition_profile.clear();
    param.incremental_module_cache_invalidation_profile_is_normalized = false;
    param.incremental_module_cache_invalidation_profile.clear();
    param.cross_module_conformance_profile_is_normalized = false;
    param.cross_module_conformance_profile.clear();
    param.has_pointer_declarator = false;
    param.pointer_declarator_depth = 0;
    param.pointer_declarator_tokens.clear();
    param.nullability_suffix_tokens.clear();
    param.has_ownership_qualifier = false;
    param.ownership_qualifier_spelling.clear();
    param.ownership_qualifier_symbol.clear();
    param.ownership_qualifier_tokens.clear();
    param.ownership_insert_retain = false;
    param.ownership_insert_release = false;
    param.ownership_insert_autorelease = false;
    param.ownership_operation_profile.clear();
    param.ownership_is_weak_reference = false;
    param.ownership_is_unowned_reference = false;
    param.ownership_is_unowned_safe_reference = false;
    param.ownership_lifetime_profile.clear();
    param.ownership_runtime_hook_profile.clear();
    param.ownership_arc_diagnostic_candidate = false;
    param.ownership_arc_fixit_available = false;
    param.ownership_arc_diagnostic_profile.clear();
    param.ownership_arc_fixit_hint.clear();
    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message = qualifier.kind == TokenKind::KwPure
                                      ? "unexpected qualifier 'pure' in parameter type annotation"
                                      : "unexpected qualifier 'extern' in parameter type annotation";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      return false;
    }

    while (At(TokenKind::Identifier) && IsOwnershipQualifierSpelling(Peek().text)) {
      const Token qualifier = Advance();
      param.has_ownership_qualifier = true;
      param.ownership_qualifier_spelling = qualifier.text;
      param.ownership_qualifier_tokens.push_back(
          MakeSemaTokenMetadata(Objc3SemaTokenKind::OwnershipQualifier, qualifier));
    }

    if (Match(TokenKind::KwI32)) {
      param.type = ValueType::I32;
    } else if (Match(TokenKind::KwBool)) {
      param.type = ValueType::Bool;
    } else if (Match(TokenKind::KwBOOL)) {
      param.type = ValueType::Bool;
    } else if (Match(TokenKind::KwNSInteger) || Match(TokenKind::KwNSUInteger)) {
      param.type = ValueType::I32;
    } else if (Match(TokenKind::KwId)) {
      param.type = ValueType::I32;
      param.id_spelling = true;
    } else if (Match(TokenKind::KwClass)) {
      param.type = ValueType::I32;
      param.class_spelling = true;
    } else if (Match(TokenKind::KwSEL)) {
      param.type = ValueType::I32;
      param.sel_spelling = true;
    } else if (Match(TokenKind::KwProtocol)) {
      param.type = ValueType::I32;
    } else if (Match(TokenKind::KwInstancetype)) {
      param.type = ValueType::I32;
      param.instancetype_spelling = true;
    } else if (At(TokenKind::Identifier)) {
      const Token type_token = Advance();
      ValueType vector_type = ValueType::Unknown;
      std::string vector_base_spelling;
      unsigned vector_lane_count = 1;
      if (TryParseVectorTypeSpelling(type_token, vector_type, vector_base_spelling, vector_lane_count)) {
        param.type = vector_type;
        param.vector_spelling = true;
        param.vector_base_spelling = vector_base_spelling;
        param.vector_lane_count = vector_lane_count;
        ParseParameterTypeSuffix(param);
        if (!param.generic_suffix_terminated) {
          return false;
        }
        param.cross_module_conformance_profile =
            BuildCrossModuleConformanceProfile(
                param.object_pointer_type_spelling,
                param.has_generic_suffix,
                param.generic_suffix_terminated,
                param.has_pointer_declarator,
                param.generic_suffix_text,
                param.object_pointer_type_name);
        param.cross_module_conformance_profile_is_normalized =
            IsCrossModuleConformanceProfileNormalized(
                param.object_pointer_type_spelling,
                param.has_generic_suffix,
                param.generic_suffix_terminated,
                param.has_pointer_declarator,
                param.generic_suffix_text,
                param.object_pointer_type_name);
        return true;
      }
      param.type = ValueType::I32;
      param.object_pointer_type_spelling = true;
      param.object_pointer_type_name = type_token.text;
    } else {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P108",
                                      "expected parameter type 'i32', 'bool', 'BOOL', 'NSInteger', 'NSUInteger', or "
                                      "'id', 'Class', 'SEL', 'Protocol', 'instancetype', object pointer spelling, "
                                      "ownership qualifiers '__strong/__weak/__autoreleasing/__unsafe_unretained', "
                                      "or vector forms "
                                      "'i32x2/i32x4/i32x8/i32x16' and 'boolx2/boolx4/boolx8/boolx16'"));
      return false;
    }

    param.typecheck_family_symbol = BuildObjcTypecheckParamFamilySymbol(param);

    ParseParameterTypeSuffix(param);
    if (!param.generic_suffix_terminated) {
      return false;
    }

    param.ownership_qualifier_symbol = BuildOwnershipQualifierSymbol(param.ownership_qualifier_spelling, false);
    const Objc3OwnershipOperationProfile param_ownership_profile =
        BuildParamOwnershipOperationProfile(param.ownership_qualifier_spelling);
    param.ownership_insert_retain = param_ownership_profile.insert_retain;
    param.ownership_insert_release = param_ownership_profile.insert_release;
    param.ownership_insert_autorelease = param_ownership_profile.insert_autorelease;
    param.ownership_operation_profile = param_ownership_profile.profile;
    const Objc3WeakUnownedLifetimeProfile param_lifetime_profile =
        BuildWeakUnownedLifetimeProfile(param.ownership_qualifier_spelling, false);
    param.ownership_is_weak_reference = param_lifetime_profile.is_weak_reference;
    param.ownership_is_unowned_reference = param_lifetime_profile.is_unowned_reference;
    param.ownership_is_unowned_safe_reference = param_lifetime_profile.is_unowned_safe_reference;
    param.ownership_lifetime_profile = param_lifetime_profile.lifetime_profile;
    param.ownership_runtime_hook_profile = param_lifetime_profile.runtime_hook_profile;
    const Objc3ArcDiagnosticFixitProfile param_arc_diagnostic_profile =
        BuildArcDiagnosticFixitProfile(param.ownership_qualifier_spelling, false, false, false);
    param.ownership_arc_diagnostic_candidate = param_arc_diagnostic_profile.diagnostic_candidate;
    param.ownership_arc_fixit_available = param_arc_diagnostic_profile.fixit_available;
    param.ownership_arc_diagnostic_profile = param_arc_diagnostic_profile.diagnostic_profile;
    param.ownership_arc_fixit_hint = param_arc_diagnostic_profile.fixit_hint;
    param.lightweight_generic_constraint_profile =
        BuildLightweightGenericConstraintProfile(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text);
    param.lightweight_generic_constraint_profile_is_normalized =
        IsLightweightGenericConstraintProfileNormalized(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated);
    param.nullability_flow_profile =
        BuildNullabilityFlowProfile(
            param.object_pointer_type_spelling,
            param.nullability_suffix_tokens.size(),
            param.has_pointer_declarator,
            param.has_generic_suffix,
            param.generic_suffix_terminated);
    param.nullability_flow_profile_is_normalized =
        IsNullabilityFlowProfileNormalized(
            param.object_pointer_type_spelling,
            param.nullability_suffix_tokens.size());
    param.protocol_qualified_object_type_profile =
        BuildProtocolQualifiedObjectTypeProfile(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text);
    param.protocol_qualified_object_type_profile_is_normalized =
        IsProtocolQualifiedObjectTypeProfileNormalized(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated);
    param.variance_bridge_cast_profile =
        BuildVarianceBridgeCastProfile(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text,
            param.ownership_qualifier_spelling);
    param.variance_bridge_cast_profile_is_normalized =
        IsVarianceBridgeCastProfileNormalized(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.generic_suffix_text,
            param.ownership_qualifier_spelling);
    param.generic_metadata_abi_profile =
        BuildGenericMetadataAbiProfile(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text,
            param.ownership_qualifier_spelling);
    param.generic_metadata_abi_profile_is_normalized =
        IsGenericMetadataAbiProfileNormalized(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text);
    param.module_import_graph_profile =
        BuildModuleImportGraphProfile(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text,
            param.object_pointer_type_name);
    param.module_import_graph_profile_is_normalized =
        IsModuleImportGraphProfileNormalized(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.generic_suffix_text);
    param.namespace_collision_shadowing_profile =
        BuildNamespaceCollisionShadowingProfile(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text,
            param.object_pointer_type_name);
    param.namespace_collision_shadowing_profile_is_normalized =
        IsNamespaceCollisionShadowingProfileNormalized(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.generic_suffix_text,
            param.object_pointer_type_name);
    param.public_private_api_partition_profile =
        BuildPublicPrivateApiPartitionProfile(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text,
            param.object_pointer_type_name);
    param.public_private_api_partition_profile_is_normalized =
        IsPublicPrivateApiPartitionProfileNormalized(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.generic_suffix_text,
            param.object_pointer_type_name);
    param.incremental_module_cache_invalidation_profile =
        BuildIncrementalModuleCacheInvalidationProfile(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text,
            param.object_pointer_type_name);
    param.incremental_module_cache_invalidation_profile_is_normalized =
        IsIncrementalModuleCacheInvalidationProfileNormalized(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text,
            param.object_pointer_type_name);
    param.cross_module_conformance_profile =
        BuildCrossModuleConformanceProfile(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text,
            param.object_pointer_type_name);
    param.cross_module_conformance_profile_is_normalized =
        IsCrossModuleConformanceProfileNormalized(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text,
            param.object_pointer_type_name);

    return true;
  }

  void ParseParameterTypeSuffix(FuncParam &param) {
    bool parsed_generic_suffix = false;
    while (true) {
      if (At(TokenKind::Less) && !parsed_generic_suffix) {
        Match(TokenKind::Less);
        const Token &open = Previous();
        param.has_generic_suffix = true;
        param.generic_suffix_terminated = false;
        param.generic_line = open.line;
        param.generic_column = open.column;
        param.generic_suffix_text = "<";
        int depth = 1;
        while (depth > 0 && !At(TokenKind::Eof)) {
          if (Match(TokenKind::Less)) {
            param.generic_suffix_text += "<";
            ++depth;
            continue;
          }
          if (Match(TokenKind::Greater)) {
            param.generic_suffix_text += ">";
            --depth;
            if (depth == 0) {
              param.generic_suffix_terminated = true;
            }
            continue;
          }
          param.generic_suffix_text += Advance().text;
        }
        if (!param.generic_suffix_terminated) {
          diagnostics_.push_back(MakeDiag(open.line, open.column, "O3P108",
                                          "unterminated generic parameter type suffix"));
          return;
        }
        parsed_generic_suffix = true;
        continue;
      }

      if (Match(TokenKind::Star)) {
        param.has_pointer_declarator = true;
        param.pointer_declarator_depth += 1;
        param.pointer_declarator_tokens.push_back(
            MakeSemaTokenMetadata(Objc3SemaTokenKind::PointerDeclarator, Previous()));
        continue;
      }

      if (At(TokenKind::Question) || At(TokenKind::Bang)) {
        param.nullability_suffix_tokens.push_back(
            MakeSemaTokenMetadata(Objc3SemaTokenKind::NullabilitySuffix, Advance()));
        continue;
      }

      if (At(TokenKind::Identifier) && IsOwnershipQualifierSpelling(Peek().text)) {
        const Token qualifier = Advance();
        param.has_ownership_qualifier = true;
        param.ownership_qualifier_spelling = qualifier.text;
        param.ownership_qualifier_tokens.push_back(
            MakeSemaTokenMetadata(Objc3SemaTokenKind::OwnershipQualifier, qualifier));
        continue;
      }

      break;
    }
  }

  std::vector<std::unique_ptr<Stmt>> ParseBlock() {
    std::vector<std::unique_ptr<Stmt>> body;
    if (!Match(TokenKind::LBrace)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P110", "missing '{' to start block"));
      block_failed_ = true;
      return {};
    }

    while (!At(TokenKind::RBrace) && !At(TokenKind::Eof)) {
      auto stmt = ParseStatement();
      if (stmt != nullptr) {
        body.push_back(std::move(stmt));
      } else {
        SynchronizeStatement();
      }
    }

    if (!Match(TokenKind::RBrace)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P111", "missing '}' to end block"));
      block_failed_ = true;
      return {};
    }

    return body;
  }

  std::vector<std::unique_ptr<Stmt>> ParseControlBody() {
    if (At(TokenKind::LBrace)) {
      return ParseBlock();
    }
    std::vector<std::unique_ptr<Stmt>> body;
    auto stmt = ParseStatement();
    if (stmt == nullptr) {
      block_failed_ = true;
      return {};
    }
    body.push_back(std::move(stmt));
    return body;
  }

  void SynchronizeTopLevel() {
    while (!At(TokenKind::Eof)) {
      if (Match(TokenKind::Semicolon)) {
        return;
      }
      if (At(TokenKind::KwModule) || At(TokenKind::KwLet) || At(TokenKind::KwFn) || At(TokenKind::KwPure) ||
          At(TokenKind::KwExtern) || At(TokenKind::KwAtInterface) || At(TokenKind::KwAtImplementation) ||
          At(TokenKind::KwAtProtocol) || At(TokenKind::KwAtProperty)) {
        return;
      }
      Advance();
    }
  }

  void SynchronizeFunctionTail() {
    if (At(TokenKind::LBrace)) {
      int depth = 0;
      while (!At(TokenKind::Eof)) {
        if (Match(TokenKind::LBrace)) {
          ++depth;
          continue;
        }
        if (Match(TokenKind::RBrace)) {
          --depth;
          if (depth <= 0) {
            return;
          }
          continue;
        }
        Advance();
      }
      return;
    }
    SynchronizeTopLevel();
  }

  void SynchronizeStatement() {
    while (!At(TokenKind::Eof)) {
      if (Match(TokenKind::Semicolon)) {
        return;
      }
      if (At(TokenKind::KwLet) || At(TokenKind::KwReturn) || At(TokenKind::KwIf) || At(TokenKind::KwDo) ||
          At(TokenKind::KwFor) || At(TokenKind::KwSwitch) || At(TokenKind::KwWhile) || At(TokenKind::KwBreak) ||
          At(TokenKind::KwContinue) || At(TokenKind::KwAtAutoreleasePool) || AtIdentifierAssignment() ||
          AtIdentifierUpdate() || AtPrefixUpdate() || At(TokenKind::RBrace)) {
        return;
      }
      Advance();
    }
  }

  std::unique_ptr<Stmt> ParseStatement() {
    if (At(TokenKind::LBrace)) {
      const Token open = Peek();
      auto body = ParseBlock();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Block;
      stmt->line = open.line;
      stmt->column = open.column;
      stmt->block_stmt = std::make_unique<BlockStmt>();
      stmt->block_stmt->line = open.line;
      stmt->block_stmt->column = open.column;
      stmt->block_stmt->body = std::move(body);
      return stmt;
    }

    if (Match(TokenKind::Semicolon)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Empty;
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      return stmt;
    }

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message = qualifier.kind == TokenKind::KwPure
                                      ? "unexpected qualifier 'pure' in statement position"
                                      : "unexpected qualifier 'extern' in statement position";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      return nullptr;
    }

    if (Match(TokenKind::KwAtAutoreleasePool)) {
      const Token marker = Previous();
      const unsigned scope_depth = autoreleasepool_scope_depth_ + 1u;
      ++autoreleasepool_scope_depth_;
      const unsigned scope_serial = ++autoreleasepool_scope_serial_;
      auto body = ParseBlock();
      --autoreleasepool_scope_depth_;
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }

      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Block;
      stmt->line = marker.line;
      stmt->column = marker.column;
      stmt->block_stmt = std::make_unique<BlockStmt>();
      stmt->block_stmt->line = marker.line;
      stmt->block_stmt->column = marker.column;
      stmt->block_stmt->body = std::move(body);
      stmt->block_stmt->is_autoreleasepool_scope = true;
      stmt->block_stmt->autoreleasepool_scope_depth = scope_depth;
      stmt->block_stmt->autoreleasepool_scope_symbol =
          BuildAutoreleasePoolScopeSymbol(scope_serial, scope_depth);
      return stmt;
    }

    if (Match(TokenKind::KwLet)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Let;
      stmt->let_stmt = std::make_unique<LetStmt>();
      const Token &name_token = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(MakeDiag(name_token.line, name_token.column, "O3P101",
                                        "invalid declaration identifier"));
        return nullptr;
      }
      stmt->let_stmt->name = Previous().text;
      stmt->let_stmt->line = Previous().line;
      stmt->let_stmt->column = Previous().column;
      stmt->line = Previous().line;
      stmt->column = Previous().column;

      if (!Match(TokenKind::Equal)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P102", "missing '='"));
        return nullptr;
      }

      stmt->let_stmt->value = ParseExpression();
      if (stmt->let_stmt->value == nullptr) {
        return nullptr;
      }

      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104",
                                        "missing ';' after declaration"));
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwReturn)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Return;
      stmt->return_stmt = std::make_unique<ReturnStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->return_stmt->line = Previous().line;
      stmt->return_stmt->column = Previous().column;
      if (Match(TokenKind::Semicolon)) {
        return stmt;
      }
      stmt->return_stmt->value = ParseExpression();
      if (stmt->return_stmt->value == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after return"));
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwIf)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::If;
      stmt->if_stmt = std::make_unique<IfStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->if_stmt->line = Previous().line;
      stmt->if_stmt->column = Previous().column;

      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after if"));
        return nullptr;
      }
      stmt->if_stmt->condition = ParseExpression();
      if (stmt->if_stmt->condition == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after if condition"));
        return nullptr;
      }

      stmt->if_stmt->then_body = ParseControlBody();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }
      if (Match(TokenKind::KwElse)) {
        stmt->if_stmt->else_body = ParseControlBody();
        if (block_failed_) {
          block_failed_ = false;
          return nullptr;
        }
      }
      return stmt;
    }

    if (Match(TokenKind::KwDo)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::DoWhile;
      stmt->do_while_stmt = std::make_unique<DoWhileStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->do_while_stmt->line = Previous().line;
      stmt->do_while_stmt->column = Previous().column;

      stmt->do_while_stmt->body = ParseControlBody();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }

      if (!Match(TokenKind::KwWhile)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P100", "missing 'while' after do block"));
        return nullptr;
      }
      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after while"));
        return nullptr;
      }
      stmt->do_while_stmt->condition = ParseExpression();
      if (stmt->do_while_stmt->condition == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after do-while condition"));
        return nullptr;
      }
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after do-while"));
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwFor)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::For;
      stmt->for_stmt = std::make_unique<ForStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->for_stmt->line = Previous().line;
      stmt->for_stmt->column = Previous().column;

      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after for"));
        return nullptr;
      }

      if (Match(TokenKind::Semicolon)) {
        stmt->for_stmt->init.kind = ForClause::Kind::None;
      } else {
        if (Match(TokenKind::KwLet)) {
          stmt->for_stmt->init.kind = ForClause::Kind::Let;
          const Token &name_token = Peek();
          if (!Match(TokenKind::Identifier)) {
            diagnostics_.push_back(MakeDiag(name_token.line, name_token.column, "O3P101",
                                            "invalid declaration identifier"));
            return nullptr;
          }
          stmt->for_stmt->init.name = Previous().text;
          stmt->for_stmt->init.line = Previous().line;
          stmt->for_stmt->init.column = Previous().column;

          if (!Match(TokenKind::Equal)) {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P102", "missing '='"));
            return nullptr;
          }

          stmt->for_stmt->init.value = ParseExpression();
          if (stmt->for_stmt->init.value == nullptr) {
            return nullptr;
          }
        } else if (AtIdentifierAssignment() || AtIdentifierUpdate()) {
          stmt->for_stmt->init.kind = ForClause::Kind::Assign;
          const Token name = Advance();
          std::string op = "=";
          if (!MatchAssignmentOperator(op)) {
            (void)MatchUpdateOperator(op);
          }
          stmt->for_stmt->init.name = name.text;
          stmt->for_stmt->init.op = op;
          stmt->for_stmt->init.line = name.line;
          stmt->for_stmt->init.column = name.column;
          if (op == "++" || op == "--") {
            stmt->for_stmt->init.value = nullptr;
          } else {
            stmt->for_stmt->init.value = ParseExpression();
            if (stmt->for_stmt->init.value == nullptr) {
              return nullptr;
            }
          }
        } else if (AtPrefixUpdate()) {
          stmt->for_stmt->init.kind = ForClause::Kind::Assign;
          std::string op = "++";
          (void)MatchUpdateOperator(op);
          const Token name = Peek();
          if (!Match(TokenKind::Identifier)) {
            diagnostics_.push_back(MakeDiag(name.line, name.column, "O3P101",
                                            "invalid assignment target"));
            return nullptr;
          }
          stmt->for_stmt->init.name = name.text;
          stmt->for_stmt->init.op = op;
          stmt->for_stmt->init.line = name.line;
          stmt->for_stmt->init.column = name.column;
          stmt->for_stmt->init.value = nullptr;
        } else {
          stmt->for_stmt->init.kind = ForClause::Kind::Expr;
          stmt->for_stmt->init.line = Peek().line;
          stmt->for_stmt->init.column = Peek().column;
          stmt->for_stmt->init.value = ParseExpression();
          if (stmt->for_stmt->init.value == nullptr) {
            return nullptr;
          }
        }
        if (!Match(TokenKind::Semicolon)) {
          const Token &token = Peek();
          diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after for init"));
          return nullptr;
        }
      }

      if (Match(TokenKind::Semicolon)) {
        stmt->for_stmt->condition = nullptr;
      } else {
        stmt->for_stmt->condition = ParseExpression();
        if (stmt->for_stmt->condition == nullptr) {
          return nullptr;
        }
        if (!Match(TokenKind::Semicolon)) {
          const Token &token = Peek();
          diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after for condition"));
          return nullptr;
        }
      }

      if (Match(TokenKind::RParen)) {
        stmt->for_stmt->step.kind = ForClause::Kind::None;
      } else {
        if (AtIdentifierAssignment() || AtIdentifierUpdate()) {
          stmt->for_stmt->step.kind = ForClause::Kind::Assign;
          const Token name = Advance();
          std::string op = "=";
          if (!MatchAssignmentOperator(op)) {
            (void)MatchUpdateOperator(op);
          }
          stmt->for_stmt->step.name = name.text;
          stmt->for_stmt->step.op = op;
          stmt->for_stmt->step.line = name.line;
          stmt->for_stmt->step.column = name.column;
          if (op == "++" || op == "--") {
            stmt->for_stmt->step.value = nullptr;
          } else {
            stmt->for_stmt->step.value = ParseExpression();
            if (stmt->for_stmt->step.value == nullptr) {
              return nullptr;
            }
          }
        } else if (AtPrefixUpdate()) {
          stmt->for_stmt->step.kind = ForClause::Kind::Assign;
          std::string op = "++";
          (void)MatchUpdateOperator(op);
          const Token name = Peek();
          if (!Match(TokenKind::Identifier)) {
            diagnostics_.push_back(MakeDiag(name.line, name.column, "O3P101",
                                            "invalid assignment target"));
            return nullptr;
          }
          stmt->for_stmt->step.name = name.text;
          stmt->for_stmt->step.op = op;
          stmt->for_stmt->step.line = name.line;
          stmt->for_stmt->step.column = name.column;
          stmt->for_stmt->step.value = nullptr;
        } else {
          stmt->for_stmt->step.kind = ForClause::Kind::Expr;
          stmt->for_stmt->step.line = Peek().line;
          stmt->for_stmt->step.column = Peek().column;
          stmt->for_stmt->step.value = ParseExpression();
          if (stmt->for_stmt->step.value == nullptr) {
            return nullptr;
          }
        }
        if (!Match(TokenKind::RParen)) {
          const Token &token = Peek();
          diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after for clauses"));
          return nullptr;
        }
      }

      stmt->for_stmt->body = ParseControlBody();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwSwitch)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Switch;
      stmt->switch_stmt = std::make_unique<SwitchStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->switch_stmt->line = Previous().line;
      stmt->switch_stmt->column = Previous().column;

      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after switch"));
        return nullptr;
      }
      stmt->switch_stmt->condition = ParseExpression();
      if (stmt->switch_stmt->condition == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after switch condition"));
        return nullptr;
      }
      if (!Match(TokenKind::LBrace)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P110", "missing '{' for switch body"));
        return nullptr;
      }

      while (!At(TokenKind::RBrace) && !At(TokenKind::Eof)) {
        if (Match(TokenKind::KwCase)) {
          SwitchCase case_stmt;
          case_stmt.line = Previous().line;
          case_stmt.column = Previous().column;
          case_stmt.is_default = false;

          if (Match(TokenKind::Number)) {
            case_stmt.value_line = Previous().line;
            case_stmt.value_column = Previous().column;
            case_stmt.value = std::atoi(Previous().text.c_str());
          } else if (Match(TokenKind::Minus) || Match(TokenKind::Plus)) {
            const Token sign = Previous();
            if (!Match(TokenKind::Number)) {
              diagnostics_.push_back(MakeDiag(sign.line, sign.column, "O3P103", "invalid case label expression"));
              return nullptr;
            }
            case_stmt.value_line = sign.line;
            case_stmt.value_column = sign.column;
            const int magnitude = std::atoi(Previous().text.c_str());
            case_stmt.value = sign.kind == TokenKind::Minus ? -magnitude : magnitude;
          } else if (Match(TokenKind::KwTrue) || Match(TokenKind::KwFalse)) {
            case_stmt.value_line = Previous().line;
            case_stmt.value_column = Previous().column;
            case_stmt.value = Previous().kind == TokenKind::KwTrue ? 1 : 0;
          } else if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
            const Token qualifier = Advance();
            const std::string message = qualifier.kind == TokenKind::KwPure
                                            ? "unexpected qualifier 'pure' in case label expression"
                                            : "unexpected qualifier 'extern' in case label expression";
            diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
            return nullptr;
          } else {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "invalid case label expression"));
            return nullptr;
          }

          if (!Match(TokenKind::Colon)) {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P107", "missing ':' after case label"));
            return nullptr;
          }

          while (!At(TokenKind::KwCase) && !At(TokenKind::KwDefault) && !At(TokenKind::RBrace) &&
                 !At(TokenKind::Eof)) {
            std::unique_ptr<Stmt> body_stmt = ParseStatement();
            if (body_stmt != nullptr) {
              case_stmt.body.push_back(std::move(body_stmt));
              continue;
            }
            SynchronizeStatement();
            if (At(TokenKind::Eof)) {
              break;
            }
          }
          stmt->switch_stmt->cases.push_back(std::move(case_stmt));
          continue;
        }

        if (Match(TokenKind::KwDefault)) {
          SwitchCase default_stmt;
          default_stmt.line = Previous().line;
          default_stmt.column = Previous().column;
          default_stmt.is_default = true;
          default_stmt.value = 0;
          default_stmt.value_line = Previous().line;
          default_stmt.value_column = Previous().column;

          if (!Match(TokenKind::Colon)) {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P107", "missing ':' after default"));
            return nullptr;
          }

          while (!At(TokenKind::KwCase) && !At(TokenKind::KwDefault) && !At(TokenKind::RBrace) &&
                 !At(TokenKind::Eof)) {
            std::unique_ptr<Stmt> body_stmt = ParseStatement();
            if (body_stmt != nullptr) {
              default_stmt.body.push_back(std::move(body_stmt));
              continue;
            }
            SynchronizeStatement();
            if (At(TokenKind::Eof)) {
              break;
            }
          }
          stmt->switch_stmt->cases.push_back(std::move(default_stmt));
          continue;
        }

        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P100", "expected 'case' or 'default' in switch body"));
        Advance();
      }

      if (!Match(TokenKind::RBrace)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P111", "missing '}' after switch body"));
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwWhile)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::While;
      stmt->while_stmt = std::make_unique<WhileStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->while_stmt->line = Previous().line;
      stmt->while_stmt->column = Previous().column;

      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after while"));
        return nullptr;
      }
      stmt->while_stmt->condition = ParseExpression();
      if (stmt->while_stmt->condition == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after while condition"));
        return nullptr;
      }

      stmt->while_stmt->body = ParseControlBody();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwBreak)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Break;
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after break"));
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwContinue)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Continue;
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after continue"));
        return nullptr;
      }
      return stmt;
    }

    if (AtIdentifierAssignment() || AtIdentifierUpdate()) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Assign;
      stmt->assign_stmt = std::make_unique<AssignStmt>();
      const Token name = Advance();
      std::string op = "=";
      if (!MatchAssignmentOperator(op)) {
        (void)MatchUpdateOperator(op);
      }
      stmt->line = name.line;
      stmt->column = name.column;
      stmt->assign_stmt->line = name.line;
      stmt->assign_stmt->column = name.column;
      stmt->assign_stmt->name = name.text;
      stmt->assign_stmt->op = op;
      if (op == "++" || op == "--") {
        stmt->assign_stmt->value = nullptr;
      } else {
        stmt->assign_stmt->value = ParseExpression();
        if (stmt->assign_stmt->value == nullptr) {
          return nullptr;
        }
      }
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after assignment"));
        return nullptr;
      }
      return stmt;
    }

    if (AtPrefixUpdate()) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Assign;
      stmt->assign_stmt = std::make_unique<AssignStmt>();
      std::string op = "++";
      const Token op_token = Peek();
      (void)MatchUpdateOperator(op);
      const Token name = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(MakeDiag(op_token.line, op_token.column, "O3P101", "invalid assignment target"));
        return nullptr;
      }
      stmt->line = name.line;
      stmt->column = name.column;
      stmt->assign_stmt->line = name.line;
      stmt->assign_stmt->column = name.column;
      stmt->assign_stmt->name = name.text;
      stmt->assign_stmt->op = op;
      stmt->assign_stmt->value = nullptr;
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after assignment"));
        return nullptr;
      }
      return stmt;
    }

    auto stmt = std::make_unique<Stmt>();
    stmt->kind = Stmt::Kind::Expr;
    stmt->expr_stmt = std::make_unique<ExprStmt>();
    stmt->line = Peek().line;
    stmt->column = Peek().column;
    stmt->expr_stmt->line = Peek().line;
    stmt->expr_stmt->column = Peek().column;
    stmt->expr_stmt->value = ParseExpression();
    if (stmt->expr_stmt->value == nullptr) {
      return nullptr;
    }
    if (!Match(TokenKind::Semicolon)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after expression"));
      return nullptr;
    }
    return stmt;
  }

  std::unique_ptr<Expr> ParseExpression() { return ParseConditional(); }

  std::unique_ptr<Expr> ParseConditional() {
    auto expr = ParseLogicalOr();
    if (expr == nullptr) {
      return nullptr;
    }
    if (!Match(TokenKind::Question)) {
      return expr;
    }

    const Token question = Previous();
    auto when_true = ParseExpression();
    if (when_true == nullptr) {
      return nullptr;
    }
    if (!Match(TokenKind::Colon)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P107", "missing ':' in conditional expression"));
      return nullptr;
    }
    auto when_false = ParseConditional();
    if (when_false == nullptr) {
      return nullptr;
    }

    auto node = std::make_unique<Expr>();
    node->kind = Expr::Kind::Conditional;
    node->line = question.line;
    node->column = question.column;
    node->left = std::move(expr);
    node->right = std::move(when_true);
    node->third = std::move(when_false);
    return node;
  }

  std::unique_ptr<Expr> ParseLogicalOr() {
    auto expr = ParseLogicalAnd();
    while (expr != nullptr && Match(TokenKind::OrOr)) {
      const Token op = Previous();
      auto rhs = ParseLogicalAnd();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseLogicalAnd() {
    auto expr = ParseBitwiseOr();
    while (expr != nullptr && Match(TokenKind::AndAnd)) {
      const Token op = Previous();
      auto rhs = ParseBitwiseOr();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseBitwiseOr() {
    auto expr = ParseBitwiseXor();
    while (expr != nullptr && Match(TokenKind::Pipe)) {
      const Token op = Previous();
      auto rhs = ParseBitwiseXor();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseBitwiseXor() {
    auto expr = ParseBitwiseAnd();
    while (expr != nullptr && Match(TokenKind::Caret)) {
      const Token op = Previous();
      auto rhs = ParseBitwiseAnd();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseBitwiseAnd() {
    auto expr = ParseEquality();
    while (expr != nullptr && Match(TokenKind::Ampersand)) {
      const Token op = Previous();
      auto rhs = ParseEquality();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseEquality() {
    auto expr = ParseRelational();
    while (expr != nullptr && (Match(TokenKind::EqualEqual) || Match(TokenKind::BangEqual))) {
      const Token op = Previous();
      auto rhs = ParseRelational();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseRelational() {
    auto expr = ParseShift();
    while (expr != nullptr &&
           (Match(TokenKind::Less) || Match(TokenKind::LessEqual) || Match(TokenKind::Greater) ||
            Match(TokenKind::GreaterEqual))) {
      const Token op = Previous();
      auto rhs = ParseShift();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseShift() {
    auto expr = ParseAdditive();
    while (expr != nullptr && (Match(TokenKind::LessLess) || Match(TokenKind::GreaterGreater))) {
      const Token op = Previous();
      auto rhs = ParseAdditive();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseAdditive() {
    auto expr = ParseMultiplicative();
    while (expr != nullptr && (At(TokenKind::Plus) || At(TokenKind::Minus))) {
      const Token op = Advance();
      auto rhs = ParseMultiplicative();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseMultiplicative() {
    auto expr = ParseUnary();
    while (expr != nullptr && (At(TokenKind::Star) || At(TokenKind::Slash) || At(TokenKind::Percent))) {
      const Token op = Advance();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseUnary() {
    if (Match(TokenKind::Bang)) {
      const Token op = Previous();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto zero = std::make_unique<Expr>();
      zero->kind = Expr::Kind::Number;
      zero->number = 0;
      zero->line = op.line;
      zero->column = op.column;

      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = "==";
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(rhs);
      node->right = std::move(zero);
      return node;
    }
    if (Match(TokenKind::Plus)) {
      const Token op = Previous();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto zero = std::make_unique<Expr>();
      zero->kind = Expr::Kind::Number;
      zero->number = 0;
      zero->line = op.line;
      zero->column = op.column;

      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = "+";
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(zero);
      node->right = std::move(rhs);
      return node;
    }
    if (Match(TokenKind::Minus)) {
      const Token op = Previous();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto zero = std::make_unique<Expr>();
      zero->kind = Expr::Kind::Number;
      zero->number = 0;
      zero->line = op.line;
      zero->column = op.column;

      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = "-";
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(zero);
      node->right = std::move(rhs);
      return node;
    }
    if (Match(TokenKind::Tilde)) {
      const Token op = Previous();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto minus_one = std::make_unique<Expr>();
      minus_one->kind = Expr::Kind::Number;
      minus_one->number = -1;
      minus_one->line = op.line;
      minus_one->column = op.column;

      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = "^";
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(rhs);
      node->right = std::move(minus_one);
      return node;
    }
    return ParsePostfix();
  }

  std::unique_ptr<Expr> ParsePostfix() {
    auto expr = ParsePrimary();
    while (expr != nullptr && Match(TokenKind::LParen)) {
      const unsigned callee_line = expr->line;
      const unsigned callee_column = expr->column;
      auto call = std::make_unique<Expr>();
      call->kind = Expr::Kind::Call;
      call->line = callee_line;
      call->column = callee_column;
      if (expr->kind != Expr::Kind::Identifier) {
        diagnostics_.push_back(MakeDiag(expr->line, expr->column, "O3P112", "call target must be identifier"));
        return nullptr;
      }
      call->ident = expr->ident;
      if (!At(TokenKind::RParen)) {
        while (true) {
          auto arg = ParseExpression();
          if (arg == nullptr) {
            return nullptr;
          }
          call->args.push_back(std::move(arg));
          if (!Match(TokenKind::Comma)) {
            break;
          }
        }
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after call"));
        return nullptr;
      }
      expr = std::move(call);
    }
    return expr;
  }

  void CollectBlockLiteralExprIdentifiers(const Expr *expr, std::vector<std::string> &used_identifiers) {
    if (expr == nullptr) {
      return;
    }
    switch (expr->kind) {
    case Expr::Kind::Identifier:
      if (!expr->ident.empty()) {
        used_identifiers.push_back(expr->ident);
      }
      return;
    case Expr::Kind::Binary:
      CollectBlockLiteralExprIdentifiers(expr->left.get(), used_identifiers);
      CollectBlockLiteralExprIdentifiers(expr->right.get(), used_identifiers);
      return;
    case Expr::Kind::Conditional:
      CollectBlockLiteralExprIdentifiers(expr->left.get(), used_identifiers);
      CollectBlockLiteralExprIdentifiers(expr->right.get(), used_identifiers);
      CollectBlockLiteralExprIdentifiers(expr->third.get(), used_identifiers);
      return;
    case Expr::Kind::Call:
      for (const auto &arg : expr->args) {
        CollectBlockLiteralExprIdentifiers(arg.get(), used_identifiers);
      }
      return;
    case Expr::Kind::MessageSend:
      CollectBlockLiteralExprIdentifiers(expr->receiver.get(), used_identifiers);
      for (const auto &arg : expr->args) {
        CollectBlockLiteralExprIdentifiers(arg.get(), used_identifiers);
      }
      return;
    case Expr::Kind::BlockLiteral:
      return;
    case Expr::Kind::Number:
    case Expr::Kind::BoolLiteral:
    case Expr::Kind::NilLiteral:
    default:
      return;
    }
  }

  void CollectBlockLiteralForClauseIdentifiers(const ForClause &clause,
                                               std::vector<std::string> &used_identifiers,
                                               std::vector<std::string> &declared_identifiers) {
    if (clause.kind == ForClause::Kind::Let && !clause.name.empty()) {
      declared_identifiers.push_back(clause.name);
    } else if ((clause.kind == ForClause::Kind::Assign || clause.kind == ForClause::Kind::Expr) &&
               !clause.name.empty()) {
      used_identifiers.push_back(clause.name);
    }
    CollectBlockLiteralExprIdentifiers(clause.value.get(), used_identifiers);
  }

  void CollectBlockLiteralStmtIdentifiers(const Stmt *stmt,
                                          std::vector<std::string> &used_identifiers,
                                          std::vector<std::string> &declared_identifiers) {
    if (stmt == nullptr) {
      return;
    }
    switch (stmt->kind) {
    case Stmt::Kind::Let:
      if (stmt->let_stmt != nullptr) {
        if (!stmt->let_stmt->name.empty()) {
          declared_identifiers.push_back(stmt->let_stmt->name);
        }
        CollectBlockLiteralExprIdentifiers(stmt->let_stmt->value.get(), used_identifiers);
      }
      return;
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt != nullptr) {
        if (!stmt->assign_stmt->name.empty()) {
          used_identifiers.push_back(stmt->assign_stmt->name);
        }
        CollectBlockLiteralExprIdentifiers(stmt->assign_stmt->value.get(), used_identifiers);
      }
      return;
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        CollectBlockLiteralExprIdentifiers(stmt->return_stmt->value.get(), used_identifiers);
      }
      return;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        CollectBlockLiteralExprIdentifiers(stmt->expr_stmt->value.get(), used_identifiers);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt != nullptr) {
        CollectBlockLiteralExprIdentifiers(stmt->if_stmt->condition.get(), used_identifiers);
        for (const auto &then_stmt : stmt->if_stmt->then_body) {
          CollectBlockLiteralStmtIdentifiers(then_stmt.get(), used_identifiers, declared_identifiers);
        }
        for (const auto &else_stmt : stmt->if_stmt->else_body) {
          CollectBlockLiteralStmtIdentifiers(else_stmt.get(), used_identifiers, declared_identifiers);
        }
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt != nullptr) {
        for (const auto &body_stmt : stmt->do_while_stmt->body) {
          CollectBlockLiteralStmtIdentifiers(body_stmt.get(), used_identifiers, declared_identifiers);
        }
        CollectBlockLiteralExprIdentifiers(stmt->do_while_stmt->condition.get(), used_identifiers);
      }
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt != nullptr) {
        CollectBlockLiteralForClauseIdentifiers(stmt->for_stmt->init, used_identifiers, declared_identifiers);
        CollectBlockLiteralExprIdentifiers(stmt->for_stmt->condition.get(), used_identifiers);
        CollectBlockLiteralForClauseIdentifiers(stmt->for_stmt->step, used_identifiers, declared_identifiers);
        for (const auto &body_stmt : stmt->for_stmt->body) {
          CollectBlockLiteralStmtIdentifiers(body_stmt.get(), used_identifiers, declared_identifiers);
        }
      }
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt != nullptr) {
        CollectBlockLiteralExprIdentifiers(stmt->switch_stmt->condition.get(), used_identifiers);
        for (const auto &switch_case : stmt->switch_stmt->cases) {
          for (const auto &case_stmt : switch_case.body) {
            CollectBlockLiteralStmtIdentifiers(case_stmt.get(), used_identifiers, declared_identifiers);
          }
        }
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt != nullptr) {
        CollectBlockLiteralExprIdentifiers(stmt->while_stmt->condition.get(), used_identifiers);
        for (const auto &body_stmt : stmt->while_stmt->body) {
          CollectBlockLiteralStmtIdentifiers(body_stmt.get(), used_identifiers, declared_identifiers);
        }
      }
      return;
    case Stmt::Kind::Block:
      if (stmt->block_stmt != nullptr) {
        for (const auto &body_stmt : stmt->block_stmt->body) {
          CollectBlockLiteralStmtIdentifiers(body_stmt.get(), used_identifiers, declared_identifiers);
        }
      }
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
    }
  }

  std::vector<std::string> BuildBlockLiteralCaptureSet(const std::vector<std::unique_ptr<Stmt>> &body,
                                                       const std::vector<std::string> &parameter_names,
                                                       bool &deterministic) {
    deterministic = true;
    std::unordered_set<std::string> parameter_name_set;
    for (const auto &name : parameter_names) {
      if (!parameter_name_set.insert(name).second) {
        deterministic = false;
      }
    }

    std::vector<std::string> used_identifiers;
    std::vector<std::string> declared_identifiers = parameter_names;
    for (const auto &stmt : body) {
      CollectBlockLiteralStmtIdentifiers(stmt.get(), used_identifiers, declared_identifiers);
    }

    std::unordered_set<std::string> declared_name_set;
    for (const auto &name : declared_identifiers) {
      if (!name.empty()) {
        declared_name_set.insert(name);
      }
    }

    std::vector<std::string> capture_names;
    capture_names.reserve(used_identifiers.size());
    for (const auto &used_name : used_identifiers) {
      if (used_name.empty() || declared_name_set.count(used_name) != 0u) {
        continue;
      }
      capture_names.push_back(used_name);
    }
    return BuildSortedUniqueStrings(std::move(capture_names));
  }

  std::unique_ptr<Expr> ParseBlockLiteralExpression() {
    const Token caret = Previous();
    auto block = std::make_unique<Expr>();
    block->kind = Expr::Kind::BlockLiteral;
    block->line = caret.line;
    block->column = caret.column;

    std::vector<std::string> parameter_names;
    if (Match(TokenKind::LParen)) {
      if (!At(TokenKind::RParen)) {
        while (true) {
          if (At(TokenKind::KwI32) || At(TokenKind::KwBool) || At(TokenKind::KwVoid)) {
            Advance();
          }
          if (!At(TokenKind::Identifier)) {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(
                token.line, token.column, "O3P166", "expected parameter identifier in block literal"));
            return nullptr;
          }
          parameter_names.push_back(Advance().text);
          if (!Match(TokenKind::Comma)) {
            break;
          }
        }
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109",
                                        "missing ')' after block literal parameter list"));
        return nullptr;
      }
    }

    if (!Match(TokenKind::LBrace)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P166", "expected '{' before block literal body"));
      return nullptr;
    }

    const auto body = ParseBlock();
    bool deterministic_capture_set = true;
    block->block_parameter_count = parameter_names.size();
    block->block_parameter_names_lexicographic = BuildSortedUniqueStrings(parameter_names);
    block->block_capture_names_lexicographic =
        BuildBlockLiteralCaptureSet(body, parameter_names, deterministic_capture_set);
    block->block_capture_count = block->block_capture_names_lexicographic.size();
    block->block_body_statement_count = body.size();
    block->block_capture_set_deterministic = deterministic_capture_set;
    block->block_capture_profile =
        BuildBlockLiteralCaptureProfile(block->block_capture_names_lexicographic);
    block->block_literal_is_normalized = true;
    block->block_abi_invoke_argument_slots = block->block_parameter_count;
    block->block_abi_capture_word_count = block->block_capture_count;
    block->block_abi_layout_profile = BuildBlockLiteralAbiLayoutProfile(
        block->block_parameter_count,
        block->block_capture_count,
        block->block_body_statement_count);
    block->block_abi_descriptor_symbol = BuildBlockLiteralAbiDescriptorSymbol(
        block->line,
        block->column,
        block->block_parameter_count,
        block->block_capture_count);
    block->block_invoke_trampoline_symbol = BuildBlockLiteralInvokeTrampolineSymbol(
        block->line,
        block->column,
        block->block_parameter_count,
        block->block_capture_count);
    block->block_abi_has_invoke_trampoline = true;
    block->block_abi_layout_is_normalized =
        block->block_literal_is_normalized && block->block_capture_set_deterministic;
    block->block_storage_mutable_capture_count = block->block_capture_count;
    block->block_storage_byref_slot_count = block->block_capture_count;
    block->block_storage_requires_byref_cells = block->block_storage_byref_slot_count > 0u;
    block->block_storage_escape_analysis_enabled = true;
    block->block_storage_escape_to_heap = block->block_storage_requires_byref_cells;
    block->block_storage_escape_profile =
        BuildBlockStorageEscapeProfile(
            block->block_storage_mutable_capture_count,
            block->block_storage_byref_slot_count,
            block->block_storage_escape_to_heap,
            block->block_body_statement_count);
    block->block_storage_byref_layout_symbol =
        BuildBlockStorageByrefLayoutSymbol(
            block->line,
            block->column,
            block->block_storage_mutable_capture_count,
            block->block_storage_byref_slot_count,
            block->block_storage_escape_to_heap);
    block->block_storage_escape_profile_is_normalized =
        block->block_literal_is_normalized && block->block_capture_set_deterministic;
    block->block_copy_helper_required = block->block_storage_mutable_capture_count > 0u;
    block->block_dispose_helper_required = block->block_storage_byref_slot_count > 0u;
    block->block_copy_dispose_profile =
        BuildBlockCopyDisposeProfile(
            block->block_storage_mutable_capture_count,
            block->block_storage_byref_slot_count,
            block->block_storage_escape_to_heap,
            block->block_body_statement_count);
    block->block_copy_helper_symbol =
        BuildBlockCopyHelperSymbol(
            block->line,
            block->column,
            block->block_storage_mutable_capture_count,
            block->block_storage_byref_slot_count,
            block->block_storage_escape_to_heap);
    block->block_dispose_helper_symbol =
        BuildBlockDisposeHelperSymbol(
            block->line,
            block->column,
            block->block_storage_mutable_capture_count,
            block->block_storage_byref_slot_count,
            block->block_storage_escape_to_heap);
    block->block_copy_dispose_profile_is_normalized =
        block->block_storage_escape_profile_is_normalized &&
        block->block_copy_helper_required == block->block_dispose_helper_required;
    block->block_determinism_perf_baseline_weight =
        BuildBlockDeterminismPerfBaselineWeight(
            block->block_parameter_count,
            block->block_capture_count,
            block->block_body_statement_count,
            block->block_copy_helper_required,
            block->block_dispose_helper_required);
    block->block_determinism_perf_baseline_profile =
        BuildBlockDeterminismPerfBaselineProfile(
            block->block_parameter_count,
            block->block_capture_count,
            block->block_body_statement_count,
            block->block_copy_helper_required,
            block->block_dispose_helper_required,
            block->block_capture_set_deterministic,
            block->block_copy_dispose_profile_is_normalized,
            block->block_determinism_perf_baseline_weight);
    block->block_determinism_perf_baseline_profile_is_normalized =
        block->block_copy_dispose_profile_is_normalized &&
        block->block_determinism_perf_baseline_weight >= block->block_capture_count;
    return block;
  }

  std::unique_ptr<Expr> ParsePrimary() {
    if (Match(TokenKind::Number)) {
      auto expr = std::make_unique<Expr>();
      expr->kind = Expr::Kind::Number;
      expr->line = Previous().line;
      expr->column = Previous().column;
      if (!ParseIntegerLiteralValue(Previous().text, expr->number)) {
        diagnostics_.push_back(MakeDiag(expr->line, expr->column, "O3P103",
                                        "invalid numeric literal '" + Previous().text + "'"));
        return nullptr;
      }
      return expr;
    }
    if (Match(TokenKind::KwTrue) || Match(TokenKind::KwFalse)) {
      auto expr = std::make_unique<Expr>();
      expr->kind = Expr::Kind::BoolLiteral;
      expr->line = Previous().line;
      expr->column = Previous().column;
      expr->bool_value = Previous().kind == TokenKind::KwTrue;
      return expr;
    }
    if (Match(TokenKind::KwNil)) {
      auto expr = std::make_unique<Expr>();
      expr->kind = Expr::Kind::NilLiteral;
      expr->line = Previous().line;
      expr->column = Previous().column;
      return expr;
    }
    if (Match(TokenKind::Caret)) {
      return ParseBlockLiteralExpression();
    }
    if (Match(TokenKind::Identifier)) {
      auto expr = std::make_unique<Expr>();
      expr->kind = Expr::Kind::Identifier;
      expr->line = Previous().line;
      expr->column = Previous().column;
      expr->ident = Previous().text;
      return expr;
    }
    if (Match(TokenKind::LParen)) {
      auto expr = ParseExpression();
      if (expr == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after expression"));
        return nullptr;
      }
      return expr;
    }

    if (Match(TokenKind::LBracket)) {
      return ParseMessageSendExpression();
    }

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message = qualifier.kind == TokenKind::KwPure
                                      ? "unexpected qualifier 'pure' in expression position"
                                      : "unexpected qualifier 'extern' in expression position";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      return nullptr;
    }

    const Token &token = Peek();
    diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "invalid expression"));
    return nullptr;
  }

  std::unique_ptr<Expr> ParseMessageSendExpression() {
    const Token open = Previous();
    auto message = std::make_unique<Expr>();
    message->kind = Expr::Kind::MessageSend;
    message->line = open.line;
    message->column = open.column;

    const std::size_t receiver_diag_count = diagnostics_.size();
    message->receiver = ParsePostfix();
    if (message->receiver == nullptr) {
      if (diagnostics_.size() == receiver_diag_count) {
        diagnostics_.push_back(MakeDiag(open.line, open.column, "O3P113",
                                        "invalid receiver expression in message send"));
      }
      return nullptr;
    }

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message_text =
          qualifier.kind == TokenKind::KwPure
              ? "unexpected qualifier 'pure' in message selector position"
              : "unexpected qualifier 'extern' in message selector position";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message_text));
      return nullptr;
    }

    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P113",
                                      "expected selector identifier in message send"));
      return nullptr;
    }

    const Token selector_head = Advance();
    message->selector = selector_head.text;
    Expr::MessageSendSelectorPiece head_piece;
    head_piece.keyword = selector_head.text;
    head_piece.line = selector_head.line;
    head_piece.column = selector_head.column;
    if (Match(TokenKind::Colon)) {
      message->message_send_form = Expr::MessageSendForm::Keyword;
      head_piece.has_argument = true;
      message->selector_lowering_pieces.push_back(head_piece);
      message->selector += ":";
      auto first_arg = ParseExpression();
      if (first_arg == nullptr) {
        return nullptr;
      }
      message->args.push_back(std::move(first_arg));

      while (true) {
        if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
          const Token qualifier = Advance();
          const std::string message_text =
              qualifier.kind == TokenKind::KwPure
                  ? "unexpected qualifier 'pure' in keyword selector segment position"
                  : "unexpected qualifier 'extern' in keyword selector segment position";
          diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message_text));
          return nullptr;
        }
        if (!At(TokenKind::Identifier)) {
          break;
        }
        const Token keyword = Advance();
        if (!Match(TokenKind::Colon)) {
          diagnostics_.push_back(MakeDiag(keyword.line, keyword.column, "O3P113",
                                          "missing ':' in keyword selector segment"));
          return nullptr;
        }
        Expr::MessageSendSelectorPiece keyword_piece;
        keyword_piece.keyword = keyword.text;
        keyword_piece.has_argument = true;
        keyword_piece.line = keyword.line;
        keyword_piece.column = keyword.column;
        message->selector_lowering_pieces.push_back(std::move(keyword_piece));
        message->selector += keyword.text;
        message->selector += ":";
        auto arg = ParseExpression();
        if (arg == nullptr) {
          return nullptr;
        }
        message->args.push_back(std::move(arg));
      }
    } else {
      message->message_send_form = Expr::MessageSendForm::Unary;
      message->selector_lowering_pieces.push_back(head_piece);
    }
    message->message_send_form_symbol = BuildMessageSendFormSymbol(message->message_send_form);
    message->selector_lowering_symbol = BuildMessageSendSelectorLoweringSymbol(message->selector_lowering_pieces);
    message->selector_lowering_is_normalized = true;
    message->dispatch_abi_receiver_slots_marshaled = 1u;
    message->dispatch_abi_selector_slots_marshaled = 1u;
    message->dispatch_abi_argument_value_slots_marshaled = static_cast<unsigned>(message->args.size());
    message->dispatch_abi_runtime_arg_slots = kDispatchAbiMarshallingRuntimeArgSlots;
    message->dispatch_abi_argument_padding_slots_marshaled = ComputeDispatchAbiArgumentPaddingSlots(
        message->args.size(), message->dispatch_abi_runtime_arg_slots);
    message->dispatch_abi_argument_total_slots_marshaled = message->dispatch_abi_argument_value_slots_marshaled +
                                                           message->dispatch_abi_argument_padding_slots_marshaled;
    message->dispatch_abi_total_slots_marshaled = message->dispatch_abi_receiver_slots_marshaled +
                                                  message->dispatch_abi_selector_slots_marshaled +
                                                  message->dispatch_abi_argument_total_slots_marshaled;
    message->dispatch_abi_marshalling_symbol = BuildDispatchAbiMarshallingSymbol(
        message->dispatch_abi_receiver_slots_marshaled, message->dispatch_abi_selector_slots_marshaled,
        message->dispatch_abi_argument_value_slots_marshaled, message->dispatch_abi_argument_padding_slots_marshaled,
        message->dispatch_abi_argument_total_slots_marshaled, message->dispatch_abi_total_slots_marshaled,
        message->dispatch_abi_runtime_arg_slots);
    message->dispatch_abi_marshalling_is_normalized = true;
    message->nil_receiver_semantics_enabled = message->receiver->kind == Expr::Kind::NilLiteral;
    message->nil_receiver_foldable = message->nil_receiver_semantics_enabled;
    message->nil_receiver_requires_runtime_dispatch = !message->nil_receiver_foldable;
    message->nil_receiver_folding_symbol = BuildNilReceiverFoldingSymbol(
        message->nil_receiver_foldable, message->nil_receiver_requires_runtime_dispatch, message->message_send_form);
    message->nil_receiver_semantics_is_normalized = true;
    message->super_dispatch_enabled = IsSuperDispatchReceiver(*message->receiver);
    message->super_dispatch_requires_class_context = message->super_dispatch_enabled;
    message->super_dispatch_symbol = BuildSuperDispatchSymbol(
        message->super_dispatch_enabled, message->super_dispatch_requires_class_context, message->message_send_form);
    message->super_dispatch_semantics_is_normalized = true;
    message->method_family_name = ClassifyMethodFamilyFromSelector(message->selector);
    message->method_family_returns_retained_result = message->method_family_name == "init" ||
                                                     message->method_family_name == "copy" ||
                                                     message->method_family_name == "mutableCopy" ||
                                                     message->method_family_name == "new";
    message->method_family_returns_related_result = message->method_family_name == "init";
    message->method_family_semantics_symbol = BuildMethodFamilySemanticsSymbol(
        message->method_family_name, message->method_family_returns_retained_result,
        message->method_family_returns_related_result);
    message->method_family_semantics_is_normalized = true;
    message->runtime_shim_host_link_required = message->nil_receiver_requires_runtime_dispatch;
    message->runtime_shim_host_link_elided = !message->runtime_shim_host_link_required;
    message->runtime_shim_host_link_declaration_parameter_count = message->dispatch_abi_runtime_arg_slots + 2u;
    message->runtime_dispatch_bridge_symbol = kRuntimeShimHostLinkDispatchSymbol;
    message->runtime_shim_host_link_symbol = BuildRuntimeShimHostLinkSymbol(
        message->runtime_shim_host_link_required, message->runtime_shim_host_link_elided,
        message->dispatch_abi_runtime_arg_slots, message->runtime_shim_host_link_declaration_parameter_count,
        message->runtime_dispatch_bridge_symbol, message->message_send_form);
    message->runtime_shim_host_link_is_normalized = true;

    if (!Match(TokenKind::RBracket)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P113",
                                      "missing ']' after message send expression"));
      return nullptr;
    }
    return message;
  }

  const std::vector<Token> &tokens_;
  std::size_t index_ = 0;
  std::vector<std::string> diagnostics_;
  bool saw_module_declaration_ = false;
  bool block_failed_ = false;
  unsigned autoreleasepool_scope_depth_ = 0;
  unsigned autoreleasepool_scope_serial_ = 0;
  Objc3AstBuilder ast_builder_;
};

}  // namespace

Objc3ParseResult ParseObjc3Program(const Objc3LexTokenStream &tokens) {
  Objc3Parser parser(tokens);
  Objc3ParseResult result;
  result.program = parser.Parse();
  result.diagnostics = parser.TakeDiagnostics();
  return result;
}
