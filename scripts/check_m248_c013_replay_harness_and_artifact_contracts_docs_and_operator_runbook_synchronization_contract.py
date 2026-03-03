#!/usr/bin/env python3
"""Fail-closed checker for M248-C013 replay harness/artifact docs/runbook synchronization contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m248-c013-replay-harness-and-artifact-contracts-docs-operator-runbook-synchronization-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_c013_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_c013_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_packet.md"
)
DEFAULT_C012_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_replay_harness_and_artifact_contracts_cross_lane_integration_sync_c012_expectations.md"
)
DEFAULT_C012_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_c012_replay_harness_and_artifact_contracts_cross_lane_integration_sync_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m248/M248-C013/"
    "replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_contract_summary.json"
)


@dataclass(frozen=True)
class AssetCheck:
    check_id: str
    lane_task: str
    relative_path: Path


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


PREREQUISITE_ASSETS: tuple[AssetCheck, ...] = (
    AssetCheck(
        "M248-C013-C012-01",
        "M248-C012",
        Path(
            "docs/contracts/m248_replay_harness_and_artifact_contracts_cross_lane_integration_sync_c012_expectations.md"
        ),
    ),
    AssetCheck(
        "M248-C013-C012-02",
        "M248-C012",
        Path(
            "scripts/check_m248_c012_replay_harness_and_artifact_contracts_cross_lane_integration_sync_contract.py"
        ),
    ),
    AssetCheck(
        "M248-C013-C012-03",
        "M248-C012",
        Path(
            "tests/tooling/test_check_m248_c012_replay_harness_and_artifact_contracts_cross_lane_integration_sync_contract.py"
        ),
    ),
    AssetCheck(
        "M248-C013-C012-04",
        "M248-C012",
        Path(
            "spec/planning/compiler/m248/m248_c012_replay_harness_and_artifact_contracts_cross_lane_integration_sync_packet.md"
        ),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-C013-DOC-EXP-01",
        "Contract ID: `objc3c-replay-harness-artifact-contracts-docs-operator-runbook-synchronization/m248-c013-v1`",
    ),
    SnippetCheck("M248-C013-DOC-EXP-02", "Dependencies: `M248-C012`"),
    SnippetCheck(
        "M248-C013-DOC-EXP-03",
        "Scope: lane-C replay harness/artifact docs and operator runbook synchronization governance with fail-closed continuity from C012.",
    ),
    SnippetCheck(
        "M248-C013-DOC-EXP-04",
        "Issue `#6829` defines canonical lane-C docs and operator runbook synchronization scope.",
    ),
    SnippetCheck("M248-C013-DOC-EXP-05", "## Deterministic Invariants"),
    SnippetCheck("M248-C013-DOC-EXP-06", "docs_runbook_sync_consistent"),
    SnippetCheck("M248-C013-DOC-EXP-07", "docs_runbook_sync_ready"),
    SnippetCheck("M248-C013-DOC-EXP-08", "docs_runbook_sync_key_ready"),
    SnippetCheck("M248-C013-DOC-EXP-09", "docs_runbook_sync_key"),
    SnippetCheck("M248-C013-DOC-EXP-10", "`check:objc3c:m248-c012-lane-c-readiness`"),
    SnippetCheck("M248-C013-DOC-EXP-11", "`check:objc3c:m248-c013-lane-c-readiness`"),
    SnippetCheck(
        "M248-C013-DOC-EXP-12",
        "`scripts/check_m248_c013_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_contract.py`",
    ),
    SnippetCheck(
        "M248-C013-DOC-EXP-13",
        "`tests/tooling/test_check_m248_c013_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_contract.py`",
    ),
    SnippetCheck(
        "M248-C013-DOC-EXP-14",
        "`tmp/reports/m248/M248-C013/replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_contract_summary.json`",
    ),
    SnippetCheck(
        "M248-C013-DOC-EXP-15",
        "`spec/planning/compiler/m248/m248_c013_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_packet.md`",
    ),
    SnippetCheck(
        "M248-C013-DOC-EXP-16",
        "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
    ),
    SnippetCheck(
        "M248-C013-DOC-EXP-17",
        "## Milestone Optimization Inputs (Mandatory Scope Inputs)",
    ),
    SnippetCheck("M248-C013-DOC-EXP-18", "`test:objc3c:perf-budget`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-C013-DOC-PKT-01", "Packet: `M248-C013`"),
    SnippetCheck("M248-C013-DOC-PKT-02", "Issue: `#6829`"),
    SnippetCheck("M248-C013-DOC-PKT-03", "Dependencies: `M248-C012`"),
    SnippetCheck("M248-C013-DOC-PKT-04", "## Dependency Anchors (M248-C012)"),
    SnippetCheck(
        "M248-C013-DOC-PKT-05",
        "`scripts/check_m248_c013_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_contract.py`",
    ),
    SnippetCheck(
        "M248-C013-DOC-PKT-06",
        "`tests/tooling/test_check_m248_c013_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_contract.py`",
    ),
    SnippetCheck(
        "M248-C013-DOC-PKT-07",
        "`check:objc3c:m248-c013-replay-harness-artifact-contracts-docs-operator-runbook-synchronization-contract`",
    ),
    SnippetCheck(
        "M248-C013-DOC-PKT-08",
        "`test:tooling:m248-c013-replay-harness-artifact-contracts-docs-operator-runbook-synchronization-contract`",
    ),
    SnippetCheck("M248-C013-DOC-PKT-09", "`check:objc3c:m248-c012-lane-c-readiness`"),
    SnippetCheck("M248-C013-DOC-PKT-10", "`check:objc3c:m248-c013-lane-c-readiness`"),
    SnippetCheck(
        "M248-C013-DOC-PKT-11",
        "`tmp/reports/m248/M248-C013/replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_contract_summary.json`",
    ),
    SnippetCheck(
        "M248-C013-DOC-PKT-12",
        "python scripts/check_m248_c013_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_contract.py --emit-json",
    ),
)

C012_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-C013-C012-DOC-01",
        "Contract ID: `objc3c-replay-harness-artifact-contracts-cross-lane-integration-sync/m248-c012-v1`",
    ),
    SnippetCheck("M248-C013-C012-DOC-02", "Dependencies: `M248-C011`"),
)

C012_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-C013-C012-PKT-01", "Packet: `M248-C012`"),
    SnippetCheck("M248-C013-C012-PKT-02", "Issue: `#6828`"),
    SnippetCheck("M248-C013-C012-PKT-03", "Dependencies: `M248-C011`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-C013-ARCH-01",
        "M248 lane-C C001 replay harness and artifact contract anchors",
    ),
    SnippetCheck(
        "M248-C013-ARCH-02",
        "M248 lane-C C002 replay harness and artifact modular split/scaffolding",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-C013-SPC-01",
        "conformance matrix implementation shall include deterministic conformance",
    ),
    SnippetCheck(
        "M248-C013-SPC-02",
        "consistency and conformance-matrix readiness/key gates that fail closed",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-C013-META-01",
        "deterministic lane-C replay metadata anchors for `M248-C001`",
    ),
    SnippetCheck(
        "M248-C013-META-02",
        "contract evidence and execution replay continuity so CI replay drift fails",
    ),
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--c012-expectations-doc", type=Path, default=DEFAULT_C012_EXPECTATIONS_DOC)
    parser.add_argument("--c012-packet-doc", type=Path, default=DEFAULT_C012_PACKET_DOC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit canonical summary JSON to stdout.",
    )
    return parser.parse_args(argv)


def check_prerequisite_assets() -> tuple[int, list[Finding]]:
    checks_total = 0
    findings: list[Finding] = []
    for asset in PREREQUISITE_ASSETS:
        checks_total += 1
        absolute = ROOT / asset.relative_path
        if not absolute.exists() or not absolute.is_file():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"{asset.lane_task} prerequisite missing: {asset.relative_path.as_posix()}",
                )
            )
    return checks_total, findings


def check_doc_contract(
    *,
    artifact_name: str,
    path: Path,
    exists_check_id: str,
    snippets: tuple[SnippetCheck, ...],
) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists() or not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required document is missing: {display_path(path)}",
            )
        )
        return checks_total, findings

    try:
        text = path.read_text(encoding="utf-8")
    except OSError as exc:
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"unable to read required document: {exc}",
            )
        )
        return checks_total, findings

    for snippet_check in snippets:
        checks_total += 1
        if snippet_check.snippet not in text:
            findings.append(
                Finding(
                    artifact=artifact_name,
                    check_id=snippet_check.check_id,
                    detail=f"expected snippet missing: {snippet_check.snippet}",
                )
            )
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    checks_total, findings = check_prerequisite_assets()

    expectations_checks, expectations_findings = check_doc_contract(
        artifact_name="expectations_doc",
        path=args.expectations_doc,
        exists_check_id="M248-C013-DOC-EXP-00",
        snippets=EXPECTATIONS_SNIPPETS,
    )
    checks_total += expectations_checks
    findings.extend(expectations_findings)

    packet_checks, packet_findings = check_doc_contract(
        artifact_name="packet_doc",
        path=args.packet_doc,
        exists_check_id="M248-C013-DOC-PKT-00",
        snippets=PACKET_SNIPPETS,
    )
    checks_total += packet_checks
    findings.extend(packet_findings)

    c012_expectations_checks, c012_expectations_findings = check_doc_contract(
        artifact_name="c012_expectations_doc",
        path=args.c012_expectations_doc,
        exists_check_id="M248-C013-C012-DOC-00",
        snippets=C012_EXPECTATIONS_SNIPPETS,
    )
    checks_total += c012_expectations_checks
    findings.extend(c012_expectations_findings)

    c012_packet_checks, c012_packet_findings = check_doc_contract(
        artifact_name="c012_packet_doc",
        path=args.c012_packet_doc,
        exists_check_id="M248-C013-C012-PKT-00",
        snippets=C012_PACKET_SNIPPETS,
    )
    checks_total += c012_packet_checks
    findings.extend(c012_packet_findings)

    architecture_checks, architecture_findings = check_doc_contract(
        artifact_name="architecture_doc",
        path=args.architecture_doc,
        exists_check_id="M248-C013-ARCH-00",
        snippets=ARCHITECTURE_SNIPPETS,
    )
    checks_total += architecture_checks
    findings.extend(architecture_findings)

    lowering_checks, lowering_findings = check_doc_contract(
        artifact_name="lowering_spec",
        path=args.lowering_spec,
        exists_check_id="M248-C013-SPC-00",
        snippets=LOWERING_SPEC_SNIPPETS,
    )
    checks_total += lowering_checks
    findings.extend(lowering_findings)

    metadata_checks, metadata_findings = check_doc_contract(
        artifact_name="metadata_spec",
        path=args.metadata_spec,
        exists_check_id="M248-C013-META-00",
        snippets=METADATA_SPEC_SNIPPETS,
    )
    checks_total += metadata_checks
    findings.extend(metadata_findings)

    findings = sorted(findings, key=lambda finding: (finding.check_id, finding.artifact, finding.detail))
    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(findings),
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in findings
        ],
    }

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary), encoding="utf-8")

    if args.emit_json:
        print(canonical_json(summary), end="")

    if not findings:
        if not args.emit_json:
            print(f"[ok] {MODE}: {summary['checks_passed']}/{summary['checks_total']} checks passed")
        return 0

    if not args.emit_json:
        print(f"{MODE}: contract drift detected ({len(findings)} failed check(s)).", file=sys.stderr)
        for finding in findings:
            print(f"- [{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
