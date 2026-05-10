"""Governance sustainability evidence contract helpers."""

from __future__ import annotations

from .cli import build_argument_parser, main, parse_arguments
from .contracts import (
    BLOCKER_METADATA,
    CONTRACT_ID,
    OWNER_CONTRACTS,
    OWNER_SPLIT,
    PACKAGE_BRIDGE,
    PUBLIC_ACTIONS,
    SUPPORT_STATE,
)
from .loading import (
    load_json_object,
    load_optional_report_payload,
    load_report_payloads,
    normalize_generated_reports,
    normalize_json_object,
    normalize_report_payloads,
    report_payload,
)
from .model import (
    GovernanceSustainabilityEvidenceInputs,
    GovernanceSustainabilityEvidencePayloads,
    JsonObject,
)
from .paths import (
    GOVERNANCE_REPORT_FAMILY,
    SELF_GENERATED_REPORTS,
    TEMP_REPORT_ROOT,
    evidence_path,
)
from .rendering import (
    generated_at_utc,
    render_console_lines,
    render_evidence_payload,
    render_summary_payload,
)
from .reporting import build_governance_sustainability_evidence_payloads
from .schema_checks import (
    claim_audit,
    generated_report_failure,
    measured_budget_state,
    release_blockers,
)


__all__ = [
    "BLOCKER_METADATA",
    "CONTRACT_ID",
    "GOVERNANCE_REPORT_FAMILY",
    "GovernanceSustainabilityEvidenceInputs",
    "GovernanceSustainabilityEvidencePayloads",
    "JsonObject",
    "OWNER_CONTRACTS",
    "OWNER_SPLIT",
    "PACKAGE_BRIDGE",
    "PUBLIC_ACTIONS",
    "SELF_GENERATED_REPORTS",
    "SUPPORT_STATE",
    "TEMP_REPORT_ROOT",
    "build_argument_parser",
    "build_governance_sustainability_evidence_payloads",
    "claim_audit",
    "evidence_path",
    "generated_at_utc",
    "generated_report_failure",
    "load_json_object",
    "load_optional_report_payload",
    "load_report_payloads",
    "main",
    "measured_budget_state",
    "normalize_generated_reports",
    "normalize_json_object",
    "normalize_report_payloads",
    "parse_arguments",
    "release_blockers",
    "render_console_lines",
    "render_evidence_payload",
    "render_summary_payload",
    "report_payload",
]
