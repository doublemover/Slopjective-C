#!/usr/bin/env python3
"""Fail-closed prerequisite checker for the M228-E001 replay/performance closeout gate."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-e001-replay-proof-performance-closeout-gate-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m228_lane_e_replay_proof_and_performance_closeout_gate_e001_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_e001_replay_proof_and_performance_closeout_gate_contract_freeze_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m228/M228-E001/replay_proof_and_performance_closeout_gate_contract_summary.json"
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
        "M228-E001-A001-01",
        "M228-A001",
        Path("docs/contracts/m228_lowering_pipeline_decomposition_pass_graph_contract_freeze_a001_expectations.md"),
    ),
    AssetCheck(
        "M228-E001-A001-02",
        "M228-A001",
        Path("scripts/check_m228_a001_lowering_pipeline_decomposition_pass_graph_contract.py"),
    ),
    AssetCheck(
        "M228-E001-A001-03",
        "M228-A001",
        Path("tests/tooling/test_check_m228_a001_lowering_pipeline_decomposition_pass_graph_contract.py"),
    ),
    AssetCheck(
        "M228-E001-B001-01",
        "M228-B001",
        Path("docs/contracts/m228_ownership_aware_lowering_behavior_contract_freeze_b001_expectations.md"),
    ),
    AssetCheck(
        "M228-E001-B001-02",
        "M228-B001",
        Path("scripts/check_m228_b001_ownership_aware_lowering_behavior_contract.py"),
    ),
    AssetCheck(
        "M228-E001-B001-03",
        "M228-B001",
        Path("tests/tooling/test_check_m228_b001_ownership_aware_lowering_behavior_contract.py"),
    ),
    AssetCheck(
        "M228-E001-D001-01",
        "M228-D001",
        Path("docs/contracts/m228_object_emission_link_path_reliability_contract_freeze_d001_expectations.md"),
    ),
    AssetCheck(
        "M228-E001-D001-02",
        "M228-D001",
        Path("scripts/check_m228_d001_object_emission_link_path_reliability_contract.py"),
    ),
    AssetCheck(
        "M228-E001-D001-03",
        "M228-D001",
        Path("tests/tooling/test_check_m228_d001_object_emission_link_path_reliability_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M228-E001-DOC-EXP-02",
        "# M228 Lane E Replay-Proof and Performance Closeout Gate Expectations (E001)",
    ),
    SnippetCheck(
        "M228-E001-DOC-EXP-03",
        "Contract ID: `objc3c-lane-e-replay-proof-performance-closeout-gate-contract/m228-e001-v1`",
    ),
    SnippetCheck("M228-E001-DOC-EXP-04", "`M228-A001`"),
    SnippetCheck("M228-E001-DOC-EXP-05", "`M228-B001`"),
    SnippetCheck("M228-E001-DOC-EXP-06", "`M228-C002`"),
    SnippetCheck("M228-E001-DOC-EXP-07", "`M228-D001`"),
    SnippetCheck(
        "M228-E001-DOC-EXP-08",
        "pending seeded C002 contract assets",
    ),
    SnippetCheck(
        "M228-E001-DOC-EXP-09",
        "`check:objc3c:m228-e001-replay-proof-performance-closeout-gate-contract`",
    ),
    SnippetCheck(
        "M228-E001-DOC-EXP-10",
        "`check:objc3c:m228-e001-lane-e-readiness`",
    ),
    SnippetCheck(
        "M228-E001-DOC-EXP-11",
        "`tmp/reports/m228/M228-E001/replay_proof_and_performance_closeout_gate_contract_summary.json`",
    ),
    SnippetCheck(
        "M228-E001-DOC-EXP-12",
        "including code/spec anchors and milestone optimization improvements",
    ),
    SnippetCheck(
        "M228-E001-DOC-EXP-13",
        "`test:objc3c:lowering-replay-proof`",
    ),
    SnippetCheck(
        "M228-E001-DOC-EXP-14",
        "`test:objc3c:perf-budget`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M228-E001-DOC-PKT-02",
        "# M228-E001 Replay-Proof and Performance Closeout Gate Contract Freeze Packet",
    ),
    SnippetCheck("M228-E001-DOC-PKT-03", "Packet: `M228-E001`"),
    SnippetCheck("M228-E001-DOC-PKT-04", "Freeze date: `2026-03-02`"),
    SnippetCheck(
        "M228-E001-DOC-PKT-05",
        "Dependencies: `M228-A001`, `M228-B001`, `M228-C002`, `M228-D001`",
    ),
    SnippetCheck(
        "M228-E001-DOC-PKT-06",
        "`scripts/check_m228_e001_replay_proof_and_performance_closeout_gate_contract.py`",
    ),
    SnippetCheck(
        "M228-E001-DOC-PKT-07",
        "`tests/tooling/test_check_m228_e001_replay_proof_and_performance_closeout_gate_contract.py`",
    ),
    SnippetCheck(
        "M228-E001-DOC-PKT-08",
        "`tmp/reports/m228/M228-E001/replay_proof_and_performance_closeout_gate_contract_summary.json`",
    ),
    SnippetCheck(
        "M228-E001-DOC-PKT-09",
        "including code/spec anchors and milestone optimization improvements",
    ),
    SnippetCheck(
        "M228-E001-DOC-PKT-10",
        "`test:objc3c:lowering-replay-proof`",
    ),
    SnippetCheck(
        "M228-E001-DOC-PKT-11",
        "`test:objc3c:perf-budget`",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M228-E001-ARCH-01",
        "M228 lane-E E001 replay-proof/performance closeout gate anchors dependency",
    ),
    SnippetCheck(
        "M228-E001-ARCH-02",
        "`M228-A001`, `M228-B001`, `M228-C002`, `M228-D001`",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M228-E001-SPC-01",
        "replay-proof/performance closeout gate wiring shall preserve explicit lane-E",
    ),
    SnippetCheck(
        "M228-E001-SPC-02",
        "dependency anchors (`M228-A001`, `M228-B001`, `M228-C002`, `M228-D001`)",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M228-E001-META-01",
        "deterministic lane-E closeout dependency anchors for `M228-A001`, `M228-B001`,",
    ),
    SnippetCheck(
        "M228-E001-META-02",
        "`M228-C002`, and `M228-D001`, including pending-lane tokens needed",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M228-E001-PKG-02",
        '"check:objc3c:m228-e001-replay-proof-performance-closeout-gate-contract": '
        '"python scripts/check_m228_e001_replay_proof_and_performance_closeout_gate_contract.py"',
    ),
    SnippetCheck(
        "M228-E001-PKG-03",
        '"test:tooling:m228-e001-replay-proof-performance-closeout-gate-contract": '
        '"python -m pytest tests/tooling/test_check_m228_e001_replay_proof_and_performance_closeout_gate_contract.py -q"',
    ),
    SnippetCheck(
        "M228-E001-PKG-04",
        '"check:objc3c:m228-e001-lane-e-readiness": '
        '"npm run check:objc3c:m228-e001-replay-proof-performance-closeout-gate-contract '
        '&& npm run test:tooling:m228-e001-replay-proof-performance-closeout-gate-contract"',
    ),
    SnippetCheck(
        "M228-E001-PKG-05",
        '"test:objc3c:lowering-replay-proof": ',
    ),
    SnippetCheck(
        "M228-E001-PKG-06",
        '"test:objc3c:perf-budget": ',
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
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_prerequisite_assets() -> tuple[int, list[Finding]]:
    checks_total = 0
    findings: list[Finding] = []
    for asset in PREREQUISITE_ASSETS:
        checks_total += 1
        absolute = ROOT / asset.relative_path
        if not absolute.exists():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"{asset.lane_task} prerequisite missing: {asset.relative_path.as_posix()}",
                )
            )
            continue
        if not absolute.is_file():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"{asset.lane_task} prerequisite path is not a file: {asset.relative_path.as_posix()}",
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
    if not path.exists():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required document is missing: {display_path(path)}",
            )
        )
        return checks_total, findings
    if not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required document path is not a file: {display_path(path)}",
            )
        )
        return checks_total, findings

    try:
        text = path.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail="required document is not valid UTF-8",
            )
        )
        return checks_total, findings
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
        exists_check_id="M228-E001-DOC-EXP-01",
        snippets=EXPECTATIONS_SNIPPETS,
    )
    checks_total += expectations_checks
    findings.extend(expectations_findings)

    packet_checks, packet_findings = check_doc_contract(
        artifact_name="packet_doc",
        path=args.packet_doc,
        exists_check_id="M228-E001-DOC-PKT-01",
        snippets=PACKET_SNIPPETS,
    )
    checks_total += packet_checks
    findings.extend(packet_findings)

    architecture_checks, architecture_findings = check_doc_contract(
        artifact_name="architecture_doc",
        path=args.architecture_doc,
        exists_check_id="M228-E001-ARCH-00",
        snippets=ARCHITECTURE_SNIPPETS,
    )
    checks_total += architecture_checks
    findings.extend(architecture_findings)

    lowering_checks, lowering_findings = check_doc_contract(
        artifact_name="lowering_spec",
        path=args.lowering_spec,
        exists_check_id="M228-E001-SPC-00",
        snippets=LOWERING_SPEC_SNIPPETS,
    )
    checks_total += lowering_checks
    findings.extend(lowering_findings)

    metadata_checks, metadata_findings = check_doc_contract(
        artifact_name="metadata_spec",
        path=args.metadata_spec,
        exists_check_id="M228-E001-META-00",
        snippets=METADATA_SPEC_SNIPPETS,
    )
    checks_total += metadata_checks
    findings.extend(metadata_findings)

    package_checks, package_findings = check_doc_contract(
        artifact_name="package_json",
        path=args.package_json,
        exists_check_id="M228-E001-PKG-01",
        snippets=PACKAGE_SNIPPETS,
    )
    checks_total += package_checks
    findings.extend(package_findings)

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

    try:
        args.summary_out.parent.mkdir(parents=True, exist_ok=True)
        args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    except OSError as exc:
        print(
            "m228-e001-replay-proof-performance-closeout-gate-contract: "
            f"error: unable to write summary: {exc}",
            file=sys.stderr,
        )
        return 2

    if not findings:
        print("m228-e001-replay-proof-performance-closeout-gate-contract: OK")
        return 0

    print(
        "m228-e001-replay-proof-performance-closeout-gate-contract: contract drift detected "
        f"({len(findings)} failed check(s)).",
        file=sys.stderr,
    )
    for finding in findings:
        print(f"- [{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
