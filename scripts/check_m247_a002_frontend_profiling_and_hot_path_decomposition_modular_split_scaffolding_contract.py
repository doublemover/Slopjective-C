#!/usr/bin/env python3
"""Fail-closed checker for M247-A002 frontend profiling and hot-path decomposition modular split/scaffolding."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-a002-frontend-profiling-hot-path-decomposition-modular-split-scaffolding-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_frontend_profiling_and_hot_path_decomposition_modular_split_scaffolding_a002_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_a002_frontend_profiling_and_hot_path_decomposition_modular_split_scaffolding_packet.md"
)
DEFAULT_A001_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_a001_expectations.md"
)
DEFAULT_A001_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_a001_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_packet.md"
)
DEFAULT_A001_CHECKER = (
    ROOT / "scripts" / "check_m247_a001_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_contract.py"
)
DEFAULT_A001_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_a001_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_contract.py"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-A002/frontend_profiling_and_hot_path_decomposition_modular_split_scaffolding_summary.json"
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
        "M247-A002-DEP-A001-01",
        Path("docs/contracts/m247_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_a001_expectations.md"),
    ),
    AssetCheck(
        "M247-A002-DEP-A001-02",
        Path("spec/planning/compiler/m247/m247_a001_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_packet.md"),
    ),
    AssetCheck(
        "M247-A002-DEP-A001-03",
        Path("scripts/check_m247_a001_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_contract.py"),
    ),
    AssetCheck(
        "M247-A002-DEP-A001-04",
        Path("tests/tooling/test_check_m247_a001_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A002-DOC-EXP-01",
        "# M247 Frontend Profiling and Hot-Path Decomposition Modular Split/Scaffolding Expectations (A002)",
    ),
    SnippetCheck(
        "M247-A002-DOC-EXP-02",
        "Contract ID: `objc3c-frontend-profiling-hot-path-decomposition-modular-split-scaffolding/m247-a002-v1`",
    ),
    SnippetCheck("M247-A002-DOC-EXP-03", "- Issue: `#6709`"),
    SnippetCheck("M247-A002-DOC-EXP-04", "Dependencies: `M247-A001`"),
    SnippetCheck(
        "M247-A002-DOC-EXP-05",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M247-A002-DOC-EXP-06",
        "docs/contracts/m247_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_a001_expectations.md",
    ),
    SnippetCheck(
        "M247-A002-DOC-EXP-07",
        "scripts/check_m247_a001_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_contract.py",
    ),
    SnippetCheck(
        "M247-A002-DOC-EXP-08",
        "tests/tooling/test_check_m247_a001_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_contract.py",
    ),
    SnippetCheck(
        "M247-A002-DOC-EXP-09",
        "`check:objc3c:m247-a002-frontend-profiling-hot-path-decomposition-modular-split-scaffolding-contract`",
    ),
    SnippetCheck(
        "M247-A002-DOC-EXP-10",
        "`python scripts/check_m247_a002_frontend_profiling_and_hot_path_decomposition_modular_split_scaffolding_contract.py --emit-json`",
    ),
    SnippetCheck("M247-A002-DOC-EXP-11", "`compile:objc3c`"),
    SnippetCheck(
        "M247-A002-DOC-EXP-12",
        "`tmp/reports/m247/M247-A002/frontend_profiling_and_hot_path_decomposition_modular_split_scaffolding_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A002-DOC-PKT-01",
        "# M247-A002 Frontend Profiling and Hot-Path Decomposition Modular Split/Scaffolding Packet",
    ),
    SnippetCheck("M247-A002-DOC-PKT-02", "Packet: `M247-A002`"),
    SnippetCheck("M247-A002-DOC-PKT-03", "Issue: `#6709`"),
    SnippetCheck("M247-A002-DOC-PKT-04", "Dependencies: `M247-A001`"),
    SnippetCheck(
        "M247-A002-DOC-PKT-05",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M247-A002-DOC-PKT-06",
        "docs/contracts/m247_frontend_profiling_and_hot_path_decomposition_modular_split_scaffolding_a002_expectations.md",
    ),
    SnippetCheck(
        "M247-A002-DOC-PKT-07",
        "scripts/check_m247_a002_frontend_profiling_and_hot_path_decomposition_modular_split_scaffolding_contract.py",
    ),
    SnippetCheck(
        "M247-A002-DOC-PKT-08",
        "tests/tooling/test_check_m247_a002_frontend_profiling_and_hot_path_decomposition_modular_split_scaffolding_contract.py",
    ),
    SnippetCheck(
        "M247-A002-DOC-PKT-09",
        "python scripts/check_m247_a002_frontend_profiling_and_hot_path_decomposition_modular_split_scaffolding_contract.py --emit-json",
    ),
    SnippetCheck("M247-A002-DOC-PKT-10", "`npm run check:objc3c:m247-a002-lane-a-readiness`"),
    SnippetCheck(
        "M247-A002-DOC-PKT-11",
        "`tmp/reports/m247/M247-A002/frontend_profiling_and_hot_path_decomposition_modular_split_scaffolding_summary.json`",
    ),
    SnippetCheck(
        "M247-A002-DOC-PKT-12",
        "spec/planning/compiler/m247/m247_a001_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_packet.md",
    ),
)

A001_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A002-A001-DOC-01",
        "# M247 Frontend Profiling and Hot-Path Decomposition Contract and Architecture Freeze Expectations (A001)",
    ),
    SnippetCheck(
        "M247-A002-A001-DOC-02",
        "Contract ID: `objc3c-frontend-profiling-hot-path-decomposition-contract-and-architecture-freeze/m247-a001-v1`",
    ),
    SnippetCheck(
        "M247-A002-A001-DOC-03",
        "optimization improvements as mandatory scope inputs.",
    ),
)

A001_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-A002-A001-PKT-01", "Packet: `M247-A001`"),
    SnippetCheck("M247-A002-A001-PKT-02", "Dependencies: none"),
    SnippetCheck(
        "M247-A002-A001-PKT-03",
        "`tmp/reports/m247/M247-A001/frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_summary.json`",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A002-ARCH-01",
        "M247 lane-A A002 frontend profiling and hot-path decomposition modular split/scaffolding anchors",
    ),
    SnippetCheck(
        "M247-A002-ARCH-02",
        "docs/contracts/m247_frontend_profiling_and_hot_path_decomposition_modular_split_scaffolding_a002_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A002-SPC-01",
        "frontend profiling and hot-path decomposition modular split/scaffolding governance shall preserve explicit",
    ),
    SnippetCheck(
        "M247-A002-SPC-02",
        "lane-A dependency anchors (`M247-A001`) and fail closed on scaffolding evidence drift",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A002-META-01",
        "deterministic lane-A frontend profiling and hot-path decomposition modular split metadata anchors for `M247-A002`",
    ),
    SnippetCheck(
        "M247-A002-META-02",
        "explicit `M247-A001` dependency continuity so profiling/hot-path modular split drift fails closed",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A002-PKG-01",
        '"check:objc3c:m247-a002-frontend-profiling-hot-path-decomposition-modular-split-scaffolding-contract": '
        '"python scripts/check_m247_a002_frontend_profiling_and_hot_path_decomposition_modular_split_scaffolding_contract.py"',
    ),
    SnippetCheck(
        "M247-A002-PKG-02",
        '"test:tooling:m247-a002-frontend-profiling-hot-path-decomposition-modular-split-scaffolding-contract": '
        '"python -m pytest tests/tooling/test_check_m247_a002_frontend_profiling_and_hot_path_decomposition_modular_split_scaffolding_contract.py -q"',
    ),
    SnippetCheck(
        "M247-A002-PKG-03",
        '"check:objc3c:m247-a002-lane-a-readiness": "python scripts/run_m247_a002_lane_a_readiness.py"',
    ),
    SnippetCheck("M247-A002-PKG-04", '"compile:objc3c": '),
    SnippetCheck("M247-A002-PKG-05", '"proof:objc3c": '),
    SnippetCheck("M247-A002-PKG-06", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M247-A002-PKG-07", '"test:objc3c:perf-budget": '),
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
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--emit-json", action="store_true", help="Emit canonical summary JSON to stdout.")
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

    try:
        text = path.read_text(encoding="utf-8")
    except OSError as exc:
        findings.append(
            Finding(
                display_path(path),
                exists_check_id,
                f"unable to read required document: {exc}",
            )
        )
        return checks_total, findings

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
        (args.expectations_doc, "M247-A002-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M247-A002-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.a001_expectations_doc, "M247-A002-A001-DOC-EXISTS", A001_EXPECTATIONS_SNIPPETS),
        (args.a001_packet_doc, "M247-A002-A001-PKT-EXISTS", A001_PACKET_SNIPPETS),
        (args.architecture_doc, "M247-A002-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M247-A002-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M247-A002-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M247-A002-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.a001_checker, "M247-A002-DEP-A001-ARG-01"),
        (args.a001_test, "M247-A002-DEP-A001-ARG-02"),
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

    failures = sorted(failures, key=finding_sort_key)
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

    if args.emit_json:
        sys.stdout.write(canonical_json(summary_payload))

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    if not args.emit_json:
        print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

