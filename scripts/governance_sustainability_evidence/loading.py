"""Evidence report loading and normalization helpers."""

from __future__ import annotations

import json
from collections.abc import Iterable, Mapping
from pathlib import Path
from typing import Any

from .model import JsonObject
from .paths import GOVERNANCE_REPORT_FAMILY, evidence_path


def normalize_json_object(payload: Mapping[str, Any] | None) -> JsonObject:
    return dict(payload) if isinstance(payload, Mapping) else {}


def normalize_generated_reports(generated_reports: Iterable[Any]) -> list[str]:
    return [str(path) for path in generated_reports]


def normalize_report_payloads(
    report_payloads: Mapping[str, Mapping[str, Any] | None],
) -> dict[str, JsonObject]:
    return {
        str(path): normalize_json_object(payload)
        for path, payload in report_payloads.items()
    }


def report_payload(
    report_payloads: Mapping[str, JsonObject],
    *path_parts: str,
) -> JsonObject:
    return report_payloads.get(evidence_path(*GOVERNANCE_REPORT_FAMILY, *path_parts), {})


def load_json_object(path: Path) -> JsonObject:
    with path.open("r", encoding="utf-8") as handle:
        payload = json.load(handle)
    return normalize_json_object(payload)


def load_optional_report_payload(root: Path, relative_path: str) -> JsonObject:
    path = root / relative_path
    return load_json_object(path) if path.is_file() else {}


def load_report_payloads(root: Path, generated_reports: Iterable[str]) -> dict[str, JsonObject]:
    return {
        relative_path: load_optional_report_payload(root, relative_path)
        for relative_path in generated_reports
    }


__all__ = [
    "load_json_object",
    "load_optional_report_payload",
    "load_report_payloads",
    "normalize_generated_reports",
    "normalize_json_object",
    "normalize_report_payloads",
    "report_payload",
]
