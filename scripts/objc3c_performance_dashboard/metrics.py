from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json

from objc3c_performance_dashboard.paths import ROOT


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
