from __future__ import annotations

import importlib.util
import io
import json
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

SCRIPT_PATH = Path(__file__).resolve().parents[2] / "scripts" / "check_objc3c_refactor_perf_guard.py"
SPEC = importlib.util.spec_from_file_location("check_objc3c_refactor_perf_guard", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_objc3c_refactor_perf_guard.py")
check_objc3c_refactor_perf_guard = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = check_objc3c_refactor_perf_guard
SPEC.loader.exec_module(check_objc3c_refactor_perf_guard)


def run_main(argv: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = check_objc3c_refactor_perf_guard.main(argv)
    return code, stdout.getvalue(), stderr.getvalue()


def write_json(path: Path, payload: dict[str, object]) -> None:
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def build_happy_telemetry() -> dict[str, object]:
    return {
        "schema_version": "objc3c-refactor-perf-telemetry.v1",
        "baseline": {"elapsed_ms": 120.0},
        "candidate": {
            "elapsed_ms": 126.0,
            "determinism": {
                "run_a_elapsed_ms": 124.0,
                "run_b_elapsed_ms": 126.0,
                "run_a_artifact_sha256": (
                    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                ),
                "run_b_artifact_sha256": (
                    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                ),
                "run_b_cache_hit": True,
            },
        },
        "samples": [
            {
                "fixture": "corpus/positive/bool_branch_return.objc3",
                "baseline_elapsed_ms": 41.0,
                "candidate_elapsed_ms": 43.0,
            },
            {
                "fixture": "corpus/positive/message_dispatch_probe.m",
                "baseline_elapsed_ms": 79.0,
                "candidate_elapsed_ms": 83.0,
            },
        ],
    }


def test_contract_mode_happy_path_is_deterministic(tmp_path: Path) -> None:
    telemetry_path = tmp_path / "telemetry.json"
    write_json(telemetry_path, build_happy_telemetry())

    args = ["--telemetry", str(telemetry_path), "--contract-mode"]
    first_code, first_stdout, first_stderr = run_main(args)
    second_code, second_stdout, second_stderr = run_main(args)

    assert first_code == 0
    assert second_code == 0
    assert first_stderr == ""
    assert second_stderr == ""
    assert first_stdout == second_stdout

    payload = json.loads(first_stdout)
    assert payload["mode"] == check_objc3c_refactor_perf_guard.MODE
    assert payload["status"] == "PASS"
    assert payload["violation_count"] == 0
    assert payload["violations"] == []
    assert payload["contract_mode"] is True
    assert payload["observed"]["sample_count"] == 2


def test_contract_mode_fails_on_perf_regression(tmp_path: Path) -> None:
    telemetry_path = tmp_path / "telemetry_fail_perf.json"
    telemetry = build_happy_telemetry()
    telemetry["candidate"] = {
        "elapsed_ms": 160.0,
        "determinism": telemetry["candidate"]["determinism"],  # type: ignore[index]
    }
    telemetry["samples"] = [
        {
            "fixture": "corpus/positive/bool_branch_return.objc3",
            "baseline_elapsed_ms": 41.0,
            "candidate_elapsed_ms": 60.0,
        },
        {
            "fixture": "corpus/positive/message_dispatch_probe.m",
            "baseline_elapsed_ms": 79.0,
            "candidate_elapsed_ms": 98.0,
        },
    ]
    write_json(telemetry_path, telemetry)

    code, stdout, stderr = run_main(
        [
            "--telemetry",
            str(telemetry_path),
            "--contract-mode",
            "--max-regression-pct",
            "10.0",
        ]
    )

    assert code == 1
    assert stderr == ""
    payload = json.loads(stdout)
    check_ids = [item["check_id"] for item in payload["violations"]]
    assert payload["status"] == "FAIL"
    assert payload["violation_count"] >= 3
    assert "PERF-001" in check_ids
    assert "PERF-002" in check_ids


def test_contract_mode_fails_on_determinism_drift(tmp_path: Path) -> None:
    telemetry_path = tmp_path / "telemetry_fail_det.json"
    telemetry = build_happy_telemetry()
    telemetry["candidate"] = {
        "elapsed_ms": 126.0,
        "determinism": {
            "run_a_elapsed_ms": 110.0,
            "run_b_elapsed_ms": 126.0,
            "run_a_artifact_sha256": (
                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            ),
            "run_b_artifact_sha256": (
                "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
            ),
            "run_b_cache_hit": False,
        },
    }
    write_json(telemetry_path, telemetry)

    code, stdout, stderr = run_main(
        [
            "--telemetry",
            str(telemetry_path),
            "--contract-mode",
            "--max-jitter-ms",
            "8.0",
        ]
    )

    assert code == 1
    assert stderr == ""
    payload = json.loads(stdout)
    check_ids = [item["check_id"] for item in payload["violations"]]
    assert payload["status"] == "FAIL"
    assert "DET-001" in check_ids
    assert "DET-002" in check_ids
    assert "DET-003" in check_ids


def test_missing_telemetry_file_returns_input_error() -> None:
    code, stdout, stderr = run_main(["--telemetry", "missing/perf_telemetry.json"])
    assert code == 2
    assert stdout == ""
    assert "telemetry file not found" in stderr
