"""Compile artifact path helpers for runnable error validation."""

from __future__ import annotations

from pathlib import Path

from .assertions import expect


def compile_artifact_paths(compile_out_dir: Path) -> dict[str, Path]:
    return {
        "manifest": compile_out_dir / "module.manifest.json",
        "registration_manifest": compile_out_dir / "module.runtime-registration-manifest.json",
        "compile_provenance": compile_out_dir / "module.compile-provenance.json",
        "object": compile_out_dir / "module.obj",
    }


def assert_compile_artifacts_exist(compile_artifacts: dict[str, Path]) -> None:
    for artifact_path in compile_artifacts.values():
        expect(artifact_path.is_file(), f"packaged compile wrapper did not publish {artifact_path}")


__all__ = ["assert_compile_artifacts_exist", "compile_artifact_paths"]
