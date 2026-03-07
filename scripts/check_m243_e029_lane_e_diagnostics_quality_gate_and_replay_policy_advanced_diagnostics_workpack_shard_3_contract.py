#!/usr/bin/env python3
"""Fail-closed checker for M243-E029 lane-E diagnostics gate/replay-policy cross-lane integration sync."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m243-e029-lane-e-diagnostics-quality-gate-replay-policy-advanced-diagnostics-workpack-shard-3-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m243_lane_e_diagnostics_quality_gate_and_replay_policy_advanced_diagnostics_workpack_shard_3_e029_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_e029_lane_e_diagnostics_quality_gate_and_replay_policy_advanced_diagnostics_workpack_shard_3_packet.md"
)
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m243/M243-E029/"
    "lane_e_diagnostics_quality_gate_and_replay_policy_cross_lane_integration_sync_contract_summary.json"
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


PREREQUISITE_ASSETS: tuple[AssetCheck, ...] = (
    AssetCheck(
        "M243-E029-E011-01",
        "M243-E028",
        Path(
            "docs/contracts/m243_lane_e_diagnostics_quality_gate_and_replay_policy_performance_quality_guardrails_e011_expectations.md"
        ),
    ),
    AssetCheck(
        "M243-E029-E011-02",
        "M243-E028",
        Path(
            "scripts/check_m243_e028_lane_e_diagnostics_quality_gate_and_replay_policy_advanced_edge_compatibility_workpack_shard_3_contract.py"
        ),
    ),
    AssetCheck(
        "M243-E029-E011-03",
        "M243-E028",
        Path(
            "tests/tooling/test_check_m243_e028_lane_e_diagnostics_quality_gate_and_replay_policy_advanced_edge_compatibility_workpack_shard_3_contract.py"
        ),
    ),
    AssetCheck(
        "M243-E029-E011-04",
        "M243-E028",
        Path(
            "spec/planning/compiler/m243/m243_e028_lane_e_diagnostics_quality_gate_and_replay_policy_advanced_edge_compatibility_workpack_shard_3_packet.md"
        ),
    ),
    AssetCheck(
        "M243-E029-A012-01",
        "M243-A012",
        Path("docs/contracts/m243_diagnostic_grammar_hooks_and_source_precision_integration_closeout_and_gate_signoff_a012_expectations.md"),
    ),
    AssetCheck(
        "M243-E029-A012-02",
        "M243-A012",
        Path("scripts/check_m243_a012_diagnostic_grammar_hooks_and_source_precision_integration_closeout_and_gate_signoff_contract.py"),
    ),
    AssetCheck(
        "M243-E029-A012-03",
        "M243-A012",
        Path("tests/tooling/test_check_m243_a012_diagnostic_grammar_hooks_and_source_precision_integration_closeout_and_gate_signoff_contract.py"),
    ),
    AssetCheck(
        "M243-E029-A012-04",
        "M243-A012",
        Path("spec/planning/compiler/m243/m243_a012_diagnostic_grammar_hooks_and_source_precision_integration_closeout_and_gate_signoff_packet.md"),
    ),
    AssetCheck(
        "M243-E029-B012-01",
        "M243-B012",
        Path("docs/contracts/m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_cross_lane_integration_sync_b012_expectations.md"),
    ),
    AssetCheck(
        "M243-E029-B012-02",
        "M243-B012",
        Path("scripts/check_m243_b012_semantic_diagnostic_taxonomy_and_fix_it_synthesis_cross_lane_integration_sync_contract.py"),
    ),
    AssetCheck(
        "M243-E029-B012-03",
        "M243-B012",
        Path("tests/tooling/test_check_m243_b012_semantic_diagnostic_taxonomy_and_fix_it_synthesis_cross_lane_integration_sync_contract.py"),
    ),
    AssetCheck(
        "M243-E029-B012-04",
        "M243-B012",
        Path("spec/planning/compiler/m243/m243_b012_semantic_diagnostic_taxonomy_and_fix_it_synthesis_cross_lane_integration_sync_packet.md"),
    ),
    AssetCheck(
        "M243-E029-C011-01",
        "M243-C011",
        Path("docs/contracts/m243_lowering_runtime_diagnostics_surfacing_performance_quality_guardrails_c011_expectations.md"),
    ),
    AssetCheck(
        "M243-E029-C011-02",
        "M243-C011",
        Path("scripts/check_m243_c011_lowering_runtime_diagnostics_surfacing_performance_quality_guardrails_contract.py"),
    ),
    AssetCheck(
        "M243-E029-C011-03",
        "M243-C011",
        Path("tests/tooling/test_check_m243_c011_lowering_runtime_diagnostics_surfacing_performance_quality_guardrails_contract.py"),
    ),
    AssetCheck(
        "M243-E029-C011-04",
        "M243-C011",
        Path("spec/planning/compiler/m243/m243_c011_lowering_runtime_diagnostics_surfacing_performance_quality_guardrails_packet.md"),
    ),
    AssetCheck(
        "M243-E029-D012-01",
        "M243-D012",
        Path("docs/contracts/m243_cli_reporting_and_output_contract_integration_cross_lane_integration_sync_d012_expectations.md"),
    ),
    AssetCheck(
        "M243-E029-D012-02",
        "M243-D012",
        Path("scripts/check_m243_d012_cli_reporting_and_output_contract_integration_cross_lane_integration_sync_contract.py"),
    ),
    AssetCheck(
        "M243-E029-D012-03",
        "M243-D012",
        Path("tests/tooling/test_check_m243_d012_cli_reporting_and_output_contract_integration_cross_lane_integration_sync_contract.py"),
    ),
    AssetCheck(
        "M243-E029-D012-04",
        "M243-D012",
        Path("spec/planning/compiler/m243/m243_d012_cli_reporting_and_output_contract_integration_cross_lane_integration_sync_packet.md"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M243-E029-DOC-EXP-01",
        "Contract ID: `objc3c-lane-e-diagnostics-quality-gate-replay-policy-advanced-diagnostics-workpack-shard-3/m243-e029-v1`",
    ),
    SnippetCheck(
        "M243-E029-DOC-EXP-02",
        "Dependencies: `M243-E028`, `M243-A012`, `M243-B012`, `M243-C011`, `M243-D012`",
    ),
    SnippetCheck("M243-E029-DOC-EXP-03", "Issue `#6515` defines the canonical lane-E dependency chain for E012."),
    SnippetCheck("M243-E029-DOC-EXP-04", "E011 contract anchors remain mandatory prerequisites:"),
    SnippetCheck("M243-E029-DOC-EXP-05", "`check:objc3c:m243-e012-lane-e-readiness`"),
    SnippetCheck("M243-E029-DOC-EXP-06", "`check:objc3c:m243-e011-lane-e-readiness`"),
    SnippetCheck("M243-E029-DOC-EXP-07", "`check:objc3c:m243-a012-lane-a-readiness`"),
    SnippetCheck("M243-E029-DOC-EXP-08", "`check:objc3c:m243-b012-lane-b-readiness`"),
    SnippetCheck("M243-E029-DOC-EXP-09", "`check:objc3c:m243-c011-lane-c-readiness`"),
    SnippetCheck("M243-E029-DOC-EXP-10", "`check:objc3c:m243-d012-lane-d-readiness`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M243-E029-DOC-PKT-01", "Packet: `M243-E029`"),
    SnippetCheck("M243-E029-DOC-PKT-02", "Issue: `#6515`"),
    SnippetCheck(
        "M243-E029-DOC-PKT-03",
        "Dependencies: `M243-E028`, `M243-A012`, `M243-B012`, `M243-C011`, `M243-D012`",
    ),
    SnippetCheck(
        "M243-E029-DOC-PKT-04",
        "`scripts/check_m243_e029_lane_e_diagnostics_quality_gate_and_replay_policy_advanced_diagnostics_workpack_shard_3_contract.py`",
    ),
    SnippetCheck(
        "M243-E029-DOC-PKT-05",
        "`tests/tooling/test_check_m243_e029_lane_e_diagnostics_quality_gate_and_replay_policy_advanced_diagnostics_workpack_shard_3_contract.py`",
    ),
    SnippetCheck(
        "M243-E029-DOC-PKT-06",
        "`tmp/reports/m243/M243-E029/lane_e_diagnostics_quality_gate_and_replay_policy_cross_lane_integration_sync_contract_summary.json`",
    ),
    SnippetCheck(
        "M243-E029-DOC-PKT-07",
        "`docs/contracts/m243_lane_e_diagnostics_quality_gate_and_replay_policy_performance_quality_guardrails_e011_expectations.md`",
    ),
)

PACKAGE_SCRIPT_KEY_CHECKS: tuple[PackageScriptKeyCheck, ...] = (
    PackageScriptKeyCheck(
        "M243-E029-CFG-01",
        "check:objc3c:m243-e029-lane-e-diagnostics-quality-gate-replay-policy-advanced-diagnostics-workpack-shard-3-contract",
    ),
    PackageScriptKeyCheck(
        "M243-E029-CFG-02",
        "test:tooling:m243-e029-lane-e-diagnostics-quality-gate-replay-policy-advanced-diagnostics-workpack-shard-3-contract",
    ),
    PackageScriptKeyCheck("M243-E029-CFG-03", "check:objc3c:m243-e012-lane-e-readiness"),
)

PACKAGE_SCRIPT_CHECKS: tuple[PackageScriptCheck, ...] = (
    PackageScriptCheck(
        "M243-E029-CFG-04",
        "check:objc3c:m243-e029-lane-e-diagnostics-quality-gate-replay-policy-advanced-diagnostics-workpack-shard-3-contract",
        "python scripts/check_m243_e029_lane_e_diagnostics_quality_gate_and_replay_policy_advanced_diagnostics_workpack_shard_3_contract.py",
    ),
    PackageScriptCheck(
        "M243-E029-CFG-05",
        "test:tooling:m243-e029-lane-e-diagnostics-quality-gate-replay-policy-advanced-diagnostics-workpack-shard-3-contract",
        "python -m pytest tests/tooling/test_check_m243_e029_lane_e_diagnostics_quality_gate_and_replay_policy_advanced_diagnostics_workpack_shard_3_contract.py -q",
    ),
    PackageScriptCheck(
        "M243-E029-CFG-06",
        "check:objc3c:m243-e012-lane-e-readiness",
        "npm run check:objc3c:m243-e011-lane-e-readiness && npm run check:objc3c:m243-a012-lane-a-readiness && npm run check:objc3c:m243-b012-lane-b-readiness && npm run check:objc3c:m243-c011-lane-c-readiness && npm run check:objc3c:m243-d012-lane-d-readiness && npm run check:objc3c:m243-e029-lane-e-diagnostics-quality-gate-replay-policy-advanced-diagnostics-workpack-shard-3-contract && npm run test:tooling:m243-e029-lane-e-diagnostics-quality-gate-replay-policy-advanced-diagnostics-workpack-shard-3-contract",
    ),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M243-E029-ARCH-01",
        "M243 lane-E E012 diagnostics quality gate/replay policy cross-lane integration sync anchors dependency references",
    ),
    SnippetCheck(
        "M243-E029-ARCH-02",
        "(`M243-E028`, `M243-A012`, `M243-B012`, `M243-C011`, and `M243-D012`)",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M243-E029-SPC-01",
        "diagnostics quality gate and replay policy advanced diagnostics workpack (shard 3) wiring shall preserve explicit",
    ),
    SnippetCheck(
        "M243-E029-SPC-02",
        "lane-E dependency anchors (`M243-E028`, `M243-A012`, `M243-B012`, `M243-C011`, and",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M243-E029-META-01",
        "deterministic lane-E diagnostics quality gate and replay policy advanced diagnostics workpack (shard 3) dependency anchors for",
    ),
    SnippetCheck(
        "M243-E029-META-02",
        "`M243-E028`, `M243-A012`, `M243-B012`, `M243-C011`, and `M243-D012`",
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
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit canonical summary JSON to stdout.",
    )
    return parser.parse_args(argv)


def check_prerequisite_assets() -> tuple[int, list[Finding]]:
    checks_total = 0
    findings: list[Finding] = []
    for asset in PREREQUISITE_ASSETS:
        checks_total += 1
        absolute = ROOT / asset.relative_path
        if not absolute.exists() or not absolute.is_file():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"{asset.lane_task} prerequisite missing: {asset.relative_path.as_posix()}",
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
                    detail=f"expected snippet missing: {snippet_check.snippet}",
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
                check_id="M243-E029-CFG-00",
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
                check_id="M243-E029-CFG-00",
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
                check_id="M243-E029-CFG-00",
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
        exists_check_id="M243-E029-DOC-EXP-00",
        snippets=EXPECTATIONS_SNIPPETS,
    )
    checks_total += expectations_checks
    findings.extend(expectations_findings)

    packet_checks, packet_findings = check_doc_contract(
        artifact_name="packet_doc",
        path=args.packet_doc,
        exists_check_id="M243-E029-DOC-PKT-00",
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
        exists_check_id="M243-E029-ARCH-00",
        snippets=ARCHITECTURE_SNIPPETS,
    )
    checks_total += architecture_checks
    findings.extend(architecture_findings)

    lowering_checks, lowering_findings = check_doc_contract(
        artifact_name="lowering_spec",
        path=args.lowering_spec,
        exists_check_id="M243-E029-SPC-00",
        snippets=LOWERING_SPEC_SNIPPETS,
    )
    checks_total += lowering_checks
    findings.extend(lowering_findings)

    metadata_checks, metadata_findings = check_doc_contract(
        artifact_name="metadata_spec",
        path=args.metadata_spec,
        exists_check_id="M243-E029-META-00",
        snippets=METADATA_SPEC_SNIPPETS,
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

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary), encoding="utf-8")

    if args.emit_json:
        print(canonical_json(summary), end="")

    if not findings:
        if not args.emit_json:
            print(f"[ok] {MODE}: {summary['checks_passed']}/{summary['checks_total']} checks passed")
        return 0

    if not args.emit_json:
        print(f"{MODE}: contract drift detected ({len(findings)} failed check(s)).", file=sys.stderr)
        for finding in findings:
            print(f"- [{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))


































