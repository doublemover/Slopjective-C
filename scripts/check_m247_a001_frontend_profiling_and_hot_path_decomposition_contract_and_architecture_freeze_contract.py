#!/usr/bin/env python3
"""Fail-closed contract checker for the M247-A001 frontend profiling contract freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-a001-frontend-profiling-hot-path-decomposition-contract-and-architecture-freeze-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_a001_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_a001_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_packet.md"
)
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m247_a001_lane_a_readiness.py"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-A001/frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_summary.json"
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
        "M247-A001-DOC-EXP-01",
        "# M247 Frontend Profiling and Hot-Path Decomposition Contract and Architecture Freeze Expectations (A001)",
    ),
    SnippetCheck(
        "M247-A001-DOC-EXP-02",
        "Contract ID: `objc3c-frontend-profiling-hot-path-decomposition-contract-and-architecture-freeze/m247-a001-v1`",
    ),
    SnippetCheck("M247-A001-DOC-EXP-03", "- Issue: `#6708`"),
    SnippetCheck("M247-A001-DOC-EXP-04", "Dependencies: none"),
    SnippetCheck(
        "M247-A001-DOC-EXP-05",
        "optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M247-A001-DOC-EXP-06",
        "`check:objc3c:m247-a001-frontend-profiling-hot-path-decomposition-contract-and-architecture-freeze-contract`",
    ),
    SnippetCheck(
        "M247-A001-DOC-EXP-07",
        "`check:objc3c:m247-a001-lane-a-readiness`",
    ),
    SnippetCheck(
        "M247-A001-DOC-EXP-08",
        "`python scripts/check_m247_a001_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_contract.py --emit-json`",
    ),
    SnippetCheck(
        "M247-A001-DOC-EXP-09",
        "`tmp/reports/m247/M247-A001/frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A001-DOC-PKT-01",
        "# M247-A001 Frontend Profiling and Hot-Path Decomposition Contract and Architecture Freeze Packet",
    ),
    SnippetCheck("M247-A001-DOC-PKT-02", "Packet: `M247-A001`"),
    SnippetCheck("M247-A001-DOC-PKT-03", "Issue: `#6708`"),
    SnippetCheck("M247-A001-DOC-PKT-04", "Dependencies: none"),
    SnippetCheck(
        "M247-A001-DOC-PKT-05",
        "`check:objc3c:m247-a001-frontend-profiling-hot-path-decomposition-contract-and-architecture-freeze-contract`",
    ),
    SnippetCheck("M247-A001-DOC-PKT-06", "`compile:objc3c`"),
    SnippetCheck(
        "M247-A001-DOC-PKT-07",
        "python scripts/check_m247_a001_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_contract.py --emit-json",
    ),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-A001-RUN-01", '"""Run M247-A001 lane-A readiness checks without deep npm nesting."""'),
    SnippetCheck(
        "M247-A001-RUN-02",
        "scripts/check_m247_a001_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_contract.py",
    ),
    SnippetCheck("M247-A001-RUN-03", "--emit-json"),
    SnippetCheck(
        "M247-A001-RUN-04",
        "tests/tooling/test_check_m247_a001_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_contract.py",
    ),
    SnippetCheck("M247-A001-RUN-05", "[ok] M247-A001 lane-A readiness chain completed"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A001-ARCH-01",
        "M247 lane-A A001 frontend profiling and hot-path decomposition contract and architecture freeze anchors",
    ),
    SnippetCheck(
        "M247-A001-ARCH-02",
        "docs/contracts/m247_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_a001_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A001-SPC-01",
        "frontend profiling and hot-path decomposition contract and architecture freeze wiring shall preserve explicit lane-A dependency anchor (`none`)",
    ),
    SnippetCheck(
        "M247-A001-SPC-02",
        "deterministic lane-A parser/AST profiling and hot-path decomposition anchors and fail closed on contract-freeze drift",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A001-META-01",
        "deterministic lane-A frontend profiling and hot-path decomposition contract-freeze metadata anchors for `M247-A001`",
    ),
    SnippetCheck(
        "M247-A001-META-02",
        "parser/AST profiling and hot-path decomposition evidence and compile-time budget continuity",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A001-PKG-01",
        '"check:objc3c:m247-a001-frontend-profiling-hot-path-decomposition-contract-and-architecture-freeze-contract": '
        '"python scripts/check_m247_a001_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_contract.py"',
    ),
    SnippetCheck(
        "M247-A001-PKG-02",
        '"test:tooling:m247-a001-frontend-profiling-hot-path-decomposition-contract-and-architecture-freeze-contract": '
        '"python -m pytest tests/tooling/test_check_m247_a001_frontend_profiling_and_hot_path_decomposition_contract_and_architecture_freeze_contract.py -q"',
    ),
    SnippetCheck(
        "M247-A001-PKG-03",
        '"check:objc3c:m247-a001-lane-a-readiness": "python scripts/run_m247_a001_lane_a_readiness.py"',
    ),
    SnippetCheck("M247-A001-PKG-04", '"compile:objc3c": '),
    SnippetCheck("M247-A001-PKG-05", '"proof:objc3c": '),
    SnippetCheck("M247-A001-PKG-06", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M247-A001-PKG-07", '"test:objc3c:perf-budget": '),
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
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--emit-json", action="store_true", help="Emit canonical summary JSON to stdout.")
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


def finding_sort_key(finding: Finding) -> tuple[str, str, str]:
    return (finding.artifact, finding.check_id, finding.detail)


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M247-A001-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M247-A001-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.readiness_runner, "M247-A001-RUN-EXISTS", READINESS_SNIPPETS),
        (args.architecture_doc, "M247-A001-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M247-A001-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M247-A001-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M247-A001-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    failures = sorted(failures, key=finding_sort_key)
    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in failures
        ],
    }

    summary_path = args.summary_out
    if not summary_path.is_absolute():
        summary_path = ROOT / summary_path
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
