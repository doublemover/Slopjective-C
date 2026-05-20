from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

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
    budget_model_path = (
        root / "tests" / "tooling" / "fixtures" / "performance_governance" / "budget_model.json"
    )
    source_path = root / "showcase" / "auroraBoard" / "main.objc3"

    portfolio_path.parent.mkdir(parents=True, exist_ok=True)
    source_path.parent.mkdir(parents=True, exist_ok=True)
    source_path.write_text("func main() -> i32 { return 0; }\n", encoding="utf-8")
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
        json.dumps(
            {
                "sample_policy": {
                    "warmup_runs": 1,
                    "measured_runs": 2,
                    "clock_source": "wall-clock-monotonic-per-step",
                    "capture_raw_samples": True,
                },
                "comparison_policy": {
                    "same_machine_required": True,
                    "same_input_family_required": True,
                    "same_checked_in_source_required": True,
                    "capture_exact_commands": True,
                    "capture_tool_versions": True,
                },
                "claimability_policy": {
                    "allowed_claim_classes": ["same-machine-raw-sample-measurement"],
                    "disallowed_claim_classes": ["cross-machine-universal"],
                    "required_claim_inputs": [
                        "checked_in_benchmark_source",
                        "approved_lab_profile",
                        "raw_sample_packets",
                    ],
                },
            }
        )
        + "\n",
        encoding="utf-8",
    )
    parameters_path.write_text(
        json.dumps({"hardware_profile_capture": {"normalization_mode": "machine-profile-ratio-plus-raw-samples"}})
        + "\n",
        encoding="utf-8",
    )
    budget_model_path.parent.mkdir(parents=True, exist_ok=True)
    budget_model_path.write_text(
        json.dumps(
            {
                "contract_id": "objc3c.performance.governance.budget.model.v1",
                "budget_families": [
                    {
                        "budget_id": "comparative-baseline",
                        "metric_definitions": [
                            {
                                "metric_id": "compile_packet_count",
                                "source_field": "derived.compile_packet_count",
                                "comparison": "min",
                                "warning_value": 3,
                                "blocking_value": 2,
                            }
                        ],
                    }
                ],
            }
        )
        + "\n",
        encoding="utf-8",
    )

    monkeypatch.setattr(benchmark, "ROOT", root)
    monkeypatch.setattr(benchmark, "PORTFOLIO_PATH", portfolio_path)
    monkeypatch.setattr(benchmark, "MEASUREMENT_POLICY_PATH", policy_path)
    monkeypatch.setattr(benchmark, "BENCHMARK_PARAMETERS_PATH", parameters_path)
    monkeypatch.setattr(benchmark, "PERFORMANCE_BUDGET_MODEL_PATH", budget_model_path)
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
        assert command[:4] == ["npm", "run", "objc3c", "--"]
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
    assert compile_packet["raw_samples"][0]["command"][:4] == ["npm", "run", "objc3c", "--"]
    assert compile_packet["reproducibility_evidence"]["workload_source"]["path"] == (
        "showcase/auroraBoard/main.objc3"
    )
    assert len(compile_packet["reproducibility_evidence"]["workload_source"]["sha256"]) == 64
    assert compile_packet["reproducibility_evidence"]["machine_profile"]["hostname"] == "fixture-host"
    assert compile_packet["reproducibility_evidence"]["regression_policy"]["budget_id"] == (
        "comparative-baseline"
    )
    assert compile_packet["reproducibility_evidence"]["regression_policy"]["thresholds"][0][
        "metric_id"
    ] == "compile_packet_count"
    assert runtime_packet["benchmark_kind"] == "runtime-wall-clock"
    assert runtime_packet["normalized_summary"]["median_duration_ms"] == 21.5
    assert runtime_packet["reproducibility_evidence"]["tool_versions"]["clang"] == "clang fixture"
