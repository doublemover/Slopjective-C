from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

SCRIPT_PATH = (
    Path(__file__).resolve().parents[2]
    / "scripts"
    / "check_objc3c_end_to_end_determinism.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_objc3c_end_to_end_determinism",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_objc3c_end_to_end_determinism.py")
checker = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = checker
SPEC.loader.exec_module(checker)


def deterministic_command() -> str:
    return (
        "from pathlib import Path; "
        "p = Path(r'{run_dir}') / 'artifact.txt'; "
        "p.parent.mkdir(parents=True, exist_ok=True); "
        "p.write_text('stable\\n', encoding='utf-8')"
    )


def drift_command() -> str:
    return (
        "from pathlib import Path; "
        "p = Path(r'{run_dir}') / 'artifact.txt'; "
        "p.parent.mkdir(parents=True, exist_ok=True); "
        "p.write_text('{run_id}\\n', encoding='utf-8')"
    )


def run_checker(tmp_path: Path, *, command_python: str, run_label: str) -> tuple[int, Path]:
    summary_json = tmp_path / f"{run_label}_summary.json"
    exit_code = checker.main(
        [
            "--replays",
            "2",
            "--artifact-glob",
            "**/*.txt",
            "--replay-root",
            str(tmp_path / "replay"),
            "--run-label",
            run_label,
            "--summary-json",
            str(summary_json),
            "--",
            sys.executable,
            "-c",
            command_python,
        ]
    )
    return exit_code, summary_json


def test_checker_passes_for_deterministic_replay_and_emits_digest_evidence(
    tmp_path: Path,
) -> None:
    code, summary_json = run_checker(
        tmp_path,
        command_python=deterministic_command(),
        run_label="pass_case",
    )
    assert code == 0
    assert summary_json.is_file()

    payload = json.loads(summary_json.read_text(encoding="utf-8"))
    assert list(payload.keys()) == [
        "mode",
        "status",
        "replays",
        "artifact_globs",
        "command_template",
        "session",
        "runs",
        "mismatches",
    ]
    assert payload["mode"] == "objc3c-end-to-end-determinism-v1"
    assert payload["status"] == "PASS"
    assert payload["replays"] == 2
    assert payload["artifact_globs"] == ["**/*.txt"]
    assert payload["mismatches"] == []

    runs = payload["runs"]
    assert len(runs) == 2
    assert runs[0]["run_id"] == "run01"
    assert runs[1]["run_id"] == "run02"
    assert runs[0]["artifact_count"] == 1
    assert runs[1]["artifact_count"] == 1
    assert runs[0]["corpus_sha256"] == runs[1]["corpus_sha256"]
    assert runs[0]["artifacts"] == runs[1]["artifacts"]


def test_checker_fails_when_artifact_digest_drifts_across_replays(tmp_path: Path) -> None:
    code, summary_json = run_checker(
        tmp_path,
        command_python=drift_command(),
        run_label="drift_case",
    )
    assert code == 1
    assert summary_json.is_file()

    payload = json.loads(summary_json.read_text(encoding="utf-8"))
    assert payload["status"] == "FAIL"
    mismatches = payload["mismatches"]
    assert len(mismatches) == 1
    mismatch = mismatches[0]
    assert mismatch["kind"] == "artifact-digest-mismatch"
    assert mismatch["run_id"] == "run02"
    assert mismatch["path"] == "artifact.txt"
    assert mismatch["baseline_sha256"] != mismatch["replay_sha256"]


def test_checker_rejects_variant_env_length_mismatch(capsys: object) -> None:
    exit_code = checker.main(
        [
            "--replays",
            "2",
            "--variant-env",
            "PYTHONHASHSEED=1",
            "--",
            sys.executable,
            "-c",
            deterministic_command(),
        ]
    )
    assert exit_code == 1
    captured = capsys.readouterr()
    assert "variant env value count mismatch" in captured.err
