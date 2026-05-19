"""Replay command parsing, expansion, execution, and result modeling."""

from __future__ import annotations

import os
import subprocess
from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import display_path
from scripts.objc3c_end_to_end_determinism.artifacts import collect_artifacts
from scripts.objc3c_end_to_end_determinism.constants import (
    MAX_STDIO_PREVIEW_CHARS,
    ROOT,
)
from scripts.objc3c_end_to_end_determinism.errors import DeterminismContractError
from scripts.objc3c_end_to_end_determinism.hashing import sha256_bytes


def parse_key_value(values: Sequence[str], *, context: str) -> dict[str, str]:
    result: dict[str, str] = {}
    for raw in values:
        if "=" not in raw:
            raise DeterminismContractError(
                f"{context} entry must use KEY=VALUE format: {raw!r}"
            )
        key, value = raw.split("=", 1)
        if not key:
            raise DeterminismContractError(
                f"{context} entry key must be non-empty: {raw!r}"
            )
        result[key] = value
    return result


def parse_variant_env(values: Sequence[str], *, replays: int) -> dict[str, list[str]]:
    mapping: dict[str, list[str]] = {}
    for raw in values:
        if "=" not in raw:
            raise DeterminismContractError(
                f"variant env entry must use KEY=v1,v2,... format: {raw!r}"
            )
        key, value_blob = raw.split("=", 1)
        if not key:
            raise DeterminismContractError(
                f"variant env key must be non-empty: {raw!r}"
            )
        values_for_key = value_blob.split(",")
        if len(values_for_key) != replays:
            raise DeterminismContractError(
                "variant env value count mismatch for "
                f"{key!r}: expected {replays}, observed {len(values_for_key)}"
            )
        mapping[key] = values_for_key
    return mapping


def normalize_command_tokens(command: Sequence[str]) -> list[str]:
    tokens = list(command)
    if tokens and tokens[0] == "--":
        tokens = tokens[1:]
    if not tokens:
        raise DeterminismContractError(
            "missing replay command. Provide command tokens after '--'."
        )
    return tokens


def expand_command_tokens(
    tokens: Sequence[str],
    *,
    run_dir: Path,
    run_label: str,
) -> list[str]:
    expanded: list[str] = []
    for token in tokens:
        replaced = (
            token.replace("{repo_root}", str(ROOT))
            .replace("{run_dir}", str(run_dir))
            .replace("{run_id}", run_label)
        )
        expanded.append(replaced)
    return expanded


def run_once(
    *,
    command_template: Sequence[str],
    run_dir: Path,
    run_label: str,
    workdir: Path,
    base_env: dict[str, str],
    variant_env: dict[str, list[str]],
    replay_index: int,
    artifact_globs: Sequence[str],
) -> dict[str, object]:
    run_dir.mkdir(parents=True, exist_ok=True)
    command = expand_command_tokens(command_template, run_dir=run_dir, run_label=run_label)

    env = os.environ.copy()
    env.update(base_env)
    applied_variant_env: dict[str, str] = {}
    for key in sorted(variant_env.keys()):
        value = variant_env[key][replay_index]
        env[key] = value
        applied_variant_env[key] = value

    completed = subprocess.run(
        command,
        cwd=workdir,
        env=env,
        check=False,
        capture_output=True,
        text=False,
    )
    stdout = completed.stdout
    stderr = completed.stderr

    run_record: dict[str, object] = {
        "run_id": run_label,
        "run_dir": display_path(run_dir),
        "command": command,
        "exit_code": completed.returncode,
        "env_overrides": dict(sorted({**base_env, **applied_variant_env}.items())),
        "stdout_sha256": sha256_bytes(stdout),
        "stderr_sha256": sha256_bytes(stderr),
        "stdout_bytes": len(stdout),
        "stderr_bytes": len(stderr),
    }
    if completed.returncode != 0:
        run_record["failure_preview"] = {
            "stdout": stdout.decode("utf-8", errors="replace")[:MAX_STDIO_PREVIEW_CHARS],
            "stderr": stderr.decode("utf-8", errors="replace")[:MAX_STDIO_PREVIEW_CHARS],
        }
        return run_record

    artifacts, digest_by_path, corpus_sha = collect_artifacts(
        run_dir=run_dir,
        artifact_globs=artifact_globs,
    )
    run_record["artifacts"] = artifacts
    run_record["artifact_count"] = len(artifacts)
    run_record["corpus_sha256"] = corpus_sha
    run_record["_digest_by_path"] = digest_by_path
    return run_record


__all__ = [
    "expand_command_tokens",
    "normalize_command_tokens",
    "parse_key_value",
    "parse_variant_env",
    "run_once",
]
