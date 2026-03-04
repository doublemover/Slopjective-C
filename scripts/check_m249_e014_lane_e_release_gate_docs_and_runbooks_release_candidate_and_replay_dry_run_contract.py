#!/usr/bin/env python3
"""Fail-closed checker for M249-E014 lane-E release-candidate replay dry-run gate."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m249-e014-lane-e-release-gate-docs-runbooks-release-candidate-and-replay-dry-run-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m249_lane_e_release_gate_docs_and_runbooks_release_candidate_and_replay_dry_run_e014_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m249"
    / "m249_e014_lane_e_release_gate_docs_and_runbooks_release_candidate_and_replay_dry_run_packet.md"
)
DEFAULT_RUNNER_SCRIPT = ROOT / "scripts" / "run_m249_e014_lane_e_readiness.py"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m249/M249-E014/lane_e_release_gate_docs_and_runbooks_release_candidate_and_replay_dry_run_summary.json"
)


@dataclass(frozen=True)
class AssetCheck:
    check_id: str
    lane_task: str
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
        "M249-E014-E013-01",
        "M249-E013",
        Path(
            "docs/contracts/m249_lane_e_release_gate_docs_and_runbooks_docs_and_operator_runbook_synchronization_e013_expectations.md"
        ),
    ),
    AssetCheck(
        "M249-E014-E013-02",
        "M249-E013",
        Path(
            "spec/planning/compiler/m249/m249_e013_lane_e_release_gate_docs_and_runbooks_docs_and_operator_runbook_synchronization_packet.md"
        ),
    ),
    AssetCheck(
        "M249-E014-E013-03",
        "M249-E013",
        Path(
            "scripts/check_m249_e013_lane_e_release_gate_docs_and_runbooks_docs_and_operator_runbook_synchronization_contract.py"
        ),
    ),
    AssetCheck(
        "M249-E014-E013-04",
        "M249-E013",
        Path(
            "tests/tooling/test_check_m249_e013_lane_e_release_gate_docs_and_runbooks_docs_and_operator_runbook_synchronization_contract.py"
        ),
    ),
    AssetCheck(
        "M249-E014-E013-05",
        "M249-E013",
        Path("scripts/run_m249_e013_lane_e_readiness.py"),
    ),
    AssetCheck(
        "M249-E014-A005-01",
        "M249-A005",
        Path(
            "docs/contracts/m249_feature_packaging_surface_and_compatibility_contracts_edge_case_and_compatibility_completion_a005_expectations.md"
        ),
    ),
    AssetCheck(
        "M249-E014-A005-02",
        "M249-A005",
        Path(
            "spec/planning/compiler/m249/m249_a005_feature_packaging_surface_and_compatibility_contracts_edge_case_and_compatibility_completion_packet.md"
        ),
    ),
    AssetCheck(
        "M249-E014-A005-03",
        "M249-A005",
        Path(
            "scripts/check_m249_a005_feature_packaging_surface_and_compatibility_contracts_edge_case_and_compatibility_completion_contract.py"
        ),
    ),
    AssetCheck(
        "M249-E014-A005-04",
        "M249-A005",
        Path(
            "tests/tooling/test_check_m249_a005_feature_packaging_surface_and_compatibility_contracts_edge_case_and_compatibility_completion_contract.py"
        ),
    ),
    AssetCheck(
        "M249-E014-A005-05",
        "M249-A005",
        Path("scripts/run_m249_a005_lane_a_readiness.py"),
    ),
    AssetCheck(
        "M249-E014-B006-01",
        "M249-B006",
        Path(
            "docs/contracts/m249_semantic_compatibility_and_migration_checks_edge_case_expansion_and_robustness_b006_expectations.md"
        ),
    ),
    AssetCheck(
        "M249-E014-B006-02",
        "M249-B006",
        Path(
            "spec/planning/compiler/m249/m249_b006_semantic_compatibility_and_migration_checks_edge_case_expansion_and_robustness_packet.md"
        ),
    ),
    AssetCheck(
        "M249-E014-B006-03",
        "M249-B006",
        Path(
            "scripts/check_m249_b006_semantic_compatibility_and_migration_checks_edge_case_expansion_and_robustness_contract.py"
        ),
    ),
    AssetCheck(
        "M249-E014-B006-04",
        "M249-B006",
        Path(
            "tests/tooling/test_check_m249_b006_semantic_compatibility_and_migration_checks_edge_case_expansion_and_robustness_contract.py"
        ),
    ),
    AssetCheck(
        "M249-E014-B006-05",
        "M249-B006",
        Path("scripts/run_m249_b006_lane_b_readiness.py"),
    ),
    AssetCheck(
        "M249-E014-C007-01",
        "M249-C007",
        Path(
            "docs/contracts/m249_ir_object_packaging_and_symbol_policy_diagnostics_hardening_c007_expectations.md"
        ),
    ),
    AssetCheck(
        "M249-E014-C007-02",
        "M249-C007",
        Path(
            "spec/planning/compiler/m249/m249_c007_ir_object_packaging_and_symbol_policy_diagnostics_hardening_packet.md"
        ),
    ),
    AssetCheck(
        "M249-E014-C007-03",
        "M249-C007",
        Path("scripts/check_m249_c007_ir_object_packaging_and_symbol_policy_diagnostics_hardening_contract.py"),
    ),
    AssetCheck(
        "M249-E014-C007-04",
        "M249-C007",
        Path(
            "tests/tooling/test_check_m249_c007_ir_object_packaging_and_symbol_policy_diagnostics_hardening_contract.py"
        ),
    ),
    AssetCheck(
        "M249-E014-D012-01",
        "M249-D012",
        Path(
            "docs/contracts/m249_installer_runtime_operations_and_support_tooling_cross_lane_integration_sync_d012_expectations.md"
        ),
    ),
    AssetCheck(
        "M249-E014-D012-02",
        "M249-D012",
        Path(
            "spec/planning/compiler/m249/m249_d012_installer_runtime_operations_and_support_tooling_cross_lane_integration_sync_packet.md"
        ),
    ),
    AssetCheck(
        "M249-E014-D012-03",
        "M249-D012",
        Path(
            "scripts/check_m249_d012_installer_runtime_operations_and_support_tooling_cross_lane_integration_sync_contract.py"
        ),
    ),
    AssetCheck(
        "M249-E014-D012-04",
        "M249-D012",
        Path(
            "tests/tooling/test_check_m249_d012_installer_runtime_operations_and_support_tooling_cross_lane_integration_sync_contract.py"
        ),
    ),
    AssetCheck(
        "M249-E014-D012-05",
        "M249-D012",
        Path("scripts/run_m249_d012_lane_d_readiness.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E014-DOC-EXP-01",
        "# M249 Lane E Release Gate, Docs, and Runbooks Release-Candidate and Replay Dry-Run Expectations (E014)",
    ),
    SnippetCheck(
        "M249-E014-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-release-gate-docs-runbooks-release-candidate-replay-dry-run/m249-e014-v1`",
    ),
    SnippetCheck("M249-E014-DOC-EXP-03", "- Issue: `#6961`"),
    SnippetCheck(
        "M249-E014-DOC-EXP-04",
        "Dependencies: `M249-E013`, `M249-A005`, `M249-B006`, `M249-C007`, `M249-D012`",
    ),
    SnippetCheck(
        "M249-E014-DOC-EXP-05",
        "`python scripts/check_m249_e013_lane_e_release_gate_docs_and_runbooks_docs_and_operator_runbook_synchronization_contract.py`",
    ),
    SnippetCheck(
        "M249-E014-DOC-EXP-06",
        "`python -m pytest tests/tooling/test_check_m249_e013_lane_e_release_gate_docs_and_runbooks_docs_and_operator_runbook_synchronization_contract.py -q`",
    ),
    SnippetCheck("M249-E014-DOC-EXP-07", "`check:objc3c:m249-a005-lane-a-readiness`"),
    SnippetCheck("M249-E014-DOC-EXP-08", "`python scripts/run_m249_b006_lane_b_readiness.py`"),
    SnippetCheck("M249-E014-DOC-EXP-09", "`check:objc3c:m249-c007-lane-c-readiness`"),
    SnippetCheck("M249-E014-DOC-EXP-15", "`python scripts/run_m249_d012_lane_d_readiness.py`"),
    SnippetCheck(
        "M249-E014-DOC-EXP-10",
        "`scripts/check_m249_e014_lane_e_release_gate_docs_and_runbooks_release_candidate_and_replay_dry_run_contract.py`",
    ),
    SnippetCheck(
        "M249-E014-DOC-EXP-11",
        "`tests/tooling/test_check_m249_e014_lane_e_release_gate_docs_and_runbooks_release_candidate_and_replay_dry_run_contract.py`",
    ),
    SnippetCheck("M249-E014-DOC-EXP-12", "`test:objc3c:parser-replay-proof`"),
    SnippetCheck("M249-E014-DOC-EXP-13", "`test:objc3c:parser-ast-extraction`"),
    SnippetCheck(
        "M249-E014-DOC-EXP-14",
        "`tmp/reports/m249/M249-E014/lane_e_release_gate_docs_and_runbooks_release_candidate_and_replay_dry_run_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E014-DOC-PKT-01",
        "# M249-E014 Lane-E Release Gate, Docs, and Runbooks Release-Candidate and Replay Dry-Run Packet",
    ),
    SnippetCheck("M249-E014-DOC-PKT-02", "Packet: `M249-E014`"),
    SnippetCheck("M249-E014-DOC-PKT-03", "Issue: `#6961`"),
    SnippetCheck(
        "M249-E014-DOC-PKT-04",
        "Dependencies: `M249-E013`, `M249-A005`, `M249-B006`, `M249-C007`, `M249-D012`",
    ),
    SnippetCheck(
        "M249-E014-DOC-PKT-05",
        "scripts/check_m249_e014_lane_e_release_gate_docs_and_runbooks_release_candidate_and_replay_dry_run_contract.py",
    ),
    SnippetCheck(
        "M249-E014-DOC-PKT-06",
        "tests/tooling/test_check_m249_e014_lane_e_release_gate_docs_and_runbooks_release_candidate_and_replay_dry_run_contract.py",
    ),
    SnippetCheck("M249-E014-DOC-PKT-07", "scripts/run_m249_e014_lane_e_readiness.py"),
    SnippetCheck(
        "M249-E014-DOC-PKT-08",
        "python scripts/check_m249_e013_lane_e_release_gate_docs_and_runbooks_docs_and_operator_runbook_synchronization_contract.py",
    ),
    SnippetCheck(
        "M249-E014-DOC-PKT-09",
        "python -m pytest tests/tooling/test_check_m249_e013_lane_e_release_gate_docs_and_runbooks_docs_and_operator_runbook_synchronization_contract.py -q",
    ),
    SnippetCheck("M249-E014-DOC-PKT-10", "check:objc3c:m249-a005-lane-a-readiness"),
    SnippetCheck("M249-E014-DOC-PKT-11", "scripts/run_m249_b006_lane_b_readiness.py"),
    SnippetCheck("M249-E014-DOC-PKT-12", "check:objc3c:m249-c007-lane-c-readiness"),
    SnippetCheck("M249-E014-DOC-PKT-15", "scripts/run_m249_d012_lane_d_readiness.py"),
    SnippetCheck(
        "M249-E014-DOC-PKT-13",
        "milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M249-E014-DOC-PKT-14",
        "tmp/reports/m249/M249-E014/lane_e_release_gate_docs_and_runbooks_release_candidate_and_replay_dry_run_summary.json",
    ),
)

RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E014-RUN-01",
        '"""Run M249-E014 lane-E readiness checks without deep npm nesting."""',
    ),
    SnippetCheck(
        "M249-E014-RUN-02",
        "scripts/check_m249_e013_lane_e_release_gate_docs_and_runbooks_docs_and_operator_runbook_synchronization_contract.py",
    ),
    SnippetCheck(
        "M249-E014-RUN-03",
        "tests/tooling/test_check_m249_e013_lane_e_release_gate_docs_and_runbooks_docs_and_operator_runbook_synchronization_contract.py",
    ),
    SnippetCheck("M249-E014-RUN-04", "scripts/run_m249_b006_lane_b_readiness.py"),
    SnippetCheck("M249-E014-RUN-05", "check:objc3c:m249-c007-lane-c-readiness"),
    SnippetCheck("M249-E014-RUN-06", "scripts/run_m249_d012_lane_d_readiness.py"),
    SnippetCheck("M249-E014-RUN-07", "check:objc3c:m249-a005-lane-a-readiness"),
    SnippetCheck(
        "M249-E014-RUN-08",
        "scripts/check_m249_e014_lane_e_release_gate_docs_and_runbooks_release_candidate_and_replay_dry_run_contract.py",
    ),
    SnippetCheck(
        "M249-E014-RUN-09",
        "tests/tooling/test_check_m249_e014_lane_e_release_gate_docs_and_runbooks_release_candidate_and_replay_dry_run_contract.py",
    ),
    SnippetCheck("M249-E014-RUN-10", "[ok] M249-E014 lane-E readiness chain completed"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E014-ARCH-01",
        "M249 lane-E E003 release gate/docs/runbooks core feature implementation",
    ),
    SnippetCheck(
        "M249-E014-ARCH-02",
        "`M249-E002`, `M249-A003`, `M249-B003`,",
    ),
    SnippetCheck(
        "M249-E014-ARCH-03",
        "`M249-C003`, and `M249-D003`",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E014-SPC-01",
        "release gate/docs/runbooks core feature implementation wiring shall preserve",
    ),
    SnippetCheck(
        "M249-E014-SPC-02",
        "`M249-E002`, `M249-A003`, `M249-B003`,",
    ),
    SnippetCheck(
        "M249-E014-SPC-03",
        "`M249-C003`, and `M249-D003`",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E014-META-01",
        "deterministic lane-E release gate/docs/runbooks core feature implementation dependency anchors for",
    ),
    SnippetCheck(
        "M249-E014-META-02",
        "`M249-E002`, `M249-A003`, `M249-B003`, `M249-C003`, and `M249-D003`",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M249-E014-PKG-01",
        '"check:objc3c:m249-e014-lane-e-release-gate-docs-runbooks-release-candidate-replay-dry-run-contract": "python scripts/check_m249_e014_lane_e_release_gate_docs_and_runbooks_release_candidate_and_replay_dry_run_contract.py"',
    ),
    SnippetCheck(
        "M249-E014-PKG-02",
        '"test:tooling:m249-e014-lane-e-release-gate-docs-runbooks-release-candidate-replay-dry-run-contract": "python -m pytest tests/tooling/test_check_m249_e014_lane_e_release_gate_docs_and_runbooks_release_candidate_and_replay_dry_run_contract.py -q"',
    ),
    SnippetCheck(
        "M249-E014-PKG-03",
        '"check:objc3c:m249-e014-lane-e-readiness": "python scripts/run_m249_e014_lane_e_readiness.py"',
    ),
    SnippetCheck("M249-E014-PKG-04", '"check:objc3c:m249-e013-lane-e-readiness": '),
    SnippetCheck("M249-E014-PKG-05", '"check:objc3c:m249-a005-lane-a-readiness": '),
    SnippetCheck("M249-E014-PKG-06", '"check:objc3c:m249-b006-lane-b-readiness": '),
    SnippetCheck("M249-E014-PKG-07", '"check:objc3c:m249-c007-lane-c-readiness": '),
    SnippetCheck("M249-E014-PKG-08", '"check:objc3c:m249-d012-lane-d-readiness": '),
    SnippetCheck("M249-E014-PKG-09", '"test:objc3c:parser-replay-proof": '),
    SnippetCheck("M249-E014-PKG-10", '"test:objc3c:parser-ast-extraction": '),
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
    parser.add_argument("--runner-script", type=Path, default=DEFAULT_RUNNER_SCRIPT)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--emit-json", action="store_true", help="Emit canonical summary JSON to stdout.")
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
                    detail=f"{asset.lane_task} prerequisite missing: {asset.relative_path.as_posix()}",
                )
            )
            continue
        if not absolute.is_file():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"{asset.lane_task} prerequisite path is not a file: {asset.relative_path.as_posix()}",
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
    if not path.exists() or not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required text artifact is missing: {display_path(path)}",
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
                detail=f"unable to read required text artifact: {exc}",
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

    dep_checks, dep_failures = check_prerequisite_assets()
    checks_total += dep_checks
    failures.extend(dep_failures)

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M249-E014-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M249-E014-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.runner_script, "M249-E014-RUN-EXISTS", RUNNER_SNIPPETS),
        (args.architecture_doc, "M249-E014-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M249-E014-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M249-E014-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M249-E014-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_text_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
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
