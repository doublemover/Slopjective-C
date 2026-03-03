#!/usr/bin/env python3
"""Fail-closed checker for M244-A012 interop surface cross-lane integration sync contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m244-a012-interop-surface-syntax-and-declaration-forms-cross-lane-integration-sync-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m244_interop_surface_syntax_and_declaration_forms_cross_lane_integration_sync_a012_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m244"
    / "m244_a012_interop_surface_syntax_and_declaration_forms_cross_lane_integration_sync_packet.md"
)
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m244/M244-A012/"
    "interop_surface_syntax_and_declaration_forms_cross_lane_integration_sync_contract_summary.json"
)


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class PackageScriptKeyCheck:
    check_id: str
    script_key: str


@dataclass(frozen=True)
class PackageScriptCheck:
    check_id: str
    script_key: str
    expected_value: str


@dataclass(frozen=True)
class LaneContract:
    lane_task: str
    contract_id: str
    relative_path: Path
    doc_link_check_id: str
    lane_contract_check_id: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


LANE_CONTRACTS: tuple[LaneContract, ...] = (
    LaneContract(
        "A011",
        "objc3c-interop-surface-syntax-and-declaration-forms-performance-and-quality-guardrails/m244-a011-v1",
        Path(
            "docs/contracts/m244_interop_surface_syntax_and_declaration_forms_performance_and_quality_guardrails_a011_expectations.md"
        ),
        "M244-A012-LINK-A011",
        "M244-A012-LANE-A011",
    ),
    LaneContract(
        "B007",
        "objc3c-interop-semantic-contracts-and-type-mediation-diagnostics-hardening/m244-b007-v1",
        Path("docs/contracts/m244_interop_semantic_contracts_and_type_mediation_diagnostics_hardening_b007_expectations.md"),
        "M244-A012-LINK-B007",
        "M244-A012-LANE-B007",
    ),
    LaneContract(
        "C007",
        "objc3c-interop-lowering-and-abi-conformance-diagnostics-hardening/m244-c007-v1",
        Path("docs/contracts/m244_interop_lowering_and_abi_conformance_diagnostics_hardening_c007_expectations.md"),
        "M244-A012-LINK-C007",
        "M244-A012-LANE-C007",
    ),
    LaneContract(
        "D004",
        "objc3c-runtime-link-bridge-path-core-feature-expansion/m244-d004-v1",
        Path("docs/contracts/m244_runtime_link_bridge_path_core_feature_expansion_d004_expectations.md"),
        "M244-A012-LINK-D004",
        "M244-A012-LANE-D004",
    ),
    LaneContract(
        "E006",
        "objc3c-lane-e-interop-conformance-gate-operations-edge-case-expansion-and-robustness-contract/m244-e006-v1",
        Path(
            "docs/contracts/m244_lane_e_interop_conformance_gate_and_operations_edge_case_expansion_and_robustness_e006_expectations.md"
        ),
        "M244-A012-LINK-E006",
        "M244-A012-LANE-E006",
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M244-A012-DOC-EXP-01",
        "Contract ID: `objc3c-interop-surface-syntax-and-declaration-forms-cross-lane-integration-sync/m244-a012-v1`",
    ),
    SnippetCheck("M244-A012-DOC-EXP-02", "Dependencies: `M244-A011`, `M244-B007`, `M244-C007`, `M244-D004`, `M244-E006`"),
    SnippetCheck("M244-A012-DOC-EXP-03", "Issue `#6529` defines canonical lane-A cross-lane integration sync scope."),
    SnippetCheck(
        "M244-A012-DOC-EXP-04",
        "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
    ),
    SnippetCheck("M244-A012-DOC-EXP-05", "`check:objc3c:m244-a012-lane-a-readiness`"),
    SnippetCheck(
        "M244-A012-DOC-EXP-06",
        "tmp/reports/m244/M244-A012/interop_surface_syntax_and_declaration_forms_cross_lane_integration_sync_contract_summary.json",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M244-A012-DOC-PKT-01", "Packet: `M244-A012`"),
    SnippetCheck("M244-A012-DOC-PKT-02", "Issue: `#6529`"),
    SnippetCheck("M244-A012-DOC-PKT-03", "Dependencies: `M244-A011`, `M244-B007`, `M244-C007`, `M244-D004`, `M244-E006`"),
    SnippetCheck(
        "M244-A012-DOC-PKT-04",
        "`scripts/check_m244_a012_interop_surface_syntax_and_declaration_forms_cross_lane_integration_sync_contract.py`",
    ),
    SnippetCheck(
        "M244-A012-DOC-PKT-05",
        "`tests/tooling/test_check_m244_a012_interop_surface_syntax_and_declaration_forms_cross_lane_integration_sync_contract.py`",
    ),
    SnippetCheck("M244-A012-DOC-PKT-06", "`check:objc3c:m244-a012-lane-a-readiness`"),
)

PACKAGE_SCRIPT_KEY_CHECKS: tuple[PackageScriptKeyCheck, ...] = (
    PackageScriptKeyCheck(
        "M244-A012-CFG-01",
        "check:objc3c:m244-a012-interop-surface-syntax-declaration-forms-cross-lane-integration-sync-contract",
    ),
    PackageScriptKeyCheck(
        "M244-A012-CFG-02",
        "test:tooling:m244-a012-interop-surface-syntax-declaration-forms-cross-lane-integration-sync-contract",
    ),
    PackageScriptKeyCheck("M244-A012-CFG-03", "check:objc3c:m244-a012-lane-a-readiness"),
)

PACKAGE_SCRIPT_CHECKS: tuple[PackageScriptCheck, ...] = (
    PackageScriptCheck(
        "M244-A012-CFG-04",
        "check:objc3c:m244-a012-interop-surface-syntax-declaration-forms-cross-lane-integration-sync-contract",
        "python scripts/check_m244_a012_interop_surface_syntax_and_declaration_forms_cross_lane_integration_sync_contract.py",
    ),
    PackageScriptCheck(
        "M244-A012-CFG-05",
        "test:tooling:m244-a012-interop-surface-syntax-declaration-forms-cross-lane-integration-sync-contract",
        "python -m pytest tests/tooling/test_check_m244_a012_interop_surface_syntax_and_declaration_forms_cross_lane_integration_sync_contract.py -q",
    ),
    PackageScriptCheck(
        "M244-A012-CFG-06",
        "check:objc3c:m244-a012-lane-a-readiness",
        "npm run check:objc3c:m244-a011-lane-a-readiness && npm run check:objc3c:m244-a012-interop-surface-syntax-declaration-forms-cross-lane-integration-sync-contract && npm run test:tooling:m244-a012-interop-surface-syntax-declaration-forms-cross-lane-integration-sync-contract",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M244-A012-ARCH-01",
        "M244 lane-A A012 interop surface syntax/declaration-form cross-lane integration sync anchors",
    ),
    SnippetCheck(
        "M244-A012-ARCH-02",
        "`M244-A011`, `M244-B007`, `M244-C007`, `M244-D004`, and `M244-E006` dependency continuity",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M244-A012-SPC-01",
        "interop surface syntax/declaration-form cross-lane integration sync governance shall preserve explicit",
    ),
    SnippetCheck(
        "M244-A012-SPC-02",
        "lane-A dependency anchors (`M244-A011`, `M244-B007`, `M244-C007`, `M244-D004`, and `M244-E006`)",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M244-A012-META-01",
        "deterministic lane-A interop surface cross-lane integration sync metadata anchors for `M244-A012`",
    ),
    SnippetCheck(
        "M244-A012-META-02",
        "`M244-A011`/`M244-B007`/`M244-C007`/`M244-D004`/`M244-E006` dependency continuity",
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


def check_doc_contract(
    *,
    artifact_name: str,
    path: Path,
    exists_check_id: str,
    snippets: tuple[SnippetCheck, ...],
) -> tuple[int, list[Finding], str]:
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
        return checks_total, findings, ""

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
        return checks_total, findings, ""

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
    return checks_total, findings, text


def check_lane_contracts(expectations_text: str) -> tuple[int, list[Finding]]:
    checks_total = 0
    findings: list[Finding] = []
    for lane_contract in LANE_CONTRACTS:
        checks_total += 1
        if lane_contract.relative_path.as_posix() not in expectations_text:
            findings.append(
                Finding(
                    artifact="expectations_doc",
                    check_id=lane_contract.doc_link_check_id,
                    detail=f"missing lane document anchor: {lane_contract.relative_path.as_posix()}",
                )
            )

        checks_total += 1
        if lane_contract.contract_id not in expectations_text:
            findings.append(
                Finding(
                    artifact="expectations_doc",
                    check_id=f"{lane_contract.doc_link_check_id}-ID",
                    detail=f"missing lane contract id anchor: {lane_contract.contract_id}",
                )
            )

        absolute = ROOT / lane_contract.relative_path
        checks_total += 1
        if not absolute.exists() or not absolute.is_file():
            findings.append(
                Finding(
                    artifact=lane_contract.relative_path.as_posix(),
                    check_id=f"{lane_contract.lane_contract_check_id}-00",
                    detail=f"missing lane contract doc: {lane_contract.relative_path.as_posix()}",
                )
            )
            continue

        try:
            lane_text = absolute.read_text(encoding="utf-8")
        except OSError as exc:
            findings.append(
                Finding(
                    artifact=lane_contract.relative_path.as_posix(),
                    check_id=f"{lane_contract.lane_contract_check_id}-00",
                    detail=f"unable to read lane contract doc: {exc}",
                )
            )
            continue

        checks_total += 1
        if f"Contract ID: `{lane_contract.contract_id}`" not in lane_text:
            findings.append(
                Finding(
                    artifact=lane_contract.relative_path.as_posix(),
                    check_id=lane_contract.lane_contract_check_id,
                    detail=f"missing contract id in lane contract doc: {lane_contract.contract_id}",
                )
            )
    return checks_total, findings


def check_package_contract(path: Path) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists() or not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id="M244-A012-CFG-00",
                detail=f"required document is missing: {display_path(path)}",
            )
        )
        return checks_total, findings

    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except (json.JSONDecodeError, OSError) as exc:
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id="M244-A012-CFG-00",
                detail=f"unable to parse package.json: {exc}",
            )
        )
        return checks_total, findings

    scripts = payload.get("scripts")
    checks_total += 1
    if not isinstance(scripts, dict):
        findings.append(
            Finding(
                artifact="package_json",
                check_id="M244-A012-CFG-00",
                detail='expected top-level "scripts" object in package.json',
            )
        )
        return checks_total, findings

    for key_check in PACKAGE_SCRIPT_KEY_CHECKS:
        checks_total += 1
        if key_check.script_key not in scripts:
            findings.append(
                Finding(
                    artifact="package_json",
                    check_id=key_check.check_id,
                    detail=f'expected scripts["{key_check.script_key}"] to exist',
                )
            )

    for script_check in PACKAGE_SCRIPT_CHECKS:
        checks_total += 1
        actual = scripts.get(script_check.script_key)
        if actual != script_check.expected_value:
            findings.append(
                Finding(
                    artifact="package_json",
                    check_id=script_check.check_id,
                    detail=(
                        f'expected scripts["{script_check.script_key}"] to equal '
                        f'"{script_check.expected_value}"'
                    ),
                )
            )
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    checks_total = 0
    findings: list[Finding] = []

    expectations_checks, expectations_findings, expectations_text = check_doc_contract(
        artifact_name="expectations_doc",
        path=args.expectations_doc,
        exists_check_id="M244-A012-DOC-EXP-00",
        snippets=EXPECTATIONS_SNIPPETS,
    )
    checks_total += expectations_checks
    findings.extend(expectations_findings)

    lane_checks, lane_findings = check_lane_contracts(expectations_text)
    checks_total += lane_checks
    findings.extend(lane_findings)

    packet_checks, packet_findings, _ = check_doc_contract(
        artifact_name="packet_doc",
        path=args.packet_doc,
        exists_check_id="M244-A012-DOC-PKT-00",
        snippets=PACKET_SNIPPETS,
    )
    checks_total += packet_checks
    findings.extend(packet_findings)

    package_checks, package_findings = check_package_contract(args.package_json)
    checks_total += package_checks
    findings.extend(package_findings)

    architecture_checks, architecture_findings, _ = check_doc_contract(
        artifact_name="architecture_doc",
        path=args.architecture_doc,
        exists_check_id="M244-A012-ARCH-00",
        snippets=ARCHITECTURE_SNIPPETS,
    )
    checks_total += architecture_checks
    findings.extend(architecture_findings)

    lowering_checks, lowering_findings, _ = check_doc_contract(
        artifact_name="lowering_spec",
        path=args.lowering_spec,
        exists_check_id="M244-A012-SPC-00",
        snippets=LOWERING_SPEC_SNIPPETS,
    )
    checks_total += lowering_checks
    findings.extend(lowering_findings)

    metadata_checks, metadata_findings, _ = check_doc_contract(
        artifact_name="metadata_spec",
        path=args.metadata_spec,
        exists_check_id="M244-A012-META-00",
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
