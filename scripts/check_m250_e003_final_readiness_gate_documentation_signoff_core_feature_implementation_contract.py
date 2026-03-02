#!/usr/bin/env python3
"""Fail-closed validator for M250-E003 final readiness core feature implementation."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m250-final-readiness-gate-documentation-signoff-core-feature-implementation-contract-e003-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_final_readiness_gate_documentation_signoff_core_feature_implementation_e003_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_e003_final_readiness_gate_documentation_signoff_core_feature_implementation_packet.md",
    "core_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_final_readiness_gate_core_feature_implementation_surface.h",
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "package_json": ROOT / "package.json",
    "e002_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_final_readiness_gate_documentation_signoff_modular_split_scaffolding_e002_expectations.md",
    "a003_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_frontend_stability_long_tail_grammar_core_feature_implementation_a003_expectations.md",
    "b003_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_semantic_stability_spec_delta_closure_core_feature_implementation_b003_expectations.md",
    "c003_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_lowering_runtime_stability_core_feature_implementation_c003_expectations.md",
    "d003_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_toolchain_runtime_ga_operations_readiness_core_feature_implementation_d003_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        (
            "M250-E003-DOC-01",
            "Contract ID: `objc3c-final-readiness-gate-documentation-signoff-core-feature-implementation/m250-e003-v1`",
        ),
        ("M250-E003-DOC-02", "`M250-E002`"),
        ("M250-E003-DOC-03", "`M250-A003`"),
        ("M250-E003-DOC-04", "`M250-B003`"),
        ("M250-E003-DOC-05", "`M250-C003`"),
        ("M250-E003-DOC-06", "`M250-D003`"),
        ("M250-E003-DOC-07", "npm run check:objc3c:m250-e003-lane-e-readiness"),
    ),
    "packet_doc": (
        ("M250-E003-PKT-01", "Packet: `M250-E003`"),
        (
            "M250-E003-PKT-02",
            "Dependencies: `M250-E002`, `M250-A003`, `M250-B003`, `M250-C003`, `M250-D003`",
        ),
        (
            "M250-E003-PKT-03",
            "scripts/check_m250_e003_final_readiness_gate_documentation_signoff_core_feature_implementation_contract.py",
        ),
    ),
    "core_surface_header": (
        ("M250-E003-SUR-01", "struct Objc3FinalReadinessGateCoreFeatureScaffold {"),
        (
            "M250-E003-SUR-02",
            "BuildObjc3FinalReadinessGateCoreFeatureImplementationSurface(",
        ),
        (
            "M250-E003-SUR-03",
            "final readiness gate lane-A core feature readiness is not satisfied",
        ),
        (
            "M250-E003-SUR-04",
            "final readiness gate lane-D core feature readiness is not satisfied",
        ),
        ("M250-E003-SUR-05", "dependency_chain_ready"),
    ),
    "frontend_types": (
        (
            "M250-E003-TYP-01",
            "struct Objc3FinalReadinessGateCoreFeatureImplementationSurface {",
        ),
        ("M250-E003-TYP-02", "bool dependency_chain_ready = false;"),
        ("M250-E003-TYP-03", "bool core_feature_impl_ready = false;"),
        ("M250-E003-TYP-04", "std::string core_feature_key;"),
    ),
    "architecture_doc": (
        (
            "M250-E003-ARC-01",
            "M250 lane-E E003 core feature implementation anchors explicit final readiness",
        ),
        ("M250-E003-ARC-02", "final_readiness_gate_core_feature_implementation_surface.h"),
    ),
    "package_json": (
        (
            "M250-E003-CFG-01",
            '"check:objc3c:m250-e003-final-readiness-gate-documentation-signoff-core-feature-implementation-contract"',
        ),
        (
            "M250-E003-CFG-02",
            '"test:tooling:m250-e003-final-readiness-gate-documentation-signoff-core-feature-implementation-contract"',
        ),
        ("M250-E003-CFG-03", '"check:objc3c:m250-e003-lane-e-readiness"'),
        ("M250-E003-CFG-04", "check:objc3c:m250-e002-lane-e-readiness"),
        ("M250-E003-CFG-05", "check:objc3c:m250-a003-lane-a-readiness"),
        ("M250-E003-CFG-06", "check:objc3c:m250-b003-lane-b-readiness"),
        ("M250-E003-CFG-07", "check:objc3c:m250-c003-lane-c-readiness"),
        ("M250-E003-CFG-08", "check:objc3c:m250-d003-lane-d-readiness"),
    ),
    "e002_contract": (
        (
            "M250-E003-DEP-01",
            "Contract ID: `objc3c-final-readiness-gate-documentation-signoff-modular-split-scaffolding/m250-e002-v1`",
        ),
    ),
    "a003_contract": (
        (
            "M250-E003-DEP-02",
            "Contract ID: `objc3c-frontend-stability-long-tail-grammar-core-feature-implementation/m250-a003-v1`",
        ),
    ),
    "b003_contract": (
        (
            "M250-E003-DEP-03",
            "Contract ID: `objc3c-semantic-stability-spec-delta-closure-core-feature-implementation/m250-b003-v1`",
        ),
    ),
    "c003_contract": (
        (
            "M250-E003-DEP-04",
            "Contract ID: `objc3c-lowering-runtime-stability-core-feature-implementation/m250-c003-v1`",
        ),
    ),
    "d003_contract": (
        (
            "M250-E003-DEP-05",
            "Contract ID: `objc3c-toolchain-runtime-ga-operations-readiness-core-feature-implementation/m250-d003-v1`",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M250-E003-FORB-01", "surface.core_feature_impl_ready = true;"),
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
            "tmp/reports/m250/M250-E003/final_readiness_gate_documentation_signoff_core_feature_implementation_contract_summary.json"
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
