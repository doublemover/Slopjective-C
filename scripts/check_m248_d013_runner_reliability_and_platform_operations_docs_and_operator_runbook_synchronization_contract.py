#!/usr/bin/env python3
"""Fail-closed checker for M248-D013 runner/platform docs/runbook synchronization."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = (
    "m248-d013-runner-reliability-platform-operations-"
    "docs-operator-runbook-synchronization-contract-v1"
)

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m248_runner_reliability_and_platform_operations_docs_and_operator_runbook_synchronization_d013_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_d013_runner_reliability_and_platform_operations_docs_and_operator_runbook_synchronization_packet.md",
    "d012_expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m248_runner_reliability_and_platform_operations_cross_lane_integration_sync_d012_expectations.md",
    "d012_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_d012_runner_reliability_and_platform_operations_cross_lane_integration_sync_packet.md",
    "d012_checker": ROOT
    / "scripts"
    / "check_m248_d012_runner_reliability_and_platform_operations_cross_lane_integration_sync_contract.py",
    "d012_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m248_d012_runner_reliability_and_platform_operations_cross_lane_integration_sync_contract.py",
    "d013_readiness_runner": ROOT / "scripts" / "run_m248_d013_lane_d_readiness.py",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "expectations_doc": (
        (
            "M248-D013-DOC-01",
            "Contract ID: `objc3c-runner-reliability-platform-operations-docs-operator-runbook-synchronization/m248-d013-v1`",
        ),
        ("M248-D013-DOC-02", "- Dependencies: `M248-D012`"),
        ("M248-D013-DOC-03", "docs_runbook_sync_consistent"),
        ("M248-D013-DOC-04", "docs_runbook_sync_ready"),
        ("M248-D013-DOC-05", "docs_runbook_sync_key_ready"),
        (
            "M248-D013-DOC-06",
            "scripts/check_m248_d013_runner_reliability_and_platform_operations_docs_and_operator_runbook_synchronization_contract.py",
        ),
        (
            "M248-D013-DOC-07",
            "tests/tooling/test_check_m248_d013_runner_reliability_and_platform_operations_docs_and_operator_runbook_synchronization_contract.py",
        ),
        (
            "M248-D013-DOC-08",
            "python scripts/check_m248_d013_runner_reliability_and_platform_operations_docs_and_operator_runbook_synchronization_contract.py --emit-json",
        ),
        (
            "M248-D013-DOC-09",
            "tmp/reports/m248/M248-D013/runner_reliability_and_platform_operations_docs_and_operator_runbook_synchronization_contract_summary.json",
        ),
        ("M248-D013-DOC-10", "npm run check:objc3c:m248-d013-lane-d-readiness"),
        ("M248-D013-DOC-11", "Issue `#6848`"),
    ),
    "packet_doc": (
        ("M248-D013-PKT-01", "Packet: `M248-D013`"),
        ("M248-D013-PKT-02", "Dependencies: `M248-D012`"),
        (
            "M248-D013-PKT-03",
            "scripts/check_m248_d013_runner_reliability_and_platform_operations_docs_and_operator_runbook_synchronization_contract.py",
        ),
        (
            "M248-D013-PKT-04",
            "tests/tooling/test_check_m248_d013_runner_reliability_and_platform_operations_docs_and_operator_runbook_synchronization_contract.py",
        ),
        ("M248-D013-PKT-05", "check:objc3c:m248-d013-lane-d-readiness"),
        (
            "M248-D013-PKT-06",
            "python scripts/check_m248_d013_runner_reliability_and_platform_operations_docs_and_operator_runbook_synchronization_contract.py --emit-json",
        ),
        (
            "M248-D013-PKT-07",
            "tmp/reports/m248/M248-D013/runner_reliability_and_platform_operations_docs_and_operator_runbook_synchronization_contract_summary.json",
        ),
        (
            "M248-D013-PKT-08",
            "python scripts/check_m248_d012_runner_reliability_and_platform_operations_cross_lane_integration_sync_contract.py",
        ),
    ),
    "d012_expectations_doc": (
        (
            "M248-D013-DEP-01",
            "Contract ID: `objc3c-runner-reliability-platform-operations-cross-lane-integration-sync/m248-d012-v1`",
        ),
    ),
    "d012_packet_doc": (
        ("M248-D013-DEP-02", "Packet: `M248-D012`"),
        ("M248-D013-DEP-03", "Dependencies"),
        ("M248-D013-DEP-04", "`M248-D011`"),
    ),
    "d012_checker": (
        (
            "M248-D013-DEP-05",
            "m248-d012-runner-reliability-platform-operations-cross-lane-integration-sync-contract-v1",
        ),
    ),
    "d012_tooling_test": (
        (
            "M248-D013-DEP-06",
            "check_m248_d012_runner_reliability_and_platform_operations_cross_lane_integration_sync_contract",
        ),
    ),
    "d013_readiness_runner": (
        (
            "M248-D013-DEP-07",
            "scripts/check_m248_d012_runner_reliability_and_platform_operations_cross_lane_integration_sync_contract.py",
        ),
        (
            "M248-D013-DEP-08",
            "tests/tooling/test_check_m248_d012_runner_reliability_and_platform_operations_cross_lane_integration_sync_contract.py",
        ),
        (
            "M248-D013-DEP-09",
            "scripts/check_m248_d013_runner_reliability_and_platform_operations_docs_and_operator_runbook_synchronization_contract.py",
        ),
        (
            "M248-D013-DEP-10",
            "tests/tooling/test_check_m248_d013_runner_reliability_and_platform_operations_docs_and_operator_runbook_synchronization_contract.py",
        ),
    ),
    "architecture_doc": (
        (
            "M248-D013-ARC-01",
            "M248 lane-D D013 docs and operator runbook synchronization anchors runner/platform operations contract integration",
        ),
    ),
    "lowering_spec": (
        (
            "M248-D013-SPC-01",
            "runner/platform operations docs and operator runbook synchronization governance shall preserve",
        ),
    ),
    "metadata_spec": (
        (
            "M248-D013-META-01",
            "deterministic lane-D runner/platform operations docs/operator runbook synchronization metadata anchors for `M248-D013`",
        ),
    ),
    "package_json": (
        (
            "M248-D013-PKG-01",
            '"check:objc3c:m248-d013-runner-reliability-platform-operations-docs-operator-runbook-synchronization-contract"',
        ),
        (
            "M248-D013-PKG-02",
            '"test:tooling:m248-d013-runner-reliability-platform-operations-docs-operator-runbook-synchronization-contract"',
        ),
        ("M248-D013-PKG-03", '"check:objc3c:m248-d013-lane-d-readiness"'),
        (
            "M248-D013-PKG-04",
            "python scripts/run_m248_d013_lane_d_readiness.py",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "package_json": (
        (
            "M248-D013-FORB-01",
            '"check:objc3c:m248-d013-lane-d-readiness": "npm run check:objc3c:m248-d011-lane-d-readiness',
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
            "tmp/reports/m248/M248-D013/runner_reliability_and_platform_operations_docs_and_operator_runbook_synchronization_contract_summary.json"
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
            findings.append(Finding(artifact, f"M248-D013-MISSING-{artifact.upper()}", str(exc)))
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
