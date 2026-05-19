from __future__ import annotations

from .pattern_model import ForbiddenPattern
from .pattern_scopes import PUBLIC_SURFACE_PATHS


TRUTH_BOUNDARY_PATTERNS: tuple[ForbiddenPattern, ...] = (
    ForbiddenPattern(
        "generated-output-source-of-truth-claim",
        "Generated outputs must not be described as editable source-of-truth inputs.",
        r"\bgenerated[-_\s]+(?:output|truth)[^\n]{0,120}\b(?:source[-_\s]+of[-_\s]+truth|authoritative\s+input|canonical\s+input)\b"
        r"|\b(?:source[-_\s]+of[-_\s]+truth|authoritative\s+input|canonical\s+input)[^\n]{0,120}\bgenerated[-_\s]+(?:output|truth)\b",
        include_paths=PUBLIC_SURFACE_PATHS,
        residue_class="projected-behavior-claim",
    ),
    ForbiddenPattern(
        "generated-output-manual-edit-claim",
        "Generated outputs may not be documented as manually editable surfaces.",
        r"\b(?:hand[-_\s]?edit(?:ed|ing)?|manual(?:ly)?\s+edit(?:ed|ing)?)[^\n]{0,120}\bgenerated[-_\s]+outputs?\b"
        r"|\bgenerated[-_\s]+outputs?[^\n]{0,120}\b(?:hand[-_\s]?edit(?:ed|ing)?|manual(?:ly)?\s+edit(?:ed|ing)?)\b",
        include_paths=PUBLIC_SURFACE_PATHS,
        residue_class="projected-behavior-claim",
    ),
    ForbiddenPattern(
        "milestone-as-behavior",
        "Milestone/proof language must not act as behavior truth.",
        r"\bmilestone-local\b|\bproof-only\b|\bproof\s+packet\b|\bproof\s+path\b",
        residue_class="report-only-completion",
    ),
    ForbiddenPattern(
        "report-only-claim",
        "Report-only completion or support claims must not replace executable evidence.",
        r"\breport[-_\s]+only(?:[-_\s]+(?:claims?|completion|evidence|proof|mode|gate|check|validation|support|surface|path))?\b"
        r"|(?:\bclaim\b|\bcomplete\b|\bsupport\b)[^\n]{0,100}\breport[-_\s]+only\b",
        residue_class="report-only-completion",
    ),
    ForbiddenPattern(
        "source-hygiene-allowlist-residue",
        "Source-hygiene allowlist behavior is retired from the hard-cutover gate.",
        r"\bsource[-_\s]+hygiene[^\n]{0,100}\ballow[-_\s]?list\b"
        r"|\ballow[-_\s]?list[^\n]{0,100}\bsource[-_\s]+hygiene\b"
        r"|\bsource[-_\s]+hygiene[-_\s]+hard[-_\s]+cutover[-_\s]+allow[-_\s]?list\.json\b",
        residue_class="report-only-completion",
    ),
    ForbiddenPattern(
        "local-only-report-truth",
        "Local-only reports must not become support truth or closeout evidence.",
        r"\blocal[-_\s]+only\s+reports?\b"
        r"|\b(?:tmp|reports)[\\/][^\n]{0,120}\b(?:truth|support|closeout|evidence)\b",
        residue_class="report-only-completion",
    ),
)
