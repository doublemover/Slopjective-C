#!/usr/bin/env python3
"""Fail-closed checker for M248-A005 suite partitioning edge-case and compatibility completion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m248-a005-suite-partitioning-fixture-ownership-edge-case-and-compatibility-completion-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_suite_partitioning_and_fixture_ownership_edge_case_and_compatibility_completion_a005_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_a005_suite_partitioning_and_fixture_ownership_edge_case_and_compatibility_completion_packet.md"
)
DEFAULT_A004_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_suite_partitioning_and_fixture_ownership_core_feature_expansion_a004_expectations.md"
)
DEFAULT_A004_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_a004_suite_partitioning_and_fixture_ownership_core_feature_expansion_packet.md"
)
DEFAULT_A004_CHECKER = (
    ROOT / "scripts" / "check_m248_a004_suite_partitioning_and_fixture_ownership_core_feature_expansion_contract.py"
)
DEFAULT_A004_TEST = (
    ROOT / "tests" / "tooling" / "test_check_m248_a004_suite_partitioning_and_fixture_ownership_core_feature_expansion_contract.py"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m248/M248-A005/"
    "suite_partitioning_and_fixture_ownership_edge_case_and_compatibility_completion_contract_summary.json"
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
        "M248-A005-DOC-EXP-01",
        "# M248 Suite Partitioning and Fixture Ownership Edge-Case and Compatibility Completion Expectations (A005)",
    ),
    SnippetCheck(
        "M248-A005-DOC-EXP-02",
        "Contract ID: `objc3c-suite-partitioning-and-fixture-ownership-edge-case-and-compatibility-completion/m248-a005-v1`",
    ),
    SnippetCheck("M248-A005-DOC-EXP-03", "Dependencies: `M248-A004`"),
    SnippetCheck("M248-A005-DOC-EXP-04", "Issue `#6792` defines canonical lane-A edge-case and compatibility completion scope."),
    SnippetCheck(
        "M248-A005-DOC-EXP-05",
        "edge-case and compatibility completion dependency anchors remain explicit, deterministic, and traceable",
    ),
    SnippetCheck(
        "M248-A005-DOC-EXP-06",
        "docs/contracts/m248_suite_partitioning_and_fixture_ownership_core_feature_expansion_a004_expectations.md",
    ),
    SnippetCheck(
        "M248-A005-DOC-EXP-07",
        "scripts/check_m248_a004_suite_partitioning_and_fixture_ownership_core_feature_expansion_contract.py",
    ),
    SnippetCheck(
        "M248-A005-DOC-EXP-08",
        "tests/tooling/test_check_m248_a004_suite_partitioning_and_fixture_ownership_core_feature_expansion_contract.py",
    ),
    SnippetCheck("M248-A005-DOC-EXP-09", "`check:objc3c:m248-a005-lane-a-readiness`"),
    SnippetCheck("M248-A005-DOC-EXP-10", "`check:objc3c:m248-a004-lane-a-readiness`"),
    SnippetCheck(
        "M248-A005-DOC-EXP-11",
        "`tmp/reports/m248/M248-A005/suite_partitioning_and_fixture_ownership_edge_case_and_compatibility_completion_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-A005-DOC-PKT-01",
        "# M248-A005 Suite Partitioning and Fixture Ownership Edge-Case and Compatibility Completion Packet",
    ),
    SnippetCheck("M248-A005-DOC-PKT-02", "Packet: `M248-A005`"),
    SnippetCheck("M248-A005-DOC-PKT-03", "Issue: `#6792`"),
    SnippetCheck("M248-A005-DOC-PKT-04", "Dependencies: `M248-A004`"),
    SnippetCheck(
        "M248-A005-DOC-PKT-05",
        "`scripts/check_m248_a005_suite_partitioning_and_fixture_ownership_edge_case_and_compatibility_completion_contract.py`",
    ),
    SnippetCheck(
        "M248-A005-DOC-PKT-06",
        "`tests/tooling/test_check_m248_a005_suite_partitioning_and_fixture_ownership_edge_case_and_compatibility_completion_contract.py`",
    ),
)

A004_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-A005-A004-DOC-01",
        "# M248 Suite Partitioning and Fixture Ownership Core Feature Expansion Expectations (A004)",
    ),
    SnippetCheck(
        "M248-A005-A004-DOC-02",
        "Contract ID: `objc3c-suite-partitioning-and-fixture-ownership-core-feature-expansion/m248-a004-v1`",
    ),
    SnippetCheck("M248-A005-A004-DOC-03", "Dependencies: `M248-A003`"),
)

A004_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-A005-A004-PKT-01", "Packet: `M248-A004`"),
    SnippetCheck("M248-A005-A004-PKT-02", "Issue: `#6791`"),
    SnippetCheck("M248-A005-A004-PKT-03", "Dependencies: `M248-A003`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-A005-ARCH-01",
        "M248 lane-A A001 suite partitioning and fixture ownership anchors",
    ),
    SnippetCheck(
        "M248-A005-ARCH-02",
        "docs/contracts/m248_suite_partitioning_and_fixture_ownership_contract_freeze_a001_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-A005-SPC-01",
        "suite partitioning and fixture ownership governance shall preserve explicit",
    ),
    SnippetCheck(
        "M248-A005-SPC-02",
        "dependency boundary anchors and fail closed on fixture partition drift",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-A005-META-01",
        "deterministic lane-A suite partitioning metadata anchors for `M248-A001`",
    ),
    SnippetCheck(
        "M248-A005-META-02",
        "fixture ownership boundary evidence and parser replay-budget continuity",
    ),
)

PACKAGE_SCRIPT_KEY_CHECKS: tuple[PackageScriptKeyCheck, ...] = (
    PackageScriptKeyCheck(
        "M248-A005-PKG-01",
        "check:objc3c:m248-a005-suite-partitioning-fixture-ownership-edge-case-and-compatibility-completion-contract",
    ),
    PackageScriptKeyCheck(
        "M248-A005-PKG-02",
        "test:tooling:m248-a005-suite-partitioning-fixture-ownership-edge-case-and-compatibility-completion-contract",
    ),
    PackageScriptKeyCheck("M248-A005-PKG-03", "check:objc3c:m248-a005-lane-a-readiness"),
    PackageScriptKeyCheck("M248-A005-PKG-04", "test:objc3c:parser-replay-proof"),
    PackageScriptKeyCheck("M248-A005-PKG-05", "test:objc3c:parser-ast-extraction"),
)

PACKAGE_SCRIPT_CHECKS: tuple[PackageScriptCheck, ...] = (
    PackageScriptCheck(
        "M248-A005-PKG-06",
        "check:objc3c:m248-a005-suite-partitioning-fixture-ownership-edge-case-and-compatibility-completion-contract",
        "python scripts/check_m248_a005_suite_partitioning_and_fixture_ownership_edge_case_and_compatibility_completion_contract.py",
    ),
    PackageScriptCheck(
        "M248-A005-PKG-07",
        "test:tooling:m248-a005-suite-partitioning-fixture-ownership-edge-case-and-compatibility-completion-contract",
        "python -m pytest tests/tooling/test_check_m248_a005_suite_partitioning_and_fixture_ownership_edge_case_and_compatibility_completion_contract.py -q",
    ),
    PackageScriptCheck(
        "M248-A005-PKG-08",
        "check:objc3c:m248-a005-lane-a-readiness",
        "npm run check:objc3c:m248-a004-lane-a-readiness && npm run check:objc3c:m248-a005-suite-partitioning-fixture-ownership-edge-case-and-compatibility-completion-contract && npm run test:tooling:m248-a005-suite-partitioning-fixture-ownership-edge-case-and-compatibility-completion-contract",
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
    parser.add_argument("--a004-expectations-doc", type=Path, default=DEFAULT_A004_EXPECTATIONS_DOC)
    parser.add_argument("--a004-packet-doc", type=Path, default=DEFAULT_A004_PACKET_DOC)
    parser.add_argument("--a004-checker", type=Path, default=DEFAULT_A004_CHECKER)
    parser.add_argument("--a004-test", type=Path, default=DEFAULT_A004_TEST)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit canonical summary JSON to stdout.",
    )
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
                check_id="M248-A005-PKG-00",
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
                check_id="M248-A005-PKG-00",
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
                check_id="M248-A005-PKG-00",
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

    for path, check_id in (
        (args.a004_checker, "M248-A005-DEP-A004-ARG-01"),
        (args.a004_test, "M248-A005-DEP-A004-ARG-02"),
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

    for artifact_name, path, exists_check_id, snippets in (
        ("expectations_doc", args.expectations_doc, "M248-A005-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        ("packet_doc", args.packet_doc, "M248-A005-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        ("a004_expectations_doc", args.a004_expectations_doc, "M248-A005-A004-DOC-EXISTS", A004_EXPECTATIONS_SNIPPETS),
        ("a004_packet_doc", args.a004_packet_doc, "M248-A005-A004-PKT-EXISTS", A004_PACKET_SNIPPETS),
        ("architecture_doc", args.architecture_doc, "M248-A005-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        ("lowering_spec", args.lowering_spec, "M248-A005-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        ("metadata_spec", args.metadata_spec, "M248-A005-META-EXISTS", METADATA_SPEC_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            artifact_name=artifact_name,
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    package_count, package_failures = check_package_contract(args.package_json)
    checks_total += package_count
    failures.extend(package_failures)

    failures = sorted(failures, key=lambda finding: (finding.check_id, finding.artifact, finding.detail))
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
        "failures": [
            {"artifact": failure.artifact, "check_id": failure.check_id, "detail": failure.detail}
            for failure in failures
        ],
    }

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")

    if args.emit_json:
        print(canonical_json(summary_payload), end="")

    if failures:
        if not args.emit_json:
            for finding in failures:
                print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1

    if not args.emit_json:
        print(f"[ok] {MODE}: {summary_payload['checks_passed']}/{summary_payload['checks_total']} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
