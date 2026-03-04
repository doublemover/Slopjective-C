#!/usr/bin/env python3
"""Fail-closed checker for M247-C014 lowering/codegen release-candidate replay dry-run."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-c014-lowering-codegen-cost-profiling-controls-release-candidate-replay-dry-run-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_lane_c_lowering_codegen_cost_profiling_and_controls_release_candidate_and_replay_dry_run_c014_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_c014_lowering_codegen_cost_profiling_and_controls_release_candidate_and_replay_dry_run_packet.md"
)
DEFAULT_C013_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_lane_c_lowering_codegen_cost_profiling_and_controls_docs_and_operator_runbook_synchronization_c013_expectations.md"
)
DEFAULT_C013_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_c013_lowering_codegen_cost_profiling_and_controls_docs_and_operator_runbook_synchronization_packet.md"
)
DEFAULT_C013_READINESS_SCRIPT = ROOT / "scripts" / "run_m247_c013_lane_c_readiness.py"
DEFAULT_READINESS_SCRIPT = ROOT / "scripts" / "run_m247_c014_lane_c_readiness.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-C014/lowering_codegen_cost_profiling_and_controls_release_candidate_and_replay_dry_run_summary.json"
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
        "M247-C014-DEP-C013-01",
        Path("docs/contracts/m247_lane_c_lowering_codegen_cost_profiling_and_controls_docs_and_operator_runbook_synchronization_c013_expectations.md"),
    ),
    AssetCheck(
        "M247-C014-DEP-C013-02",
        Path("spec/planning/compiler/m247/m247_c013_lowering_codegen_cost_profiling_and_controls_docs_and_operator_runbook_synchronization_packet.md"),
    ),
    AssetCheck(
        "M247-C014-DEP-C013-03",
        Path("scripts/check_m247_c013_lowering_codegen_cost_profiling_and_controls_docs_and_operator_runbook_synchronization_contract.py"),
    ),
    AssetCheck(
        "M247-C014-DEP-C013-04",
        Path("tests/tooling/test_check_m247_c013_lowering_codegen_cost_profiling_and_controls_docs_and_operator_runbook_synchronization_contract.py"),
    ),
    AssetCheck(
        "M247-C014-DEP-C013-05",
        Path("scripts/run_m247_c013_lane_c_readiness.py"),
    ),
    AssetCheck(
        "M247-C014-OWN-01",
        Path(
            "docs/contracts/m247_lane_c_lowering_codegen_cost_profiling_and_controls_release_candidate_and_replay_dry_run_c014_expectations.md"
        ),
    ),
    AssetCheck(
        "M247-C014-OWN-02",
        Path(
            "spec/planning/compiler/m247/m247_c014_lowering_codegen_cost_profiling_and_controls_release_candidate_and_replay_dry_run_packet.md"
        ),
    ),
    AssetCheck(
        "M247-C014-OWN-03",
        Path("scripts/check_m247_c014_lowering_codegen_cost_profiling_and_controls_release_candidate_and_replay_dry_run_contract.py"),
    ),
    AssetCheck(
        "M247-C014-OWN-04",
        Path(
            "tests/tooling/test_check_m247_c014_lowering_codegen_cost_profiling_and_controls_release_candidate_and_replay_dry_run_contract.py"
        ),
    ),
    AssetCheck(
        "M247-C014-OWN-05",
        Path("scripts/run_m247_c014_lane_c_readiness.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-C014-DOC-EXP-01",
        "# M247 Lane C Lowering/Codegen Cost Profiling and Controls Release-Candidate and Replay Dry-Run Expectations (C014)",
    ),
    SnippetCheck(
        "M247-C014-DOC-EXP-02",
        "Contract ID: `objc3c-lane-c-lowering-codegen-cost-profiling-controls-release-candidate-replay-dry-run/m247-c014-v1`",
    ),
    SnippetCheck(
        "M247-C014-DOC-EXP-03",
        "Issue `#6755` defines canonical lane-C release-candidate and replay dry-run scope.",
    ),
    SnippetCheck("M247-C014-DOC-EXP-04", "Dependencies: `M247-C013`"),
    SnippetCheck(
        "M247-C014-DOC-EXP-05",
        "Predecessor anchors inherited via `M247-C013`: `M247-C001`, `M247-C002`, `M247-C003`, `M247-C004`, `M247-C005`, `M247-C006`, `M247-C007`, `M247-C008`, `M247-C009`, `M247-C010`, `M247-C011`, `M247-C012`.",
    ),
    SnippetCheck(
        "M247-C014-DOC-EXP-06",
        "docs/contracts/m247_lane_c_lowering_codegen_cost_profiling_and_controls_docs_and_operator_runbook_synchronization_c013_expectations.md",
    ),
    SnippetCheck(
        "M247-C014-DOC-EXP-07",
        "scripts/run_m247_c013_lane_c_readiness.py",
    ),
    SnippetCheck(
        "M247-C014-DOC-EXP-08",
        "scripts/check_m247_c014_lowering_codegen_cost_profiling_and_controls_release_candidate_and_replay_dry_run_contract.py",
    ),
    SnippetCheck(
        "M247-C014-DOC-EXP-09",
        "tests/tooling/test_check_m247_c014_lowering_codegen_cost_profiling_and_controls_release_candidate_and_replay_dry_run_contract.py",
    ),
    SnippetCheck(
        "M247-C014-DOC-EXP-10",
        "Release-candidate and replay dry-run consistency/readiness and replay-dry-run-key continuity remain deterministic and fail-closed across lane-C readiness wiring.",
    ),
    SnippetCheck(
        "M247-C014-DOC-EXP-11",
        "Code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M247-C014-DOC-EXP-12",
        "tmp/reports/m247/M247-C014/lowering_codegen_cost_profiling_and_controls_release_candidate_and_replay_dry_run_summary.json",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-C014-DOC-PKT-01",
        "# M247-C014 Lowering/Codegen Cost Profiling and Controls Release-Candidate and Replay Dry-Run Packet",
    ),
    SnippetCheck("M247-C014-DOC-PKT-02", "Packet: `M247-C014`"),
    SnippetCheck("M247-C014-DOC-PKT-03", "Issue: `#6755`"),
    SnippetCheck("M247-C014-DOC-PKT-04", "Freeze date: `2026-03-04`"),
    SnippetCheck("M247-C014-DOC-PKT-05", "Dependencies: `M247-C013`"),
    SnippetCheck(
        "M247-C014-DOC-PKT-06",
        "Predecessor anchors inherited via `M247-C013`: `M247-C001`, `M247-C002`, `M247-C003`, `M247-C004`, `M247-C005`, `M247-C006`, `M247-C007`, `M247-C008`, `M247-C009`, `M247-C010`, `M247-C011`, `M247-C012`.",
    ),
    SnippetCheck(
        "M247-C014-DOC-PKT-07",
        "scripts/check_m247_c014_lowering_codegen_cost_profiling_and_controls_release_candidate_and_replay_dry_run_contract.py",
    ),
    SnippetCheck(
        "M247-C014-DOC-PKT-08",
        "tests/tooling/test_check_m247_c014_lowering_codegen_cost_profiling_and_controls_release_candidate_and_replay_dry_run_contract.py",
    ),
    SnippetCheck(
        "M247-C014-DOC-PKT-09",
        "scripts/run_m247_c014_lane_c_readiness.py",
    ),
    SnippetCheck(
        "M247-C014-DOC-PKT-10",
        "scripts/run_m247_c013_lane_c_readiness.py",
    ),
    SnippetCheck(
        "M247-C014-DOC-PKT-11",
        "treated as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M247-C014-DOC-PKT-12",
        "tmp/reports/m247/M247-C014/lowering_codegen_cost_profiling_and_controls_release_candidate_and_replay_dry_run_summary.json",
    ),
)

C013_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-C014-C013-EXP-01",
        "Contract ID: `objc3c-lane-c-lowering-codegen-cost-profiling-controls-docs-and-operator-runbook-synchronization/m247-c013-v1`",
    ),
    SnippetCheck(
        "M247-C014-C013-EXP-02",
        "Dependencies: `M247-C012`",
    ),
    SnippetCheck(
        "M247-C014-C013-EXP-03",
        "Issue `#6754` defines canonical lane-C docs and operator runbook synchronization scope.",
    ),
    SnippetCheck(
        "M247-C014-C013-EXP-04",
        "scripts/run_m247_c013_lane_c_readiness.py",
    ),
    SnippetCheck(
        "M247-C014-C013-EXP-05",
        "tmp/reports/m247/M247-C013/lowering_codegen_cost_profiling_and_controls_docs_and_operator_runbook_synchronization_summary.json",
    ),
)

C013_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-C014-C013-PKT-01", "Packet: `M247-C013`"),
    SnippetCheck("M247-C014-C013-PKT-02", "Issue: `#6754`"),
    SnippetCheck("M247-C014-C013-PKT-03", "Dependencies: `M247-C012`"),
    SnippetCheck("M247-C014-C013-PKT-04", "scripts/run_m247_c013_lane_c_readiness.py"),
    SnippetCheck("M247-C014-C013-PKT-05", "scripts/run_m247_c012_lane_c_readiness.py"),
)

C013_READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-C014-C013-RUN-01",
        "scripts/run_m247_c012_lane_c_readiness.py",
    ),
    SnippetCheck(
        "M247-C014-C013-RUN-02",
        "scripts/check_m247_c013_lowering_codegen_cost_profiling_and_controls_docs_and_operator_runbook_synchronization_contract.py",
    ),
    SnippetCheck(
        "M247-C014-C013-RUN-03",
        "tests/tooling/test_check_m247_c013_lowering_codegen_cost_profiling_and_controls_docs_and_operator_runbook_synchronization_contract.py",
    ),
    SnippetCheck(
        "M247-C014-C013-RUN-04",
        "[ok] M247-C013 lane-C readiness chain completed",
    ),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-C014-RUN-01",
        "scripts/run_m247_c013_lane_c_readiness.py",
    ),
    SnippetCheck(
        "M247-C014-RUN-02",
        "scripts/check_m247_c014_lowering_codegen_cost_profiling_and_controls_release_candidate_and_replay_dry_run_contract.py",
    ),
    SnippetCheck(
        "M247-C014-RUN-03",
        "tests/tooling/test_check_m247_c014_lowering_codegen_cost_profiling_and_controls_release_candidate_and_replay_dry_run_contract.py",
    ),
    SnippetCheck(
        "M247-C014-RUN-04",
        "[ok] M247-C014 lane-C readiness chain completed",
    ),
    SnippetCheck(
        "M247-C014-RUN-05",
        "command failed with exit code",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-C014-PKG-01",
        '"check:objc3c:m247-c002-lane-c-readiness": ',
    ),
    SnippetCheck("M247-C014-PKG-02", '"compile:objc3c": '),
    SnippetCheck("M247-C014-PKG-03", '"proof:objc3c": '),
    SnippetCheck("M247-C014-PKG-04", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M247-C014-PKG-05", '"test:objc3c:perf-budget": '),
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
    parser.add_argument("--c013-expectations-doc", type=Path, default=DEFAULT_C013_EXPECTATIONS_DOC)
    parser.add_argument("--c013-packet-doc", type=Path, default=DEFAULT_C013_PACKET_DOC)
    parser.add_argument("--c013-readiness-script", type=Path, default=DEFAULT_C013_READINESS_SCRIPT)
    parser.add_argument("--readiness-script", type=Path, default=DEFAULT_READINESS_SCRIPT)
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


def check_text_artifact(
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

    dep_checks, dep_failures = check_prerequisite_assets()
    checks_total += dep_checks
    failures.extend(dep_failures)

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M247-C014-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M247-C014-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.c013_expectations_doc, "M247-C014-C013-EXP-EXISTS", C013_EXPECTATIONS_SNIPPETS),
        (args.c013_packet_doc, "M247-C014-C013-PKT-EXISTS", C013_PACKET_SNIPPETS),
        (args.c013_readiness_script, "M247-C014-C013-RUN-EXISTS", C013_READINESS_SNIPPETS),
        (args.readiness_script, "M247-C014-RUN-EXISTS", READINESS_SNIPPETS),
        (args.package_json, "M247-C014-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_text_artifact(path=path, exists_check_id=exists_check_id, snippets=snippets)
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
