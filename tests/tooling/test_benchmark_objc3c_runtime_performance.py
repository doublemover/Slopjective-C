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


def _runtime_workload_rows(payload: dict[str, object]) -> dict[str, dict[str, object]]:
    return {
        str(row["workload_id"]): row
        for row in payload.get("workload_families", [])
        if isinstance(row, dict) and row.get("workload_id")
    }


def _write_temp_runtime_contracts(root: Path) -> None:
    runtime_root = root / "tests" / "tooling" / "fixtures" / "runtime_performance"
    malformed_metadata = runtime_root / "malformed_metadata" / "truncated_dispatch.meta.json"
    probe_script = root / "scripts" / "probe_objc3c_runtime_scale_evidence.py"
    malformed_metadata.parent.mkdir(parents=True, exist_ok=True)
    malformed_metadata.write_text('{"schema_version": 1, "stable_diagnostic":', encoding="utf-8")
    probe_script.parent.mkdir(parents=True, exist_ok=True)
    probe_script.write_text("#!/usr/bin/env python3\n", encoding="utf-8")
    _write_json(
        runtime_root / "workload_replay_contract.json",
        {
            "contract_id": "objc3c.runtime.performance.workload.replay.contract.v1",
            "schema_version": 1,
            "replay_policy": {
                "replay_key_fields": [
                    "workload_id",
                    "acceptance_case_id",
                    "hot_path_family",
                    "fixture_sha256",
                    "probe_sha256",
                    "budget_metric_id",
                ]
            },
            "workload_replay_rows": [
                {
                    "workload_id": "dispatch-cache",
                    "acceptance_case_id": "dispatch-fast-path",
                    "fixture": "tests/tooling/fixtures/native/dispatch.objc3",
                    "probe": "tests/tooling/runtime/dispatch_probe.cpp",
                    "hot_path_family": "dispatch-cache",
                    "min_replay_samples": 1,
                    "required_summary_fields": ["case_total_ms"],
                    "allowed_nondeterministic_packet_fields": ["duration_ms", "run_dir"],
                }
            ],
        },
    )
    _write_json(
        runtime_root / "metadata_resilience_contract.json",
        {
            "contract_id": "objc3c.runtime.performance.metadata.resilience.contract.v1",
            "schema_version": 1,
            "malformed_metadata_cases": [
                {
                    "case_id": "dispatch-malformed-metadata",
                    "workload_id": "dispatch-cache",
                    "source_path": "tests/tooling/fixtures/native/negative.objc3",
                    "metadata_path": "tests/tooling/fixtures/runtime_performance/malformed_metadata/truncated_dispatch.meta.json",
                    "malformed_kind": "invalid-json",
                    "stable_diagnostic": {
                        "code": "O3PERF8159",
                        "source_range": {"start_line": 1, "start_column": 1},
                    },
                    "expected_handling": {
                        "fail_closed": True,
                        "packet_written": False,
                    },
                }
            ],
            "metadata_fuzz_contracts": [
                {
                    "fuzz_id": "dispatch-malformed-corpus",
                    "corpus_paths": [
                        "tests/tooling/fixtures/runtime_performance/malformed_metadata/truncated_dispatch.meta.json"
                    ],
                    "max_payload_bytes": 4096,
                    "support_authority": False,
                }
            ],
        },
    )
    _write_json(
        runtime_root / "stress_sanitizer_contract.json",
        {
            "contract_id": "objc3c.runtime.performance.stress.sanitizer.contract.v1",
            "schema_version": 1,
            "scale_evidence_probe": {
                "probe_script": "scripts/probe_objc3c_runtime_scale_evidence.py",
                "summary_report": "tmp/reports/runtime-performance/scale-summary.json",
                "support_authority": False,
            },
            "sanitizer_contracts": [
                {
                    "sanitizer_id": "dispatch-asan",
                    "workload_id": "dispatch-cache",
                    "targeted_command": (
                        "npm run objc3c -- benchmark-runtime-performance -- "
                        "--workload-id dispatch-cache --warmup-runs 0 --measured-runs 3"
                    ),
                    "opt_in_only": True,
                    "required_failure_modes": ["nonzero-exit-on-sanitizer-finding"],
                    "support_authority": False,
                }
            ],
            "stress_scale_contracts": [
                {
                    "stress_id": "dispatch-replay-scale",
                    "workload_id": "dispatch-cache",
                    "scale_factor": 8,
                    "min_measured_runs": 3,
                    "packet_invariants": ["same-replay-key-across-samples"],
                    "support_authority": False,
                }
            ],
        },
    )


def test_runtime_performance_fixture_manifest_validates_checked_in_cases() -> None:
    manifest = benchmark.load_json(benchmark.FIXTURE_MANIFEST)

    summary = benchmark.validate_fixture_manifest(manifest)

    assert summary == {
        "contract_id": "objc3c.runtime.performance.executable.fixture.manifest.v1",
        "positive_case_count": 9,
        "negative_case_count": 2,
    }


def test_runtime_performance_fixture_manifest_rejects_missing_negative_source() -> None:
    manifest = benchmark.load_json(benchmark.FIXTURE_MANIFEST)
    manifest["negative_cases"][0]["source_path"] = "tests/tooling/fixtures/native/missing.objc3"

    with pytest.raises(RuntimeError, match="missing source_path"):
        benchmark.validate_fixture_manifest(manifest)


def test_runtime_performance_contract_surfaces_validate_checked_in_contracts() -> None:
    workload_manifest = benchmark.load_json(benchmark.WORKLOAD_MANIFEST)
    summary = benchmark.validate_runtime_performance_contracts(
        replay_contract=benchmark.load_json(benchmark.REPLAY_CONTRACT),
        metadata_resilience_contract=benchmark.load_json(benchmark.METADATA_RESILIENCE_CONTRACT),
        stress_sanitizer_contract=benchmark.load_json(benchmark.STRESS_SANITIZER_CONTRACT),
        workload_rows=_runtime_workload_rows(workload_manifest),
    )

    assert summary["replay"]["row_count"] == len(_runtime_workload_rows(workload_manifest))
    assert summary["metadata_resilience"]["invalid_json_case_count"] == 1
    assert summary["metadata_resilience"]["fuzz_contract_count"] == 1
    assert summary["stress_sanitizer"]["sanitizer_contract_count"] == 4
    assert summary["stress_sanitizer"]["stress_scale_contract_count"] == 8
    assert summary["stress_sanitizer"]["scale_evidence_probe"]["summary_report"] == (
        "tmp/reports/runtime-performance/scale-summary.json"
    )
    assert {row["path"] for row in summary["contract_files"]} == {
        "tests/tooling/fixtures/runtime_performance/workload_replay_contract.json",
        "tests/tooling/fixtures/runtime_performance/metadata_resilience_contract.json",
        "tests/tooling/fixtures/runtime_performance/stress_sanitizer_contract.json",
    }


def test_runtime_performance_replay_contract_rejects_workload_drift() -> None:
    workload_manifest = benchmark.load_json(benchmark.WORKLOAD_MANIFEST)
    replay_contract = benchmark.load_json(benchmark.REPLAY_CONTRACT)
    replay_contract["workload_replay_rows"][0]["fixture"] = "tests/tooling/fixtures/native/drift.objc3"

    with pytest.raises(RuntimeError, match="drifted field fixture"):
        benchmark.validate_workload_replay_contract(
            replay_contract,
            workload_rows=_runtime_workload_rows(workload_manifest),
        )


def test_runtime_performance_replay_contract_rejects_unpublished_summary_field() -> None:
    workload_manifest = benchmark.load_json(benchmark.WORKLOAD_MANIFEST)
    replay_contract = benchmark.load_json(benchmark.REPLAY_CONTRACT)
    replay_contract["workload_replay_rows"][0]["required_summary_fields"].append("not_in_manifest")

    with pytest.raises(RuntimeError, match="requires fields absent from workload manifest"):
        benchmark.validate_workload_replay_contract(
            replay_contract,
            workload_rows=_runtime_workload_rows(workload_manifest),
        )


def test_runtime_performance_replay_sample_rejects_missing_required_summary_field() -> None:
    replay_contract = benchmark.load_json(benchmark.REPLAY_CONTRACT)

    with pytest.raises(RuntimeError, match="missing replay summary fields"):
        benchmark.validate_replay_sample_summary(
            replay_contract=replay_contract,
            workload_id="dispatch-cache",
            sample_summary={"baseline_cache_entry_count": 1},
        )


def test_runtime_performance_metadata_resilience_rejects_valid_json_for_invalid_case(tmp_path: Path) -> None:
    source_path = tmp_path / "tests" / "tooling" / "fixtures" / "native" / "negative.objc3"
    metadata_path = tmp_path / "tests" / "tooling" / "fixtures" / "native" / "valid.meta.json"
    source_path.parent.mkdir(parents=True, exist_ok=True)
    source_path.write_text("func main() -> i32 { return 0; }\n", encoding="utf-8")
    _write_json(metadata_path, {"schema_version": 1})

    with pytest.raises(RuntimeError, match="expected invalid JSON"):
        benchmark.validate_metadata_resilience_contract(
            {
                "contract_id": "objc3c.runtime.performance.metadata.resilience.contract.v1",
                "schema_version": 1,
                "malformed_metadata_cases": [
                    {
                        "case_id": "valid-json-drift",
                        "workload_id": "dispatch-cache",
                        "source_path": "tests/tooling/fixtures/native/negative.objc3",
                        "metadata_path": "tests/tooling/fixtures/native/valid.meta.json",
                        "malformed_kind": "invalid-json",
                        "stable_diagnostic": {
                            "code": "O3PERF8159",
                            "source_range": {"start_line": 1, "start_column": 1},
                        },
                        "expected_handling": {
                            "fail_closed": True,
                            "packet_written": False,
                        },
                    }
                ],
                "metadata_fuzz_contracts": [],
            },
            root=tmp_path,
        )


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
    _write_temp_runtime_contracts(root)
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
    assert payload["runtime_contract_summary"]["replay"]["row_count"] == 1
    assert payload["runtime_contract_summary"]["metadata_resilience"]["invalid_json_case_count"] == 1
    assert payload["workloads"][0]["reproducibility_evidence"]["replay_key"]["workload_id"] == "dispatch-cache"
    assert payload["workloads"][0]["reproducibility_evidence"]["contract_evidence"]
    packet = json.loads((root / payload["packet_paths"][0]).read_text(encoding="utf-8"))
    assert packet["replay_key"]["workload_id"] == "dispatch-cache"
