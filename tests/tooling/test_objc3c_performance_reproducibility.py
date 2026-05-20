from __future__ import annotations

import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_performance_reproducibility import (
    build_runtime_workload_reproducibility_evidence,
    build_workload_reproducibility_evidence,
)


def _measurement_policy() -> dict[str, object]:
    return {
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
            "required_claim_inputs": ["checked_in_benchmark_source", "raw_sample_packets"],
        },
    }


def _budget_model() -> dict[str, object]:
    return {
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
            },
            {
                "budget_id": "runtime-hot-path",
                "metric_definitions": [
                    {
                        "metric_id": "dispatch_wall_clock_ms",
                        "source_field": "workloads[dispatch-cache].summary.median_duration_ms",
                        "comparison": "max",
                        "warning_value": 3300.0,
                        "blocking_value": 3800.0,
                    }
                ],
            },
        ],
    }


def test_workload_reproducibility_evidence_includes_source_policy_and_thresholds(tmp_path: Path) -> None:
    source = tmp_path / "showcase" / "auroraBoard" / "main.objc3"
    source.parent.mkdir(parents=True)
    source.write_text("func main() -> i32 { return 0; }\n", encoding="utf-8")

    evidence = build_workload_reproducibility_evidence(
        root=tmp_path,
        workload={"workload_id": "auroraBoard", "source": "showcase/auroraBoard/main.objc3"},
        measurement_policy=_measurement_policy(),
        benchmark_parameters={"hardware_profile_capture": {"normalization_mode": "raw-samples"}},
        budget_model=_budget_model(),
        budget_id="comparative-baseline",
        profile={"hostname": "fixture-host"},
        versions={"python": "3.13.0"},
    )

    assert evidence["contract_id"] == "objc3c.performance.reproducibility.evidence.v1"
    assert evidence["workload_source"]["path"] == "showcase/auroraBoard/main.objc3"
    assert len(evidence["workload_source"]["sha256"]) == 64
    assert evidence["machine_profile"]["hostname"] == "fixture-host"
    assert evidence["regression_policy"]["thresholds"][0]["metric_id"] == "compile_packet_count"


def test_reproducibility_evidence_rejects_missing_workload_source(tmp_path: Path) -> None:
    with pytest.raises(RuntimeError, match="performance workload source is missing"):
        build_workload_reproducibility_evidence(
            root=tmp_path,
            workload={"workload_id": "auroraBoard", "source": "showcase/auroraBoard/main.objc3"},
            measurement_policy=_measurement_policy(),
            benchmark_parameters={"hardware_profile_capture": {"normalization_mode": "raw-samples"}},
            budget_model=_budget_model(),
            budget_id="comparative-baseline",
            profile={},
            versions={},
        )


def test_runtime_reproducibility_evidence_maps_workload_to_budget_threshold(tmp_path: Path) -> None:
    fixture = tmp_path / "tests" / "tooling" / "fixtures" / "native" / "dispatch.objc3"
    probe = tmp_path / "tests" / "tooling" / "runtime" / "dispatch_probe.cpp"
    fixture.parent.mkdir(parents=True)
    probe.parent.mkdir(parents=True)
    fixture.write_text("func dispatch() -> i32 { return 1; }\n", encoding="utf-8")
    probe.write_text("int main() { return 0; }\n", encoding="utf-8")

    evidence = build_runtime_workload_reproducibility_evidence(
        root=tmp_path,
        workload={
            "workload_id": "dispatch-cache",
            "acceptance_case_id": "dispatch-fast-path",
            "fixture": "tests/tooling/fixtures/native/dispatch.objc3",
            "probe": "tests/tooling/runtime/dispatch_probe.cpp",
        },
        workload_manifest_path=tmp_path / "tests" / "tooling" / "fixtures" / "runtime_performance" / "workload_manifest.json",
        artifact_surface_path=tmp_path / "tests" / "tooling" / "fixtures" / "runtime_performance" / "artifact_surface.json",
        budget_model=_budget_model(),
        profile={},
        versions={},
    )

    assert evidence["contract_id"] == "objc3c.runtime.performance.reproducibility.evidence.v1"
    assert evidence["regression_policy"]["budget_id"] == "runtime-hot-path"
    assert evidence["regression_policy"]["threshold"]["metric_id"] == "dispatch_wall_clock_ms"
    assert len(evidence["workload_source"]["fixture_sha256"]) == 64
    assert len(evidence["workload_source"]["probe_sha256"]) == 64
