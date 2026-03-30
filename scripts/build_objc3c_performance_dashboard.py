#!/usr/bin/env python3
"""Build the live performance governance dashboard from upstream performance reports."""

from __future__ import annotations

import json
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "performance_governance"
BUDGET_MODEL_PATH = FIXTURE_ROOT / "budget_model.json"
CLAIM_POLICY_PATH = FIXTURE_ROOT / "claim_policy.json"
TRIAGE_POLICY_PATH = FIXTURE_ROOT / "breach_triage_policy.json"
LAB_POLICY_PATH = FIXTURE_ROOT / "lab_policy.json"
WAIVERS_PATH = FIXTURE_ROOT / "waivers.json"
PERFORMANCE_SUMMARY_PATH = ROOT / "tmp" / "reports" / "performance" / "benchmark-summary.json"
PERFORMANCE_INTEGRATION_PATH = ROOT / "tmp" / "reports" / "performance" / "integration-summary.json"
COMPARATIVE_SUMMARY_PATH = ROOT / "tmp" / "reports" / "performance" / "comparative-baselines-summary.json"
COMPILER_SUMMARY_PATH = ROOT / "tmp" / "reports" / "compiler-throughput" / "benchmark-summary.json"
COMPILER_INTEGRATION_PATH = ROOT / "tmp" / "reports" / "compiler-throughput" / "integration-summary.json"
RUNTIME_SUMMARY_PATH = ROOT / "tmp" / "reports" / "runtime-performance" / "benchmark-summary.json"
RUNTIME_INTEGRATION_PATH = ROOT / "tmp" / "reports" / "runtime-performance" / "integration-summary.json"
OUTPUT_PATH = ROOT / "tmp" / "reports" / "performance-governance" / "dashboard-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.performance.governance.dashboard.summary.v1"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def require_json(path: Path, *, kind: str) -> dict[str, Any]:
    if not path.is_file():
        raise RuntimeError(f"missing {kind}: {repo_rel(path)}")
    return load_json(path)


def parse_timestamp(raw_value: Any) -> datetime:
    if not isinstance(raw_value, str) or not raw_value:
        raise RuntimeError("missing generated_at_utc timestamp")
    normalized = raw_value.replace("Z", "+00:00")
    parsed = datetime.fromisoformat(normalized)
    if parsed.tzinfo is None:
        parsed = parsed.replace(tzinfo=timezone.utc)
    return parsed.astimezone(timezone.utc)


def report_timestamp(payload: dict[str, Any], path: Path) -> datetime:
    raw_value = payload.get("generated_at_utc")
    if isinstance(raw_value, str) and raw_value:
        return parse_timestamp(raw_value)
    return datetime.fromtimestamp(path.stat().st_mtime, tz=timezone.utc)


def get_nested_value(payload: dict[str, Any], source_field: str, derived: dict[str, float | int]) -> float | int | None:
    if source_field.startswith("derived."):
        return derived.get(source_field.split(".", 1)[1])
    if source_field.startswith("workloads["):
        workload_id, _, remainder = source_field[len("workloads[") :].partition("]")
        remainder = remainder.lstrip(".")
        workloads = payload.get("workloads", [])
        if not isinstance(workloads, list):
            return None
        target = None
        for workload in workloads:
            if isinstance(workload, dict) and workload.get("workload_id") == workload_id:
                target = workload
                break
        if not isinstance(target, dict):
            return None
        current: Any = target
        for segment in remainder.split("."):
            if not isinstance(current, dict):
                return None
            current = current.get(segment)
        if isinstance(current, (int, float)):
            return current
        return None
    current: Any = payload
    for segment in source_field.split("."):
        if not isinstance(current, dict):
            return None
        current = current.get(segment)
    if isinstance(current, (int, float)):
        return current
    return None


def build_breach_lookup(triage_policy: dict[str, Any]) -> dict[str, dict[str, Any]]:
    lookup: dict[str, dict[str, Any]] = {}
    for entry in triage_policy.get("classifications", []):
        if not isinstance(entry, dict):
            continue
        classification = str(entry.get("classification", ""))
        for breach_id in entry.get("allowed_breach_ids", []):
            if isinstance(breach_id, str):
                lookup[breach_id] = {
                    "classification": classification,
                }
    return lookup


def build_taxonomy_lookup(budget_model: dict[str, Any]) -> dict[str, dict[str, Any]]:
    lookup: dict[str, dict[str, Any]] = {}
    for entry in budget_model.get("breach_taxonomy", []):
        if isinstance(entry, dict) and isinstance(entry.get("breach_id"), str):
            lookup[str(entry["breach_id"])] = entry
    return lookup


def load_packet_machine_profiles(paths: list[str]) -> list[dict[str, Any]]:
    profiles: list[dict[str, Any]] = []
    for relative_path in paths:
        packet_path = ROOT / relative_path
        if not packet_path.is_file():
            continue
        payload = load_json(packet_path)
        profile = payload.get("machine_profile")
        if isinstance(profile, dict):
            profiles.append(profile)
    return profiles


def summarize_machine_profile(profile: dict[str, Any]) -> tuple[Any, ...]:
    return (
        profile.get("os"),
        profile.get("arch"),
        profile.get("cpu_model"),
        profile.get("cpu_count"),
    )


def apply_waiver(
    breach: dict[str, Any],
    waivers: list[dict[str, Any]],
    taxonomy_lookup: dict[str, dict[str, Any]],
) -> dict[str, Any]:
    for waiver in waivers:
        if waiver.get("status") != "active":
            continue
        if waiver.get("budget_id") != breach.get("budget_id"):
            continue
        waiver_metric = waiver.get("metric_id")
        if waiver_metric and waiver_metric != breach.get("metric_id"):
            continue
        if waiver.get("breach_id") != breach.get("breach_id"):
            continue
        waived = dict(breach)
        waived["original_breach_id"] = breach["breach_id"]
        waived["breach_id"] = "waived-regression"
        taxonomy = taxonomy_lookup["waived-regression"]
        waived["severity"] = taxonomy["severity"]
        waived["operator_action"] = taxonomy["operator_action"]
        waived["waived"] = True
        waived["waiver_id"] = waiver.get("waiver_id")
        return waived
    breach["waived"] = False
    return breach


def main() -> int:
    try:
        budget_model = require_json(BUDGET_MODEL_PATH, kind="budget model")
        claim_policy = require_json(CLAIM_POLICY_PATH, kind="claim policy")
        triage_policy = require_json(TRIAGE_POLICY_PATH, kind="breach triage policy")
        lab_policy = require_json(LAB_POLICY_PATH, kind="lab policy")
        waivers_payload = require_json(WAIVERS_PATH, kind="waiver registry")
        performance_summary = require_json(PERFORMANCE_SUMMARY_PATH, kind="performance benchmark summary")
        performance_integration = require_json(PERFORMANCE_INTEGRATION_PATH, kind="performance integration summary")
        comparative_summary = require_json(COMPARATIVE_SUMMARY_PATH, kind="comparative baseline summary")
        compiler_summary = require_json(COMPILER_SUMMARY_PATH, kind="compiler throughput summary")
        compiler_integration = require_json(COMPILER_INTEGRATION_PATH, kind="compiler throughput integration summary")
        runtime_summary = require_json(RUNTIME_SUMMARY_PATH, kind="runtime performance summary")
        runtime_integration = require_json(RUNTIME_INTEGRATION_PATH, kind="runtime performance integration summary")
    except RuntimeError as exc:
        print(f"objc3c-performance-dashboard: FAIL\n- {exc}", file=sys.stderr)
        return 1

    failures: list[str] = []
    waivers = waivers_payload.get("waivers", [])
    if not isinstance(waivers, list):
        failures.append("waiver registry waivers field drifted")
        waivers = []

    taxonomy_lookup = build_taxonomy_lookup(budget_model)
    breach_lookup = build_breach_lookup(triage_policy)

    compile_packets = [path for path in comparative_summary.get("telemetry_packets", []) if isinstance(path, str) and "/compile/" in path.replace("\\", "/")]
    runtime_packets = [path for path in comparative_summary.get("telemetry_packets", []) if isinstance(path, str) and "/runtime/" in path.replace("\\", "/")]
    now = datetime.now(timezone.utc)
    derived = {
        "compile_packet_count": len(compile_packets),
        "runtime_packet_count": len(runtime_packets),
        "performance_report_age_hours": round((now - report_timestamp(performance_summary, PERFORMANCE_SUMMARY_PATH)).total_seconds() / 3600.0, 3),
        "compiler_throughput_report_age_hours": round((now - report_timestamp(compiler_summary, COMPILER_SUMMARY_PATH)).total_seconds() / 3600.0, 3),
        "runtime_performance_report_age_hours": round((now - report_timestamp(runtime_summary, RUNTIME_SUMMARY_PATH)).total_seconds() / 3600.0, 3),
    }

    packet_paths: list[str] = []
    for summary in (performance_summary, comparative_summary, runtime_summary):
        for relative_path in summary.get("telemetry_packets", summary.get("packet_paths", [])):
            if isinstance(relative_path, str):
                packet_paths.append(relative_path.replace("\\", "/"))
    machine_profiles = load_packet_machine_profiles(packet_paths)
    machine_profile_keys = {summarize_machine_profile(profile) for profile in machine_profiles}
    machine_profiles_consistent = len(machine_profile_keys) <= 1

    stale_report_paths: list[str] = []
    max_report_age_hours = 24.0
    for path, age_hours in (
        (repo_rel(PERFORMANCE_SUMMARY_PATH), derived["performance_report_age_hours"]),
        (repo_rel(COMPILER_SUMMARY_PATH), derived["compiler_throughput_report_age_hours"]),
        (repo_rel(RUNTIME_SUMMARY_PATH), derived["runtime_performance_report_age_hours"]),
    ):
        if float(age_hours) > max_report_age_hours:
            stale_report_paths.append(path)

    environment_issues: list[str] = []
    if not machine_profiles_consistent:
        environment_issues.append("machine profile drift detected across live telemetry packets")
    if stale_report_paths:
        environment_issues.append("one or more upstream performance reports exceeded the freshness ceiling")

    summary_lookup = {
        "tmp/reports/compiler-throughput/benchmark-summary.json": compiler_summary,
        "tmp/reports/runtime-performance/benchmark-summary.json": runtime_summary,
        "tmp/reports/performance/comparative-baselines-summary.json": comparative_summary,
        "tmp/reports/performance/integration-summary.json": performance_integration,
    }

    budget_family_summaries: list[dict[str, Any]] = []
    breaches: list[dict[str, Any]] = []
    for family in budget_model.get("budget_families", []):
        if not isinstance(family, dict):
            failures.append("budget_families contains a non-object entry")
            continue
        budget_id = str(family.get("budget_id", ""))
        summary_path = str(family.get("summary_path", ""))
        child_payload = summary_lookup.get(summary_path)
        report_status = "MISSING"
        metric_summaries: list[dict[str, Any]] = []
        if child_payload is None:
            breaches.append(
                {
                    "budget_id": budget_id,
                    "metric_id": "summary_presence",
                    "breach_id": "coverage-gap",
                    "severity": taxonomy_lookup["coverage-gap"]["severity"],
                    "operator_action": taxonomy_lookup["coverage-gap"]["operator_action"],
                    "child_report_path": summary_path,
                    "classification": breach_lookup["coverage-gap"]["classification"],
                }
            )
        else:
            report_status = str(child_payload.get("status", "FAIL"))
            if report_status != str(family.get("required_status", "PASS")):
                breaches.append(
                    {
                        "budget_id": budget_id,
                        "metric_id": "summary_status",
                        "breach_id": "coverage-gap",
                        "severity": taxonomy_lookup["coverage-gap"]["severity"],
                        "operator_action": taxonomy_lookup["coverage-gap"]["operator_action"],
                        "child_report_path": summary_path,
                        "classification": breach_lookup["coverage-gap"]["classification"],
                    }
                )
            for metric in family.get("metric_definitions", []):
                if not isinstance(metric, dict):
                    failures.append(f"{budget_id} metric definition drifted")
                    continue
                metric_id = str(metric.get("metric_id", ""))
                comparison = str(metric.get("comparison", ""))
                observed_value = get_nested_value(child_payload, str(metric.get("source_field", "")), derived)
                metric_summary: dict[str, Any] = {
                    "metric_id": metric_id,
                    "comparison": comparison,
                }
                if observed_value is None:
                    metric_summary["breach_id"] = "coverage-gap"
                    breaches.append(
                        {
                            "budget_id": budget_id,
                            "metric_id": metric_id,
                            "breach_id": "coverage-gap",
                            "severity": taxonomy_lookup["coverage-gap"]["severity"],
                            "operator_action": taxonomy_lookup["coverage-gap"]["operator_action"],
                            "child_report_path": summary_path,
                            "classification": breach_lookup["coverage-gap"]["classification"],
                        }
                    )
                    metric_summaries.append(metric_summary)
                    continue
                metric_summary["observed_value"] = observed_value
                warning_value = metric.get("warning_value")
                blocking_value = metric.get("blocking_value")
                if isinstance(warning_value, (int, float)):
                    metric_summary["warning_value"] = warning_value
                if isinstance(blocking_value, (int, float)):
                    metric_summary["blocking_value"] = blocking_value

                breach_id: str | None = None
                allowed_value: float | int | None = None
                if comparison == "max":
                    if isinstance(blocking_value, (int, float)) and float(observed_value) > float(blocking_value):
                        breach_id = "hard-regression"
                        allowed_value = blocking_value
                    elif isinstance(warning_value, (int, float)) and float(observed_value) > float(warning_value):
                        breach_id = "soft-regression"
                        allowed_value = warning_value
                elif comparison == "min":
                    if isinstance(blocking_value, (int, float)) and float(observed_value) < float(blocking_value):
                        breach_id = "coverage-gap"
                        allowed_value = blocking_value
                    elif isinstance(warning_value, (int, float)) and float(observed_value) < float(warning_value):
                        breach_id = "soft-regression"
                        allowed_value = warning_value
                else:
                    failures.append(f"{budget_id} metric {metric_id} comparison drifted")

                if breach_id is not None:
                    taxonomy = taxonomy_lookup[breach_id]
                    breach = {
                        "budget_id": budget_id,
                        "metric_id": metric_id,
                        "breach_id": breach_id,
                        "severity": taxonomy["severity"],
                        "operator_action": taxonomy["operator_action"],
                        "child_report_path": summary_path,
                        "classification": breach_lookup[breach_id]["classification"],
                        "observed_value": observed_value,
                        "allowed_value": allowed_value,
                    }
                    breach = apply_waiver(breach, waivers, taxonomy_lookup)
                    metric_summary["breach_id"] = breach["breach_id"]
                    breaches.append(breach)
                metric_summaries.append(metric_summary)

        budget_family_summaries.append(
            {
                "budget_id": budget_id,
                "child_report_path": summary_path,
                "report_status": report_status,
                "metric_summaries": metric_summaries,
            }
        )

    if environment_issues:
        env_breach = {
            "budget_id": "lab-policy",
            "metric_id": "machine-profile-consistency",
            "breach_id": "environment-drift",
            "severity": taxonomy_lookup["environment-drift"]["severity"],
            "operator_action": taxonomy_lookup["environment-drift"]["operator_action"],
            "child_report_path": repo_rel(LAB_POLICY_PATH),
            "classification": breach_lookup["environment-drift"]["classification"],
            "waived": False,
        }
        breaches.append(env_breach)

    expired_waivers: list[dict[str, Any]] = []
    for waiver in waivers:
        if not isinstance(waiver, dict):
            failures.append("waiver entry drifted")
            continue
        try:
            expires_at = parse_timestamp(waiver.get("expires_at_utc"))
        except RuntimeError:
            continue
        status = "active" if expires_at >= now else "expired"
        waiver["status"] = status
        if status == "expired":
            expired_waivers.append(waiver)

    blocking_breach_count = sum(1 for breach in breaches if breach.get("severity") == "blocking")
    warning_breach_count = sum(1 for breach in breaches if breach.get("severity") in {"warning", "caution"})
    if blocking_breach_count or expired_waivers:
        release_status = "blocked"
    elif warning_breach_count:
        release_status = "caution"
    else:
        release_status = "release-ready"

    allowed_statuses = [entry.get("status") for entry in claim_policy.get("claim_statuses", []) if isinstance(entry, dict)]
    if release_status not in allowed_statuses:
        failures.append("claim policy no longer admits the derived release_status")

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "budget_model_path": repo_rel(BUDGET_MODEL_PATH),
        "claim_policy_path": repo_rel(CLAIM_POLICY_PATH),
        "breach_triage_policy_path": repo_rel(TRIAGE_POLICY_PATH),
        "lab_policy_path": repo_rel(LAB_POLICY_PATH),
        "upstream_reports": {
            "performance_summary": repo_rel(PERFORMANCE_SUMMARY_PATH),
            "performance_integration": repo_rel(PERFORMANCE_INTEGRATION_PATH),
            "comparative_summary": repo_rel(COMPARATIVE_SUMMARY_PATH),
            "compiler_summary": repo_rel(COMPILER_SUMMARY_PATH),
            "compiler_integration": repo_rel(COMPILER_INTEGRATION_PATH),
            "runtime_summary": repo_rel(RUNTIME_SUMMARY_PATH),
            "runtime_integration": repo_rel(RUNTIME_INTEGRATION_PATH),
        },
        "release_status": release_status,
        "claim_ready": release_status == "release-ready",
        "budget_family_summaries": budget_family_summaries,
        "breaches": breaches,
        "waivers": waivers,
        "environment_drift": {
            "machine_profiles_consistent": machine_profiles_consistent,
            "stale_report_paths": stale_report_paths,
            "issues": environment_issues,
        },
        "blocking_breach_count": blocking_breach_count,
        "warning_breach_count": warning_breach_count,
        "failures": failures,
    }
    OUTPUT_PATH.parent.mkdir(parents=True, exist_ok=True)
    OUTPUT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(OUTPUT_PATH)}")
    print("objc3c-performance-dashboard: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
