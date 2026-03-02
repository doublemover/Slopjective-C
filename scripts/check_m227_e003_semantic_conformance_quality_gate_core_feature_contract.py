#!/usr/bin/env python3
"""Fail-closed prerequisite checker for the M227-E003 lane-E core-feature quality gate."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-e003-semantic-conformance-lane-e-core-feature-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m227_lane_e_semantic_conformance_core_feature_implementation_e003_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_e003_semantic_conformance_lane_e_core_feature_packet.md"
)
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m227/m227_e003_semantic_conformance_lane_e_core_feature_contract_summary.json")


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
        "M227-E003-E002-01",
        "M227-E002",
        Path("docs/contracts/m227_lane_e_semantic_conformance_modular_split_e002_expectations.md"),
    ),
    AssetCheck(
        "M227-E003-E002-02",
        "M227-E002",
        Path("scripts/check_m227_e002_semantic_conformance_lane_e_modular_split_contract.py"),
    ),
    AssetCheck(
        "M227-E003-E002-03",
        "M227-E002",
        Path("tests/tooling/test_check_m227_e002_semantic_conformance_lane_e_modular_split_contract.py"),
    ),
    AssetCheck(
        "M227-E003-E002-04",
        "M227-E002",
        Path("spec/planning/compiler/m227/m227_e002_semantic_conformance_lane_e_modular_split_packet.md"),
    ),
    AssetCheck(
        "M227-E003-A008-01",
        "M227-A008",
        Path("docs/contracts/m227_semantic_pass_recovery_determinism_hardening_expectations.md"),
    ),
    AssetCheck(
        "M227-E003-A008-02",
        "M227-A008",
        Path("scripts/check_m227_a008_semantic_pass_recovery_determinism_hardening_contract.py"),
    ),
    AssetCheck(
        "M227-E003-A008-03",
        "M227-A008",
        Path("tests/tooling/test_check_m227_a008_semantic_pass_recovery_determinism_hardening_contract.py"),
    ),
    AssetCheck(
        "M227-E003-A008-04",
        "M227-A008",
        Path("spec/planning/compiler/m227/m227_a008_semantic_pass_recovery_determinism_hardening_packet.md"),
    ),
    AssetCheck(
        "M227-E003-B006-01",
        "M227-B006",
        Path("docs/contracts/m227_type_system_objc3_forms_edge_robustness_b006_expectations.md"),
    ),
    AssetCheck(
        "M227-E003-B006-02",
        "M227-B006",
        Path("scripts/check_m227_b006_type_system_objc3_forms_edge_robustness_contract.py"),
    ),
    AssetCheck(
        "M227-E003-B006-03",
        "M227-B006",
        Path("tests/tooling/test_check_m227_b006_type_system_objc3_forms_edge_robustness_contract.py"),
    ),
    AssetCheck(
        "M227-E003-B006-04",
        "M227-B006",
        Path("spec/planning/compiler/m227/m227_b006_type_system_objc3_forms_edge_robustness_packet.md"),
    ),
    AssetCheck(
        "M227-E003-C004-01",
        "M227-C004",
        Path("docs/contracts/m227_typed_sema_to_lowering_core_feature_expansion_c004_expectations.md"),
    ),
    AssetCheck(
        "M227-E003-C004-02",
        "M227-C004",
        Path("scripts/check_m227_c004_typed_sema_to_lowering_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M227-E003-C004-03",
        "M227-C004",
        Path("tests/tooling/test_check_m227_c004_typed_sema_to_lowering_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M227-E003-C004-04",
        "M227-C004",
        Path("spec/planning/compiler/m227/m227_c004_typed_sema_to_lowering_core_feature_expansion_packet.md"),
    ),
    AssetCheck(
        "M227-E003-D003-01",
        "M227-D003",
        Path("docs/contracts/m227_runtime_facing_type_metadata_core_feature_d003_expectations.md"),
    ),
    AssetCheck(
        "M227-E003-D003-02",
        "M227-D003",
        Path("scripts/check_m227_d003_runtime_facing_type_metadata_core_feature_contract.py"),
    ),
    AssetCheck(
        "M227-E003-D003-03",
        "M227-D003",
        Path("tests/tooling/test_check_m227_d003_runtime_facing_type_metadata_core_feature_contract.py"),
    ),
    AssetCheck(
        "M227-E003-D003-04",
        "M227-D003",
        Path("spec/planning/compiler/m227/m227_d003_runtime_facing_type_metadata_core_feature_packet.md"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-E003-DOC-EXP-02",
        "# M227 Lane E Semantic Conformance Core Feature Implementation Expectations (E003)",
    ),
    SnippetCheck(
        "M227-E003-DOC-EXP-03",
        "Contract ID: `objc3c-lane-e-semantic-conformance-core-feature-implementation-contract/m227-e003-v1`",
    ),
    SnippetCheck("M227-E003-DOC-EXP-04", "`M227-E002`"),
    SnippetCheck("M227-E003-DOC-EXP-05", "`M227-A008`"),
    SnippetCheck("M227-E003-DOC-EXP-06", "`M227-B006`"),
    SnippetCheck("M227-E003-DOC-EXP-07", "`M227-C004`"),
    SnippetCheck("M227-E003-DOC-EXP-08", "`M227-D003`"),
    SnippetCheck(
        "M227-E003-DOC-EXP-09",
        "`check:objc3c:m227-e003-semantic-conformance-lane-e-core-feature-contract`",
    ),
    SnippetCheck(
        "M227-E003-DOC-EXP-10",
        "`check:objc3c:m227-e003-lane-e-core-feature-readiness`",
    ),
    SnippetCheck(
        "M227-E003-DOC-EXP-11",
        "`tmp/reports/m227/m227_e003_semantic_conformance_lane_e_core_feature_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-E003-DOC-PKT-02",
        "# M227-E003 Semantic Conformance Lane-E Core Feature Implementation Packet",
    ),
    SnippetCheck("M227-E003-DOC-PKT-03", "Packet: `M227-E003`"),
    SnippetCheck("M227-E003-DOC-PKT-04", "Freeze date: `2026-03-02`"),
    SnippetCheck(
        "M227-E003-DOC-PKT-05",
        "Dependencies: `M227-E002`, `M227-A008`, `M227-B006`, `M227-C004`, `M227-D003`",
    ),
    SnippetCheck(
        "M227-E003-DOC-PKT-06",
        "`scripts/check_m227_e003_semantic_conformance_quality_gate_core_feature_contract.py`",
    ),
    SnippetCheck(
        "M227-E003-DOC-PKT-07",
        "`tests/tooling/test_check_m227_e003_semantic_conformance_quality_gate_core_feature_contract.py`",
    ),
    SnippetCheck(
        "M227-E003-DOC-PKT-08",
        "`npm run check:objc3c:m227-e003-lane-e-core-feature-readiness`",
    ),
    SnippetCheck("M227-E003-DOC-PKT-09", "`npm run build:objc3c-native`"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-E003-PKG-02",
        '"check:objc3c:m227-e003-semantic-conformance-lane-e-core-feature-contract": '
        '"python scripts/check_m227_e003_semantic_conformance_quality_gate_core_feature_contract.py"',
    ),
    SnippetCheck(
        "M227-E003-PKG-03",
        '"test:tooling:m227-e003-semantic-conformance-lane-e-core-feature-contract": '
        '"python -m pytest tests/tooling/test_check_m227_e003_semantic_conformance_quality_gate_core_feature_contract.py -q"',
    ),
    SnippetCheck(
        "M227-E003-PKG-04",
        '"check:objc3c:m227-e003-lane-e-core-feature-readiness": '
        '"npm run check:objc3c:m227-e003-semantic-conformance-lane-e-core-feature-contract '
        '&& npm run test:tooling:m227-e003-semantic-conformance-lane-e-core-feature-contract"',
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
        exists_check_id="M227-E003-DOC-EXP-01",
        snippets=EXPECTATIONS_SNIPPETS,
    )
    checks_total += expectations_checks
    findings.extend(expectations_findings)

    packet_checks, packet_findings = check_doc_contract(
        artifact_name="packet_doc",
        path=args.packet_doc,
        exists_check_id="M227-E003-DOC-PKT-01",
        snippets=PACKET_SNIPPETS,
    )
    checks_total += packet_checks
    findings.extend(packet_findings)

    package_checks, package_findings = check_doc_contract(
        artifact_name="package_json",
        path=args.package_json,
        exists_check_id="M227-E003-PKG-01",
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
            "m227-e003-semantic-conformance-lane-e-core-feature-contract: "
            f"error: unable to write summary: {exc}",
            file=sys.stderr,
        )
        return 2

    if not findings:
        print("m227-e003-semantic-conformance-lane-e-core-feature-contract: OK")
        return 0

    print(
        "m227-e003-semantic-conformance-lane-e-core-feature-contract: contract drift detected "
        f"({len(findings)} failed check(s)).",
        file=sys.stderr,
    )
    for finding in findings:
        print(f"- [{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
