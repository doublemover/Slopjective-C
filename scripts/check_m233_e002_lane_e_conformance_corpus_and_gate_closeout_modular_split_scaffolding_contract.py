#!/usr/bin/env python3
"""Fail-closed checker for M233-E002 conformance corpus and gate closeout modular split/scaffolding freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m233-e002-lane-e-conformance-corpus-gate-closeout-modular-split-scaffolding-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m233_lane_e_conformance_corpus_and_gate_closeout_modular_split_scaffolding_e002_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m233"
    / "m233_e002_lane_e_conformance_corpus_and_gate_closeout_modular_split_scaffolding_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m233/M233-E002/lane_e_conformance_corpus_gate_closeout_modular_split_scaffolding_summary.json"
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
        "M233-E002-DEP-E001-01",
        Path("docs/contracts/m233_lane_e_conformance_corpus_and_gate_closeout_contract_and_architecture_freeze_e001_expectations.md"),
    ),
    AssetCheck(
        "M233-E002-DEP-E001-02",
        Path("spec/planning/compiler/m233/m233_e001_lane_e_conformance_corpus_and_gate_closeout_contract_and_architecture_freeze_packet.md"),
    ),
    AssetCheck(
        "M233-E002-DEP-E001-03",
        Path("scripts/check_m233_e001_lane_e_conformance_corpus_and_gate_closeout_contract_and_architecture_freeze_contract.py"),
    ),
    AssetCheck(
        "M233-E002-DEP-E001-04",
        Path(
            "tests/tooling/"
            "test_check_m233_e001_lane_e_conformance_corpus_and_gate_closeout_contract_and_architecture_freeze_contract.py"
        ),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M233-E002-DOC-EXP-01",
        "# M233 Lane E Conformance Corpus and Gate Closeout Modular Split/Scaffolding Expectations (E002)",
    ),
    SnippetCheck(
        "M233-E002-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-conformance-corpus-gate-closeout-modular-split-scaffolding/m233-e002-v1`",
    ),
    SnippetCheck("M233-E002-DOC-EXP-02A", "Issue: `#5655`"),
    SnippetCheck("M233-E002-DOC-EXP-03", "`M233-E001`"),
    SnippetCheck("M233-E002-DOC-EXP-04", "`M233-A001`"),
    SnippetCheck("M233-E002-DOC-EXP-05", "`M233-B002`"),
    SnippetCheck("M233-E002-DOC-EXP-06", "`M233-C003`"),
    SnippetCheck("M233-E002-DOC-EXP-07", "`M233-D003`"),
    SnippetCheck(
        "M233-E002-DOC-EXP-08",
        "milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M233-E002-DOC-EXP-09",
        "`check:objc3c:m233-e002-lane-e-conformance-corpus-gate-closeout-modular-split-scaffolding-contract`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M233-E002-DOC-PKT-01",
        "# M233-E002 Lane-E Conformance Corpus and Gate Closeout Modular Split/Scaffolding Packet",
    ),
    SnippetCheck("M233-E002-DOC-PKT-02", "Packet: `M233-E002`"),
    SnippetCheck("M233-E002-DOC-PKT-02A", "Issue: `#5655`"),
    SnippetCheck(
        "M233-E002-DOC-PKT-03",
        "Dependencies: `M233-E001`, `M233-A001`, `M233-B002`, `M233-C003`, `M233-D003`",
    ),
    SnippetCheck(
        "M233-E002-DOC-PKT-04",
        "`scripts/check_m233_e002_lane_e_conformance_corpus_and_gate_closeout_modular_split_scaffolding_contract.py`",
    ),
    SnippetCheck(
        "M233-E002-DOC-PKT-05",
        "`tests/tooling/test_check_m233_e002_lane_e_conformance_corpus_and_gate_closeout_modular_split_scaffolding_contract.py`",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M233-E002-ARCH-01",
        "M233 lane-E E002 conformance corpus and gate closeout modular split/scaffolding",
    ),
    SnippetCheck(
        "M233-E002-ARCH-02",
        "`M233-E001`, `M233-A001`, `M233-B002`,",
    ),
    SnippetCheck(
        "M233-E002-ARCH-03",
        "`M233-C003`, and `M233-D003`",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M233-E002-SPC-01",
        "conformance corpus and gate closeout modular split/scaffolding wiring shall preserve",
    ),
    SnippetCheck(
        "M233-E002-SPC-02",
        "`M233-E001`, `M233-A001`, `M233-B002`,",
    ),
    SnippetCheck(
        "M233-E002-SPC-03",
        "`M233-C003`, and `M233-D003`",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M233-E002-META-01",
        "deterministic lane-E conformance corpus and gate closeout modular split/scaffolding dependency anchors for",
    ),
    SnippetCheck(
        "M233-E002-META-02",
        "`M233-E001`, `M233-A001`, `M233-B002`, `M233-C003`, and `M233-D003`",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M233-E002-PKG-01",
        '"check:objc3c:m233-e002-lane-e-conformance-corpus-gate-closeout-modular-split-scaffolding-contract": '
        '"python scripts/check_m233_e002_lane_e_conformance_corpus_and_gate_closeout_modular_split_scaffolding_contract.py"',
    ),
    SnippetCheck(
        "M233-E002-PKG-02",
        '"test:tooling:m233-e002-lane-e-conformance-corpus-gate-closeout-modular-split-scaffolding-contract": '
        '"python -m pytest tests/tooling/test_check_m233_e002_lane_e_conformance_corpus_and_gate_closeout_modular_split_scaffolding_contract.py -q"',
    ),
    SnippetCheck(
        "M233-E002-PKG-03",
        '"check:objc3c:m233-e002-lane-e-readiness": '
        '"npm run check:objc3c:m233-e001-lane-e-readiness '
        '&& npm run check:objc3c:m233-e002-lane-e-conformance-corpus-gate-closeout-modular-split-scaffolding-contract '
        '&& npm run test:tooling:m233-e002-lane-e-conformance-corpus-gate-closeout-modular-split-scaffolding-contract"',
    ),
    SnippetCheck("M233-E002-PKG-04", '"test:objc3c:parser-replay-proof": '),
    SnippetCheck("M233-E002-PKG-05", '"test:objc3c:parser-ast-extraction": '),
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
                    detail=f"missing prerequisite asset: {asset.relative_path.as_posix()}",
                )
            )
            continue
        if not absolute.is_file():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"prerequisite path is not a file: {asset.relative_path.as_posix()}",
                )
            )
    return checks_total, findings


def check_doc_contract(
    *,
    path: Path,
    exists_check_id: str,
    snippets: tuple[SnippetCheck, ...],
) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists():
        findings.append(Finding(display_path(path), exists_check_id, f"required document is missing: {display_path(path)}"))
        return checks_total, findings
    if not path.is_file():
        findings.append(Finding(display_path(path), exists_check_id, f"required path is not a file: {display_path(path)}"))
        return checks_total, findings

    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    dep_checks, dep_failures = check_prerequisite_assets()
    checks_total += dep_checks
    failures.extend(dep_failures)

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M233-E002-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M233-E002-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M233-E002-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M233-E002-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M233-E002-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M233-E002-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

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

