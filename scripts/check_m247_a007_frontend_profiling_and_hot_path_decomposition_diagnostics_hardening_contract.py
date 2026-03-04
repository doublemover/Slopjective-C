#!/usr/bin/env python3
"""Fail-closed checker for M247-A007 frontend profiling/hot-path diagnostics hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-a007-frontend-profiling-hot-path-decomposition-diagnostics-hardening-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_frontend_profiling_and_hot_path_decomposition_diagnostics_hardening_a007_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_a007_frontend_profiling_and_hot_path_decomposition_diagnostics_hardening_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-A007/frontend_profiling_and_hot_path_decomposition_diagnostics_hardening_contract_summary.json"
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


EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A007-DOC-EXP-01",
        "# M247 Frontend Profiling and Hot-Path Decomposition Diagnostics Hardening Expectations (A007)",
    ),
    SnippetCheck(
        "M247-A007-DOC-EXP-02",
        "Contract ID: `objc3c-frontend-profiling-hot-path-decomposition-diagnostics-hardening/m247-a007-v1`",
    ),
    SnippetCheck("M247-A007-DOC-EXP-03", "Issue `#6714` defines canonical lane-A diagnostics hardening scope."),
    SnippetCheck(
        "M247-A007-DOC-EXP-04",
        "Performance profiling and compile-time budgets are mandatory diagnostics",
    ),
    SnippetCheck("M247-A007-DOC-EXP-05", "Dependencies: `M247-A006`"),
    SnippetCheck("M247-A007-DOC-EXP-06", "`M247-A006` remains a mandatory dependency token"),
    SnippetCheck(
        "M247-A007-DOC-EXP-07",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck("M247-A007-DOC-EXP-08", "`check:objc3c:m247-a007-lane-a-readiness`"),
    SnippetCheck(
        "M247-A007-DOC-EXP-09",
        "`tmp/reports/m247/M247-A007/frontend_profiling_and_hot_path_decomposition_diagnostics_hardening_contract_summary.json`",
    ),
    SnippetCheck("M247-A007-DOC-EXP-10", "`npm run check:objc3c:m247-a006-lane-a-readiness`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A007-DOC-PKT-01",
        "# M247-A007 Frontend Profiling and Hot-Path Decomposition Diagnostics Hardening Packet",
    ),
    SnippetCheck("M247-A007-DOC-PKT-02", "Packet: `M247-A007`"),
    SnippetCheck("M247-A007-DOC-PKT-03", "Issue: `#6714`"),
    SnippetCheck("M247-A007-DOC-PKT-04", "Dependencies: `M247-A006`"),
    SnippetCheck("M247-A007-DOC-PKT-05", "Freeze date: `2026-03-04`"),
    SnippetCheck(
        "M247-A007-DOC-PKT-06",
        "check:objc3c:m247-a007-frontend-profiling-hot-path-decomposition-diagnostics-hardening-contract",
    ),
    SnippetCheck("M247-A007-DOC-PKT-07", "check:objc3c:m247-a007-lane-a-readiness"),
    SnippetCheck(
        "M247-A007-DOC-PKT-08",
        "optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M247-A007-DOC-PKT-09",
        "compile-time budget evidence remain mandatory diagnostics hardening inputs.",
    ),
    SnippetCheck(
        "M247-A007-DOC-PKT-10",
        "tmp/reports/m247/M247-A007/frontend_profiling_and_hot_path_decomposition_diagnostics_hardening_contract_summary.json",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A007-ARCH-01",
        "M247 lane-A A007 frontend profiling and hot-path decomposition diagnostics hardening anchors",
    ),
    SnippetCheck(
        "M247-A007-ARCH-02",
        "m247_frontend_profiling_and_hot_path_decomposition_diagnostics_hardening_a007_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A007-SPC-01",
        "frontend profiling and hot-path decomposition diagnostics hardening wiring",
    ),
    SnippetCheck(
        "M247-A007-SPC-02",
        "lane-A dependency anchor (`M247-A006`) and fail closed",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A007-META-01",
        "frontend profiling and hot-path decomposition",
    ),
    SnippetCheck(
        "M247-A007-META-02",
        "diagnostics-hardening metadata anchors for `M247-A007`",
    ),
    SnippetCheck("M247-A007-META-03", "`M247-A006` dependency continuity"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A007-PKG-01",
        '"check:objc3c:m247-a007-frontend-profiling-hot-path-decomposition-diagnostics-hardening-contract": '
        '"python scripts/check_m247_a007_frontend_profiling_and_hot_path_decomposition_diagnostics_hardening_contract.py"',
    ),
    SnippetCheck(
        "M247-A007-PKG-02",
        '"test:tooling:m247-a007-frontend-profiling-hot-path-decomposition-diagnostics-hardening-contract": '
        '"python -m pytest tests/tooling/test_check_m247_a007_frontend_profiling_and_hot_path_decomposition_diagnostics_hardening_contract.py -q"',
    ),
    SnippetCheck(
        "M247-A007-PKG-03",
        '"check:objc3c:m247-a007-lane-a-readiness": '
        '"npm run check:objc3c:m247-a006-lane-a-readiness '
        "&& npm run check:objc3c:m247-a007-frontend-profiling-hot-path-decomposition-diagnostics-hardening-contract "
        '&& npm run test:tooling:m247-a007-frontend-profiling-hot-path-decomposition-diagnostics-hardening-contract"',
    ),
    SnippetCheck("M247-A007-PKG-04", '"compile:objc3c": '),
    SnippetCheck("M247-A007-PKG-05", '"test:objc3c:perf-budget": '),
    SnippetCheck("M247-A007-PKG-06", '"test:objc3c:parser-replay-proof": '),
    SnippetCheck("M247-A007-PKG-07", '"test:objc3c:parser-ast-extraction": '),
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
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required document is missing: {display_path(path)}",
            )
        )
        return checks_total, findings
    if not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required path is not a file: {display_path(path)}",
            )
        )
        return checks_total, findings

    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(
                Finding(
                    artifact=display_path(path),
                    check_id=snippet.check_id,
                    detail=f"missing required snippet: {snippet.snippet}",
                )
            )
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M247-A007-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M247-A007-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M247-A007-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M247-A007-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M247-A007-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M247-A007-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        check_count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += check_count
        failures.extend(findings)

    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in failures
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
