#!/usr/bin/env python3
"""Fail-closed contract checker for M248-B015 semantic/lowering advanced core workpack shard1."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m248-b015-semantic-lowering-test-architecture-advanced-core-workpack-shard1-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_semantic_lowering_test_architecture_advanced_core_workpack_shard1_b015_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_b015_semantic_lowering_test_architecture_advanced_core_workpack_shard1_packet.md"
)
DEFAULT_B014_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_b014_expectations.md"
)
DEFAULT_B014_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_b014_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_packet.md"
)
DEFAULT_B014_CHECKER = (
    ROOT
    / "scripts"
    / "check_m248_b014_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_contract.py"
)
DEFAULT_B014_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m248_b014_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_contract.py"
)
DEFAULT_B014_READINESS_RUNNER = ROOT / "scripts" / "run_m248_b014_lane_b_readiness.py"
DEFAULT_B014_RUN_SCRIPT = (
    ROOT
    / "scripts"
    / "run_m248_b014_semantic_lowering_test_architecture_release_replay_dry_run.ps1"
)
DEFAULT_B015_READINESS_RUNNER = ROOT / "scripts" / "run_m248_b015_lane_b_readiness.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m248/M248-B015/"
    "semantic_lowering_test_architecture_advanced_core_workpack_shard1_contract_summary.json"
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
        "M248-B015-DOC-EXP-01",
        "# M248 Semantic/Lowering Test Architecture Advanced Core Workpack (Shard 1) Expectations (B015)",
    ),
    SnippetCheck(
        "M248-B015-DOC-EXP-02",
        "Contract ID: `objc3c-semantic-lowering-test-architecture-advanced-core-workpack-shard1/m248-b015-v1`",
    ),
    SnippetCheck("M248-B015-DOC-EXP-03", "- Dependencies: `M248-B014`"),
    SnippetCheck(
        "M248-B015-DOC-EXP-04",
        "Issue `#6815` defines canonical lane-B advanced core workpack (shard 1) scope.",
    ),
    SnippetCheck(
        "M248-B015-DOC-EXP-05",
        "including code/spec anchors and milestone optimization improvements as",
    ),
    SnippetCheck(
        "M248-B015-DOC-EXP-06",
        "`scripts/check_m248_b015_semantic_lowering_test_architecture_advanced_core_workpack_shard1_contract.py`",
    ),
    SnippetCheck(
        "M248-B015-DOC-EXP-07",
        "`tests/tooling/test_check_m248_b015_semantic_lowering_test_architecture_advanced_core_workpack_shard1_contract.py`",
    ),
    SnippetCheck(
        "M248-B015-DOC-EXP-08",
        "`check:objc3c:m248-b015-lane-b-readiness`",
    ),
    SnippetCheck(
        "M248-B015-DOC-EXP-09",
        "python scripts/check_m248_b015_semantic_lowering_test_architecture_advanced_core_workpack_shard1_contract.py --emit-json",
    ),
    SnippetCheck(
        "M248-B015-DOC-EXP-10",
        "python scripts/run_m248_b015_lane_b_readiness.py",
    ),
    SnippetCheck(
        "M248-B015-DOC-EXP-11",
        "`tmp/reports/m248/M248-B015/semantic_lowering_test_architecture_advanced_core_workpack_shard1_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-B015-DOC-PKT-01",
        "# M248-B015 Semantic/Lowering Test Architecture Advanced Core Workpack (Shard 1) Packet",
    ),
    SnippetCheck("M248-B015-DOC-PKT-02", "Packet: `M248-B015`"),
    SnippetCheck("M248-B015-DOC-PKT-03", "Issue: `#6815`"),
    SnippetCheck("M248-B015-DOC-PKT-04", "Dependencies: `M248-B014`"),
    SnippetCheck(
        "M248-B015-DOC-PKT-05",
        "`scripts/check_m248_b015_semantic_lowering_test_architecture_advanced_core_workpack_shard1_contract.py`",
    ),
    SnippetCheck(
        "M248-B015-DOC-PKT-06",
        "`tests/tooling/test_check_m248_b015_semantic_lowering_test_architecture_advanced_core_workpack_shard1_contract.py`",
    ),
    SnippetCheck(
        "M248-B015-DOC-PKT-07",
        "`scripts/run_m248_b015_lane_b_readiness.py`",
    ),
    SnippetCheck(
        "M248-B015-DOC-PKT-08",
        "`check:objc3c:m248-b015-lane-b-readiness`",
    ),
    SnippetCheck(
        "M248-B015-DOC-PKT-09",
        "`test:objc3c:sema-pass-manager-diagnostics-bus`",
    ),
    SnippetCheck(
        "M248-B015-DOC-PKT-10",
        "`test:objc3c:lowering-regression`",
    ),
)

B014_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-B015-B014-DOC-01",
        "Contract ID: `objc3c-semantic-lowering-test-architecture-release-candidate-replay-dry-run/m248-b014-v1`",
    ),
    SnippetCheck("M248-B015-B014-DOC-02", "- Dependencies: `M248-B013`"),
)

B014_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-B015-B014-PKT-01", "Packet: `M248-B014`"),
    SnippetCheck("M248-B015-B014-PKT-02", "Issue: `#6814`"),
    SnippetCheck("M248-B015-B014-PKT-03", "Dependencies: `M248-B013`"),
)

B015_RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-B015-RUN-01", "scripts/run_m248_b014_lane_b_readiness.py"),
    SnippetCheck(
        "M248-B015-RUN-02",
        "scripts/check_m248_b015_semantic_lowering_test_architecture_advanced_core_workpack_shard1_contract.py",
    ),
    SnippetCheck(
        "M248-B015-RUN-03",
        "tests/tooling/test_check_m248_b015_semantic_lowering_test_architecture_advanced_core_workpack_shard1_contract.py",
    ),
    SnippetCheck("M248-B015-RUN-04", '"-m"'),
    SnippetCheck("M248-B015-RUN-05", "[ok] M248-B015 lane-B readiness chain completed"),
)

B015_RUNNER_FORBIDDEN_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-B015-RUN-FORB-01", "npm run"),
)

PACKAGE_SCRIPT_KEY_CHECKS: tuple[PackageScriptKeyCheck, ...] = (
    PackageScriptKeyCheck(
        "M248-B015-CFG-01",
        "check:objc3c:m248-b015-semantic-lowering-test-architecture-advanced-core-workpack-shard1-contract",
    ),
    PackageScriptKeyCheck(
        "M248-B015-CFG-02",
        "test:tooling:m248-b015-semantic-lowering-test-architecture-advanced-core-workpack-shard1-contract",
    ),
    PackageScriptKeyCheck("M248-B015-CFG-03", "check:objc3c:m248-b015-lane-b-readiness"),
    PackageScriptKeyCheck("M248-B015-CFG-04", "check:objc3c:m248-b014-lane-b-readiness"),
)

PACKAGE_SCRIPT_CHECKS: tuple[PackageScriptCheck, ...] = (
    PackageScriptCheck(
        "M248-B015-CFG-05",
        "check:objc3c:m248-b015-semantic-lowering-test-architecture-advanced-core-workpack-shard1-contract",
        "python scripts/check_m248_b015_semantic_lowering_test_architecture_advanced_core_workpack_shard1_contract.py",
    ),
    PackageScriptCheck(
        "M248-B015-CFG-06",
        "test:tooling:m248-b015-semantic-lowering-test-architecture-advanced-core-workpack-shard1-contract",
        "python -m pytest tests/tooling/test_check_m248_b015_semantic_lowering_test_architecture_advanced_core_workpack_shard1_contract.py -q",
    ),
    PackageScriptCheck(
        "M248-B015-CFG-07",
        "check:objc3c:m248-b015-lane-b-readiness",
        "python scripts/run_m248_b015_lane_b_readiness.py",
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
    parser.add_argument("--b014-expectations-doc", type=Path, default=DEFAULT_B014_EXPECTATIONS_DOC)
    parser.add_argument("--b014-packet-doc", type=Path, default=DEFAULT_B014_PACKET_DOC)
    parser.add_argument("--b014-checker", type=Path, default=DEFAULT_B014_CHECKER)
    parser.add_argument("--b014-test", type=Path, default=DEFAULT_B014_TEST)
    parser.add_argument("--b014-readiness-runner", type=Path, default=DEFAULT_B014_READINESS_RUNNER)
    parser.add_argument("--b014-run-script", type=Path, default=DEFAULT_B014_RUN_SCRIPT)
    parser.add_argument("--b015-readiness-runner", type=Path, default=DEFAULT_B015_READINESS_RUNNER)
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


def check_forbidden_snippets(
    *,
    path: Path,
    snippets: tuple[SnippetCheck, ...],
) -> tuple[int, list[Finding]]:
    checks_total = 0
    findings: list[Finding] = []
    if not path.exists() or not path.is_file():
        for snippet in snippets:
            checks_total += 1
            findings.append(
                Finding(
                    display_path(path),
                    snippet.check_id,
                    f"required path missing before forbidden-snippet check: {display_path(path)}",
                )
            )
        return checks_total, findings
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet in text:
            findings.append(
                Finding(
                    display_path(path),
                    snippet.check_id,
                    f"forbidden snippet present: {snippet.snippet}",
                )
            )
    return checks_total, findings


def check_dependency_paths(paths: tuple[tuple[Path, str], ...]) -> tuple[int, list[Finding]]:
    checks_total = 0
    failures: list[Finding] = []
    for path, check_id in paths:
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
    return checks_total, failures


def check_package_contract(path: Path) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists() or not path.is_file():
        findings.append(
            Finding(
                display_path(path),
                "M248-B015-CFG-00",
                f"required package manifest is missing: {display_path(path)}",
            )
        )
        return checks_total, findings

    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        findings.append(
            Finding(
                display_path(path),
                "M248-B015-CFG-00",
                f"unable to parse package manifest: {exc}",
            )
        )
        return checks_total, findings

    scripts = payload.get("scripts")
    checks_total += 1
    if not isinstance(scripts, dict):
        findings.append(
            Finding(
                "package_json",
                "M248-B015-CFG-00",
                'expected top-level "scripts" object in package.json',
            )
        )
        return checks_total, findings

    for key_check in PACKAGE_SCRIPT_KEY_CHECKS:
        checks_total += 1
        if key_check.script_key not in scripts:
            findings.append(
                Finding(
                    "package_json",
                    key_check.check_id,
                    f'expected scripts["{key_check.script_key}"] to exist',
                )
            )

    for script_check in PACKAGE_SCRIPT_CHECKS:
        checks_total += 1
        actual = scripts.get(script_check.script_key)
        if actual != script_check.expected_value:
            findings.append(
                Finding(
                    "package_json",
                    script_check.check_id,
                    (
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

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M248-B015-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M248-B015-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.b014_expectations_doc, "M248-B015-B014-DOC-EXISTS", B014_EXPECTATIONS_SNIPPETS),
        (args.b014_packet_doc, "M248-B015-B014-PKT-EXISTS", B014_PACKET_SNIPPETS),
        (args.b015_readiness_runner, "M248-B015-RUN-EXISTS", B015_RUNNER_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    checks, findings = check_forbidden_snippets(
        path=args.b015_readiness_runner,
        snippets=B015_RUNNER_FORBIDDEN_SNIPPETS,
    )
    checks_total += checks
    failures.extend(findings)

    count, findings = check_dependency_paths(
        (
            (args.b014_checker, "M248-B015-DEP-B014-01"),
            (args.b014_test, "M248-B015-DEP-B014-02"),
            (args.b014_readiness_runner, "M248-B015-DEP-B014-03"),
            (args.b014_run_script, "M248-B015-DEP-B014-04"),
        )
    )
    checks_total += count
    failures.extend(findings)

    count, findings = check_package_contract(args.package_json)
    checks_total += count
    failures.extend(findings)

    failures = sorted(failures, key=lambda failure: (failure.artifact, failure.check_id, failure.detail))
    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
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
        print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
