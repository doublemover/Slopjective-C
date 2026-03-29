#!/usr/bin/env python3
"""Validate open-blocker audit runner artifacts against the checker contract."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CHECKER_MODE = "open-blocker-audit-contract-v1"
EXIT_OK = 0
EXIT_CONTRACT_DRIFT = 2

SUMMARY_KEYS = [
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
SNAPSHOT_KEYS = [
    "contract_id",
    "contract_version",
    "generated_at_utc",
    "source",
    "open_blocker_count",
    "open_blockers",
]
FINAL_STATUS_TO_EXIT = {
    "ok": 0,
    "open-blockers": 1,
    "runner-error": 2,
}


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def load_json(path: Path, *, label: str) -> dict[str, Any]:
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError:
        raise ValueError(f"{label} file does not exist: {display_path(path)}") from None
    except json.JSONDecodeError as exc:
        raise ValueError(
            f"{label} is not valid JSON: {exc.msg} at {exc.lineno}:{exc.colno}"
        ) from None
    if not isinstance(payload, dict):
        raise ValueError(f"{label} root must be a JSON object")
    return payload


def check_key_order(payload: dict[str, Any], *, expected: list[str], label: str) -> list[str]:
    observed = list(payload.keys())
    if observed == expected:
        return []
    return [
        f"{label} key order drift: expected={expected!r} observed={observed!r}."
    ]


def validate_extract_log(extract_log_path: Path) -> list[str]:
    try:
        text = extract_log_path.read_text(encoding="utf-8")
    except FileNotFoundError:
        return [f"extract log does not exist: {display_path(extract_log_path)}."]

    findings: list[str] = []
    if not text.startswith("# extract_open_blockers snapshot-json command output\n"):
        findings.append(
            "extract log header drift: expected '# extract_open_blockers snapshot-json command output'."
        )
    if "\n## stdout\n" not in text:
        findings.append("extract log missing '## stdout' section.")
    if "\n## stderr\n" not in text:
        findings.append("extract log missing '## stderr' section.")
    return findings


def validate_summary(
    summary: dict[str, Any],
    *,
    expected_runner: str,
    contract_id: str,
    contract_version: str,
    summary_path: Path,
    snapshot_path: Path,
    extract_log_path: Path,
) -> list[str]:
    findings = check_key_order(summary, expected=SUMMARY_KEYS, label="summary")

    if summary.get("runner") != expected_runner:
        findings.append(
            f"summary.runner drift: expected={expected_runner!r} observed={summary.get('runner')!r}."
        )
    if summary.get("contract_id") != contract_id:
        findings.append(
            f"summary.contract_id drift: expected={contract_id!r} observed={summary.get('contract_id')!r}."
        )
    if summary.get("contract_version") != contract_version:
        findings.append(
            f"summary.contract_version drift: expected={contract_version!r} observed={summary.get('contract_version')!r}."
        )

    artifacts = summary.get("artifacts")
    if not isinstance(artifacts, dict):
        findings.append("summary.artifacts must be an object.")
    else:
        expected_artifacts = {
            "summary_json": summary_path.name,
            "snapshot_json": display_path(snapshot_path),
            "extract_log": extract_log_path.name,
        }
        for key, expected_value in expected_artifacts.items():
            observed = artifacts.get(key)
            if observed != expected_value:
                findings.append(
                    f"summary.artifacts.{key} drift: expected={expected_value!r} observed={observed!r}."
                )

    final_status = summary.get("final_status")
    final_exit_code = summary.get("final_exit_code")
    expected_exit_code = FINAL_STATUS_TO_EXIT.get(final_status)
    if expected_exit_code is None:
        findings.append(f"summary.final_status is invalid: {final_status!r}.")
    elif final_exit_code != expected_exit_code:
        findings.append(
            "summary final status/exit mismatch: "
            f"status={final_status!r} expected_exit={expected_exit_code} observed_exit={final_exit_code!r}."
        )

    audit = summary.get("audit")
    if not isinstance(audit, dict):
        findings.append("summary.audit must be an object.")
    elif audit.get("extract_exit_code") != 0:
        findings.append(
            f"summary.audit.extract_exit_code drift: expected=0 observed={audit.get('extract_exit_code')!r}."
        )

    return findings


def validate_snapshot(
    snapshot: dict[str, Any],
    *,
    summary: dict[str, Any],
    contract_id: str,
    contract_version: str,
) -> list[str]:
    findings = check_key_order(snapshot, expected=SNAPSHOT_KEYS, label="snapshot")

    if snapshot.get("contract_id") != contract_id:
        findings.append(
            f"snapshot.contract_id drift: expected={contract_id!r} observed={snapshot.get('contract_id')!r}."
        )
    if snapshot.get("contract_version") != contract_version:
        findings.append(
            f"snapshot.contract_version drift: expected={contract_version!r} observed={snapshot.get('contract_version')!r}."
        )

    open_blockers = snapshot.get("open_blockers")
    if not isinstance(open_blockers, list):
        findings.append("snapshot.open_blockers must be a list.")
        open_blocker_count = None
    else:
        open_blocker_count = len(open_blockers)

    if snapshot.get("open_blocker_count") != open_blocker_count:
        findings.append(
            "snapshot.open_blocker_count drift: "
            f"expected={open_blocker_count!r} observed={snapshot.get('open_blocker_count')!r}."
        )

    inputs = summary.get("inputs")
    if isinstance(inputs, dict):
        if snapshot.get("generated_at_utc") != inputs.get("generated_at_utc"):
            findings.append(
                "snapshot.generated_at_utc drift: "
                f"expected={inputs.get('generated_at_utc')!r} observed={snapshot.get('generated_at_utc')!r}."
            )
        if snapshot.get("source") != inputs.get("source"):
            findings.append(
                f"snapshot.source drift: expected={inputs.get('source')!r} observed={snapshot.get('source')!r}."
            )

    audit = summary.get("audit")
    if isinstance(audit, dict) and audit.get("open_blocker_count") != snapshot.get("open_blocker_count"):
        findings.append(
            "summary.audit.open_blocker_count drift: "
            f"expected={snapshot.get('open_blocker_count')!r} observed={audit.get('open_blocker_count')!r}."
        )

    return findings


def build_output(
    *,
    expected_runner: str,
    contract_id: str,
    contract_version: str,
    summary_path: Path,
    snapshot_path: Path,
    extract_log_path: Path,
    findings: list[str],
) -> dict[str, Any]:
    exit_code = EXIT_OK if not findings else EXIT_CONTRACT_DRIFT
    return {
        "mode": CHECKER_MODE,
        "contract": {
            "expected_runner": expected_runner,
            "contract_id": contract_id,
            "contract_version": contract_version,
        },
        "artifacts": {
            "summary": display_path(summary_path),
            "snapshot": display_path(snapshot_path),
            "extract_log": display_path(extract_log_path),
        },
        "ok": not findings,
        "exit_code": exit_code,
        "finding_count": len(findings),
        "findings": findings,
    }


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary", required=True)
    parser.add_argument("--snapshot", required=True)
    parser.add_argument("--extract-log", required=True)
    parser.add_argument("--contract-id", required=True)
    parser.add_argument("--contract-version", required=True)
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    summary_path = Path(args.summary)
    snapshot_path = Path(args.snapshot)
    extract_log_path = Path(args.extract_log)
    expected_runner = f"{args.contract_id}/{args.contract_version}"

    findings: list[str] = []
    try:
        summary = load_json(summary_path, label="summary")
        snapshot = load_json(snapshot_path, label="snapshot")
    except ValueError as exc:
        findings.append(str(exc))
        output = build_output(
            expected_runner=expected_runner,
            contract_id=args.contract_id,
            contract_version=args.contract_version,
            summary_path=summary_path,
            snapshot_path=snapshot_path,
            extract_log_path=extract_log_path,
            findings=findings,
        )
        print(json.dumps(output, indent=2))
        print(
            f"open-blocker-audit-contract: contract drift detected ({len(findings)} finding(s)).",
            file=sys.stderr,
        )
        return EXIT_CONTRACT_DRIFT

    findings.extend(
        validate_summary(
            summary,
            expected_runner=expected_runner,
            contract_id=args.contract_id,
            contract_version=args.contract_version,
            summary_path=summary_path,
            snapshot_path=snapshot_path,
            extract_log_path=extract_log_path,
        )
    )
    findings.extend(
        validate_snapshot(
            snapshot,
            summary=summary,
            contract_id=args.contract_id,
            contract_version=args.contract_version,
        )
    )
    findings.extend(validate_extract_log(extract_log_path))

    output = build_output(
        expected_runner=expected_runner,
        contract_id=args.contract_id,
        contract_version=args.contract_version,
        summary_path=summary_path,
        snapshot_path=snapshot_path,
        extract_log_path=extract_log_path,
        findings=findings,
    )
    print(json.dumps(output, indent=2))
    if findings:
        print(
            f"open-blocker-audit-contract: contract drift detected ({len(findings)} finding(s)).",
            file=sys.stderr,
        )
        return EXIT_CONTRACT_DRIFT
    return EXIT_OK


if __name__ == "__main__":
    raise SystemExit(main())
