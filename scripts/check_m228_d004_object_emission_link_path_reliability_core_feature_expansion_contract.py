#!/usr/bin/env python3
"""Fail-closed validator for M228-D004 object emission/link-path core feature expansion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-d004-object-emission-link-path-reliability-core-feature-expansion-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "core_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "io"
    / "objc3_toolchain_runtime_ga_operations_core_feature_surface.h",
    "frontend_anchor_source": ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "d003_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_object_emission_link_path_reliability_core_feature_implementation_d003_expectations.md",
    "d003_checker": ROOT
    / "scripts"
    / "check_m228_d003_object_emission_link_path_reliability_core_feature_implementation_contract.py",
    "d003_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m228_d003_object_emission_link_path_reliability_core_feature_implementation_contract.py",
    "d003_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_d003_object_emission_link_path_reliability_core_feature_implementation_packet.md",
    "package_json": ROOT / "package.json",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_object_emission_link_path_reliability_core_feature_expansion_d004_expectations.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_d004_object_emission_link_path_reliability_core_feature_expansion_packet.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M228-D004-SUR-01", "bool backend_output_path_deterministic = false;"),
        ("M228-D004-SUR-02", "bool backend_output_payload_consistent = false;"),
        ("M228-D004-SUR-03", "bool core_feature_expansion_ready = false;"),
        ("M228-D004-SUR-04", "std::string core_feature_expansion_key;"),
        ("M228-D004-SUR-05", "BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureExpansionKey("),
        ("M228-D004-SUR-06", "surface.backend_output_path_deterministic ="),
        ("M228-D004-SUR-07", "surface.backend_output_payload_consistent ="),
        ("M228-D004-SUR-08", "surface.core_feature_expansion_ready ="),
        (
            "M228-D004-SUR-09",
            "surface.core_feature_impl_ready = surface.core_feature_impl_ready && surface.core_feature_expansion_ready;",
        ),
        (
            "M228-D004-SUR-10",
            "surface.core_feature_expansion_key = BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureExpansionKey(surface);",
        ),
        (
            "M228-D004-SUR-11",
            'surface.failure_reason = "toolchain/runtime backend output path is not deterministic";',
        ),
        (
            "M228-D004-SUR-12",
            'surface.failure_reason = "toolchain/runtime backend output marker payload is inconsistent";',
        ),
        (
            "M228-D004-SUR-13",
            'surface.failure_reason = "toolchain/runtime core feature expansion is not ready";',
        ),
    ),
    "frontend_anchor_source": (
        (
            "M228-D004-ANC-01",
            'const std::filesystem::path backend_out = out_dir / (emit_prefix + ".object-backend.txt");',
        ),
        (
            "M228-D004-ANC-02",
            'const std::string backend_text = wants_clang_backend ? "clang\\n" : "llvm-direct\\n";',
        ),
        ("M228-D004-ANC-03", "std::string backend_output_payload;"),
        ("M228-D004-ANC-04", "backend_output_payload = backend_text;"),
        ("M228-D004-ANC-05", "BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureSurface("),
        ("M228-D004-ANC-06", "backend_out,"),
        ("M228-D004-ANC-07", "backend_output_payload);"),
        ("M228-D004-ANC-08", "toolchain/runtime core feature fail-closed: "),
        ("M228-D004-ANC-09", '" [O3E002]"'),
    ),
    "architecture_doc": (
        (
            "M228-D004-ARC-01",
            "M228 lane-D D004 core feature expansion anchors explicit backend marker-path",
        ),
        (
            "M228-D004-ARC-02",
            "`io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h`",
        ),
        (
            "M228-D004-ARC-03",
            "`libobjc3c_frontend/frontend_anchor.cpp` so object emission/link-path",
        ),
    ),
    "lowering_spec": (
        (
            "M228-D004-SPC-01",
            "toolchain/runtime core feature expansion shall remain fail-closed on backend",
        ),
        (
            "M228-D004-SPC-02",
            "marker payload-to-route consistency drift",
        ),
    ),
    "metadata_spec": (
        (
            "M228-D004-META-01",
            "deterministic toolchain/runtime core-feature expansion readiness/key markers for",
        ),
    ),
    "d003_contract_doc": (
        (
            "M228-D004-DEP-01",
            "Contract ID: `objc3c-object-emission-link-path-reliability-core-feature-implementation/m228-d003-v1`",
        ),
    ),
    "d003_checker": (
        (
            "M228-D004-DEP-02",
            'MODE = "m228-d003-object-emission-link-path-reliability-core-feature-implementation-contract-v1"',
        ),
    ),
    "d003_tooling_test": (
        (
            "M228-D004-DEP-03",
            "check_m228_d003_object_emission_link_path_reliability_core_feature_implementation_contract",
        ),
    ),
    "d003_packet_doc": (
        ("M228-D004-DEP-04", "Packet: `M228-D003`"),
        ("M228-D004-DEP-05", "Dependencies: `M228-D002`"),
    ),
    "package_json": (
        (
            "M228-D004-CFG-01",
            '"check:objc3c:m228-d004-object-emission-link-path-reliability-core-feature-expansion-contract"',
        ),
        (
            "M228-D004-CFG-02",
            '"test:tooling:m228-d004-object-emission-link-path-reliability-core-feature-expansion-contract"',
        ),
        ("M228-D004-CFG-03", '"check:objc3c:m228-d004-lane-d-readiness"'),
        (
            "M228-D004-CFG-04",
            "npm run check:objc3c:m228-d003-lane-d-readiness && npm run check:objc3c:m228-d004-object-emission-link-path-reliability-core-feature-expansion-contract",
        ),
        ("M228-D004-CFG-05", '"compile:objc3c":'),
        ("M228-D004-CFG-06", '"proof:objc3c":'),
        ("M228-D004-CFG-07", '"test:objc3c:execution-replay-proof":'),
        ("M228-D004-CFG-08", '"test:objc3c:perf-budget":'),
    ),
    "contract_doc": (
        (
            "M228-D004-DOC-01",
            "Contract ID: `objc3c-object-emission-link-path-reliability-core-feature-expansion/m228-d004-v1`",
        ),
        ("M228-D004-DOC-02", "Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface"),
        ("M228-D004-DOC-03", "BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureExpansionKey"),
        ("M228-D004-DOC-04", "IsObjc3ToolchainRuntimeGaOperationsCoreFeatureSurfaceReady"),
        ("M228-D004-DOC-05", "O3E002"),
        (
            "M228-D004-DOC-06",
            "scripts/check_m228_d004_object_emission_link_path_reliability_core_feature_expansion_contract.py",
        ),
        (
            "M228-D004-DOC-07",
            "tests/tooling/test_check_m228_d004_object_emission_link_path_reliability_core_feature_expansion_contract.py",
        ),
        ("M228-D004-DOC-08", "npm run check:objc3c:m228-d004-lane-d-readiness"),
        ("M228-D004-DOC-09", "npm run build:objc3c-native"),
        (
            "M228-D004-DOC-10",
            "tmp/reports/m228/M228-D004/object_emission_link_path_reliability_core_feature_expansion_contract_summary.json",
        ),
        ("M228-D004-DOC-11", "Dependencies: `M228-D003`"),
        (
            "M228-D004-DOC-12",
            "spec/planning/compiler/m228/m228_d004_object_emission_link_path_reliability_core_feature_expansion_packet.md",
        ),
    ),
    "planning_packet": (
        (
            "M228-D004-PKT-01",
            "# M228-D004 Object Emission and Link Path Reliability Core Feature Expansion Packet",
        ),
        ("M228-D004-PKT-02", "Packet: `M228-D004`"),
        ("M228-D004-PKT-03", "Milestone: `M228`"),
        ("M228-D004-PKT-04", "Dependencies: `M228-D003`"),
        (
            "M228-D004-PKT-05",
            "docs/contracts/m228_object_emission_link_path_reliability_core_feature_expansion_d004_expectations.md",
        ),
        (
            "M228-D004-PKT-06",
            "scripts/check_m228_d004_object_emission_link_path_reliability_core_feature_expansion_contract.py",
        ),
        (
            "M228-D004-PKT-07",
            "tests/tooling/test_check_m228_d004_object_emission_link_path_reliability_core_feature_expansion_contract.py",
        ),
        ("M228-D004-PKT-08", "check:objc3c:m228-d004-lane-d-readiness"),
        (
            "M228-D004-PKT-09",
            "tmp/reports/m228/M228-D004/object_emission_link_path_reliability_core_feature_expansion_contract_summary.json",
        ),
        ("M228-D004-PKT-10", "backend marker path/payload determinism"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M228-D004-FORB-01", "surface.core_feature_expansion_ready = true;"),
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
            "tmp/reports/m228/M228-D004/object_emission_link_path_reliability_core_feature_expansion_contract_summary.json"
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
