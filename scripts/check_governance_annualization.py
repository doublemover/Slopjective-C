#!/usr/bin/env python3
"""Validate deterministic governance annualization payloads."""

from __future__ import annotations

import argparse
import json
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "governance-annualization-deterministic"
REQUIRED_FIELDS: tuple[str, ...] = (
    "annual_review_due_utc",
    "annual_review_completed_utc",
    "cadence.utc_anchor",
    "quorum.present_voters",
    "quorum.vendor_count",
    "quorum.spec_editor_present",
    "quorum.tooling_owner_present",
    "quorum.recusal_ratio",
    "publication_windows.attestation_deadline_utc",
    "publication_windows.minutes_deadline_utc",
    "publication_windows.packet_deadline_utc",
    "exceptions",
    "exceptions[].exception_id",
    "exceptions[].status",
    "exceptions[].opened_utc",
    "exceptions[].expires_utc",
    "exceptions[].renewed_utc",
    "exceptions[].renewal_expires_utc",
    "escalation_records",
    "escalation_records[].escalation_id",
    "escalation_records[].linked_exception_id",
    "escalation_records[].owner",
    "escalation_records[].opened_utc",
    "escalation_records[].response_due_utc",
    "escalation_records[].status",
    "escalation_records[].closed_utc",
)
EXCEPTION_STATUSES: tuple[str, ...] = ("proposed", "active", "expired", "closed")
ESCALATION_STATUSES: tuple[str, ...] = ("active", "closed", "superseded")
PUBLICATION_ATTESTATION_MAX_HOURS = 4
PUBLICATION_MINUTES_MAX_HOURS = 24
PUBLICATION_PACKET_MAX_HOURS = 72


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def resolve_input_path(raw_path: Path) -> Path:
    if raw_path.is_absolute():
        return raw_path
    return ROOT / raw_path


def write_stdout(value: str) -> None:
    buffer = getattr(sys.stdout, "buffer", None)
    if buffer is not None:
        buffer.write(value.encode("utf-8"))
        return
    sys.stdout.write(value)


def load_json(path: Path) -> Any:
    if not path.exists():
        raise ValueError(f"input JSON file does not exist: {display_path(path)}")
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except OSError as exc:
        raise ValueError(f"unable to read input JSON {display_path(path)}: {exc}") from exc
    except json.JSONDecodeError as exc:
        raise ValueError(f"invalid JSON in input file {display_path(path)}: {exc}") from exc


def require_object(value: Any, *, context: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise ValueError(f"{context} must be an object")
    return value


def require_list(value: Any, *, context: str) -> list[Any]:
    if not isinstance(value, list):
        raise ValueError(f"{context} must be an array")
    return value


def require_string(payload: dict[str, Any], *, key: str, context: str) -> str:
    value = payload.get(key)
    if not isinstance(value, str) or not value.strip():
        raise ValueError(f"{context} field '{key}' must be a non-empty string")
    return value.strip()


def require_string_allow_empty(payload: dict[str, Any], *, key: str, context: str) -> str:
    value = payload.get(key)
    if not isinstance(value, str):
        raise ValueError(f"{context} field '{key}' must be a string")
    return value


def require_optional_string(payload: dict[str, Any], *, key: str, context: str) -> str | None:
    value = payload.get(key)
    if value is None:
        return None
    if not isinstance(value, str):
        raise ValueError(f"{context} field '{key}' must be a string or null")
    normalized = value.strip()
    if not normalized:
        raise ValueError(f"{context} field '{key}' must not be empty when provided")
    return normalized


def require_int(payload: dict[str, Any], *, key: str, context: str) -> int:
    value = payload.get(key)
    if isinstance(value, bool) or not isinstance(value, int):
        raise ValueError(f"{context} field '{key}' must be an integer")
    return value


def require_bool(payload: dict[str, Any], *, key: str, context: str) -> bool:
    value = payload.get(key)
    if not isinstance(value, bool):
        raise ValueError(f"{context} field '{key}' must be a boolean")
    return value


def require_ratio(payload: dict[str, Any], *, key: str, context: str) -> float:
    value = payload.get(key)
    if isinstance(value, bool) or not isinstance(value, (int, float)):
        raise ValueError(f"{context} field '{key}' must be a number")
    ratio = float(value)
    if ratio < 0.0 or ratio > 1.0:
        raise ValueError(f"{context} field '{key}' must be in range [0.0, 1.0]")
    return ratio


def parse_utc_timestamp(raw_value: str, *, context: str, key: str) -> datetime:
    normalized = raw_value.strip()
    if normalized.endswith("Z"):
        normalized = f"{normalized[:-1]}+00:00"
    try:
        parsed = datetime.fromisoformat(normalized)
    except ValueError as exc:
        raise ValueError(f"{context} field '{key}' must be a valid ISO-8601 timestamp") from exc
    if parsed.tzinfo is None:
        raise ValueError(f"{context} field '{key}' must include timezone information")
    return parsed.astimezone(timezone.utc)


def parse_optional_utc_timestamp(
    payload: dict[str, Any], *, key: str, context: str
) -> datetime | None:
    value = payload.get(key)
    if value is None:
        return None
    if not isinstance(value, str) or not value.strip():
        raise ValueError(f"{context} field '{key}' must be a non-empty string or null")
    return parse_utc_timestamp(value, context=context, key=key)


def format_utc_timestamp(value: datetime) -> str:
    return value.astimezone(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z")


def bool_text(value: bool) -> str:
    return "true" if value else "false"


def validate_payload(payload: Any, *, input_path: Path) -> dict[str, object]:
    root = require_object(payload, context="input JSON root")

    annual_review_due_raw = require_string(root, key="annual_review_due_utc", context="input JSON")
    annual_review_completed_raw = require_string(
        root,
        key="annual_review_completed_utc",
        context="input JSON",
    )

    cadence = require_object(root.get("cadence"), context="input JSON field 'cadence'")
    cadence_anchor_raw = require_string(cadence, key="utc_anchor", context="cadence")

    quorum = require_object(root.get("quorum"), context="input JSON field 'quorum'")
    present_voters = require_int(quorum, key="present_voters", context="quorum")
    vendor_count = require_int(quorum, key="vendor_count", context="quorum")
    spec_editor_present = require_bool(quorum, key="spec_editor_present", context="quorum")
    tooling_owner_present = require_bool(quorum, key="tooling_owner_present", context="quorum")
    recusal_ratio = require_ratio(quorum, key="recusal_ratio", context="quorum")

    publication_windows = require_object(
        root.get("publication_windows"),
        context="input JSON field 'publication_windows'",
    )
    attestation_deadline_raw = require_string(
        publication_windows,
        key="attestation_deadline_utc",
        context="publication_windows",
    )
    minutes_deadline_raw = require_string(
        publication_windows,
        key="minutes_deadline_utc",
        context="publication_windows",
    )
    packet_deadline_raw = require_string(
        publication_windows,
        key="packet_deadline_utc",
        context="publication_windows",
    )
    exceptions_value = root.get("exceptions", [])
    exceptions_raw = require_list(exceptions_value, context="input JSON field 'exceptions'")
    escalation_records_value = root.get("escalation_records", [])
    escalation_records_raw = require_list(
        escalation_records_value,
        context="input JSON field 'escalation_records'",
    )

    annual_review_due = parse_utc_timestamp(
        annual_review_due_raw,
        context="input JSON",
        key="annual_review_due_utc",
    )
    annual_review_completed = parse_utc_timestamp(
        annual_review_completed_raw,
        context="input JSON",
        key="annual_review_completed_utc",
    )
    cadence_anchor = parse_utc_timestamp(cadence_anchor_raw, context="cadence", key="utc_anchor")
    attestation_deadline = parse_utc_timestamp(
        attestation_deadline_raw,
        context="publication_windows",
        key="attestation_deadline_utc",
    )
    minutes_deadline = parse_utc_timestamp(
        minutes_deadline_raw,
        context="publication_windows",
        key="minutes_deadline_utc",
    )
    packet_deadline = parse_utc_timestamp(
        packet_deadline_raw,
        context="publication_windows",
        key="packet_deadline_utc",
    )

    annual_review_pass = annual_review_completed <= annual_review_due
    q01_pass = present_voters >= 5
    q02_pass = vendor_count >= 3
    q03_pass = spec_editor_present and tooling_owner_present
    q04_pass = recusal_ratio <= 0.40
    quorum_pass = q01_pass and q02_pass and q03_pass and q04_pass

    anchor_before_attestation = cadence_anchor < attestation_deadline
    attestation_before_minutes = attestation_deadline < minutes_deadline
    minutes_before_packet = minutes_deadline < packet_deadline
    publication_chronology_pass = (
        anchor_before_attestation and attestation_before_minutes and minutes_before_packet
    )

    attestation_delta_hours = (attestation_deadline - cadence_anchor).total_seconds() / 3600.0
    minutes_delta_hours = (minutes_deadline - cadence_anchor).total_seconds() / 3600.0
    packet_delta_hours = (packet_deadline - cadence_anchor).total_seconds() / 3600.0
    attestation_within_deadline = (
        anchor_before_attestation and attestation_delta_hours <= PUBLICATION_ATTESTATION_MAX_HOURS
    )
    minutes_within_deadline = (
        attestation_before_minutes and minutes_delta_hours <= PUBLICATION_MINUTES_MAX_HOURS
    )
    packet_within_deadline = (
        minutes_before_packet and packet_delta_hours <= PUBLICATION_PACKET_MAX_HOURS
    )
    publication_deadline_regression_pass = (
        attestation_within_deadline and minutes_within_deadline and packet_within_deadline
    )
    failures: list[dict[str, str]] = []

    exception_records: list[dict[str, object]] = []
    expired_exception_ids: list[str] = []
    exception_expiry_order_pass = True
    exception_renewal_pair_pass = True
    exception_renewal_window_pass = True
    exception_state_valid_pass = True
    for index, raw_exception in enumerate(exceptions_raw):
        context = f"exceptions[{index}]"
        exception = require_object(raw_exception, context=context)
        exception_id = require_string(exception, key="exception_id", context=context)
        status = require_string(exception, key="status", context=context)
        if status not in EXCEPTION_STATUSES:
            raise ValueError(
                f"{context} field 'status' must be one of {', '.join(EXCEPTION_STATUSES)}"
            )
        opened_utc = parse_utc_timestamp(
            require_string(exception, key="opened_utc", context=context),
            context=context,
            key="opened_utc",
        )
        expires_utc = parse_utc_timestamp(
            require_string(exception, key="expires_utc", context=context),
            context=context,
            key="expires_utc",
        )
        renewed_utc = parse_optional_utc_timestamp(exception, key="renewed_utc", context=context)
        renewal_expires_utc = parse_optional_utc_timestamp(
            exception,
            key="renewal_expires_utc",
            context=context,
        )

        expiry_after_open = expires_utc > opened_utc
        renewal_pair_complete = (renewed_utc is None) == (renewal_expires_utc is None)
        renewal_window_valid = True
        if renewed_utc is not None and renewal_expires_utc is not None:
            renewal_window_valid = renewed_utc > expires_utc and renewal_expires_utc > renewed_utc

        effective_expires_utc = expires_utc
        if renewed_utc is not None and renewal_expires_utc is not None and renewal_window_valid:
            effective_expires_utc = renewal_expires_utc
        expired_at_review = annual_review_completed > effective_expires_utc
        state_valid = True
        if status in ("proposed", "active"):
            state_valid = not expired_at_review
        elif status == "expired":
            state_valid = expired_at_review

        if expired_at_review and status != "closed":
            expired_exception_ids.append(exception_id)

        exception_records.append(
            {
                "exception_id": exception_id,
                "status": status,
                "opened_utc": format_utc_timestamp(opened_utc),
                "expires_utc": format_utc_timestamp(expires_utc),
                "renewed_utc": (
                    format_utc_timestamp(renewed_utc) if renewed_utc is not None else None
                ),
                "renewal_expires_utc": (
                    format_utc_timestamp(renewal_expires_utc)
                    if renewal_expires_utc is not None
                    else None
                ),
                "effective_expires_utc": format_utc_timestamp(effective_expires_utc),
                "expired_at_review": expired_at_review,
                "predicates": {
                    "expiry_after_open": expiry_after_open,
                    "renewal_pair_complete": renewal_pair_complete,
                    "renewal_window_valid": renewal_window_valid,
                    "state_valid": state_valid,
                },
            }
        )

        if not expiry_after_open:
            failures.append(
                {
                    "code": "exception_expiry_window_invalid",
                    "message": f"exception '{exception_id}' must have expires_utc after opened_utc",
                }
            )
            exception_expiry_order_pass = False
        if not renewal_pair_complete:
            failures.append(
                {
                    "code": "exception_renewal_pair_incomplete",
                    "message": (
                        f"exception '{exception_id}' must provide both renewed_utc and "
                        "renewal_expires_utc together"
                    ),
                }
            )
            exception_renewal_pair_pass = False
        if not renewal_window_valid:
            failures.append(
                {
                    "code": "exception_renewal_window_invalid",
                    "message": (
                        f"exception '{exception_id}' renewal window must be strictly after "
                        "expires_utc"
                    ),
                }
            )
            exception_renewal_window_pass = False
        if not state_valid:
            if status == "expired":
                message = (
                    f"exception '{exception_id}' has status=expired but is not expired at "
                    "annual_review_completed_utc"
                )
                code = "exception_state_not_expired"
            else:
                message = (
                    f"exception '{exception_id}' is expired at annual_review_completed_utc "
                    "without a valid renewal window"
                )
                code = "exception_state_expired_without_renewal"
            failures.append({"code": code, "message": message})
            exception_state_valid_pass = False

    escalation_records: list[dict[str, object]] = []
    escalation_owner_pass = True
    escalation_response_window_pass = True
    escalation_closure_pass = True
    ew04_exception_links: set[str] = set()
    for index, raw_escalation in enumerate(escalation_records_raw):
        context = f"escalation_records[{index}]"
        escalation = require_object(raw_escalation, context=context)
        escalation_id = require_string(escalation, key="escalation_id", context=context)
        linked_exception_id = require_optional_string(
            escalation,
            key="linked_exception_id",
            context=context,
        )
        owner_raw = require_string_allow_empty(escalation, key="owner", context=context)
        owner = owner_raw.strip()
        opened_utc = parse_utc_timestamp(
            require_string(escalation, key="opened_utc", context=context),
            context=context,
            key="opened_utc",
        )
        response_due_utc = parse_utc_timestamp(
            require_string(escalation, key="response_due_utc", context=context),
            context=context,
            key="response_due_utc",
        )
        status = require_string(escalation, key="status", context=context)
        if status not in ESCALATION_STATUSES:
            raise ValueError(
                f"{context} field 'status' must be one of {', '.join(ESCALATION_STATUSES)}"
            )
        closed_utc = parse_optional_utc_timestamp(escalation, key="closed_utc", context=context)

        owner_complete = bool(owner)
        response_window_valid = response_due_utc > opened_utc
        closure_valid = True
        if status == "closed":
            closure_valid = closed_utc is not None and closed_utc >= opened_utc
        elif closed_utc is not None:
            closure_valid = False

        if escalation_id == "EW-M07-04" and linked_exception_id is not None:
            ew04_exception_links.add(linked_exception_id)

        escalation_records.append(
            {
                "escalation_id": escalation_id,
                "linked_exception_id": linked_exception_id,
                "owner": owner,
                "opened_utc": format_utc_timestamp(opened_utc),
                "response_due_utc": format_utc_timestamp(response_due_utc),
                "status": status,
                "closed_utc": format_utc_timestamp(closed_utc) if closed_utc is not None else None,
                "predicates": {
                    "owner_complete": owner_complete,
                    "response_window_valid": response_window_valid,
                    "closure_valid": closure_valid,
                },
            }
        )

        if not owner_complete:
            failures.append(
                {
                    "code": "escalation_owner_incomplete",
                    "message": f"escalation '{escalation_id}' owner must be non-empty",
                }
            )
            escalation_owner_pass = False
        if not response_window_valid:
            failures.append(
                {
                    "code": "escalation_response_window_invalid",
                    "message": (
                        f"escalation '{escalation_id}' must have response_due_utc after opened_utc"
                    ),
                }
            )
            escalation_response_window_pass = False
        if not closure_valid:
            failures.append(
                {
                    "code": "escalation_closure_invalid",
                    "message": (
                        f"escalation '{escalation_id}' closure fields are inconsistent with status"
                    ),
                }
            )
            escalation_closure_pass = False

    if not annual_review_pass:
        failures.append(
            {
                "code": "annual_review_deadline_miss",
                "message": (
                    "annual_review_completed_utc must be less than or equal to "
                    "annual_review_due_utc"
                ),
            }
        )
    if not q01_pass:
        failures.append({"code": "quorum_q01_fail", "message": "present_voters must be >= 5"})
    if not q02_pass:
        failures.append({"code": "quorum_q02_fail", "message": "vendor_count must be >= 3"})
    if not q03_pass:
        failures.append(
            {
                "code": "quorum_q03_fail",
                "message": "spec_editor_present and tooling_owner_present must both be true",
            }
        )
    if not q04_pass:
        failures.append({"code": "quorum_q04_fail", "message": "recusal_ratio must be <= 0.40"})
    if not anchor_before_attestation:
        failures.append(
            {
                "code": "publication_anchor_attestation_order_fail",
                "message": "cadence.utc_anchor must be before publication_windows.attestation_deadline_utc",
            }
        )
    if not attestation_before_minutes:
        failures.append(
            {
                "code": "publication_attestation_minutes_order_fail",
                "message": "publication_windows.attestation_deadline_utc must be before publication_windows.minutes_deadline_utc",
            }
        )
    if not minutes_before_packet:
        failures.append(
            {
                "code": "publication_minutes_packet_order_fail",
                "message": "publication_windows.minutes_deadline_utc must be before publication_windows.packet_deadline_utc",
            }
        )
    if not attestation_within_deadline:
        failures.append(
            {
                "code": "publication_attestation_deadline_regression",
                "message": (
                    "publication_windows.attestation_deadline_utc must be no later than "
                    "cadence.utc_anchor + 4h"
                ),
            }
        )
    if not minutes_within_deadline:
        failures.append(
            {
                "code": "publication_minutes_deadline_regression",
                "message": (
                    "publication_windows.minutes_deadline_utc must be no later than "
                    "cadence.utc_anchor + 24h"
                ),
            }
        )
    if not packet_within_deadline:
        failures.append(
            {
                "code": "publication_packet_deadline_regression",
                "message": (
                    "publication_windows.packet_deadline_utc must be no later than "
                    "cadence.utc_anchor + 72h"
                ),
            }
        )

    exception_escalation_linkage_pass = True
    for exception_id in expired_exception_ids:
        if exception_id not in ew04_exception_links:
            failures.append(
                {
                    "code": "exception_expiry_escalation_missing",
                    "message": (
                        f"exception '{exception_id}' is expired and requires an EW-M07-04 "
                        "escalation record"
                    ),
                }
            )
            exception_escalation_linkage_pass = False

    exit_code = 0 if not failures else 1
    result = "PASS" if exit_code == 0 else "FAIL"

    exception_renewal_pass = (
        exception_expiry_order_pass
        and exception_renewal_pair_pass
        and exception_renewal_window_pass
        and exception_state_valid_pass
    )
    escalation_contract_pass = (
        escalation_owner_pass and escalation_response_window_pass and escalation_closure_pass
    )

    return {
        "mode": MODE,
        "input": display_path(input_path),
        "required_fields": list(REQUIRED_FIELDS),
        "annual_review": {
            "annual_review_due_utc": format_utc_timestamp(annual_review_due),
            "annual_review_completed_utc": format_utc_timestamp(annual_review_completed),
            "completed_on_or_before_due": annual_review_pass,
        },
        "cadence": {"utc_anchor": format_utc_timestamp(cadence_anchor)},
        "quorum": {
            "present_voters": present_voters,
            "vendor_count": vendor_count,
            "spec_editor_present": spec_editor_present,
            "tooling_owner_present": tooling_owner_present,
            "recusal_ratio": recusal_ratio,
            "predicates": [
                {"id": "Q-01", "rule": "present_voters >= 5", "pass": q01_pass},
                {"id": "Q-02", "rule": "vendor_count >= 3", "pass": q02_pass},
                {
                    "id": "Q-03",
                    "rule": "spec_editor_present = true and tooling_owner_present = true",
                    "pass": q03_pass,
                },
                {"id": "Q-04", "rule": "recusal_ratio <= 0.40", "pass": q04_pass},
            ],
            "pass": quorum_pass,
        },
        "publication_windows": {
            "attestation_deadline_utc": format_utc_timestamp(attestation_deadline),
            "minutes_deadline_utc": format_utc_timestamp(minutes_deadline),
            "packet_deadline_utc": format_utc_timestamp(packet_deadline),
            "hours_from_anchor": {
                "attestation": attestation_delta_hours,
                "minutes": minutes_delta_hours,
                "packet": packet_delta_hours,
            },
            "chronology": {
                "anchor_before_attestation": anchor_before_attestation,
                "attestation_before_minutes": attestation_before_minutes,
                "minutes_before_packet": minutes_before_packet,
                "pass": publication_chronology_pass,
            },
            "deadline_regressions": {
                "attestation_within_4h": attestation_within_deadline,
                "minutes_within_24h": minutes_within_deadline,
                "packet_within_72h": packet_within_deadline,
                "pass": publication_deadline_regression_pass,
            },
        },
        "exceptions": {
            "count": len(exception_records),
            "records": exception_records,
            "pass": exception_renewal_pass and exception_escalation_linkage_pass,
        },
        "escalation_records": {
            "count": len(escalation_records),
            "records": escalation_records,
            "owner_completeness_pass": escalation_owner_pass,
            "pass": escalation_contract_pass,
        },
        "checks": {
            "annual_review_pass": annual_review_pass,
            "quorum_pass": quorum_pass,
            "publication_chronology_pass": publication_chronology_pass,
            "publication_deadline_regression_pass": publication_deadline_regression_pass,
            "exception_renewal_pass": exception_renewal_pass,
            "exception_escalation_linkage_pass": exception_escalation_linkage_pass,
            "escalation_owner_pass": escalation_owner_pass,
            "escalation_contract_pass": escalation_contract_pass,
        },
        "failures": failures,
        "result": result,
        "exit_code": exit_code,
    }


def render_json(payload: dict[str, object]) -> str:
    return json.dumps(payload, indent=2) + "\n"


def render_markdown(payload: dict[str, object]) -> str:
    annual_review = payload["annual_review"]
    cadence = payload["cadence"]
    quorum = payload["quorum"]
    publication_windows = payload["publication_windows"]
    exceptions = payload["exceptions"]
    escalation_records = payload["escalation_records"]
    checks = payload["checks"]
    failures = payload["failures"]
    assert isinstance(annual_review, dict)
    assert isinstance(cadence, dict)
    assert isinstance(quorum, dict)
    assert isinstance(publication_windows, dict)
    assert isinstance(exceptions, dict)
    assert isinstance(escalation_records, dict)
    assert isinstance(checks, dict)
    assert isinstance(failures, list)

    chronology = publication_windows["chronology"]
    deadline_regressions = publication_windows["deadline_regressions"]
    hours_from_anchor = publication_windows["hours_from_anchor"]
    predicates = quorum["predicates"]
    exception_rows = exceptions["records"]
    escalation_rows = escalation_records["records"]
    assert isinstance(chronology, dict)
    assert isinstance(deadline_regressions, dict)
    assert isinstance(hours_from_anchor, dict)
    assert isinstance(predicates, list)
    assert isinstance(exception_rows, list)
    assert isinstance(escalation_rows, list)

    lines = [
        "# Governance Annualization Check",
        "",
        f"- Mode: `{payload['mode']}`",
        f"- Input: `{payload['input']}`",
        f"- Result: `{payload['result']}`",
        f"- Exit code: `{payload['exit_code']}`",
        "",
        "## Annual Review",
        "",
        f"- annual_review_due_utc: `{annual_review['annual_review_due_utc']}`",
        f"- annual_review_completed_utc: `{annual_review['annual_review_completed_utc']}`",
        (
            "- completed_on_or_before_due: "
            f"`{bool_text(bool(annual_review['completed_on_or_before_due']))}`"
        ),
        "",
        "## Quorum",
        "",
        f"- present_voters: `{quorum['present_voters']}`",
        f"- vendor_count: `{quorum['vendor_count']}`",
        f"- spec_editor_present: `{bool_text(bool(quorum['spec_editor_present']))}`",
        f"- tooling_owner_present: `{bool_text(bool(quorum['tooling_owner_present']))}`",
        f"- recusal_ratio: `{quorum['recusal_ratio']}`",
        f"- quorum_pass: `{bool_text(bool(quorum['pass']))}`",
        "",
        "| Predicate | Rule | Pass |",
        "| --- | --- | --- |",
    ]
    for entry in predicates:
        assert isinstance(entry, dict)
        lines.append(
            f"| `{entry['id']}` | {entry['rule']} | `{bool_text(bool(entry['pass']))}` |"
        )

    lines.extend(
        [
            "",
            "## Publication Windows",
            "",
            f"- utc_anchor: `{cadence['utc_anchor']}`",
            (
                "- attestation_deadline_utc: "
                f"`{publication_windows['attestation_deadline_utc']}`"
            ),
            f"- minutes_deadline_utc: `{publication_windows['minutes_deadline_utc']}`",
            f"- packet_deadline_utc: `{publication_windows['packet_deadline_utc']}`",
            "",
            "| Chronology check | Pass |",
            "| --- | --- |",
            (
                "| `cadence.utc_anchor < publication_windows.attestation_deadline_utc` "
                f"| `{bool_text(bool(chronology['anchor_before_attestation']))}` |"
            ),
            (
                "| `publication_windows.attestation_deadline_utc < "
                "publication_windows.minutes_deadline_utc` "
                f"| `{bool_text(bool(chronology['attestation_before_minutes']))}` |"
            ),
            (
                "| `publication_windows.minutes_deadline_utc < "
                "publication_windows.packet_deadline_utc` "
                f"| `{bool_text(bool(chronology['minutes_before_packet']))}` |"
            ),
            (
                "| `publication_chronology_pass` "
                f"| `{bool_text(bool(chronology['pass']))}` |"
            ),
            "",
            "| Deadline regression check | Pass |",
            "| --- | --- |",
            (
                "| `publication_windows.attestation_deadline_utc <= cadence.utc_anchor + 4h` "
                f"| `{bool_text(bool(deadline_regressions['attestation_within_4h']))}` |"
            ),
            (
                "| `publication_windows.minutes_deadline_utc <= cadence.utc_anchor + 24h` "
                f"| `{bool_text(bool(deadline_regressions['minutes_within_24h']))}` |"
            ),
            (
                "| `publication_windows.packet_deadline_utc <= cadence.utc_anchor + 72h` "
                f"| `{bool_text(bool(deadline_regressions['packet_within_72h']))}` |"
            ),
            (
                "| `publication_deadline_regression_pass` "
                f"| `{bool_text(bool(deadline_regressions['pass']))}` |"
            ),
            "",
            f"- hours_from_anchor.attestation: `{hours_from_anchor['attestation']}`",
            f"- hours_from_anchor.minutes: `{hours_from_anchor['minutes']}`",
            f"- hours_from_anchor.packet: `{hours_from_anchor['packet']}`",
            "",
            "## Exceptions",
            "",
            f"- exception_count: `{exceptions['count']}`",
            f"- exceptions_pass: `{bool_text(bool(exceptions['pass']))}`",
            "",
            "## Escalation Records",
            "",
            f"- escalation_count: `{escalation_records['count']}`",
            (
                "- escalation_owner_completeness_pass: "
                f"`{bool_text(bool(escalation_records['owner_completeness_pass']))}`"
            ),
            (
                "- escalation_records_pass: "
                f"`{bool_text(bool(escalation_records['pass']))}`"
            ),
            "",
            "## Check Summary",
            "",
            f"- annual_review_pass: `{bool_text(bool(checks['annual_review_pass']))}`",
            f"- quorum_pass: `{bool_text(bool(checks['quorum_pass']))}`",
            (
                "- publication_chronology_pass: "
                f"`{bool_text(bool(checks['publication_chronology_pass']))}`"
            ),
            (
                "- publication_deadline_regression_pass: "
                f"`{bool_text(bool(checks['publication_deadline_regression_pass']))}`"
            ),
            (
                "- exception_renewal_pass: "
                f"`{bool_text(bool(checks['exception_renewal_pass']))}`"
            ),
            (
                "- exception_escalation_linkage_pass: "
                f"`{bool_text(bool(checks['exception_escalation_linkage_pass']))}`"
            ),
            (
                "- escalation_owner_pass: "
                f"`{bool_text(bool(checks['escalation_owner_pass']))}`"
            ),
            (
                "- escalation_contract_pass: "
                f"`{bool_text(bool(checks['escalation_contract_pass']))}`"
            ),
            "",
            "## Failures",
            "",
        ]
    )

    if exception_rows:
        lines.extend(
            [
                "### Exception Predicates",
                "",
                "| Exception ID | Status | Expired at review | Predicates pass |",
                "| --- | --- | --- | --- |",
            ]
        )
        for row in exception_rows:
            assert isinstance(row, dict)
            predicates_raw = row["predicates"]
            assert isinstance(predicates_raw, dict)
            predicates_pass = all(bool(value) for value in predicates_raw.values())
            lines.append(
                f"| `{row['exception_id']}` | `{row['status']}` | "
                f"`{bool_text(bool(row['expired_at_review']))}` | "
                f"`{bool_text(predicates_pass)}` |"
            )
        lines.append("")

    if escalation_rows:
        lines.extend(
            [
                "### Escalation Predicates",
                "",
                "| Escalation ID | Linked exception | Status | Predicates pass |",
                "| --- | --- | --- | --- |",
            ]
        )
        for row in escalation_rows:
            assert isinstance(row, dict)
            predicates_raw = row["predicates"]
            assert isinstance(predicates_raw, dict)
            predicates_pass = all(bool(value) for value in predicates_raw.values())
            linked_exception_id = row["linked_exception_id"] if row["linked_exception_id"] else "none"
            lines.append(
                f"| `{row['escalation_id']}` | `{linked_exception_id}` | "
                f"`{row['status']}` | `{bool_text(predicates_pass)}` |"
            )
        lines.append("")

    if failures:
        for failure in failures:
            assert isinstance(failure, dict)
            lines.append(f"- `{failure['code']}`: {failure['message']}")
    else:
        lines.append("- _none_")

    return "\n".join(lines).rstrip() + "\n"


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="check_governance_annualization.py",
        description=(
            "Validate deterministic governance annualization payload fields, quorum "
            "predicates, publication deadlines, exception renewal validity, and "
            "escalation evidence integrity."
        ),
    )
    parser.add_argument(
        "--input",
        type=Path,
        required=True,
        help="Path to governance annualization payload JSON.",
    )
    parser.add_argument(
        "--format",
        choices=("json", "markdown"),
        default="json",
        help="Output format: json (default) or markdown.",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    input_path = resolve_input_path(args.input)

    try:
        payload = load_json(input_path)
        report = validate_payload(payload, input_path=input_path)
    except ValueError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2

    if args.format == "markdown":
        write_stdout(render_markdown(report))
    else:
        write_stdout(render_json(report))

    return int(report["exit_code"])


if __name__ == "__main__":
    raise SystemExit(main())
