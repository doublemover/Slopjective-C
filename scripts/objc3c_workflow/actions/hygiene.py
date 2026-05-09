"""Repository hygiene, dependency, and source-cleanliness workflow facade."""

from __future__ import annotations

from .hygiene_composites import _run_steps, action_lint, action_validate_repo_superclean
from .hygiene_paths import (
    DEPENDENCY_BOUNDARIES_PY,
    RELEASE_EVIDENCE_PY,
    REPO_SUPERCLEAN_SURFACE_PY,
    SOURCE_HYGIENE_AUTHENTICITY_PY,
    SOURCE_HYGIENE_HARD_CUTOVER_PY,
    SPEC_LINT_PY,
    TASK_HYGIENE_PY,
)
from .hygiene_source_policy import (
    action_check_dependency_boundaries,
    action_check_release_evidence,
    action_check_repo_superclean_surface,
    action_check_source_hygiene_authenticity,
    action_check_source_hygiene_hard_cutover,
    action_check_task_hygiene,
    action_lint_spec,
)

__all__ = [
    "DEPENDENCY_BOUNDARIES_PY",
    "RELEASE_EVIDENCE_PY",
    "REPO_SUPERCLEAN_SURFACE_PY",
    "SOURCE_HYGIENE_AUTHENTICITY_PY",
    "SOURCE_HYGIENE_HARD_CUTOVER_PY",
    "SPEC_LINT_PY",
    "TASK_HYGIENE_PY",
    "_run_steps",
    "action_check_dependency_boundaries",
    "action_check_release_evidence",
    "action_check_repo_superclean_surface",
    "action_check_source_hygiene_authenticity",
    "action_check_source_hygiene_hard_cutover",
    "action_check_task_hygiene",
    "action_lint",
    "action_lint_spec",
    "action_validate_repo_superclean",
]
