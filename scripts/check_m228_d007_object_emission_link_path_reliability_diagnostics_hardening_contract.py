#!/usr/bin/env python3
"""Fail-closed validator for M228-D007 object emission/link-path diagnostics hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-d007-object-emission-link-path-reliability-diagnostics-hardening-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "core_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "io"
    / "objc3_toolchain_runtime_ga_operations_core_feature_surface.h",
    "d006_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_object_emission_link_path_reliability_edge_case_expansion_and_robustness_d006_expectations.md",
    "d006_checker": ROOT
    / "scripts"
    / "check_m228_d006_object_emission_link_path_reliability_edge_case_expansion_and_robustness_contract.py",
    "d006_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m228_d006_object_emission_link_path_reliability_edge_case_expansion_and_robustness_contract.py",
    "d006_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_d006_object_emission_link_path_reliability_edge_case_expansion_and_robustness_packet.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_object_emission_link_path_reliability_diagnostics_hardening_d007_expectations.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_d007_object_emission_link_path_reliability_diagnostics_hardening_packet.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M228-D007-SUR-01", "bool diagnostics_hardening_key_ready = false;"),
        ("M228-D007-SUR-02", "BuildObjc3ToolchainRuntimeGaOperationsDiagnosticsHardeningKey("),
        ("M228-D007-SUR-03", ";backend_output_path_deterministic="),
        ("M228-D007-SUR-04", ";backend_output_path="),
        ("M228-D007-SUR-05", "surface.diagnostics_hardening_key ="),
        ("M228-D007-SUR-06", "surface.diagnostics_hardening_key_ready ="),
        (
            "M228-D007-SUR-07",
            'surface.diagnostics_hardening_key.find("backend=" + surface.backend_route_key) != std::string::npos',
        ),
        (
            "M228-D007-SUR-08",
            'surface.diagnostics_hardening_key.find(";backend_output_path=" + surface.backend_output_path) !=',
        ),
        (
            "M228-D007-SUR-09",
            "surface.core_feature_impl_ready = surface.core_feature_impl_ready && surface.diagnostics_hardening_key_ready;",
        ),
        ("M228-D007-SUR-10", ";diagnostics_hardening_key_ready="),
        ("M228-D007-SUR-11", "toolchain/runtime diagnostics hardening key is not ready"),
        (
            "M228-D007-SUR-12",
            "inline bool IsObjc3ToolchainRuntimeGaOperationsDiagnosticsHardeningReady(",
        ),
        ("M228-D007-SUR-13", "!surface.diagnostics_hardening_key_ready"),
    ),
    "d006_contract_doc": (
        (
            "M228-D007-DEP-01",
            "Contract ID: `objc3c-object-emission-link-path-reliability-edge-case-expansion-and-robustness/m228-d006-v1`",
        ),
    ),
    "d006_checker": (
        (
            "M228-D007-DEP-02",
            'MODE = "m228-d006-object-emission-link-path-reliability-edge-case-expansion-and-robustness-contract-v1"',
        ),
    ),
    "d006_tooling_test": (
        (
            "M228-D007-DEP-03",
            "check_m228_d006_object_emission_link_path_reliability_edge_case_expansion_and_robustness_contract",
        ),
    ),
    "d006_packet_doc": (
        ("M228-D007-DEP-04", "Packet: `M228-D006`"),
        ("M228-D007-DEP-05", "Dependencies: `M228-D005`"),
    ),
    "contract_doc": (
        (
            "M228-D007-DOC-01",
            "Contract ID: `objc3c-object-emission-link-path-reliability-diagnostics-hardening/m228-d007-v1`",
        ),
        ("M228-D007-DOC-02", "Dependencies: `M228-D006`"),
        ("M228-D007-DOC-03", "diagnostics_hardening_key_ready"),
        (
            "M228-D007-DOC-04",
            "IsObjc3ToolchainRuntimeGaOperationsDiagnosticsHardeningReady",
        ),
        (
            "M228-D007-DOC-05",
            "scripts/check_m228_d007_object_emission_link_path_reliability_diagnostics_hardening_contract.py",
        ),
        (
            "M228-D007-DOC-06",
            "tests/tooling/test_check_m228_d007_object_emission_link_path_reliability_diagnostics_hardening_contract.py",
        ),
        (
            "M228-D007-DOC-07",
            "tmp/reports/m228/M228-D007/object_emission_link_path_reliability_diagnostics_hardening_contract_summary.json",
        ),
        ("M228-D007-DOC-08", "Shared-file deltas required for full lane-D readiness"),
        ("M228-D007-DOC-09", "package.json"),
        ("M228-D007-DOC-10", "native/objc3c/src/ARCHITECTURE.md"),
        ("M228-D007-DOC-11", "spec/LOWERING_AND_RUNTIME_CONTRACTS.md"),
        ("M228-D007-DOC-12", "spec/MODULE_METADATA_AND_ABI_TABLES.md"),
    ),
    "planning_packet": (
        (
            "M228-D007-PKT-01",
            "# M228-D007 Object Emission and Link Path Reliability Diagnostics Hardening Packet",
        ),
        ("M228-D007-PKT-02", "Packet: `M228-D007`"),
        ("M228-D007-PKT-03", "Milestone: `M228`"),
        ("M228-D007-PKT-04", "Dependencies: `M228-D006`"),
        (
            "M228-D007-PKT-05",
            "docs/contracts/m228_object_emission_link_path_reliability_diagnostics_hardening_d007_expectations.md",
        ),
        (
            "M228-D007-PKT-06",
            "scripts/check_m228_d007_object_emission_link_path_reliability_diagnostics_hardening_contract.py",
        ),
        (
            "M228-D007-PKT-07",
            "tests/tooling/test_check_m228_d007_object_emission_link_path_reliability_diagnostics_hardening_contract.py",
        ),
        (
            "M228-D007-PKT-08",
            "tmp/reports/m228/M228-D007/object_emission_link_path_reliability_diagnostics_hardening_contract_summary.json",
        ),
        ("M228-D007-PKT-09", "diagnostics hardening"),
        ("M228-D007-PKT-10", "Shared-file deltas required for full lane-D readiness"),
        (
            "M228-D007-PKT-11",
            "python scripts/check_m228_d006_object_emission_link_path_reliability_edge_case_expansion_and_robustness_contract.py",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M228-D007-FORB-01", "surface.diagnostics_hardening_key_ready = true;"),
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
            "tmp/reports/m228/M228-D007/"
            "object_emission_link_path_reliability_diagnostics_hardening_contract_summary.json"
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
