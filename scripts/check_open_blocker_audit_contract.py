#!/usr/bin/env python3
"""Fail-closed contract checker for open-blocker-audit artifacts."""

from __future__ import annotations

import argparse
import json
import re
import sys
from datetime import datetime
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "open-blocker-audit-contract-v1"

DEFAULT_CONTRACT_ID = "open-blocker-audit-runner"
DEFAULT_CONTRACT_VERSION = "v0.1"

SUMMARY_FILENAME = "open_blocker_audit_summary.json"
SNAPSHOT_FILENAME = "open_blockers.snapshot.json"
EXTRACT_LOG_FILENAME = "extract_open_blockers.log"
EXTRACT_COMMAND_KEY = "extract_open_blockers_snapshot_json"
EXTRACT_SCRIPT_DISPLAY_PATH = "scripts/extract_open_blockers.py"
EXTRACT_LOG_PREFIX = "# extract_open_blockers snapshot-json command output\n\n## stdout\n\n"
EXTRACT_LOG_SEPARATOR = "\n\n## stderr\n\n"

EXIT_OK = 0
EXIT_CONTRACT_DRIFT = 2

ISO_UTC_SECOND_RE = re.compile(r"^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z$")
WINDOWS_ABSOLUTE_RE = re.compile(r"^[A-Za-z]:/")

EXPECTED_SUMMARY_KEYS = [
    "runner",
    "contract_id",
    "contract_version",
    "inputs",
    "scope",
    "artifacts",
    "audit",
    "commands",
    "errors",
    "final_status",
    "final_exit_code",
]
EXPECTED_INPUT_KEYS = [
    "audit_root",
    "effective_audit_root",
    "include_globs",
    "exclude_paths",
    "extractor_exclude_paths",
    "generated_at_utc",
    "source",
]
EXPECTED_SCOPE_KEYS = ["included_markdown_count", "excluded_markdown_count"]
EXPECTED_ARTIFACT_KEYS = [
    "output_dir",
    "snapshot_json",
    "extract_log",
    "summary_json",
    "report_markdown",
]
EXPECTED_AUDIT_KEYS = ["extract_attempted", "extract_exit_code", "open_blocker_count"]
EXPECTED_COMMAND_KEYS = ["argv", "exit_code", "stdout_bytes", "stderr_bytes"]
EXPECTED_SNAPSHOT_KEYS = [
    "contract_id",
    "contract_version",
    "generated_at_utc",
    "source",
    "open_blocker_count",
    "open_blockers",
]
EXPECTED_SNAPSHOT_ROW_KEYS = ["blocker_id", "source_path", "line_number", "line"]


def normalize_newlines(text: str) -> str:
    return text.replace("\r\n", "\n").replace("\r", "\n")


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(normalize_newlines(content), encoding="utf-8", newline="\n")


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def resolve_input_path(raw_path: Path) -> Path:
    if raw_path.is_absolute():
        return raw_path
    return ROOT / raw_path


def canonical_json_text(payload: dict[str, object]) -> str:
    return json.dumps(payload, indent=2) + "\n"


def add_finding(findings: list[dict[str, str]], check_id: str, detail: str) -> None:
    findings.append({"check_id": check_id, "detail": detail})


def normalize_space(value: str) -> str:
    return " ".join(value.strip().split())


def validate_contract_arg(value: str, *, flag: str) -> str:
    if not value or value.strip() != value:
        raise ValueError(f"{flag} must be a non-empty canonical string")
    if normalize_space(value) != value:
        raise ValueError(f"{flag} must not contain repeated internal whitespace")
    if "/" in value:
        raise ValueError(f"{flag} must not include '/'")
    return value


def load_text(path: Path, *, label: str) -> str:
    if not path.exists():
        raise ValueError(f"{label} does not exist: {display_path(path)}")
    if not path.is_file():
        raise ValueError(f"{label} is not a file: {display_path(path)}")
    try:
        text = path.read_text(encoding="utf-8")
    except UnicodeDecodeError as exc:
        raise ValueError(f"{label} is not valid UTF-8: {display_path(path)}") from exc
    except OSError as exc:
        raise ValueError(f"unable to read {label} {display_path(path)}: {exc}") from exc

    if "\r" in text:
        raise ValueError(f"{label} must use LF newlines only: {display_path(path)}")
    if not text.endswith("\n"):
        raise ValueError(f"{label} must end with a trailing newline: {display_path(path)}")
    return text


def parse_json_object(raw: str, *, label: str, path: Path) -> dict[str, object]:
    try:
        payload = json.loads(raw)
    except json.JSONDecodeError as exc:
        raise ValueError(
            f"{label} is not valid JSON: {display_path(path)} ({exc.msg} at {exc.lineno}:{exc.colno})"
        ) from exc
    if not isinstance(payload, dict):
        raise ValueError(f"{label} root must be an object: {display_path(path)}")
    return payload


def check_key_order(
    findings: list[dict[str, str]],
    *,
    check_id: str,
    payload: object,
    expected: list[str],
    field_path: str,
) -> dict[str, object] | None:
    if not isinstance(payload, dict):
        add_finding(findings, check_id, f"{field_path} must be an object")
        return None
    observed = list(payload.keys())
    if observed != expected:
        add_finding(
            findings,
            check_id,
            f"{field_path} key order drift: expected={expected!r} observed={observed!r}",
        )
    return payload


def parse_non_negative_int(value: object, *, field_path: str) -> int | None:
    if isinstance(value, bool) or not isinstance(value, int) or value < 0:
        return None
    return value


def parse_positive_int(value: object, *, field_path: str) -> int | None:
    if isinstance(value, bool) or not isinstance(value, int) or value <= 0:
        return None
    return value


def validate_generated_at_utc(value: object) -> bool:
    if not isinstance(value, str) or value.strip() != value:
        return False
    if not ISO_UTC_SECOND_RE.fullmatch(value):
        return False
    try:
        datetime.strptime(value, "%Y-%m-%dT%H:%M:%SZ")
    except ValueError:
        return False
    return True


def validate_source(value: object) -> bool:
    if not isinstance(value, str) or not value:
        return False
    if value.strip() != value:
        return False
    return normalize_space(value) == value


def validate_display_path(value: object, *, allow_absolute: bool = True) -> bool:
    if not isinstance(value, str) or not value:
        return False
    if value.strip() != value:
        return False
    if "\\" in value:
        return False
    if not allow_absolute and (value.startswith("/") or WINDOWS_ABSOLUTE_RE.match(value)):
        return False
    return True


def determine_expected_final(errors: list[str], blocker_count: int | None) -> tuple[int, str]:
    if errors:
        return 2, "runner-error"
    if blocker_count is None:
        return 2, "runner-error"
    if blocker_count > 0:
        return 1, "open-blockers"
    return 0, "ok"


def validate_log_sections(log_text: str) -> tuple[str, str] | None:
    if not log_text.startswith(EXTRACT_LOG_PREFIX):
        return None
    body = log_text[len(EXTRACT_LOG_PREFIX) :]
    if EXTRACT_LOG_SEPARATOR not in body:
        return None
    stdout_block, stderr_with_nl = body.split(EXTRACT_LOG_SEPARATOR, 1)
    if not stderr_with_nl.endswith("\n"):
        return None
    stderr_block = stderr_with_nl[:-1]
    if not stdout_block or not stderr_block:
        return None
    return stdout_block, stderr_block


def validate_contract(
    *,
    summary_path: Path,
    snapshot_path: Path,
    extract_log_path: Path,
    contract_id: str,
    contract_version: str,
) -> list[dict[str, str]]:
    findings: list[dict[str, str]] = []

    if summary_path.name != SUMMARY_FILENAME:
        add_finding(
            findings,
            "OBA-PATH-01",
            f"summary path must end with {SUMMARY_FILENAME!r}; observed={display_path(summary_path)!r}",
        )
    if snapshot_path.name != SNAPSHOT_FILENAME:
        add_finding(
            findings,
            "OBA-PATH-02",
            f"snapshot path must end with {SNAPSHOT_FILENAME!r}; observed={display_path(snapshot_path)!r}",
        )
    if extract_log_path.name != EXTRACT_LOG_FILENAME:
        add_finding(
            findings,
            "OBA-PATH-03",
            f"extract log path must end with {EXTRACT_LOG_FILENAME!r}; observed={display_path(extract_log_path)!r}",
        )

    summary_text: str | None = None
    snapshot_text: str | None = None
    log_text: str | None = None

    try:
        summary_text = load_text(summary_path, label="summary artifact")
    except ValueError as exc:
        add_finding(findings, "OBA-SUM-00", str(exc))
    try:
        snapshot_text = load_text(snapshot_path, label="snapshot artifact")
    except ValueError as exc:
        add_finding(findings, "OBA-SNAP-00", str(exc))
    try:
        log_text = load_text(extract_log_path, label="extract log artifact")
    except ValueError as exc:
        add_finding(findings, "OBA-LOG-00", str(exc))

    if summary_text is None or snapshot_text is None or log_text is None:
        return sorted(findings, key=lambda item: item["check_id"])

    summary_payload: dict[str, object] | None = None
    snapshot_payload: dict[str, object] | None = None

    try:
        summary_payload = parse_json_object(summary_text, label="summary artifact", path=summary_path)
    except ValueError as exc:
        add_finding(findings, "OBA-SUM-01", str(exc))
    try:
        snapshot_payload = parse_json_object(snapshot_text, label="snapshot artifact", path=snapshot_path)
    except ValueError as exc:
        add_finding(findings, "OBA-SNAP-01", str(exc))

    if summary_payload is None or snapshot_payload is None:
        return sorted(findings, key=lambda item: item["check_id"])

    if summary_text != canonical_json_text(summary_payload):
        add_finding(
            findings,
            "OBA-SUM-02",
            "summary JSON formatting/key-order drift from canonical json.dumps(..., indent=2)+LF",
        )
    if snapshot_text != canonical_json_text(snapshot_payload):
        add_finding(
            findings,
            "OBA-SNAP-02",
            "snapshot JSON formatting/key-order drift from canonical json.dumps(..., indent=2)+LF",
        )

    summary = check_key_order(
        findings,
        check_id="OBA-SUM-03",
        payload=summary_payload,
        expected=EXPECTED_SUMMARY_KEYS,
        field_path="summary",
    )
    snapshot = check_key_order(
        findings,
        check_id="OBA-SNAP-03",
        payload=snapshot_payload,
        expected=EXPECTED_SNAPSHOT_KEYS,
        field_path="snapshot",
    )
    if summary is None or snapshot is None:
        return sorted(findings, key=lambda item: item["check_id"])

    expected_runner = f"{contract_id}/{contract_version}"
    if summary.get("runner") != expected_runner:
        add_finding(
            findings,
            "OBA-SUM-04",
            f"summary.runner drift: expected={expected_runner!r} observed={summary.get('runner')!r}",
        )
    if summary.get("contract_id") != contract_id:
        add_finding(
            findings,
            "OBA-SUM-05",
            f"summary.contract_id drift: expected={contract_id!r} observed={summary.get('contract_id')!r}",
        )
    if summary.get("contract_version") != contract_version:
        add_finding(
            findings,
            "OBA-SUM-06",
            f"summary.contract_version drift: expected={contract_version!r} observed={summary.get('contract_version')!r}",
        )

    if snapshot.get("contract_id") != contract_id:
        add_finding(
            findings,
            "OBA-SNAP-04",
            f"snapshot.contract_id drift: expected={contract_id!r} observed={snapshot.get('contract_id')!r}",
        )
    if snapshot.get("contract_version") != contract_version:
        add_finding(
            findings,
            "OBA-SNAP-05",
            f"snapshot.contract_version drift: expected={contract_version!r} observed={snapshot.get('contract_version')!r}",
        )

    inputs = check_key_order(
        findings,
        check_id="OBA-SUM-07",
        payload=summary.get("inputs"),
        expected=EXPECTED_INPUT_KEYS,
        field_path="summary.inputs",
    )
    scope = check_key_order(
        findings,
        check_id="OBA-SUM-08",
        payload=summary.get("scope"),
        expected=EXPECTED_SCOPE_KEYS,
        field_path="summary.scope",
    )
    artifacts = check_key_order(
        findings,
        check_id="OBA-SUM-09",
        payload=summary.get("artifacts"),
        expected=EXPECTED_ARTIFACT_KEYS,
        field_path="summary.artifacts",
    )
    audit = check_key_order(
        findings,
        check_id="OBA-SUM-10",
        payload=summary.get("audit"),
        expected=EXPECTED_AUDIT_KEYS,
        field_path="summary.audit",
    )

    commands_obj = summary.get("commands")
    if not isinstance(commands_obj, dict):
        add_finding(findings, "OBA-SUM-11", "summary.commands must be an object")
        commands_obj = None

    errors_obj = summary.get("errors")
    if not isinstance(errors_obj, list) or any(not isinstance(item, str) for item in errors_obj):
        add_finding(findings, "OBA-SUM-12", "summary.errors must be a list of strings")
        errors_obj = []

    final_status = summary.get("final_status")
    final_exit_code = summary.get("final_exit_code")
    if not isinstance(final_status, str):
        add_finding(findings, "OBA-SUM-13", "summary.final_status must be a string")
    if parse_non_negative_int(final_exit_code, field_path="summary.final_exit_code") is None:
        add_finding(findings, "OBA-SUM-14", "summary.final_exit_code must be a non-negative integer")

    if inputs is not None:
        for field in ("audit_root", "effective_audit_root"):
            if not validate_display_path(inputs.get(field)):
                add_finding(findings, "OBA-SUM-15", f"summary.inputs.{field} must be canonical display path")
        for field in ("include_globs", "exclude_paths", "extractor_exclude_paths"):
            value = inputs.get(field)
            if not isinstance(value, list) or any(not isinstance(item, str) or item.strip() != item for item in value):
                add_finding(findings, "OBA-SUM-16", f"summary.inputs.{field} must be a list of canonical strings")

        if not validate_generated_at_utc(inputs.get("generated_at_utc")):
            add_finding(findings, "OBA-SUM-17", "summary.inputs.generated_at_utc must be strict UTC timestamp")
        if not validate_source(inputs.get("source")):
            add_finding(findings, "OBA-SUM-18", "summary.inputs.source must be canonical non-empty source")

    if scope is not None:
        for field in ("included_markdown_count", "excluded_markdown_count"):
            if parse_non_negative_int(scope.get(field), field_path=f"summary.scope.{field}") is None:
                add_finding(findings, "OBA-SUM-19", f"summary.scope.{field} must be non-negative integer")

    command_payload: dict[str, object] | None = None
    if artifacts is not None and audit is not None and commands_obj is not None:
        extract_attempted = audit.get("extract_attempted")
        if not isinstance(extract_attempted, bool):
            add_finding(findings, "OBA-SUM-20", "summary.audit.extract_attempted must be boolean")
            extract_attempted = False

        extract_exit_code = parse_non_negative_int(
            audit.get("extract_exit_code"), field_path="summary.audit.extract_exit_code"
        )
        if extract_attempted and extract_exit_code is None:
            add_finding(
                findings,
                "OBA-SUM-21",
                "summary.audit.extract_exit_code must be non-negative integer when extract_attempted=true",
            )

        if extract_attempted:
            if list(commands_obj.keys()) != [EXTRACT_COMMAND_KEY]:
                add_finding(
                    findings,
                    "OBA-SUM-22",
                    f"summary.commands keys drift: expected={[EXTRACT_COMMAND_KEY]!r} observed={list(commands_obj.keys())!r}",
                )
            command_candidate = commands_obj.get(EXTRACT_COMMAND_KEY)
            command_payload = check_key_order(
                findings,
                check_id="OBA-SUM-23",
                payload=command_candidate,
                expected=EXPECTED_COMMAND_KEYS,
                field_path=f"summary.commands.{EXTRACT_COMMAND_KEY}",
            )
            if artifacts.get("extract_log") != EXTRACT_LOG_FILENAME:
                add_finding(
                    findings,
                    "OBA-SUM-24",
                    f"summary.artifacts.extract_log drift: expected={EXTRACT_LOG_FILENAME!r} observed={artifacts.get('extract_log')!r}",
                )
        else:
            if commands_obj:
                add_finding(findings, "OBA-SUM-25", "summary.commands must be empty when extract_attempted=false")
            if audit.get("extract_exit_code") is not None:
                add_finding(findings, "OBA-SUM-26", "summary.audit.extract_exit_code must be null when extract_attempted=false")
            if artifacts.get("extract_log") is not None:
                add_finding(findings, "OBA-SUM-27", "summary.artifacts.extract_log must be null when extract_attempted=false")

    snapshot_rows = snapshot.get("open_blockers")
    open_blocker_count = parse_non_negative_int(snapshot.get("open_blocker_count"), field_path="snapshot.open_blocker_count")
    if open_blocker_count is None:
        add_finding(findings, "OBA-SNAP-06", "snapshot.open_blocker_count must be non-negative integer")
        open_blocker_count = 0

    if not validate_generated_at_utc(snapshot.get("generated_at_utc")):
        add_finding(findings, "OBA-SNAP-07", "snapshot.generated_at_utc must be strict UTC timestamp")
    if not validate_source(snapshot.get("source")):
        add_finding(findings, "OBA-SNAP-08", "snapshot.source must be canonical non-empty source")

    if not isinstance(snapshot_rows, list):
        add_finding(findings, "OBA-SNAP-09", "snapshot.open_blockers must be a list")
        snapshot_rows = []

    canonical_rows: list[dict[str, object]] = []
    seen_row_ids: set[tuple[str, str, int]] = set()
    for index, row in enumerate(snapshot_rows):
        row_path = f"snapshot.open_blockers[{index}]"
        row_obj = check_key_order(
            findings,
            check_id="OBA-SNAP-10",
            payload=row,
            expected=EXPECTED_SNAPSHOT_ROW_KEYS,
            field_path=row_path,
        )
        if row_obj is None:
            continue

        blocker_id = row_obj.get("blocker_id")
        source_path = row_obj.get("source_path")
        line_number = parse_positive_int(row_obj.get("line_number"), field_path=f"{row_path}.line_number")
        line_alias = parse_positive_int(row_obj.get("line"), field_path=f"{row_path}.line")

        if not isinstance(blocker_id, str) or not blocker_id or blocker_id.strip() != blocker_id:
            add_finding(findings, "OBA-SNAP-11", f"{row_path}.blocker_id must be canonical non-empty string")
            continue
        if not validate_display_path(source_path, allow_absolute=False):
            add_finding(findings, "OBA-SNAP-12", f"{row_path}.source_path must be canonical relative path")
            continue
        if line_number is None:
            add_finding(findings, "OBA-SNAP-13", f"{row_path}.line_number must be positive integer")
            continue
        if line_alias is None:
            add_finding(findings, "OBA-SNAP-14", f"{row_path}.line must be positive integer")
            continue
        if line_alias != line_number:
            add_finding(
                findings,
                "OBA-SNAP-15",
                f"line alias mismatch for {row_path}: line_number={line_number} line={line_alias}",
            )

        identity = (blocker_id, str(source_path), line_number)
        if identity in seen_row_ids:
            add_finding(findings, "OBA-SNAP-16", f"duplicate snapshot row detected: {identity!r}")
        seen_row_ids.add(identity)

        canonical_rows.append(
            {
                "blocker_id": blocker_id,
                "source_path": str(source_path),
                "line_number": line_number,
                "line": line_number,
            }
        )

    expected_order = sorted(
        canonical_rows,
        key=lambda item: (str(item["source_path"]), int(item["line_number"]), str(item["blocker_id"])),
    )
    if canonical_rows != expected_order:
        add_finding(
            findings,
            "OBA-SNAP-17",
            "snapshot.open_blockers must be sorted by source_path, line_number, blocker_id",
        )

    if open_blocker_count != len(canonical_rows):
        add_finding(
            findings,
            "OBA-SNAP-18",
            f"snapshot.open_blocker_count mismatch: declared={open_blocker_count} observed={len(canonical_rows)}",
        )

    if inputs is not None:
        if inputs.get("generated_at_utc") != snapshot.get("generated_at_utc"):
            add_finding(
                findings,
                "OBA-X-01",
                f"generated_at_utc parity drift: summary.inputs={inputs.get('generated_at_utc')!r} snapshot={snapshot.get('generated_at_utc')!r}",
            )
        if inputs.get("source") != snapshot.get("source"):
            add_finding(
                findings,
                "OBA-X-02",
                f"source parity drift: summary.inputs={inputs.get('source')!r} snapshot={snapshot.get('source')!r}",
            )

    if artifacts is not None:
        if artifacts.get("snapshot_json") != display_path(snapshot_path):
            add_finding(
                findings,
                "OBA-X-03",
                f"summary.artifacts.snapshot_json parity drift: expected={display_path(snapshot_path)!r} observed={artifacts.get('snapshot_json')!r}",
            )
        if artifacts.get("summary_json") != SUMMARY_FILENAME:
            add_finding(
                findings,
                "OBA-X-04",
                f"summary.artifacts.summary_json drift: expected={SUMMARY_FILENAME!r} observed={artifacts.get('summary_json')!r}",
            )

    if audit is not None:
        audit_count = parse_non_negative_int(audit.get("open_blocker_count"), field_path="summary.audit.open_blocker_count")
        if audit_count is None:
            add_finding(findings, "OBA-X-05", "summary.audit.open_blocker_count must be non-negative integer")
        elif audit_count != open_blocker_count:
            add_finding(
                findings,
                "OBA-X-06",
                f"open_blocker_count parity drift: summary.audit={audit_count} snapshot={open_blocker_count}",
            )

    if isinstance(errors_obj, list):
        expected_exit_code, expected_status = determine_expected_final(errors_obj, open_blocker_count)
        if final_status != expected_status or final_exit_code != expected_exit_code:
            add_finding(
                findings,
                "OBA-X-07",
                "status/exit mismatch: final_status/final_exit_code determine_final_exit drift: "
                f"expected=({expected_status!r}, {expected_exit_code}) "
                f"observed=({final_status!r}, {final_exit_code!r})",
            )

    log_sections = validate_log_sections(log_text)
    if log_sections is None:
        add_finding(
            findings,
            "OBA-LOG-01",
            "extract log format drift: expected deterministic stdout/stderr markdown sections",
        )
    else:
        stdout_block, stderr_block = log_sections
        expected_stdout_block = canonical_json_text(
            {
                "generated_at_utc": snapshot.get("generated_at_utc"),
                "source": snapshot.get("source"),
                "open_blocker_count": open_blocker_count,
                "open_blockers": canonical_rows,
            }
        ).rstrip("\n")
        if stdout_block != expected_stdout_block:
            add_finding(
                findings,
                "OBA-LOG-02",
                "extract log stdout block drift: expected canonical runner snapshot JSON payload",
            )
        if stderr_block != "_empty_":
            add_finding(
                findings,
                "OBA-LOG-03",
                f"extract log stderr drift: expected '_empty_' observed={stderr_block!r}",
            )

        if command_payload is not None:
            argv = command_payload.get("argv")
            if not isinstance(argv, list) or any(not isinstance(token, str) for token in argv):
                add_finding(findings, "OBA-X-08", "summary.commands argv must be list of strings")
            else:
                expected_argv = [
                    "python",
                    EXTRACT_SCRIPT_DISPLAY_PATH,
                    "--root",
                    str(inputs.get("effective_audit_root")) if inputs is not None else "",
                    "--format",
                    "snapshot-json",
                    "--generated-at-utc",
                    str(snapshot.get("generated_at_utc")),
                    "--source",
                    str(snapshot.get("source")),
                ]
                if inputs is not None and isinstance(inputs.get("extractor_exclude_paths"), list):
                    for pattern in inputs["extractor_exclude_paths"]:
                        expected_argv.extend(["--exclude-path", pattern])
                if argv != expected_argv:
                    add_finding(
                        findings,
                        "OBA-X-09",
                        f"extract argv parity drift: expected={expected_argv!r} observed={argv!r}",
                    )

            exit_code = parse_non_negative_int(command_payload.get("exit_code"), field_path="commands.exit_code")
            stdout_bytes = parse_non_negative_int(command_payload.get("stdout_bytes"), field_path="commands.stdout_bytes")
            stderr_bytes = parse_non_negative_int(command_payload.get("stderr_bytes"), field_path="commands.stderr_bytes")
            if exit_code is None:
                add_finding(findings, "OBA-X-10", "summary.commands.exit_code must be non-negative integer")
            if stdout_bytes is None:
                add_finding(findings, "OBA-X-11", "summary.commands.stdout_bytes must be non-negative integer")
            if stderr_bytes is None:
                add_finding(findings, "OBA-X-12", "summary.commands.stderr_bytes must be non-negative integer")

            if audit is not None:
                audit_exit = parse_non_negative_int(audit.get("extract_exit_code"), field_path="audit.extract_exit_code")
                if audit_exit is not None and exit_code is not None and audit_exit != exit_code:
                    add_finding(
                        findings,
                        "OBA-X-13",
                        f"extract exit-code parity drift: summary.audit={audit_exit} summary.commands={exit_code}",
                    )

            computed_stdout_bytes = len((stdout_block + "\n").encode("utf-8"))
            computed_stderr_bytes = 0 if stderr_block == "_empty_" else len((stderr_block + "\n").encode("utf-8"))
            if stdout_bytes is not None and stdout_bytes != computed_stdout_bytes:
                add_finding(
                    findings,
                    "OBA-X-14",
                    f"stdout_bytes parity drift: summary={stdout_bytes} computed={computed_stdout_bytes}",
                )
            if stderr_bytes is not None and stderr_bytes != computed_stderr_bytes:
                add_finding(
                    findings,
                    "OBA-X-15",
                    f"stderr_bytes parity drift: summary={stderr_bytes} computed={computed_stderr_bytes}",
                )

    return sorted(findings, key=lambda item: item["check_id"])


def format_command_token(token: str) -> str:
    if not token:
        return '""'
    if any(char.isspace() for char in token) or '"' in token:
        return json.dumps(token)
    return token


def render_invocation(argv: Sequence[str]) -> str:
    tokens = ["python", "scripts/check_open_blocker_audit_contract.py", *argv]
    return " ".join(format_command_token(token) for token in tokens)


def render_transcript(
    *,
    argv: Sequence[str],
    output_mode: str,
    summary_path: Path,
    snapshot_path: Path,
    extract_log_path: Path,
    contract_id: str,
    contract_version: str,
    stdout_text: str,
    stderr_text: str,
    exit_code: int,
) -> str:
    stdout_block = stdout_text if stdout_text else "_empty_\n"
    stderr_block = stderr_text if stderr_text else "_empty_\n"
    if stdout_block and not stdout_block.endswith("\n"):
        stdout_block += "\n"
    if stderr_block and not stderr_block.endswith("\n"):
        stderr_block += "\n"

    return (
        "# check_open_blocker_audit_contract command output\n\n"
        f"- checker_mode: {MODE}\n"
        f"- output_mode: {output_mode}\n"
        f"- summary: {display_path(summary_path)}\n"
        f"- snapshot: {display_path(snapshot_path)}\n"
        f"- extract_log: {display_path(extract_log_path)}\n"
        f"- contract_id: {contract_id}\n"
        f"- contract_version: {contract_version}\n\n"
        "## command\n\n"
        f"{render_invocation(argv)}\n\n"
        "## stdout\n\n"
        f"{stdout_block}\n"
        "## stderr\n\n"
        f"{stderr_block}\n"
        "## exit_code\n\n"
        f"{exit_code}\n"
    )


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="check_open_blocker_audit_contract.py",
        description=(
            "Validate deterministic fail-closed contract parity across "
            "open_blocker_audit_summary.json, inputs/open_blockers.snapshot.json, "
            "and extract_open_blockers.log artifacts."
        ),
    )
    parser.add_argument(
        "--summary",
        "--summary-json",
        "--summary-path",
        dest="summary",
        type=Path,
        required=True,
        help="Path to open_blocker_audit_summary.json.",
    )
    parser.add_argument(
        "--snapshot",
        "--snapshot-json",
        "--snapshot-path",
        dest="snapshot",
        type=Path,
        required=True,
        help="Path to inputs/open_blockers.snapshot.json.",
    )
    parser.add_argument(
        "--extract-log",
        "--log",
        "--extract-log-path",
        "--log-path",
        dest="extract_log",
        type=Path,
        required=True,
        help="Path to extract_open_blockers.log.",
    )
    parser.add_argument(
        "--contract-id",
        default=DEFAULT_CONTRACT_ID,
        help=f"Expected contract_id value in summary/snapshot (default: {DEFAULT_CONTRACT_ID})",
    )
    parser.add_argument(
        "--contract-version",
        default=DEFAULT_CONTRACT_VERSION,
        help=f"Expected contract_version value in summary/snapshot (default: {DEFAULT_CONTRACT_VERSION})",
    )
    parser.add_argument(
        "--json-indent",
        type=int,
        default=2,
        help="JSON output indent width (default: 2)",
    )
    parser.add_argument(
        "--output-mode",
        "--output-format",
        dest="output_mode",
        choices=("json", "deterministic-json"),
        default="json",
        help=(
            "Output mode for stdout payload: json honors --json-indent; "
            "deterministic-json emits canonical indent=2 output."
        ),
    )
    parser.add_argument(
        "--transcript",
        "--transcript-path",
        dest="transcript",
        type=Path,
        help=(
            "Optional path to write deterministic command transcript markdown "
            "(stdout/stderr/exit captured)."
        ),
    )
    return parser


def render_fail_closed_diagnostics(findings: list[dict[str, str]], *, extract_log_path: Path) -> str:
    lines = [
        "open-blocker-audit-contract: contract drift detected "
        f"({len(findings)} finding(s)).",
    ]

    check_ids = {item["check_id"] for item in findings}
    if any(check_id.startswith("OBA-SUM-") for check_id in check_ids):
        lines.append("summary schema drift: contract_id contract_version key order drift")
    if any(check_id.startswith("OBA-SNAP-") for check_id in check_ids):
        lines.append("snapshot parity: key order drift contract_id contract_version")
    if any(check_id.startswith("OBA-LOG-") for check_id in check_ids):
        lines.append(f"log contract drift: {extract_log_path.name}")
    if any(check_id == "OBA-X-07" for check_id in check_ids):
        lines.append("status/exit mismatch: final_status final_exit_code")

    lines.append("drift findings:")
    for finding in findings:
        lines.append(f"- [{finding['check_id']}] {finding['detail']}")
    return "\n".join(lines) + "\n"


def main(argv: Sequence[str] | None = None) -> int:
    cli_argv = list(argv) if argv is not None else list(sys.argv[1:])
    args = build_parser().parse_args(cli_argv)
    summary_path = resolve_input_path(args.summary)
    snapshot_path = resolve_input_path(args.snapshot)
    extract_log_path = resolve_input_path(args.extract_log)

    contract_id = DEFAULT_CONTRACT_ID
    contract_version = DEFAULT_CONTRACT_VERSION
    exit_code = EXIT_OK
    stdout_text = ""
    stderr_text = ""

    try:
        contract_id = validate_contract_arg(args.contract_id, flag="--contract-id")
        contract_version = validate_contract_arg(args.contract_version, flag="--contract-version")
    except ValueError as exc:
        stderr_text = f"open-blocker-audit-contract: error: {exc}\n"
        exit_code = EXIT_CONTRACT_DRIFT
    else:
        findings = validate_contract(
            summary_path=summary_path,
            snapshot_path=snapshot_path,
            extract_log_path=extract_log_path,
            contract_id=contract_id,
            contract_version=contract_version,
        )

        if findings:
            stderr_text = render_fail_closed_diagnostics(findings, extract_log_path=extract_log_path)
            exit_code = EXIT_CONTRACT_DRIFT
        else:
            output = {
                "mode": MODE,
                "contract": {
                    "expected_runner": f"{contract_id}/{contract_version}",
                    "contract_id": contract_id,
                    "contract_version": contract_version,
                },
                "artifacts": {
                    "summary": display_path(summary_path),
                    "snapshot": display_path(snapshot_path),
                    "extract_log": display_path(extract_log_path),
                },
                "ok": True,
                "exit_code": EXIT_OK,
                "finding_count": 0,
                "findings": [],
            }
            if args.output_mode == "deterministic-json":
                stdout_text = canonical_json_text(output)
            else:
                stdout_text = json.dumps(output, indent=max(args.json_indent, 0)) + "\n"
            exit_code = EXIT_OK

    if args.transcript is not None:
        transcript_path = resolve_input_path(args.transcript)
        transcript_text = render_transcript(
            argv=cli_argv,
            output_mode=args.output_mode,
            summary_path=summary_path,
            snapshot_path=snapshot_path,
            extract_log_path=extract_log_path,
            contract_id=contract_id,
            contract_version=contract_version,
            stdout_text=stdout_text,
            stderr_text=stderr_text,
            exit_code=exit_code,
        )
        try:
            write_text(transcript_path, transcript_text)
        except OSError as exc:
            stderr_text = (
                f"{stderr_text}open-blocker-audit-contract: error: unable to write transcript "
                f"{display_path(transcript_path)}: {exc}\n"
            )
            stdout_text = ""
            exit_code = EXIT_CONTRACT_DRIFT

    if stdout_text:
        print(stdout_text, end="")
    if stderr_text:
        print(stderr_text, file=sys.stderr, end="")
    return exit_code


if __name__ == "__main__":
    raise SystemExit(main())
