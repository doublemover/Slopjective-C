#!/usr/bin/env python3
"""Fail-closed validator for M248-D016 runner/platform advanced edge compatibility workpack (shard 1)."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m248-runner-reliability-platform-operations-advanced-edge-compatibility-workpack-shard1-contract-d016-v1"

ARTIFACTS: dict[str, Path] = {
    "parse_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_parse_lowering_readiness_surface.h",
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "frontend_artifacts": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_frontend_artifacts.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "d015_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m248_runner_reliability_and_platform_operations_advanced_core_workpack_shard1_d015_expectations.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m248_runner_reliability_and_platform_operations_advanced_edge_compatibility_workpack_shard1_d016_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m248"
    / "m248_d016_runner_reliability_and_platform_operations_advanced_edge_compatibility_workpack_shard1_packet.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "parse_surface_header": (
        (
            "M248-D016-SUR-01",
            "IsObjc3ToolchainRuntimeGaOperationsAdvancedEdgeCompatibilityConsistent(",
        ),
        (
            "M248-D016-SUR-02",
            "IsObjc3ToolchainRuntimeGaOperationsAdvancedEdgeCompatibilityReady(",
        ),
        (
            "M248-D016-SUR-03",
            "BuildObjc3ToolchainRuntimeGaOperationsAdvancedEdgeCompatibilityKey(",
        ),
        (
            "M248-D016-SUR-04",
            "toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent",
        ),
        (
            "M248-D016-SUR-05",
            "toolchain_runtime_ga_operations_advanced_edge_compatibility_ready",
        ),
        (
            "M248-D016-SUR-06",
            "toolchain_runtime_ga_operations_advanced_edge_compatibility_key",
        ),
        (
            "M248-D016-SUR-07",
            "toolchain/runtime GA operations advanced edge compatibility workpack is inconsistent",
        ),
        (
            "M248-D016-SUR-08",
            "toolchain/runtime GA operations advanced edge compatibility workpack is not ready",
        ),
    ),
    "frontend_types": (
        (
            "M248-D016-TYP-01",
            "bool toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent = false;",
        ),
        (
            "M248-D016-TYP-02",
            "bool toolchain_runtime_ga_operations_advanced_edge_compatibility_ready = false;",
        ),
        (
            "M248-D016-TYP-03",
            "std::string toolchain_runtime_ga_operations_advanced_edge_compatibility_key;",
        ),
    ),
    "frontend_artifacts": (
        (
            "M248-D016-ART-01",
            '\\"toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent\\": ',
        ),
        (
            "M248-D016-ART-02",
            '\\"toolchain_runtime_ga_operations_advanced_edge_compatibility_ready\\": ',
        ),
        (
            "M248-D016-ART-03",
            '\\"toolchain_runtime_ga_operations_advanced_edge_compatibility_key\\":\\"',
        ),
    ),
    "architecture_doc": (
        ("M248-D016-ARC-01", "M248 lane-D D016 advanced edge compatibility workpack (shard 1)"),
        (
            "M248-D016-ARC-02",
            "toolchain_runtime_ga_operations_advanced_edge_compatibility_*",
        ),
    ),
    "d015_contract_doc": (
        (
            "M248-D016-DEP-01",
            "Contract ID: `objc3c-runner-reliability-platform-operations-advanced-core-workpack-shard1/m248-d015-v1`",
        ),
    ),
    "contract_doc": (
        (
            "M248-D016-DOC-01",
            "Contract ID: `objc3c-runner-reliability-platform-operations-advanced-edge-compatibility-workpack-shard1/m248-d016-v1`",
        ),
        (
            "M248-D016-DOC-02",
            "IsObjc3ToolchainRuntimeGaOperationsAdvancedEdgeCompatibilityConsistent",
        ),
        (
            "M248-D016-DOC-03",
            "IsObjc3ToolchainRuntimeGaOperationsAdvancedEdgeCompatibilityReady",
        ),
        (
            "M248-D016-DOC-04",
            "scripts/check_m248_d016_runner_reliability_and_platform_operations_advanced_edge_compatibility_workpack_shard1_contract.py",
        ),
        (
            "M248-D016-DOC-05",
            "tests/tooling/test_check_m248_d016_runner_reliability_and_platform_operations_advanced_edge_compatibility_workpack_shard1_contract.py",
        ),
        ("M248-D016-DOC-06", "npm run check:objc3c:m248-d016-lane-d-readiness"),
    ),
    "packet_doc": (
        ("M248-D016-PKT-01", "Packet: `M248-D016`"),
        ("M248-D016-PKT-02", "Dependencies: `M248-D015`"),
        ("M248-D016-PKT-03", "Issue: `#6851`"),
        (
            "M248-D016-PKT-04",
            "scripts/check_m248_d016_runner_reliability_and_platform_operations_advanced_edge_compatibility_workpack_shard1_contract.py",
        ),
    ),
    "lowering_spec": (
        (
            "M248-D016-SPC-01",
            "runner/platform operations advanced edge compatibility workpack (shard 1) governance shall preserve",
        ),
    ),
    "metadata_spec": (
        (
            "M248-D016-META-01",
            "deterministic lane-D runner/platform operations advanced edge compatibility workpack (shard 1) metadata anchors for `M248-D016`",
        ),
    ),
    "package_json": (
        (
            "M248-D016-CFG-01",
            '"check:objc3c:m248-d016-runner-reliability-platform-operations-advanced-edge-compatibility-workpack-shard1-contract"',
        ),
        (
            "M248-D016-CFG-02",
            '"test:tooling:m248-d016-runner-reliability-platform-operations-advanced-edge-compatibility-workpack-shard1-contract"',
        ),
        ("M248-D016-CFG-03", '"check:objc3c:m248-d016-lane-d-readiness"'),
        ("M248-D016-CFG-04", "python scripts/run_m248_d016_lane_d_readiness.py"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "parse_surface_header": (
        (
            "M248-D016-FORB-01",
            "toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent = true;",
        ),
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
            "tmp/reports/m248/M248-D016/runner_reliability_and_platform_operations_advanced_edge_compatibility_workpack_shard1_contract_summary.json"
        ),
    )
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit canonical summary JSON to stdout.",
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

    if args.emit_json:
        print(canonical_json(summary), end="")

    if ok:
        if not args.emit_json:
            print(f"[ok] {MODE}: {passed_checks}/{total_checks} checks passed")
        return 0

    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
