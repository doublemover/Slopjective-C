#!/usr/bin/env python3
"""Fail-closed validator for M250-E023 final readiness advanced diagnostics shard2."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m250-final-readiness-gate-documentation-signoff-advanced-diagnostics-workpack-shard2-contract-e023-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_final_readiness_gate_documentation_signoff_advanced_diagnostics_workpack_shard2_e023_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_e023_final_readiness_gate_documentation_signoff_advanced_diagnostics_workpack_shard2_packet.md",
    "core_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_final_readiness_gate_core_feature_implementation_surface.h",
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "package_json": ROOT / "package.json",
    "e022_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_final_readiness_gate_documentation_signoff_advanced_edge_compatibility_workpack_shard2_e022_expectations.md",
    "a009_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_frontend_stability_long_tail_grammar_conformance_matrix_a009_expectations.md",
    "b010_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_semantic_stability_spec_delta_closure_conformance_corpus_expansion_b010_expectations.md",
    "c011_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_lowering_runtime_stability_performance_quality_guardrails_c011_expectations.md",
    "d019_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_toolchain_runtime_ga_operations_readiness_advanced_integration_workpack_shard1_d019_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        (
            "M250-E023-DOC-01",
            "Contract ID: `objc3c-final-readiness-gate-documentation-signoff-advanced-diagnostics-workpack-shard2/m250-e023-v1`",
        ),
        ("M250-E023-DOC-02", "`M250-E022`"),
        ("M250-E023-DOC-03", "`M250-A009`"),
        ("M250-E023-DOC-04", "`M250-B010`"),
        ("M250-E023-DOC-05", "`M250-C011`"),
        ("M250-E023-DOC-06", "`M250-D019`"),
        (
            "M250-E023-DOC-07",
            "scripts/check_m250_e023_final_readiness_gate_documentation_signoff_advanced_diagnostics_workpack_shard2_contract.py",
        ),
        (
            "M250-E023-DOC-08",
            "tests/tooling/test_check_m250_e023_final_readiness_gate_documentation_signoff_advanced_diagnostics_workpack_shard2_contract.py",
        ),
        ("M250-E023-DOC-09", "npm run check:objc3c:m250-e023-lane-e-readiness"),
    ),
    "packet_doc": (
        ("M250-E023-PKT-01", "Packet: `M250-E023`"),
        (
            "M250-E023-PKT-02",
            "Dependencies: `M250-E022`, `M250-A009`, `M250-B010`, `M250-C011`, `M250-D019`",
        ),
        (
            "M250-E023-PKT-03",
            "scripts/check_m250_e023_final_readiness_gate_documentation_signoff_advanced_diagnostics_workpack_shard2_contract.py",
        ),
    ),
    "core_surface_header": (
        (
            "M250-E023-SUR-01",
            "BuildObjc3FinalReadinessGateAdvancedDiagnosticsShard2Key(",
        ),
        ("M250-E023-SUR-02", "const bool lane_advanced_diagnostics_shard2_consistent ="),
        ("M250-E023-SUR-03", "const bool advanced_diagnostics_shard2_consistent ="),
        ("M250-E023-SUR-04", "const bool advanced_diagnostics_shard2_ready ="),
        ("M250-E023-SUR-05", "surface.advanced_diagnostics_shard2_consistent ="),
        ("M250-E023-SUR-06", "surface.advanced_diagnostics_shard2_ready ="),
        ("M250-E023-SUR-07", "surface.advanced_diagnostics_shard2_key ="),
        ("M250-E023-SUR-08", "final-readiness-gate-advanced-diagnostics-shard2:v1:"),
        ("M250-E023-SUR-09", ";advanced_diagnostics_shard2_consistent="),
        ("M250-E023-SUR-10", ";advanced_diagnostics_shard2_ready="),
        ("M250-E023-SUR-11", ";advanced_diagnostics_shard2_key_ready="),
        ("M250-E023-SUR-12", "surface.advanced_diagnostics_shard2_ready &&"),
        ("M250-E023-SUR-13", "final readiness gate advanced diagnostics workpack shard2 is inconsistent"),
        (
            "M250-E023-SUR-14",
            "final readiness gate advanced diagnostics workpack shard2 consistency is not satisfied",
        ),
        ("M250-E023-SUR-15", "final readiness gate advanced diagnostics workpack shard2 is not ready"),
        ("M250-E023-SUR-16", "final readiness gate advanced diagnostics workpack shard2 key is not ready"),
    ),
    "frontend_types": (
        ("M250-E023-TYP-01", "bool advanced_diagnostics_shard2_consistent = false;"),
        ("M250-E023-TYP-02", "bool advanced_diagnostics_shard2_ready = false;"),
        ("M250-E023-TYP-03", "std::string advanced_diagnostics_shard2_key;"),
    ),
    "architecture_doc": (
        (
            "M250-E023-ARC-01",
            "M250 lane-E E023 advanced diagnostics shard2 anchors explicit final",
        ),
        ("M250-E023-ARC-02", "advanced_diagnostics_shard2_*"),
    ),
    "package_json": (
        (
            "M250-E023-CFG-01",
            '"check:objc3c:m250-e023-final-readiness-gate-documentation-signoff-advanced-diagnostics-workpack-shard2-contract"',
        ),
        (
            "M250-E023-CFG-02",
            '"test:tooling:m250-e023-final-readiness-gate-documentation-signoff-advanced-diagnostics-workpack-shard2-contract"',
        ),
        ("M250-E023-CFG-03", '"check:objc3c:m250-e023-lane-e-readiness"'),
        ("M250-E023-CFG-04", "check:objc3c:m250-e022-lane-e-readiness"),
        ("M250-E023-CFG-05", "check:objc3c:m250-a009-lane-a-readiness"),
        ("M250-E023-CFG-06", "check:objc3c:m250-b010-lane-b-readiness"),
        ("M250-E023-CFG-07", "check:objc3c:m250-c011-lane-c-readiness"),
        ("M250-E023-CFG-08", "check:objc3c:m250-d019-lane-d-readiness"),
    ),
    "e022_contract": (
        (
            "M250-E023-DEP-01",
            "Contract ID: `objc3c-final-readiness-gate-documentation-signoff-advanced-edge-compatibility-workpack-shard2/m250-e022-v1`",
        ),
    ),
    "a009_contract": (
        (
            "M250-E023-DEP-02",
            "Contract ID: `objc3c-frontend-stability-long-tail-grammar-conformance-matrix/m250-a009-v1`",
        ),
    ),
    "b010_contract": (
        (
            "M250-E023-DEP-03",
            "Contract ID: `objc3c-semantic-stability-spec-delta-closure-conformance-corpus-expansion/m250-b010-v1`",
        ),
    ),
    "c011_contract": (
        (
            "M250-E023-DEP-04",
            "Contract ID: `objc3c-lowering-runtime-stability-performance-quality-guardrails/m250-c011-v1`",
        ),
    ),
    "d019_contract": (
        (
            "M250-E023-DEP-05",
            "Contract ID: `objc3c-toolchain-runtime-ga-operations-readiness-advanced-integration-workpack-shard1/m250-d019-v1`",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M250-E023-FORB-01", "surface.advanced_diagnostics_shard2_ready = true;"),
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
            "tmp/reports/m250/M250-E023/final_readiness_gate_documentation_signoff_advanced_diagnostics_workpack_shard2_contract_summary.json"
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






