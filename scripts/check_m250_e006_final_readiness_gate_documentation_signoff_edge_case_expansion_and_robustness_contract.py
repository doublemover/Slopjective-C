#!/usr/bin/env python3
"""Fail-closed validator for M250-E006 final readiness edge-case expansion/robustness."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m250-final-readiness-gate-documentation-signoff-edge-case-expansion-and-robustness-contract-e006-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_final_readiness_gate_documentation_signoff_edge_case_expansion_and_robustness_e006_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_e006_final_readiness_gate_documentation_signoff_edge_case_expansion_and_robustness_packet.md",
    "core_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_final_readiness_gate_core_feature_implementation_surface.h",
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "package_json": ROOT / "package.json",
    "e005_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_final_readiness_gate_documentation_signoff_edge_case_compatibility_completion_e005_expectations.md",
    "a002_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_frontend_stability_long_tail_grammar_modular_split_a002_expectations.md",
    "b003_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_semantic_stability_spec_delta_closure_core_feature_implementation_b003_expectations.md",
    "c003_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_lowering_runtime_stability_core_feature_implementation_c003_expectations.md",
    "d005_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_toolchain_runtime_ga_operations_readiness_edge_case_compatibility_completion_d005_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        (
            "M250-E006-DOC-01",
            "Contract ID: `objc3c-final-readiness-gate-documentation-signoff-edge-case-expansion-and-robustness/m250-e006-v1`",
        ),
        ("M250-E006-DOC-02", "`M250-E005`"),
        ("M250-E006-DOC-03", "`M250-A002`"),
        ("M250-E006-DOC-04", "`M250-B003`"),
        ("M250-E006-DOC-05", "`M250-C003`"),
        ("M250-E006-DOC-06", "`M250-D005`"),
        (
            "M250-E006-DOC-07",
            "scripts/check_m250_e006_final_readiness_gate_documentation_signoff_edge_case_expansion_and_robustness_contract.py",
        ),
        (
            "M250-E006-DOC-08",
            "tests/tooling/test_check_m250_e006_final_readiness_gate_documentation_signoff_edge_case_expansion_and_robustness_contract.py",
        ),
        ("M250-E006-DOC-09", "npm run check:objc3c:m250-e006-lane-e-readiness"),
    ),
    "packet_doc": (
        ("M250-E006-PKT-01", "Packet: `M250-E006`"),
        (
            "M250-E006-PKT-02",
            "Dependencies: `M250-E005`, `M250-A002`, `M250-B003`, `M250-C003`, `M250-D005`",
        ),
        (
            "M250-E006-PKT-03",
            "scripts/check_m250_e006_final_readiness_gate_documentation_signoff_edge_case_expansion_and_robustness_contract.py",
        ),
    ),
    "core_surface_header": (
        ("M250-E006-SUR-01", "BuildObjc3FinalReadinessGateEdgeCaseRobustnessKey("),
        ("M250-E006-SUR-02", "const bool lane_edge_case_expansion_consistent ="),
        ("M250-E006-SUR-03", "const bool edge_case_expansion_consistent ="),
        ("M250-E006-SUR-04", "const bool edge_case_robustness_ready ="),
        ("M250-E006-SUR-05", "surface.edge_case_expansion_consistent ="),
        ("M250-E006-SUR-06", "surface.edge_case_robustness_ready ="),
        ("M250-E006-SUR-07", "surface.edge_case_robustness_key ="),
        ("M250-E006-SUR-08", "final-readiness-gate-edge-case-robustness:v1:"),
        ("M250-E006-SUR-09", ";edge_case_expansion_consistent="),
        ("M250-E006-SUR-10", ";edge_case_robustness_ready="),
        ("M250-E006-SUR-11", ";edge_case_robustness_key_ready="),
        ("M250-E006-SUR-12", "surface.edge_case_robustness_ready &&"),
        ("M250-E006-SUR-13", "final readiness gate edge-case expansion is inconsistent"),
        (
            "M250-E006-SUR-14",
            "final readiness gate edge-case expansion consistency is not satisfied",
        ),
        ("M250-E006-SUR-15", "final readiness gate edge-case robustness is not ready"),
        ("M250-E006-SUR-16", "final readiness gate edge-case robustness key is not ready"),
    ),
    "frontend_types": (
        ("M250-E006-TYP-01", "bool edge_case_expansion_consistent = false;"),
        ("M250-E006-TYP-02", "bool edge_case_robustness_ready = false;"),
        ("M250-E006-TYP-03", "std::string edge_case_robustness_key;"),
    ),
    "architecture_doc": (
        (
            "M250-E006-ARC-01",
            "M250 lane-E E006 edge-case expansion and robustness anchors explicit final",
        ),
        ("M250-E006-ARC-02", "edge_case_expansion_*"),
        ("M250-E006-ARC-03", "edge_case_robustness_*"),
    ),
    "package_json": (
        (
            "M250-E006-CFG-01",
            '"check:objc3c:m250-e006-final-readiness-gate-documentation-signoff-edge-case-expansion-and-robustness-contract"',
        ),
        (
            "M250-E006-CFG-02",
            '"test:tooling:m250-e006-final-readiness-gate-documentation-signoff-edge-case-expansion-and-robustness-contract"',
        ),
        ("M250-E006-CFG-03", '"check:objc3c:m250-e006-lane-e-readiness"'),
        ("M250-E006-CFG-04", "check:objc3c:m250-e005-lane-e-readiness"),
        ("M250-E006-CFG-05", "check:objc3c:m250-a002-lane-a-readiness"),
        ("M250-E006-CFG-06", "check:objc3c:m250-b003-lane-b-readiness"),
        ("M250-E006-CFG-07", "check:objc3c:m250-c003-lane-c-readiness"),
        ("M250-E006-CFG-08", "check:objc3c:m250-d005-lane-d-readiness"),
    ),
    "e005_contract": (
        (
            "M250-E006-DEP-01",
            "Contract ID: `objc3c-final-readiness-gate-documentation-signoff-edge-case-compatibility-completion/m250-e005-v1`",
        ),
    ),
    "a002_contract": (
        (
            "M250-E006-DEP-02",
            "Contract ID: `objc3c-frontend-stability-long-tail-grammar-modular-split/m250-a002-v1`",
        ),
    ),
    "b003_contract": (
        (
            "M250-E006-DEP-03",
            "Contract ID: `objc3c-semantic-stability-spec-delta-closure-core-feature-implementation/m250-b003-v1`",
        ),
    ),
    "c003_contract": (
        (
            "M250-E006-DEP-04",
            "Contract ID: `objc3c-lowering-runtime-stability-core-feature-implementation/m250-c003-v1`",
        ),
    ),
    "d005_contract": (
        (
            "M250-E006-DEP-05",
            "Contract ID: `objc3c-toolchain-runtime-ga-operations-readiness-edge-case-compatibility-completion/m250-d005-v1`",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M250-E006-FORB-01", "surface.edge_case_robustness_ready = true;"),
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
            "tmp/reports/m250/M250-E006/final_readiness_gate_documentation_signoff_edge_case_expansion_and_robustness_contract_summary.json"
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
