#!/usr/bin/env python3
"""Fail-closed checker for M227-E001 semantic conformance lane-E quality-gate freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-e001-semantic-conformance-lane-e-quality-gate-contract-architecture-freeze-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT / "docs" / "contracts" / "m227_lane_e_semantic_conformance_quality_gate_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_e001_semantic_conformance_lane_e_quality_gate_contract_and_architecture_freeze_packet.md"
)
DEFAULT_FREEZE_DOC = (
    ROOT / "spec" / "planning" / "compiler" / "m227" / "m227_e001_semantic_conformance_lane_e_quality_gate_contract_freeze.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m227/m227_e001_semantic_conformance_lane_e_quality_gate_contract_summary.json")


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
        "M227-E001-A001-01",
        "M227-A001",
        Path("docs/contracts/m227_semantic_pass_decomposition_expectations.md"),
    ),
    AssetCheck(
        "M227-E001-A001-02",
        "M227-A001",
        Path("scripts/check_m227_a001_semantic_pass_decomposition_contract.py"),
    ),
    AssetCheck(
        "M227-E001-A001-03",
        "M227-A001",
        Path("tests/tooling/test_check_m227_a001_semantic_pass_decomposition_contract.py"),
    ),
    AssetCheck(
        "M227-E001-A001-04",
        "M227-A001",
        Path("spec/planning/compiler/m227/m227_a001_semantic_pass_contract_freeze.md"),
    ),
    AssetCheck(
        "M227-E001-B002-01",
        "M227-B002",
        Path("docs/contracts/m227_type_system_objc3_forms_modular_split_expectations.md"),
    ),
    AssetCheck(
        "M227-E001-B002-02",
        "M227-B002",
        Path("scripts/check_m227_b002_type_system_objc3_forms_modular_split_contract.py"),
    ),
    AssetCheck(
        "M227-E001-B002-03",
        "M227-B002",
        Path("tests/tooling/test_check_m227_b002_type_system_objc3_forms_modular_split_contract.py"),
    ),
    AssetCheck(
        "M227-E001-B002-04",
        "M227-B002",
        Path("spec/planning/compiler/m227/m227_b002_type_system_objc3_forms_modular_split_packet.md"),
    ),
    AssetCheck(
        "M227-E001-C001-01",
        "M227-C001",
        Path("docs/contracts/m227_typed_sema_to_lowering_contract_expectations.md"),
    ),
    AssetCheck(
        "M227-E001-C001-02",
        "M227-C001",
        Path("scripts/check_m227_c001_typed_sema_to_lowering_contract.py"),
    ),
    AssetCheck(
        "M227-E001-C001-03",
        "M227-C001",
        Path("tests/tooling/test_check_m227_c001_typed_sema_to_lowering_contract.py"),
    ),
    AssetCheck(
        "M227-E001-C001-04",
        "M227-C001",
        Path("spec/planning/compiler/m227/m227_c001_typed_sema_to_lowering_contract_and_architecture_freeze_packet.md"),
    ),
    AssetCheck(
        "M227-E001-D001-01",
        "M227-D001",
        Path("docs/contracts/m227_runtime_facing_type_metadata_semantics_expectations.md"),
    ),
    AssetCheck(
        "M227-E001-D001-02",
        "M227-D001",
        Path("scripts/check_m227_d001_runtime_facing_type_metadata_semantics_contract.py"),
    ),
    AssetCheck(
        "M227-E001-D001-03",
        "M227-D001",
        Path("tests/tooling/test_check_m227_d001_runtime_facing_type_metadata_semantics_contract.py"),
    ),
    AssetCheck(
        "M227-E001-D001-04",
        "M227-D001",
        Path("spec/planning/compiler/m227/m227_d001_runtime_facing_type_metadata_semantics_contract_freeze.md"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-E001-DOC-EXP-02",
        "# M227 Lane E Semantic Conformance Quality Gate Contract and Architecture Freeze Expectations (E001)",
    ),
    SnippetCheck(
        "M227-E001-DOC-EXP-03",
        "Contract ID: `objc3c-lane-e-semantic-conformance-quality-gate-contract/m227-e001-v1`",
    ),
    SnippetCheck("M227-E001-DOC-EXP-04", "`M227-A001`"),
    SnippetCheck("M227-E001-DOC-EXP-05", "`M227-B002`"),
    SnippetCheck("M227-E001-DOC-EXP-06", "`M227-C001`"),
    SnippetCheck("M227-E001-DOC-EXP-07", "`M227-D001`"),
    SnippetCheck(
        "M227-E001-DOC-EXP-08",
        "`spec/planning/compiler/m227/m227_e001_semantic_conformance_lane_e_quality_gate_contract_and_architecture_freeze_packet.md`",
    ),
    SnippetCheck(
        "M227-E001-DOC-EXP-09",
        "`check:objc3c:m227-a001-lane-a-readiness`",
    ),
    SnippetCheck(
        "M227-E001-DOC-EXP-10",
        "`check:objc3c:m227-e001-lane-e-quality-gate-readiness`",
    ),
    SnippetCheck(
        "M227-E001-DOC-EXP-11",
        "`tmp/reports/m227/m227_e001_semantic_conformance_lane_e_quality_gate_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-E001-DOC-PKT-02",
        "# M227-E001 Semantic Conformance Lane-E Quality Gate Contract and Architecture Freeze Packet",
    ),
    SnippetCheck("M227-E001-DOC-PKT-03", "Packet: `M227-E001`"),
    SnippetCheck("M227-E001-DOC-PKT-04", "Dependencies: `M227-A001`, `M227-B002`, `M227-C001`, `M227-D001`"),
    SnippetCheck(
        "M227-E001-DOC-PKT-05",
        "`scripts/check_m227_e001_semantic_conformance_lane_e_quality_gate_contract.py`",
    ),
    SnippetCheck(
        "M227-E001-DOC-PKT-06",
        "`tests/tooling/test_check_m227_e001_semantic_conformance_lane_e_quality_gate_contract.py`",
    ),
    SnippetCheck(
        "M227-E001-DOC-PKT-07",
        "`npm run check:objc3c:m227-e001-lane-e-quality-gate-readiness`",
    ),
    SnippetCheck(
        "M227-E001-DOC-PKT-08",
        "`tmp/reports/m227/m227_e001_semantic_conformance_lane_e_quality_gate_contract_summary.json`",
    ),
)

FREEZE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M227-E001-DOC-FRZ-02", "# M227-E001 Semantic Conformance Lane-E Quality Gate Contract Freeze"),
    SnippetCheck("M227-E001-DOC-FRZ-03", "Packet: `M227-E001`"),
    SnippetCheck("M227-E001-DOC-FRZ-04", "Dependencies: `M227-A001`, `M227-B002`, `M227-C001`, `M227-D001`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-E001-ARCH-02",
        "M227 lane-E E001 semantic conformance quality-gate contract and architecture freeze anchors dependency references (`M227-A001`, `M227-B002`, `M227-C001`, and `M227-D001`)",
    ),
    SnippetCheck(
        "M227-E001-ARCH-03",
        "`check:objc3c:m227-e001-lane-e-quality-gate-readiness`",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-E001-SPC-02",
        "semantic conformance lane-E quality-gate contract and architecture freeze wiring shall preserve explicit lane-E dependency anchors (`M227-A001`, `M227-B002`, `M227-C001`, and `M227-D001`)",
    ),
    SnippetCheck(
        "M227-E001-SPC-03",
        "preserve readiness continuity across `check:objc3c:m227-a001-lane-a-readiness`, `check:objc3c:m227-b002-lane-b-readiness`, `check:objc3c:m227-c001-lane-c-readiness`, and `check:objc3c:m227-d001-lane-d-readiness`",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-E001-META-02",
        "deterministic lane-E semantic conformance quality-gate dependency anchors for `M227-A001`, `M227-B002`, `M227-C001`, and `M227-D001`",
    ),
    SnippetCheck(
        "M227-E001-META-03",
        "with fail-closed readiness continuity (`check:objc3c:m227-a001-lane-a-readiness`, `check:objc3c:m227-b002-lane-b-readiness`, `check:objc3c:m227-c001-lane-c-readiness`, `check:objc3c:m227-d001-lane-d-readiness`)",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-E001-PKG-02",
        '"check:objc3c:m227-a001-semantic-pass-decomposition-contract": "python scripts/check_m227_a001_semantic_pass_decomposition_contract.py"',
    ),
    SnippetCheck(
        "M227-E001-PKG-03",
        '"test:tooling:m227-a001-semantic-pass-decomposition-contract": "python -m pytest tests/tooling/test_check_m227_a001_semantic_pass_decomposition_contract.py -q"',
    ),
    SnippetCheck(
        "M227-E001-PKG-04",
        '"check:objc3c:m227-a001-lane-a-readiness": "npm run check:objc3c:m227-a001-semantic-pass-decomposition-contract && npm run test:tooling:m227-a001-semantic-pass-decomposition-contract"',
    ),
    SnippetCheck(
        "M227-E001-PKG-05",
        '"check:objc3c:m227-e001-semantic-conformance-lane-e-quality-gate-contract": "python scripts/check_m227_e001_semantic_conformance_lane_e_quality_gate_contract.py"',
    ),
    SnippetCheck(
        "M227-E001-PKG-06",
        '"test:tooling:m227-e001-semantic-conformance-lane-e-quality-gate-contract": "python -m pytest tests/tooling/test_check_m227_e001_semantic_conformance_lane_e_quality_gate_contract.py -q"',
    ),
    SnippetCheck(
        "M227-E001-PKG-07",
        '"check:objc3c:m227-e001-lane-e-quality-gate-readiness": "npm run check:objc3c:m227-a001-lane-a-readiness && npm run check:objc3c:m227-b002-lane-b-readiness && npm run check:objc3c:m227-c001-lane-c-readiness && npm run check:objc3c:m227-d001-lane-d-readiness && npm run check:objc3c:m227-e001-semantic-conformance-lane-e-quality-gate-contract && npm run test:tooling:m227-e001-semantic-conformance-lane-e-quality-gate-contract"',
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
    parser.add_argument("--freeze-doc", type=Path, default=DEFAULT_FREEZE_DOC)
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

    for artifact_name, path, exists_check_id, snippets in (
        ("expectations_doc", args.expectations_doc, "M227-E001-DOC-EXP-01", EXPECTATIONS_SNIPPETS),
        ("packet_doc", args.packet_doc, "M227-E001-DOC-PKT-01", PACKET_SNIPPETS),
        ("freeze_doc", args.freeze_doc, "M227-E001-DOC-FRZ-01", FREEZE_SNIPPETS),
        ("architecture_doc", args.architecture_doc, "M227-E001-ARCH-01", ARCHITECTURE_SNIPPETS),
        ("lowering_spec", args.lowering_spec, "M227-E001-SPC-01", LOWERING_SPEC_SNIPPETS),
        ("metadata_spec", args.metadata_spec, "M227-E001-META-01", METADATA_SPEC_SNIPPETS),
        ("package_json", args.package_json, "M227-E001-PKG-01", PACKAGE_SNIPPETS),
    ):
        section_checks, section_findings = check_doc_contract(
            artifact_name=artifact_name,
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += section_checks
        findings.extend(section_findings)

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

    summary_out = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    try:
        summary_out.parent.mkdir(parents=True, exist_ok=True)
        summary_out.write_text(canonical_json(summary), encoding="utf-8")
    except OSError as exc:
        print(
            "m227-e001-semantic-conformance-lane-e-quality-gate-contract: "
            f"error: unable to write summary: {exc}",
            file=sys.stderr,
        )
        return 2

    if not findings:
        print("m227-e001-semantic-conformance-lane-e-quality-gate-contract: OK")
        return 0

    print(
        "m227-e001-semantic-conformance-lane-e-quality-gate-contract: contract drift detected "
        f"({len(findings)} failed check(s)).",
        file=sys.stderr,
    )
    for finding in findings:
        print(f"- [{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
