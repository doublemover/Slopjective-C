#!/usr/bin/env python3
"""Fail-closed checker for M248-E008 lane-E CI governance gate/closeout policy recovery and determinism hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m248-e008-lane-e-ci-governance-gate-closeout-policy-recovery-and-determinism-hardening-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_lane_e_ci_governance_gate_and_closeout_policy_recovery_and_determinism_hardening_e008_expectations.md"
)
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m248/M248-E008/lane_e_ci_governance_gate_closeout_policy_recovery_and_determinism_hardening_summary.json"
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
        "M248-E008-DEP-E007-01",
        Path("docs/contracts/m248_lane_e_ci_governance_gate_and_closeout_policy_diagnostics_hardening_e007_expectations.md"),
    ),
    AssetCheck(
        "M248-E008-DEP-E007-02",
        Path("scripts/check_m248_e007_ci_governance_gate_and_closeout_policy_diagnostics_hardening_contract.py"),
    ),
    AssetCheck(
        "M248-E008-DEP-E007-03",
        Path("tests/tooling/test_check_m248_e007_ci_governance_gate_and_closeout_policy_diagnostics_hardening_contract.py"),
    ),
    AssetCheck(
        "M248-E008-DEP-A003-01",
        Path("docs/contracts/m248_suite_partitioning_and_fixture_ownership_core_feature_implementation_a003_expectations.md"),
    ),
    AssetCheck(
        "M248-E008-DEP-A003-02",
        Path("scripts/check_m248_a003_suite_partitioning_and_fixture_ownership_core_feature_implementation_contract.py"),
    ),
    AssetCheck(
        "M248-E008-DEP-A003-03",
        Path("tests/tooling/test_check_m248_a003_suite_partitioning_and_fixture_ownership_core_feature_implementation_contract.py"),
    ),
    AssetCheck(
        "M248-E008-DEP-B004-01",
        Path("docs/contracts/m248_semantic_lowering_test_architecture_core_feature_expansion_b004_expectations.md"),
    ),
    AssetCheck(
        "M248-E008-DEP-B004-02",
        Path("spec/planning/compiler/m248/m248_b004_semantic_lowering_test_architecture_core_feature_expansion_packet.md"),
    ),
    AssetCheck(
        "M248-E008-DEP-B004-03",
        Path("scripts/check_m248_b004_semantic_lowering_test_architecture_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M248-E008-DEP-B004-04",
        Path("tests/tooling/test_check_m248_b004_semantic_lowering_test_architecture_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M248-E008-DEP-C004-01",
        Path("docs/contracts/m248_replay_harness_and_artifact_contracts_core_feature_expansion_c004_expectations.md"),
    ),
    AssetCheck(
        "M248-E008-DEP-C004-02",
        Path("spec/planning/compiler/m248/m248_c004_replay_harness_and_artifact_contracts_core_feature_expansion_packet.md"),
    ),
    AssetCheck(
        "M248-E008-DEP-C004-03",
        Path("scripts/check_m248_c004_replay_harness_and_artifact_contracts_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M248-E008-DEP-C004-04",
        Path("tests/tooling/test_check_m248_c004_replay_harness_and_artifact_contracts_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M248-E008-DEP-D006-01",
        Path("docs/contracts/m248_runner_reliability_and_platform_operations_edge_case_expansion_and_robustness_d006_expectations.md"),
    ),
    AssetCheck(
        "M248-E008-DEP-D006-02",
        Path("spec/planning/compiler/m248/m248_d006_runner_reliability_and_platform_operations_edge_case_expansion_and_robustness_packet.md"),
    ),
    AssetCheck(
        "M248-E008-DEP-D006-03",
        Path("scripts/check_m248_d006_runner_reliability_and_platform_operations_edge_case_expansion_and_robustness_contract.py"),
    ),
    AssetCheck(
        "M248-E008-DEP-D006-04",
        Path("tests/tooling/test_check_m248_d006_runner_reliability_and_platform_operations_edge_case_expansion_and_robustness_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-E008-DOC-EXP-01",
        "# M248 Lane E CI Governance Gate and Closeout Policy Recovery and Determinism Hardening Expectations (E008)",
    ),
    SnippetCheck(
        "M248-E008-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-ci-governance-gate-closeout-policy-recovery-and-determinism-hardening/m248-e008-v1`",
    ),
    SnippetCheck(
        "M248-E008-DOC-EXP-03",
        "Issue `#6868` defines canonical lane-E recovery and determinism hardening scope.",
    ),
    SnippetCheck(
        "M248-E008-DOC-EXP-04",
        "Dependencies: `M248-E007`, `M248-A003`, `M248-B004`, `M248-C004`, `M248-D006`",
    ),
    SnippetCheck(
        "M248-E008-DOC-EXP-05",
        "Fail closed unless M248 lane-E recovery and determinism hardening dependency",
    ),
    SnippetCheck(
        "M248-E008-DOC-EXP-06",
        "Readiness command chain enforces E007 and lane A/B/C/D dependency",
    ),
    SnippetCheck(
        "M248-E008-DOC-EXP-07",
        "prerequisites before E008 evidence checks run.",
    ),
    SnippetCheck(
        "M248-E008-DOC-EXP-08",
        "`check:objc3c:m248-e008-ci-governance-gate-closeout-policy-recovery-determinism-hardening-contract`",
    ),
    SnippetCheck(
        "M248-E008-DOC-EXP-09",
        "`test:tooling:m248-e008-ci-governance-gate-closeout-policy-recovery-determinism-hardening-contract`",
    ),
    SnippetCheck("M248-E008-DOC-EXP-10", "`check:objc3c:m248-e008-lane-e-readiness`"),
    SnippetCheck("M248-E008-DOC-EXP-11", "`compile:objc3c`"),
    SnippetCheck("M248-E008-DOC-EXP-12", "`proof:objc3c`"),
    SnippetCheck("M248-E008-DOC-EXP-13", "`test:objc3c:execution-replay-proof`"),
    SnippetCheck("M248-E008-DOC-EXP-14", "`test:objc3c:perf-budget`"),
    SnippetCheck(
        "M248-E008-DOC-EXP-15",
        "`tmp/reports/m248/M248-E008/lane_e_ci_governance_gate_closeout_policy_recovery_and_determinism_hardening_summary.json`",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-E008-PKG-01",
        '"check:objc3c:m248-e008-ci-governance-gate-closeout-policy-recovery-determinism-hardening-contract": '
        '"python scripts/check_m248_e008_ci_governance_gate_and_closeout_policy_recovery_and_determinism_hardening_contract.py"',
    ),
    SnippetCheck(
        "M248-E008-PKG-02",
        '"test:tooling:m248-e008-ci-governance-gate-closeout-policy-recovery-determinism-hardening-contract": '
        '"python -m pytest tests/tooling/test_check_m248_e008_ci_governance_gate_and_closeout_policy_recovery_and_determinism_hardening_contract.py -q"',
    ),
    SnippetCheck(
        "M248-E008-PKG-03",
        '"check:objc3c:m248-e008-lane-e-readiness": '
        '"npm run check:objc3c:m248-e007-lane-e-readiness '
        "&& npm run check:objc3c:m248-a003-lane-a-readiness "
        "&& npm run check:objc3c:m248-b004-lane-b-readiness "
        "&& npm run check:objc3c:m248-c004-lane-c-readiness "
        "&& npm run check:objc3c:m248-d006-lane-d-readiness "
        "&& npm run check:objc3c:m248-e008-ci-governance-gate-closeout-policy-recovery-determinism-hardening-contract "
        '&& npm run test:tooling:m248-e008-ci-governance-gate-closeout-policy-recovery-determinism-hardening-contract"',
    ),
    SnippetCheck("M248-E008-PKG-04", '"compile:objc3c": '),
    SnippetCheck("M248-E008-PKG-05", '"proof:objc3c": '),
    SnippetCheck("M248-E008-PKG-06", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M248-E008-PKG-07", '"test:objc3c:perf-budget": '),
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


def check_text_contract(
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
                f"required text artifact is missing: {display_path(path)}",
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


def finding_sort_key(finding: Finding) -> tuple[str, str, str]:
    return (finding.artifact, finding.check_id, finding.detail)


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    dep_checks, dep_failures = check_prerequisite_assets()
    checks_total += dep_checks
    failures.extend(dep_failures)

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M248-E008-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.package_json, "M248-E008-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_text_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    failures = sorted(failures, key=finding_sort_key)
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
