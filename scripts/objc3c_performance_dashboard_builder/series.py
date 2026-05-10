from __future__ import annotations

from dataclasses import dataclass
from datetime import datetime, timezone
from typing import Any

from objc3c_tooling.paths import repo_rel

from objc3c_performance_dashboard.metrics import apply_waiver
from objc3c_performance_dashboard.metrics import get_nested_value
from objc3c_performance_dashboard.metrics import load_packet_machine_profiles
from objc3c_performance_dashboard.metrics import parse_timestamp
from objc3c_performance_dashboard.metrics import report_timestamp
from objc3c_performance_dashboard.metrics import summarize_machine_profile
from objc3c_performance_dashboard.paths import COMPILER_SUMMARY_PATH
from objc3c_performance_dashboard.paths import LAB_POLICY_PATH
from objc3c_performance_dashboard.paths import PERFORMANCE_SUMMARY_PATH
from objc3c_performance_dashboard.paths import RUNTIME_SUMMARY_PATH

from objc3c_performance_dashboard_builder.input_loading import DashboardInputs

MAX_REPORT_AGE_HOURS = 24.0


@dataclass(frozen=True)
class DashboardSeries:
    compile_packets: list[str]
    runtime_packets: list[str]
    derived: dict[str, float | int]
    machine_profiles_consistent: bool
    stale_report_paths: list[str]
    environment_issues: list[str]


@dataclass(frozen=True)
class BudgetEvaluation:
    budget_family_summaries: list[dict[str, Any]]
    breaches: list[dict[str, Any]]


def collect_packet_paths(*summaries: dict[str, Any]) -> list[str]:
    packet_paths: list[str] = []
    for summary in summaries:
        for relative_path in summary.get("telemetry_packets", summary.get("packet_paths", [])):
            if isinstance(relative_path, str):
                packet_paths.append(relative_path.replace("\\", "/"))
    return packet_paths


def build_dashboard_series(
    inputs: DashboardInputs,
    *,
    now: datetime | None = None,
) -> DashboardSeries:
    observed_at = now or datetime.now(timezone.utc)
    compile_packets = [
        path
        for path in inputs.comparative_summary.get("telemetry_packets", [])
        if isinstance(path, str) and "/compile/" in path.replace("\\", "/")
    ]
    runtime_packets = [
        path
        for path in inputs.comparative_summary.get("telemetry_packets", [])
        if isinstance(path, str) and "/runtime/" in path.replace("\\", "/")
    ]
    derived = {
        "compile_packet_count": len(compile_packets),
        "runtime_packet_count": len(runtime_packets),
        "performance_report_age_hours": round(
            (observed_at - report_timestamp(inputs.performance_summary, PERFORMANCE_SUMMARY_PATH)).total_seconds()
            / 3600.0,
            3,
        ),
        "compiler_throughput_report_age_hours": round(
            (observed_at - report_timestamp(inputs.compiler_summary, COMPILER_SUMMARY_PATH)).total_seconds()
            / 3600.0,
            3,
        ),
        "runtime_performance_report_age_hours": round(
            (observed_at - report_timestamp(inputs.runtime_summary, RUNTIME_SUMMARY_PATH)).total_seconds()
            / 3600.0,
            3,
        ),
    }

    packet_paths = collect_packet_paths(
        inputs.performance_summary,
        inputs.comparative_summary,
        inputs.runtime_summary,
    )
    machine_profiles = load_packet_machine_profiles(packet_paths)
    machine_profile_keys = {summarize_machine_profile(profile) for profile in machine_profiles}
    machine_profiles_consistent = len(machine_profile_keys) <= 1

    stale_report_paths: list[str] = []
    for path, age_hours in (
        (repo_rel(PERFORMANCE_SUMMARY_PATH), derived["performance_report_age_hours"]),
        (repo_rel(COMPILER_SUMMARY_PATH), derived["compiler_throughput_report_age_hours"]),
        (repo_rel(RUNTIME_SUMMARY_PATH), derived["runtime_performance_report_age_hours"]),
    ):
        if float(age_hours) > MAX_REPORT_AGE_HOURS:
            stale_report_paths.append(path)

    environment_issues: list[str] = []
    if not machine_profiles_consistent:
        environment_issues.append("machine profile drift detected across live telemetry packets")
    if stale_report_paths:
        environment_issues.append("one or more upstream performance reports exceeded the freshness ceiling")

    return DashboardSeries(
        compile_packets=compile_packets,
        runtime_packets=runtime_packets,
        derived=derived,
        machine_profiles_consistent=machine_profiles_consistent,
        stale_report_paths=stale_report_paths,
        environment_issues=environment_issues,
    )


def build_summary_lookup(inputs: DashboardInputs) -> dict[str, dict[str, Any]]:
    return {
        "tmp/reports/compiler-throughput/benchmark-summary.json": inputs.compiler_summary,
        "tmp/reports/runtime-performance/benchmark-summary.json": inputs.runtime_summary,
        "tmp/reports/performance/comparative-baselines-summary.json": inputs.comparative_summary,
        "tmp/reports/performance/integration-summary.json": inputs.performance_integration,
    }


def evaluate_budget_families(
    *,
    budget_model: dict[str, Any],
    summary_lookup: dict[str, dict[str, Any]],
    derived: dict[str, float | int],
    taxonomy_lookup: dict[str, dict[str, Any]],
    breach_lookup: dict[str, dict[str, Any]],
    waivers: list[dict[str, Any]],
    failures: list[str],
) -> BudgetEvaluation:
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
                    if isinstance(allowed_value, (int, float)) and float(allowed_value) != 0.0:
                        breach["ratio"] = round(float(observed_value) / float(allowed_value), 6)
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

    return BudgetEvaluation(
        budget_family_summaries=budget_family_summaries,
        breaches=breaches,
    )


def append_environment_breach(
    *,
    breaches: list[dict[str, Any]],
    taxonomy_lookup: dict[str, dict[str, Any]],
    breach_lookup: dict[str, dict[str, Any]],
) -> None:
    breaches.append(
        {
            "budget_id": "lab-policy",
            "metric_id": "machine-profile-consistency",
            "breach_id": "environment-drift",
            "severity": taxonomy_lookup["environment-drift"]["severity"],
            "operator_action": taxonomy_lookup["environment-drift"]["operator_action"],
            "child_report_path": repo_rel(LAB_POLICY_PATH),
            "classification": breach_lookup["environment-drift"]["classification"],
            "waived": False,
        }
    )


def evaluate_waivers(
    *,
    waivers: list[dict[str, Any]],
    failures: list[str],
    now: datetime,
) -> list[dict[str, Any]]:
    expired_waivers: list[dict[str, Any]] = []
    required_waiver_fields = [
        "waiver_id",
        "budget_id",
        "breach_id",
        "owner",
        "expires_at_utc",
        "evidence_paths",
    ]
    for waiver in waivers:
        if not isinstance(waiver, dict):
            failures.append("waiver entry drifted")
            continue
        for field_name in required_waiver_fields:
            if field_name not in waiver:
                failures.append(f"waiver entry missing required field {field_name}")
        try:
            expires_at = parse_timestamp(waiver.get("expires_at_utc"))
        except RuntimeError:
            failures.append("waiver entry has invalid expires_at_utc")
            continue
        status = "active" if expires_at >= now else "expired"
        waiver["status"] = status
        if status == "expired":
            expired_waivers.append(waiver)
    return expired_waivers
