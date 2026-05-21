from __future__ import annotations

import hashlib
from pathlib import Path
from typing import Any, Sequence

from objc3c_tooling.paths import repo_rel


def _require_mapping(payload: dict[str, Any], key: str, owner: str) -> dict[str, Any]:
    value = payload.get(key)
    if not isinstance(value, dict):
        raise RuntimeError(f"{owner} missing object field {key}")
    return value


def _require_sequence(payload: dict[str, Any], key: str, owner: str) -> list[Any]:
    value = payload.get(key)
    if not isinstance(value, list):
        raise RuntimeError(f"{owner} missing array field {key}")
    return list(value)


def _root_relative(root: Path, path: Path) -> str:
    try:
        return path.relative_to(root).as_posix()
    except ValueError:
        return repo_rel(path)


def file_sha256(path: Path) -> str:
    if not path.is_file():
        raise RuntimeError(f"performance contract file is missing: {path}")
    return hashlib.sha256(path.read_bytes()).hexdigest()


def source_sha256(root: Path, source_path: str) -> str:
    path = root / source_path
    if not path.is_file():
        raise RuntimeError(f"performance workload source is missing: {source_path}")
    return hashlib.sha256(path.read_bytes()).hexdigest()


def checked_in_contract_evidence(root: Path, contract_paths: Sequence[Path]) -> list[dict[str, str]]:
    rows: list[dict[str, str]] = []
    root_resolved = root.resolve()
    for contract_path in contract_paths:
        resolved = contract_path.resolve()
        try:
            resolved.relative_to(root_resolved)
        except ValueError as exc:
            raise RuntimeError(f"performance contract file escapes repo: {contract_path}") from exc
        rows.append(
            {
                "path": _root_relative(root, resolved),
                "sha256": file_sha256(resolved),
            }
        )
    return rows


def budget_family_thresholds(budget_model: dict[str, Any], budget_id: str) -> list[dict[str, Any]]:
    families = _require_sequence(budget_model, "budget_families", "performance budget model")
    for family in families:
        if not isinstance(family, dict):
            continue
        if family.get("budget_id") != budget_id:
            continue
        metrics = _require_sequence(family, "metric_definitions", f"performance budget {budget_id}")
        rows: list[dict[str, Any]] = []
        for metric in metrics:
            if not isinstance(metric, dict):
                raise RuntimeError(f"performance budget {budget_id} contains a non-object metric")
            rows.append(
                {
                    "metric_id": str(metric["metric_id"]),
                    "source_field": str(metric["source_field"]),
                    "comparison": str(metric["comparison"]),
                    "warning_value": metric["warning_value"],
                    "blocking_value": metric["blocking_value"],
                }
            )
        if not rows:
            raise RuntimeError(f"performance budget {budget_id} has no metric definitions")
        return rows
    raise RuntimeError(f"performance budget model missing budget_id {budget_id}")


def runtime_budget_metric_for_workload(budget_model: dict[str, Any], workload_id: str) -> dict[str, Any]:
    metric_by_workload = {
        "startup-installation": "startup_wall_clock_ms",
        "dispatch-cache": "dispatch_wall_clock_ms",
        "reflection-query": "reflection_wall_clock_ms",
        "ownership-helpers": "ownership_wall_clock_ms",
        "storage-ownership-reflection": "storage_ownership_reflection_wall_clock_ms",
        "stdlib-core-runtime": "stdlib_core_wall_clock_ms",
        "stdlib-concurrency-runtime": "stdlib_concurrency_wall_clock_ms",
        "registration-replay": "registration_replay_wall_clock_ms",
        "block-arc-runtime": "block_arc_runtime_wall_clock_ms",
        "live-error-runtime": "live_error_runtime_wall_clock_ms",
        "async-error-foreign-boundary": "async_error_foreign_boundary_wall_clock_ms",
        "live-concurrency-runtime": "live_concurrency_wall_clock_ms",
    }
    metric_id = metric_by_workload.get(workload_id)
    if metric_id is None:
        raise RuntimeError(f"runtime performance workload has no budget metric: {workload_id}")
    for metric in budget_family_thresholds(budget_model, "runtime-hot-path"):
        if metric["metric_id"] == metric_id:
            return metric
    raise RuntimeError(f"runtime performance budget missing metric_id {metric_id}")


def build_workload_reproducibility_evidence(
    *,
    root: Path,
    workload: dict[str, Any],
    measurement_policy: dict[str, Any],
    benchmark_parameters: dict[str, Any],
    budget_model: dict[str, Any],
    budget_id: str,
    profile: dict[str, Any],
    versions: dict[str, str],
) -> dict[str, Any]:
    workload_id = str(workload["workload_id"])
    source_path = str(workload["source"])
    sample_policy = _require_mapping(measurement_policy, "sample_policy", "performance measurement policy")
    comparison_policy = _require_mapping(
        measurement_policy, "comparison_policy", "performance measurement policy"
    )
    claimability_policy = _require_mapping(
        measurement_policy, "claimability_policy", "performance measurement policy"
    )
    return {
        "contract_id": "objc3c.performance.reproducibility.evidence.v1",
        "workload_id": workload_id,
        "workload_source": {
            "path": source_path,
            "sha256": source_sha256(root, source_path),
        },
        "machine_profile": profile,
        "tool_versions": versions,
        "sample_policy": {
            "warmup_runs": sample_policy.get("warmup_runs"),
            "measured_runs": sample_policy.get("measured_runs"),
            "clock_source": sample_policy.get("clock_source"),
            "capture_raw_samples": sample_policy.get("capture_raw_samples"),
        },
        "comparison_policy": {
            "same_machine_required": comparison_policy.get("same_machine_required"),
            "same_input_family_required": comparison_policy.get("same_input_family_required"),
            "same_checked_in_source_required": comparison_policy.get("same_checked_in_source_required"),
            "capture_exact_commands": comparison_policy.get("capture_exact_commands"),
            "capture_tool_versions": comparison_policy.get("capture_tool_versions"),
        },
        "normalization_policy": _require_mapping(
            benchmark_parameters,
            "hardware_profile_capture",
            "performance benchmark parameters",
        ),
        "claimability_policy": {
            "allowed_claim_classes": _require_sequence(
                claimability_policy,
                "allowed_claim_classes",
                "performance claimability policy",
            ),
            "disallowed_claim_classes": _require_sequence(
                claimability_policy,
                "disallowed_claim_classes",
                "performance claimability policy",
            ),
            "required_claim_inputs": _require_sequence(
                claimability_policy,
                "required_claim_inputs",
                "performance claimability policy",
            ),
        },
        "regression_policy": {
            "budget_model_path": "tests/tooling/fixtures/performance_governance/budget_model.json",
            "budget_model_contract_id": str(budget_model["contract_id"]),
            "budget_id": budget_id,
            "thresholds": budget_family_thresholds(budget_model, budget_id),
        },
    }


def build_runtime_workload_reproducibility_evidence(
    *,
    root: Path,
    workload: dict[str, Any],
    workload_manifest_path: Path,
    artifact_surface_path: Path,
    budget_model: dict[str, Any],
    profile: dict[str, Any],
    versions: dict[str, str],
    contract_paths: Sequence[Path] = (),
) -> dict[str, Any]:
    workload_id = str(workload["workload_id"])
    fixture = str(workload["fixture"])
    probe = str(workload["probe"])
    fixture_sha256 = source_sha256(root, fixture)
    probe_sha256 = source_sha256(root, probe)
    threshold = runtime_budget_metric_for_workload(budget_model, workload_id)
    return {
        "contract_id": "objc3c.runtime.performance.reproducibility.evidence.v1",
        "workload_id": workload_id,
        "acceptance_case_id": str(workload["acceptance_case_id"]),
        "workload_manifest_path": _root_relative(root, workload_manifest_path),
        "artifact_surface_path": _root_relative(root, artifact_surface_path),
        "workload_source": {
            "fixture": fixture,
            "fixture_sha256": fixture_sha256,
            "probe": probe,
            "probe_sha256": probe_sha256,
        },
        "replay_key": {
            "workload_id": workload_id,
            "acceptance_case_id": str(workload["acceptance_case_id"]),
            "hot_path_family": str(workload["hot_path_family"]),
            "fixture_sha256": fixture_sha256,
            "probe_sha256": probe_sha256,
            "budget_metric_id": str(threshold["metric_id"]),
        },
        "machine_profile": profile,
        "tool_versions": versions,
        "contract_evidence": checked_in_contract_evidence(root, contract_paths),
        "regression_policy": {
            "budget_model_path": "tests/tooling/fixtures/performance_governance/budget_model.json",
            "budget_model_contract_id": str(budget_model["contract_id"]),
            "budget_id": "runtime-hot-path",
            "threshold": threshold,
        },
    }
