from __future__ import annotations

import json
import re
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import repo_rel


ROOT = Path(__file__).resolve().parents[1]
PERFORMANCE_GOVERNANCE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "performance_governance"
RUNTIME_PERFORMANCE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "runtime_performance"
BUDGET_MODEL_PATH = PERFORMANCE_GOVERNANCE_ROOT / "budget_model.json"
SOURCE_SURFACE_PATH = PERFORMANCE_GOVERNANCE_ROOT / "source_surface.json"
OWNER_CONTRACTS_PATH = PERFORMANCE_GOVERNANCE_ROOT / "owner_contracts.json"
RUNTIME_SOURCE_SURFACE_PATH = RUNTIME_PERFORMANCE_ROOT / "source_surface.json"
RUNTIME_ARTIFACT_SURFACE_PATH = RUNTIME_PERFORMANCE_ROOT / "artifact_surface.json"
WORKLOAD_MANIFEST_PATH = RUNTIME_PERFORMANCE_ROOT / "workload_manifest.json"
REPLAY_CONTRACT_PATH = RUNTIME_PERFORMANCE_ROOT / "workload_replay_contract.json"
METADATA_RESILIENCE_CONTRACT_PATH = RUNTIME_PERFORMANCE_ROOT / "metadata_resilience_contract.json"
STRESS_SANITIZER_CONTRACT_PATH = RUNTIME_PERFORMANCE_ROOT / "stress_sanitizer_contract.json"
DASHBOARD_SCHEMA_PATH = ROOT / "schemas" / "objc3c-performance-dashboard-summary-v1.schema.json"
PUBLIC_REPORT_SCHEMA_PATH = ROOT / "schemas" / "objc3c-performance-public-report-v1.schema.json"

EXPECTED_RUNTIME_CONTRACT_PATHS = (
    "tests/tooling/fixtures/runtime_performance/workload_replay_contract.json",
    "tests/tooling/fixtures/runtime_performance/metadata_resilience_contract.json",
    "tests/tooling/fixtures/runtime_performance/stress_sanitizer_contract.json",
)
RUNTIME_WORKLOAD_SOURCE_FIELD = re.compile(
    r"^workloads\[(?P<workload_id>[A-Za-z0-9._-]+)\]\.summary\.median_duration_ms$"
)


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"expected JSON object: {repo_rel(path)}")
    return payload


def _as_list(payload: dict[str, Any], key: str, owner: str) -> list[Any]:
    value = payload.get(key)
    if not isinstance(value, list):
        raise RuntimeError(f"{owner} missing list field {key}")
    return list(value)


def _as_dict(payload: dict[str, Any], key: str, owner: str) -> dict[str, Any]:
    value = payload.get(key)
    if not isinstance(value, dict):
        raise RuntimeError(f"{owner} missing object field {key}")
    return value


def _string_list(payload: dict[str, Any], key: str, owner: str) -> list[str]:
    values = _as_list(payload, key, owner)
    if not all(isinstance(value, str) and value for value in values):
        raise RuntimeError(f"{owner} field {key} must contain only non-empty strings")
    return [str(value) for value in values]


def _runtime_hot_path_family(budget_model: dict[str, Any]) -> dict[str, Any]:
    for family in _as_list(budget_model, "budget_families", "performance budget model"):
        if isinstance(family, dict) and family.get("budget_id") == "runtime-hot-path":
            return family
    raise RuntimeError("performance budget model missing runtime-hot-path budget")


def _runtime_metrics(budget_model: dict[str, Any]) -> dict[str, dict[str, Any]]:
    family = _runtime_hot_path_family(budget_model)
    metrics: dict[str, dict[str, Any]] = {}
    for metric in _as_list(family, "metric_definitions", "runtime-hot-path budget"):
        if not isinstance(metric, dict):
            raise RuntimeError("runtime-hot-path budget contains a non-object metric")
        metric_id = str(metric.get("metric_id", ""))
        source_field = str(metric.get("source_field", ""))
        if not metric_id or not RUNTIME_WORKLOAD_SOURCE_FIELD.match(source_field):
            raise RuntimeError(f"runtime-hot-path metric {metric_id or '<missing>'} has invalid workload source field")
        metrics[metric_id] = metric
    return metrics


def _runtime_metric_workload_ids(metrics: dict[str, dict[str, Any]]) -> dict[str, str]:
    workload_by_metric: dict[str, str] = {}
    for metric_id, metric in metrics.items():
        match = RUNTIME_WORKLOAD_SOURCE_FIELD.match(str(metric["source_field"]))
        if match is None:
            raise RuntimeError(f"runtime-hot-path metric {metric_id} source field drifted")
        workload_by_metric[metric_id] = match.group("workload_id")
    return workload_by_metric


def _workload_ids_from_manifest(workload_manifest: dict[str, Any]) -> set[str]:
    rows = _as_list(workload_manifest, "workload_families", "runtime performance workload manifest")
    return {
        str(row["workload_id"])
        for row in rows
        if isinstance(row, dict) and isinstance(row.get("workload_id"), str)
    }


def _workload_ids_from_replay_contract(replay_contract: dict[str, Any]) -> set[str]:
    rows = _as_list(replay_contract, "workload_replay_rows", "runtime performance replay contract")
    return {
        str(row["workload_id"])
        for row in rows
        if isinstance(row, dict) and isinstance(row.get("workload_id"), str)
    }


def _workload_ids_from_metadata_contract(metadata_contract: dict[str, Any]) -> set[str]:
    workload_ids: set[str] = set()
    for case in _as_list(metadata_contract, "malformed_metadata_cases", "metadata resilience contract"):
        if isinstance(case, dict) and isinstance(case.get("workload_id"), str):
            workload_ids.add(str(case["workload_id"]))
    for fuzz_contract in _as_list(metadata_contract, "metadata_fuzz_contracts", "metadata resilience contract"):
        if not isinstance(fuzz_contract, dict):
            continue
        if fuzz_contract.get("support_authority") is not False:
            raise RuntimeError("metadata resilience fuzz contracts must remain non-authoritative support evidence")
        if isinstance(fuzz_contract.get("workload_id"), str):
            workload_ids.add(str(fuzz_contract["workload_id"]))
    return workload_ids


def _workload_ids_from_stress_contract(stress_contract: dict[str, Any]) -> set[str]:
    workload_ids: set[str] = set()
    for collection_name in ("sanitizer_contracts", "stress_scale_contracts"):
        for contract in _as_list(stress_contract, collection_name, "stress sanitizer contract"):
            if not isinstance(contract, dict):
                continue
            if contract.get("support_authority") is not False:
                raise RuntimeError(f"{collection_name} rows must remain non-authoritative support evidence")
            if isinstance(contract.get("workload_id"), str):
                workload_ids.add(str(contract["workload_id"]))
    return workload_ids


def _owner_contract_runtime_paths(owner_contracts: dict[str, Any]) -> list[str]:
    for owner in _as_list(owner_contracts, "benchmark_source_owners", "performance owner contracts"):
        if isinstance(owner, dict) and owner.get("owner_id") == "runtime-performance-source-owner":
            return _string_list(owner, "contract_paths", "runtime performance source owner")
    raise RuntimeError("performance owner contracts missing runtime-performance-source-owner")


def _schema_requires_field(schema: dict[str, Any], field_name: str, owner: str) -> None:
    required = _string_list(schema, "required", owner)
    properties = _as_dict(schema, "properties", owner)
    if field_name not in required:
        raise RuntimeError(f"{owner} does not require {field_name}")
    if field_name not in properties:
        raise RuntimeError(f"{owner} does not define {field_name}")


def _expect_exact_paths(actual: list[str], expected: tuple[str, ...], owner: str) -> None:
    if actual != list(expected):
        raise RuntimeError(f"{owner} runtime contract paths drifted: {actual!r}")
    for relative_path in actual:
        if not (ROOT / relative_path).is_file():
            raise RuntimeError(f"{owner} references missing runtime contract path: {relative_path}")


def validate_runtime_contract_linkage(
    *,
    budget_model: dict[str, Any] | None = None,
    source_surface: dict[str, Any] | None = None,
    owner_contracts: dict[str, Any] | None = None,
    runtime_source_surface: dict[str, Any] | None = None,
    runtime_artifact_surface: dict[str, Any] | None = None,
    workload_manifest: dict[str, Any] | None = None,
    replay_contract: dict[str, Any] | None = None,
    metadata_resilience_contract: dict[str, Any] | None = None,
    stress_sanitizer_contract: dict[str, Any] | None = None,
    dashboard_schema: dict[str, Any] | None = None,
    public_report_schema: dict[str, Any] | None = None,
) -> dict[str, Any]:
    budget_model = budget_model or load_json(BUDGET_MODEL_PATH)
    source_surface = source_surface or load_json(SOURCE_SURFACE_PATH)
    owner_contracts = owner_contracts or load_json(OWNER_CONTRACTS_PATH)
    runtime_source_surface = runtime_source_surface or load_json(RUNTIME_SOURCE_SURFACE_PATH)
    runtime_artifact_surface = runtime_artifact_surface or load_json(RUNTIME_ARTIFACT_SURFACE_PATH)
    workload_manifest = workload_manifest or load_json(WORKLOAD_MANIFEST_PATH)
    replay_contract = replay_contract or load_json(REPLAY_CONTRACT_PATH)
    metadata_resilience_contract = metadata_resilience_contract or load_json(METADATA_RESILIENCE_CONTRACT_PATH)
    stress_sanitizer_contract = stress_sanitizer_contract or load_json(STRESS_SANITIZER_CONTRACT_PATH)
    dashboard_schema = dashboard_schema or load_json(DASHBOARD_SCHEMA_PATH)
    public_report_schema = public_report_schema or load_json(PUBLIC_REPORT_SCHEMA_PATH)

    linkage = _as_dict(budget_model, "runtime_contract_linkage", "performance budget model")
    contract_paths = _string_list(linkage, "contract_paths", "runtime contract linkage")
    _expect_exact_paths(contract_paths, EXPECTED_RUNTIME_CONTRACT_PATHS, "runtime contract linkage")
    _expect_exact_paths(
        _string_list(runtime_source_surface, "runtime_performance_contract_paths", "runtime source surface"),
        EXPECTED_RUNTIME_CONTRACT_PATHS,
        "runtime source surface",
    )
    _expect_exact_paths(
        _string_list(runtime_artifact_surface, "contract_surfaces", "runtime artifact surface"),
        EXPECTED_RUNTIME_CONTRACT_PATHS,
        "runtime artifact surface",
    )
    _expect_exact_paths(_owner_contract_runtime_paths(owner_contracts), EXPECTED_RUNTIME_CONTRACT_PATHS, "owner contracts")

    checked_sources = set(_string_list(source_surface, "checked_in_sources", "performance source surface"))
    runtime_owner_paths = set(
        _as_dict(source_surface, "owner_split", "performance source surface").get("runtime_performance", [])
    )
    for contract_path in EXPECTED_RUNTIME_CONTRACT_PATHS:
        if contract_path not in checked_sources:
            raise RuntimeError(f"performance source surface checked_in_sources missing {contract_path}")
        if contract_path not in runtime_owner_paths:
            raise RuntimeError(f"performance source surface runtime_performance owner split missing {contract_path}")

    replay_policy = _as_dict(replay_contract, "replay_policy", "runtime replay contract")
    replay_key_fields = set(_string_list(replay_policy, "replay_key_fields", "runtime replay contract policy"))
    if "budget_metric_id" not in replay_key_fields:
        raise RuntimeError("runtime replay contract must bind budget_metric_id into replay keys")

    metrics = _runtime_metrics(budget_model)
    workload_by_metric = _runtime_metric_workload_ids(metrics)
    linked_workload_by_metric: dict[str, str] = {}
    linked_metric_by_workload: dict[str, str] = {}
    for link in _as_list(linkage, "workload_budget_links", "runtime contract linkage"):
        if not isinstance(link, dict):
            raise RuntimeError("runtime contract linkage workload_budget_links contains a non-object row")
        workload_id = str(link.get("workload_id", ""))
        metric_id = str(link.get("metric_id", ""))
        if link.get("contract_role") != "replay-key-budget-metric":
            raise RuntimeError(f"runtime contract linkage row {workload_id} has invalid contract_role")
        if metric_id not in metrics:
            raise RuntimeError(f"runtime contract linkage row {workload_id} references unknown metric {metric_id}")
        if workload_by_metric[metric_id] != workload_id:
            raise RuntimeError(
                f"runtime contract linkage row {workload_id} does not match metric source field for {metric_id}"
            )
        linked_workload_by_metric[metric_id] = workload_id
        linked_metric_by_workload[workload_id] = metric_id

    if linked_workload_by_metric != workload_by_metric:
        raise RuntimeError("runtime contract linkage does not exactly cover runtime-hot-path budget metrics")

    workload_manifest_ids = _workload_ids_from_manifest(workload_manifest)
    replay_workload_ids = _workload_ids_from_replay_contract(replay_contract)
    metadata_workload_ids = _workload_ids_from_metadata_contract(metadata_resilience_contract)
    stress_workload_ids = _workload_ids_from_stress_contract(stress_sanitizer_contract)
    linked_workloads = set(linked_metric_by_workload)
    for owner, workload_ids in (
        ("workload manifest", workload_manifest_ids),
        ("replay contract", replay_workload_ids),
        ("metadata resilience contract", metadata_workload_ids),
        ("stress sanitizer contract", stress_workload_ids),
    ):
        missing = sorted(workload_ids - linked_workloads)
        if missing:
            raise RuntimeError(f"{owner} has runtime workloads missing budget linkage: {missing}")

    release_evidence = _as_dict(linkage, "release_evidence", "runtime contract linkage")
    if release_evidence.get("support_authority") is not False:
        raise RuntimeError("runtime contract release evidence must remain non-authoritative for support claims")
    if release_evidence.get("required_runtime_summary_field") != "runtime_contract_summary":
        raise RuntimeError("runtime contract release evidence must require runtime_contract_summary")
    dashboard_field = str(release_evidence.get("dashboard_field", ""))
    public_field = str(release_evidence.get("public_summary_field", ""))
    if dashboard_field != "runtime_contract_evidence" or public_field != "runtime_contract_evidence":
        raise RuntimeError("runtime contract release evidence field names drifted")
    _schema_requires_field(dashboard_schema, dashboard_field, "performance dashboard schema")
    _schema_requires_field(public_report_schema, public_field, "performance public report schema")

    required_counts = _as_dict(
        release_evidence,
        "required_contract_summary_counts",
        "runtime contract release evidence",
    )
    return {
        "contract_id": "objc3c.performance.governance.runtime.contract.linkage.summary.v1",
        "status": "PASS",
        "contract_paths": contract_paths,
        "runtime_budget_metric_ids": sorted(linked_workload_by_metric),
        "runtime_workload_ids": sorted(linked_metric_by_workload),
        "replay_workload_count": len(replay_workload_ids),
        "metadata_resilience_workload_count": len(metadata_workload_ids),
        "stress_sanitizer_workload_count": len(stress_workload_ids),
        "release_evidence": {
            "dashboard_field": dashboard_field,
            "public_summary_field": public_field,
            "support_authority": False,
            "required_contract_summary_counts": required_counts,
        },
    }


def _nested_value(payload: dict[str, Any], dotted_path: str) -> Any:
    current: Any = payload
    for part in dotted_path.split("."):
        if not isinstance(current, dict):
            return None
        current = current.get(part)
    return current


def build_runtime_contract_evidence(
    *,
    runtime_summary: dict[str, Any],
    budget_model: dict[str, Any],
) -> dict[str, Any]:
    linkage = budget_model.get("runtime_contract_linkage")
    if not isinstance(linkage, dict):
        return {
            "evidence_id": "objc3c.performance.governance.runtime-contract-evidence.v1",
            "status": "NOT_CONFIGURED",
            "support_authority": False,
            "contract_paths": [],
            "budget_metric_ids": [],
            "workload_ids": [],
            "summary_counts": {},
        }

    contract_paths = _string_list(linkage, "contract_paths", "runtime contract linkage")
    workload_links = _as_list(linkage, "workload_budget_links", "runtime contract linkage")
    release_evidence = _as_dict(linkage, "release_evidence", "runtime contract linkage")
    required_counts = _as_dict(
        release_evidence,
        "required_contract_summary_counts",
        "runtime contract release evidence",
    )
    runtime_contract_summary = runtime_summary.get("runtime_contract_summary")
    status = "PASS"
    failures: list[str] = []
    summary_counts: dict[str, Any] = {}
    if not isinstance(runtime_contract_summary, dict):
        status = "FAIL"
        failures.append("runtime performance summary missing runtime_contract_summary")
    else:
        contract_files = runtime_contract_summary.get("contract_files", [])
        summary_paths = [
            str(row.get("path", ""))
            for row in contract_files
            if isinstance(row, dict) and isinstance(row.get("path"), str)
        ]
        if summary_paths != contract_paths:
            status = "FAIL"
            failures.append("runtime performance summary contract_files drifted from budget linkage")
        for dotted_path, expected_count in required_counts.items():
            actual_count = _nested_value(runtime_contract_summary, dotted_path)
            summary_counts[str(dotted_path)] = actual_count
            if actual_count != expected_count:
                status = "FAIL"
                failures.append(f"runtime_contract_summary.{dotted_path} expected {expected_count}, saw {actual_count}")

    return {
        "evidence_id": "objc3c.performance.governance.runtime-contract-evidence.v1",
        "status": status,
        "support_authority": False,
        "contract_paths": contract_paths,
        "budget_metric_ids": [
            str(row["metric_id"])
            for row in workload_links
            if isinstance(row, dict) and isinstance(row.get("metric_id"), str)
        ],
        "workload_ids": [
            str(row["workload_id"])
            for row in workload_links
            if isinstance(row, dict) and isinstance(row.get("workload_id"), str)
        ],
        "summary_counts": summary_counts,
        "failures": failures,
    }


__all__ = [
    "BUDGET_MODEL_PATH",
    "EXPECTED_RUNTIME_CONTRACT_PATHS",
    "build_runtime_contract_evidence",
    "load_json",
    "validate_runtime_contract_linkage",
]
