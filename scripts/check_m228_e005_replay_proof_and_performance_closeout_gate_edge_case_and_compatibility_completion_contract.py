#!/usr/bin/env python3
"""Fail-closed checker for M228-E005 replay/performance closeout edge-case compatibility completion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-e005-replay-proof-performance-closeout-gate-edge-case-and-compatibility-completion-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m228_lane_e_replay_proof_and_performance_closeout_gate_edge_case_and_compatibility_completion_e005_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_e005_replay_proof_and_performance_closeout_gate_edge_case_and_compatibility_completion_packet.md"
)
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m228/M228-E005/"
    "replay_proof_and_performance_closeout_gate_edge_case_and_compatibility_completion_contract_summary.json"
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
class PackageScriptCheck:
    check_id: str
    script_key: str
    expected_value: str


@dataclass(frozen=True)
class PackageScriptKeyCheck:
    check_id: str
    script_key: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


PREREQUISITE_ASSETS: tuple[AssetCheck, ...] = (
    AssetCheck(
        "M228-E005-E004-01",
        "M228-E004",
        Path("docs/contracts/m228_lane_e_replay_proof_and_performance_closeout_gate_core_feature_expansion_e004_expectations.md"),
    ),
    AssetCheck(
        "M228-E005-E004-02",
        "M228-E004",
        Path("scripts/check_m228_e004_replay_proof_and_performance_closeout_gate_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M228-E005-E004-03",
        "M228-E004",
        Path("tests/tooling/test_check_m228_e004_replay_proof_and_performance_closeout_gate_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M228-E005-E004-04",
        "M228-E004",
        Path("spec/planning/compiler/m228/m228_e004_replay_proof_and_performance_closeout_gate_core_feature_expansion_packet.md"),
    ),
    AssetCheck(
        "M228-E005-A004-01",
        "M228-A004",
        Path("docs/contracts/m228_lowering_pipeline_decomposition_pass_graph_core_feature_expansion_a004_expectations.md"),
    ),
    AssetCheck(
        "M228-E005-A004-02",
        "M228-A004",
        Path("scripts/check_m228_a004_lowering_pipeline_decomposition_pass_graph_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M228-E005-A004-03",
        "M228-A004",
        Path("tests/tooling/test_check_m228_a004_lowering_pipeline_decomposition_pass_graph_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M228-E005-B006-01",
        "M228-B006",
        Path("docs/contracts/m228_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_b006_expectations.md"),
    ),
    AssetCheck(
        "M228-E005-B006-02",
        "M228-B006",
        Path("scripts/check_m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract.py"),
    ),
    AssetCheck(
        "M228-E005-B006-03",
        "M228-B006",
        Path("tests/tooling/test_check_m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract.py"),
    ),
    AssetCheck(
        "M228-E005-B006-04",
        "M228-B006",
        Path("spec/planning/compiler/m228/m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_packet.md"),
    ),
    AssetCheck(
        "M228-E005-C004-01",
        "M228-C004",
        Path("docs/contracts/m228_ir_emission_completeness_core_feature_expansion_c004_expectations.md"),
    ),
    AssetCheck(
        "M228-E005-C004-02",
        "M228-C004",
        Path("scripts/check_m228_c004_ir_emission_completeness_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M228-E005-C004-03",
        "M228-C004",
        Path("tests/tooling/test_check_m228_c004_ir_emission_completeness_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M228-E005-C004-04",
        "M228-C004",
        Path("spec/planning/compiler/m228/m228_c004_ir_emission_completeness_core_feature_expansion_packet.md"),
    ),
    AssetCheck(
        "M228-E005-D005-01",
        "M228-D005",
        Path("docs/contracts/m228_object_emission_link_path_reliability_edge_case_and_compatibility_completion_d005_expectations.md"),
    ),
    AssetCheck(
        "M228-E005-D005-02",
        "M228-D005",
        Path("scripts/check_m228_d005_object_emission_link_path_reliability_edge_case_and_compatibility_completion_contract.py"),
    ),
    AssetCheck(
        "M228-E005-D005-03",
        "M228-D005",
        Path("tests/tooling/test_check_m228_d005_object_emission_link_path_reliability_edge_case_and_compatibility_completion_contract.py"),
    ),
    AssetCheck(
        "M228-E005-D005-04",
        "M228-D005",
        Path("spec/planning/compiler/m228/m228_d005_object_emission_link_path_reliability_edge_case_and_compatibility_completion_packet.md"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M228-E005-DOC-EXP-02",
        "# M228 Lane E Replay-Proof and Performance Closeout Gate Edge-Case and Compatibility Completion Expectations (E005)",
    ),
    SnippetCheck(
        "M228-E005-DOC-EXP-03",
        "Contract ID: `objc3c-lane-e-replay-proof-performance-closeout-gate-edge-case-and-compatibility-completion-contract/m228-e005-v1`",
    ),
    SnippetCheck("M228-E005-DOC-EXP-04", "`M228-E004`"),
    SnippetCheck("M228-E005-DOC-EXP-05", "`M228-A004`"),
    SnippetCheck("M228-E005-DOC-EXP-06", "`M228-B006`"),
    SnippetCheck("M228-E005-DOC-EXP-07", "`M228-C004`"),
    SnippetCheck("M228-E005-DOC-EXP-08", "`M228-D005`"),
    SnippetCheck("M228-E005-DOC-EXP-09", "Issue `#5285` dependency list includes `M228-B006` and `M228-C010`."),
    SnippetCheck("M228-E005-DOC-EXP-10", "`M228-B006` and `M228-C010` are preserved as pending-lane tokens"),
    SnippetCheck("M228-E005-DOC-EXP-11", "`check:objc3c:m228-e005-replay-proof-performance-closeout-gate-edge-case-and-compatibility-completion-contract`"),
    SnippetCheck("M228-E005-DOC-EXP-12", "`check:objc3c:m228-e005-lane-e-readiness`"),
    SnippetCheck("M228-E005-DOC-EXP-13", "`tmp/reports/m228/M228-E005/replay_proof_and_performance_closeout_gate_edge_case_and_compatibility_completion_contract_summary.json`"),
    SnippetCheck("M228-E005-DOC-EXP-14", "## Architecture and Spec Anchors"),
    SnippetCheck("M228-E005-DOC-EXP-15", "`native/objc3c/src/ARCHITECTURE.md` includes a lane-E E005 anchor line"),
    SnippetCheck("M228-E005-DOC-EXP-16", "`spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-E E005 fail-closed"),
    SnippetCheck("M228-E005-DOC-EXP-17", "`spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-E E005"),
    SnippetCheck("M228-E005-DOC-EXP-18", "`test:objc3c:lowering-replay-proof`"),
    SnippetCheck("M228-E005-DOC-EXP-19", "`test:objc3c:perf-budget`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M228-E005-DOC-PKT-02",
        "# M228-E005 Replay-Proof and Performance Closeout Gate Edge-Case and Compatibility Completion Packet",
    ),
    SnippetCheck("M228-E005-DOC-PKT-03", "Packet: `M228-E005`"),
    SnippetCheck("M228-E005-DOC-PKT-04", "Freeze date: `2026-03-02`"),
    SnippetCheck(
        "M228-E005-DOC-PKT-05",
        "Dependencies: `M228-E004`, `M228-A004`, `M228-B006`, `M228-C004`, `M228-D005`",
    ),
    SnippetCheck("M228-E005-DOC-PKT-06", "Issue dependency continuity tokens:"),
    SnippetCheck("M228-E005-DOC-PKT-07", "`M228-B006`"),
    SnippetCheck("M228-E005-DOC-PKT-08", "`M228-C010`"),
    SnippetCheck(
        "M228-E005-DOC-PKT-09",
        "`scripts/check_m228_e005_replay_proof_and_performance_closeout_gate_edge_case_and_compatibility_completion_contract.py`",
    ),
    SnippetCheck(
        "M228-E005-DOC-PKT-10",
        "`tests/tooling/test_check_m228_e005_replay_proof_and_performance_closeout_gate_edge_case_and_compatibility_completion_contract.py`",
    ),
    SnippetCheck(
        "M228-E005-DOC-PKT-11",
        "`tmp/reports/m228/M228-E005/replay_proof_and_performance_closeout_gate_edge_case_and_compatibility_completion_contract_summary.json`",
    ),
    SnippetCheck("M228-E005-DOC-PKT-12", "`test:objc3c:lowering-replay-proof`"),
    SnippetCheck("M228-E005-DOC-PKT-13", "`test:objc3c:perf-budget`"),
    SnippetCheck("M228-E005-DOC-PKT-14", "`native/objc3c/src/ARCHITECTURE.md`"),
)

PACKAGE_SCRIPT_CHECKS: tuple[PackageScriptCheck, ...] = (
    PackageScriptCheck(
        "M228-E005-CFG-01",
        "check:objc3c:m228-e005-replay-proof-performance-closeout-gate-edge-case-and-compatibility-completion-contract",
        "python scripts/check_m228_e005_replay_proof_and_performance_closeout_gate_edge_case_and_compatibility_completion_contract.py",
    ),
    PackageScriptCheck(
        "M228-E005-CFG-02",
        "test:tooling:m228-e005-replay-proof-performance-closeout-gate-edge-case-and-compatibility-completion-contract",
        "python -m pytest tests/tooling/test_check_m228_e005_replay_proof_and_performance_closeout_gate_edge_case_and_compatibility_completion_contract.py -q",
    ),
    PackageScriptCheck(
        "M228-E005-CFG-04",
        "check:objc3c:m228-e005-lane-e-readiness",
        "npm run check:objc3c:m228-e004-lane-e-readiness && npm run check:objc3c:m228-a004-lane-a-readiness && npm run check:objc3c:m228-b006-lane-b-readiness && npm run check:objc3c:m228-c004-lane-c-readiness && npm run check:objc3c:m228-d005-lane-d-readiness && npm run check:objc3c:m228-e005-replay-proof-performance-closeout-gate-edge-case-and-compatibility-completion-contract && npm run test:tooling:m228-e005-replay-proof-performance-closeout-gate-edge-case-and-compatibility-completion-contract",
    ),
)

PACKAGE_SCRIPT_KEY_CHECKS: tuple[PackageScriptKeyCheck, ...] = (
    PackageScriptKeyCheck(
        "M228-E005-CFG-03",
        "check:objc3c:m228-e005-lane-e-readiness",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M228-E005-ARCH-01",
        "M228 lane-E E005 replay-proof/performance closeout edge-case and compatibility completion anchors",
    ),
)

LOWERING_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M228-E005-SPC-01",
        "replay-proof/performance edge-case compatibility completion closeout wiring",
    ),
)

METADATA_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M228-E005-META-01",
        "deterministic lane-E edge-case compatibility completion dependency anchors",
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
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
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


def check_doc_contract(
    *,
    artifact_name: str,
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
                detail=f"required document path is not a file: {display_path(path)}",
            )
        )
        return checks_total, findings

    try:
        text = path.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail="required document is not valid UTF-8",
            )
        )
        return checks_total, findings
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
                    detail=f"expected snippet missing: {snippet_check.snippet}",
                )
            )
    return checks_total, findings


def check_package_contract(path: Path) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id="M228-E005-CFG-00",
                detail=f"required document is missing: {display_path(path)}",
            )
        )
        return checks_total, findings
    if not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id="M228-E005-CFG-00",
                detail=f"required document path is not a file: {display_path(path)}",
            )
        )
        return checks_total, findings

    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except UnicodeDecodeError:
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id="M228-E005-CFG-00",
                detail="required document is not valid UTF-8",
            )
        )
        return checks_total, findings
    except json.JSONDecodeError as exc:
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id="M228-E005-CFG-00",
                detail=f"required document is not valid JSON: {exc}",
            )
        )
        return checks_total, findings
    except OSError as exc:
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id="M228-E005-CFG-00",
                detail=f"unable to read required document: {exc}",
            )
        )
        return checks_total, findings

    scripts = payload.get("scripts")
    checks_total += 1
    if not isinstance(scripts, dict):
        findings.append(
            Finding(
                artifact="package_json",
                check_id="M228-E005-CFG-00",
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

    checks_total, findings = check_prerequisite_assets()

    expectations_checks, expectations_findings = check_doc_contract(
        artifact_name="expectations_doc",
        path=args.expectations_doc,
        exists_check_id="M228-E005-DOC-EXP-01",
        snippets=EXPECTATIONS_SNIPPETS,
    )
    checks_total += expectations_checks
    findings.extend(expectations_findings)

    packet_checks, packet_findings = check_doc_contract(
        artifact_name="packet_doc",
        path=args.packet_doc,
        exists_check_id="M228-E005-DOC-PKT-01",
        snippets=PACKET_SNIPPETS,
    )
    checks_total += packet_checks
    findings.extend(packet_findings)

    package_checks, package_findings = check_package_contract(args.package_json)
    checks_total += package_checks
    findings.extend(package_findings)

    architecture_checks, architecture_findings = check_doc_contract(
        artifact_name="architecture_doc",
        path=args.architecture_doc,
        exists_check_id="M228-E005-ARCH-00",
        snippets=ARCHITECTURE_SNIPPETS,
    )
    checks_total += architecture_checks
    findings.extend(architecture_findings)

    lowering_checks, lowering_findings = check_doc_contract(
        artifact_name="lowering_spec",
        path=args.lowering_spec,
        exists_check_id="M228-E005-SPC-00",
        snippets=LOWERING_SNIPPETS,
    )
    checks_total += lowering_checks
    findings.extend(lowering_findings)

    metadata_checks, metadata_findings = check_doc_contract(
        artifact_name="metadata_spec",
        path=args.metadata_spec,
        exists_check_id="M228-E005-META-00",
        snippets=METADATA_SNIPPETS,
    )
    checks_total += metadata_checks
    findings.extend(metadata_findings)

    findings = sorted(findings, key=lambda finding: (finding.check_id, finding.artifact, finding.detail))
    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(findings),
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in findings
        ],
    }

    try:
        args.summary_out.parent.mkdir(parents=True, exist_ok=True)
        args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    except OSError as exc:
        print(
            "m228-e005-replay-proof-performance-closeout-gate-edge-case-and-compatibility-completion-contract: "
            f"error: {exc}",
            file=sys.stderr,
        )
        return 2

    if not findings:
        print("m228-e005-replay-proof-performance-closeout-gate-edge-case-and-compatibility-completion-contract: OK")
        return 0

    print(
        "m228-e005-replay-proof-performance-closeout-gate-edge-case-and-compatibility-completion-contract: "
        f"contract drift detected ({len(findings)} failed check(s)).",
        file=sys.stderr,
    )
    for finding in findings:
        print(f"- [{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
