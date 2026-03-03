#!/usr/bin/env python3
"""Fail-closed checker for M228-E006 replay/performance closeout edge-case robustness."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-e006-replay-proof-performance-closeout-gate-edge-case-expansion-and-robustness-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m228_lane_e_replay_proof_and_performance_closeout_gate_edge_case_expansion_and_robustness_e006_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_e006_replay_proof_and_performance_closeout_gate_edge_case_expansion_and_robustness_packet.md"
)
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m228/M228-E006/"
    "replay_proof_and_performance_closeout_gate_edge_case_expansion_and_robustness_contract_summary.json"
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
        "M228-E006-E005-01",
        "M228-E005",
        Path(
            "docs/contracts/m228_lane_e_replay_proof_and_performance_closeout_gate_edge_case_and_compatibility_completion_e005_expectations.md"
        ),
    ),
    AssetCheck(
        "M228-E006-E005-02",
        "M228-E005",
        Path(
            "scripts/check_m228_e005_replay_proof_and_performance_closeout_gate_edge_case_and_compatibility_completion_contract.py"
        ),
    ),
    AssetCheck(
        "M228-E006-A006-01",
        "M228-A006",
        Path("docs/contracts/m228_lowering_pipeline_decomposition_pass_graph_edge_case_expansion_and_robustness_a006_expectations.md"),
    ),
    AssetCheck(
        "M228-E006-B006-01",
        "M228-B006",
        Path("docs/contracts/m228_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_b006_expectations.md"),
    ),
    AssetCheck(
        "M228-E006-C006-01",
        "M228-C006",
        Path("docs/contracts/m228_ir_emission_completeness_edge_case_expansion_and_robustness_c006_expectations.md"),
    ),
    AssetCheck(
        "M228-E006-D006-01",
        "M228-D006",
        Path("docs/contracts/m228_object_emission_link_path_reliability_edge_case_expansion_and_robustness_d006_expectations.md"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M228-E006-DOC-01",
        "Contract ID: `objc3c-lane-e-replay-proof-performance-closeout-gate-edge-case-expansion-and-robustness-contract/m228-e006-v1`",
    ),
    SnippetCheck("M228-E006-DOC-02", "`M228-E005`"),
    SnippetCheck("M228-E006-DOC-03", "`M228-A006`"),
    SnippetCheck("M228-E006-DOC-04", "`M228-B006`"),
    SnippetCheck("M228-E006-DOC-05", "`M228-C006`"),
    SnippetCheck("M228-E006-DOC-06", "`M228-D006`"),
    SnippetCheck("M228-E006-DOC-07", "`check:objc3c:m228-e006-lane-e-readiness`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M228-E006-PKT-01", "Packet: `M228-E006`"),
    SnippetCheck(
        "M228-E006-PKT-02",
        "Dependencies: `M228-E005`, `M228-A006`, `M228-B006`, `M228-C006`, `M228-D006`",
    ),
    SnippetCheck(
        "M228-E006-PKT-03",
        "`scripts/check_m228_e006_replay_proof_and_performance_closeout_gate_edge_case_expansion_and_robustness_contract.py`",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M228-E006-CFG-01",
        '"check:objc3c:m228-e006-replay-proof-performance-closeout-gate-edge-case-expansion-and-robustness-contract"',
    ),
    SnippetCheck(
        "M228-E006-CFG-02",
        '"test:tooling:m228-e006-replay-proof-performance-closeout-gate-edge-case-expansion-and-robustness-contract"',
    ),
    SnippetCheck(
        "M228-E006-CFG-03",
        '"check:objc3c:m228-e006-lane-e-readiness"',
    ),
    SnippetCheck("M228-E006-CFG-04", "npm run check:objc3c:m228-e005-lane-e-readiness"),
    SnippetCheck("M228-E006-CFG-05", "npm run check:objc3c:m228-a006-lane-a-readiness"),
    SnippetCheck("M228-E006-CFG-06", "npm run check:objc3c:m228-b006-lane-b-readiness"),
    SnippetCheck("M228-E006-CFG-07", "npm run check:objc3c:m228-c006-lane-c-readiness"),
    SnippetCheck("M228-E006-CFG-08", "npm run check:objc3c:m228-d006-lane-d-readiness"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M228-E006-ARC-01", "M228 lane-E E006 edge-case expansion and robustness"),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M228-E006-SPEC-01", "lane-E edge-case expansion and robustness closeout"),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M228-E006-META-01", "lane-E E006"),
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


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


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def check_snippets(
    artifact: str,
    text: str,
    checks: Sequence[SnippetCheck],
    findings: list[Finding],
) -> int:
    passed = 0
    for check in checks:
        if check.snippet in text:
            passed += 1
        else:
            findings.append(
                Finding(artifact, check.check_id, f"missing snippet: {check.snippet}")
            )
    return passed


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    findings: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for asset in PREREQUISITE_ASSETS:
        checks_total += 1
        path = ROOT / asset.relative_path
        if path.exists() and path.is_file():
            checks_passed += 1
        else:
            findings.append(
                Finding(
                    "prerequisite_asset",
                    asset.check_id,
                    f"{asset.lane_task} asset missing: {asset.relative_path.as_posix()}",
                )
            )

    expectations_text = load_text(args.expectations_doc, artifact="expectations_doc")
    checks_total += len(EXPECTATIONS_SNIPPETS)
    checks_passed += check_snippets(
        "expectations_doc",
        expectations_text,
        EXPECTATIONS_SNIPPETS,
        findings,
    )

    packet_text = load_text(args.packet_doc, artifact="packet_doc")
    checks_total += len(PACKET_SNIPPETS)
    checks_passed += check_snippets("packet_doc", packet_text, PACKET_SNIPPETS, findings)

    package_text = load_text(args.package_json, artifact="package_json")
    checks_total += len(PACKAGE_SNIPPETS)
    checks_passed += check_snippets("package_json", package_text, PACKAGE_SNIPPETS, findings)

    architecture_text = load_text(args.architecture_doc, artifact="architecture_doc")
    checks_total += len(ARCHITECTURE_SNIPPETS)
    checks_passed += check_snippets(
        "architecture_doc", architecture_text, ARCHITECTURE_SNIPPETS, findings
    )

    lowering_text = load_text(args.lowering_spec, artifact="lowering_spec")
    checks_total += len(LOWERING_SPEC_SNIPPETS)
    checks_passed += check_snippets("lowering_spec", lowering_text, LOWERING_SPEC_SNIPPETS, findings)

    metadata_text = load_text(args.metadata_spec, artifact="metadata_spec")
    checks_total += len(METADATA_SPEC_SNIPPETS)
    checks_passed += check_snippets("metadata_spec", metadata_text, METADATA_SPEC_SNIPPETS, findings)

    ok = not findings
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail}
            for f in findings
        ],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if ok:
        return 0
    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
