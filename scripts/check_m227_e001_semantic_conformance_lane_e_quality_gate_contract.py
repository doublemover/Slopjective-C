#!/usr/bin/env python3
"""Fail-closed prerequisite checker for the M227-E001 semantic conformance lane-E quality gate."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-e001-semantic-conformance-lane-e-quality-gate-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT / "docs" / "contracts" / "m227_lane_e_semantic_conformance_quality_gate_expectations.md"
)
DEFAULT_FREEZE_DOC = (
    ROOT / "spec" / "planning" / "compiler" / "m227" / "m227_e001_semantic_conformance_lane_e_quality_gate_contract_freeze.md"
)
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
        "M227-E001-A002-01",
        "M227-A002",
        Path("docs/contracts/m227_semantic_pass_modular_split_expectations.md"),
    ),
    AssetCheck(
        "M227-E001-A002-02",
        "M227-A002",
        Path("scripts/check_m227_a002_semantic_pass_modular_split_contract.py"),
    ),
    AssetCheck(
        "M227-E001-A002-03",
        "M227-A002",
        Path("tests/tooling/test_check_m227_a002_semantic_pass_modular_split_contract.py"),
    ),
    AssetCheck(
        "M227-E001-A002-04",
        "M227-A002",
        Path("spec/planning/compiler/m227/m227_a002_semantic_pass_modular_split_packet.md"),
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
    SnippetCheck("M227-E001-DOC-EXP-02", "# M227 Lane E Semantic Conformance and Quality Gate Expectations (E001)"),
    SnippetCheck(
        "M227-E001-DOC-EXP-03",
        "Contract ID: `objc3c-lane-e-semantic-conformance-quality-gate-contract/m227-e001-v1`",
    ),
    SnippetCheck("M227-E001-DOC-EXP-04", "`M227-A001`"),
    SnippetCheck("M227-E001-DOC-EXP-05", "`M227-B002`"),
    SnippetCheck("M227-E001-DOC-EXP-06", "`M227-C001`"),
    SnippetCheck("M227-E001-DOC-EXP-07", "`M227-D001`"),
    SnippetCheck(
        "M227-E001-DOC-EXP-09",
        "`check:objc3c:m227-e001-semantic-conformance-lane-e-quality-gate-contract`",
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

FREEZE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M227-E001-DOC-FRZ-02", "# M227-E001 Semantic Conformance Lane-E Quality Gate Contract Freeze"),
    SnippetCheck("M227-E001-DOC-FRZ-03", "Packet: `M227-E001`"),
    SnippetCheck("M227-E001-DOC-FRZ-04", "Freeze date: `2026-03-02`"),
    SnippetCheck("M227-E001-DOC-FRZ-05", "Dependencies: `M227-A001`, `M227-B002`, `M227-C001`, `M227-D001`"),
    SnippetCheck(
        "M227-E001-DOC-FRZ-06",
        "`python scripts/check_m227_e001_semantic_conformance_lane_e_quality_gate_contract.py`",
    ),
    SnippetCheck(
        "M227-E001-DOC-FRZ-07",
        "`npm run check:objc3c:m227-e001-lane-e-quality-gate-readiness`",
    ),
    SnippetCheck(
        "M227-E001-DOC-FRZ-08",
        "`tmp/reports/m227/m227_e001_semantic_conformance_lane_e_quality_gate_contract_summary.json`",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-E001-PKG-02",
        '"check:objc3c:m227-e001-semantic-conformance-lane-e-quality-gate-contract": '
        '"python scripts/check_m227_e001_semantic_conformance_lane_e_quality_gate_contract.py"',
    ),
    SnippetCheck(
        "M227-E001-PKG-03",
        '"test:tooling:m227-e001-semantic-conformance-lane-e-quality-gate-contract": '
        '"python -m pytest tests/tooling/test_check_m227_e001_semantic_conformance_lane_e_quality_gate_contract.py -q"',
    ),
    SnippetCheck(
        "M227-E001-PKG-04",
        '"check:objc3c:m227-e001-lane-e-quality-gate-readiness": '
        '"npm run check:objc3c:m227-e001-semantic-conformance-lane-e-quality-gate-contract '
        '&& npm run test:tooling:m227-e001-semantic-conformance-lane-e-quality-gate-contract"',
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
    parser.add_argument("--freeze-doc", type=Path, default=DEFAULT_FREEZE_DOC)
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
        exists_check_id="M227-E001-DOC-EXP-01",
        snippets=EXPECTATIONS_SNIPPETS,
    )
    checks_total += expectations_checks
    findings.extend(expectations_findings)

    freeze_checks, freeze_findings = check_doc_contract(
        artifact_name="freeze_doc",
        path=args.freeze_doc,
        exists_check_id="M227-E001-DOC-FRZ-01",
        snippets=FREEZE_SNIPPETS,
    )
    checks_total += freeze_checks
    findings.extend(freeze_findings)

    package_checks, package_findings = check_doc_contract(
        artifact_name="package_json",
        path=args.package_json,
        exists_check_id="M227-E001-PKG-01",
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
