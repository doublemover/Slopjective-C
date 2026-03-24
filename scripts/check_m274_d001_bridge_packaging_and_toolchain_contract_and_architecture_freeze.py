#!/usr/bin/env python3
"""Checker for M274-D001 bridge packaging and toolchain contract freeze."""

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
MODE = "m274-d001-part11-bridge-packaging-toolchain-contract-v1"
ISSUE = "M274-D001"
ISSUE_NUMBER = "#7370"
CONTRACT_ID = "objc3c-part11-bridge-packaging-and-toolchain-contract/m274-d001-v1"
SOURCE_CONTRACT_ID = "objc3c-part11-ffi-metadata-interface-preservation/m274-c003-v1"
SOURCE_SOURCE_CONTRACT_ID = "objc3c-part11-foreign-call-and-lifetime-lowering/m274-c002-v1"
PRESERVATION_CONTRACT_ID = "objc3c-part11-foreign-surface-interface-preservation/m274-a003-v1"
LINK_PLAN_CONTRACT_ID = "objc3c-cross-module-runtime-packaging-link-plan/m258-d002-v1"
RUNTIME_ARCHIVE = "artifacts/lib/objc3_runtime.lib"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m274" / "M274-D001" / "bridge_packaging_toolchain_contract_summary.json"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m274" / "d001"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m274_bridge_packaging_and_toolchain_contract_and_architecture_freeze_d001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m274" / "m274_d001_bridge_packaging_and_toolchain_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_TABLES = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCH_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
LOWER_H = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWER_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
PROCESS_H = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.h"
PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
RUNTIME_INTERNAL = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PACKAGE_JSON = ROOT / "package.json"
CHECKER_FILE = ROOT / "scripts" / "check_m274_d001_bridge_packaging_and_toolchain_contract_and_architecture_freeze.py"
READINESS_RUNNER = ROOT / "scripts" / "run_m274_d001_lane_d_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m274_d001_bridge_packaging_and_toolchain_contract_and_architecture_freeze.py"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_LIB = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
PROVIDER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m274_d001_bridge_packaging_toolchain_provider.objc3"
CONSUMER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m274_d001_bridge_packaging_toolchain_consumer.objc3"
RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m274_d001_bridge_packaging_toolchain_probe.cpp"
IMPORT_ARTIFACT = "module.runtime-import-surface.json"
LINK_PLAN_ARTIFACT = "module.cross-module-runtime-link-plan.json"
MANIFEST_ARTIFACT = "module.manifest.json"
REGISTRATION_MANIFEST_ARTIFACT = "module.runtime-registration-manifest.json"


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
        SnippetCheck("M274-D001-EXP-01", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M274-D001-EXP-02", "objc3_runtime_copy_part11_bridge_packaging_toolchain_snapshot_for_testing"),
        SnippetCheck("M274-D001-EXP-03", "`M274-D002`"),
    ),
    PACKET_DOC: (
        SnippetCheck("M274-D001-PKT-01", "# Packet: `M274-D001`"),
        SnippetCheck("M274-D001-PKT-02", ISSUE_NUMBER),
        SnippetCheck("M274-D001-PKT-03", "module.cross-module-runtime-link-plan.json"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M274-D001-DOCSRC-01", "## M274 bridge packaging and toolchain contract"),
        SnippetCheck("M274-D001-DOCSRC-02", "objc3_runtime_copy_part11_bridge_packaging_toolchain_snapshot_for_testing"),
    ),
    DOC_NATIVE: (
        SnippetCheck("M274-D001-DOC-01", "## M274 bridge packaging and toolchain contract"),
        SnippetCheck("M274-D001-DOC-02", RUNTIME_ARCHIVE),
    ),
    SPEC_ATTR: (
        SnippetCheck("M274-D001-ATTR-01", "## M274 bridge packaging and toolchain contract (D001)"),
        SnippetCheck("M274-D001-ATTR-02", "expected_part11_ffi_contract_id"),
    ),
    SPEC_TABLES: (
        SnippetCheck("M274-D001-TBL-01", CONTRACT_ID),
        SnippetCheck("M274-D001-TBL-02", "module.cross-module-runtime-link-plan.json"),
    ),
    ARCH_DOC: (
        SnippetCheck("M274-D001-ARCH-01", "## M274 bridge packaging and toolchain contract (D001)"),
        SnippetCheck("M274-D001-ARCH-02", "TryBuildObjc3CrossModuleRuntimeLinkPlanArtifact(...)"),
    ),
    RUNTIME_README: (
        SnippetCheck("M274-D001-RTR-01", "## M274 bridge packaging/toolchain contract probe"),
        SnippetCheck("M274-D001-RTR-02", "m274_d001_bridge_packaging_toolchain_probe.cpp"),
    ),
    LOWER_H: (
        SnippetCheck("M274-D001-LH-01", "kObjc3Part11BridgePackagingToolchainContractId"),
        SnippetCheck("M274-D001-LH-02", "kObjc3Part11BridgePackagingToolchainPackagingModel"),
    ),
    LOWER_CPP: (
        SnippetCheck("M274-D001-LCPP-01", "Objc3Part11BridgePackagingToolchainSummary"),
        SnippetCheck("M274-D001-LCPP-02", "next_issue=M274-D002"),
    ),
    PROCESS_H: (
        SnippetCheck("M274-D001-PH-01", "expected_part11_ffi_contract_id"),
        SnippetCheck("M274-D001-PH-02", "part11_ffi_metadata_interface_preservation_present"),
    ),
    PROCESS_CPP: (
        SnippetCheck("M274-D001-PC-01", "part11_ffi_cross_module_packaging_ready"),
        SnippetCheck("M274-D001-PC-02", "cross-module runtime link-plan Part 11 ffi preservation surface incomplete for"),
    ),
    DRIVER_CPP: (
        SnippetCheck("M274-D001-DRV-01", "expected_part11_ffi_contract_id"),
        SnippetCheck("M274-D001-DRV-02", "imported_input.part11_ffi_metadata_interface_preservation_present ="),
    ),
    RUNTIME_INTERNAL: (
        SnippetCheck("M274-D001-RIH-01", "objc3_runtime_part11_bridge_packaging_toolchain_snapshot"),
        SnippetCheck("M274-D001-RIH-02", "objc3_runtime_copy_part11_bridge_packaging_toolchain_snapshot_for_testing"),
    ),
    RUNTIME_CPP: (
        SnippetCheck("M274-D001-RCPP-01", "M274-D001 bridge-packaging/toolchain anchor:"),
        SnippetCheck("M274-D001-RCPP-02", "header-module-and-bridge-generation-remain-unclaimed-until-m274-d002"),
    ),
    IR_CPP: (
        SnippetCheck("M274-D001-IR-01", "; part11_bridge_packaging_toolchain_contract = "),
        SnippetCheck("M274-D001-IR-02", "!objc3.objc_part11_bridge_packaging_and_toolchain_contract = !{!111}"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M274-D001-PKG-01", '"check:objc3c:m274-d001-bridge-packaging-and-toolchain-contract-and-architecture-freeze"'),
        SnippetCheck("M274-D001-PKG-02", '"test:tooling:m274-d001-bridge-packaging-and-toolchain-contract-and-architecture-freeze"'),
        SnippetCheck("M274-D001-PKG-03", '"check:objc3c:m274-d001-lane-d-readiness"'),
    ),
    PROVIDER_FIXTURE: (
        SnippetCheck("M274-D001-FIX-01", "module m274_d001_bridge_packaging_toolchain_provider;"),
        SnippetCheck("M274-D001-FIX-02", "objc_foreign"),
        SnippetCheck("M274-D001-FIX-03", "objc_header_name"),
    ),
    CONSUMER_FIXTURE: (
        SnippetCheck("M274-D001-FIX-04", "module m274_d001_bridge_packaging_toolchain_consumer;"),
        SnippetCheck("M274-D001-FIX-05", "objc_import_module(named(\"BridgeConsumerKit\"))"),
    ),
    RUNTIME_PROBE: (
        SnippetCheck("M274-D001-PROBE-01", "objc3_runtime_copy_part11_bridge_packaging_toolchain_snapshot_for_testing"),
        SnippetCheck("M274-D001-PROBE-02", "packaging_topology_ready"),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M274-D001-RUN-01", "M274-C002 + M274-C003 + M274-D001"),
        SnippetCheck("M274-D001-RUN-02", "build_objc3c_native_docs.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M274-D001-TEST-01", "def test_checker_passes_static() -> None:"),
        SnippetCheck("M274-D001-TEST-02", "def test_checker_passes_dynamic() -> None:"),
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
        ensure_summary = ROOT / "tmp" / "reports" / "m274" / "M274-D001" / "ensure_build_summary.json"
        build = run_command([
            sys.executable,
            str(BUILD_HELPER),
            "--mode",
            "fast",
            "--reason",
            "m274-d001-validation",
            "--summary-out",
            str(ensure_summary),
        ])
        checks_total += 1
        require(build.returncode == 0, display_path(BUILD_HELPER), "M274-D001-BUILD-01", build.stderr or build.stdout or "fast build failed", failures)

        provider_out = PROBE_ROOT / "provider"
        consumer_out = PROBE_ROOT / "consumer"
        provider_compile = compile_fixture(fixture=PROVIDER_FIXTURE, out_dir=provider_out, registration_order_ordinal=1)
        checks_total += 1
        require(provider_compile.returncode == 0, display_path(PROVIDER_FIXTURE), "M274-D001-DYN-01", provider_compile.stderr or provider_compile.stdout or "provider compile failed", failures)

        provider_manifest_path = provider_out / MANIFEST_ARTIFACT
        provider_registration_manifest_path = provider_out / REGISTRATION_MANIFEST_ARTIFACT
        provider_import_path = provider_out / IMPORT_ARTIFACT
        provider_ir_path = provider_out / "module.ll"
        for check_id, path in (
            ("M274-D001-DYN-02", provider_manifest_path),
            ("M274-D001-DYN-03", provider_registration_manifest_path),
            ("M274-D001-DYN-04", provider_import_path),
            ("M274-D001-DYN-05", provider_ir_path),
        ):
            checks_total += 1
            require(path.exists(), display_path(path), check_id, f"missing artifact: {path.name}", failures)

        provider_surface: dict[str, Any] = {}
        provider_import_surface: dict[str, Any] = {}
        if not failures:
            provider_manifest = load_json(provider_manifest_path)
            provider_surface = semantic_surface(provider_manifest).get("objc_part11_ffi_metadata_and_interface_preservation", {})
            provider_registration_manifest = load_json(provider_registration_manifest_path)
            provider_import_payload = load_json(provider_import_path)
            provider_import_surface = provider_import_payload.get("objc_part11_ffi_metadata_and_interface_preservation", {})
            checks_total += 1
            require(isinstance(provider_surface, dict), display_path(provider_manifest_path), "M274-D001-DYN-06", "provider semantic Part 11 surface missing", failures)
            checks_total += 1
            require(isinstance(provider_import_surface, dict), display_path(provider_import_path), "M274-D001-DYN-07", "provider runtime-import Part 11 surface missing", failures)
            checks_total += 1
            require(provider_registration_manifest.get("runtime_support_library_archive_relative_path") == RUNTIME_ARCHIVE, display_path(provider_registration_manifest_path), "M274-D001-DYN-08", "unexpected runtime library archive path", failures)
            provider_ir_text = read_text(provider_ir_path)
            checks_total += 1
            require("; part11_bridge_packaging_toolchain_contract = " in provider_ir_text, display_path(provider_ir_path), "M274-D001-DYN-09", "missing Part 11 packaging/toolchain IR comment", failures)
            checks_total += 1
            require("!objc3.objc_part11_bridge_packaging_and_toolchain_contract = !{!111}" in provider_ir_text, display_path(provider_ir_path), "M274-D001-DYN-10", "missing Part 11 packaging/toolchain IR metadata", failures)
            checks_total += 1
            require(provider_import_surface.get("contract_id") == SOURCE_CONTRACT_ID, display_path(provider_import_path), "M274-D001-DYN-11", "provider import surface contract mismatch", failures)
            checks_total += 1
            require(provider_import_surface.get("source_contract_id") == SOURCE_SOURCE_CONTRACT_ID, display_path(provider_import_path), "M274-D001-DYN-12", "provider import surface source contract mismatch", failures)
            checks_total += 1
            require(provider_import_surface.get("preservation_contract_id") == PRESERVATION_CONTRACT_ID, display_path(provider_import_path), "M274-D001-DYN-13", "provider import surface preservation contract mismatch", failures)

        consumer_compile = compile_fixture(
            fixture=CONSUMER_FIXTURE,
            out_dir=consumer_out,
            registration_order_ordinal=2,
            import_surface=provider_import_path,
        )
        checks_total += 1
        require(consumer_compile.returncode == 0, display_path(CONSUMER_FIXTURE), "M274-D001-DYN-14", consumer_compile.stderr or consumer_compile.stdout or "consumer compile failed", failures)

        consumer_plan_path = consumer_out / LINK_PLAN_ARTIFACT
        consumer_registration_manifest_path = consumer_out / REGISTRATION_MANIFEST_ARTIFACT
        consumer_import_path = consumer_out / IMPORT_ARTIFACT
        consumer_ir_path = consumer_out / "module.ll"
        for check_id, path in (
            ("M274-D001-DYN-15", consumer_plan_path),
            ("M274-D001-DYN-16", consumer_registration_manifest_path),
            ("M274-D001-DYN-17", consumer_import_path),
            ("M274-D001-DYN-18", consumer_ir_path),
        ):
            checks_total += 1
            require(path.exists(), display_path(path), check_id, f"missing artifact: {path.name}", failures)

        consumer_link_plan: dict[str, Any] = {}
        if not failures:
            consumer_link_plan = load_json(consumer_plan_path)
            consumer_registration_manifest = load_json(consumer_registration_manifest_path)
            consumer_import_payload = load_json(consumer_import_path)
            imported_modules = consumer_link_plan.get("imported_modules")
            checks_total += 1
            require(consumer_link_plan.get("contract_id") == LINK_PLAN_CONTRACT_ID, display_path(consumer_plan_path), "M274-D001-DYN-19", "link-plan contract mismatch", failures)
            checks_total += 1
            require(consumer_link_plan.get("expected_part11_ffi_contract_id") == SOURCE_CONTRACT_ID, display_path(consumer_plan_path), "M274-D001-DYN-20", "expected Part 11 ffi contract mismatch", failures)
            checks_total += 1
            require(consumer_link_plan.get("expected_part11_ffi_source_contract_id") == SOURCE_SOURCE_CONTRACT_ID, display_path(consumer_plan_path), "M274-D001-DYN-21", "expected Part 11 ffi source contract mismatch", failures)
            checks_total += 1
            require(consumer_link_plan.get("expected_part11_ffi_preservation_contract_id") == PRESERVATION_CONTRACT_ID, display_path(consumer_plan_path), "M274-D001-DYN-22", "expected Part 11 ffi preservation contract mismatch", failures)
            checks_total += 1
            require(consumer_link_plan.get("part11_ffi_imported_module_count") == 1, display_path(consumer_plan_path), "M274-D001-DYN-23", "expected one imported Part 11 ffi module", failures)
            checks_total += 1
            require(consumer_link_plan.get("part11_ffi_cross_module_packaging_ready") is True, display_path(consumer_plan_path), "M274-D001-DYN-24", "Part 11 cross-module packaging should be ready", failures)
            checks_total += 1
            require(consumer_link_plan.get("runtime_support_library_archive_relative_path") == RUNTIME_ARCHIVE, display_path(consumer_plan_path), "M274-D001-DYN-25", "link plan runtime archive path mismatch", failures)
            checks_total += 1
            require(consumer_registration_manifest.get("runtime_support_library_archive_relative_path") == RUNTIME_ARCHIVE, display_path(consumer_registration_manifest_path), "M274-D001-DYN-26", "consumer registration manifest runtime archive mismatch", failures)
            checks_total += 1
            require(isinstance(consumer_import_payload.get("objc_part11_ffi_metadata_and_interface_preservation"), dict), display_path(consumer_import_path), "M274-D001-DYN-27", "consumer import surface missing Part 11 payload", failures)
            checks_total += 1
            require(isinstance(imported_modules, list) and len(imported_modules) == 1, display_path(consumer_plan_path), "M274-D001-DYN-28", "unexpected imported module payload", failures)
            consumer_ir_text = read_text(consumer_ir_path)
            checks_total += 1
            require("; part11_bridge_packaging_toolchain_contract = " in consumer_ir_text, display_path(consumer_ir_path), "M274-D001-DYN-29", "consumer IR comment missing", failures)
            checks_total += 1
            require("!objc3.objc_part11_bridge_packaging_and_toolchain_contract = !{!111}" in consumer_ir_text, display_path(consumer_ir_path), "M274-D001-DYN-30", "consumer IR metadata missing", failures)
            checks_total += 1
            require("m274_d001_bridge_packaging_toolchain_provider" in (consumer_link_plan.get("part11_ffi_imported_module_names_lexicographic") or []), display_path(consumer_plan_path), "M274-D001-DYN-31", "provider module name missing from lexicographic Part 11 import list", failures)
            linker_response_artifact = consumer_link_plan.get("linker_response_artifact")
            checks_total += 1
            require(isinstance(linker_response_artifact, str) and linker_response_artifact != "", display_path(consumer_plan_path), "M274-D001-DYN-32", "linker response artifact path missing", failures)
            if isinstance(linker_response_artifact, str) and linker_response_artifact:
                linker_response_path = (consumer_out / linker_response_artifact).resolve()
                checks_total += 1
                require(linker_response_path.exists(), display_path(linker_response_path), "M274-D001-DYN-33", "linker response sidecar missing", failures)
            if isinstance(imported_modules, list) and imported_modules:
                imported = imported_modules[0]
                checks_total += 1
                require(imported.get("part11_ffi_metadata_interface_preservation_present") is True, display_path(consumer_plan_path), "M274-D001-DYN-34", "imported Part 11 presence missing", failures)
                checks_total += 1
                require(imported.get("part11_ffi_contract_id") == SOURCE_CONTRACT_ID, display_path(consumer_plan_path), "M274-D001-DYN-35", "imported Part 11 contract mismatch", failures)
                checks_total += 1
                require(imported.get("part11_ffi_source_contract_id") == SOURCE_SOURCE_CONTRACT_ID, display_path(consumer_plan_path), "M274-D001-DYN-36", "imported Part 11 source contract mismatch", failures)
                checks_total += 1
                require(imported.get("part11_ffi_preservation_contract_id") == PRESERVATION_CONTRACT_ID, display_path(consumer_plan_path), "M274-D001-DYN-37", "imported Part 11 preservation contract mismatch", failures)
                checks_total += 1
                require(imported.get("part11_ffi_runtime_import_artifact_ready") is True, display_path(consumer_plan_path), "M274-D001-DYN-38", "imported Part 11 runtime-import readiness missing", failures)
                checks_total += 1
                require(imported.get("part11_ffi_separate_compilation_preservation_ready") is True, display_path(consumer_plan_path), "M274-D001-DYN-39", "imported Part 11 separate-compilation readiness missing", failures)
                checks_total += 1
                require(imported.get("part11_ffi_deterministic") is True, display_path(consumer_plan_path), "M274-D001-DYN-40", "imported Part 11 deterministic flag missing", failures)
                checks_total += 1
                require(imported.get("part11_ffi_replay_key") == provider_import_surface.get("replay_key"), display_path(consumer_plan_path), "M274-D001-DYN-41", "imported Part 11 replay key mismatch", failures)
                checks_total += 1
                require(imported.get("part11_ffi_lowering_replay_key") == provider_import_surface.get("lowering_replay_key"), display_path(consumer_plan_path), "M274-D001-DYN-42", "imported Part 11 lowering replay key mismatch", failures)
                checks_total += 1
                require(imported.get("part11_ffi_preservation_replay_key") == provider_import_surface.get("preservation_replay_key"), display_path(consumer_plan_path), "M274-D001-DYN-43", "imported Part 11 preservation replay key mismatch", failures)

        probe_dir = PROBE_ROOT / "runtime_probe"
        probe_dir.mkdir(parents=True, exist_ok=True)
        probe_exe = probe_dir / "m274_d001_bridge_packaging_toolchain_probe.exe"
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
        require(probe_compile.returncode == 0, display_path(RUNTIME_PROBE), "M274-D001-DYN-44", probe_compile.stderr or probe_compile.stdout or "runtime probe compile failed", failures)

        runtime_probe_output: dict[str, str] = {}
        if probe_compile.returncode == 0:
            probe_run = run_command([str(probe_exe)])
            checks_total += 1
            require(probe_run.returncode == 0, display_path(probe_exe), "M274-D001-DYN-45", probe_run.stderr or probe_run.stdout or "runtime probe failed", failures)
            runtime_probe_output = parse_probe_output(probe_run.stdout)
            for offset, (key, value) in enumerate((
                ("copy_status", "0"),
                ("packaging_topology_ready", "1"),
                ("operator_visible_evidence_ready", "1"),
                ("header_generation_ready", "0"),
                ("module_generation_ready", "0"),
                ("bridge_generation_ready", "0"),
                ("deterministic", "1"),
                ("runtime_support_library_archive_relative_path", RUNTIME_ARCHIVE),
            ), start=46):
                checks_total += 1
                require(runtime_probe_output.get(key) == value, display_path(probe_exe), f"M274-D001-DYN-{offset}", f"unexpected {key}: {runtime_probe_output}", failures)

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
            print(f"[fail] {failure.artifact} :: {failure.check_id} :: {failure.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {CONTRACT_ID} validated")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
