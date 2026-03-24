#!/usr/bin/env python3
"""Checker for M274-D002 header/module/bridge generation implementation."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m274-d002-part11-header-module-bridge-generation-v1"
ISSUE = "M274-D002"
ISSUE_NUMBER = "#7371"
CONTRACT_ID = "objc3c-part11-header-module-and-bridge-generation/m274-d002-v1"
SOURCE_CONTRACT_ID = "objc3c-part11-bridge-packaging-and-toolchain-contract/m274-d001-v1"
PRESERVATION_CONTRACT_ID = "objc3c-part11-ffi-metadata-interface-preservation/m274-c003-v1"
LINK_PLAN_CONTRACT_ID = "objc3c-cross-module-runtime-packaging-link-plan/m258-d002-v1"
HEADER_ARTIFACT = "module.part11-bridge.h"
MODULE_ARTIFACT = "module.part11-bridge.modulemap"
BRIDGE_ARTIFACT = "module.part11-bridge.json"
IMPORT_ARTIFACT = "module.runtime-import-surface.json"
LINK_PLAN_ARTIFACT = "module.cross-module-runtime-link-plan.json"
MANIFEST_ARTIFACT = "module.manifest.json"
REGISTRATION_MANIFEST_ARTIFACT = "module.runtime-registration-manifest.json"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m274" / "M274-D002" / "header_module_bridge_generation_summary.json"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m274" / "d002"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m274_header_module_and_bridge_generation_implementation_core_feature_implementation_d002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m274" / "m274_d002_header_module_and_bridge_generation_implementation_core_feature_implementation_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_TABLES = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCH_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
FRONTEND_TYPES_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_ARTIFACTS_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.h"
FRONTEND_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
MANIFEST_ARTIFACTS_H = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.h"
MANIFEST_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
IMPORT_SURFACE_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.h"
IMPORT_SURFACE_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.cpp"
PROCESS_H = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.h"
PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
LOWER_H = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWER_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_H = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
RUNTIME_INTERNAL = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
PACKAGE_JSON = ROOT / "package.json"
CHECKER_FILE = ROOT / "scripts" / "check_m274_d002_header_module_and_bridge_generation_implementation_core_feature_implementation.py"
READINESS_RUNNER = ROOT / "scripts" / "run_m274_d002_lane_d_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m274_d002_header_module_and_bridge_generation_implementation_core_feature_implementation.py"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_LIB = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
PROVIDER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m274_d002_header_module_bridge_provider.objc3"
CONSUMER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m274_d002_header_module_bridge_consumer.objc3"
RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m274_d002_header_module_bridge_generation_probe.cpp"
SURFACE_KEY = "objc_part11_header_module_and_bridge_generation"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


STATIC_SNIPPETS: dict[Path, tuple[SnippetCheck, ...]] = {
    EXPECTATIONS_DOC: (
        SnippetCheck("M274-D002-EXP-01", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M274-D002-EXP-02", HEADER_ARTIFACT),
        SnippetCheck("M274-D002-EXP-03", "objc3_runtime_copy_part11_bridge_generation_snapshot_for_testing"),
    ),
    PACKET_DOC: (
        SnippetCheck("M274-D002-PKT-01", "# Packet: `M274-D002`"),
        SnippetCheck("M274-D002-PKT-02", ISSUE_NUMBER),
        SnippetCheck("M274-D002-PKT-03", BRIDGE_ARTIFACT),
    ),
    DOC_SOURCE: (
        SnippetCheck("M274-D002-DOCSRC-01", "## M274 header, module, and bridge generation implementation"),
        SnippetCheck("M274-D002-DOCSRC-02", HEADER_ARTIFACT),
    ),
    DOC_NATIVE: (
        SnippetCheck("M274-D002-DOC-01", "## M274 header, module, and bridge generation implementation"),
        SnippetCheck("M274-D002-DOC-02", BRIDGE_ARTIFACT),
    ),
    SPEC_ATTR: (
        SnippetCheck("M274-D002-ATTR-01", "## M274 header, module, and bridge generation implementation (D002)"),
        SnippetCheck("M274-D002-ATTR-02", SURFACE_KEY),
    ),
    SPEC_TABLES: (
        SnippetCheck("M274-D002-TBL-01", CONTRACT_ID),
        SnippetCheck("M274-D002-TBL-02", HEADER_ARTIFACT),
    ),
    ARCH_DOC: (
        SnippetCheck("M274-D002-ARCH-01", "## M274 header, module, and bridge generation implementation (D002)"),
        SnippetCheck("M274-D002-ARCH-02", "objc3_runtime_copy_part11_bridge_generation_snapshot_for_testing"),
    ),
    RUNTIME_README: (
        SnippetCheck("M274-D002-RTR-01", "## M274 header/module/bridge generation probe"),
        SnippetCheck("M274-D002-RTR-02", "m274_d002_header_module_bridge_generation_probe.cpp"),
    ),
    FRONTEND_TYPES_H: (
        SnippetCheck("M274-D002-TYP-01", "kObjc3Part11HeaderModuleBridgeGenerationContractId"),
        SnippetCheck("M274-D002-TYP-02", "Objc3Part11HeaderModuleBridgeGenerationSummary"),
    ),
    FRONTEND_ARTIFACTS_H: (
        SnippetCheck("M274-D002-FAH-01", "part11_bridge_header_artifact_text"),
        SnippetCheck("M274-D002-FAH-02", "part11_bridge_artifact_json"),
    ),
    FRONTEND_ARTIFACTS_CPP: (
        SnippetCheck("M274-D002-FAC-01", "BuildPart11HeaderModuleBridgeGenerationSummary("),
        SnippetCheck("M274-D002-FAC-02", "BuildPart11BridgeHeaderArtifactText("),
        SnippetCheck("M274-D002-FAC-03", SURFACE_KEY),
    ),
    MANIFEST_ARTIFACTS_H: (
        SnippetCheck("M274-D002-MAH-01", "BuildPart11BridgeHeaderArtifactPath"),
        SnippetCheck("M274-D002-MAH-02", "WritePart11BridgeArtifact"),
    ),
    MANIFEST_ARTIFACTS_CPP: (
        SnippetCheck("M274-D002-MAC-01", "BuildPart11BridgeModuleArtifactPath"),
        SnippetCheck("M274-D002-MAC-02", "WritePart11BridgeHeaderArtifact"),
    ),
    IMPORT_SURFACE_H: (
        SnippetCheck("M274-D002-IMPH-01", "part11_header_module_bridge_generation_present"),
        SnippetCheck("M274-D002-IMPH-02", "part11_bridge_header_artifact_relative_path"),
    ),
    IMPORT_SURFACE_CPP: (
        SnippetCheck("M274-D002-IMPC-01", "PopulateImportedPart11HeaderModuleBridgeGeneration("),
        SnippetCheck("M274-D002-IMPC-02", "unexpected Part 11 header/module/bridge generation contract id in import surface"),
    ),
    PROCESS_H: (
        SnippetCheck("M274-D002-PH-01", "expected_part11_header_module_bridge_contract_id"),
        SnippetCheck("M274-D002-PH-02", "part11_header_module_bridge_generation_present"),
    ),
    PROCESS_CPP: (
        SnippetCheck("M274-D002-PC-01", "part11_header_module_bridge_cross_module_packaging_ready"),
        SnippetCheck("M274-D002-PC-02", "cross-module runtime link-plan Part 11 bridge-generation surface incomplete for"),
    ),
    DRIVER_CPP: (
        SnippetCheck("M274-D002-DRV-01", "expected_part11_header_module_bridge_contract_id"),
        SnippetCheck("M274-D002-DRV-02", "WritePart11BridgeHeaderArtifact("),
    ),
    FRONTEND_ANCHOR_CPP: (
        SnippetCheck("M274-D002-FEA-01", "BuildPart11BridgeHeaderArtifactPath"),
        SnippetCheck("M274-D002-FEA-02", "part11_bridge_module_artifact_text"),
    ),
    LOWER_H: (
        SnippetCheck("M274-D002-LH-01", "kObjc3Part11HeaderModuleBridgeGenerationLoweringContractId"),
        SnippetCheck("M274-D002-LH-02", "Objc3Part11HeaderModuleBridgeGenerationBoundarySummary"),
    ),
    LOWER_CPP: (
        SnippetCheck("M274-D002-LCPP-01", "Objc3Part11HeaderModuleBridgeGenerationBoundarySummary"),
        SnippetCheck("M274-D002-LCPP-02", "header_generation_ready=true"),
    ),
    IR_H: (
        SnippetCheck("M274-D002-IRH-01", "lowering_part11_header_module_bridge_generation_key"),
    ),
    IR_CPP: (
        SnippetCheck("M274-D002-IR-01", "; part11_header_module_and_bridge_generation = "),
        SnippetCheck("M274-D002-IR-02", "!objc3.objc_part11_header_module_and_bridge_generation = !{!112}"),
    ),
    RUNTIME_INTERNAL: (
        SnippetCheck("M274-D002-RIH-01", "objc3_runtime_part11_bridge_generation_snapshot"),
        SnippetCheck("M274-D002-RIH-02", "objc3_runtime_copy_part11_bridge_generation_snapshot_for_testing"),
    ),
    RUNTIME_CPP: (
        SnippetCheck("M274-D002-RCPP-01", "objc3_runtime_copy_part11_bridge_generation_snapshot_for_testing"),
        SnippetCheck("M274-D002-RCPP-02", "module.part11-bridge.json"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M274-D002-PKG-01", '"check:objc3c:m274-d002-header-module-and-bridge-generation-implementation-core-feature-implementation"'),
        SnippetCheck("M274-D002-PKG-02", '"test:tooling:m274-d002-header-module-and-bridge-generation-implementation-core-feature-implementation"'),
        SnippetCheck("M274-D002-PKG-03", '"check:objc3c:m274-d002-lane-d-readiness"'),
    ),
    PROVIDER_FIXTURE: (
        SnippetCheck("M274-D002-FIX-01", "module m274_d002_header_module_bridge_provider;"),
        SnippetCheck("M274-D002-FIX-02", "objc_foreign"),
        SnippetCheck("M274-D002-FIX-03", "objc_swift_name"),
    ),
    CONSUMER_FIXTURE: (
        SnippetCheck("M274-D002-FIX-04", "module m274_d002_header_module_bridge_consumer;"),
        SnippetCheck("M274-D002-FIX-05", "BridgeConsumerKit"),
    ),
    RUNTIME_PROBE: (
        SnippetCheck("M274-D002-PROBE-01", "objc3_runtime_copy_part11_bridge_generation_snapshot_for_testing"),
        SnippetCheck("M274-D002-PROBE-02", "header_artifact_relative_path"),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M274-D002-RUN-01", "M274-D001 + M274-D002"),
        SnippetCheck("M274-D002-RUN-02", "build_objc3c_native_docs.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M274-D002-TEST-01", "def test_checker_passes_static() -> None:"),
        SnippetCheck("M274-D002-TEST-02", "def test_checker_passes_dynamic() -> None:"),
    ),
}


def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def canonical_json(payload: Any) -> str:
    return json.dumps(payload, indent=2, sort_keys=True) + "\n"


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def run_command(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(list(command), cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def resolve_clangxx() -> str:
    candidates = (shutil.which("clang++"), shutil.which("clang++.exe"), r"C:\Program Files\LLVM\bin\clang++.exe")
    for candidate in candidates:
        if candidate and Path(candidate).exists():
            return str(candidate)
    return "clang++"


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def semantic_surface(manifest: dict[str, Any]) -> dict[str, Any]:
    frontend = manifest.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    surface = pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    if not isinstance(surface, dict):
        raise TypeError("manifest missing frontend.pipeline.semantic_surface")
    return surface


def parse_probe_output(text: str) -> dict[str, str]:
    parsed: dict[str, str] = {}
    for raw_line in text.splitlines():
        line = raw_line.strip()
        if not line or "=" not in line:
            continue
        key, value = line.split("=", 1)
        parsed[key.strip()] = value.strip()
    return parsed


def compile_fixture(*, fixture: Path, out_dir: Path, registration_order_ordinal: int, import_surface: Path | None = None) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [
        str(NATIVE_EXE),
        str(fixture),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--objc3-bootstrap-registration-order-ordinal",
        str(registration_order_ordinal),
    ]
    if import_surface is not None:
        command.extend(["--objc3-import-runtime-surface", str(import_surface)])
    return run_command(command)


def build_summary(skip_dynamic_probes: bool) -> tuple[dict[str, object], list[Finding]]:
    failures: list[Finding] = []
    checks_total = 0
    static_summary: dict[str, object] = {}
    for path, snippets in STATIC_SNIPPETS.items():
        checks_total += len(snippets)
        passed = ensure_snippets(path, snippets, failures)
        static_summary[display_path(path)] = {"checks": len(snippets), "passed": passed}

    dynamic_summary: dict[str, object] = {"skipped": skip_dynamic_probes}
    if not skip_dynamic_probes:
        ensure_summary = ROOT / "tmp" / "reports" / "m274" / ISSUE / "ensure_build_summary.json"
        build = run_command([
            sys.executable,
            str(BUILD_HELPER),
            "--mode",
            "fast",
            "--reason",
            "m274-d002-validation",
            "--summary-out",
            str(ensure_summary),
        ])
        checks_total += 1
        require(build.returncode == 0, display_path(BUILD_HELPER), "M274-D002-BUILD-01", build.stderr or build.stdout or "fast build failed", failures)

        provider_out = PROBE_ROOT / "provider"
        consumer_out = PROBE_ROOT / "consumer"
        provider_compile = compile_fixture(fixture=PROVIDER_FIXTURE, out_dir=provider_out, registration_order_ordinal=1)
        checks_total += 1
        require(provider_compile.returncode == 0, display_path(PROVIDER_FIXTURE), "M274-D002-DYN-01", provider_compile.stderr or provider_compile.stdout or "provider compile failed", failures)

        provider_manifest_path = provider_out / MANIFEST_ARTIFACT
        provider_import_path = provider_out / IMPORT_ARTIFACT
        provider_ir_path = provider_out / "module.ll"
        provider_header_path = provider_out / HEADER_ARTIFACT
        provider_modulemap_path = provider_out / MODULE_ARTIFACT
        provider_bridge_path = provider_out / BRIDGE_ARTIFACT
        for check_id, path in (
            ("M274-D002-DYN-02", provider_manifest_path),
            ("M274-D002-DYN-03", provider_import_path),
            ("M274-D002-DYN-04", provider_ir_path),
            ("M274-D002-DYN-05", provider_header_path),
            ("M274-D002-DYN-06", provider_modulemap_path),
            ("M274-D002-DYN-07", provider_bridge_path),
        ):
            checks_total += 1
            require(path.exists(), display_path(path), check_id, f"missing artifact: {path.name}", failures)

        provider_surface: dict[str, Any] = {}
        provider_import_surface: dict[str, Any] = {}
        if not failures:
            provider_manifest = load_json(provider_manifest_path)
            provider_surface = semantic_surface(provider_manifest).get(SURFACE_KEY, {})
            provider_import_payload = load_json(provider_import_path)
            provider_import_surface = provider_import_payload.get(SURFACE_KEY, {})
            provider_bridge_payload = load_json(provider_bridge_path)
            provider_header_text = read_text(provider_header_path)
            provider_modulemap_text = read_text(provider_modulemap_path)
            provider_ir_text = read_text(provider_ir_path)
            checks_total += 1
            require(isinstance(provider_surface, dict), display_path(provider_manifest_path), "M274-D002-DYN-08", "provider semantic Part 11 D002 surface missing", failures)
            checks_total += 1
            require(isinstance(provider_import_surface, dict), display_path(provider_import_path), "M274-D002-DYN-09", "provider import Part 11 D002 surface missing", failures)
            checks_total += 1
            require(provider_surface.get("contract_id") == CONTRACT_ID, display_path(provider_manifest_path), "M274-D002-DYN-10", "provider manifest contract mismatch", failures)
            checks_total += 1
            require(provider_import_surface.get("contract_id") == CONTRACT_ID, display_path(provider_import_path), "M274-D002-DYN-11", "provider import contract mismatch", failures)
            checks_total += 1
            require(provider_import_surface.get("source_contract_id") == SOURCE_CONTRACT_ID, display_path(provider_import_path), "M274-D002-DYN-12", "provider import source contract mismatch", failures)
            checks_total += 1
            require(provider_import_surface.get("preservation_contract_id") == PRESERVATION_CONTRACT_ID, display_path(provider_import_path), "M274-D002-DYN-13", "provider import preservation contract mismatch", failures)
            for offset, key in enumerate(("runtime_generation_ready", "cross_module_packaging_ready", "deterministic"), start=14):
                checks_total += 1
                require(provider_import_surface.get(key) is True, display_path(provider_import_path), f"M274-D002-DYN-{offset}", f"provider import {key} not true", failures)
            checks_total += 1
            require(provider_import_surface.get("header_artifact_relative_path") == HEADER_ARTIFACT, display_path(provider_import_path), "M274-D002-DYN-17", "provider header path mismatch", failures)
            checks_total += 1
            require(provider_import_surface.get("module_artifact_relative_path") == MODULE_ARTIFACT, display_path(provider_import_path), "M274-D002-DYN-18", "provider module path mismatch", failures)
            checks_total += 1
            require(provider_import_surface.get("bridge_artifact_relative_path") == BRIDGE_ARTIFACT, display_path(provider_import_path), "M274-D002-DYN-19", "provider bridge path mismatch", failures)
            checks_total += 1
            require("ffiInbound" in provider_header_text and "ffiHeaderBridge" in provider_header_text and "#pragma once" in provider_header_text, display_path(provider_header_path), "M274-D002-DYN-20", "provider header missing expected declarations", failures)
            checks_total += 1
            require("module m274_d002_header_module_bridge_provider_objc3_part11_bridge" in provider_modulemap_text, display_path(provider_modulemap_path), "M274-D002-DYN-21", "provider modulemap missing generated module name", failures)
            checks_total += 1
            require(provider_bridge_payload.get("contract_id") == CONTRACT_ID, display_path(provider_bridge_path), "M274-D002-DYN-22", "provider bridge payload contract mismatch", failures)
            checks_total += 1
            require(len(provider_bridge_payload.get("foreign_callables", [])) == 2, display_path(provider_bridge_path), "M274-D002-DYN-23", "provider bridge payload foreign callable count mismatch", failures)
            checks_total += 1
            require("; part11_header_module_and_bridge_generation = " in provider_ir_text, display_path(provider_ir_path), "M274-D002-DYN-24", "missing provider IR comment", failures)
            checks_total += 1
            require("!objc3.objc_part11_header_module_and_bridge_generation = !{!112}" in provider_ir_text, display_path(provider_ir_path), "M274-D002-DYN-25", "missing provider IR metadata", failures)

        consumer_compile = compile_fixture(
            fixture=CONSUMER_FIXTURE,
            out_dir=consumer_out,
            registration_order_ordinal=2,
            import_surface=provider_import_path,
        )
        checks_total += 1
        require(consumer_compile.returncode == 0, display_path(CONSUMER_FIXTURE), "M274-D002-DYN-26", consumer_compile.stderr or consumer_compile.stdout or "consumer compile failed", failures)

        consumer_plan_path = consumer_out / LINK_PLAN_ARTIFACT
        consumer_import_path = consumer_out / IMPORT_ARTIFACT
        consumer_ir_path = consumer_out / "module.ll"
        consumer_header_path = consumer_out / HEADER_ARTIFACT
        consumer_modulemap_path = consumer_out / MODULE_ARTIFACT
        consumer_bridge_path = consumer_out / BRIDGE_ARTIFACT
        for check_id, path in (
            ("M274-D002-DYN-27", consumer_plan_path),
            ("M274-D002-DYN-28", consumer_import_path),
            ("M274-D002-DYN-29", consumer_ir_path),
            ("M274-D002-DYN-30", consumer_header_path),
            ("M274-D002-DYN-31", consumer_modulemap_path),
            ("M274-D002-DYN-32", consumer_bridge_path),
        ):
            checks_total += 1
            require(path.exists(), display_path(path), check_id, f"missing artifact: {path.name}", failures)

        consumer_link_plan: dict[str, Any] = {}
        consumer_import_surface: dict[str, Any] = {}
        if not failures:
            consumer_link_plan = load_json(consumer_plan_path)
            consumer_import_payload = load_json(consumer_import_path)
            consumer_import_surface = consumer_import_payload.get(SURFACE_KEY, {})
            consumer_ir_text = read_text(consumer_ir_path)
            imported_modules = consumer_link_plan.get("imported_modules")
            checks_total += 1
            require(consumer_link_plan.get("contract_id") == LINK_PLAN_CONTRACT_ID, display_path(consumer_plan_path), "M274-D002-DYN-33", "consumer link-plan contract mismatch", failures)
            checks_total += 1
            require(consumer_link_plan.get("expected_part11_header_module_bridge_contract_id") == CONTRACT_ID, display_path(consumer_plan_path), "M274-D002-DYN-34", "consumer expected D002 contract mismatch", failures)
            checks_total += 1
            require(consumer_link_plan.get("expected_part11_header_module_bridge_source_contract_id") == SOURCE_CONTRACT_ID, display_path(consumer_plan_path), "M274-D002-DYN-35", "consumer expected D002 source contract mismatch", failures)
            checks_total += 1
            require(consumer_link_plan.get("expected_part11_header_module_bridge_preservation_contract_id") == PRESERVATION_CONTRACT_ID, display_path(consumer_plan_path), "M274-D002-DYN-36", "consumer expected D002 preservation contract mismatch", failures)
            checks_total += 1
            require(consumer_link_plan.get("expected_part11_bridge_header_artifact_relative_path") == HEADER_ARTIFACT, display_path(consumer_plan_path), "M274-D002-DYN-37", "consumer expected header artifact mismatch", failures)
            checks_total += 1
            require(consumer_link_plan.get("expected_part11_bridge_module_artifact_relative_path") == MODULE_ARTIFACT, display_path(consumer_plan_path), "M274-D002-DYN-38", "consumer expected module artifact mismatch", failures)
            checks_total += 1
            require(consumer_link_plan.get("expected_part11_bridge_artifact_relative_path") == BRIDGE_ARTIFACT, display_path(consumer_plan_path), "M274-D002-DYN-39", "consumer expected bridge artifact mismatch", failures)
            checks_total += 1
            require(consumer_link_plan.get("part11_header_module_bridge_imported_module_count") == 1, display_path(consumer_plan_path), "M274-D002-DYN-40", "consumer imported D002 module count mismatch", failures)
            checks_total += 1
            require(consumer_link_plan.get("part11_header_module_bridge_cross_module_packaging_ready") is True, display_path(consumer_plan_path), "M274-D002-DYN-41", "consumer D002 cross-module packaging should be ready", failures)
            checks_total += 1
            require(isinstance(consumer_import_surface, dict) and consumer_import_surface.get("contract_id") == CONTRACT_ID, display_path(consumer_import_path), "M274-D002-DYN-42", "consumer import D002 surface missing", failures)
            checks_total += 1
            require("; part11_header_module_and_bridge_generation = " in consumer_ir_text, display_path(consumer_ir_path), "M274-D002-DYN-43", "missing consumer IR comment", failures)
            checks_total += 1
            require("!objc3.objc_part11_header_module_and_bridge_generation = !{!112}" in consumer_ir_text, display_path(consumer_ir_path), "M274-D002-DYN-44", "missing consumer IR metadata", failures)
            checks_total += 1
            require(isinstance(imported_modules, list) and len(imported_modules) == 1, display_path(consumer_plan_path), "M274-D002-DYN-45", "unexpected imported module payload", failures)
            if isinstance(imported_modules, list) and imported_modules:
                imported = imported_modules[0]
                checks_total += 1
                require(imported.get("part11_header_module_bridge_generation_present") is True, display_path(consumer_plan_path), "M274-D002-DYN-46", "imported D002 presence missing", failures)
                checks_total += 1
                require(imported.get("part11_header_module_bridge_contract_id") == CONTRACT_ID, display_path(consumer_plan_path), "M274-D002-DYN-47", "imported D002 contract mismatch", failures)
                checks_total += 1
                require(imported.get("part11_header_module_bridge_source_contract_id") == SOURCE_CONTRACT_ID, display_path(consumer_plan_path), "M274-D002-DYN-48", "imported D002 source contract mismatch", failures)
                checks_total += 1
                require(imported.get("part11_header_module_bridge_preservation_contract_id") == PRESERVATION_CONTRACT_ID, display_path(consumer_plan_path), "M274-D002-DYN-49", "imported D002 preservation contract mismatch", failures)
                checks_total += 1
                require(imported.get("part11_header_module_bridge_runtime_generation_ready") is True, display_path(consumer_plan_path), "M274-D002-DYN-50", "imported D002 runtime generation should be ready", failures)
                checks_total += 1
                require(imported.get("part11_header_module_bridge_cross_module_packaging_ready") is True, display_path(consumer_plan_path), "M274-D002-DYN-51", "imported D002 cross-module packaging should be ready", failures)
                checks_total += 1
                require(imported.get("part11_header_module_bridge_deterministic") is True, display_path(consumer_plan_path), "M274-D002-DYN-52", "imported D002 deterministic flag missing", failures)
                checks_total += 1
                require(imported.get("part11_header_module_bridge_replay_key") == provider_import_surface.get("replay_key"), display_path(consumer_plan_path), "M274-D002-DYN-53", "imported D002 replay key mismatch", failures)
                checks_total += 1
                require(imported.get("part11_header_module_bridge_preservation_replay_key") == provider_import_surface.get("preservation_replay_key"), display_path(consumer_plan_path), "M274-D002-DYN-54", "imported D002 preservation replay key mismatch", failures)
                checks_total += 1
                require(imported.get("part11_bridge_header_artifact_relative_path") == HEADER_ARTIFACT, display_path(consumer_plan_path), "M274-D002-DYN-55", "imported D002 header artifact path mismatch", failures)
                checks_total += 1
                require(imported.get("part11_bridge_module_artifact_relative_path") == MODULE_ARTIFACT, display_path(consumer_plan_path), "M274-D002-DYN-56", "imported D002 module artifact path mismatch", failures)
                checks_total += 1
                require(imported.get("part11_bridge_artifact_relative_path") == BRIDGE_ARTIFACT, display_path(consumer_plan_path), "M274-D002-DYN-57", "imported D002 bridge artifact path mismatch", failures)
                checks_total += 1
                require(imported.get("part11_header_module_bridge_local_foreign_callable_count") == provider_import_surface.get("local_foreign_callable_count"), display_path(consumer_plan_path), "M274-D002-DYN-58", "imported D002 local foreign callable count mismatch", failures)

        probe_dir = PROBE_ROOT / "runtime_probe"
        probe_dir.mkdir(parents=True, exist_ok=True)
        probe_exe = probe_dir / "m274_d002_header_module_bridge_generation_probe.exe"
        clangxx = resolve_clangxx()
        probe_compile = run_command([
            clangxx,
            "-std=c++20",
            "-D_DLL",
            "-D_MT",
            "-Xclang",
            "--dependent-lib=msvcrt",
            "-I",
            str(ROOT / "native" / "objc3c" / "src"),
            str(RUNTIME_PROBE),
            str(RUNTIME_LIB),
            "-o",
            str(probe_exe),
        ])
        checks_total += 1
        require(probe_compile.returncode == 0, display_path(RUNTIME_PROBE), "M274-D002-DYN-59", probe_compile.stderr or probe_compile.stdout or "runtime probe compile failed", failures)

        runtime_probe_output: dict[str, str] = {}
        if probe_compile.returncode == 0:
            probe_run = run_command([str(probe_exe)])
            checks_total += 1
            require(probe_run.returncode == 0, display_path(probe_exe), "M274-D002-DYN-60", probe_run.stderr or probe_run.stdout or "runtime probe failed", failures)
            runtime_probe_output = parse_probe_output(probe_run.stdout)
            for offset, (key, value) in enumerate((
                ("copy_status", "0"),
                ("runtime_generation_ready", "1"),
                ("cross_module_packaging_ready", "1"),
                ("header_generation_ready", "1"),
                ("module_generation_ready", "1"),
                ("bridge_generation_ready", "1"),
                ("deterministic", "1"),
                ("header_artifact_relative_path", HEADER_ARTIFACT),
                ("module_artifact_relative_path", MODULE_ARTIFACT),
                ("bridge_artifact_relative_path", BRIDGE_ARTIFACT),
            ), start=61):
                checks_total += 1
                require(runtime_probe_output.get(key) == value, display_path(probe_exe), f"M274-D002-DYN-{offset}", f"unexpected {key}: {runtime_probe_output}", failures)

        dynamic_summary = {
            "provider_import_surface": provider_import_surface,
            "consumer_link_plan": consumer_link_plan,
            "runtime_probe": runtime_probe_output,
        }

    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
        "failures": [failure.__dict__ for failure in failures],
        "static_contract": static_summary,
        "dynamic_probes": dynamic_summary,
    }
    return payload, failures


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    payload, failures = build_summary(skip_dynamic_probes=args.skip_dynamic_probes)
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(payload), encoding="utf-8")
    if failures:
        for failure in failures:
            print(f"[fail] {failure.check_id} :: {failure.artifact} :: {failure.detail}", file=sys.stderr)
        print(f"[info] wrote summary to {display_path(args.summary_out)}", file=sys.stderr)
        return 1
    print(f"[ok] {ISSUE} passed {payload['checks_passed']} checks")
    print(f"[info] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
