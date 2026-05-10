"""Compile artifact ownership snapshots for runtime acceptance provenance."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any

from .checksums import file_sha256_hex
from .checksums import sha256_text_hex


@dataclass(frozen=True)
class CompileArtifactEntry:
    path: str
    byte_count: int
    sha256: str

    def payload(self) -> dict[str, Any]:
        return {
            "path": self.path,
            "byte_count": self.byte_count,
            "sha256": self.sha256,
        }


@dataclass(frozen=True)
class CompileArtifactSet:
    entries: tuple[CompileArtifactEntry, ...]
    digest_sha256: str

    @property
    def count(self) -> int:
        return len(self.entries)

    def entry_payloads(self) -> list[dict[str, Any]]:
        return [entry.payload() for entry in self.entries]

    def provenance_payload_fields(self) -> dict[str, Any]:
        return {
            "artifact_count": self.count,
            "artifact_set_digest_sha256": self.digest_sha256,
            "emitted_artifacts": self.entry_payloads(),
        }

    def registration_manifest_fields(self) -> dict[str, Any]:
        return {
            "compile_output_artifact_count": self.count,
            "compile_output_artifact_set_digest_sha256": self.digest_sha256,
        }


def collect_compile_artifact_set(
    *,
    compile_dir: Path,
    emit_prefix: str,
    provenance_file_name: str,
) -> CompileArtifactSet:
    entries: list[CompileArtifactEntry] = []
    registration_manifest_name = f"{emit_prefix}.runtime-registration-manifest.json"
    lower_prefix = emit_prefix.lower()
    for artifact in sorted(compile_dir.iterdir(), key=lambda entry: entry.name):
        if not artifact.is_file():
            continue
        if artifact.name in {provenance_file_name, registration_manifest_name}:
            continue
        lower_name = artifact.name.lower()
        if not (
            lower_name == lower_prefix
            or lower_name.startswith(f"{lower_prefix}.")
            or lower_name.startswith(f"{lower_prefix}-")
        ):
            continue
        entries.append(
            CompileArtifactEntry(
                path=artifact.name,
                byte_count=artifact.stat().st_size,
                sha256=file_sha256_hex(artifact),
            )
        )
    digest_lines = [
        f"{entry.path}|{entry.byte_count}|{entry.sha256}" for entry in entries
    ]
    return CompileArtifactSet(
        entries=tuple(entries),
        digest_sha256=sha256_text_hex("\n".join(digest_lines)),
    )


__all__ = [
    "CompileArtifactEntry",
    "CompileArtifactSet",
    "collect_compile_artifact_set",
]
