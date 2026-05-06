from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class ForbiddenPattern:
    pattern_id: str
    description: str
    regex: str
    severity: str = "error"


FORBIDDEN_PATTERNS: tuple[ForbiddenPattern, ...] = (
    ForbiddenPattern(
        "runtime-dispatch-pseudo-success",
        "Runtime dispatch must not expose pseudo-success ComputeDispatchResult paths.",
        r"\bComputeDispatchResult\b",
    ),
    ForbiddenPattern(
        "unresolved-dispatch-pseudo-success",
        "Unresolved runtime dispatch must hard-fail instead of documenting pseudo-success behavior.",
        r"\bunresolved\b[^\n]{0,120}\bpseudo(?:[-_\s]+success)?\b",
    ),
    ForbiddenPattern(
        "retired-msgsend-compatibility-dispatch",
        "Retired msgsend compatibility dispatch surfaces are removed from the hard cutover.",
        r"(?<![A-Za-z0-9_])objc3_msgsend_i32(?:[-_]shim)?(?![A-Za-z0-9_])"
        r"|(?<![A-Za-z0-9_])[A-Za-z0-9_]*compatibility_(?:runtime_)?dispatch_symbol(?![A-Za-z0-9_])"
        r"|(?<![A-Za-z0-9_])compatibility[-_\s]+(?:runtime[-_\s]+)?dispatch(?:[-_\s]+symbol)?(?![A-Za-z0-9_])",
    ),
    ForbiddenPattern(
        "runtime-shim-token",
        "Runtime shim terminology is not allowed on the authoritative surface.",
        r"\bruntime[._\s-]?shim\b",
    ),
    ForbiddenPattern(
        "public-compatibility-mode",
        "Compatibility-mode surfaces are removed in the canonical-only cutover.",
        r"\bcompatibility_mode\b|\bcompatibility[-\s]+mode\b|--objc3-compat-mode|\bObjc3\w*CompatibilityMode\b|OBJC3C_FRONTEND_COMPATIBILITY_MODE",
    ),
    ForbiddenPattern(
        "retired-public-workflow-runner",
        "The public workflow runner file was replaced by the canonical scripts.objc3c_workflow module.",
        r"\bobjc3c_public_workflow_runner\.py\b",
    ),
    ForbiddenPattern(
        "retired-npm-workflow-command",
        "Workflow actions must route through npm run objc3c -- <action>, not retired colon-style npm command names.",
        r"\bnpm\s+run\s+(?!objc3c\b)[a-z0-9][a-z0-9:_-]*:",
    ),
    ForbiddenPattern(
        "legacy-language-profile-enum",
        "Language profile enums are canonical-only and must not retain legacy values.",
        r"\bkLegacy\b|\bLegacy\s*=\s*1\b|\bObjc3\w*LanguageProfile::k?Legacy\b",
    ),
    ForbiddenPattern(
        "canonical-rejection-diagnostics-surface",
        "Migration-assist surfaces are removed in the hard cutover.",
        r"\bmigration[_-]?assist\b|--objc3-canonical-rejection-diagnostics|\blegacy_literal_diagnostics\b",
    ),
    ForbiddenPattern(
        "backward-compatible-alias-wording",
        "Backward-compatible alias claims are removed from active hard-cutover surfaces.",
        r"\bbackward[-\s]+compatible\s+aliases\b",
    ),
    ForbiddenPattern(
        "legacy-spec-redirect-wording",
        "Legacy spec redirect claims are removed from active hard-cutover surfaces.",
        r"\blegacy\s+spec\s+redirects?\b",
    ),
    ForbiddenPattern(
        "deterministic-fallback-wording",
        "Fallback wording is not allowed as active behavior documentation.",
        r"\bdeterministic\s+fallbacks?\b|\bfallback\s+paths?\b|\bfallback\s+dispatch(?:es)?\b|\bfallback\s+behaviors?\b",
    ),
    ForbiddenPattern(
        "fallback-only-wording",
        "Fallback-only wording is not allowed as active behavior documentation.",
        r"\bfallback[-\s]+only\b",
    ),
    ForbiddenPattern(
        "deterministic-runtime-arithmetic",
        "Deterministic arithmetic runtime-dispatch formulas are retired from strict dispatch surfaces.",
        r"\bdeterministic\s+arithmetic\b|\barithmetic\s+formula\b|\bruntime\s+dispatch[^\n]{0,120}\barithmetic\b",
    ),
    ForbiddenPattern(
        "shim-wording",
        "Shim wording is not allowed on authoritative active paths.",
        r"\bshim\b|\bcompatibility\s+shim\b",
    ),
    ForbiddenPattern(
        "old-mode-wording",
        "Old-mode wording is not allowed on authoritative active paths.",
        r"\bold\s+mode\b",
    ),
    ForbiddenPattern(
        "stale-monolithic-cmake",
        "The native CMake graph must not carry monolithic future-state commentary.",
        r"monolithic\s+`?src/main\.cpp`?|planned\s+steady-state\s+split",
    ),
    ForbiddenPattern(
        "milestone-as-behavior",
        "Milestone/proof language must not act as behavior truth.",
        r"\bmilestone-local\b|\bproof-only\b|\bproof\s+packet\b|\bproof\s+path\b",
    ),
)
