from __future__ import annotations

from .pattern_model import (
    ForbiddenPattern,
    ForbiddenPatternGroup,
    flatten_pattern_groups,
)
from .patterns_cutover import HARD_CUTOVER_RESIDUE_PATTERNS
from .patterns_language import LANGUAGE_PATTERNS
from .patterns_public_claims import PUBLIC_CLAIM_PATTERNS
from .patterns_runtime import RUNTIME_PATTERNS
from .patterns_structure import STRUCTURE_PATTERNS
from .patterns_truth import TRUTH_BOUNDARY_PATTERNS
from .patterns_workflow import WORKFLOW_PATTERNS


FORBIDDEN_PATTERN_GROUPS: tuple[ForbiddenPatternGroup, ...] = (
    ForbiddenPatternGroup(
        group_id="runtime-contract-residue",
        description="Runtime dispatch residue, pseudo-success, and retired runtime compatibility surfaces.",
        owner_surface="scripts/source_hygiene/patterns_runtime.py",
        patterns=RUNTIME_PATTERNS,
    ),
    ForbiddenPatternGroup(
        group_id="language-mode-residue",
        description="Retired language-profile and canonical-rejection wording.",
        owner_surface="scripts/source_hygiene/patterns_language.py",
        patterns=LANGUAGE_PATTERNS,
    ),
    ForbiddenPatternGroup(
        group_id="implementation-hard-cutover-residue",
        description="Implementation shim, fallback, migration, and legacy surfaces.",
        owner_surface="scripts/source_hygiene/patterns_cutover.py",
        patterns=HARD_CUTOVER_RESIDUE_PATTERNS,
    ),
    ForbiddenPatternGroup(
        group_id="workflow-command-residue",
        description="Retired public command, workflow facade, and alias surfaces.",
        owner_surface="scripts/source_hygiene/patterns_workflow.py",
        patterns=WORKFLOW_PATTERNS,
    ),
    ForbiddenPatternGroup(
        group_id="public-claim-residue",
        description="Public support claims for aliases, shims, fallbacks, migration lanes, and retired behavior.",
        owner_surface="scripts/source_hygiene/patterns_public_claims.py",
        patterns=PUBLIC_CLAIM_PATTERNS,
    ),
    ForbiddenPatternGroup(
        group_id="structure-residue",
        description="Source layout and module-ownership residue that would hide hard-cutover boundaries.",
        owner_surface="scripts/source_hygiene/patterns_structure.py",
        patterns=STRUCTURE_PATTERNS,
    ),
    ForbiddenPatternGroup(
        group_id="truth-boundary-residue",
        description="Generated-output, local-report, report-only, and retired allowlist truth-boundary residue.",
        owner_surface="scripts/source_hygiene/patterns_truth.py",
        patterns=TRUTH_BOUNDARY_PATTERNS,
    ),
)

FORBIDDEN_PATTERNS: tuple[ForbiddenPattern, ...] = flatten_pattern_groups(
    FORBIDDEN_PATTERN_GROUPS
)

__all__ = ["ForbiddenPattern", "FORBIDDEN_PATTERN_GROUPS", "FORBIDDEN_PATTERNS"]
