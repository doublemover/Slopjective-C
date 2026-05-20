"""Repo superclean and release-evidence action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

CORE_SUPERCLEAN_ACTION_SPECS: dict[str, ActionSpec] = {
    "check-release-evidence": ActionSpec(
        "check-release-evidence",
        "check the contract-backed release evidence surface",
        "python:scripts/check_release_evidence.py",
        validation_tier="repo",
        guarantee_owner=(
            "release evidence indexes stay coherent and replayable from the checked-in "
            "release evidence contract, schemas, and generated tmp index"
        ),
    ),
    "check-repo-superclean-surface": ActionSpec(
        "check-repo-superclean-surface",
        "refresh and check the build-emitted repo superclean source-of-truth artifact",
        "runner-internal + native build source contracts + python:scripts/check_repo_superclean_surface.py",
        validation_tier="repo",
        guarantee_owner=(
            "native build emits the canonical repo-cleanup roots, outputs, and command "
            "names as one source-of-truth artifact"
        ),
    ),
    "validate-repo-superclean": ActionSpec(
        "validate-repo-superclean",
        "build the canonical repo surface and run the integrated hygiene/docs/superclean checks",
        "runner-internal + native build contracts + task hygiene gate",
        validation_tier="repo",
        guarantee_owner=(
            "repo roots, checked-in docs, generated outputs, and machine-owned boundaries "
            "remain canonical and enforced"
        ),
    ),
}
