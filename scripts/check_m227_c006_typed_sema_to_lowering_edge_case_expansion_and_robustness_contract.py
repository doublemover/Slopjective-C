#!/usr/bin/env python3
"""Fail-closed validator for M227-C006 typed sema-to-lowering edge-case expansion and robustness."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-typed-sema-to-lowering-edge-case-expansion-and-robustness-c006-v1"

ARTIFACTS: dict[str, Path] = {
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "typed_surface": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_typed_sema_to_lowering_contract_surface.h",
    "parse_readiness": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_parse_lowering_readiness_surface.h",
    "c005_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m227_typed_sema_to_lowering_edge_case_compatibility_completion_c005_expectations.md",
    "c005_checker": ROOT
    / "scripts"
    / "check_m227_c005_typed_sema_to_lowering_edge_case_compatibility_completion_contract.py",
    "c005_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m227_c005_typed_sema_to_lowering_edge_case_compatibility_completion_contract.py",
    "c005_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_c005_typed_sema_to_lowering_edge_case_compatibility_completion_packet.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m227_typed_sema_to_lowering_edge_case_expansion_and_robustness_c006_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_c006_typed_sema_to_lowering_edge_case_expansion_and_robustness_packet.md",
    "package_json": ROOT / "package.json",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "frontend_types": (
        ("M227-C006-TYP-01", "bool typed_core_feature_edge_case_expansion_consistent = false;"),
        ("M227-C006-TYP-02", "bool typed_core_feature_edge_case_robustness_ready = false;"),
        ("M227-C006-TYP-03", "std::string typed_core_feature_edge_case_robustness_key;"),
        ("M227-C006-TYP-04", "bool typed_sema_edge_case_expansion_consistent = false;"),
        ("M227-C006-TYP-05", "bool typed_sema_edge_case_robustness_ready = false;"),
        ("M227-C006-TYP-06", "std::string typed_sema_edge_case_robustness_key;"),
    ),
    "typed_surface": (
        ("M227-C006-SUR-01", "BuildObjc3TypedSemaToLoweringCoreFeatureEdgeRobustnessKey("),
        ("M227-C006-SUR-02", "surface.typed_core_feature_edge_case_expansion_consistent ="),
        ("M227-C006-SUR-03", "surface.typed_core_feature_edge_case_robustness_ready ="),
        ("M227-C006-SUR-04", "surface.typed_core_feature_edge_case_robustness_key ="),
        ("M227-C006-SUR-05", "typed_core_feature_edge_case_robustness_key_ready"),
        ("M227-C006-SUR-06", "typed sema-to-lowering edge-case expansion is inconsistent"),
        ("M227-C006-SUR-07", "typed sema-to-lowering edge-case robustness is not ready"),
        ("M227-C006-SUR-08", "typed sema-to-lowering edge-case robustness key is empty"),
    ),
    "parse_readiness": (
        ("M227-C006-REA-01", "surface.typed_sema_edge_case_expansion_consistent ="),
        ("M227-C006-REA-02", "surface.typed_sema_edge_case_robustness_key ="),
        ("M227-C006-REA-03", "surface.typed_sema_edge_case_robustness_ready ="),
        ("M227-C006-REA-04", "const bool typed_edge_case_robustness_alignment ="),
        ("M227-C006-REA-05", "surface.typed_sema_edge_case_robustness_ready &&"),
        ("M227-C006-REA-06", "!surface.typed_sema_edge_case_robustness_key.empty() &&"),
        ("M227-C006-REA-07", "typed sema-to-lowering edge-case robustness drifted from parse/lowering readiness"),
    ),
    "c005_contract_doc": (
        (
            "M227-C006-DEP-01",
            "Contract ID: `objc3c-typed-sema-to-lowering-edge-case-compatibility-completion/m227-c005-v1`",
        ),
    ),
    "c005_checker": (
        ("M227-C006-DEP-02", 'MODE = "m227-typed-sema-to-lowering-edge-case-compatibility-completion-c005-v1"'),
    ),
    "c005_tooling_test": (
        ("M227-C006-DEP-03", "def test_contract_passes_on_repository_sources"),
    ),
    "c005_packet_doc": (
        ("M227-C006-DEP-04", "Packet: `M227-C005`"),
    ),
    "contract_doc": (
        (
            "M227-C006-DOC-01",
            "Contract ID: `objc3c-typed-sema-to-lowering-edge-case-expansion-robustness/m227-c006-v1`",
        ),
        ("M227-C006-DOC-02", "Execute issue `#5126`"),
        ("M227-C006-DOC-03", "Dependencies: `M227-C005`"),
        ("M227-C006-DOC-04", "typed_core_feature_edge_case_robustness_key"),
        ("M227-C006-DOC-05", "check:objc3c:m227-c006-lane-c-readiness"),
    ),
    "packet_doc": (
        ("M227-C006-PKT-01", "Packet: `M227-C006`"),
        ("M227-C006-PKT-02", "Issue: `#5126`"),
        ("M227-C006-PKT-03", "Dependencies: `M227-C005`"),
        (
            "M227-C006-PKT-04",
            "python scripts/check_m227_c006_typed_sema_to_lowering_edge_case_expansion_and_robustness_contract.py",
        ),
    ),
    "package_json": (
        (
            "M227-C006-PKG-01",
            '"check:objc3c:m227-c006-typed-sema-to-lowering-edge-case-expansion-and-robustness-contract"',
        ),
        (
            "M227-C006-PKG-02",
            '"test:tooling:m227-c006-typed-sema-to-lowering-edge-case-expansion-and-robustness-contract"',
        ),
        ("M227-C006-PKG-03", '"check:objc3c:m227-c006-lane-c-readiness"'),
        (
            "M227-C006-PKG-04",
            "scripts/check_m227_c005_typed_sema_to_lowering_edge_case_compatibility_completion_contract.py",
        ),
    ),
    "architecture_doc": (
        (
            "M227-C006-ARC-01",
            "M227 lane-C C006 typed sema-to-lowering edge-case expansion and robustness anchors",
        ),
    ),
    "lowering_spec": (
        (
            "M227-C006-SPC-01",
            "typed sema-to-lowering edge-case expansion and robustness governance shall preserve explicit lane-C dependency anchors (`M227-C006`, `M227-C005`)",
        ),
    ),
    "metadata_spec": (
        (
            "M227-C006-META-01",
            "deterministic lane-C typed sema-to-lowering edge-case expansion and robustness metadata anchors for `M227-C006`",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "typed_surface": (
        ("M227-C006-FORB-01", "surface.typed_core_feature_edge_case_robustness_ready = true;"),
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
        default=Path("tmp/reports/m227/M227-C006/typed_sema_to_lowering_edge_case_expansion_and_robustness_contract_summary.json"),
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
