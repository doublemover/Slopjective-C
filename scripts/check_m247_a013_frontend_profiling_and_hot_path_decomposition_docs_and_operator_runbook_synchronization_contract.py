#!/usr/bin/env python3
"""Fail-closed contract checker for M247-A013 frontend profiling docs and operator runbook synchronization."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-a013-frontend-profiling-hot-path-decomposition-docs-operator-runbook-synchronization-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_frontend_profiling_and_hot_path_decomposition_docs_and_operator_runbook_synchronization_a013_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_a013_frontend_profiling_and_hot_path_decomposition_docs_and_operator_runbook_synchronization_packet.md"
)
DEFAULT_A012_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_frontend_profiling_and_hot_path_decomposition_cross_lane_integration_sync_a012_expectations.md"
)
DEFAULT_A012_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_a012_frontend_profiling_and_hot_path_decomposition_cross_lane_integration_sync_packet.md"
)
DEFAULT_A012_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_a012_frontend_profiling_and_hot_path_decomposition_cross_lane_integration_sync_contract.py"
)
DEFAULT_A012_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_a012_frontend_profiling_and_hot_path_decomposition_cross_lane_integration_sync_contract.py"
)
DEFAULT_A012_READINESS_RUNNER = ROOT / "scripts" / "run_m247_a012_lane_a_readiness.py"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m247_a013_lane_a_readiness.py"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-A013/frontend_profiling_and_hot_path_decomposition_docs_and_operator_runbook_synchronization_summary.json"
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
        "M247-A013-DOC-EXP-01",
        "# M247 Frontend Profiling and Hot-Path Decomposition Docs and Operator Runbook Synchronization Expectations (A013)",
    ),
    SnippetCheck(
        "M247-A013-DOC-EXP-02",
        "Contract ID: `objc3c-frontend-profiling-hot-path-decomposition-docs-operator-runbook-synchronization/m247-a013-v1`",
    ),
    SnippetCheck("M247-A013-DOC-EXP-03", "Dependencies: `M247-A012`"),
    SnippetCheck("M247-A013-DOC-EXP-04", "Issue `#6720` defines canonical lane-A docs and operator runbook synchronization scope."),
    SnippetCheck("M247-A013-DOC-EXP-05", "docs_runbook_sync_consistent"),
    SnippetCheck("M247-A013-DOC-EXP-06", "docs_runbook_sync_ready"),
    SnippetCheck("M247-A013-DOC-EXP-07", "docs_runbook_sync_key_ready"),
    SnippetCheck("M247-A013-DOC-EXP-08", "docs_runbook_sync_key"),
    SnippetCheck("M247-A013-DOC-EXP-09", "`M247-B013`"),
    SnippetCheck("M247-A013-DOC-EXP-10", "`M247-C013`"),
    SnippetCheck("M247-A013-DOC-EXP-11", "`M247-D013`"),
    SnippetCheck("M247-A013-DOC-EXP-12", "`M247-E013`"),
    SnippetCheck(
        "M247-A013-DOC-EXP-13",
        "`scripts/check_m247_a012_frontend_profiling_and_hot_path_decomposition_cross_lane_integration_sync_contract.py`",
    ),
    SnippetCheck(
        "M247-A013-DOC-EXP-14",
        "`check:objc3c:m247-a013-frontend-profiling-hot-path-decomposition-docs-operator-runbook-synchronization-contract`",
    ),
    SnippetCheck(
        "M247-A013-DOC-EXP-15",
        "`tmp/reports/m247/M247-A013/frontend_profiling_and_hot_path_decomposition_docs_and_operator_runbook_synchronization_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-A013-DOC-PKT-01", "Packet: `M247-A013`"),
    SnippetCheck("M247-A013-DOC-PKT-02", "Issue: `#6720`"),
    SnippetCheck("M247-A013-DOC-PKT-03", "Dependencies: `M247-A012`"),
    SnippetCheck(
        "M247-A013-DOC-PKT-04",
        "`scripts/check_m247_a013_frontend_profiling_and_hot_path_decomposition_docs_and_operator_runbook_synchronization_contract.py`",
    ),
    SnippetCheck("M247-A013-DOC-PKT-05", "`M247-B013`"),
    SnippetCheck("M247-A013-DOC-PKT-06", "`M247-C013`"),
    SnippetCheck("M247-A013-DOC-PKT-07", "`M247-D013`"),
    SnippetCheck("M247-A013-DOC-PKT-08", "`M247-E013`"),
    SnippetCheck(
        "M247-A013-DOC-PKT-09",
        "tmp/reports/m247/M247-A013/frontend_profiling_and_hot_path_decomposition_docs_and_operator_runbook_synchronization_summary.json",
    ),
)

A012_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A013-A012-DOC-01",
        "Contract ID: `objc3c-frontend-profiling-hot-path-decomposition-cross-lane-integration-sync/m247-a012-v1`",
    ),
    SnippetCheck("M247-A013-A012-DOC-02", "Dependencies: `M247-A011`"),
)

A012_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-A013-A012-PKT-01", "Packet: `M247-A012`"),
    SnippetCheck("M247-A013-A012-PKT-02", "Issue: `#6719`"),
    SnippetCheck("M247-A013-A012-PKT-03", "Dependencies: `M247-A011`"),
)

READINESS_RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A013-RUN-01",
        '"""Run M247-A013 lane-A readiness checks without deep npm nesting."""',
    ),
    SnippetCheck("M247-A013-RUN-02", "scripts/run_m247_a012_lane_a_readiness.py"),
    SnippetCheck(
        "M247-A013-RUN-03",
        "scripts/check_m247_a013_frontend_profiling_and_hot_path_decomposition_docs_and_operator_runbook_synchronization_contract.py",
    ),
    SnippetCheck(
        "M247-A013-RUN-04",
        "tests/tooling/test_check_m247_a013_frontend_profiling_and_hot_path_decomposition_docs_and_operator_runbook_synchronization_contract.py",
    ),
    SnippetCheck("M247-A013-RUN-05", "[ok] M247-A013 lane-A readiness chain completed"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A013-ARCH-01",
        "M247 lane-A A013 frontend profiling and hot-path decomposition docs and operator runbook synchronization anchors",
    ),
    SnippetCheck(
        "M247-A013-ARCH-02",
        "m247_frontend_profiling_and_hot_path_decomposition_docs_and_operator_runbook_synchronization_a013_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A013-SPC-01",
        "frontend profiling and hot-path decomposition docs and operator runbook synchronization wiring",
    ),
    SnippetCheck(
        "M247-A013-SPC-02",
        "lane-A dependency anchor (`M247-A012`) and fail closed",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A013-META-01",
        "docs and operator runbook synchronization metadata anchors for `M247-A013`",
    ),
    SnippetCheck("M247-A013-META-02", "`M247-A012` dependency continuity"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A013-PKG-01",
        '"check:objc3c:m247-a013-frontend-profiling-hot-path-decomposition-docs-operator-runbook-synchronization-contract": '
        '"python scripts/check_m247_a013_frontend_profiling_and_hot_path_decomposition_docs_and_operator_runbook_synchronization_contract.py"',
    ),
    SnippetCheck(
        "M247-A013-PKG-02",
        '"test:tooling:m247-a013-frontend-profiling-hot-path-decomposition-docs-operator-runbook-synchronization-contract": '
        '"python -m pytest tests/tooling/test_check_m247_a013_frontend_profiling_and_hot_path_decomposition_docs_and_operator_runbook_synchronization_contract.py -q"',
    ),
    SnippetCheck(
        "M247-A013-PKG-03",
        '"check:objc3c:m247-a013-lane-a-readiness": "python scripts/run_m247_a013_lane_a_readiness.py"',
    ),
    SnippetCheck("M247-A013-PKG-04", '"compile:objc3c": '),
    SnippetCheck("M247-A013-PKG-05", '"test:objc3c:perf-budget": '),
    SnippetCheck("M247-A013-PKG-06", '"test:objc3c:parser-replay-proof": '),
    SnippetCheck("M247-A013-PKG-07", '"test:objc3c:parser-ast-extraction": '),
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
    parser.add_argument("--a012-expectations-doc", type=Path, default=DEFAULT_A012_EXPECTATIONS_DOC)
    parser.add_argument("--a012-packet-doc", type=Path, default=DEFAULT_A012_PACKET_DOC)
    parser.add_argument("--a012-checker", type=Path, default=DEFAULT_A012_CHECKER)
    parser.add_argument("--a012-test", type=Path, default=DEFAULT_A012_TEST)
    parser.add_argument("--a012-readiness-runner", type=Path, default=DEFAULT_A012_READINESS_RUNNER)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_doc_contract(*, path: Path, exists_check_id: str, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
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
        (args.expectations_doc, "M247-A013-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M247-A013-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.a012_expectations_doc, "M247-A013-A012-DOC-EXISTS", A012_EXPECTATIONS_SNIPPETS),
        (args.a012_packet_doc, "M247-A013-A012-PKT-EXISTS", A012_PACKET_SNIPPETS),
        (args.readiness_runner, "M247-A013-RUN-EXISTS", READINESS_RUNNER_SNIPPETS),
        (args.architecture_doc, "M247-A013-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M247-A013-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M247-A013-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M247-A013-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.a012_checker, "M247-A013-DEP-A012-ARG-01"),
        (args.a012_test, "M247-A013-DEP-A012-ARG-02"),
        (args.a012_readiness_runner, "M247-A013-DEP-A012-ARG-03"),
    ):
        checks_total += 1
        if not path.exists():
            failures.append(
                Finding(
                    artifact=display_path(path),
                    check_id=check_id,
                    detail=f"required dependency path is missing: {display_path(path)}",
                )
            )
            continue
        if not path.is_file():
            failures.append(
                Finding(
                    artifact=display_path(path),
                    check_id=check_id,
                    detail=f"required dependency path is not a file: {display_path(path)}",
                )
            )

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

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

