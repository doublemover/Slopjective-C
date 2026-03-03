#!/usr/bin/env python3
"""Fail-closed checker for M227-D007 runtime-facing type metadata diagnostics hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-d007-runtime-facing-type-metadata-diagnostics-hardening-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m227_runtime_facing_type_metadata_diagnostics_hardening_d007_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_d007_runtime_facing_type_metadata_diagnostics_hardening_packet.md"
)
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_TYPED_SURFACE = (
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_typed_sema_to_lowering_contract_surface.h"
)
DEFAULT_PARSE_READINESS = (
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_parse_lowering_readiness_surface.h"
)
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m227/M227-D007/runtime_facing_type_metadata_diagnostics_hardening_contract_summary.json"
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
class PackageScriptKeyCheck:
    check_id: str
    script_key: str


@dataclass(frozen=True)
class PackageScriptCheck:
    check_id: str
    script_key: str
    expected_value: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


PREREQUISITE_ASSETS: tuple[AssetCheck, ...] = (
    AssetCheck(
        "M227-D007-D006-01",
        "M227-D006",
        Path("docs/contracts/m227_runtime_facing_type_metadata_edge_case_expansion_and_robustness_d006_expectations.md"),
    ),
    AssetCheck(
        "M227-D007-D006-02",
        "M227-D006",
        Path("spec/planning/compiler/m227/m227_d006_runtime_facing_type_metadata_edge_case_expansion_and_robustness_packet.md"),
    ),
    AssetCheck(
        "M227-D007-D006-03",
        "M227-D006",
        Path("scripts/check_m227_d006_runtime_facing_type_metadata_edge_case_expansion_and_robustness_contract.py"),
    ),
    AssetCheck(
        "M227-D007-D006-04",
        "M227-D006",
        Path("tests/tooling/test_check_m227_d006_runtime_facing_type_metadata_edge_case_expansion_and_robustness_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-D007-DOC-EXP-01",
        "Contract ID: `objc3c-runtime-facing-type-metadata-diagnostics-hardening/m227-d007-v1`",
    ),
    SnippetCheck("M227-D007-DOC-EXP-02", "Dependencies: `M227-D006`"),
    SnippetCheck(
        "M227-D007-DOC-EXP-03",
        "Issue `#5153` defines canonical lane-D diagnostics hardening scope.",
    ),
    SnippetCheck("M227-D007-DOC-EXP-04", "`check:objc3c:m227-d007-lane-d-readiness`"),
    SnippetCheck(
        "M227-D007-DOC-EXP-05",
        "`python scripts/check_m227_d006_runtime_facing_type_metadata_edge_case_expansion_and_robustness_contract.py`",
    ),
    SnippetCheck(
        "M227-D007-DOC-EXP-06",
        "`python scripts/check_m227_d007_runtime_facing_type_metadata_diagnostics_hardening_contract.py`",
    ),
    SnippetCheck(
        "M227-D007-DOC-EXP-07",
        "`python -m pytest tests/tooling/test_check_m227_d007_runtime_facing_type_metadata_diagnostics_hardening_contract.py -q`",
    ),
    SnippetCheck(
        "M227-D007-DOC-EXP-08",
        "tmp/reports/m227/M227-D007/runtime_facing_type_metadata_diagnostics_hardening_contract_summary.json",
    ),
    SnippetCheck(
        "M227-D007-DOC-EXP-09",
        "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M227-D007-DOC-PKT-01", "Packet: `M227-D007`"),
    SnippetCheck("M227-D007-DOC-PKT-02", "Issue: `#5153`"),
    SnippetCheck("M227-D007-DOC-PKT-03", "Dependencies: `M227-D006`"),
    SnippetCheck(
        "M227-D007-DOC-PKT-04",
        "`scripts/check_m227_d007_runtime_facing_type_metadata_diagnostics_hardening_contract.py`",
    ),
    SnippetCheck(
        "M227-D007-DOC-PKT-05",
        "`tests/tooling/test_check_m227_d007_runtime_facing_type_metadata_diagnostics_hardening_contract.py`",
    ),
    SnippetCheck("M227-D007-DOC-PKT-06", "`check:objc3c:m227-d007-lane-d-readiness`"),
    SnippetCheck(
        "M227-D007-DOC-PKT-07",
        "`python scripts/check_m227_d007_runtime_facing_type_metadata_diagnostics_hardening_contract.py --emit-json`",
    ),
    SnippetCheck(
        "M227-D007-DOC-PKT-08",
        "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
    ),
)

FRONTEND_TYPES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M227-D007-TYP-01", "bool typed_diagnostics_hardening_consistent = false;"),
    SnippetCheck("M227-D007-TYP-02", "bool typed_diagnostics_hardening_ready = false;"),
    SnippetCheck("M227-D007-TYP-03", "std::string typed_diagnostics_hardening_key;"),
    SnippetCheck("M227-D007-TYP-04", "bool typed_sema_diagnostics_hardening_consistent = false;"),
    SnippetCheck("M227-D007-TYP-05", "bool typed_sema_diagnostics_hardening_ready = false;"),
    SnippetCheck("M227-D007-TYP-06", "std::string typed_sema_diagnostics_hardening_key;"),
)

TYPED_SURFACE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M227-D007-SUR-01", "surface.typed_diagnostics_hardening_consistent ="),
    SnippetCheck("M227-D007-SUR-02", "surface.typed_diagnostics_hardening_ready ="),
    SnippetCheck("M227-D007-SUR-03", "surface.typed_diagnostics_hardening_key ="),
    SnippetCheck("M227-D007-SUR-04", "const bool typed_diagnostics_hardening_key_ready ="),
    SnippetCheck(
        "M227-D007-SUR-05",
        'surface.failure_reason = "typed sema-to-lowering diagnostics hardening is inconsistent";',
    ),
    SnippetCheck(
        "M227-D007-SUR-06",
        'surface.failure_reason = "typed sema-to-lowering diagnostics hardening is not ready";',
    ),
    SnippetCheck(
        "M227-D007-SUR-07",
        'surface.failure_reason = "typed sema-to-lowering diagnostics hardening key is empty";',
    ),
)

PARSE_READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M227-D007-REA-01", "surface.typed_sema_diagnostics_hardening_consistent ="),
    SnippetCheck("M227-D007-REA-02", "surface.typed_sema_diagnostics_hardening_ready ="),
    SnippetCheck("M227-D007-REA-03", "surface.typed_sema_diagnostics_hardening_key ="),
    SnippetCheck(
        "M227-D007-REA-04",
        "surface.typed_sema_diagnostics_hardening_consistent ==",
    ),
    SnippetCheck(
        "M227-D007-REA-05",
        "typed_sema_to_lowering_contract_surface.typed_diagnostics_hardening_consistent &&",
    ),
    SnippetCheck(
        "M227-D007-REA-06",
        "typed_sema_to_lowering_contract_surface.typed_diagnostics_hardening_ready &&",
    ),
    SnippetCheck(
        "M227-D007-REA-07",
        "typed_sema_to_lowering_contract_surface.typed_diagnostics_hardening_key;",
    ),
    SnippetCheck(
        "M227-D007-REA-08",
        'surface.failure_reason = "typed sema-to-lowering diagnostics hardening drifted from parse/lowering readiness";',
    ),
)

PACKAGE_SCRIPT_KEY_CHECKS: tuple[PackageScriptKeyCheck, ...] = (
    PackageScriptKeyCheck(
        "M227-D007-CFG-01",
        "check:objc3c:m227-d007-runtime-facing-type-metadata-diagnostics-hardening-contract",
    ),
    PackageScriptKeyCheck(
        "M227-D007-CFG-02",
        "test:tooling:m227-d007-runtime-facing-type-metadata-diagnostics-hardening-contract",
    ),
    PackageScriptKeyCheck("M227-D007-CFG-03", "check:objc3c:m227-d007-lane-d-readiness"),
    PackageScriptKeyCheck("M227-D007-CFG-04", "check:objc3c:m227-d006-lane-d-readiness"),
    PackageScriptKeyCheck("M227-D007-CFG-08", "compile:objc3c"),
    PackageScriptKeyCheck("M227-D007-CFG-09", "proof:objc3c"),
    PackageScriptKeyCheck("M227-D007-CFG-10", "test:objc3c:execution-replay-proof"),
    PackageScriptKeyCheck("M227-D007-CFG-11", "test:objc3c:perf-budget"),
)

PACKAGE_SCRIPT_CHECKS: tuple[PackageScriptCheck, ...] = (
    PackageScriptCheck(
        "M227-D007-CFG-05",
        "check:objc3c:m227-d007-runtime-facing-type-metadata-diagnostics-hardening-contract",
        "python scripts/check_m227_d007_runtime_facing_type_metadata_diagnostics_hardening_contract.py",
    ),
    PackageScriptCheck(
        "M227-D007-CFG-06",
        "test:tooling:m227-d007-runtime-facing-type-metadata-diagnostics-hardening-contract",
        "python -m pytest tests/tooling/test_check_m227_d007_runtime_facing_type_metadata_diagnostics_hardening_contract.py -q",
    ),
    PackageScriptCheck(
        "M227-D007-CFG-07",
        "check:objc3c:m227-d007-lane-d-readiness",
        "npm run check:objc3c:m227-d006-lane-d-readiness && npm run check:objc3c:m227-d007-runtime-facing-type-metadata-diagnostics-hardening-contract && npm run test:tooling:m227-d007-runtime-facing-type-metadata-diagnostics-hardening-contract",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-D007-ARCH-01",
        "M227 lane-D D007 runtime-facing type metadata diagnostics hardening anchors",
    ),
    SnippetCheck(
        "M227-D007-ARCH-02",
        "docs/contracts/m227_runtime_facing_type_metadata_diagnostics_hardening_d007_expectations.md",
    ),
    SnippetCheck(
        "M227-D007-ARCH-03",
        "and fail-closed against `M227-D006` dependency drift.",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-D007-SPC-01",
        "runtime-facing type metadata diagnostics hardening governance shall preserve explicit lane-D dependency anchors (`M227-D007`, `M227-D006`)",
    ),
    SnippetCheck(
        "M227-D007-SPC-02",
        "runtime-facing type metadata diagnostics-hardening consistency, readiness, key, or alignment continuity drift before recovery/determinism validation advances.",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M227-D007-META-01",
        "deterministic lane-D runtime-facing type metadata diagnostics hardening metadata anchors for `M227-D007`",
    ),
    SnippetCheck(
        "M227-D007-META-02",
        "explicit `M227-D006` dependency continuity and fail-closed diagnostics-hardening evidence continuity",
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
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--typed-surface", type=Path, default=DEFAULT_TYPED_SURFACE)
    parser.add_argument("--parse-readiness", type=Path, default=DEFAULT_PARSE_READINESS)
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


def check_package_contract(path: Path) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists() or not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id="M227-D007-CFG-00",
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
                check_id="M227-D007-CFG-00",
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
                check_id="M227-D007-CFG-00",
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

    checks_total, findings = check_prerequisite_assets()

    expectations_checks, expectations_findings = check_doc_contract(
        artifact_name="expectations_doc",
        path=args.expectations_doc,
        exists_check_id="M227-D007-DOC-EXP-00",
        snippets=EXPECTATIONS_SNIPPETS,
    )
    checks_total += expectations_checks
    findings.extend(expectations_findings)

    packet_checks, packet_findings = check_doc_contract(
        artifact_name="packet_doc",
        path=args.packet_doc,
        exists_check_id="M227-D007-DOC-PKT-00",
        snippets=PACKET_SNIPPETS,
    )
    checks_total += packet_checks
    findings.extend(packet_findings)

    frontend_checks, frontend_findings = check_doc_contract(
        artifact_name="frontend_types",
        path=args.frontend_types,
        exists_check_id="M227-D007-TYP-00",
        snippets=FRONTEND_TYPES_SNIPPETS,
    )
    checks_total += frontend_checks
    findings.extend(frontend_findings)

    typed_checks, typed_findings = check_doc_contract(
        artifact_name="typed_surface",
        path=args.typed_surface,
        exists_check_id="M227-D007-SUR-00",
        snippets=TYPED_SURFACE_SNIPPETS,
    )
    checks_total += typed_checks
    findings.extend(typed_findings)

    readiness_checks, readiness_findings = check_doc_contract(
        artifact_name="parse_readiness",
        path=args.parse_readiness,
        exists_check_id="M227-D007-REA-00",
        snippets=PARSE_READINESS_SNIPPETS,
    )
    checks_total += readiness_checks
    findings.extend(readiness_findings)

    package_checks, package_findings = check_package_contract(args.package_json)
    checks_total += package_checks
    findings.extend(package_findings)

    architecture_checks, architecture_findings = check_doc_contract(
        artifact_name="architecture_doc",
        path=args.architecture_doc,
        exists_check_id="M227-D007-ARCH-00",
        snippets=ARCHITECTURE_SNIPPETS,
    )
    checks_total += architecture_checks
    findings.extend(architecture_findings)

    lowering_checks, lowering_findings = check_doc_contract(
        artifact_name="lowering_spec",
        path=args.lowering_spec,
        exists_check_id="M227-D007-SPC-00",
        snippets=LOWERING_SPEC_SNIPPETS,
    )
    checks_total += lowering_checks
    findings.extend(lowering_findings)

    metadata_checks, metadata_findings = check_doc_contract(
        artifact_name="metadata_spec",
        path=args.metadata_spec,
        exists_check_id="M227-D007-META-00",
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
