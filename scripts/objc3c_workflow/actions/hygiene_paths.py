"""Repository hygiene and source-policy workflow paths."""

from __future__ import annotations

from ..environment import ROOT

REPO_SUPERCLEAN_SURFACE_PY = ROOT / "scripts" / "check_repo_superclean_surface.py"
DEPENDENCY_BOUNDARIES_PY = ROOT / "scripts" / "check_objc3c_dependency_boundaries.py"
RELEASE_EVIDENCE_PY = ROOT / "scripts" / "check_release_evidence.py"
SOURCE_HYGIENE_AUTHENTICITY_PY = ROOT / "scripts" / "check_source_hygiene_authenticity.py"
SOURCE_HYGIENE_HARD_CUTOVER_PY = ROOT / "scripts" / "check_source_hygiene_hard_cutover.py"
SPEC_LINT_PY = ROOT / "scripts" / "spec_lint.py"
TASK_HYGIENE_PY = ROOT / "scripts" / "ci" / "run_task_hygiene_gate.py"
