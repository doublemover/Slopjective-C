#!/usr/bin/env python3
"""Fail-closed checker for M230-A006 conformance corpus governance/sharding edge-case expansion and robustness."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m230-a006-conformance-corpus-governance-and-sharding-edge-case-expansion-and-robustness-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m230_conformance_corpus_governance_and_sharding_edge_case_expansion_and_robustness_a006_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m230"
    / "m230_a006_conformance_corpus_governance_and_sharding_edge_case_expansion_and_robustness_packet.md"
)
DEFAULT_A005_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m230_conformance_corpus_governance_and_sharding_edge_case_and_compatibility_completion_a005_expectations.md"
)
DEFAULT_A005_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m230"
    / "m230_a005_conformance_corpus_governance_and_sharding_edge_case_and_compatibility_completion_packet.md"
)
DEFAULT_A005_CHECKER = (
    ROOT / "scripts" / "check_m230_a005_conformance_corpus_governance_and_sharding_edge_case_and_compatibility_completion_contract.py"
)
DEFAULT_A005_TEST = (
    ROOT / "tests" / "tooling" / "test_check_m230_a005_conformance_corpus_governance_and_sharding_edge_case_and_compatibility_completion_contract.py"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m230/M230-A006/conformance_corpus_governance_and_sharding_edge_case_expansion_and_robustness_summary.json"
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
        "M230-A006-DEP-A005-01",
        Path("docs/contracts/m230_conformance_corpus_governance_and_sharding_edge_case_and_compatibility_completion_a005_expectations.md"),
    ),
    (
        "M230-A006-DEP-A005-02",
        Path("spec/planning/compiler/m230/m230_a005_conformance_corpus_governance_and_sharding_edge_case_and_compatibility_completion_packet.md"),
    ),
    (
        "M230-A006-DEP-A005-03",
        Path("scripts/check_m230_a005_conformance_corpus_governance_and_sharding_edge_case_and_compatibility_completion_contract.py"),
    ),
    (
        "M230-A006-DEP-A005-04",
        Path("tests/tooling/test_check_m230_a005_conformance_corpus_governance_and_sharding_edge_case_and_compatibility_completion_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M230-A006-DOC-EXP-01",
        "# M230 Conformance Corpus Governance and Sharding Edge-Case Expansion and Robustness Expectations (A006)",
    ),
    SnippetCheck(
        "M230-A006-DOC-EXP-02",
        "Contract ID: `objc3c-conformance-corpus-governance-and-sharding-edge-case-expansion-and-robustness/m230-a006-v1`",
    ),
    SnippetCheck("M230-A006-DOC-EXP-03", "Dependencies: `M230-A005`"),
    SnippetCheck(
        "M230-A006-DOC-EXP-04",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M230-A006-DOC-EXP-05",
        "check:objc3c:m230-a006-conformance-corpus-governance-and-sharding-edge-case-expansion-and-robustness-contract",
    ),
    SnippetCheck(
        "M230-A006-DOC-EXP-06",
        "check:objc3c:m230-a006-lane-a-readiness",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M230-A006-DOC-PKT-01",
        "# M230-A006 Conformance Corpus Governance and Sharding Edge-Case Expansion and Robustness Packet",
    ),
    SnippetCheck("M230-A006-DOC-PKT-02", "Packet: `M230-A006`"),
    SnippetCheck("M230-A006-DOC-PKT-03", "Dependencies: `M230-A005`"),
    SnippetCheck(
        "M230-A006-DOC-PKT-04",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M230-A006-DOC-PKT-05",
        "spec/planning/compiler/m230/m230_a005_conformance_corpus_governance_and_sharding_edge_case_and_compatibility_completion_packet.md",
    ),
)

A005_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M230-A006-A005-EXP-01",
        "# M230 Conformance Corpus Governance and Sharding Edge-Case and Compatibility Completion Expectations (A005)",
    ),
    SnippetCheck(
        "M230-A006-A005-EXP-02",
        "Contract ID: `objc3c-conformance-corpus-governance-and-sharding-edge-case-and-compatibility-completion/m230-a005-v1`",
    ),
)

A005_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M230-A006-A005-PKT-01", "Packet: `M230-A005`"),
    SnippetCheck("M230-A006-A005-PKT-02", "Dependencies: `M230-A004`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M230-A006-ARCH-01",
        "M230 lane-A A006 conformance corpus governance and sharding edge-case expansion and robustness anchors",
    ),
    SnippetCheck(
        "M230-A006-ARCH-02",
        "m230_conformance_corpus_governance_and_sharding_edge_case_expansion_and_robustness_a006_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M230-A006-SPC-01",
        "conformance corpus governance and sharding edge-case expansion and robustness governance shall preserve explicit",
    ),
    SnippetCheck(
        "M230-A006-SPC-02",
        "lane-A dependency anchors (`M230-A005`) and fail closed on edge-case-expansion-and-robustness evidence drift",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M230-A006-META-01",
        "deterministic lane-A conformance corpus governance and sharding edge-case expansion and robustness anchors for `M230-A006`",
    ),
    SnippetCheck(
        "M230-A006-META-02",
        "explicit `M230-A005` dependency continuity so conformance corpus governance/sharding edge-case-expansion-and-robustness drift fails closed",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M230-A006-PKG-01",
        '"check:objc3c:m230-a006-conformance-corpus-governance-and-sharding-edge-case-expansion-and-robustness-contract": '
        '"python scripts/check_m230_a006_conformance_corpus_governance_and_sharding_edge_case_expansion_and_robustness_contract.py"',
    ),
    SnippetCheck(
        "M230-A006-PKG-02",
        '"test:tooling:m230-a006-conformance-corpus-governance-and-sharding-edge-case-expansion-and-robustness-contract": '
        '"python -m pytest tests/tooling/test_check_m230_a006_conformance_corpus_governance_and_sharding_edge_case_expansion_and_robustness_contract.py -q"',
    ),
    SnippetCheck(
        "M230-A006-PKG-03",
        '"check:objc3c:m230-a006-lane-a-readiness": '
        '"npm run check:objc3c:m230-a005-lane-a-readiness '
        '&& npm run check:objc3c:m230-a006-conformance-corpus-governance-and-sharding-edge-case-expansion-and-robustness-contract '
        '&& npm run test:tooling:m230-a006-conformance-corpus-governance-and-sharding-edge-case-expansion-and-robustness-contract"',
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
    parser.add_argument("--a005-expectations-doc", type=Path, default=DEFAULT_A005_EXPECTATIONS_DOC)
    parser.add_argument("--a005-packet-doc", type=Path, default=DEFAULT_A005_PACKET_DOC)
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
        (args.expectations_doc, "M230-A006-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M230-A006-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.a005_expectations_doc, "M230-A006-A005-EXP-EXISTS", A005_EXPECTATIONS_SNIPPETS),
        (args.a005_packet_doc, "M230-A006-A005-PKT-EXISTS", A005_PACKET_SNIPPETS),
        (args.architecture_doc, "M230-A006-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M230-A006-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M230-A006-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M230-A006-PKG-EXISTS", PACKAGE_SNIPPETS),
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



