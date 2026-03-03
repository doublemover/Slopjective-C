#!/usr/bin/env python3
"""Fail-closed checker for M248-A012 suite partitioning cross-lane integration sync contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m248-a012-suite-partitioning-fixture-ownership-cross-lane-integration-sync-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_a012_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_packet.md"
)
DEFAULT_A011_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_suite_partitioning_and_fixture_ownership_performance_and_quality_guardrails_a011_expectations.md"
)
DEFAULT_A011_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_a011_suite_partitioning_and_fixture_ownership_performance_and_quality_guardrails_packet.md"
)
DEFAULT_A011_CHECKER = (
    ROOT
    / "scripts"
    / "check_m248_a011_suite_partitioning_and_fixture_ownership_performance_and_quality_guardrails_contract.py"
)
DEFAULT_A011_TOOLING_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m248_a011_suite_partitioning_and_fixture_ownership_performance_and_quality_guardrails_contract.py"
)
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m248/M248-A012/suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_contract_summary.json"
)


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class PackageScriptKeyCheck:
    check_id: str
    script_key: str


@dataclass(frozen=True)
class PackageScriptCheck:
    check_id: str
    script_key: str
    expected_value: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-A012-DOC-EXP-01",
        "Contract ID: `objc3c-suite-partitioning-and-fixture-ownership-cross-lane-integration-sync/m248-a012-v1`",
    ),
    SnippetCheck("M248-A012-DOC-EXP-02", "Dependencies: `M248-A011`"),
    SnippetCheck("M248-A012-DOC-EXP-03", "Issue `#6799` defines canonical lane-A cross-lane integration sync scope."),
    SnippetCheck("M248-A012-DOC-EXP-04", "cross_lane_integration_sync_consistent"),
    SnippetCheck("M248-A012-DOC-EXP-05", "cross_lane_integration_sync_ready"),
    SnippetCheck("M248-A012-DOC-EXP-06", "cross_lane_integration_sync_key_ready"),
    SnippetCheck("M248-A012-DOC-EXP-07", "cross_lane_integration_sync_key"),
    SnippetCheck(
        "M248-A012-DOC-EXP-08",
        "scripts/check_m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_contract.py",
    ),
    SnippetCheck(
        "M248-A012-DOC-EXP-09",
        "tests/tooling/test_check_m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_contract.py",
    ),
    SnippetCheck(
        "M248-A012-DOC-EXP-10",
        "tmp/reports/m248/M248-A012/suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_contract_summary.json",
    ),
    SnippetCheck("M248-A012-DOC-EXP-11", "`check:objc3c:m248-a012-lane-a-readiness`"),
    SnippetCheck("M248-A012-DOC-EXP-12", "`test:objc3c:parser-replay-proof`"),
    SnippetCheck("M248-A012-DOC-EXP-13", "`test:objc3c:parser-ast-extraction`"),
    SnippetCheck(
        "M248-A012-DOC-EXP-14",
        "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-A012-DOC-PKT-01", "Packet: `M248-A012`"),
    SnippetCheck("M248-A012-DOC-PKT-02", "Issue: `#6799`"),
    SnippetCheck("M248-A012-DOC-PKT-03", "Dependencies: `M248-A011`"),
    SnippetCheck(
        "M248-A012-DOC-PKT-04",
        "`scripts/check_m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_contract.py`",
    ),
    SnippetCheck(
        "M248-A012-DOC-PKT-05",
        "`tests/tooling/test_check_m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_contract.py`",
    ),
    SnippetCheck("M248-A012-DOC-PKT-06", "`check:objc3c:m248-a012-lane-a-readiness`"),
    SnippetCheck("M248-A012-DOC-PKT-07", "`test:objc3c:parser-replay-proof`"),
    SnippetCheck("M248-A012-DOC-PKT-08", "`test:objc3c:parser-ast-extraction`"),
    SnippetCheck(
        "M248-A012-DOC-PKT-09",
        "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
    ),
)

A011_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-A012-DEP-01",
        "Contract ID: `objc3c-suite-partitioning-and-fixture-ownership-performance-and-quality-guardrails/m248-a011-v1`",
    ),
)

A011_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-A012-DEP-02", "Packet: `M248-A011`"),
    SnippetCheck("M248-A012-DEP-03", "Dependencies: `M248-A010`"),
)

A011_CHECKER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-A012-DEP-04",
        "m248-a011-suite-partitioning-fixture-ownership-performance-and-quality-guardrails-contract-v1",
    ),
)

A011_TOOLING_TEST_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-A012-DEP-05",
        "check_m248_a011_suite_partitioning_and_fixture_ownership_performance_and_quality_guardrails_contract",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-A012-ARCH-01",
        "M248 lane-A A008 suite partitioning and fixture ownership recovery and determinism hardening",
    ),
    SnippetCheck(
        "M248-A012-ARCH-02",
        "docs/contracts/m248_suite_partitioning_and_fixture_ownership_recovery_and_determinism_hardening_a008_expectations.md",
    ),
    SnippetCheck(
        "M248-A012-ARCH-03",
        "and fail-closed against `M248-A007` dependency drift.",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-A012-SPC-01",
        "suite partitioning and fixture ownership governance shall preserve explicit",
    ),
    SnippetCheck(
        "M248-A012-SPC-02",
        "lane-A dependency boundary anchors and fail closed on fixture partition drift",
    ),
    SnippetCheck(
        "M248-A012-SPC-03",
        "suite partitioning and fixture ownership recovery and determinism hardening governance",
    ),
    SnippetCheck(
        "M248-A012-SPC-04",
        "closed on recovery and determinism hardening evidence drift before downstream",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-A012-META-01",
        "deterministic lane-A suite partitioning and fixture ownership recovery and determinism hardening metadata anchors for `M248-A008`",
    ),
    SnippetCheck(
        "M248-A012-META-02",
        "explicit `M248-A007` dependency continuity and fail-closed recovery/determinism evidence continuity",
    ),
)

PACKAGE_SCRIPT_KEY_CHECKS: tuple[PackageScriptKeyCheck, ...] = (
    PackageScriptKeyCheck(
        "M248-A012-PKG-01",
        "check:objc3c:m248-a012-suite-partitioning-fixture-ownership-cross-lane-integration-sync-contract",
    ),
    PackageScriptKeyCheck(
        "M248-A012-PKG-02",
        "test:tooling:m248-a012-suite-partitioning-fixture-ownership-cross-lane-integration-sync-contract",
    ),
    PackageScriptKeyCheck("M248-A012-PKG-03", "check:objc3c:m248-a012-lane-a-readiness"),
    PackageScriptKeyCheck("M248-A012-PKG-04", "test:objc3c:parser-replay-proof"),
    PackageScriptKeyCheck("M248-A012-PKG-05", "test:objc3c:parser-ast-extraction"),
)

PACKAGE_SCRIPT_CHECKS: tuple[PackageScriptCheck, ...] = (
    PackageScriptCheck(
        "M248-A012-PKG-06",
        "check:objc3c:m248-a012-suite-partitioning-fixture-ownership-cross-lane-integration-sync-contract",
        "python scripts/check_m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_contract.py",
    ),
    PackageScriptCheck(
        "M248-A012-PKG-07",
        "test:tooling:m248-a012-suite-partitioning-fixture-ownership-cross-lane-integration-sync-contract",
        "python -m pytest tests/tooling/test_check_m248_a012_suite_partitioning_and_fixture_ownership_cross_lane_integration_sync_contract.py -q",
    ),
    PackageScriptCheck(
        "M248-A012-PKG-08",
        "check:objc3c:m248-a012-lane-a-readiness",
        "npm run check:objc3c:m248-a011-lane-a-readiness && npm run check:objc3c:m248-a012-suite-partitioning-fixture-ownership-cross-lane-integration-sync-contract && npm run test:tooling:m248-a012-suite-partitioning-fixture-ownership-cross-lane-integration-sync-contract",
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
    parser.add_argument("--a011-expectations-doc", type=Path, default=DEFAULT_A011_EXPECTATIONS_DOC)
    parser.add_argument("--a011-packet-doc", type=Path, default=DEFAULT_A011_PACKET_DOC)
    parser.add_argument("--a011-checker", type=Path, default=DEFAULT_A011_CHECKER)
    parser.add_argument("--a011-tooling-test", type=Path, default=DEFAULT_A011_TOOLING_TEST)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
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


def check_package_contract(path: Path) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists() or not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id="M248-A012-PKG-00",
                detail=f"required document is missing: {display_path(path)}",
            )
        )
        return checks_total, findings

    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except (json.JSONDecodeError, OSError) as exc:
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id="M248-A012-PKG-00",
                detail=f"unable to parse package.json: {exc}",
            )
        )
        return checks_total, findings

    scripts = payload.get("scripts")
    checks_total += 1
    if not isinstance(scripts, dict):
        findings.append(
            Finding(
                artifact="package_json",
                check_id="M248-A012-PKG-00",
                detail='expected top-level "scripts" object in package.json',
            )
        )
        return checks_total, findings

    for key_check in PACKAGE_SCRIPT_KEY_CHECKS:
        checks_total += 1
        if key_check.script_key not in scripts:
            findings.append(
                Finding(
                    artifact="package_json",
                    check_id=key_check.check_id,
                    detail=f'expected scripts["{key_check.script_key}"] to exist',
                )
            )

    for script_check in PACKAGE_SCRIPT_CHECKS:
        checks_total += 1
        actual = scripts.get(script_check.script_key)
        if actual != script_check.expected_value:
            findings.append(
                Finding(
                    artifact="package_json",
                    check_id=script_check.check_id,
                    detail=(
                        f'expected scripts["{script_check.script_key}"] to equal '
                        f'"{script_check.expected_value}"'
                    ),
                )
            )
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    checks_total = 0
    failures: list[Finding] = []

    for artifact_name, path, exists_check_id, snippets in (
        ("expectations_doc", args.expectations_doc, "M248-A012-DOC-EXP-00", EXPECTATIONS_SNIPPETS),
        ("packet_doc", args.packet_doc, "M248-A012-DOC-PKT-00", PACKET_SNIPPETS),
        ("a011_expectations_doc", args.a011_expectations_doc, "M248-A012-DEP-00", A011_EXPECTATIONS_SNIPPETS),
        ("a011_packet_doc", args.a011_packet_doc, "M248-A012-DEP-00", A011_PACKET_SNIPPETS),
        ("a011_checker", args.a011_checker, "M248-A012-DEP-00", A011_CHECKER_SNIPPETS),
        ("a011_tooling_test", args.a011_tooling_test, "M248-A012-DEP-00", A011_TOOLING_TEST_SNIPPETS),
        ("architecture_doc", args.architecture_doc, "M248-A012-ARCH-00", ARCHITECTURE_SNIPPETS),
        ("lowering_spec", args.lowering_spec, "M248-A012-SPC-00", LOWERING_SPEC_SNIPPETS),
        ("metadata_spec", args.metadata_spec, "M248-A012-META-00", METADATA_SPEC_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            artifact_name=artifact_name,
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    count, findings = check_package_contract(args.package_json)
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
