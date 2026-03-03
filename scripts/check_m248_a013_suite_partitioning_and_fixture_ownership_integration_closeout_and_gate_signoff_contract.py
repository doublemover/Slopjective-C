#!/usr/bin/env python3
"""Fail-closed checker for M248-A013 suite partitioning integration closeout/signoff contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m248-a013-suite-partitioning-fixture-ownership-integration-closeout-and-gate-signoff-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_a013_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_a013_suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_packet.md"
)
DEFAULT_A012_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_a012_expectations.md"
)
DEFAULT_A012_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_packet.md"
)
DEFAULT_A012_CHECKER = (
    ROOT
    / "scripts"
    / "check_m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_contract.py"
)
DEFAULT_A012_TOOLING_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_contract.py"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m248/M248-A013/suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_contract_summary.json"
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
        "M248-A013-DOC-EXP-01",
        "Contract ID: `objc3c-suite-partitioning-and-fixture-ownership-integration-closeout-and-gate-signoff/m248-a013-v1`",
    ),
    SnippetCheck("M248-A013-DOC-EXP-02", "Dependencies: `M248-A012`"),
    SnippetCheck(
        "M248-A013-DOC-EXP-03",
        "Issue `#6800` defines canonical lane-A integration closeout and gate signoff scope.",
    ),
    SnippetCheck("M248-A013-DOC-EXP-04", "integration_closeout_and_gate_signoff_consistent"),
    SnippetCheck("M248-A013-DOC-EXP-05", "integration_closeout_and_gate_signoff_ready"),
    SnippetCheck("M248-A013-DOC-EXP-06", "integration_closeout_and_gate_signoff_key_ready"),
    SnippetCheck("M248-A013-DOC-EXP-07", "integration_closeout_and_gate_signoff_key"),
    SnippetCheck(
        "M248-A013-DOC-EXP-08",
        "scripts/check_m248_a013_suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_contract.py",
    ),
    SnippetCheck(
        "M248-A013-DOC-EXP-09",
        "tests/tooling/test_check_m248_a013_suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_contract.py",
    ),
    SnippetCheck(
        "M248-A013-DOC-EXP-10",
        "tmp/reports/m248/M248-A013/suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_contract_summary.json",
    ),
    SnippetCheck(
        "M248-A013-DOC-EXP-11",
        "`check:objc3c:m248-a013-suite-partitioning-fixture-ownership-integration-closeout-and-gate-signoff-contract`",
    ),
    SnippetCheck(
        "M248-A013-DOC-EXP-12",
        "`test:tooling:m248-a013-suite-partitioning-fixture-ownership-integration-closeout-and-gate-signoff-contract`",
    ),
    SnippetCheck("M248-A013-DOC-EXP-13", "`check:objc3c:m248-a012-lane-a-readiness`"),
    SnippetCheck("M248-A013-DOC-EXP-14", "`check:objc3c:m248-a013-lane-a-readiness`"),
    SnippetCheck("M248-A013-DOC-EXP-15", "`npm run check:objc3c:m248-a013-lane-a-readiness`"),
    SnippetCheck("M248-A013-DOC-EXP-16", "`test:objc3c:parser-replay-proof`"),
    SnippetCheck("M248-A013-DOC-EXP-17", "`test:objc3c:parser-ast-extraction`"),
    SnippetCheck(
        "M248-A013-DOC-EXP-18",
        "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
    ),
    SnippetCheck(
        "M248-A013-DOC-EXP-19",
        "`python scripts/check_m248_a013_suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_contract.py --emit-json`",
    ),
    SnippetCheck(
        "M248-A013-DOC-EXP-20",
        "`python -m pytest tests/tooling/test_check_m248_a013_suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_contract.py -q`",
    ),
    SnippetCheck(
        "M248-A013-DOC-EXP-21",
        "`python scripts/check_m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_contract.py`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-A013-DOC-PKT-01", "Packet: `M248-A013`"),
    SnippetCheck("M248-A013-DOC-PKT-02", "Issue: `#6800`"),
    SnippetCheck("M248-A013-DOC-PKT-03", "Dependencies: `M248-A012`"),
    SnippetCheck(
        "M248-A013-DOC-PKT-04",
        "`scripts/check_m248_a013_suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_contract.py`",
    ),
    SnippetCheck(
        "M248-A013-DOC-PKT-05",
        "`tests/tooling/test_check_m248_a013_suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_contract.py`",
    ),
    SnippetCheck(
        "M248-A013-DOC-PKT-06",
        "`check:objc3c:m248-a013-suite-partitioning-fixture-ownership-integration-closeout-and-gate-signoff-contract`",
    ),
    SnippetCheck(
        "M248-A013-DOC-PKT-07",
        "`test:tooling:m248-a013-suite-partitioning-fixture-ownership-integration-closeout-and-gate-signoff-contract`",
    ),
    SnippetCheck("M248-A013-DOC-PKT-08", "`check:objc3c:m248-a013-lane-a-readiness`"),
    SnippetCheck(
        "M248-A013-DOC-PKT-09",
        "`docs/contracts/m248_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_a012_expectations.md`",
    ),
    SnippetCheck(
        "M248-A013-DOC-PKT-10",
        "`spec/planning/compiler/m248/m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_packet.md`",
    ),
    SnippetCheck(
        "M248-A013-DOC-PKT-11",
        "`scripts/check_m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_contract.py`",
    ),
    SnippetCheck(
        "M248-A013-DOC-PKT-12",
        "`tests/tooling/test_check_m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_contract.py`",
    ),
    SnippetCheck(
        "M248-A013-DOC-PKT-13",
        "`python scripts/check_m248_a013_suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_contract.py --emit-json`",
    ),
    SnippetCheck("M248-A013-DOC-PKT-14", "`npm run check:objc3c:m248-a013-lane-a-readiness`"),
    SnippetCheck(
        "M248-A013-DOC-PKT-15",
        "`tmp/reports/m248/M248-A013/suite_partitioning_and_fixture_ownership_integration_closeout_and_gate_signoff_contract_summary.json`",
    ),
    SnippetCheck(
        "M248-A013-DOC-PKT-16",
        "`python scripts/check_m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_contract.py`",
    ),
    SnippetCheck("M248-A013-DOC-PKT-17", "`test:objc3c:parser-replay-proof`"),
    SnippetCheck("M248-A013-DOC-PKT-18", "`test:objc3c:parser-ast-extraction`"),
    SnippetCheck(
        "M248-A013-DOC-PKT-19",
        "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
    ),
)

A012_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-A013-DEP-01",
        "Contract ID: `objc3c-suite-partitioning-and-fixture-ownership-cross-lane-integration-sync/m248-a012-v1`",
    ),
    SnippetCheck("M248-A013-DEP-02", "Dependencies: `M248-A011`"),
    SnippetCheck(
        "M248-A013-DEP-03",
        "Issue `#6799` defines canonical lane-A cross-lane integration sync scope.",
    ),
)

A012_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-A013-DEP-04", "Packet: `M248-A012`"),
    SnippetCheck("M248-A013-DEP-05", "Dependencies: `M248-A011`"),
    SnippetCheck("M248-A013-DEP-06", "Issue: `#6799`"),
)

A012_CHECKER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-A013-DEP-07",
        "m248-a012-suite-partitioning-fixture-ownership-cross-lane-integration-sync-contract-v1",
    ),
)

A012_TOOLING_TEST_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-A013-DEP-08",
        "check_m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_contract",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-A013-ARCH-01",
        "M248 lane-A A008 suite partitioning and fixture ownership recovery and determinism hardening",
    ),
    SnippetCheck(
        "M248-A013-ARCH-02",
        "docs/contracts/m248_suite_partitioning_and_fixture_ownership_recovery_and_determinism_hardening_a008_expectations.md",
    ),
    SnippetCheck(
        "M248-A013-ARCH-03",
        "and fail-closed against `M248-A007` dependency drift.",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-A013-SPC-01",
        "suite partitioning and fixture ownership governance shall preserve explicit",
    ),
    SnippetCheck(
        "M248-A013-SPC-02",
        "lane-A dependency boundary anchors and fail closed on fixture partition drift",
    ),
    SnippetCheck(
        "M248-A013-SPC-03",
        "suite partitioning and fixture ownership recovery and determinism hardening governance",
    ),
    SnippetCheck(
        "M248-A013-SPC-04",
        "closed on recovery and determinism hardening evidence drift before downstream",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-A013-META-01",
        "deterministic lane-A suite partitioning and fixture ownership recovery and determinism hardening metadata anchors for `M248-A008`",
    ),
    SnippetCheck(
        "M248-A013-META-02",
        "explicit `M248-A007` dependency continuity and fail-closed recovery/determinism evidence continuity",
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
    parser.add_argument("--a012-expectations-doc", type=Path, default=DEFAULT_A012_EXPECTATIONS_DOC)
    parser.add_argument("--a012-packet-doc", type=Path, default=DEFAULT_A012_PACKET_DOC)
    parser.add_argument("--a012-checker", type=Path, default=DEFAULT_A012_CHECKER)
    parser.add_argument("--a012-tooling-test", type=Path, default=DEFAULT_A012_TOOLING_TEST)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--emit-json", action="store_true", help="Emit canonical summary JSON to stdout.")
    return parser.parse_args(argv)


def check_doc_contract(
    *,
    artifact_name: str,
    path: Path,
    exists_check_id: str,
    snippets: tuple[SnippetCheck, ...],
) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists() or not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required document is missing: {display_path(path)}",
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

    for snippet_check in snippets:
        checks_total += 1
        if snippet_check.snippet not in text:
            findings.append(
                Finding(
                    artifact=artifact_name,
                    check_id=snippet_check.check_id,
                    detail=f"missing required snippet: {snippet_check.snippet}",
                )
            )
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    checks_total = 0
    failures: list[Finding] = []

    for artifact_name, path, exists_check_id, snippets in (
        ("expectations_doc", args.expectations_doc, "M248-A013-DOC-EXP-00", EXPECTATIONS_SNIPPETS),
        ("packet_doc", args.packet_doc, "M248-A013-DOC-PKT-00", PACKET_SNIPPETS),
        ("a012_expectations_doc", args.a012_expectations_doc, "M248-A013-DEP-00", A012_EXPECTATIONS_SNIPPETS),
        ("a012_packet_doc", args.a012_packet_doc, "M248-A013-DEP-00", A012_PACKET_SNIPPETS),
        ("a012_checker", args.a012_checker, "M248-A013-DEP-00", A012_CHECKER_SNIPPETS),
        ("a012_tooling_test", args.a012_tooling_test, "M248-A013-DEP-00", A012_TOOLING_TEST_SNIPPETS),
        ("architecture_doc", args.architecture_doc, "M248-A013-ARCH-00", ARCHITECTURE_SNIPPETS),
        ("lowering_spec", args.lowering_spec, "M248-A013-SPC-00", LOWERING_SPEC_SNIPPETS),
        ("metadata_spec", args.metadata_spec, "M248-A013-META-00", METADATA_SPEC_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            artifact_name=artifact_name,
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    failures = sorted(failures, key=lambda finding: (finding.check_id, finding.artifact, finding.detail))
    summary = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
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
    summary_path.write_text(canonical_json(summary), encoding="utf-8")

    if args.emit_json:
        print(canonical_json(summary), end="")

    if failures:
        if not args.emit_json:
            for finding in failures:
                print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1

    if not args.emit_json:
        print(f"[ok] {MODE}: {summary['checks_passed']}/{summary['checks_total']} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
