#!/usr/bin/env python3
"""Fail-closed prerequisite checker for the M228-E002 replay/performance closeout modular split contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-e002-replay-proof-performance-closeout-gate-modular-split-scaffolding-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m228_lane_e_replay_proof_and_performance_closeout_gate_modular_split_scaffolding_e002_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_e002_replay_proof_and_performance_closeout_gate_modular_split_scaffolding_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m228/M228-E002/replay_proof_and_performance_closeout_gate_modular_split_scaffolding_contract_summary.json"
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
        "M228-E002-E001-01",
        "M228-E001",
        Path("docs/contracts/m228_lane_e_replay_proof_and_performance_closeout_gate_e001_expectations.md"),
    ),
    AssetCheck(
        "M228-E002-E001-02",
        "M228-E001",
        Path("scripts/check_m228_e001_replay_proof_and_performance_closeout_gate_contract.py"),
    ),
    AssetCheck(
        "M228-E002-E001-03",
        "M228-E001",
        Path("tests/tooling/test_check_m228_e001_replay_proof_and_performance_closeout_gate_contract.py"),
    ),
    AssetCheck(
        "M228-E002-E001-04",
        "M228-E001",
        Path("spec/planning/compiler/m228/m228_e001_replay_proof_and_performance_closeout_gate_contract_freeze_packet.md"),
    ),
    AssetCheck(
        "M228-E002-A002-01",
        "M228-A002",
        Path("docs/contracts/m228_lowering_pipeline_decomposition_pass_graph_modular_split_a002_expectations.md"),
    ),
    AssetCheck(
        "M228-E002-A002-02",
        "M228-A002",
        Path("scripts/check_m228_a002_lowering_pipeline_decomposition_pass_graph_modular_split_contract.py"),
    ),
    AssetCheck(
        "M228-E002-A002-03",
        "M228-A002",
        Path("tests/tooling/test_check_m228_a002_lowering_pipeline_decomposition_pass_graph_modular_split_contract.py"),
    ),
    AssetCheck(
        "M228-E002-B002-01",
        "M228-B002",
        Path("docs/contracts/m228_ownership_aware_lowering_behavior_modular_split_scaffolding_b002_expectations.md"),
    ),
    AssetCheck(
        "M228-E002-B002-02",
        "M228-B002",
        Path("scripts/check_m228_b002_ownership_aware_lowering_behavior_modular_split_scaffolding_contract.py"),
    ),
    AssetCheck(
        "M228-E002-B002-03",
        "M228-B002",
        Path("tests/tooling/test_check_m228_b002_ownership_aware_lowering_behavior_modular_split_scaffolding_contract.py"),
    ),
    AssetCheck(
        "M228-E002-D002-01",
        "M228-D002",
        Path("docs/contracts/m228_object_emission_link_path_modular_split_scaffolding_d002_expectations.md"),
    ),
    AssetCheck(
        "M228-E002-D002-02",
        "M228-D002",
        Path("scripts/check_m228_d002_object_emission_link_path_modular_split_scaffolding_contract.py"),
    ),
    AssetCheck(
        "M228-E002-D002-03",
        "M228-D002",
        Path("tests/tooling/test_check_m228_d002_object_emission_link_path_modular_split_scaffolding_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M228-E002-DOC-EXP-02",
        "# M228 Lane E Replay-Proof and Performance Closeout Gate Modular Split and Scaffolding Expectations (E002)",
    ),
    SnippetCheck(
        "M228-E002-DOC-EXP-03",
        "Contract ID: `objc3c-lane-e-replay-proof-performance-closeout-gate-modular-split-scaffolding-contract/m228-e002-v1`",
    ),
    SnippetCheck("M228-E002-DOC-EXP-04", "`M228-E001`"),
    SnippetCheck("M228-E002-DOC-EXP-05", "`M228-A002`"),
    SnippetCheck("M228-E002-DOC-EXP-06", "`M228-B002`"),
    SnippetCheck("M228-E002-DOC-EXP-07", "`M228-C004`"),
    SnippetCheck("M228-E002-DOC-EXP-08", "`M228-D002`"),
    SnippetCheck("M228-E002-DOC-EXP-09", "pending GH seed"),
    SnippetCheck(
        "M228-E002-DOC-EXP-10",
        "including code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M228-E002-DOC-EXP-11",
        "`check:objc3c:m228-e002-replay-proof-performance-closeout-gate-modular-split-scaffolding-contract`",
    ),
    SnippetCheck(
        "M228-E002-DOC-EXP-12",
        "`check:objc3c:m228-e002-lane-e-readiness`",
    ),
    SnippetCheck(
        "M228-E002-DOC-EXP-13",
        "`tmp/reports/m228/M228-E002/replay_proof_and_performance_closeout_gate_modular_split_scaffolding_contract_summary.json`",
    ),
    SnippetCheck("M228-E002-DOC-EXP-14", "`test:objc3c:lowering-replay-proof`"),
    SnippetCheck("M228-E002-DOC-EXP-15", "`test:objc3c:perf-budget`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M228-E002-DOC-PKT-02",
        "# M228-E002 Replay-Proof and Performance Closeout Gate Modular Split and Scaffolding Packet",
    ),
    SnippetCheck("M228-E002-DOC-PKT-03", "Packet: `M228-E002`"),
    SnippetCheck("M228-E002-DOC-PKT-04", "Freeze date: `2026-03-02`"),
    SnippetCheck(
        "M228-E002-DOC-PKT-05",
        "Dependencies: `M228-E001`, `M228-A002`, `M228-B002`, `M228-C004`, `M228-D002`",
    ),
    SnippetCheck(
        "M228-E002-DOC-PKT-06",
        "including code/spec anchors and milestone optimization",
    ),
    SnippetCheck(
        "M228-E002-DOC-PKT-07",
        "`scripts/check_m228_e002_replay_proof_and_performance_closeout_gate_modular_split_scaffolding_contract.py`",
    ),
    SnippetCheck(
        "M228-E002-DOC-PKT-08",
        "`tests/tooling/test_check_m228_e002_replay_proof_and_performance_closeout_gate_modular_split_scaffolding_contract.py`",
    ),
    SnippetCheck(
        "M228-E002-DOC-PKT-09",
        "`tmp/reports/m228/M228-E002/replay_proof_and_performance_closeout_gate_modular_split_scaffolding_contract_summary.json`",
    ),
    SnippetCheck("M228-E002-DOC-PKT-10", "`test:objc3c:lowering-replay-proof`"),
    SnippetCheck("M228-E002-DOC-PKT-11", "`test:objc3c:perf-budget`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M228-E002-ARCH-01",
        "M228 lane-E E002 replay-proof/performance closeout modular split/scaffolding anchors dependency references",
    ),
    SnippetCheck(
        "M228-E002-ARCH-02",
        "`M228-E001`, `M228-A002`, `M228-B002`",
    ),
    SnippetCheck(
        "M228-E002-ARCH-03",
        "lane-C C004 modular split/scaffolding",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M228-E002-SPC-01",
        "replay-proof/performance modular split and scaffolding closeout gate wiring",
    ),
    SnippetCheck(
        "M228-E002-SPC-02",
        "`M228-E001`, `M228-A002`",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M228-E002-META-01",
        "deterministic lane-E modular split/scaffolding closeout dependency anchors for `M228-E001`, `M228-A002`,",
    ),
    SnippetCheck(
        "M228-E002-META-02",
        "`M228-B002`, `M228-C004`, and `M228-D002`, including pending-lane tokens",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M228-E002-PKG-02",
        '"check:objc3c:m228-e002-replay-proof-performance-closeout-gate-modular-split-scaffolding-contract": '
        '"python scripts/check_m228_e002_replay_proof_and_performance_closeout_gate_modular_split_scaffolding_contract.py"',
    ),
    SnippetCheck(
        "M228-E002-PKG-03",
        '"test:tooling:m228-e002-replay-proof-performance-closeout-gate-modular-split-scaffolding-contract": '
        '"python -m pytest tests/tooling/test_check_m228_e002_replay_proof_and_performance_closeout_gate_modular_split_scaffolding_contract.py -q"',
    ),
    SnippetCheck(
        "M228-E002-PKG-04",
        '"check:objc3c:m228-e002-lane-e-readiness": '
        '"npm run check:objc3c:m228-e001-lane-e-readiness '
        '&& npm run check:objc3c:m228-a002-lane-a-readiness '
        '&& npm run check:objc3c:m228-b002-lane-b-readiness '
        '&& npm run check:objc3c:m228-d002-lane-d-readiness '
        '&& npm run check:objc3c:m228-e002-replay-proof-performance-closeout-gate-modular-split-scaffolding-contract '
        '&& npm run test:tooling:m228-e002-replay-proof-performance-closeout-gate-modular-split-scaffolding-contract"',
    ),
    SnippetCheck("M228-E002-PKG-05", '"test:objc3c:lowering-replay-proof": '),
    SnippetCheck("M228-E002-PKG-06", '"test:objc3c:perf-budget": '),
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
        exists_check_id="M228-E002-DOC-EXP-01",
        snippets=EXPECTATIONS_SNIPPETS,
    )
    checks_total += expectations_checks
    findings.extend(expectations_findings)

    packet_checks, packet_findings = check_doc_contract(
        artifact_name="packet_doc",
        path=args.packet_doc,
        exists_check_id="M228-E002-DOC-PKT-01",
        snippets=PACKET_SNIPPETS,
    )
    checks_total += packet_checks
    findings.extend(packet_findings)

    architecture_checks, architecture_findings = check_doc_contract(
        artifact_name="architecture_doc",
        path=args.architecture_doc,
        exists_check_id="M228-E002-ARCH-00",
        snippets=ARCHITECTURE_SNIPPETS,
    )
    checks_total += architecture_checks
    findings.extend(architecture_findings)

    lowering_checks, lowering_findings = check_doc_contract(
        artifact_name="lowering_spec",
        path=args.lowering_spec,
        exists_check_id="M228-E002-SPC-00",
        snippets=LOWERING_SPEC_SNIPPETS,
    )
    checks_total += lowering_checks
    findings.extend(lowering_findings)

    metadata_checks, metadata_findings = check_doc_contract(
        artifact_name="metadata_spec",
        path=args.metadata_spec,
        exists_check_id="M228-E002-META-00",
        snippets=METADATA_SPEC_SNIPPETS,
    )
    checks_total += metadata_checks
    findings.extend(metadata_findings)

    package_checks, package_findings = check_doc_contract(
        artifact_name="package_json",
        path=args.package_json,
        exists_check_id="M228-E002-PKG-01",
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
            "m228-e002-replay-proof-performance-closeout-gate-modular-split-scaffolding-contract: "
            f"error: unable to write summary: {exc}",
            file=sys.stderr,
        )
        return 2

    if not findings:
        print("m228-e002-replay-proof-performance-closeout-gate-modular-split-scaffolding-contract: OK")
        return 0

    print(
        "m228-e002-replay-proof-performance-closeout-gate-modular-split-scaffolding-contract: "
        f"contract drift detected ({len(findings)} failed check(s)).",
        file=sys.stderr,
    )
    for finding in findings:
        print(f"- [{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
