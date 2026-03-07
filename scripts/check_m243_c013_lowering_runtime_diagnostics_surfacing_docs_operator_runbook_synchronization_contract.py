#!/usr/bin/env python3
"""Fail-closed checker for M243-C013 lowering/runtime diagnostics surfacing docs and operator runbook synchronization."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = (
    "m243-c013-lowering-runtime-diagnostics-surfacing-"
    "docs-operator-runbook-synchronization-contract-v1"
)

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_lowering_runtime_diagnostics_surfacing_docs_operator_runbook_synchronization_c013_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_c013_lowering_runtime_diagnostics_surfacing_docs_operator_runbook_synchronization_packet.md",
    "c012_expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_lowering_runtime_diagnostics_surfacing_cross_lane_integration_sync_c012_expectations.md",
    "c012_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_c012_lowering_runtime_diagnostics_surfacing_cross_lane_integration_sync_packet.md",
    "c012_checker": ROOT
    / "scripts"
    / "check_m243_c012_lowering_runtime_diagnostics_surfacing_cross_lane_integration_sync_contract.py",
    "c012_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m243_c012_lowering_runtime_diagnostics_surfacing_cross_lane_integration_sync_contract.py",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "expectations_doc": (
        (
            "M243-C013-DOC-01",
            "Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-docs-operator-runbook-synchronization/m243-c013-v1`",
        ),
        ("M243-C013-DOC-02", "- Dependencies: `M243-C012`"),
        ("M243-C013-DOC-03", "docs_operator_runbook_synchronization_consistent"),
        ("M243-C013-DOC-04", "docs_operator_runbook_synchronization_ready"),
        ("M243-C013-DOC-05", "docs_operator_runbook_synchronization_key_ready"),
        (
            "M243-C013-DOC-06",
            "scripts/check_m243_c013_lowering_runtime_diagnostics_surfacing_docs_operator_runbook_synchronization_contract.py",
        ),
        (
            "M243-C013-DOC-07",
            "tests/tooling/test_check_m243_c013_lowering_runtime_diagnostics_surfacing_docs_operator_runbook_synchronization_contract.py",
        ),
        (
            "M243-C013-DOC-08",
            "python scripts/check_m243_c013_lowering_runtime_diagnostics_surfacing_docs_operator_runbook_synchronization_contract.py --emit-json",
        ),
        (
            "M243-C013-DOC-09",
            "tmp/reports/m243/M243-C013/lowering_runtime_diagnostics_surfacing_docs_operator_runbook_synchronization_contract_summary.json",
        ),
        ("M243-C013-DOC-10", "npm run check:objc3c:m243-c013-lane-c-readiness"),
    ),
    "packet_doc": (
        ("M243-C013-PKT-01", "Packet: `M243-C013`"),
        ("M243-C013-PKT-02", "Dependencies: `M243-C012`"),
        (
            "M243-C013-PKT-03",
            "scripts/check_m243_c013_lowering_runtime_diagnostics_surfacing_docs_operator_runbook_synchronization_contract.py",
        ),
        (
            "M243-C013-PKT-04",
            "tests/tooling/test_check_m243_c013_lowering_runtime_diagnostics_surfacing_docs_operator_runbook_synchronization_contract.py",
        ),
        ("M243-C013-PKT-05", "check:objc3c:m243-c013-lane-c-readiness"),
        (
            "M243-C013-PKT-06",
            "python scripts/check_m243_c013_lowering_runtime_diagnostics_surfacing_docs_operator_runbook_synchronization_contract.py --emit-json",
        ),
        (
            "M243-C013-PKT-07",
            "tmp/reports/m243/M243-C013/lowering_runtime_diagnostics_surfacing_docs_operator_runbook_synchronization_contract_summary.json",
        ),
        (
            "M243-C013-PKT-08",
            "python scripts/check_m243_c012_lowering_runtime_diagnostics_surfacing_cross_lane_integration_sync_contract.py",
        ),
    ),
    "c012_expectations_doc": (
        (
            "M243-C013-DEP-01",
            "Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-cross-lane-integration-sync/m243-c012-v1`",
        ),
    ),
    "c012_packet_doc": (
        ("M243-C013-DEP-02", "Packet: `M243-C012`"),
        ("M243-C013-DEP-03", "Dependencies: `M243-C011`"),
    ),
    "c012_checker": (
        (
            "M243-C013-DEP-04",
            "cross-lane-integration-sync-contract-v1",
        ),
    ),
    "c012_tooling_test": (
        (
            "M243-C013-DEP-05",
            "check_m243_c012_lowering_runtime_diagnostics_surfacing_cross_lane_integration_sync_contract",
        ),
    ),
    "architecture_doc": (
        (
            "M243-C013-ARC-01",
            "M243 lane-C C013 docs and operator runbook synchronization anchors lowering/runtime diagnostics surfacing",
        ),
    ),
    "lowering_spec": (
        (
            "M243-C013-SPC-01",
            "lowering/runtime diagnostics surfacing docs and operator runbook synchronization shall preserve",
        ),
        ("M243-C013-SPC-02", "lane-C dependency anchors (`M243-C012`)"),
    ),
    "metadata_spec": (
        (
            "M243-C013-META-01",
            "deterministic lane-C lowering/runtime diagnostics surfacing docs and operator runbook synchronization metadata anchors for `M243-C013` with explicit",
        ),
        ("M243-C013-META-02", "`M243-C012` dependency continuity"),
    ),
    "package_json": (
        (
            "M243-C013-PKG-01",
            '"check:objc3c:m243-c013-lowering-runtime-diagnostics-surfacing-docs-operator-runbook-synchronization-contract"',
        ),
        (
            "M243-C013-PKG-02",
            '"test:tooling:m243-c013-lowering-runtime-diagnostics-surfacing-docs-operator-runbook-synchronization-contract"',
        ),
        ("M243-C013-PKG-03", '"check:objc3c:m243-c013-lane-c-readiness"'),
        (
            "M243-C013-PKG-04",
            "npm run check:objc3c:m243-c012-lane-c-readiness && npm run check:objc3c:m243-c013-lowering-runtime-diagnostics-surfacing-docs-operator-runbook-synchronization-contract && npm run test:tooling:m243-c013-lowering-runtime-diagnostics-surfacing-docs-operator-runbook-synchronization-contract",
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
            "tmp/reports/m243/M243-C013/lowering_runtime_diagnostics_surfacing_docs_operator_runbook_synchronization_contract_summary.json"
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


