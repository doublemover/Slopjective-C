#!/usr/bin/env python3
"""Fail-closed checker for M243-D022 CLI/reporting output docs/runbook synchronization."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = (
    "m243-d022-cli-reporting-output-contract-integration-"
    "integration-closeout-gate-sign-off-contract-v1"
)

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_cli_reporting_and_output_contract_integration_integration_closeout_gate_sign_off_d022_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_d022_cli_reporting_and_output_contract_integration_integration_closeout_gate_sign_off_packet.md",
    "d021_expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_cli_reporting_and_output_contract_integration_advanced_core_workpack_shard_2_d021_expectations.md",
    "d021_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_d021_cli_reporting_and_output_contract_integration_advanced_core_workpack_shard_2_packet.md",
    "d021_checker": ROOT
    / "scripts"
    / "check_m243_d021_cli_reporting_and_output_contract_integration_advanced_core_workpack_shard_2_contract.py",
    "d021_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m243_d021_cli_reporting_and_output_contract_integration_advanced_core_workpack_shard_2_contract.py",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "expectations_doc": (
        (
            "M243-D022-DOC-01",
            "Contract ID: `objc3c-cli-reporting-output-contract-integration-integration-closeout-gate-sign-off/m243-d022-v1`",
        ),
        ("M243-D022-DOC-02", "- Dependencies: `M243-D021`"),
        ("M243-D022-DOC-03", "docs_runbook_sync_consistent"),
        ("M243-D022-DOC-04", "docs_runbook_sync_ready"),
        ("M243-D022-DOC-05", "docs_runbook_sync_key_ready"),
        (
            "M243-D022-DOC-06",
            "scripts/check_m243_d022_cli_reporting_and_output_contract_integration_integration_closeout_gate_sign_off_contract.py",
        ),
        (
            "M243-D022-DOC-07",
            "tests/tooling/test_check_m243_d022_cli_reporting_and_output_contract_integration_integration_closeout_gate_sign_off_contract.py",
        ),
        (
            "M243-D022-DOC-08",
            "python scripts/check_m243_d022_cli_reporting_and_output_contract_integration_integration_closeout_gate_sign_off_contract.py --emit-json",
        ),
        (
            "M243-D022-DOC-09",
            "tmp/reports/m243/M243-D022/cli_reporting_and_output_contract_integration_integration_closeout_gate_sign_off_contract_summary.json",
        ),
        ("M243-D022-DOC-10", "npm run check:objc3c:m243-d022-lane-d-readiness"),
        ("M243-D022-DOC-11", "Issue `#6486`"),
    ),
    "packet_doc": (
        ("M243-D022-PKT-01", "Packet: `M243-D022`"),
        ("M243-D022-PKT-02", "Dependencies: `M243-D021`"),
        (
            "M243-D022-PKT-03",
            "scripts/check_m243_d022_cli_reporting_and_output_contract_integration_integration_closeout_gate_sign_off_contract.py",
        ),
        (
            "M243-D022-PKT-04",
            "tests/tooling/test_check_m243_d022_cli_reporting_and_output_contract_integration_integration_closeout_gate_sign_off_contract.py",
        ),
        ("M243-D022-PKT-05", "check:objc3c:m243-d022-lane-d-readiness"),
        (
            "M243-D022-PKT-06",
            "python scripts/check_m243_d022_cli_reporting_and_output_contract_integration_integration_closeout_gate_sign_off_contract.py --emit-json",
        ),
        (
            "M243-D022-PKT-07",
            "tmp/reports/m243/M243-D022/cli_reporting_and_output_contract_integration_integration_closeout_gate_sign_off_contract_summary.json",
        ),
        (
            "M243-D022-PKT-08",
            "python scripts/check_m243_d021_cli_reporting_and_output_contract_integration_advanced_core_workpack_shard_2_contract.py",
        ),
    ),
    "d021_expectations_doc": (
        (
            "M243-D022-DEP-01",
            "Contract ID: `objc3c-cli-reporting-output-contract-integration-advanced-core-workpack-shard-2/m243-d021-v1`",
        ),
    ),
    "d021_packet_doc": (
        ("M243-D022-DEP-02", "Packet: `M243-D021`"),
        ("M243-D022-DEP-03", "Dependencies: `M243-D020`"),
    ),
    "d021_checker": (
        (
            "M243-D022-DEP-04",
            "advanced-core-workpack-shard-2-contract-v1",
        ),
    ),
    "d021_tooling_test": (
        (
            "M243-D022-DEP-05",
            "check_m243_d021_cli_reporting_and_output_contract_integration_advanced_core_workpack_shard_2_contract",
        ),
    ),
    "architecture_doc": (
        (
            "M243-D022-ARC-01",
            "M243 lane-D D022 integration closeout and gate sign-off anchors CLI/reporting output contract integration",
        ),
    ),
    "lowering_spec": (
        (
            "M243-D022-SPC-01",
            "CLI/reporting and output integration closeout and gate sign-off governance shall preserve",
        ),
    ),
    "metadata_spec": (
        (
            "M243-D022-META-01",
            "deterministic lane-D CLI/reporting output docs/operator runbook synchronization metadata anchors for `M243-D022`",
        ),
    ),
    "package_json": (
        (
            "M243-D022-PKG-01",
            '"check:objc3c:m243-d022-cli-reporting-output-contract-integration-integration-closeout-gate-sign-off-contract"',
        ),
        (
            "M243-D022-PKG-02",
            '"test:tooling:m243-d022-cli-reporting-output-contract-integration-integration-closeout-gate-sign-off-contract"',
        ),
        ("M243-D022-PKG-03", '"check:objc3c:m243-d022-lane-d-readiness"'),
        (
            "M243-D022-PKG-04",
            "npm run check:objc3c:m243-d021-lane-d-readiness && npm run check:objc3c:m243-d022-cli-reporting-output-contract-integration-integration-closeout-gate-sign-off-contract && npm run test:tooling:m243-d022-cli-reporting-output-contract-integration-integration-closeout-gate-sign-off-contract",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "package_json": (
        (
            "M243-D022-FORB-01",
            '"check:objc3c:m243-d022-lane-d-readiness": "npm run check:objc3c:m243-d020-lane-d-readiness',
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
            "tmp/reports/m243/M243-D022/cli_reporting_and_output_contract_integration_integration_closeout_gate_sign_off_contract_summary.json"
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
        raise FileNotFoundError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    for artifact, path in ARTIFACTS.items():
        total_checks += 1
        try:
            text = load_text(path, artifact=artifact)
            passed_checks += 1
        except FileNotFoundError as exc:
            findings.append(Finding(artifact, f"M243-D022-MISSING-{artifact.upper()}", str(exc)))
            continue

        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                passed_checks += 1
            else:
                findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))

        for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                findings.append(Finding(artifact, check_id, f"forbidden snippet present: {snippet}"))
            else:
                passed_checks += 1

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









