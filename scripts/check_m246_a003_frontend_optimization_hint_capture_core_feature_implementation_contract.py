#!/usr/bin/env python3
"""Fail-closed checker for M246-A003 frontend optimization hint core feature implementation."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m246-a003-frontend-optimization-hint-capture-core-feature-implementation-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_frontend_optimization_hint_capture_core_feature_implementation_a003_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_a003_frontend_optimization_hint_capture_core_feature_implementation_packet.md"
)
DEFAULT_A002_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_frontend_optimization_hint_capture_modular_split_scaffolding_a002_expectations.md"
)
DEFAULT_A002_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_a002_frontend_optimization_hint_capture_modular_split_scaffolding_packet.md"
)
DEFAULT_A002_CHECKER = (
    ROOT / "scripts" / "check_m246_a002_frontend_optimization_hint_capture_modular_split_scaffolding_contract.py"
)
DEFAULT_A002_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m246_a002_frontend_optimization_hint_capture_modular_split_scaffolding_contract.py"
)
DEFAULT_RUN_SCRIPT = ROOT / "scripts" / "run_m246_a003_lane_a_readiness.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m246/M246-A003/frontend_optimization_hint_capture_core_feature_implementation_summary.json"
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
        "M246-A003-DEP-A002-01",
        Path("docs/contracts/m246_frontend_optimization_hint_capture_modular_split_scaffolding_a002_expectations.md"),
    ),
    AssetCheck(
        "M246-A003-DEP-A002-02",
        Path("spec/planning/compiler/m246/m246_a002_frontend_optimization_hint_capture_modular_split_scaffolding_packet.md"),
    ),
    AssetCheck(
        "M246-A003-DEP-A002-03",
        Path("scripts/check_m246_a002_frontend_optimization_hint_capture_modular_split_scaffolding_contract.py"),
    ),
    AssetCheck(
        "M246-A003-DEP-A002-04",
        Path("tests/tooling/test_check_m246_a002_frontend_optimization_hint_capture_modular_split_scaffolding_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-A003-DOC-EXP-01",
        "# M246 Frontend Optimization Hint Capture Core Feature Implementation Expectations (A003)",
    ),
    SnippetCheck(
        "M246-A003-DOC-EXP-02",
        "Contract ID: `objc3c-frontend-optimization-hint-capture-core-feature-implementation/m246-a003-v1`",
    ),
    SnippetCheck(
        "M246-A003-DOC-EXP-03",
        "Issue `#5050` defines canonical lane-A core feature implementation scope.",
    ),
    SnippetCheck("M246-A003-DOC-EXP-04", "Dependencies: `M246-A002`"),
    SnippetCheck(
        "M246-A003-DOC-EXP-05",
        "optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M246-A003-DOC-EXP-06",
        "docs/contracts/m246_frontend_optimization_hint_capture_modular_split_scaffolding_a002_expectations.md",
    ),
    SnippetCheck(
        "M246-A003-DOC-EXP-07",
        "scripts/check_m246_a002_frontend_optimization_hint_capture_modular_split_scaffolding_contract.py",
    ),
    SnippetCheck("M246-A003-DOC-EXP-08", "`python scripts/run_m246_a003_lane_a_readiness.py`"),
    SnippetCheck(
        "M246-A003-DOC-EXP-09",
        "`tmp/reports/m246/M246-A003/frontend_optimization_hint_capture_core_feature_implementation_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-A003-DOC-PKT-01",
        "# M246-A003 Frontend Optimization Hint Capture Core Feature Implementation Packet",
    ),
    SnippetCheck("M246-A003-DOC-PKT-02", "Packet: `M246-A003`"),
    SnippetCheck("M246-A003-DOC-PKT-03", "Wave: `W38`"),
    SnippetCheck("M246-A003-DOC-PKT-04", "Issue: `#5050`"),
    SnippetCheck("M246-A003-DOC-PKT-05", "Dependencies: `M246-A002`"),
    SnippetCheck(
        "M246-A003-DOC-PKT-06",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M246-A003-DOC-PKT-07",
        "scripts/check_m246_a003_frontend_optimization_hint_capture_core_feature_implementation_contract.py",
    ),
    SnippetCheck(
        "M246-A003-DOC-PKT-08",
        "tests/tooling/test_check_m246_a003_frontend_optimization_hint_capture_core_feature_implementation_contract.py",
    ),
    SnippetCheck("M246-A003-DOC-PKT-09", "`python scripts/run_m246_a003_lane_a_readiness.py`"),
    SnippetCheck(
        "M246-A003-DOC-PKT-10",
        "`tmp/reports/m246/M246-A003/frontend_optimization_hint_capture_core_feature_implementation_summary.json`",
    ),
)

A002_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-A003-A002-DOC-01",
        "# M246 Frontend Optimization Hint Capture Modular Split/Scaffolding Expectations (A002)",
    ),
    SnippetCheck(
        "M246-A003-A002-DOC-02",
        "Contract ID: `objc3c-frontend-optimization-hint-capture-modular-split-scaffolding/m246-a002-v1`",
    ),
    SnippetCheck("M246-A003-A002-DOC-03", "Dependencies: `M246-A001`"),
)

A002_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-A003-A002-PKT-01", "Packet: `M246-A002`"),
    SnippetCheck("M246-A003-A002-PKT-02", "Dependencies: `M246-A001`"),
)

RUN_SCRIPT_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-A003-RUN-01", "check:objc3c:m246-a002-lane-a-readiness"),
    SnippetCheck(
        "M246-A003-RUN-02",
        "scripts/check_m246_a003_frontend_optimization_hint_capture_core_feature_implementation_contract.py",
    ),
    SnippetCheck(
        "M246-A003-RUN-03",
        "tests/tooling/test_check_m246_a003_frontend_optimization_hint_capture_core_feature_implementation_contract.py",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-A003-PKG-01", '"check:objc3c:m246-a002-lane-a-readiness": '),
    SnippetCheck("M246-A003-PKG-02", '"compile:objc3c": '),
    SnippetCheck("M246-A003-PKG-03", '"proof:objc3c": '),
    SnippetCheck("M246-A003-PKG-04", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M246-A003-PKG-05", '"test:objc3c:perf-budget": '),
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
    parser.add_argument("--a002-expectations-doc", type=Path, default=DEFAULT_A002_EXPECTATIONS_DOC)
    parser.add_argument("--a002-packet-doc", type=Path, default=DEFAULT_A002_PACKET_DOC)
    parser.add_argument("--a002-checker", type=Path, default=DEFAULT_A002_CHECKER)
    parser.add_argument("--a002-test", type=Path, default=DEFAULT_A002_TEST)
    parser.add_argument("--run-script", type=Path, default=DEFAULT_RUN_SCRIPT)
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
        (args.expectations_doc, "M246-A003-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M246-A003-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.a002_expectations_doc, "M246-A003-A002-DOC-EXISTS", A002_EXPECTATIONS_SNIPPETS),
        (args.a002_packet_doc, "M246-A003-A002-PKT-EXISTS", A002_PACKET_SNIPPETS),
        (args.run_script, "M246-A003-RUN-EXISTS", RUN_SCRIPT_SNIPPETS),
        (args.package_json, "M246-A003-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.a002_checker, "M246-A003-DEP-A002-ARG-01"),
        (args.a002_test, "M246-A003-DEP-A002-ARG-02"),
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
