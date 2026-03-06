#!/usr/bin/env python3
"""Fail-closed checker for M229-A007 class/protocol/category metadata diagnostics hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m229-a007-class-protocol-category-metadata-generation-diagnostics-hardening-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m229_class_protocol_category_metadata_generation_diagnostics_hardening_a007_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m229"
    / "m229_a007_class_protocol_category_metadata_generation_diagnostics_hardening_packet.md"
)
DEFAULT_A006_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m229_class_protocol_category_metadata_generation_edge_case_expansion_and_robustness_a006_expectations.md"
)
DEFAULT_A006_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m229"
    / "m229_a006_class_protocol_category_metadata_generation_edge_case_expansion_and_robustness_packet.md"
)
DEFAULT_A006_CHECKER = (
    ROOT / "scripts" / "check_m229_a006_class_protocol_category_metadata_generation_edge_case_expansion_and_robustness_contract.py"
)
DEFAULT_A006_TEST = (
    ROOT / "tests" / "tooling" / "test_check_m229_a006_class_protocol_category_metadata_generation_edge_case_expansion_and_robustness_contract.py"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m229/M229-A007/class_protocol_category_metadata_generation_diagnostics_hardening_summary.json"
)


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


PREREQUISITE_ASSETS: tuple[tuple[str, Path], ...] = (
    (
        "M229-A007-DEP-A006-01",
        Path("docs/contracts/m229_class_protocol_category_metadata_generation_edge_case_expansion_and_robustness_a006_expectations.md"),
    ),
    (
        "M229-A007-DEP-A006-02",
        Path("spec/planning/compiler/m229/m229_a006_class_protocol_category_metadata_generation_edge_case_expansion_and_robustness_packet.md"),
    ),
    (
        "M229-A007-DEP-A006-03",
        Path("scripts/check_m229_a006_class_protocol_category_metadata_generation_edge_case_expansion_and_robustness_contract.py"),
    ),
    (
        "M229-A007-DEP-A006-04",
        Path("tests/tooling/test_check_m229_a006_class_protocol_category_metadata_generation_edge_case_expansion_and_robustness_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M229-A007-DOC-EXP-01",
        "# M229 Class/Protocol/Category Metadata Generation Diagnostics Hardening Expectations (A007)",
    ),
    SnippetCheck(
        "M229-A007-DOC-EXP-02",
        "Contract ID: `objc3c-class-protocol-category-metadata-generation-diagnostics-hardening/m229-a007-v1`",
    ),
    SnippetCheck("M229-A007-DOC-EXP-03", "Dependencies: `M229-A006`"),
    SnippetCheck(
        "M229-A007-DOC-EXP-04",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M229-A007-DOC-EXP-05",
        "check:objc3c:m229-a007-class-protocol-category-metadata-generation-diagnostics-hardening-contract",
    ),
    SnippetCheck(
        "M229-A007-DOC-EXP-06",
        "check:objc3c:m229-a007-lane-a-readiness",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M229-A007-DOC-PKT-01",
        "# M229-A007 Class/Protocol/Category Metadata Generation Diagnostics Hardening Packet",
    ),
    SnippetCheck("M229-A007-DOC-PKT-02", "Packet: `M229-A007`"),
    SnippetCheck("M229-A007-DOC-PKT-03", "Dependencies: `M229-A006`"),
    SnippetCheck(
        "M229-A007-DOC-PKT-04",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M229-A007-DOC-PKT-05",
        "spec/planning/compiler/m229/m229_a006_class_protocol_category_metadata_generation_edge_case_expansion_and_robustness_packet.md",
    ),
)

A006_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M229-A007-A006-EXP-01",
        "# M229 Class/Protocol/Category Metadata Generation Edge-case Expansion and Robustness Expectations (A006)",
    ),
    SnippetCheck(
        "M229-A007-A006-EXP-02",
        "Contract ID: `objc3c-class-protocol-category-metadata-generation-edge-case-expansion-and-robustness/m229-a006-v1`",
    ),
)

A006_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M229-A007-A006-PKT-01", "Packet: `M229-A006`"),
    SnippetCheck("M229-A007-A006-PKT-02", "Dependencies: `M229-A005`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M229-A007-ARCH-01",
        "M229 lane-A A007 class/protocol/category metadata generation diagnostics hardening anchors",
    ),
    SnippetCheck(
        "M229-A007-ARCH-02",
        "m229_class_protocol_category_metadata_generation_diagnostics_hardening_a007_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M229-A007-SPC-01",
        "class/protocol/category metadata generation diagnostics hardening governance shall preserve explicit",
    ),
    SnippetCheck(
        "M229-A007-SPC-02",
        "lane-A dependency anchors (`M229-A006`) and fail closed on diagnostics-hardening evidence drift",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M229-A007-META-01",
        "deterministic lane-A class/protocol/category metadata generation diagnostics hardening anchors for `M229-A007`",
    ),
    SnippetCheck(
        "M229-A007-META-02",
        "explicit `M229-A006` dependency continuity so class/protocol/category metadata diagnostics-hardening drift fails closed",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M229-A007-PKG-01",
        '"check:objc3c:m229-a007-class-protocol-category-metadata-generation-diagnostics-hardening-contract": '
        '"python scripts/check_m229_a007_class_protocol_category_metadata_generation_diagnostics_hardening_contract.py"',
    ),
    SnippetCheck(
        "M229-A007-PKG-02",
        '"test:tooling:m229-a007-class-protocol-category-metadata-generation-diagnostics-hardening-contract": '
        '"python -m pytest tests/tooling/test_check_m229_a007_class_protocol_category_metadata_generation_diagnostics_hardening_contract.py -q"',
    ),
    SnippetCheck(
        "M229-A007-PKG-03",
        '"check:objc3c:m229-a007-lane-a-readiness": '
        '"npm run check:objc3c:m229-a006-lane-a-readiness '
        '&& npm run check:objc3c:m229-a007-class-protocol-category-metadata-generation-diagnostics-hardening-contract '
        '&& npm run test:tooling:m229-a007-class-protocol-category-metadata-generation-diagnostics-hardening-contract"',
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
    parser.add_argument("--a006-expectations-doc", type=Path, default=DEFAULT_A006_EXPECTATIONS_DOC)
    parser.add_argument("--a006-packet-doc", type=Path, default=DEFAULT_A006_PACKET_DOC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_file(path: Path, exists_check_id: str, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists() or not path.is_file():
        findings.append(Finding(display_path(path), exists_check_id, f"required file is missing: {display_path(path)}"))
        return checks_total, findings

    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def check_prerequisites() -> tuple[int, list[Finding]]:
    checks_total = len(PREREQUISITE_ASSETS)
    findings: list[Finding] = []
    for check_id, rel in PREREQUISITE_ASSETS:
        target = ROOT / rel
        if not target.exists() or not target.is_file():
            findings.append(Finding(rel.as_posix(), check_id, f"missing required dependency artifact: {rel.as_posix()}"))
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    count, findings = check_prerequisites()
    checks_total += count
    failures.extend(findings)

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M229-A007-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M229-A007-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.a006_expectations_doc, "M229-A007-A006-EXP-EXISTS", A006_EXPECTATIONS_SNIPPETS),
        (args.a006_packet_doc, "M229-A007-A006-PKT-EXISTS", A006_PACKET_SNIPPETS),
        (args.architecture_doc, "M229-A007-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M229-A007-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M229-A007-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M229-A007-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_file(path, exists_check_id, snippets)
        checks_total += count
        failures.extend(findings)

    checks_passed = checks_total - len(failures)
    payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail}
            for f in failures
        ],
    }

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(payload), encoding="utf-8")

    if failures:
        for f in failures:
            print(f"[{f.check_id}] {f.artifact}: {f.detail}", file=sys.stderr)
        return 1

    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))



