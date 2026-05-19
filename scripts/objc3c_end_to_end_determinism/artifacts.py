"""Artifact discovery and digest corpus construction."""

from __future__ import annotations

from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import display_path
from scripts.objc3c_end_to_end_determinism.errors import DeterminismContractError
from scripts.objc3c_end_to_end_determinism.hashing import sha256_bytes


def collect_artifacts(
    *,
    run_dir: Path,
    artifact_globs: Sequence[str],
) -> tuple[list[dict[str, object]], dict[str, str], str]:
    discovered: dict[str, Path] = {}
    for pattern in artifact_globs:
        for path in sorted(run_dir.glob(pattern)):
            if not path.is_file():
                continue
            relative = path.relative_to(run_dir).as_posix()
            discovered[relative] = path

    if not discovered:
        raise DeterminismContractError(
            f"no artifacts matched under {display_path(run_dir)} for patterns {list(artifact_globs)!r}"
        )

    entries: list[dict[str, object]] = []
    digest_by_path: dict[str, str] = {}
    corpus_lines: list[str] = []
    for relative in sorted(discovered.keys()):
        path = discovered[relative]
        data = path.read_bytes()
        digest = sha256_bytes(data)
        entry = {
            "path": relative,
            "bytes": len(data),
            "sha256": digest,
        }
        entries.append(entry)
        digest_by_path[relative] = digest
        corpus_lines.append(f"{relative}|{digest}|{len(data)}")

    corpus_payload = "\n".join(corpus_lines) + "\n"
    corpus_sha = sha256_bytes(corpus_payload.encode("utf-8"))
    return entries, digest_by_path, corpus_sha


__all__ = ["collect_artifacts"]
