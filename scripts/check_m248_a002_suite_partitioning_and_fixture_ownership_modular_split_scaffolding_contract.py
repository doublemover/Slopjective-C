#!/usr/bin/env python3
"""Fail-closed checker for M248-A002 suite partitioning modular split/scaffolding."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m248-a002-suite-partitioning-fixture-ownership-modular-split-scaffolding-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_suite_partitioning_and_fixture_ownership_modular_split_scaffolding_a002_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_a002_suite_partitioning_and_fixture_ownership_modular_split_scaffolding_packet.md"
)
DEFAULT_A001_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_suite_partitioning_and_fixture_ownership_contract_freeze_a001_expectations.md"
)
DEFAULT_A001_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_a001_suite_partitioning_and_fixture_ownership_contract_freeze_packet.md"
)
DEFAULT_A001_CHECKER = ROOT / "scripts" / "check_m248_a001_suite_partitioning_and_fixture_ownership_contract.py"
DEFAULT_A001_TEST = ROOT / "tests" / "tooling" / "test_check_m248_a001_suite_partitioning_and_fixture_ownership_contract.py"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m248/M248-A002/suite_partitioning_and_fixture_ownership_modular_split_scaffolding_contract_summary.json"
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
        "M248-A002-DEP-A001-01",
        Path("docs/contracts/m248_suite_partitioning_and_fixture_ownership_contract_freeze_a001_expectations.md"),
    ),
    AssetCheck(
        "M248-A002-DEP-A001-02",
        Path("spec/planning/compiler/m248/m248_a001_suite_partitioning_and_fixture_ownership_contract_freeze_packet.md"),
    ),
    AssetCheck(
        "M248-A002-DEP-A001-03",
        Path("scripts/check_m248_a001_suite_partitioning_and_fixture_ownership_contract.py"),
    ),
    AssetCheck(
        "M248-A002-DEP-A001-04",
        Path("tests/tooling/test_check_m248_a001_suite_partitioning_and_fixture_ownership_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-A002-DOC-EXP-01",
        "# M248 Suite Partitioning and Fixture Ownership Modular Split/Scaffolding Expectations (A002)",
    ),
    SnippetCheck(
        "M248-A002-DOC-EXP-02",
        "Contract ID: `objc3c-suite-partitioning-and-fixture-ownership-modular-split-scaffolding/m248-a002-v1`",
    ),
    SnippetCheck("M248-A002-DOC-EXP-03", "Dependencies: `M248-A001`"),
    SnippetCheck(
        "M248-A002-DOC-EXP-04",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M248-A002-DOC-EXP-05",
        "docs/contracts/m248_suite_partitioning_and_fixture_ownership_contract_freeze_a001_expectations.md",
    ),
    SnippetCheck(
        "M248-A002-DOC-EXP-06",
        "scripts/check_m248_a001_suite_partitioning_and_fixture_ownership_contract.py",
    ),
    SnippetCheck(
        "M248-A002-DOC-EXP-07",
        "tests/tooling/test_check_m248_a001_suite_partitioning_and_fixture_ownership_contract.py",
    ),
    SnippetCheck("M248-A002-DOC-EXP-08", "`test:objc3c:parser-replay-proof`"),
    SnippetCheck(
        "M248-A002-DOC-EXP-09",
        "`tmp/reports/m248/M248-A002/suite_partitioning_and_fixture_ownership_modular_split_scaffolding_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-A002-DOC-PKT-01",
        "# M248-A002 Suite Partitioning and Fixture Ownership Modular Split/Scaffolding Packet",
    ),
    SnippetCheck("M248-A002-DOC-PKT-02", "Packet: `M248-A002`"),
    SnippetCheck("M248-A002-DOC-PKT-03", "Dependencies: `M248-A001`"),
    SnippetCheck(
        "M248-A002-DOC-PKT-04",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M248-A002-DOC-PKT-05",
        "docs/contracts/m248_suite_partitioning_and_fixture_ownership_modular_split_scaffolding_a002_expectations.md",
    ),
    SnippetCheck(
        "M248-A002-DOC-PKT-06",
        "scripts/check_m248_a002_suite_partitioning_and_fixture_ownership_modular_split_scaffolding_contract.py",
    ),
    SnippetCheck(
        "M248-A002-DOC-PKT-07",
        "tests/tooling/test_check_m248_a002_suite_partitioning_and_fixture_ownership_modular_split_scaffolding_contract.py",
    ),
    SnippetCheck("M248-A002-DOC-PKT-08", "`npm run check:objc3c:m248-a002-lane-a-readiness`"),
    SnippetCheck(
        "M248-A002-DOC-PKT-09",
        "`tmp/reports/m248/M248-A002/suite_partitioning_and_fixture_ownership_modular_split_scaffolding_contract_summary.json`",
    ),
    SnippetCheck(
        "M248-A002-DOC-PKT-10",
        "spec/planning/compiler/m248/m248_a001_suite_partitioning_and_fixture_ownership_contract_freeze_packet.md",
    ),
)

A001_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-A002-A001-DOC-01",
        "# M248 Suite Partitioning and Fixture Ownership Contract Freeze Expectations (A001)",
    ),
    SnippetCheck(
        "M248-A002-A001-DOC-02",
        "Contract ID: `objc3c-suite-partitioning-and-fixture-ownership-contract/m248-a001-v1`",
    ),
    SnippetCheck(
        "M248-A002-A001-DOC-03",
        "optimization improvements as mandatory scope inputs.",
    ),
)

A001_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-A002-A001-PKT-01", "Packet: `M248-A001`"),
    SnippetCheck("M248-A002-A001-PKT-02", "Dependencies: none"),
    SnippetCheck(
        "M248-A002-A001-PKT-03",
        "`tmp/reports/m248/M248-A001/suite_partitioning_and_fixture_ownership_contract_summary.json`",
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
    parser.add_argument("--a001-expectations-doc", type=Path, default=DEFAULT_A001_EXPECTATIONS_DOC)
    parser.add_argument("--a001-packet-doc", type=Path, default=DEFAULT_A001_PACKET_DOC)
    parser.add_argument("--a001-checker", type=Path, default=DEFAULT_A001_CHECKER)
    parser.add_argument("--a001-test", type=Path, default=DEFAULT_A001_TEST)
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
        findings.append(
            Finding(
                display_path(path),
                exists_check_id,
                f"required document is missing: {display_path(path)}",
            )
        )
        return checks_total, findings
    if not path.is_file():
        findings.append(
            Finding(
                display_path(path),
                exists_check_id,
                f"required path is not a file: {display_path(path)}",
            )
        )
        return checks_total, findings

    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(
                Finding(
                    display_path(path),
                    snippet.check_id,
                    f"missing required snippet: {snippet.snippet}",
                )
            )
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    dep_checks, dep_failures = check_prerequisite_assets()
    checks_total += dep_checks
    failures.extend(dep_failures)

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M248-A002-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M248-A002-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.a001_expectations_doc, "M248-A002-A001-DOC-EXISTS", A001_EXPECTATIONS_SNIPPETS),
        (args.a001_packet_doc, "M248-A002-A001-PKT-EXISTS", A001_PACKET_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.a001_checker, "M248-A002-DEP-A001-ARG-01"),
        (args.a001_test, "M248-A002-DEP-A001-ARG-02"),
    ):
        checks_total += 1
        if not path.exists():
            failures.append(
                Finding(
                    display_path(path),
                    check_id,
                    f"required dependency path is missing: {display_path(path)}",
                )
            )
            continue
        if not path.is_file():
            failures.append(
                Finding(
                    display_path(path),
                    check_id,
                    f"required dependency path is not a file: {display_path(path)}",
                )
            )

    checks_passed = checks_total - len(failures)
    summary_payload = {
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
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
