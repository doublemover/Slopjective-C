"""Data carriers for the runnable storage/reflection E2E checker."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class E2ERunPaths:
    package_root: Path
    manifest_path: Path
    compile_out_dir: Path
    probe_exe: Path


@dataclass(frozen=True)
class PackagedToolchain:
    compile_script: Path
    storage_fixture: Path
    runtime_library: Path
    storage_probe: Path
    smoke_script: Path
    replay_script: Path


@dataclass(frozen=True)
class CompileArtifacts:
    manifest: Path
    registration_manifest: Path
    compile_provenance: Path
    object: Path

    def as_dict(self) -> dict[str, Path]:
        return {
            "manifest": self.manifest,
            "registration_manifest": self.registration_manifest,
            "compile_provenance": self.compile_provenance,
            "object": self.object,
        }


__all__ = [
    "CompileArtifacts",
    "E2ERunPaths",
    "PackagedToolchain",
]
