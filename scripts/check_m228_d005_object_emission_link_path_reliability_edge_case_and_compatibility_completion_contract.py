#!/usr/bin/env python3
"""Fail-closed validator for M228-D005 object emission/link-path edge-case compatibility completion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-d005-object-emission-link-path-reliability-edge-case-and-compatibility-completion-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "core_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "io"
    / "objc3_toolchain_runtime_ga_operations_core_feature_surface.h",
    "frontend_anchor_source": ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp",
    "d004_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_object_emission_link_path_reliability_core_feature_expansion_d004_expectations.md",
    "d004_checker": ROOT
    / "scripts"
    / "check_m228_d004_object_emission_link_path_reliability_core_feature_expansion_contract.py",
    "d004_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m228_d004_object_emission_link_path_reliability_core_feature_expansion_contract.py",
    "d004_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_d004_object_emission_link_path_reliability_core_feature_expansion_packet.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_object_emission_link_path_reliability_edge_case_and_compatibility_completion_d005_expectations.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_d005_object_emission_link_path_reliability_edge_case_and_compatibility_completion_packet.md",
    "package_json": ROOT / "package.json",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M228-D005-SUR-01", "bool edge_case_compatibility_consistent = false;"),
        ("M228-D005-SUR-02", "bool edge_case_compatibility_ready = false;"),
        ("M228-D005-SUR-03", "std::string edge_case_compatibility_key;"),
        ("M228-D005-SUR-04", "BuildObjc3ToolchainRuntimeGaOperationsEdgeCaseCompatibilityKey("),
        ("M228-D005-SUR-05", "const bool edge_case_route_compatibility_consistent ="),
        ("M228-D005-SUR-06", "const bool edge_case_output_compatibility_consistent ="),
        ("M228-D005-SUR-07", "surface.edge_case_compatibility_consistent ="),
        ("M228-D005-SUR-08", "surface.edge_case_compatibility_ready ="),
        (
            "M228-D005-SUR-09",
            "surface.edge_case_compatibility_key =",
        ),
        (
            "M228-D005-SUR-10",
            "surface.core_feature_impl_ready = surface.core_feature_impl_ready && surface.edge_case_compatibility_ready;",
        ),
        (
            "M228-D005-SUR-11",
            "surface.core_feature_impl_ready && !surface.edge_case_compatibility_key.empty();",
        ),
        ("M228-D005-SUR-12", ";edge_case_compatibility_consistent="),
        ("M228-D005-SUR-13", ";edge_case_compatibility_ready="),
        ("M228-D005-SUR-14", ";edge_case_compatibility_key_ready="),
        ("M228-D005-SUR-15", "toolchain/runtime edge-case compatibility is inconsistent"),
        ("M228-D005-SUR-16", "toolchain/runtime edge-case compatibility is not ready"),
        ("M228-D005-SUR-17", "toolchain/runtime edge-case compatibility key is not ready"),
    ),
    "frontend_anchor_source": (
        ("M228-D005-ANC-01", '#include "io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h"'),
        ("M228-D005-ANC-02", "BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureSurface("),
        ("M228-D005-ANC-03", "IsObjc3ToolchainRuntimeGaOperationsCoreFeatureSurfaceReady("),
        ("M228-D005-ANC-04", "toolchain/runtime core feature fail-closed: "),
        ("M228-D005-ANC-05", '" [O3E002]"'),
    ),
    "d004_contract_doc": (
        (
            "M228-D005-DEP-01",
            "Contract ID: `objc3c-object-emission-link-path-reliability-core-feature-expansion/m228-d004-v1`",
        ),
    ),
    "d004_checker": (
        (
            "M228-D005-DEP-02",
            'MODE = "m228-d004-object-emission-link-path-reliability-core-feature-expansion-contract-v1"',
        ),
    ),
    "d004_tooling_test": (
        (
            "M228-D005-DEP-03",
            "check_m228_d004_object_emission_link_path_reliability_core_feature_expansion_contract",
        ),
    ),
    "d004_packet_doc": (
        ("M228-D005-DEP-04", "Packet: `M228-D004`"),
        ("M228-D005-DEP-05", "Dependencies: `M228-D003`"),
    ),
    "contract_doc": (
        (
            "M228-D005-DOC-01",
            "Contract ID: `objc3c-object-emission-link-path-reliability-edge-case-and-compatibility-completion/m228-d005-v1`",
        ),
        ("M228-D005-DOC-02", "edge_case_compatibility_consistent"),
        ("M228-D005-DOC-03", "edge_case_compatibility_ready"),
        ("M228-D005-DOC-04", "edge_case_compatibility_key"),
        ("M228-D005-DOC-05", "BuildObjc3ToolchainRuntimeGaOperationsEdgeCaseCompatibilityKey"),
        ("M228-D005-DOC-06", "O3E002"),
        (
            "M228-D005-DOC-07",
            "scripts/check_m228_d005_object_emission_link_path_reliability_edge_case_and_compatibility_completion_contract.py",
        ),
        (
            "M228-D005-DOC-08",
            "tests/tooling/test_check_m228_d005_object_emission_link_path_reliability_edge_case_and_compatibility_completion_contract.py",
        ),
        ("M228-D005-DOC-09", "Dependencies: `M228-D004`"),
        (
            "M228-D005-DOC-10",
            "spec/planning/compiler/m228/m228_d005_object_emission_link_path_reliability_edge_case_and_compatibility_completion_packet.md",
        ),
        (
            "M228-D005-DOC-11",
            "tmp/reports/m228/M228-D005/object_emission_link_path_reliability_edge_case_and_compatibility_completion_contract_summary.json",
        ),
        ("M228-D005-DOC-12", "check:objc3c:m228-d005-lane-d-readiness"),
        ("M228-D005-DOC-13", "native/objc3c/src/ARCHITECTURE.md"),
    ),
    "planning_packet": (
        (
            "M228-D005-PKT-01",
            "# M228-D005 Object Emission and Link Path Reliability Edge-Case and Compatibility Completion Packet",
        ),
        ("M228-D005-PKT-02", "Packet: `M228-D005`"),
        ("M228-D005-PKT-03", "Milestone: `M228`"),
        ("M228-D005-PKT-04", "Dependencies: `M228-D004`"),
        (
            "M228-D005-PKT-05",
            "docs/contracts/m228_object_emission_link_path_reliability_edge_case_and_compatibility_completion_d005_expectations.md",
        ),
        (
            "M228-D005-PKT-06",
            "scripts/check_m228_d005_object_emission_link_path_reliability_edge_case_and_compatibility_completion_contract.py",
        ),
        (
            "M228-D005-PKT-07",
            "tests/tooling/test_check_m228_d005_object_emission_link_path_reliability_edge_case_and_compatibility_completion_contract.py",
        ),
        (
            "M228-D005-PKT-08",
            "tmp/reports/m228/M228-D005/object_emission_link_path_reliability_edge_case_and_compatibility_completion_contract_summary.json",
        ),
        ("M228-D005-PKT-09", "edge-case and compatibility completion"),
        ("M228-D005-PKT-10", "`check:objc3c:m228-d005-lane-d-readiness`"),
        ("M228-D005-PKT-11", "`native/objc3c/src/ARCHITECTURE.md`"),
    ),
    "package_json": (
        (
            "M228-D005-CFG-01",
            '"check:objc3c:m228-d005-object-emission-link-path-reliability-edge-case-and-compatibility-completion-contract"',
        ),
        (
            "M228-D005-CFG-02",
            '"test:tooling:m228-d005-object-emission-link-path-reliability-edge-case-and-compatibility-completion-contract"',
        ),
        ("M228-D005-CFG-03", '"check:objc3c:m228-d005-lane-d-readiness"'),
        (
            "M228-D005-CFG-04",
            "npm run check:objc3c:m228-d004-lane-d-readiness && npm run check:objc3c:m228-d005-object-emission-link-path-reliability-edge-case-and-compatibility-completion-contract && npm run test:tooling:m228-d005-object-emission-link-path-reliability-edge-case-and-compatibility-completion-contract",
        ),
    ),
    "architecture_doc": (
        (
            "M228-D005-ARCH-01",
            "M228 lane-D D005 edge-case and compatibility completion anchors",
        ),
    ),
    "lowering_spec": (
        (
            "M228-D005-SPC-01",
            "toolchain/runtime edge-case compatibility completion shall remain",
        ),
    ),
    "metadata_spec": (
        (
            "M228-D005-META-01",
            "deterministic lane-D toolchain/runtime edge-case compatibility",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M228-D005-FORB-01", "surface.edge_case_compatibility_ready = true;"),
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
            "tmp/reports/m228/M228-D005/"
            "object_emission_link_path_reliability_edge_case_and_compatibility_completion_contract_summary.json"
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
