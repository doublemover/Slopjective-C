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
        "baseline": {"elapsed_ms": 120.0, "peak_rss_mb": 100.0},
        "candidate": {
            "elapsed_ms": 126.0,
            "peak_rss_mb": 104.0,
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
    assert payload["strict_mode"] is False
    assert payload["observed"]["sample_count"] == 2
    assert payload["limits"]["max_memory_regression_pct"] == 10.0


def test_contract_mode_fails_on_perf_regression(tmp_path: Path) -> None:
    telemetry_path = tmp_path / "telemetry_fail_perf.json"
    telemetry = build_happy_telemetry()
    telemetry["candidate"] = {
        "elapsed_ms": 160.0,
        "peak_rss_mb": 104.0,
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
        "peak_rss_mb": 104.0,
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


def test_strict_mode_resolves_defaults_and_writes_evidence(tmp_path: Path) -> None:
    telemetry_path = tmp_path / "telemetry_strict.json"
    evidence_path = tmp_path / "strict_evidence.json"
    telemetry = build_happy_telemetry()
    telemetry["baseline"] = {"elapsed_ms": 100.0, "peak_rss_mb": 100.0}
    telemetry["candidate"] = {
        "elapsed_ms": 104.0,
        "peak_rss_mb": 104.0,
        "determinism": {
            "run_a_elapsed_ms": 101.0,
            "run_b_elapsed_ms": 104.0,
            "run_a_artifact_sha256": (
                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            ),
            "run_b_artifact_sha256": (
                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            ),
            "run_b_cache_hit": True,
        },
    }
    telemetry["samples"] = [
        {
            "fixture": "corpus/positive/bool_branch_return.objc3",
            "baseline_elapsed_ms": 50.0,
            "candidate_elapsed_ms": 52.0,
        },
        {
            "fixture": "corpus/positive/message_dispatch_probe.m",
            "baseline_elapsed_ms": 50.0,
            "candidate_elapsed_ms": 52.0,
        },
    ]
    write_json(telemetry_path, telemetry)

    code, stdout, stderr = run_main(
        [
            "--strict",
            "--telemetry",
            str(telemetry_path),
            "--evidence-output",
            str(evidence_path),
        ]
    )
    assert code == 0
    assert stderr == ""

    payload = json.loads(stdout)
    assert payload["status"] == "PASS"
    assert payload["strict_mode"] is True
    assert payload["contract_mode"] is True
    assert payload["limits"]["max_regression_pct"] == 5.0
    assert payload["limits"]["max_jitter_ms"] == 4.0
    assert payload["limits"]["max_memory_regression_pct"] == 5.0
    assert payload["evidence_output"] == evidence_path.as_posix()

    evidence_payload = json.loads(evidence_path.read_text(encoding="utf-8"))
    assert evidence_payload == payload


def test_strict_mode_uses_default_telemetry_path(monkeypatch: object, tmp_path: Path) -> None:
    telemetry_path = tmp_path / "strict_default_telemetry.json"
    write_json(telemetry_path, build_happy_telemetry())
    monkeypatch.setattr(
        check_objc3c_refactor_perf_guard,
        "DEFAULT_STRICT_TELEMETRY",
        telemetry_path,
    )

    code, stdout, stderr = run_main(["--strict"])
    assert code == 1
    assert stderr == ""
    payload = json.loads(stdout)
    assert payload["strict_mode"] is True
    assert payload["telemetry"] == telemetry_path.as_posix()
    assert payload["status"] == "FAIL"


def test_strict_mode_rejects_relaxed_threshold_override(tmp_path: Path) -> None:
    telemetry_path = tmp_path / "strict_invalid_threshold.json"
    write_json(telemetry_path, build_happy_telemetry())
    code, stdout, stderr = run_main(
        [
            "--strict",
            "--telemetry",
            str(telemetry_path),
            "--max-regression-pct",
            "6.0",
        ]
    )
    assert code == 2
    assert stdout == ""
    assert "--max-regression-pct must be <= 5.000000 in --strict mode" in stderr


def test_strict_mode_fails_on_memory_regression(tmp_path: Path) -> None:
    telemetry_path = tmp_path / "strict_memory_drift.json"
    telemetry = build_happy_telemetry()
    telemetry["candidate"] = {
        "elapsed_ms": 124.0,
        "peak_rss_mb": 111.0,
        "determinism": telemetry["candidate"]["determinism"],  # type: ignore[index]
    }
    write_json(telemetry_path, telemetry)

    code, stdout, stderr = run_main(
        [
            "--strict",
            "--telemetry",
            str(telemetry_path),
        ]
    )
    assert code == 1
    assert stderr == ""
    payload = json.loads(stdout)
    check_ids = [item["check_id"] for item in payload["violations"]]
    assert payload["status"] == "FAIL"
    assert "PERF-003" in check_ids
