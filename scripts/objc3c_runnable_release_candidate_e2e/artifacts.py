"""Artifact inventory parsing for runnable release-candidate validation."""

from __future__ import annotations

from pathlib import Path

from .assertions import expect
from .catalog import COMPILE_ARTIFACT_FILES
from .catalog import EXPECTED_VALIDATE_ARTIFACTS


def compile_artifact_paths(compile_out_dir: Path) -> dict[str, Path]:
    return {
        artifact_key: compile_out_dir / artifact_name
        for artifact_key, artifact_name in COMPILE_ARTIFACT_FILES
    }


def assert_compile_artifacts(compile_artifacts: dict[str, Path]) -> None:
    for artifact_path in compile_artifacts.values():
        expect(artifact_path.is_file(), f"packaged compile wrapper did not publish {artifact_path}")


def collect_validate_artifacts(validate_out_dir: Path) -> list[str]:
    return sorted(path.name for path in validate_out_dir.glob("module.objc3-*.json"))


def assert_validate_artifacts(validate_artifacts: list[str]) -> None:
    expect(
        validate_artifacts == list(EXPECTED_VALIDATE_ARTIFACTS),
        "packaged release-candidate validation drifted from the final artifact inventory",
    )


__all__ = [
    "assert_compile_artifacts",
    "assert_validate_artifacts",
    "collect_validate_artifacts",
    "compile_artifact_paths",
]
