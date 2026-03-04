#!/usr/bin/env python3
"""Fail-closed checker for M247-A015 frontend profiling advanced core workpack shard 1."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-a015-frontend-profiling-hot-path-decomposition-advanced-core-workpack-shard-1-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_frontend_profiling_and_hot_path_decomposition_advanced_core_workpack_shard_1_a015_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_a015_frontend_profiling_and_hot_path_decomposition_advanced_core_workpack_shard_1_packet.md"
)
DEFAULT_A014_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_frontend_profiling_and_hot_path_decomposition_release_candidate_and_replay_dry_run_a014_expectations.md"
)
DEFAULT_A014_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_a014_frontend_profiling_and_hot_path_decomposition_release_candidate_and_replay_dry_run_packet.md"
)
DEFAULT_A014_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_a014_frontend_profiling_and_hot_path_decomposition_release_candidate_and_replay_dry_run_contract.py"
)
DEFAULT_A014_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_a014_frontend_profiling_and_hot_path_decomposition_release_candidate_and_replay_dry_run_contract.py"
)
DEFAULT_A014_READINESS_RUNNER = ROOT / "scripts" / "run_m247_a014_lane_a_readiness.py"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m247_a015_lane_a_readiness.py"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-A015/frontend_profiling_and_hot_path_decomposition_advanced_core_workpack_shard_1_contract_summary.json"
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
        "M247-A015-DOC-EXP-01",
        "# M247 Frontend Profiling and Hot-Path Decomposition Advanced Core Workpack (Shard 1) Expectations (A015)",
    ),
    SnippetCheck(
        "M247-A015-DOC-EXP-02",
        "Contract ID: `objc3c-frontend-profiling-hot-path-decomposition-advanced-core-workpack-shard-1/m247-a015-v1`",
    ),
    SnippetCheck("M247-A015-DOC-EXP-03", "Dependencies: `M247-A014`"),
    SnippetCheck(
        "M247-A015-DOC-EXP-04",
        "Issue `#6722` defines canonical lane-A advanced core workpack (shard 1) scope.",
    ),
    SnippetCheck(
        "M247-A015-DOC-EXP-05",
        "`scripts/check_m247_a014_frontend_profiling_and_hot_path_decomposition_release_candidate_and_replay_dry_run_contract.py`",
    ),
    SnippetCheck(
        "M247-A015-DOC-EXP-06",
        "advanced-core-workpack command sequencing and advanced-core-workpack-shard-1-key",
    ),
    SnippetCheck(
        "M247-A015-DOC-EXP-07",
        "`check:objc3c:m247-a015-frontend-profiling-hot-path-decomposition-advanced-core-workpack-shard-1-contract`",
    ),
    SnippetCheck(
        "M247-A015-DOC-EXP-08",
        "`test:tooling:m247-a015-frontend-profiling-hot-path-decomposition-advanced-core-workpack-shard-1-contract`",
    ),
    SnippetCheck("M247-A015-DOC-EXP-09", "`check:objc3c:m247-a015-lane-a-readiness`"),
    SnippetCheck(
        "M247-A015-DOC-EXP-10",
        "`python scripts/run_m247_a014_lane_a_readiness.py`",
    ),
    SnippetCheck(
        "M247-A015-DOC-EXP-11",
        "`python scripts/check_m247_a015_frontend_profiling_and_hot_path_decomposition_advanced_core_workpack_shard_1_contract.py`",
    ),
    SnippetCheck(
        "M247-A015-DOC-EXP-12",
        "`tmp/reports/m247/M247-A015/frontend_profiling_and_hot_path_decomposition_advanced_core_workpack_shard_1_contract_summary.json`",
    ),
    SnippetCheck(
        "M247-A015-DOC-EXP-13",
        "optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck("M247-A015-DOC-EXP-14", "`compile:objc3c`"),
    SnippetCheck("M247-A015-DOC-EXP-15", "`test:objc3c:perf-budget`"),
    SnippetCheck("M247-A015-DOC-EXP-16", "`test:objc3c:parser-replay-proof`"),
    SnippetCheck("M247-A015-DOC-EXP-17", "`test:objc3c:parser-ast-extraction`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A015-DOC-PKT-01",
        "# M247-A015 Frontend Profiling and Hot-Path Decomposition Advanced Core Workpack (Shard 1) Packet",
    ),
    SnippetCheck("M247-A015-DOC-PKT-02", "Packet: `M247-A015`"),
    SnippetCheck("M247-A015-DOC-PKT-03", "Issue: `#6722`"),
    SnippetCheck("M247-A015-DOC-PKT-04", "Dependencies: `M247-A014`"),
    SnippetCheck(
        "M247-A015-DOC-PKT-05",
        "`scripts/check_m247_a015_frontend_profiling_and_hot_path_decomposition_advanced_core_workpack_shard_1_contract.py`",
    ),
    SnippetCheck(
        "M247-A015-DOC-PKT-06",
        "`tests/tooling/test_check_m247_a015_frontend_profiling_and_hot_path_decomposition_advanced_core_workpack_shard_1_contract.py`",
    ),
    SnippetCheck("M247-A015-DOC-PKT-07", "`scripts/run_m247_a015_lane_a_readiness.py`"),
    SnippetCheck(
        "M247-A015-DOC-PKT-08",
        "`scripts/check_m247_a014_frontend_profiling_and_hot_path_decomposition_release_candidate_and_replay_dry_run_contract.py`",
    ),
    SnippetCheck(
        "M247-A015-DOC-PKT-09",
        "`check:objc3c:m247-a015-frontend-profiling-hot-path-decomposition-advanced-core-workpack-shard-1-contract`",
    ),
    SnippetCheck(
        "M247-A015-DOC-PKT-10",
        "`test:tooling:m247-a015-frontend-profiling-hot-path-decomposition-advanced-core-workpack-shard-1-contract`",
    ),
    SnippetCheck("M247-A015-DOC-PKT-11", "`check:objc3c:m247-a015-lane-a-readiness`"),
    SnippetCheck("M247-A015-DOC-PKT-12", "`check:objc3c:m247-a014-lane-a-readiness`"),
    SnippetCheck("M247-A015-DOC-PKT-13", "mandatory scope inputs."),
    SnippetCheck(
        "M247-A015-DOC-PKT-14",
        "`tmp/reports/m247/M247-A015/frontend_profiling_and_hot_path_decomposition_advanced_core_workpack_shard_1_contract_summary.json`",
    ),
)

A014_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A015-A014-DOC-01",
        "Contract ID: `objc3c-frontend-profiling-hot-path-decomposition-release-candidate-and-replay-dry-run/m247-a014-v1`",
    ),
    SnippetCheck("M247-A015-A014-DOC-02", "Dependencies: `M247-A013`"),
    SnippetCheck(
        "M247-A015-A014-DOC-03",
        "Issue `#6721` defines canonical lane-A release-candidate and replay dry-run scope.",
    ),
)

A014_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-A015-A014-PKT-01", "Packet: `M247-A014`"),
    SnippetCheck("M247-A015-A014-PKT-02", "Issue: `#6721`"),
    SnippetCheck("M247-A015-A014-PKT-03", "Dependencies: `M247-A013`"),
)

A014_READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-A015-A014-RUN-01", '"""Run M247-A014 lane-A readiness checks without deep npm nesting."""'),
    SnippetCheck("M247-A015-A014-RUN-02", "scripts/run_m247_a013_lane_a_readiness.py"),
    SnippetCheck(
        "M247-A015-A014-RUN-03",
        "scripts/check_m247_a014_frontend_profiling_and_hot_path_decomposition_release_candidate_and_replay_dry_run_contract.py",
    ),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-A015-RUN-01", '"""Run M247-A015 lane-A readiness checks without deep npm nesting."""'),
    SnippetCheck("M247-A015-RUN-02", "scripts/run_m247_a014_lane_a_readiness.py"),
    SnippetCheck(
        "M247-A015-RUN-03",
        "scripts/check_m247_a015_frontend_profiling_and_hot_path_decomposition_advanced_core_workpack_shard_1_contract.py",
    ),
    SnippetCheck(
        "M247-A015-RUN-04",
        "tests/tooling/test_check_m247_a015_frontend_profiling_and_hot_path_decomposition_advanced_core_workpack_shard_1_contract.py",
    ),
    SnippetCheck("M247-A015-RUN-05", 'BASELINE_DEPENDENCIES = ("M247-A014",)'),
    SnippetCheck("M247-A015-RUN-06", "[ok] M247-A015 lane-A readiness chain completed"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A015-ARCH-01",
        "M247 lane-A A014 frontend profiling and hot-path decomposition release-candidate and replay dry-run anchors",
    ),
    SnippetCheck(
        "M247-A015-ARCH-02",
        "`M247-A013` dependency continuity and release-candidate/replay dry-run evidence remain fail-closed.",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A015-SPC-01",
        "frontend profiling and hot-path decomposition release-candidate and replay dry-run wiring",
    ),
    SnippetCheck(
        "M247-A015-SPC-02",
        "release-candidate/replay command sequencing continuity, release_candidate_replay_key continuity, or contract-gating evidence commands drift.",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A015-META-01",
        "release-candidate/replay dry-run metadata anchors for `M247-A014`",
    ),
    SnippetCheck(
        "M247-A015-META-02",
        "with explicit `M247-A013` dependency continuity so lane-A release-candidate/replay dry-run contract-gating evidence remains fail-closed.",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-A015-PKG-01",
        '"check:objc3c:m247-a014-frontend-profiling-hot-path-decomposition-release-candidate-and-replay-dry-run-contract": '
        '"python scripts/check_m247_a014_frontend_profiling_and_hot_path_decomposition_release_candidate_and_replay_dry_run_contract.py"',
    ),
    SnippetCheck(
        "M247-A015-PKG-02",
        '"test:tooling:m247-a014-frontend-profiling-hot-path-decomposition-release-candidate-and-replay-dry-run-contract": '
        '"python -m pytest tests/tooling/test_check_m247_a014_frontend_profiling_and_hot_path_decomposition_release_candidate_and_replay_dry_run_contract.py -q"',
    ),
    SnippetCheck(
        "M247-A015-PKG-03",
        '"check:objc3c:m247-a014-lane-a-readiness": "python scripts/run_m247_a014_lane_a_readiness.py"',
    ),
    SnippetCheck("M247-A015-PKG-04", '"compile:objc3c": '),
    SnippetCheck("M247-A015-PKG-05", '"test:objc3c:perf-budget": '),
    SnippetCheck("M247-A015-PKG-06", '"test:objc3c:parser-replay-proof": '),
    SnippetCheck("M247-A015-PKG-07", '"test:objc3c:parser-ast-extraction": '),
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
    parser.add_argument("--a014-expectations-doc", type=Path, default=DEFAULT_A014_EXPECTATIONS_DOC)
    parser.add_argument("--a014-packet-doc", type=Path, default=DEFAULT_A014_PACKET_DOC)
    parser.add_argument("--a014-checker", type=Path, default=DEFAULT_A014_CHECKER)
    parser.add_argument("--a014-test", type=Path, default=DEFAULT_A014_TEST)
    parser.add_argument("--a014-readiness-runner", type=Path, default=DEFAULT_A014_READINESS_RUNNER)
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
        (args.expectations_doc, "M247-A015-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M247-A015-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.a014_expectations_doc, "M247-A015-A014-DOC-EXISTS", A014_EXPECTATIONS_SNIPPETS),
        (args.a014_packet_doc, "M247-A015-A014-PKT-EXISTS", A014_PACKET_SNIPPETS),
        (args.a014_readiness_runner, "M247-A015-A014-RUN-EXISTS", A014_READINESS_SNIPPETS),
        (args.readiness_runner, "M247-A015-RUN-EXISTS", READINESS_SNIPPETS),
        (args.architecture_doc, "M247-A015-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M247-A015-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M247-A015-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M247-A015-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.a014_checker, "M247-A015-DEP-A014-ARG-01"),
        (args.a014_test, "M247-A015-DEP-A014-ARG-02"),
        (args.a014_readiness_runner, "M247-A015-DEP-A014-ARG-03"),
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

    status = "ok" if not failures else "error"
    print(f"[{status}] M247-A015 contract checks passed: {checks_passed}/{checks_total}")
    print(f"[info] summary: {display_path(summary_path)}")
    if failures:
        for failure in failures:
            print(
                f"[error] {failure.check_id}::{failure.artifact}::{failure.detail}",
                file=sys.stderr,
            )
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

