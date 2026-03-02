#!/usr/bin/env python3
"""Fail-closed validator for M228-D001 object emission/link-path reliability freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-d001-object-emission-link-path-reliability-freeze-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "io_process_header": ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.h",
    "io_process_source": ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp",
    "io_scaffold_header": ROOT / "native" / "objc3c" / "src" / "io" / "objc3_toolchain_runtime_ga_operations_scaffold.h",
    "io_core_feature_header": ROOT / "native" / "objc3c" / "src" / "io" / "objc3_toolchain_runtime_ga_operations_core_feature_surface.h",
    "frontend_anchor_source": ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
    "contract_doc": ROOT / "docs" / "contracts" / "m228_object_emission_link_path_reliability_contract_freeze_d001_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "io_process_header": (
        ("M228-D001-IOH-01", "int RunObjectiveCCompile(const std::filesystem::path &clang_path,"),
        ("M228-D001-IOH-02", "int RunIRCompile(const std::filesystem::path &clang_path,"),
        ("M228-D001-IOH-03", "int RunIRCompileLLVMDirect(const std::filesystem::path &llc_path,"),
    ),
    "io_process_source": (
        ("M228-D001-IOCPP-01", "void NormalizeCoffTimestamp(const std::filesystem::path &object_out)"),
        ("M228-D001-IOCPP-02", "int RunObjectiveCCompile(const std::filesystem::path &clang_path,"),
        ("M228-D001-IOCPP-03", "int RunIRCompile(const std::filesystem::path &clang_path,"),
        ("M228-D001-IOCPP-04", "int RunIRCompileLLVMDirect(const std::filesystem::path &llc_path,"),
        ("M228-D001-IOCPP-05", "error = \"llvm-direct object emission failed: llc executable not found: \" + llc_path.string();"),
        ("M228-D001-IOCPP-06", "error = \"llvm-direct object emission failed: llc exited with status \" + std::to_string(llc_status) + \" for \" +"),
    ),
    "io_scaffold_header": (
        ("M228-D001-SCF-01", "struct Objc3ToolchainRuntimeGaOperationsScaffold {"),
        ("M228-D001-SCF-02", "bool object_artifact_ready = false;"),
        ("M228-D001-SCF-03", "std::string backend_route_key;"),
        ("M228-D001-SCF-04", "BuildObjc3ToolchainRuntimeGaOperationsScaffoldKey("),
        ("M228-D001-SCF-05", "BuildObjc3ToolchainRuntimeGaOperationsScaffold("),
    ),
    "io_core_feature_header": (
        ("M228-D001-CORE-01", "struct Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface {"),
        ("M228-D001-CORE-02", "bool backend_route_deterministic = false;"),
        ("M228-D001-CORE-03", "bool backend_output_payload_consistent = false;"),
        ("M228-D001-CORE-04", "BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureSurface("),
        ("M228-D001-CORE-05", "surface.failure_reason = \"toolchain/runtime backend object emission command failed\";"),
        ("M228-D001-CORE-06", "surface.failure_reason = \"toolchain/runtime core feature implementation is not ready\";"),
    ),
    "frontend_anchor_source": (
        ("M228-D001-ANC-01", "compile_status = RunIRCompile(std::filesystem::path(options->clang_path), ir_out, object_out);"),
        ("M228-D001-ANC-02", "compile_status = RunIRCompileLLVMDirect(std::filesystem::path(options->llc_path), ir_out, object_out, backend_error);"),
    ),
    "architecture_doc": (
        ("M228-D001-ARCH-01", "M228 lane-D D001 object emission/link-path reliability freeze anchors compile"),
        ("M228-D001-ARCH-02", "io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h"),
    ),
    "lowering_spec": (
        ("M228-D001-SPC-01", "object emission and link-path routing (clang/llvm-direct) shall remain"),
    ),
    "metadata_spec": (
        ("M228-D001-META-01", "deterministic object-emission backend route keys and output markers for"),
    ),
    "package_json": (
        ("M228-D001-CFG-01", '"check:objc3c:m228-d001-object-emission-link-path-reliability-contract"'),
        ("M228-D001-CFG-02", '"test:tooling:m228-d001-object-emission-link-path-reliability-contract"'),
        ("M228-D001-CFG-03", '"check:objc3c:m228-d001-lane-d-readiness"'),
    ),
    "contract_doc": (
        ("M228-D001-DOC-01", "Contract ID: `objc3c-object-emission-link-path-reliability-freeze/m228-d001-v1`"),
        ("M228-D001-DOC-02", "RunIRCompileLLVMDirect"),
        ("M228-D001-DOC-03", "BuildObjc3ToolchainRuntimeGaOperationsScaffold"),
        ("M228-D001-DOC-04", "tmp/reports/m228/M228-D001/object_emission_link_path_reliability_contract_summary.json"),
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
        default=Path("tmp/reports/m228/M228-D001/object_emission_link_path_reliability_contract_summary.json"),
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

    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": total_checks,
        "checks_passed": passed_checks,
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in findings
        ],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if findings:
        for finding in findings:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
