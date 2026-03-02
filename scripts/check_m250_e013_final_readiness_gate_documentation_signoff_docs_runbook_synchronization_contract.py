#!/usr/bin/env python3
"""Fail-closed validator for M250-E013 final readiness docs/runbook synchronization."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m250-final-readiness-gate-documentation-signoff-docs-runbook-synchronization-contract-e013-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_final_readiness_gate_documentation_signoff_docs_runbook_synchronization_e013_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_e013_final_readiness_gate_documentation_signoff_docs_runbook_synchronization_packet.md",
    "core_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_final_readiness_gate_core_feature_implementation_surface.h",
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "package_json": ROOT / "package.json",
    "e012_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_final_readiness_gate_documentation_signoff_cross_lane_integration_sync_e012_expectations.md",
    "a005_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_frontend_stability_long_tail_grammar_edge_compatibility_completion_a005_expectations.md",
    "b006_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_semantic_stability_spec_delta_closure_edge_case_expansion_and_robustness_b006_expectations.md",
    "c006_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_lowering_runtime_stability_edge_case_expansion_and_robustness_c006_expectations.md",
    "d011_contract": ROOT
    / "docs"
    / "contracts"
    / "m250_toolchain_runtime_ga_operations_readiness_performance_quality_guardrails_d011_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        (
            "M250-E013-DOC-01",
            "Contract ID: `objc3c-final-readiness-gate-documentation-signoff-docs-runbook-synchronization/m250-e013-v1`",
        ),
        ("M250-E013-DOC-02", "`M250-E012`"),
        ("M250-E013-DOC-03", "`M250-A005`"),
        ("M250-E013-DOC-04", "`M250-B006`"),
        ("M250-E013-DOC-05", "`M250-C006`"),
        ("M250-E013-DOC-06", "`M250-D011`"),
        (
            "M250-E013-DOC-07",
            "scripts/check_m250_e013_final_readiness_gate_documentation_signoff_docs_runbook_synchronization_contract.py",
        ),
        (
            "M250-E013-DOC-08",
            "tests/tooling/test_check_m250_e013_final_readiness_gate_documentation_signoff_docs_runbook_synchronization_contract.py",
        ),
        ("M250-E013-DOC-09", "npm run check:objc3c:m250-e013-lane-e-readiness"),
    ),
    "packet_doc": (
        ("M250-E013-PKT-01", "Packet: `M250-E013`"),
        (
            "M250-E013-PKT-02",
            "Dependencies: `M250-E012`, `M250-A005`, `M250-B006`, `M250-C006`, `M250-D011`",
        ),
        (
            "M250-E013-PKT-03",
            "scripts/check_m250_e013_final_readiness_gate_documentation_signoff_docs_runbook_synchronization_contract.py",
        ),
    ),
    "core_surface_header": (
        ("M250-E013-SUR-01", "BuildObjc3FinalReadinessGateDocsRunbookSyncKey("),
        ("M250-E013-SUR-02", "const bool lane_docs_runbook_sync_consistent ="),
        ("M250-E013-SUR-03", "const bool docs_runbook_sync_consistent ="),
        ("M250-E013-SUR-04", "const bool docs_runbook_sync_ready ="),
        ("M250-E013-SUR-05", "surface.docs_runbook_sync_consistent ="),
        ("M250-E013-SUR-06", "surface.docs_runbook_sync_ready ="),
        ("M250-E013-SUR-07", "surface.docs_runbook_sync_key ="),
        ("M250-E013-SUR-08", "final-readiness-gate-docs-runbook-sync:v1:"),
        ("M250-E013-SUR-09", ";docs_runbook_sync_consistent="),
        ("M250-E013-SUR-10", ";docs_runbook_sync_ready="),
        ("M250-E013-SUR-11", ";docs_runbook_sync_key_ready="),
        ("M250-E013-SUR-12", "surface.docs_runbook_sync_ready &&"),
        ("M250-E013-SUR-13", "final readiness gate docs and operator runbook sync is inconsistent"),
        (
            "M250-E013-SUR-14",
            "final readiness gate docs and operator runbook sync consistency is not satisfied",
        ),
        ("M250-E013-SUR-15", "final readiness gate docs and operator runbook sync is not ready"),
        ("M250-E013-SUR-16", "final readiness gate docs and operator runbook sync key is not ready"),
    ),
    "frontend_types": (
        ("M250-E013-TYP-01", "bool docs_runbook_sync_consistent = false;"),
        ("M250-E013-TYP-02", "bool docs_runbook_sync_ready = false;"),
        ("M250-E013-TYP-03", "std::string docs_runbook_sync_key;"),
    ),
    "architecture_doc": (
        (
            "M250-E013-ARC-01",
            "M250 lane-E E013 docs/runbook synchronization anchors explicit final",
        ),
        ("M250-E013-ARC-02", "docs_runbook_sync_*"),
    ),
    "package_json": (
        (
            "M250-E013-CFG-01",
            '"check:objc3c:m250-e013-final-readiness-gate-documentation-signoff-docs-runbook-synchronization-contract"',
        ),
        (
            "M250-E013-CFG-02",
            '"test:tooling:m250-e013-final-readiness-gate-documentation-signoff-docs-runbook-synchronization-contract"',
        ),
        ("M250-E013-CFG-03", '"check:objc3c:m250-e013-lane-e-readiness"'),
        ("M250-E013-CFG-04", "check:objc3c:m250-e012-lane-e-readiness"),
        ("M250-E013-CFG-05", "check:objc3c:m250-a005-lane-a-readiness"),
        ("M250-E013-CFG-06", "check:objc3c:m250-b006-lane-b-readiness"),
        ("M250-E013-CFG-07", "check:objc3c:m250-c006-lane-c-readiness"),
        ("M250-E013-CFG-08", "check:objc3c:m250-d011-lane-d-readiness"),
    ),
    "e012_contract": (
        (
            "M250-E013-DEP-01",
            "Contract ID: `objc3c-final-readiness-gate-documentation-signoff-cross-lane-integration-sync/m250-e012-v1`",
        ),
    ),
    "a005_contract": (
        (
            "M250-E013-DEP-02",
            "Contract ID: `objc3c-frontend-stability-long-tail-grammar-edge-compatibility-completion/m250-a005-v1`",
        ),
    ),
    "b006_contract": (
        (
            "M250-E013-DEP-03",
            "Contract ID: `objc3c-semantic-stability-spec-delta-closure-edge-case-expansion-and-robustness/m250-b006-v1`",
        ),
    ),
    "c006_contract": (
        (
            "M250-E013-DEP-04",
            "Contract ID: `objc3c-lowering-runtime-stability-edge-case-expansion-and-robustness/m250-c006-v1`",
        ),
    ),
    "d011_contract": (
        (
            "M250-E013-DEP-05",
            "Contract ID: `objc3c-toolchain-runtime-ga-operations-readiness-performance-quality-guardrails/m250-d011-v1`",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M250-E013-FORB-01", "surface.docs_runbook_sync_ready = true;"),
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
            "tmp/reports/m250/M250-E013/final_readiness_gate_documentation_signoff_docs_runbook_synchronization_contract_summary.json"
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
