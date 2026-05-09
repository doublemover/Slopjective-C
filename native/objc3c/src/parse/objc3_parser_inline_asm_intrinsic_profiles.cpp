#include "parse/objc3_parser_inline_asm_intrinsic_profiles.h"

#include <memory>
#include <sstream>
#include <vector>

#include "parse/objc3_parser_profile_helpers.h"
#include "parse/objc3_parser_profile_symbol_walk.h"

namespace objc3c::parse {
namespace {

bool IsInlineAsmCallSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered == "asm" || lowered == "__asm" || lowered == "__asm__" ||
         lowered.rfind("asm_", 0) == 0 || lowered.rfind("__asm_", 0) == 0 ||
         lowered.find("inline_asm") != std::string::npos;
}

bool IsIntrinsicCallSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.rfind("__builtin_", 0) == 0 ||
         lowered.rfind("llvm.", 0) == 0 ||
         lowered.rfind("llvm_", 0) == 0 ||
         lowered.find("intrinsic") != std::string::npos;
}

bool IsPrivilegedIntrinsicCallSymbol(const std::string &symbol) {
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

void CollectInlineAsmIntrinsicSitesFromSymbol(
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

void CollectInlineAsmIntrinsicProfileSymbol(
    const std::string &symbol,
    void *context) {
  auto *counts = static_cast<Objc3InlineAsmIntrinsicSiteCounts *>(context);
  CollectInlineAsmIntrinsicSitesFromSymbol(symbol, *counts);
}

Objc3InlineAsmIntrinsicSiteCounts CountInlineAsmIntrinsicSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3InlineAsmIntrinsicSiteCounts counts;
  Objc3ProfileSymbolWalker walker{
      &CollectInlineAsmIntrinsicProfileSymbol, &counts};
  WalkObjc3ProfileSymbolsInBody(body, walker);
  return counts;
}

Objc3InlineAsmIntrinsicGovernanceProfile
BuildInlineAsmIntrinsicGovernanceProfileFromCounts(
    std::size_t inline_asm_sites,
    std::size_t intrinsic_sites,
    std::size_t governed_intrinsic_sites,
    std::size_t privileged_intrinsic_sites) {
  Objc3InlineAsmIntrinsicGovernanceProfile profile;
  profile.inline_asm_sites = inline_asm_sites;
  profile.intrinsic_sites = intrinsic_sites;
  profile.governed_intrinsic_sites = governed_intrinsic_sites;
  profile.privileged_intrinsic_sites = privileged_intrinsic_sites;
  profile.inline_asm_intrinsic_sites =
      profile.inline_asm_sites + profile.intrinsic_sites;
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
  if (profile.normalized_sites + profile.gate_blocked_sites !=
      profile.inline_asm_intrinsic_sites) {
    profile.contract_violation_sites += 1u;
  }

  profile.deterministic_inline_asm_intrinsic_governance_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

}  // namespace

std::string BuildInlineAsmIntrinsicGovernanceProfile(
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
      << (deterministic_inline_asm_intrinsic_governance_handoff ? "true"
                                                                : "false");
  return out.str();
}

bool IsInlineAsmIntrinsicGovernanceProfileNormalized(
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

Objc3InlineAsmIntrinsicGovernanceProfile
BuildInlineAsmIntrinsicGovernanceProfileFromFunction(const FunctionDecl &fn) {
  const Objc3InlineAsmIntrinsicSiteCounts counts =
      CountInlineAsmIntrinsicSitesInBody(fn.body);
  return BuildInlineAsmIntrinsicGovernanceProfileFromCounts(
      counts.inline_asm_sites,
      counts.intrinsic_sites,
      counts.governed_intrinsic_sites,
      counts.privileged_intrinsic_sites);
}

Objc3InlineAsmIntrinsicGovernanceProfile
BuildInlineAsmIntrinsicGovernanceProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3InlineAsmIntrinsicSiteCounts counts;
  Objc3ProfileSymbolWalker walker{
      &CollectInlineAsmIntrinsicProfileSymbol, &counts};
  WalkObjc3ProfileSymbolsInOpaqueMethodBody(method, walker);
  return BuildInlineAsmIntrinsicGovernanceProfileFromCounts(
      counts.inline_asm_sites,
      counts.intrinsic_sites,
      counts.governed_intrinsic_sites,
      counts.privileged_intrinsic_sites);
}

}  // namespace objc3c::parse
