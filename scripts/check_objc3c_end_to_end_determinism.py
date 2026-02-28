#!/usr/bin/env python3
"""Fail-closed end-to-end determinism gate for repeated objc3c replay runs."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import shlex
import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "objc3c-end-to-end-determinism-v1"
DEFAULT_REPLAY_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-end-to-end-determinism"
MAX_STDIO_PREVIEW_CHARS = 2000


class DeterminismContractError(ValueError):
    """Raised when CLI input cannot be validated for deterministic replay."""


def sha256_bytes(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


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


def compare_runs(runs: Sequence[dict[str, object]]) -> list[dict[str, object]]:
    if not runs:
        return []
    baseline = runs[0]
    baseline_digests = baseline.get("_digest_by_path")
    if not isinstance(baseline_digests, dict):
        return [
            {
                "kind": "baseline-missing-artifacts",
                "run_id": str(baseline.get("run_id", "run01")),
                "message": "baseline run did not produce artifact digest evidence",
            }
        ]

    baseline_paths = sorted(str(path) for path in baseline_digests.keys())
    mismatches: list[dict[str, object]] = []
    for replay in runs[1:]:
        replay_id = str(replay.get("run_id", "unknown"))
        replay_digests = replay.get("_digest_by_path")
        if not isinstance(replay_digests, dict):
            mismatches.append(
                {
                    "kind": "replay-missing-artifacts",
                    "run_id": replay_id,
                    "message": "replay run did not produce artifact digest evidence",
                }
            )
            continue

        replay_paths = sorted(str(path) for path in replay_digests.keys())
        missing = sorted(set(baseline_paths) - set(replay_paths))
        unexpected = sorted(set(replay_paths) - set(baseline_paths))
        for path in missing:
            mismatches.append(
                {
                    "kind": "artifact-missing",
                    "run_id": replay_id,
                    "path": path,
                }
            )
        for path in unexpected:
            mismatches.append(
                {
                    "kind": "artifact-unexpected",
                    "run_id": replay_id,
                    "path": path,
                }
            )

        for path in sorted(set(baseline_paths) & set(replay_paths)):
            base_sha = str(baseline_digests[path])
            replay_sha = str(replay_digests[path])
            if base_sha != replay_sha:
                mismatches.append(
                    {
                        "kind": "artifact-digest-mismatch",
                        "run_id": replay_id,
                        "path": path,
                        "baseline_sha256": base_sha,
                        "replay_sha256": replay_sha,
                    }
                )
    return mismatches


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="check_objc3c_end_to_end_determinism.py",
        description=(
            "Replay command execution and compare artifact digests across runs "
            "to enforce end-to-end determinism."
        ),
    )
    parser.add_argument(
        "--replays",
        type=int,
        default=2,
        help="Number of replay runs to execute (default: 2).",
    )
    parser.add_argument(
        "--artifact-glob",
        action="append",
        dest="artifact_globs",
        default=[],
        help=(
            "Artifact glob pattern relative to each run directory. "
            "May be passed multiple times. Defaults to '**/*' when omitted."
        ),
    )
    parser.add_argument(
        "--workdir",
        type=Path,
        default=ROOT,
        help=f"Working directory for replay command (default: {display_path(ROOT)}).",
    )
    parser.add_argument(
        "--replay-root",
        type=Path,
        default=DEFAULT_REPLAY_ROOT,
        help=(
            "Root directory for per-run replay artifacts "
            f"(default: {display_path(DEFAULT_REPLAY_ROOT)})."
        ),
    )
    parser.add_argument(
        "--run-label",
        default="latest",
        help="Replay session label under replay-root (default: latest).",
    )
    parser.add_argument(
        "--summary-json",
        type=Path,
        default=None,
        help="Optional path for digest evidence JSON summary.",
    )
    parser.add_argument(
        "--env",
        action="append",
        default=[],
        metavar="KEY=VALUE",
        help="Static env override applied to every replay run.",
    )
    parser.add_argument(
        "--variant-env",
        action="append",
        default=[],
        metavar="KEY=v1,v2,...",
        help="Per-run env override values; value count must equal --replays.",
    )
    parser.add_argument(
        "command",
        nargs=argparse.REMAINDER,
        help=(
            "Command tokens after '--'. Supports {repo_root}, {run_dir}, {run_id} "
            "placeholders."
        ),
    )
    return parser


def check_determinism(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)

    if args.replays < 2:
        raise DeterminismContractError("--replays must be >= 2")

    command_template = normalize_command_tokens(args.command)
    artifact_globs = tuple(args.artifact_globs) if args.artifact_globs else ("**/*",)
    workdir = args.workdir.resolve()
    replay_root = args.replay_root.resolve()
    run_label = str(args.run_label)
    if not run_label:
        raise DeterminismContractError("--run-label must be non-empty")

    base_env = parse_key_value(args.env, context="--env")
    variant_env = parse_variant_env(args.variant_env, replays=args.replays)

    session_dir = replay_root / run_label
    session_dir.mkdir(parents=True, exist_ok=True)
    summary_json = args.summary_json.resolve() if args.summary_json else (session_dir / "summary.json")
    summary_json.parent.mkdir(parents=True, exist_ok=True)

    runs: list[dict[str, object]] = []
    command_failures: list[dict[str, object]] = []
    for index in range(args.replays):
        run_id = f"run{index + 1:02d}"
        run_dir = session_dir / run_id
        run_record = run_once(
            command_template=command_template,
            run_dir=run_dir,
            run_label=run_id,
            workdir=workdir,
            base_env=base_env,
            variant_env=variant_env,
            replay_index=index,
            artifact_globs=artifact_globs,
        )
        runs.append(run_record)
        if int(run_record["exit_code"]) != 0:
            command_failures.append(
                {
                    "kind": "command-exit-nonzero",
                    "run_id": run_id,
                    "exit_code": run_record["exit_code"],
                }
            )

    mismatches = compare_runs(runs)
    failures = command_failures + mismatches

    for run in runs:
        run.pop("_digest_by_path", None)

    payload: dict[str, object] = {
        "mode": MODE,
        "status": "PASS" if not failures else "FAIL",
        "replays": args.replays,
        "artifact_globs": list(artifact_globs),
        "command_template": list(command_template),
        "session": {
            "workdir": display_path(workdir),
            "replay_root": display_path(replay_root),
            "run_label": run_label,
            "session_dir": display_path(session_dir),
            "summary_json": display_path(summary_json),
        },
        "runs": runs,
        "mismatches": failures,
    }
    summary_json.write_text(canonical_json(payload), encoding="utf-8")

    if failures:
        print(
            "objc3c-end-to-end-determinism: FAIL "
            f"(runs={args.replays}, mismatches={len(failures)})",
            file=sys.stderr,
        )
        for mismatch in failures[:40]:
            print(f"- {mismatch}", file=sys.stderr)
        print(
            "- Regenerate/replay command:\n"
            + shlex.join(
                [
                    "python",
                    "scripts/check_objc3c_end_to_end_determinism.py",
                    "--replays",
                    str(args.replays),
                    "--replay-root",
                    display_path(replay_root),
                    "--run-label",
                    run_label,
                    "--",
                    *command_template,
                ]
            ),
            file=sys.stderr,
        )
        print(
            f"- Digest evidence: {display_path(summary_json)}",
            file=sys.stderr,
        )
        return 1

    print(
        "objc3c-end-to-end-determinism: OK "
        f"(runs={args.replays}, artifacts={runs[0]['artifact_count']}, "
        f"summary={display_path(summary_json)})"
    )
    return 0


def main(argv: Sequence[str] | None = None) -> int:
    try:
        return check_determinism(argv)
    except DeterminismContractError as exc:
        print(f"objc3c-end-to-end-determinism: error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
