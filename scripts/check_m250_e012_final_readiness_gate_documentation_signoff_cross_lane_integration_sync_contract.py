#!/usr/bin/env python3
"""Fail-closed validator for M250-E012 final readiness cross-lane integration sync."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m250-final-readiness-gate-documentation-signoff-cross-lane-integration-sync-contract-e012-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_final_readiness_gate_documentation_signoff_cross_lane_integration_sync_e012_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_e012_final_readiness_gate_documentation_signoff_cross_lane_integration_sync_packet.md",
    "core_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_final_readiness_gate_core_feature_implementation_surface.h",
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "package_json": ROOT / "package.json",
    "e011_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_final_readiness_gate_documentation_signoff_performance_quality_guardrails_e011_expectations.md",
    "a004_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_frontend_stability_long_tail_grammar_core_feature_expansion_a004_expectations.md",
    "b005_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_semantic_stability_spec_delta_closure_edge_case_compatibility_completion_b005_expectations.md",
    "c006_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_lowering_runtime_stability_edge_case_expansion_and_robustness_c006_expectations.md",
    "d010_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_toolchain_runtime_ga_operations_readiness_conformance_corpus_expansion_d010_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        (
            "M250-E012-DOC-01",
            "Contract ID: `objc3c-final-readiness-gate-documentation-signoff-cross-lane-integration-sync/m250-e012-v1`",
        ),
        ("M250-E012-DOC-02", "`M250-E011`"),
        ("M250-E012-DOC-03", "`M250-A004`"),
        ("M250-E012-DOC-04", "`M250-B005`"),
        ("M250-E012-DOC-05", "`M250-C006`"),
        ("M250-E012-DOC-06", "`M250-D010`"),
        (
            "M250-E012-DOC-07",
            "scripts/check_m250_e012_final_readiness_gate_documentation_signoff_cross_lane_integration_sync_contract.py",
        ),
        (
            "M250-E012-DOC-08",
            "tests/tooling/test_check_m250_e012_final_readiness_gate_documentation_signoff_cross_lane_integration_sync_contract.py",
        ),
        ("M250-E012-DOC-09", "npm run check:objc3c:m250-e012-lane-e-readiness"),
    ),
    "packet_doc": (
        ("M250-E012-PKT-01", "Packet: `M250-E012`"),
        (
            "M250-E012-PKT-02",
            "Dependencies: `M250-E011`, `M250-A004`, `M250-B005`, `M250-C006`, `M250-D010`",
        ),
        (
            "M250-E012-PKT-03",
            "scripts/check_m250_e012_final_readiness_gate_documentation_signoff_cross_lane_integration_sync_contract.py",
        ),
    ),
    "core_surface_header": (
        ("M250-E012-SUR-01", "BuildObjc3FinalReadinessGateCrossLaneIntegrationKey("),
        ("M250-E012-SUR-02", "const bool lane_cross_lane_integration_consistent ="),
        ("M250-E012-SUR-03", "const bool cross_lane_integration_consistent ="),
        ("M250-E012-SUR-04", "const bool cross_lane_integration_ready ="),
        ("M250-E012-SUR-05", "surface.cross_lane_integration_consistent ="),
        ("M250-E012-SUR-06", "surface.cross_lane_integration_ready ="),
        ("M250-E012-SUR-07", "surface.cross_lane_integration_key ="),
        ("M250-E012-SUR-08", "final-readiness-gate-cross-lane-integration:v1:"),
        ("M250-E012-SUR-09", ";cross_lane_integration_consistent="),
        ("M250-E012-SUR-10", ";cross_lane_integration_ready="),
        ("M250-E012-SUR-11", ";cross_lane_integration_key_ready="),
        ("M250-E012-SUR-12", "surface.cross_lane_integration_ready &&"),
        ("M250-E012-SUR-13", "final readiness gate cross-lane integration sync is inconsistent"),
        (
            "M250-E012-SUR-14",
            "final readiness gate cross-lane integration sync consistency is not satisfied",
        ),
        ("M250-E012-SUR-15", "final readiness gate cross-lane integration sync is not ready"),
        ("M250-E012-SUR-16", "final readiness gate cross-lane integration sync key is not ready"),
    ),
    "frontend_types": (
        ("M250-E012-TYP-01", "bool cross_lane_integration_consistent = false;"),
        ("M250-E012-TYP-02", "bool cross_lane_integration_ready = false;"),
        ("M250-E012-TYP-03", "std::string cross_lane_integration_key;"),
    ),
    "architecture_doc": (
        (
            "M250-E012-ARC-01",
            "M250 lane-E E012 cross-lane integration sync anchors explicit final",
        ),
        ("M250-E012-ARC-02", "cross_lane_integration_*"),
    ),
    "package_json": (
        (
            "M250-E012-CFG-01",
            '"check:objc3c:m250-e012-final-readiness-gate-documentation-signoff-cross-lane-integration-sync-contract"',
        ),
        (
            "M250-E012-CFG-02",
            '"test:tooling:m250-e012-final-readiness-gate-documentation-signoff-cross-lane-integration-sync-contract"',
        ),
        ("M250-E012-CFG-03", '"check:objc3c:m250-e012-lane-e-readiness"'),
        ("M250-E012-CFG-04", "check:objc3c:m250-e011-lane-e-readiness"),
        ("M250-E012-CFG-05", "check:objc3c:m250-a004-lane-a-readiness"),
        ("M250-E012-CFG-06", "check:objc3c:m250-b005-lane-b-readiness"),
        ("M250-E012-CFG-07", "check:objc3c:m250-c006-lane-c-readiness"),
        ("M250-E012-CFG-08", "check:objc3c:m250-d010-lane-d-readiness"),
    ),
    "e011_contract": (
        (
            "M250-E012-DEP-01",
            "Contract ID: `objc3c-final-readiness-gate-documentation-signoff-performance-quality-guardrails/m250-e011-v1`",
        ),
    ),
    "a004_contract": (
        (
            "M250-E012-DEP-02",
            "Contract ID: `objc3c-frontend-stability-long-tail-grammar-core-feature-expansion/m250-a004-v1`",
        ),
    ),
    "b005_contract": (
        (
            "M250-E012-DEP-03",
            "Contract ID: `objc3c-semantic-stability-spec-delta-closure-edge-case-compatibility-completion/m250-b005-v1`",
        ),
    ),
    "c006_contract": (
        (
            "M250-E012-DEP-04",
            "Contract ID: `objc3c-lowering-runtime-stability-edge-case-expansion-and-robustness/m250-c006-v1`",
        ),
    ),
    "d010_contract": (
        (
            "M250-E012-DEP-05",
            "Contract ID: `objc3c-toolchain-runtime-ga-operations-readiness-conformance-corpus-expansion/m250-d010-v1`",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M250-E012-FORB-01", "surface.cross_lane_integration_ready = true;"),
    ),
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path(
            "tmp/reports/m250/M250-E012/final_readiness_gate_documentation_signoff_cross_lane_integration_sync_contract_summary.json"
        ),
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    for artifact, path in ARTIFACTS.items():
        text = load_text(path, artifact=artifact)
        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                passed_checks += 1
            else:
                findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))
        for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                findings.append(Finding(artifact, check_id, f"forbidden snippet present: {snippet}"))
            else:
                passed_checks += 1

    ok = not findings
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": total_checks,
        "checks_passed": passed_checks,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in findings
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
