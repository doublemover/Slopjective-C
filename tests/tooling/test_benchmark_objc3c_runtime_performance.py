from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path
from types import SimpleNamespace

import pytest


ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

SCRIPT_PATH = ROOT / "scripts" / "benchmark_objc3c_runtime_performance.py"
SPEC = importlib.util.spec_from_file_location("benchmark_objc3c_runtime_performance", SCRIPT_PATH)
assert SPEC is not None and SPEC.loader is not None
benchmark = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = benchmark
SPEC.loader.exec_module(benchmark)


def _write_json(path: Path, payload: dict[str, object]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def test_runtime_performance_fixture_manifest_validates_checked_in_cases() -> None:
    manifest = benchmark.load_json(benchmark.FIXTURE_MANIFEST)

    summary = benchmark.validate_fixture_manifest(manifest)

    assert summary == {
        "contract_id": "objc3c.runtime.performance.executable.fixture.manifest.v1",
        "positive_case_count": 2,
        "negative_case_count": 2,
    }


def test_runtime_performance_fixture_manifest_rejects_missing_negative_source() -> None:
    manifest = benchmark.load_json(benchmark.FIXTURE_MANIFEST)
    manifest["negative_cases"][0]["source_path"] = "tests/tooling/fixtures/native/missing.objc3"

    with pytest.raises(RuntimeError, match="missing source_path"):
        benchmark.validate_fixture_manifest(manifest)


def test_runtime_performance_summary_includes_fixture_manifest_contract(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    root = tmp_path
    workload_manifest = root / "tests" / "tooling" / "fixtures" / "runtime_performance" / "workload_manifest.json"
    artifact_surface = root / "tests" / "tooling" / "fixtures" / "runtime_performance" / "artifact_surface.json"
    fixture_manifest = root / "tests" / "tooling" / "fixtures" / "runtime_performance" / "executable_fixture_manifest.json"
    budget_model = root / "tests" / "tooling" / "fixtures" / "performance_governance" / "budget_model.json"
    fixture_source = root / "tests" / "tooling" / "fixtures" / "native" / "dispatch.objc3"
    negative_source = root / "tests" / "tooling" / "fixtures" / "native" / "negative.objc3"
    probe = root / "tests" / "tooling" / "runtime" / "dispatch_probe.cpp"
    negative_meta = root / "tests" / "tooling" / "fixtures" / "native" / "negative.meta.json"
    summary_out = root / "tmp" / "reports" / "runtime-performance" / "benchmark-summary.json"

    fixture_source.parent.mkdir(parents=True, exist_ok=True)
    fixture_source.write_text("func main() -> i32 { return 0; }\n", encoding="utf-8")
    negative_source.write_text("fn risky() throws -> i32 { return 0; }\n", encoding="utf-8")
    probe.parent.mkdir(parents=True, exist_ok=True)
    probe.write_text("int main() { return 0; }\n", encoding="utf-8")
    _write_json(negative_meta, {"schema_version": 1})
    _write_json(
        workload_manifest,
        {
            "contract_id": "objc3c.runtime.performance.workload.manifest.v1",
            "workload_families": [
                {
                    "workload_id": "dispatch-cache",
                    "acceptance_case_id": "dispatch-fast-path",
                    "fixture": "tests/tooling/fixtures/native/dispatch.objc3",
                    "probe": "tests/tooling/runtime/dispatch_probe.cpp",
                    "hot_path_family": "dispatch-cache",
                    "measured_fields": ["case_total_ms"],
                }
            ],
        },
    )
    _write_json(artifact_surface, {"contract_id": "objc3c.runtime.performance.artifact.surface.v1"})
    _write_json(
        fixture_manifest,
        {
            "contract_id": "objc3c.runtime.performance.executable.fixture.manifest.v1",
            "schema_version": 1,
            "positive_cases": [
                {
                    "case_id": "dispatch-positive",
                    "workload_id": "dispatch-cache",
                    "source_path": "tests/tooling/fixtures/native/dispatch.objc3",
                    "probe_path": "tests/tooling/runtime/dispatch_probe.cpp",
                }
            ],
            "negative_cases": [
                {
                    "case_id": "dispatch-negative",
                    "workload_id": "dispatch-cache",
                    "source_path": "tests/tooling/fixtures/native/negative.objc3",
                    "metadata_path": "tests/tooling/fixtures/native/negative.meta.json",
                    "stable_diagnostic": {
                        "code": "O3S221",
                        "source_range": {"start_line": 1, "start_column": 1},
                    },
                }
            ],
        },
    )
    _write_json(
        budget_model,
        {
            "contract_id": "objc3c.performance.governance.budget.model.v1",
            "budget_families": [
                {
                    "budget_id": "runtime-hot-path",
                    "metric_definitions": [
                        {
                            "metric_id": "dispatch_wall_clock_ms",
                            "source_field": "workloads[dispatch-cache].summary.median_duration_ms",
                            "comparison": "max",
                            "warning_value": 100.0,
                            "blocking_value": 200.0,
                        }
                    ],
                }
            ],
        },
    )

    class FakeAcceptance:
        def ensure_native_binaries(self) -> None:
            return None

        def find_clangxx(self) -> Path:
            return root / "clang++.exe"

        def check_live_dispatch_fast_path_case(self, clangxx: Path, run_dir: Path) -> object:
            del clangxx, run_dir
            return SimpleNamespace(
                passed=True,
                summary={"case_total_ms": 1.0},
                probe="tests/tooling/runtime/dispatch_probe.cpp",
                fixture="tests/tooling/fixtures/native/dispatch.objc3",
            )

    monkeypatch.setattr(benchmark, "ROOT", root)
    monkeypatch.setattr(benchmark, "WORKLOAD_MANIFEST", workload_manifest)
    monkeypatch.setattr(benchmark, "ARTIFACT_SURFACE", artifact_surface)
    monkeypatch.setattr(benchmark, "FIXTURE_MANIFEST", fixture_manifest)
    monkeypatch.setattr(benchmark, "PERFORMANCE_BUDGET_MODEL", budget_model)
    monkeypatch.setattr(benchmark, "SUMMARY_OUT", summary_out)
    monkeypatch.setattr(benchmark, "machine_profile", lambda: {"hostname": "fixture-host"})
    monkeypatch.setattr(benchmark, "tool_versions", lambda: {"python": "3.13.0"})
    monkeypatch.setattr(benchmark, "load_runtime_acceptance_module", lambda: FakeAcceptance())
    monkeypatch.setattr(benchmark, "repo_rel", lambda path: Path(path).resolve().relative_to(root).as_posix())
    monkeypatch.setattr(
        sys,
        "argv",
        [
            "benchmark_objc3c_runtime_performance.py",
            "--summary-out",
            str(summary_out),
            "--fixture-manifest",
            str(fixture_manifest),
            "--workload-id",
            "dispatch-cache",
            "--warmup-runs",
            "0",
            "--measured-runs",
            "1",
        ],
    )

    assert benchmark.main() == 0

    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["fixture_manifest_contract_id"] == (
        "objc3c.runtime.performance.executable.fixture.manifest.v1"
    )
    assert payload["fixture_manifest_summary"]["positive_case_count"] == 1
    assert payload["fixture_manifest_summary"]["negative_case_count"] == 1
