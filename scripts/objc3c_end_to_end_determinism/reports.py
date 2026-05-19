"""Summary payload construction and terminal output rendering."""

from __future__ import annotations

import shlex
import sys
from pathlib import Path
from typing import Sequence

from objc3c_tooling.json_io import canonical_json
from objc3c_tooling.paths import display_path
from scripts.objc3c_end_to_end_determinism.constants import MODE


def build_summary_payload(
    *,
    replays: int,
    artifact_globs: Sequence[str],
    command_template: Sequence[str],
    workdir: Path,
    replay_root: Path,
    run_label: str,
    session_dir: Path,
    summary_json: Path,
    runs: list[dict[str, object]],
    failures: Sequence[dict[str, object]],
) -> dict[str, object]:
    for run in runs:
        run.pop("_digest_by_path", None)

    return {
        "mode": MODE,
        "status": "PASS" if not failures else "FAIL",
        "replays": replays,
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
        "mismatches": list(failures),
    }


def write_summary_payload(*, summary_json: Path, payload: dict[str, object]) -> None:
    summary_json.write_text(canonical_json(payload), encoding="utf-8")


def emit_failure_report(
    *,
    replays: int,
    failures: Sequence[dict[str, object]],
    replay_root: Path,
    run_label: str,
    command_template: Sequence[str],
    summary_json: Path,
) -> None:
    print(
        "objc3c-end-to-end-determinism: FAIL "
        f"(runs={replays}, mismatches={len(failures)})",
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
                str(replays),
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


def emit_success_report(*, replays: int, artifact_count: int, summary_json: Path) -> None:
    print(
        "objc3c-end-to-end-determinism: OK "
        f"(runs={replays}, artifacts={artifact_count}, "
        f"summary={display_path(summary_json)})"
    )


__all__ = [
    "build_summary_payload",
    "emit_failure_report",
    "emit_success_report",
    "write_summary_payload",
]
