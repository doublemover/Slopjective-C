from __future__ import annotations

from .pattern_model import ForbiddenPattern
from .patterns_cutover import HARD_CUTOVER_RESIDUE_PATTERNS
from .patterns_language import LANGUAGE_PATTERNS
from .patterns_public_claims import PUBLIC_CLAIM_PATTERNS
from .patterns_runtime import RUNTIME_PATTERNS
from .patterns_structure import STRUCTURE_PATTERNS
from .patterns_truth import TRUTH_BOUNDARY_PATTERNS
from .patterns_workflow import WORKFLOW_PATTERNS


FORBIDDEN_PATTERNS: tuple[ForbiddenPattern, ...] = (
    *RUNTIME_PATTERNS,
    *LANGUAGE_PATTERNS,
    *HARD_CUTOVER_RESIDUE_PATTERNS,
    *WORKFLOW_PATTERNS,
    *PUBLIC_CLAIM_PATTERNS,
    *STRUCTURE_PATTERNS,
    *TRUTH_BOUNDARY_PATTERNS,
)

__all__ = ["ForbiddenPattern", "FORBIDDEN_PATTERNS"]
