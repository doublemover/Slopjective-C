from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class ForbiddenPattern:
    pattern_id: str
    description: str
    regex: str
    severity: str = "error"


DEFAULT_SCAN_ROOTS: tuple[str, ...] = (
    "native/objc3c",
    "scripts",
    "docs",
    "schemas",
    "package.json",
    ".github/workflows",
)

DEFAULT_EXCLUDES: tuple[str, ...] = (
    ".git/**",
    "node_modules/**",
    "tmp/**",
    "artifacts/**",
    "reports/**",
    "scripts/source_hygiene/**",
    "scripts/check_source_hygiene_hard_cutover.py",
    "schemas/source-hygiene-hard-cutover-report-v1.schema.json",
    "tests/tooling/source_hygiene/**",
    "native/objc3c/src/config/**",
    "native/objc3c/src/diagnostics/modes/**",
)

TEXT_SUFFIXES: frozenset[str] = frozenset(
    {
        ".c",
        ".cc",
        ".cmake",
        ".cpp",
        ".h",
        ".hpp",
        ".inc",
        ".json",
        ".md",
        ".ps1",
        ".py",
        ".txt",
        ".yaml",
        ".yml",
    }
)

FORBIDDEN_PATTERNS: tuple[ForbiddenPattern, ...] = (
    ForbiddenPattern(
        "runtime-dispatch-pseudo-success",
        "Runtime dispatch must not expose pseudo-success ComputeDispatchResult paths.",
        r"\bComputeDispatchResult\b",
    ),
    ForbiddenPattern(
        "runtime-shim-token",
        "Runtime shim terminology is not allowed on the authoritative surface.",
        r"\bruntime[_-]?shim\b",
    ),
    ForbiddenPattern(
        "public-compatibility-mode",
        "Compatibility-mode surfaces are removed in the canonical-only cutover.",
        r"\bcompatibility_mode\b|--objc3-compat-mode|\bObjc3\w*CompatibilityMode\b|OBJC3C_FRONTEND_COMPATIBILITY_MODE",
    ),
    ForbiddenPattern(
        "legacy-language-profile-enum",
        "Language profile enums are canonical-only and must not retain legacy values.",
        r"\bkLegacy\b|\bLegacy\s*=\s*1\b|\bObjc3\w*LanguageProfile::k?Legacy\b",
    ),
    ForbiddenPattern(
        "canonical-rejection-diagnostics-surface",
        "Migration-assist surfaces are removed in the hard cutover.",
        r"\bmigration[_-]?assist\b|--objc3-canonical-rejection-diagnostics",
    ),
    ForbiddenPattern(
        "deterministic-fallback-wording",
        "Fallback wording is not allowed as active behavior documentation.",
        r"\bdeterministic\s+fallback\b|\bfallback\s+path\b|\bfallback\s+dispatch\b",
    ),
    ForbiddenPattern(
        "shim-wording",
        "Shim wording is not allowed on authoritative active paths.",
        r"\bcompatibility\s+shim\b|\boptional\s+C\s+ABI\s+shim\b|\bshim\s+fixture\b|\bshim\s+case\b",
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
