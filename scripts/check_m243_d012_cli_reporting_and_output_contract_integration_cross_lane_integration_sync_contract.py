#!/usr/bin/env python3
"""Fail-closed checker for M243-D012 CLI/reporting output cross-lane integration sync."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = (
    "m243-d012-cli-reporting-output-contract-integration-"
    "cross-lane-integration-sync-contract-v1"
)

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_cli_reporting_and_output_contract_integration_cross_lane_integration_sync_d012_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_d012_cli_reporting_and_output_contract_integration_cross_lane_integration_sync_packet.md",
    "d011_expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_cli_reporting_and_output_contract_integration_performance_quality_guardrails_d011_expectations.md",
    "d011_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_d011_cli_reporting_and_output_contract_integration_performance_quality_guardrails_packet.md",
    "d011_checker": ROOT
    / "scripts"
    / "check_m243_d011_cli_reporting_and_output_contract_integration_performance_quality_guardrails_contract.py",
    "d011_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m243_d011_cli_reporting_and_output_contract_integration_performance_quality_guardrails_contract.py",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "expectations_doc": (
        (
            "M243-D012-DOC-01",
            "Contract ID: `objc3c-cli-reporting-output-contract-integration-cross-lane-integration-sync/m243-d012-v1`",
        ),
        ("M243-D012-DOC-02", "- Dependencies: `M243-D011`"),
        ("M243-D012-DOC-03", "cross_lane_integration_sync_consistent"),
        ("M243-D012-DOC-04", "cross_lane_integration_sync_ready"),
        ("M243-D012-DOC-05", "cross_lane_integration_sync_key_ready"),
        (
            "M243-D012-DOC-06",
            "scripts/check_m243_d012_cli_reporting_and_output_contract_integration_cross_lane_integration_sync_contract.py",
        ),
        (
            "M243-D012-DOC-07",
            "tests/tooling/test_check_m243_d012_cli_reporting_and_output_contract_integration_cross_lane_integration_sync_contract.py",
        ),
        (
            "M243-D012-DOC-08",
            "python scripts/check_m243_d012_cli_reporting_and_output_contract_integration_cross_lane_integration_sync_contract.py --emit-json",
        ),
        (
            "M243-D012-DOC-09",
            "tmp/reports/m243/M243-D012/cli_reporting_and_output_contract_integration_cross_lane_integration_sync_contract_summary.json",
        ),
        ("M243-D012-DOC-10", "npm run check:objc3c:m243-d012-lane-d-readiness"),
    ),
    "packet_doc": (
        ("M243-D012-PKT-01", "Packet: `M243-D012`"),
        ("M243-D012-PKT-02", "Dependencies: `M243-D011`"),
        (
            "M243-D012-PKT-03",
            "scripts/check_m243_d012_cli_reporting_and_output_contract_integration_cross_lane_integration_sync_contract.py",
        ),
        (
            "M243-D012-PKT-04",
            "tests/tooling/test_check_m243_d012_cli_reporting_and_output_contract_integration_cross_lane_integration_sync_contract.py",
        ),
        ("M243-D012-PKT-05", "check:objc3c:m243-d012-lane-d-readiness"),
        (
            "M243-D012-PKT-06",
            "python scripts/check_m243_d012_cli_reporting_and_output_contract_integration_cross_lane_integration_sync_contract.py --emit-json",
        ),
        (
            "M243-D012-PKT-07",
            "tmp/reports/m243/M243-D012/cli_reporting_and_output_contract_integration_cross_lane_integration_sync_contract_summary.json",
        ),
        (
            "M243-D012-PKT-08",
            "python scripts/check_m243_d011_cli_reporting_and_output_contract_integration_performance_quality_guardrails_contract.py",
        ),
    ),
    "d011_expectations_doc": (
        (
            "M243-D012-DEP-01",
            "Contract ID: `objc3c-cli-reporting-output-contract-integration-performance-quality-guardrails/m243-d011-v1`",
        ),
    ),
    "d011_packet_doc": (
        ("M243-D012-DEP-02", "Packet: `M243-D011`"),
        ("M243-D012-DEP-03", "Dependencies: `M243-D010`"),
    ),
    "d011_checker": (
        (
            "M243-D012-DEP-04",
            "performance-quality-guardrails-contract-v1",
        ),
    ),
    "d011_tooling_test": (
        (
            "M243-D012-DEP-05",
            "check_m243_d011_cli_reporting_and_output_contract_integration_performance_quality_guardrails_contract",
        ),
    ),
    "architecture_doc": (
        (
            "M243-D012-ARC-01",
            "M243 lane-D D012 cross-lane integration sync anchors CLI/reporting output contract integration",
        ),
    ),
    "lowering_spec": (
        (
            "M243-D012-SPC-01",
            "CLI/reporting and output cross-lane integration sync governance shall preserve",
        ),
    ),
    "metadata_spec": (
        (
            "M243-D012-META-01",
            "deterministic lane-D CLI/reporting output cross-lane integration sync metadata anchors for `M243-D012`",
        ),
    ),
    "package_json": (
        (
            "M243-D012-PKG-01",
            '"check:objc3c:m243-d012-cli-reporting-output-contract-integration-cross-lane-integration-sync-contract"',
        ),
        (
            "M243-D012-PKG-02",
            '"test:tooling:m243-d012-cli-reporting-output-contract-integration-cross-lane-integration-sync-contract"',
        ),
        ("M243-D012-PKG-03", '"check:objc3c:m243-d012-lane-d-readiness"'),
        (
            "M243-D012-PKG-04",
            "npm run check:objc3c:m243-d011-lane-d-readiness && npm run check:objc3c:m243-d012-cli-reporting-output-contract-integration-cross-lane-integration-sync-contract && npm run test:tooling:m243-d012-cli-reporting-output-contract-integration-cross-lane-integration-sync-contract",
        ),
    ),
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path(
            "tmp/reports/m243/M243-D012/cli_reporting_and_output_contract_integration_cross_lane_integration_sync_contract_summary.json"
        ),
    )
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit canonical summary JSON to stdout.",
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    for artifact, path in ARTIFACTS.items():
        text = load_text(path, artifact=artifact)
        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                passed_checks += 1
            else:
                findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))

    ok = not findings
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": total_checks,
        "checks_passed": passed_checks,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in findings
        ],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if args.emit_json:
        print(canonical_json(summary), end="")

    if ok:
        if not args.emit_json:
            print(f"[ok] {MODE}: {passed_checks}/{total_checks} checks passed")
        return 0

    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

