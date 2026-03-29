from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "benchmark_objc3c_performance.py"
SPEC = importlib.util.spec_from_file_location("benchmark_objc3c_performance", SCRIPT_PATH)
assert SPEC is not None and SPEC.loader is not None
benchmark = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = benchmark
SPEC.loader.exec_module(benchmark)


def test_benchmark_writes_compile_and_runtime_packets(tmp_path: Path, monkeypatch) -> None:
    root = tmp_path
    summary_out = root / "tmp" / "reports" / "performance" / "benchmark-summary.json"
    portfolio_path = root / "tests" / "tooling" / "fixtures" / "performance" / "benchmark_portfolio.json"
    policy_path = root / "tests" / "tooling" / "fixtures" / "performance" / "measurement_policy.json"
    parameters_path = root / "tests" / "tooling" / "fixtures" / "performance" / "benchmark_parameters.json"

    portfolio_path.parent.mkdir(parents=True, exist_ok=True)
    portfolio_path.write_text(
        json.dumps(
            {
                "objc3_workloads": [
                    {
                        "workload_id": "auroraBoard",
                        "source": "showcase/auroraBoard/main.objc3",
                    }
                ]
            }
        )
        + "\n",
        encoding="utf-8",
    )
    policy_path.write_text(
        json.dumps({"sample_policy": {"warmup_runs": 1, "measured_runs": 2}}) + "\n",
        encoding="utf-8",
    )
    parameters_path.write_text(
        json.dumps({"hardware_profile_capture": {"normalization_mode": "machine-profile-ratio-plus-raw-samples"}})
        + "\n",
        encoding="utf-8",
    )

    monkeypatch.setattr(benchmark, "ROOT", root)
    monkeypatch.setattr(benchmark, "PUBLIC_RUNNER", root / "scripts" / "objc3c_public_workflow_runner.py")
    monkeypatch.setattr(benchmark, "PORTFOLIO_PATH", portfolio_path)
    monkeypatch.setattr(benchmark, "MEASUREMENT_POLICY_PATH", policy_path)
    monkeypatch.setattr(benchmark, "BENCHMARK_PARAMETERS_PATH", parameters_path)
    monkeypatch.setattr(benchmark, "SUMMARY_OUT", summary_out)

    def fake_run_capture(command: list[str]):
        class Result:
            def __init__(self) -> None:
                self.returncode = 0
                self.stdout = "ok\n"
                self.stderr = ""

        return Result()

    durations = iter([10.0, 11.0, 12.0, 20.0, 21.0, 22.0])

    def fake_run_timed_step(command: list[str]) -> dict[str, object]:
        return {
            "command": command,
            "exit_code": 0,
            "duration_ms": next(durations),
            "stdout": "ok\n",
            "stderr": "",
        }

    monkeypatch.setattr(benchmark, "run_capture", fake_run_capture)
    monkeypatch.setattr(benchmark, "run_timed_step", fake_run_timed_step)
    monkeypatch.setattr(
        benchmark,
        "machine_profile",
        lambda: {
            "hostname": "fixture-host",
            "os": "fixture-os",
            "arch": "x64",
            "cpu_model": "fixture-cpu",
            "cpu_count": 8,
            "python_version": "3.13.0",
        },
    )
    monkeypatch.setattr(benchmark, "tool_versions", lambda: {"python": "3.13.0", "clang": "clang fixture"})
    monkeypatch.setattr(sys, "argv", ["benchmark_objc3c_performance.py", "--warmup-runs", "1", "--measured-runs", "2"])

    exit_code = benchmark.main()

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["contract_id"] == "objc3c.performance.benchmark.summary.v1"
    assert payload["status"] == "PASS"
    assert payload["telemetry_packets"] == [
        "tmp/reports/performance/compile/auroraBoard.json",
        "tmp/reports/performance/runtime/auroraBoard.json",
    ]

    compile_packet = json.loads((root / "tmp" / "reports" / "performance" / "compile" / "auroraBoard.json").read_text(encoding="utf-8"))
    runtime_packet = json.loads((root / "tmp" / "reports" / "performance" / "runtime" / "auroraBoard.json").read_text(encoding="utf-8"))
    assert compile_packet["benchmark_kind"] == "compile-latency"
    assert compile_packet["normalized_summary"]["sample_count"] == 2
    assert runtime_packet["benchmark_kind"] == "runtime-wall-clock"
    assert runtime_packet["normalized_summary"]["median_duration_ms"] == 21.5
