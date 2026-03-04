#!/usr/bin/env python3
"""Fail-closed checker for M248-E028 lane-E CI governance advanced edge compatibility workpack shard3."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m248-e028-lane-e-ci-governance-gate-closeout-policy-advanced-edge-compatibility-workpack-shard3-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m248_lane_e_ci_governance_gate_and_closeout_policy_advanced_edge_compatibility_workpack_shard3_e028_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_e028_ci_governance_gate_and_closeout_policy_advanced_edge_compatibility_workpack_shard3_packet.md"
)
DEFAULT_FINAL_READINESS_SURFACE = (
    ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_final_readiness_gate_core_feature_implementation_surface.h"
)
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m248_e028_lane_e_readiness.py"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m248/M248-E028/lane_e_ci_governance_gate_closeout_policy_advanced_edge_compatibility_workpack_shard3_summary.json"
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
        "M248-E028-DEP-01",
        "M248-E027",
        Path(
            "docs/contracts/m248_lane_e_ci_governance_gate_and_closeout_policy_advanced_core_workpack_shard3_e027_expectations.md"
        ),
    ),
    AssetCheck(
        "M248-E028-DEP-02",
        "M248-A010",
        Path("docs/contracts/m248_suite_partitioning_and_fixture_ownership_conformance_corpus_expansion_a010_expectations.md"),
    ),
    AssetCheck(
        "M248-E028-DEP-03",
        "M248-B013",
        Path(
            "docs/contracts/m248_semantic_lowering_test_architecture_docs_and_operator_runbook_synchronization_b013_expectations.md"
        ),
    ),
    AssetCheck(
        "M248-E028-DEP-04",
        "M248-C015",
        Path("docs/contracts/m248_replay_harness_and_artifact_contracts_advanced_core_workpack_shard1_c015_expectations.md"),
    ),
    AssetCheck(
        "M248-E028-DEP-05",
        "M248-D020",
        Path("docs/contracts/m248_runner_reliability_and_platform_operations_advanced_performance_workpack_shard1_d020_expectations.md"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-E028-DOC-EXP-01",
        "# M248 Lane E CI Governance Gate and Closeout Policy Advanced Edge Compatibility Workpack (Shard 3) Expectations (E028)",
    ),
    SnippetCheck(
        "M248-E028-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-ci-governance-gate-closeout-policy-advanced-edge-compatibility-workpack-shard3/m248-e028-v1`",
    ),
    SnippetCheck(
        "M248-E028-DOC-EXP-03",
        "Dependencies: `M248-E027`, `M248-A010`, `M248-B013`, `M248-C015`, `M248-D020`",
    ),
    SnippetCheck(
        "M248-E028-DOC-EXP-04",
        "Issue `#6888` defines canonical lane-E advanced edge compatibility workpack (shard 3) scope.",
    ),
    SnippetCheck("M248-E028-DOC-EXP-05", "`advanced_edge_compatibility_shard3_consistent`"),
    SnippetCheck("M248-E028-DOC-EXP-06", "`advanced_edge_compatibility_shard3_ready`"),
    SnippetCheck("M248-E028-DOC-EXP-07", "`advanced_edge_compatibility_shard3_key`"),
    SnippetCheck("M248-E028-DOC-EXP-08", "`check:objc3c:m248-e028-lane-e-readiness`"),
    SnippetCheck("M248-E028-DOC-EXP-09", "`check:objc3c:m248-e027-lane-e-readiness`"),
    SnippetCheck(
        "M248-E028-DOC-EXP-10",
        "`tmp/reports/m248/M248-E028/lane_e_ci_governance_gate_closeout_policy_advanced_edge_compatibility_workpack_shard3_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-E028-DOC-PKT-01", "Packet: `M248-E028`"),
    SnippetCheck("M248-E028-DOC-PKT-02", "Issue: `#6888`"),
    SnippetCheck(
        "M248-E028-DOC-PKT-03",
        "Dependencies: `M248-E027`, `M248-A010`, `M248-B013`, `M248-C015`, `M248-D020`",
    ),
    SnippetCheck(
        "M248-E028-DOC-PKT-04",
        "`scripts/check_m248_e028_ci_governance_gate_and_closeout_policy_advanced_edge_compatibility_workpack_shard3_contract.py`",
    ),
    SnippetCheck(
        "M248-E028-DOC-PKT-05",
        "`tests/tooling/test_check_m248_e028_ci_governance_gate_and_closeout_policy_advanced_edge_compatibility_workpack_shard3_contract.py`",
    ),
    SnippetCheck("M248-E028-DOC-PKT-06", "`scripts/run_m248_e028_lane_e_readiness.py`"),
    SnippetCheck("M248-E028-DOC-PKT-07", "`advanced_edge_compatibility_shard3_consistent`"),
    SnippetCheck("M248-E028-DOC-PKT-08", "`advanced_edge_compatibility_shard3_ready`"),
    SnippetCheck("M248-E028-DOC-PKT-09", "`advanced_edge_compatibility_shard3_key`"),
    SnippetCheck("M248-E028-DOC-PKT-10", "`check:objc3c:m248-e028-lane-e-readiness`"),
    SnippetCheck("M248-E028-DOC-PKT-11", "`check:objc3c:m248-e027-lane-e-readiness`"),
)

FINAL_READINESS_SURFACE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-E028-SUR-01", "BuildObjc3FinalReadinessGateAdvancedEdgeCompatibilityShard3Key("),
    SnippetCheck("M248-E028-SUR-02", "const bool lane_advanced_edge_compatibility_shard3_consistent ="),
    SnippetCheck("M248-E028-SUR-03", "const bool advanced_edge_compatibility_shard3_consistent ="),
    SnippetCheck("M248-E028-SUR-04", "const bool advanced_edge_compatibility_shard3_ready ="),
    SnippetCheck("M248-E028-SUR-05", "surface.advanced_edge_compatibility_shard3_consistent ="),
    SnippetCheck("M248-E028-SUR-06", "surface.advanced_edge_compatibility_shard3_ready ="),
    SnippetCheck("M248-E028-SUR-07", "surface.advanced_edge_compatibility_shard3_key ="),
    SnippetCheck("M248-E028-SUR-08", "final-readiness-gate-advanced-edge-compatibility-shard3:v1:"),
    SnippetCheck("M248-E028-SUR-09", ";advanced_edge_compatibility_shard3_consistent="),
    SnippetCheck("M248-E028-SUR-10", ";advanced_edge_compatibility_shard3_ready="),
    SnippetCheck("M248-E028-SUR-11", ";advanced_edge_compatibility_shard3_key_ready="),
    SnippetCheck("M248-E028-SUR-12", "surface.advanced_edge_compatibility_shard3_ready &&"),
    SnippetCheck("M248-E028-SUR-13", "surface.advanced_core_shard3_ready &&"),
    SnippetCheck(
        "M248-E028-SUR-14",
        "final readiness gate advanced edge compatibility workpack shard3 is inconsistent",
    ),
    SnippetCheck(
        "M248-E028-SUR-15",
        "final readiness gate advanced edge compatibility workpack shard3 consistency is not satisfied",
    ),
    SnippetCheck("M248-E028-SUR-16", "final readiness gate advanced edge compatibility workpack shard3 is not ready"),
    SnippetCheck("M248-E028-SUR-17", "final readiness gate advanced edge compatibility workpack shard3 key is not ready"),
)

FRONTEND_TYPES_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-E028-TYP-01", "bool advanced_edge_compatibility_shard3_consistent = false;"),
    SnippetCheck("M248-E028-TYP-02", "bool advanced_edge_compatibility_shard3_ready = false;"),
    SnippetCheck("M248-E028-TYP-03", "std::string advanced_edge_compatibility_shard3_key;"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-E028-ARC-01", "advanced_edge_compatibility_shard3_*"),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M248-E028-CFG-01",
        '"check:objc3c:m248-e028-ci-governance-gate-closeout-policy-advanced-edge-compatibility-workpack-shard3-contract"',
    ),
    SnippetCheck(
        "M248-E028-CFG-02",
        '"test:tooling:m248-e028-ci-governance-gate-closeout-policy-advanced-edge-compatibility-workpack-shard3-contract"',
    ),
    SnippetCheck("M248-E028-CFG-03", '"check:objc3c:m248-e028-lane-e-readiness"'),
    SnippetCheck("M248-E028-CFG-04", "python scripts/run_m248_e028_lane_e_readiness.py"),
    SnippetCheck("M248-E028-CFG-05", "check:objc3c:m248-e027-lane-e-readiness"),
)

READINESS_RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-E028-RUN-01", "scripts/run_m248_e027_lane_e_readiness.py"),
    SnippetCheck(
        "M248-E028-RUN-02",
        "scripts/check_m248_e028_ci_governance_gate_and_closeout_policy_advanced_edge_compatibility_workpack_shard3_contract.py",
    ),
    SnippetCheck(
        "M248-E028-RUN-03",
        "tests/tooling/test_check_m248_e028_ci_governance_gate_and_closeout_policy_advanced_edge_compatibility_workpack_shard3_contract.py",
    ),
    SnippetCheck("M248-E028-RUN-04", "[ok] M248-E028 lane-E readiness chain completed"),
)

FORBIDDEN_SURFACE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-E028-FORB-01", "surface.advanced_edge_compatibility_shard3_ready = true;"),
)

FORBIDDEN_READINESS_RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M248-E028-RUN-FORB-01", "npm run "),
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
    parser.add_argument("--final-readiness-surface", type=Path, default=DEFAULT_FINAL_READINESS_SURFACE)
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
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
    required_snippets: tuple[SnippetCheck, ...],
    forbidden_snippets: tuple[SnippetCheck, ...] = (),
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

    for snippet in required_snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(
                Finding(
                    artifact=display_path(path),
                    check_id=snippet.check_id,
                    detail=f"missing required snippet: {snippet.snippet}",
                )
            )
    for snippet in forbidden_snippets:
        checks_total += 1
        if snippet.snippet in text:
            findings.append(
                Finding(
                    artifact=display_path(path),
                    check_id=snippet.check_id,
                    detail=f"forbidden snippet present: {snippet.snippet}",
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

    text_contracts: tuple[tuple[Path, str, tuple[SnippetCheck, ...], tuple[SnippetCheck, ...]], ...] = (
        (args.expectations_doc, "M248-E028-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS, ()),
        (args.packet_doc, "M248-E028-DOC-PKT-EXISTS", PACKET_SNIPPETS, ()),
        (args.final_readiness_surface, "M248-E028-SUR-EXISTS", FINAL_READINESS_SURFACE_SNIPPETS, FORBIDDEN_SURFACE_SNIPPETS),
        (args.frontend_types, "M248-E028-TYP-EXISTS", FRONTEND_TYPES_SNIPPETS, ()),
        (args.architecture_doc, "M248-E028-ARC-EXISTS", ARCHITECTURE_SNIPPETS, ()),
        (args.package_json, "M248-E028-CFG-EXISTS", PACKAGE_SNIPPETS, ()),
        (args.readiness_runner, "M248-E028-RUN-EXISTS", READINESS_RUNNER_SNIPPETS, FORBIDDEN_READINESS_RUNNER_SNIPPETS),
    )

    for path, exists_check_id, required_snippets, forbidden_snippets in text_contracts:
        doc_checks, doc_failures = check_text_contract(
            path=path,
            exists_check_id=exists_check_id,
            required_snippets=required_snippets,
            forbidden_snippets=forbidden_snippets,
        )
        checks_total += doc_checks
        failures.extend(doc_failures)

    failures.sort(key=finding_sort_key)
    summary = {
        "mode": MODE,
        "ok": len(failures) == 0,
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

    summary_path = args.summary_out
    if not summary_path.is_absolute():
        summary_path = ROOT / summary_path
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary), encoding="utf-8")

    if args.emit_json:
        json.dump(summary, sys.stdout, indent=2)
        sys.stdout.write("\n")
    elif summary["ok"]:
        print(f"[ok] {MODE}: {summary['checks_passed']}/{summary['checks_total']} checks passed")

    if summary["ok"]:
        return 0
    for finding in failures:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
