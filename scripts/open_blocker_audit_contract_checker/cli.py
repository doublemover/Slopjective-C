from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

from .constants import EXIT_CONTRACT_DRIFT, EXIT_OK
from .loading import load_json
from .log_validation import validate_extract_log
from .output import build_output
from .snapshot_validation import validate_snapshot
from .summary_validation import validate_summary


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Validate open-blocker audit runner artifacts against the checker contract."
        )
    )
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
            "open-blocker-audit-contract: contract drift detected "
            f"({len(findings)} finding(s)).",
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
            "open-blocker-audit-contract: contract drift detected "
            f"({len(findings)} finding(s)).",
            file=sys.stderr,
        )
        return EXIT_CONTRACT_DRIFT
    return EXIT_OK
