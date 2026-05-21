from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path
from types import SimpleNamespace
from typing import Sequence


ROOT = Path(__file__).resolve().parents[2]
for search_path in (ROOT, ROOT / "scripts"):
    if str(search_path) not in sys.path:
        sys.path.insert(0, str(search_path))

SCRIPT_PATH = ROOT / "scripts" / "check_objc3c_runtime_performance_integration.py"
SPEC = importlib.util.spec_from_file_location(
    "check_objc3c_runtime_performance_integration", SCRIPT_PATH
)
assert SPEC is not None and SPEC.loader is not None
checker = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = checker
SPEC.loader.exec_module(checker)


def _write_json(path: Path, payload: dict[str, object]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def test_runtime_performance_integration_runs_scale_evidence_probe(
    tmp_path: Path, monkeypatch
) -> None:
    runtime_fixtures = tmp_path / "tests" / "tooling" / "fixtures" / "runtime_performance"
    reports = tmp_path / "tmp" / "reports" / "runtime-performance"
    packet_path = reports / "dispatch-cache-packet.json"
    scale_probe = tmp_path / "scripts" / "probe_objc3c_runtime_scale_evidence.py"

    _write_json(
        runtime_fixtures / "source_surface.json",
        {
            "contract_id": "objc3c.runtime.performance.source.surface.v1",
            "hot_path_families": ["dispatch-cache"],
        },
    )
    _write_json(
        runtime_fixtures / "workload_manifest.json",
        {
            "contract_id": "objc3c.runtime.performance.workload.manifest.v1",
            "workload_families": [{"workload_id": "dispatch-cache"}],
        },
    )
    _write_json(
        runtime_fixtures / "artifact_surface.json",
        {
            "contract_id": "objc3c.runtime.performance.artifact.surface.v1",
            "required_packet_fields": ["contract_id", "workload_id"],
        },
    )
    _write_json(
        reports / "benchmark-summary.json",
        {
            "contract_id": "objc3c.runtime.performance.summary.v1",
            "workload_manifest_contract_id": "objc3c.runtime.performance.workload.manifest.v1",
            "artifact_surface_contract_id": "objc3c.runtime.performance.artifact.surface.v1",
            "selected_workload_ids": ["dispatch-cache"],
            "packet_paths": ["tmp/reports/runtime-performance/dispatch-cache-packet.json"],
            "workloads": [
                {
                    "workload_id": "dispatch-cache",
                    "summary": {"sample_count": 1},
                }
            ],
        },
    )
    _write_json(
        packet_path,
        {
            "contract_id": "objc3c.runtime.performance.packet.v1",
            "workload_id": "dispatch-cache",
        },
    )
    _write_json(
        reports / "runnable-end-to-end-summary.json",
        {
            "contract_id": "objc3c.runtime.performance.runnable.end.to.end.summary.v1",
        },
    )
    _write_json(
        reports / "scale-summary.json",
        {
            "contract_id": "objc3c.runtime.performance.scale.evidence.summary.v1",
            "status": "PASS",
            "support_authority": False,
            "summary_counts": {
                "stress_scale": 8,
                "scale_scenarios": 4,
            },
        },
    )

    monkeypatch.setattr(checker, "ROOT", tmp_path)
    monkeypatch.setattr(checker, "SOURCE_SURFACE", runtime_fixtures / "source_surface.json")
    monkeypatch.setattr(checker, "WORKLOAD_MANIFEST", runtime_fixtures / "workload_manifest.json")
    monkeypatch.setattr(checker, "ARTIFACT_SURFACE", runtime_fixtures / "artifact_surface.json")
    monkeypatch.setattr(checker, "BENCHMARK_SUMMARY", reports / "benchmark-summary.json")
    monkeypatch.setattr(checker, "RUNNABLE_SUMMARY", reports / "runnable-end-to-end-summary.json")
    monkeypatch.setattr(checker, "SCALE_EVIDENCE_SUMMARY", reports / "scale-summary.json")
    monkeypatch.setattr(checker, "SCALE_EVIDENCE_PROBE", scale_probe)
    monkeypatch.setattr(checker, "REPORT_PATH", reports / "integration-summary.json")
    monkeypatch.setattr(
        checker,
        "repo_rel",
        lambda path: Path(path).relative_to(tmp_path).as_posix(),
    )
    monkeypatch.setattr(
        checker,
        "public_workflow_command",
        lambda *args: ["npm", "run", "objc3c", "--", *args],
    )

    commands: list[Sequence[str]] = []

    def fake_run_capture(command: Sequence[str]) -> SimpleNamespace:
        commands.append(command)
        return SimpleNamespace(returncode=0)

    monkeypatch.setattr(checker, "run_capture", fake_run_capture)

    assert checker.main() == 0

    scale_commands = [
        command
        for command in commands
        if any("probe_objc3c_runtime_scale_evidence.py" in str(part) for part in command)
    ]
    assert scale_commands == [[sys.executable, str(scale_probe)]]

    report = json.loads((reports / "integration-summary.json").read_text(encoding="utf-8"))
    assert report["status"] == "PASS"
    assert "tmp/reports/runtime-performance/scale-summary.json" in report[
        "child_report_paths"
    ]
