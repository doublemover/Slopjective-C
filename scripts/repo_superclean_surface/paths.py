"""Shared paths for the build-owned repo-superclean surface artifact."""

from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
REPO_SUPERCLEAN_SOURCE_OF_TRUTH_FILENAME = "repo_superclean_source_of_truth.json"
REPO_SUPERCLEAN_ACTIVE_BUILD_DIR = ROOT / "tmp" / "build-objc3c-native"
REPO_SUPERCLEAN_SOURCE_OF_TRUTH = (
    REPO_SUPERCLEAN_ACTIVE_BUILD_DIR / REPO_SUPERCLEAN_SOURCE_OF_TRUTH_FILENAME
)
LEGACY_REPO_SUPERCLEAN_ARTIFACT_SOURCE_OF_TRUTH = (
    ROOT
    / "tmp"
    / "artifacts"
    / "objc3c-native"
    / REPO_SUPERCLEAN_SOURCE_OF_TRUTH_FILENAME
)


def repo_relative_path(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


REPO_SUPERCLEAN_SOURCE_OF_TRUTH_RELATIVE = repo_relative_path(
    REPO_SUPERCLEAN_SOURCE_OF_TRUTH
)
LEGACY_REPO_SUPERCLEAN_ARTIFACT_SOURCE_OF_TRUTH_RELATIVE = repo_relative_path(
    LEGACY_REPO_SUPERCLEAN_ARTIFACT_SOURCE_OF_TRUTH
)


__all__ = [
    "LEGACY_REPO_SUPERCLEAN_ARTIFACT_SOURCE_OF_TRUTH",
    "LEGACY_REPO_SUPERCLEAN_ARTIFACT_SOURCE_OF_TRUTH_RELATIVE",
    "REPO_SUPERCLEAN_ACTIVE_BUILD_DIR",
    "REPO_SUPERCLEAN_SOURCE_OF_TRUTH",
    "REPO_SUPERCLEAN_SOURCE_OF_TRUTH_FILENAME",
    "REPO_SUPERCLEAN_SOURCE_OF_TRUTH_RELATIVE",
    "ROOT",
    "repo_relative_path",
]
