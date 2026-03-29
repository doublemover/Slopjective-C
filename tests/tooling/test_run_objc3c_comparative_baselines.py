from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "run_objc3c_comparative_baselines.py"
SPEC = importlib.util.spec_from_file_location("run_objc3c_comparative_baselines", SCRIPT_PATH)
assert SPEC is not None and SPEC.loader is not None
runner = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = runner
SPEC.loader.exec_module(runner)


def test_runner_writes_compile_and_runtime_packets(tmp_path: Path, monkeypatch) -> None:
    root = tmp_path
    summary_out = root / "tmp" / "reports" / "performance" / "comparative-baselines-summary.json"
    manifest_path = root / "tests" / "tooling" / "fixtures" / "performance" / "comparative_baseline_manifest.json"
    policy_path = root / "tests" / "tooling" / "fixtures" / "performance" / "measurement_policy.json"
    parameters_path = root / "tests" / "tooling" / "fixtures" / "performance" / "benchmark_parameters.json"

    manifest_path.parent.mkdir(parents=True, exist_ok=True)
    manifest_path.write_text(
        json.dumps(
            {
                "baseline_entries": [
                    {
                        "baseline_id": "cpp_reference",
                        "language": "c++",
                        "source": "tests/tooling/fixtures/performance/baselines/cpp_reference_workload.cpp",
                        "availability_probe": {"tool": "clang++", "args": ["--version"]},
                        "compile_invocation": {"tool": "clang++", "args": ["{source}", "-o", "{output_exe}"]},
                        "runtime_invocation": {"tool": "{output_exe}", "args": []},
                        "supports_runtime_baseline": True,
                    }
                ]
            }
        )
        + "\n",
        encoding="utf-8",
    )
    policy_path.write_text(json.dumps({"sample_policy": {"warmup_runs": 1, "measured_runs": 2}}) + "\n", encoding="utf-8")
    parameters_path.write_text(
        json.dumps({"hardware_profile_capture": {"normalization_mode": "machine-profile-ratio-plus-raw-samples"}})
        + "\n",
        encoding="utf-8",
    )

    monkeypatch.setattr(runner, "ROOT", root)
    monkeypatch.setattr(runner, "MANIFEST_PATH", manifest_path)
    monkeypatch.setattr(runner, "MEASUREMENT_POLICY_PATH", policy_path)
    monkeypatch.setattr(runner, "BENCHMARK_PARAMETERS_PATH", parameters_path)
    monkeypatch.setattr(runner, "SUMMARY_OUT", summary_out)
    monkeypatch.setattr(
        runner,
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

    durations = iter([1.0, 9.0, 10.0, 11.0, 19.0, 20.0, 21.0])

    def fake_run_timed_step(command: list[str]) -> dict[str, object]:
        return {
            "command": command,
            "exit_code": 0,
            "duration_ms": next(durations),
            "stdout": "ok\n",
            "stderr": "",
        }

    monkeypatch.setattr(runner, "run_timed_step", fake_run_timed_step)
    monkeypatch.setattr(sys, "argv", ["run_objc3c_comparative_baselines.py", "--warmup-runs", "1", "--measured-runs", "2"])

    exit_code = runner.main()

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["contract_id"] == "objc3c.performance.comparative.baselines.summary.v1"
    assert payload["status"] == "PASS"
    assert payload["telemetry_packets"] == [
        "tmp/reports/performance/baselines/compile/cpp_reference.json",
        "tmp/reports/performance/baselines/runtime/cpp_reference.json",
    ]
