#!/usr/bin/env python3
"""Fail-closed checker for M244-D010 runtime/link bridge-path conformance corpus expansion contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m244-d010-runtime-link-bridge-path-conformance-corpus-expansion-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m244_runtime_link_bridge_path_conformance_corpus_expansion_d010_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m244"
    / "m244_d010_runtime_link_bridge_path_conformance_corpus_expansion_packet.md"
)
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m244/M244-D010/runtime_link_bridge_path_conformance_corpus_expansion_contract_summary.json"
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
        "M244-D010-D009-01",
        "M244-D009",
        Path("docs/contracts/m244_runtime_link_bridge_path_conformance_matrix_implementation_d009_expectations.md"),
    ),
    AssetCheck(
        "M244-D010-D009-02",
        "M244-D009",
        Path("spec/planning/compiler/m244/m244_d009_runtime_link_bridge_path_conformance_matrix_implementation_packet.md"),
    ),
    AssetCheck(
        "M244-D010-D009-03",
        "M244-D009",
        Path("scripts/check_m244_d009_runtime_link_bridge_path_conformance_matrix_implementation_contract.py"),
    ),
    AssetCheck(
        "M244-D010-D009-04",
        "M244-D009",
        Path("tests/tooling/test_check_m244_d009_runtime_link_bridge_path_conformance_matrix_implementation_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M244-D010-DOC-EXP-01",
        "Contract ID: `objc3c-runtime-link-bridge-path-conformance-corpus-expansion/m244-d010-v1`",
    ),
    SnippetCheck("M244-D010-DOC-EXP-02", "Dependencies: `M244-D009`"),
    SnippetCheck("M244-D010-DOC-EXP-03", "Issue `#6582` defines canonical lane-D conformance corpus expansion scope."),
    SnippetCheck("M244-D010-DOC-EXP-04", "`check:objc3c:m244-d010-lane-d-readiness`"),
    SnippetCheck("M244-D010-DOC-EXP-05", "`check:objc3c:m244-d009-lane-d-readiness`"),
    SnippetCheck("M244-D010-DOC-EXP-06", "`test:objc3c:execution-replay-proof`"),
    SnippetCheck(
        "M244-D010-DOC-EXP-07",
        "tmp/reports/m244/M244-D010/runtime_link_bridge_path_conformance_corpus_expansion_contract_summary.json",
    ),
    SnippetCheck(
        "M244-D010-DOC-EXP-08",
        "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M244-D010-DOC-PKT-01", "Packet: `M244-D010`"),
    SnippetCheck("M244-D010-DOC-PKT-02", "Issue: `#6582`"),
    SnippetCheck("M244-D010-DOC-PKT-03", "Dependencies: `M244-D009`"),
    SnippetCheck(
        "M244-D010-DOC-PKT-04",
        "`scripts/check_m244_d010_runtime_link_bridge_path_conformance_corpus_expansion_contract.py`",
    ),
    SnippetCheck(
        "M244-D010-DOC-PKT-05",
        "`tests/tooling/test_check_m244_d010_runtime_link_bridge_path_conformance_corpus_expansion_contract.py`",
    ),
    SnippetCheck("M244-D010-DOC-PKT-06", "`check:objc3c:m244-d010-lane-d-readiness`"),
    SnippetCheck(
        "M244-D010-DOC-PKT-07",
        "`python scripts/check_m244_d010_runtime_link_bridge_path_conformance_corpus_expansion_contract.py --emit-json`",
    ),
    SnippetCheck(
        "M244-D010-DOC-PKT-08",
        "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
    ),
)

PACKAGE_SCRIPT_KEY_CHECKS: tuple[PackageScriptKeyCheck, ...] = (
    PackageScriptKeyCheck(
        "M244-D010-CFG-01",
        "check:objc3c:m244-d010-runtime-link-bridge-path-conformance-corpus-expansion-contract",
    ),
    PackageScriptKeyCheck(
        "M244-D010-CFG-02",
        "test:tooling:m244-d010-runtime-link-bridge-path-conformance-corpus-expansion-contract",
    ),
    PackageScriptKeyCheck("M244-D010-CFG-03", "check:objc3c:m244-d010-lane-d-readiness"),
    PackageScriptKeyCheck("M244-D010-CFG-07", "compile:objc3c"),
    PackageScriptKeyCheck("M244-D010-CFG-08", "proof:objc3c"),
    PackageScriptKeyCheck("M244-D010-CFG-09", "test:objc3c:execution-replay-proof"),
    PackageScriptKeyCheck("M244-D010-CFG-10", "test:objc3c:perf-budget"),
)

PACKAGE_SCRIPT_CHECKS: tuple[PackageScriptCheck, ...] = (
    PackageScriptCheck(
        "M244-D010-CFG-04",
        "check:objc3c:m244-d010-runtime-link-bridge-path-conformance-corpus-expansion-contract",
        "python scripts/check_m244_d010_runtime_link_bridge_path_conformance_corpus_expansion_contract.py",
    ),
    PackageScriptCheck(
        "M244-D010-CFG-05",
        "test:tooling:m244-d010-runtime-link-bridge-path-conformance-corpus-expansion-contract",
        "python -m pytest tests/tooling/test_check_m244_d010_runtime_link_bridge_path_conformance_corpus_expansion_contract.py -q",
    ),
    PackageScriptCheck(
        "M244-D010-CFG-06",
        "check:objc3c:m244-d010-lane-d-readiness",
        "npm run check:objc3c:m244-d009-lane-d-readiness && npm run check:objc3c:m244-d010-runtime-link-bridge-path-conformance-corpus-expansion-contract && npm run test:tooling:m244-d010-runtime-link-bridge-path-conformance-corpus-expansion-contract",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M244-D010-ARCH-01",
        "M244 lane-D D010 runtime/link bridge-path conformance corpus expansion anchors",
    ),
    SnippetCheck(
        "M244-D010-ARCH-02",
        "docs/contracts/m244_runtime_link_bridge_path_conformance_corpus_expansion_d010_expectations.md",
    ),
    SnippetCheck(
        "M244-D010-ARCH-03",
        "and fail-closed against `M244-D009` dependency drift.",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M244-D010-SPC-01",
        "runtime/link bridge-path conformance corpus expansion governance shall preserve explicit lane-D dependency anchors (`M244-D009`)",
    ),
    SnippetCheck(
        "M244-D010-SPC-02",
        "conformance corpus expansion evidence drift before downstream runtime projection and metadata integration advances.",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M244-D010-META-01",
        "deterministic lane-D runtime/link bridge-path conformance corpus expansion metadata anchors for `M244-D010`",
    ),
    SnippetCheck(
        "M244-D010-META-02",
        "explicit `M244-D009` dependency continuity and fail-closed evidence continuity",
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
                check_id="M244-D010-CFG-00",
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
                check_id="M244-D010-CFG-00",
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
                check_id="M244-D010-CFG-00",
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
        exists_check_id="M244-D010-DOC-EXP-00",
        snippets=EXPECTATIONS_SNIPPETS,
    )
    checks_total += expectations_checks
    findings.extend(expectations_findings)

    packet_checks, packet_findings = check_doc_contract(
        artifact_name="packet_doc",
        path=args.packet_doc,
        exists_check_id="M244-D010-DOC-PKT-00",
        snippets=PACKET_SNIPPETS,
    )
    checks_total += packet_checks
    findings.extend(packet_findings)

    package_checks, package_findings = check_package_contract(args.package_json)
    checks_total += package_checks
    findings.extend(package_findings)

    architecture_checks, architecture_findings = check_doc_contract(
        artifact_name="architecture_doc",
        path=args.architecture_doc,
        exists_check_id="M244-D010-ARCH-00",
        snippets=ARCHITECTURE_SNIPPETS,
    )
    checks_total += architecture_checks
    findings.extend(architecture_findings)

    lowering_checks, lowering_findings = check_doc_contract(
        artifact_name="lowering_spec",
        path=args.lowering_spec,
        exists_check_id="M244-D010-SPC-00",
        snippets=LOWERING_SPEC_SNIPPETS,
    )
    checks_total += lowering_checks
    findings.extend(lowering_findings)

    metadata_checks, metadata_findings = check_doc_contract(
        artifact_name="metadata_spec",
        path=args.metadata_spec,
        exists_check_id="M244-D010-META-00",
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


