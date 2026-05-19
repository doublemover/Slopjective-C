from __future__ import annotations

from .pattern_model import ForbiddenPattern


RUNTIME_PATTERNS: tuple[ForbiddenPattern, ...] = (
    ForbiddenPattern(
        "runtime-dispatch-pseudo-success",
        "Runtime dispatch must not expose pseudo-success ComputeDispatchResult paths.",
        r"\bComputeDispatchResult\b",
        residue_class="projected-behavior-claim",
    ),
    ForbiddenPattern(
        "unresolved-dispatch-pseudo-success",
        "Unresolved runtime dispatch must hard-fail instead of documenting pseudo-success behavior.",
        r"\bunresolved\b[^\n]{0,120}\bpseudo(?:[-_\s]+success)?\b",
        residue_class="projected-behavior-claim",
    ),
    ForbiddenPattern(
        "retired-msgsend-compatibility-dispatch",
        "Retired msgsend compatibility dispatch surfaces are removed from the hard cutover.",
        r"(?<![A-Za-z0-9_])objc3_msgsend_i32(?:[-_]shim)?(?![A-Za-z0-9_])"
        r"|(?<![A-Za-z0-9_])[A-Za-z0-9_]*compatibility_(?:runtime_)?dispatch_symbol(?![A-Za-z0-9_])"
        r"|(?<![A-Za-z0-9_])compatibility[-_\s]+(?:runtime[-_\s]+)?dispatch(?:[-_\s]+symbol)?(?![A-Za-z0-9_])",
        residue_class="legacy-compatibility-text",
    ),
    ForbiddenPattern(
        "runtime-shim-token",
        "Runtime shim terminology is not allowed on the authoritative surface.",
        r"\bruntime[._\s-]?shim\b",
        residue_class="shim-fallback-language",
    ),
    ForbiddenPattern(
        "deterministic-runtime-arithmetic",
        "Deterministic arithmetic runtime-dispatch formulas are retired from strict dispatch surfaces.",
        r"\bdeterministic\s+arithmetic\b|\barithmetic\s+formula\b|\bruntime\s+dispatch[^\n]{0,120}\barithmetic\b",
        residue_class="projected-behavior-claim",
    ),
)
