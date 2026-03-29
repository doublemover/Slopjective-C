from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "benchmark_objc3c_runtime_inspector.py"
SPEC = importlib.util.spec_from_file_location("benchmark_objc3c_runtime_inspector", SCRIPT_PATH)
assert SPEC is not None and SPEC.loader is not None
benchmark = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = benchmark
SPEC.loader.exec_module(benchmark)


def test_benchmark_writes_reproducible_summary(tmp_path: Path, monkeypatch) -> None:
    root = tmp_path
    workspace_path = root / "tmp" / "artifacts" / "playground" / "fixture-abc123" / "workspace.json"
    runtime_inspector_path = root / "tmp" / "reports" / "objc3c-public-workflow" / "runtime-inspector.json"
    capability_explorer_path = root / "tmp" / "reports" / "objc3c-public-workflow" / "capability-explorer.json"
    summary_out = root / "tmp" / "reports" / "objc3c-public-workflow" / "runtime-inspector-benchmark.json"
    object_path = root / "tmp" / "artifacts" / "playground" / "fixture-abc123" / "build" / "module.obj"

    workspace_path.parent.mkdir(parents=True, exist_ok=True)
    runtime_inspector_path.parent.mkdir(parents=True, exist_ok=True)
    capability_explorer_path.parent.mkdir(parents=True, exist_ok=True)
    object_path.parent.mkdir(parents=True, exist_ok=True)

    workspace_path.write_text(
        json.dumps(
            {
                "contract_id": "objc3c.playground.workspace.v1",
                "source_path": "tests/tooling/fixtures/native/hello.objc3",
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    runtime_inspector_path.write_text(
        json.dumps(
            {
                "contract_id": "objc3c.runtime.metadata.object.inspection.harness.v1",
                "object_path": "tmp/artifacts/playground/fixture-abc123/build/module.obj",
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    capability_explorer_path.write_text(
        json.dumps(
            {
                "mode": "objc3c-llvm-capabilities-v2",
                "ok": True,
                "sema_type_system_parity": {"parity_ready": True},
                "clang": {"version_duration_ms": 1.25},
                "llc": {"version_duration_ms": 2.5},
                "llc_features": {
                    "help_duration_ms": 0.5,
                    "version_with_filetype_duration_ms": 0.75,
                },
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    object_path.write_bytes(b"OBJC3")

    monkeypatch.setattr(benchmark, "ROOT", root)
    monkeypatch.setattr(benchmark, "PUBLIC_RUNNER", root / "scripts" / "objc3c_public_workflow_runner.py")
    monkeypatch.setattr(benchmark, "DEFAULT_SOURCE", root / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3")
    monkeypatch.setattr(benchmark, "SUMMARY_OUT", summary_out)

    def fake_run_step(name: str, command: list[str]) -> dict[str, object]:
        stdout = ""
        if name == "materialize-playground-workspace":
            stdout = "workspace_path: tmp/artifacts/playground/fixture-abc123/workspace.json\n"
        return {
            "name": name,
            "command": command,
            "exit_code": 0,
            "duration_ms": 12.5,
            "stdout": stdout,
        }

    monkeypatch.setattr(benchmark, "run_step", fake_run_step)
    monkeypatch.setattr(sys, "argv", ["benchmark_objc3c_runtime_inspector.py"])

    exit_code = benchmark.main()

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["contract_id"] == "objc3c.runtime.inspector.benchmark.v1"
    assert payload["ok"] is True
    assert payload["reports"]["workspace"] == "tmp/artifacts/playground/fixture-abc123/workspace.json"
    assert payload["measurements"]["runtime_inspector_object_size_bytes"] == 5
    assert payload["measurements"]["capability_probe_durations_ms"]["clang_version"] == 1.25
