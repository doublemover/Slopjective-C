#include "parse/objc3_parser_unsafe_pointer_profiles.h"

#include <memory>
#include <sstream>
#include <vector>

namespace objc3c::parse {
namespace {

bool IsUnsafeOwnershipQualifierSpelling(const std::string &spelling) {
  return spelling == "__unsafe_unretained";
}

std::size_t CountRawPointerTypeSites(
    const std::vector<FuncParam> &params,
    bool has_return_pointer_declarator) {
  std::size_t sites = has_return_pointer_declarator ? 1u : 0u;
  for (const auto &param : params) {
    if (param.has_pointer_declarator) {
      sites += 1u;
    }
  }
  return sites;
}

std::size_t CountUnsafeKeywordSites(
    const std::vector<FuncParam> &params,
    const std::string &return_ownership_qualifier_spelling) {
  std::size_t sites =
      IsUnsafeOwnershipQualifierSpelling(return_ownership_qualifier_spelling)
          ? 1u
          : 0u;
  for (const auto &param : params) {
    if (IsUnsafeOwnershipQualifierSpelling(
            param.ownership_qualifier_spelling)) {
      sites += 1u;
    }
  }
  return sites;
}

#include "parse/objc3_parser_unsafe_pointer_arithmetic_sites.inc"

Objc3UnsafePointerExtensionProfile BuildUnsafePointerExtensionProfileFromCounts(
    std::size_t unsafe_keyword_sites,
    std::size_t pointer_arithmetic_sites,
    std::size_t raw_pointer_type_sites) {
  Objc3UnsafePointerExtensionProfile profile;
  profile.unsafe_keyword_sites = unsafe_keyword_sites;
  profile.pointer_arithmetic_sites = pointer_arithmetic_sites;
  profile.raw_pointer_type_sites = raw_pointer_type_sites;
  profile.unsafe_operation_sites =
      pointer_arithmetic_sites + raw_pointer_type_sites;
  profile.unsafe_pointer_extension_sites =
      profile.unsafe_keyword_sites + profile.pointer_arithmetic_sites +
      profile.raw_pointer_type_sites;

  const bool gate_open = unsafe_keyword_sites > 0u;
  profile.gate_blocked_sites =
      gate_open ? 0u
                : (profile.pointer_arithmetic_sites +
                   profile.raw_pointer_type_sites);
  profile.normalized_sites =
      profile.unsafe_pointer_extension_sites - profile.gate_blocked_sites;

  if (profile.unsafe_keyword_sites > profile.unsafe_pointer_extension_sites ||
      profile.pointer_arithmetic_sites >
          profile.unsafe_pointer_extension_sites ||
      profile.raw_pointer_type_sites > profile.unsafe_pointer_extension_sites ||
      profile.unsafe_operation_sites >
          profile.unsafe_pointer_extension_sites ||
      profile.normalized_sites > profile.unsafe_pointer_extension_sites ||
      profile.gate_blocked_sites > profile.unsafe_pointer_extension_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.gate_blocked_sites !=
      profile.unsafe_pointer_extension_sites) {
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

}  // namespace

std::string BuildUnsafePointerExtensionProfile(
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

bool IsUnsafePointerExtensionProfileNormalized(
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

Objc3UnsafePointerExtensionProfile BuildUnsafePointerExtensionProfileFromFunction(
    const FunctionDecl &fn) {
  const std::size_t unsafe_keyword_sites =
      CountUnsafeKeywordSites(fn.params,
                              fn.return_ownership_qualifier_spelling);
  const std::size_t raw_pointer_type_sites =
      CountRawPointerTypeSites(fn.params, fn.has_return_pointer_declarator);
  const std::size_t pointer_arithmetic_sites =
      CountPointerArithmeticSitesInBody(fn.body);
  return BuildUnsafePointerExtensionProfileFromCounts(
      unsafe_keyword_sites, pointer_arithmetic_sites, raw_pointer_type_sites);
}

Objc3UnsafePointerExtensionProfile BuildUnsafePointerExtensionProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  const std::size_t unsafe_keyword_sites =
      CountUnsafeKeywordSites(method.params,
                              method.return_ownership_qualifier_spelling);
  const std::size_t raw_pointer_type_sites = CountRawPointerTypeSites(
      method.params, method.has_return_pointer_declarator);
  const std::size_t pointer_arithmetic_sites =
      method.has_body && raw_pointer_type_sites > 0u ? 1u : 0u;
  return BuildUnsafePointerExtensionProfileFromCounts(
      unsafe_keyword_sites, pointer_arithmetic_sites, raw_pointer_type_sites);
}

}  // namespace objc3c::parse
