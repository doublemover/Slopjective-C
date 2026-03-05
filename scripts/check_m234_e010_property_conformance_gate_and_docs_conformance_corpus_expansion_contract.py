#!/usr/bin/env python3
"""Fail-closed checker for M234-E010 property conformance gate and docs conformance corpus expansion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m234-e010-property-conformance-gate-docs-conformance-corpus-expansion-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m234_property_conformance_gate_and_docs_conformance_corpus_expansion_e010_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m234"
    / "m234_e010_property_conformance_gate_and_docs_conformance_corpus_expansion_packet.md"
)
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m234/M234-E010/property_conformance_gate_docs_conformance_corpus_expansion_summary.json"
)


@dataclass(frozen=True)
class AssetCheck:
    check_id: str
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
        "M234-E010-DEP-E009-01",
        Path("docs/contracts/m234_property_conformance_gate_and_docs_conformance_matrix_implementation_e009_expectations.md"),
    ),
    AssetCheck(
        "M234-E010-DEP-E009-02",
        Path("spec/planning/compiler/m234/m234_e009_property_conformance_gate_and_docs_conformance_matrix_implementation_packet.md"),
    ),
    AssetCheck(
        "M234-E010-DEP-E009-03",
        Path("scripts/check_m234_e009_property_conformance_gate_and_docs_conformance_matrix_implementation_contract.py"),
    ),
    AssetCheck(
        "M234-E010-DEP-E009-04",
        Path("tests/tooling/test_check_m234_e009_property_conformance_gate_and_docs_conformance_matrix_implementation_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-E010-DOC-EXP-01",
        "# M234 Property Conformance Gate and Docs Conformance Corpus Expansion Expectations (E010)",
    ),
    SnippetCheck(
        "M234-E010-DOC-EXP-02",
        "Contract ID: `objc3c-property-conformance-gate-docs-conformance-corpus-expansion/m234-e010-v1`",
    ),
    SnippetCheck(
        "M234-E010-DOC-EXP-03",
        "Issue `#5757` defines canonical lane-E conformance corpus expansion scope.",
    ),
    SnippetCheck(
        "M234-E010-DOC-EXP-04",
        "Dependencies: `M234-E009`, `M234-A010`, `M234-B011`, `M234-C011`, `M234-D008`",
    ),
    SnippetCheck(
        "M234-E010-DOC-EXP-05",
        "docs/contracts/m234_property_conformance_gate_and_docs_conformance_matrix_implementation_e009_expectations.md",
    ),
    SnippetCheck(
        "M234-E010-DOC-EXP-06",
        "`M234-A010`",
    ),
    SnippetCheck(
        "M234-E010-DOC-EXP-07",
        "`M234-B011`",
    ),
    SnippetCheck(
        "M234-E010-DOC-EXP-08",
        "`M234-C011`",
    ),
    SnippetCheck(
        "M234-E010-DOC-EXP-09",
        "`M234-D008`",
    ),
    SnippetCheck(
        "M234-E010-DOC-EXP-10",
        "conformance corpus expansion traceability, and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M234-E010-DOC-EXP-11",
        "Predecessor anchor: `M234-E009` conformance matrix implementation continuity is the mandatory baseline for E010.",
    ),
    SnippetCheck(
        "M234-E010-DOC-EXP-12",
        "`scripts/check_m234_e010_property_conformance_gate_and_docs_conformance_corpus_expansion_contract.py`",
    ),
    SnippetCheck(
        "M234-E010-DOC-EXP-13",
        "`tests/tooling/test_check_m234_e010_property_conformance_gate_and_docs_conformance_corpus_expansion_contract.py`",
    ),
    SnippetCheck(
        "M234-E010-DOC-EXP-14",
        "`tmp/reports/m234/M234-E010/property_conformance_gate_docs_conformance_corpus_expansion_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M234-E010-DOC-PKT-01",
        "# M234-E010 Property Conformance Gate and Docs Conformance Corpus Expansion Packet",
    ),
    SnippetCheck("M234-E010-DOC-PKT-02", "Packet: `M234-E010`"),
    SnippetCheck("M234-E010-DOC-PKT-03", "Issue: `#5757`"),
    SnippetCheck(
        "M234-E010-DOC-PKT-04",
        "Dependencies: `M234-E009`, `M234-A010`, `M234-B011`, `M234-C011`, `M234-D008`",
    ),
    SnippetCheck("M234-E010-DOC-PKT-05", "Predecessor: `M234-E009`"),
    SnippetCheck("M234-E010-DOC-PKT-06", "Theme: conformance corpus expansion"),
    SnippetCheck(
        "M234-E010-DOC-PKT-07",
        "docs/contracts/m234_property_conformance_gate_and_docs_conformance_corpus_expansion_e010_expectations.md",
    ),
    SnippetCheck(
        "M234-E010-DOC-PKT-08",
        "scripts/check_m234_e010_property_conformance_gate_and_docs_conformance_corpus_expansion_contract.py",
    ),
    SnippetCheck(
        "M234-E010-DOC-PKT-09",
        "tests/tooling/test_check_m234_e010_property_conformance_gate_and_docs_conformance_corpus_expansion_contract.py",
    ),
    SnippetCheck(
        "M234-E010-DOC-PKT-10",
        "spec/planning/compiler/m234/m234_e009_property_conformance_gate_and_docs_conformance_matrix_implementation_packet.md",
    ),
    SnippetCheck(
        "M234-E010-DOC-PKT-11",
        "conformance corpus expansion traceability, and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M234-E010-DOC-PKT-12",
        "tmp/reports/m234/M234-E010/property_conformance_gate_docs_conformance_corpus_expansion_summary.json",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M234-E010-PKG-01", '"compile:objc3c": '),
    SnippetCheck("M234-E010-PKG-02", '"proof:objc3c": '),
    SnippetCheck("M234-E010-PKG-03", '"test:objc3c:diagnostics-replay-proof": '),
    SnippetCheck("M234-E010-PKG-04", '"test:objc3c:execution-replay-proof": '),
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
    failures: list[Finding] = []
    for asset in PREREQUISITE_ASSETS:
        checks_total += 1
        absolute = ROOT / asset.relative_path
        if not absolute.exists():
            failures.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"missing prerequisite asset: {asset.relative_path.as_posix()}",
                )
            )
            continue
        if not absolute.is_file():
            failures.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"prerequisite path is not a file: {asset.relative_path.as_posix()}",
                )
            )
    return checks_total, failures


def check_doc_contract(
    *,
    path: Path,
    exists_check_id: str,
    snippets: tuple[SnippetCheck, ...],
) -> tuple[int, list[Finding]]:
    checks_total = 1
    failures: list[Finding] = []
    if not path.exists():
        failures.append(Finding(display_path(path), exists_check_id, f"required document is missing: {display_path(path)}"))
        return checks_total, failures
    if not path.is_file():
        failures.append(Finding(display_path(path), exists_check_id, f"required path is not a file: {display_path(path)}"))
        return checks_total, failures

    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, failures


def finding_sort_key(finding: Finding) -> tuple[str, str, str]:
    return (finding.artifact, finding.check_id, finding.detail)


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    dep_checks, dep_failures = check_prerequisite_assets()
    checks_total += dep_checks
    failures.extend(dep_failures)

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M234-E010-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M234-E010-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.package_json, "M234-E010-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, finding_batch = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(finding_batch)

    failures = sorted(failures, key=finding_sort_key)
    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [{"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail} for f in failures],
    }

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
