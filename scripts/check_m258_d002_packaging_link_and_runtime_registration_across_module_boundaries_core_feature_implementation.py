#!/usr/bin/env python3
"""Fail-closed checker for M258-D002 cross-module runtime packaging."""

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
CONTRACT_ID = "objc3c-cross-module-runtime-packaging-link-plan/m258-d002-v1"
SOURCE_ORCHESTRATION_CONTRACT_ID = "objc3c-cross-module-build-runtime-orchestration/m258-d001-v1"
IMPORT_SURFACE_CONTRACT_ID = "objc3c-runtime-aware-import-module-frontend-closure/m258-a002-v1"
REGISTRATION_MANIFEST_CONTRACT_ID = "objc3c-translation-unit-registration-manifest/m254-a002-v1"
AUTHORITY_MODEL = "runtime-import-surface-plus-imported-registration-manifest-peer-artifacts-drive-cross-module-link-plan"
PACKAGING_MODEL = "compiler-emits-cross-module-link-plan-and-merged-linker-response"
REGISTRATION_SCOPE_MODEL = "registration-ordinal-sorted-link-plan-drives-multi-image-startup-registration"
LINK_OBJECT_ORDER_MODEL = "ascending-registration-ordinal-then-translation-unit-identity-key"
LINK_PLAN_ARTIFACT = "module.cross-module-runtime-link-plan.json"
LINKER_RESPONSE_ARTIFACT = "module.cross-module-runtime-linker-options.rsp"
IMPORT_ARTIFACT = "module.runtime-import-surface.json"
MANIFEST_ARTIFACT = "module.manifest.json"
REGISTRATION_MANIFEST_ARTIFACT = "module.runtime-registration-manifest.json"
DISCOVERY_ARTIFACT = "module.runtime-metadata-discovery.json"
RUNTIME_LINKER_RESPONSE_ARTIFACT = "module.runtime-metadata-linker-options.rsp"
OBJECT_ARTIFACT = "module.obj"
BACKEND_ARTIFACT = "module.object-backend.txt"
EXPECTED_MODULES = ["runtimePackagingConsumer", "runtimePackagingProvider"]
EXPECTED_PROVIDER_PROTOCOL = "ImportedWorker"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m258" / "M258-D002" / "cross_module_runtime_packaging_summary.json"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m258" / "d002"
PROVIDER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m258_d002_runtime_packaging_provider.objc3"
CONSUMER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m258_d002_runtime_packaging_consumer.objc3"
PROBE_SOURCE = ROOT / "tests" / "tooling" / "runtime" / "m258_d002_cross_module_runtime_packaging_probe.cpp"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
DEFAULT_RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m258_packaging_link_and_runtime_registration_across_module_boundaries_core_feature_implementation_d002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m258" / "m258_d002_packaging_link_and_runtime_registration_across_module_boundaries_core_feature_implementation_packet.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
API_H = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "api.h"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m258_d002_lane_d_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m258_d002_packaging_link_and_runtime_registration_across_module_boundaries_core_feature_implementation.py"
NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"


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
        SnippetCheck("M258-D002-DOC-01", "# M258 Packaging, Link, And Runtime Registration Across Module Boundaries Core Feature Implementation Expectations (D002)"),
        SnippetCheck("M258-D002-DOC-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M258-D002-DOC-03", "`module.cross-module-runtime-link-plan.json`"),
        SnippetCheck("M258-D002-DOC-04", "D002 does not claim fully bound source method-body semantics across modules"),
    ),
    PACKET_DOC: (
        SnippetCheck("M258-D002-PKT-01", "# M258-D002 Packaging, Link, And Runtime Registration Across Module Boundaries Core Feature Implementation Packet"),
        SnippetCheck("M258-D002-PKT-02", "Dependencies: `M258-D001`, `M258-C002`"),
        SnippetCheck("M258-D002-PKT-03", "Next issue: `M258-E001`"),
        SnippetCheck("M258-D002-PKT-04", "`module.cross-module-runtime-linker-options.rsp`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M258-D002-NDOC-01", "## Cross-module runtime packaging, link planning, and registration (M258-D002)"),
        SnippetCheck("M258-D002-NDOC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M258-D002-NDOC-03", "`module.cross-module-runtime-link-plan.json`"),
    ),
    LOWERING_SPEC: (
        SnippetCheck("M258-D002-SPC-01", "## M258 cross-module runtime packaging, linking, and registration (D002)"),
        SnippetCheck("M258-D002-SPC-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M258-D002-SPC-03", "`ascending-registration-ordinal-then-translation-unit-identity-key`"),
    ),
    METADATA_SPEC: (
        SnippetCheck("M258-D002-META-01", "## M258 cross-module runtime packaging and registration anchors (D002)"),
        SnippetCheck("M258-D002-META-02", f"`{CONTRACT_ID}`"),
        SnippetCheck("M258-D002-META-03", "`module.cross-module-runtime-linker-options.rsp`"),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M258-D002-ARCH-01", "## M258 cross-module runtime packaging and registration (D002)"),
        SnippetCheck("M258-D002-ARCH-02", "the driver emits an ordered cross-module link plan and merged linker response"),
    ),
    FRONTEND_ARTIFACTS: (
        SnippetCheck("M258-D002-FRONT-01", "M258-D002 cross-module runtime packaging anchor:"),
        SnippetCheck("M258-D002-FRONT-02", "driver/runtime packaging path now materializes the ordered"),
    ),
    IR_EMITTER_CPP: (
        SnippetCheck("M258-D002-IR-01", "M258-D002 cross-module runtime packaging anchor:"),
        SnippetCheck("M258-D002-IR-02", "object-local and does not directly orchestrate multi-image packaging"),
    ),
    API_H: (
        SnippetCheck("M258-D002-API-01", "M258-D002 cross-module runtime packaging anchor:"),
        SnippetCheck("M258-D002-API-02", "filesystem-emitted link plans and runtime-registration artifacts"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M258-D002-PKG-01", '"check:objc3c:m258-d002-packaging-link-and-runtime-registration-across-module-boundaries"'),
        SnippetCheck("M258-D002-PKG-02", '"test:tooling:m258-d002-packaging-link-and-runtime-registration-across-module-boundaries"'),
        SnippetCheck("M258-D002-PKG-03", '"check:objc3c:m258-d002-lane-d-readiness"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M258-D002-RUN-01", "M258-C002 + M258-D001 + M258-D002"),
        SnippetCheck("M258-D002-RUN-02", "check_m258_d002_packaging_link_and_runtime_registration_across_module_boundaries_core_feature_implementation.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M258-D002-TEST-01", "def test_checker_passes_static"),
        SnippetCheck("M258-D002-TEST-02", "def test_checker_passes_dynamic"),
    ),
    PROBE_SOURCE: (
        SnippetCheck("M258-D002-PRB-01", 'objc3_runtime_copy_protocol_conformance_query_for_testing('),
        SnippetCheck("M258-D002-PRB-02", '"ImportedProvider", "ImportedWorker"'),
        SnippetCheck("M258-D002-PRB-03", "post_replay_imported_provider_protocol_value"),
    ),
}


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def normalize_path_text(path: Path | str) -> str:
    return str(Path(path).resolve()).replace("\\", "/")


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(read_text(path))
    if not isinstance(payload, dict):
        raise TypeError(f"expected object in {display_path(path)}")
    return payload


def run_command(command: Sequence[str], cwd: Path = ROOT) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=str(cwd),
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def resolve_tool(name: str) -> str | None:
    direct = shutil.which(name)
    if direct:
        return direct
    if not name.endswith(".exe"):
        direct = shutil.which(name + ".exe")
        if direct:
            return direct
    if sys.platform == "win32":
        llvm_bin = Path("C:/Program Files/LLVM/bin")
        candidate = llvm_bin / name
        if candidate.exists():
            return str(candidate)
        if not name.endswith(".exe"):
            candidate = llvm_bin / f"{name}.exe"
            if candidate.exists():
                return str(candidate)
    return None


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    failures: list[Finding] = []
    if not path.exists():
        return len(snippets) + 1, [Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}")]
    text = read_text(path)
    for snippet in snippets:
        if snippet.snippet not in text:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return len(snippets) + 1, failures


def ensure_binaries(failures: list[Finding]) -> int:
    checks = 0
    if NATIVE_EXE.exists():
        checks += require(True, display_path(NATIVE_EXE), "M258-D002-BIN-READY", "native binary present", failures)
        return checks
    build = run_command([NPM_EXECUTABLE, "run", "build:objc3c-native"])
    checks += require(build.returncode == 0, f"{NPM_EXECUTABLE} run build:objc3c-native", "M258-D002-BUILD", build.stderr or build.stdout or "native build failed", failures)
    checks += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M258-D002-NATIVE-EXISTS", "native binary missing after build", failures)
    return checks


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


def parse_response_lines(path: Path) -> list[str]:
    return [line.strip() for line in read_text(path).splitlines() if line.strip()]


def build_summary(skip_dynamic_probes: bool) -> tuple[dict[str, object], list[Finding], int, int]:
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    static_summary: dict[str, object] = {}
    for path, snippets in STATIC_SNIPPETS.items():
        path_checks, path_failures = check_static_contract(path, snippets)
        checks_total += path_checks
        checks_passed += path_checks - len(path_failures)
        failures.extend(path_failures)
        static_summary[display_path(path)] = {"checks": path_checks, "ok": not path_failures}

    dynamic_summary: dict[str, Any] = {"skipped": skip_dynamic_probes}
    if skip_dynamic_probes:
        return {"static_contracts": static_summary, "dynamic_probes": dynamic_summary}, failures, checks_total, checks_passed

    checks_total += ensure_binaries(failures)
    checks_passed = checks_total - len(failures)
    if failures:
        return {"static_contracts": static_summary, "dynamic_probes": dynamic_summary}, failures, checks_total, checks_passed

    provider_dir = PROBE_ROOT / "provider"
    consumer_dir = PROBE_ROOT / "consumer"
    provider_compile = compile_fixture(
        fixture=PROVIDER_FIXTURE,
        out_dir=provider_dir,
        registration_order_ordinal=1,
    )
    provider_import = provider_dir / IMPORT_ARTIFACT
    provider_manifest = provider_dir / REGISTRATION_MANIFEST_ARTIFACT
    provider_discovery = provider_dir / DISCOVERY_ARTIFACT
    provider_rsp = provider_dir / RUNTIME_LINKER_RESPONSE_ARTIFACT
    provider_obj = provider_dir / OBJECT_ARTIFACT
    provider_backend = provider_dir / BACKEND_ARTIFACT
    checks_total += 7
    checks_passed += require(provider_compile.returncode == 0, display_path(PROVIDER_FIXTURE), "M258-D002-PROVIDER-COMPILE", provider_compile.stderr or provider_compile.stdout or "provider compile failed", failures)
    checks_passed += require(provider_import.exists(), display_path(provider_import), "M258-D002-PROVIDER-IMPORT", "provider import surface missing", failures)
    checks_passed += require(provider_manifest.exists(), display_path(provider_manifest), "M258-D002-PROVIDER-MANIFEST", "provider registration manifest missing", failures)
    checks_passed += require(provider_discovery.exists(), display_path(provider_discovery), "M258-D002-PROVIDER-DISCOVERY", "provider discovery artifact missing", failures)
    checks_passed += require(provider_rsp.exists(), display_path(provider_rsp), "M258-D002-PROVIDER-RSP", "provider runtime linker response missing", failures)
    checks_passed += require(provider_obj.exists(), display_path(provider_obj), "M258-D002-PROVIDER-OBJ", "provider object missing", failures)
    checks_passed += require(provider_backend.exists() and read_text(provider_backend).strip() == "llvm-direct", display_path(provider_backend), "M258-D002-PROVIDER-BACKEND", "provider backend must be llvm-direct", failures)
    if failures:
        return {"static_contracts": static_summary, "dynamic_probes": dynamic_summary}, failures, checks_total, checks_passed

    consumer_compile = compile_fixture(
        fixture=CONSUMER_FIXTURE,
        out_dir=consumer_dir,
        registration_order_ordinal=2,
        import_surface=provider_import,
    )
    consumer_import = consumer_dir / IMPORT_ARTIFACT
    consumer_manifest = consumer_dir / REGISTRATION_MANIFEST_ARTIFACT
    consumer_discovery = consumer_dir / DISCOVERY_ARTIFACT
    consumer_rsp = consumer_dir / RUNTIME_LINKER_RESPONSE_ARTIFACT
    consumer_obj = consumer_dir / OBJECT_ARTIFACT
    consumer_backend = consumer_dir / BACKEND_ARTIFACT
    consumer_plan = consumer_dir / LINK_PLAN_ARTIFACT
    consumer_plan_rsp = consumer_dir / LINKER_RESPONSE_ARTIFACT
    checks_total += 9
    checks_passed += require(consumer_compile.returncode == 0, display_path(CONSUMER_FIXTURE), "M258-D002-CONSUMER-COMPILE", consumer_compile.stderr or consumer_compile.stdout or "consumer compile failed", failures)
    checks_passed += require(consumer_import.exists(), display_path(consumer_import), "M258-D002-CONSUMER-IMPORT", "consumer import surface missing", failures)
    checks_passed += require(consumer_manifest.exists(), display_path(consumer_manifest), "M258-D002-CONSUMER-MANIFEST", "consumer registration manifest missing", failures)
    checks_passed += require(consumer_discovery.exists(), display_path(consumer_discovery), "M258-D002-CONSUMER-DISCOVERY", "consumer discovery artifact missing", failures)
    checks_passed += require(consumer_rsp.exists(), display_path(consumer_rsp), "M258-D002-CONSUMER-RSP", "consumer runtime linker response missing", failures)
    checks_passed += require(consumer_obj.exists(), display_path(consumer_obj), "M258-D002-CONSUMER-OBJ", "consumer object missing", failures)
    checks_passed += require(consumer_backend.exists() and read_text(consumer_backend).strip() == "llvm-direct", display_path(consumer_backend), "M258-D002-CONSUMER-BACKEND", "consumer backend must be llvm-direct", failures)
    checks_passed += require(consumer_plan.exists(), display_path(consumer_plan), "M258-D002-PLAN", "consumer cross-module link plan missing", failures)
    checks_passed += require(consumer_plan_rsp.exists(), display_path(consumer_plan_rsp), "M258-D002-PLAN-RSP", "consumer cross-module linker response missing", failures)
    if failures:
        return {"static_contracts": static_summary, "dynamic_probes": dynamic_summary}, failures, checks_total, checks_passed

    provider_import_payload = load_json(provider_import)
    provider_manifest_payload = load_json(provider_manifest)
    consumer_import_payload = load_json(consumer_import)
    consumer_manifest_payload = load_json(consumer_manifest)
    plan_payload = load_json(consumer_plan)
    response_lines = parse_response_lines(consumer_plan_rsp)

    checks = []
    checks.append((plan_payload.get("contract_id") == CONTRACT_ID, display_path(consumer_plan), "M258-D002-PLAN-CONTRACT", "unexpected D002 contract id"))
    checks.append((plan_payload.get("source_orchestration_contract_id") == SOURCE_ORCHESTRATION_CONTRACT_ID, display_path(consumer_plan), "M258-D002-PLAN-SOURCE-D001", "unexpected source orchestration contract id"))
    checks.append((plan_payload.get("import_surface_contract_id") == IMPORT_SURFACE_CONTRACT_ID, display_path(consumer_plan), "M258-D002-PLAN-IMPORT-CONTRACT", "unexpected import surface contract id"))
    checks.append((plan_payload.get("registration_manifest_contract_id") == REGISTRATION_MANIFEST_CONTRACT_ID, display_path(consumer_plan), "M258-D002-PLAN-MANIFEST-CONTRACT", "unexpected registration-manifest contract id"))
    checks.append((plan_payload.get("authority_model") == AUTHORITY_MODEL, display_path(consumer_plan), "M258-D002-PLAN-AUTHORITY", "unexpected authority model"))
    checks.append((plan_payload.get("packaging_model") == PACKAGING_MODEL, display_path(consumer_plan), "M258-D002-PLAN-PACKAGING", "unexpected packaging model"))
    checks.append((plan_payload.get("registration_scope_model") == REGISTRATION_SCOPE_MODEL, display_path(consumer_plan), "M258-D002-PLAN-SCOPE", "unexpected registration scope model"))
    checks.append((plan_payload.get("link_object_order_model") == LINK_OBJECT_ORDER_MODEL, display_path(consumer_plan), "M258-D002-PLAN-ORDER-MODEL", "unexpected link object order model"))
    checks.append((plan_payload.get("module_names_lexicographic") == EXPECTED_MODULES, display_path(consumer_plan), "M258-D002-PLAN-MODULES", "unexpected module set"))
    checks.append((plan_payload.get("module_image_count") == 2, display_path(consumer_plan), "M258-D002-PLAN-MODULE-COUNT", "unexpected module-image count"))
    checks.append((plan_payload.get("direct_import_input_count") == 1, display_path(consumer_plan), "M258-D002-PLAN-DIRECT-COUNT", "unexpected direct import count"))
    checks.append((plan_payload.get("direct_import_surface_artifact_paths") == [normalize_path_text(provider_import)], display_path(consumer_plan), "M258-D002-PLAN-DIRECT-SURFACE", "unexpected direct import surface path list"))
    checks.append((plan_payload.get("runtime_support_library_archive_relative_path") == provider_manifest_payload.get("runtime_support_library_archive_relative_path") == consumer_manifest_payload.get("runtime_support_library_archive_relative_path"), display_path(consumer_plan), "M258-D002-PLAN-RUNTIME-LIB", "runtime library path mismatch across plan/manifests"))
    checks.append((plan_payload.get("object_format") == provider_manifest_payload.get("object_format") == consumer_manifest_payload.get("object_format") == "coff", display_path(consumer_plan), "M258-D002-PLAN-FORMAT", "object format mismatch across plan/manifests"))
    local_module = plan_payload.get("local_module") if isinstance(plan_payload.get("local_module"), dict) else {}
    imported_modules = plan_payload.get("imported_modules") if isinstance(plan_payload.get("imported_modules"), list) else []
    imported_module = imported_modules[0] if len(imported_modules) == 1 and isinstance(imported_modules[0], dict) else {}
    checks.append((local_module.get("module_name") == consumer_import_payload.get("module_name") == "runtimePackagingConsumer", display_path(consumer_plan), "M258-D002-LOCAL-MODULE", "unexpected local module payload"))
    checks.append((local_module.get("import_surface_artifact_relative_path") == normalize_path_text(consumer_import), display_path(consumer_plan), "M258-D002-LOCAL-IMPORT-PATH", "unexpected local import surface path"))
    checks.append((local_module.get("registration_manifest_artifact_relative_path") == normalize_path_text(consumer_manifest), display_path(consumer_plan), "M258-D002-LOCAL-MANIFEST-PATH", "unexpected local registration manifest path"))
    checks.append((local_module.get("object_artifact_relative_path") == normalize_path_text(consumer_obj), display_path(consumer_plan), "M258-D002-LOCAL-OBJ-PATH", "unexpected local object path"))
    checks.append((local_module.get("translation_unit_identity_model") == consumer_manifest_payload.get("translation_unit_identity_model"), display_path(consumer_plan), "M258-D002-LOCAL-IDENTITY-MODEL", "unexpected local identity model"))
    checks.append((local_module.get("translation_unit_identity_key") == consumer_manifest_payload.get("translation_unit_identity_key"), display_path(consumer_plan), "M258-D002-LOCAL-IDENTITY-KEY", "unexpected local identity key"))
    checks.append((local_module.get("translation_unit_registration_order_ordinal") == 2 == consumer_manifest_payload.get("translation_unit_registration_order_ordinal"), display_path(consumer_plan), "M258-D002-LOCAL-ORDINAL", "unexpected local registration ordinal"))
    checks.append((local_module.get("driver_linker_flags") == consumer_manifest_payload.get("driver_linker_flags"), display_path(consumer_plan), "M258-D002-LOCAL-FLAGS", "unexpected local linker flags"))
    checks.append((imported_module.get("module_name") == provider_import_payload.get("module_name") == "runtimePackagingProvider", display_path(consumer_plan), "M258-D002-IMPORTED-MODULE", "unexpected imported module payload"))
    checks.append((imported_module.get("import_surface_artifact_path") == normalize_path_text(provider_import), display_path(consumer_plan), "M258-D002-IMPORTED-IMPORT-PATH", "unexpected imported import surface path"))
    checks.append((imported_module.get("registration_manifest_artifact_path") == normalize_path_text(provider_manifest), display_path(consumer_plan), "M258-D002-IMPORTED-MANIFEST-PATH", "unexpected imported registration manifest path"))
    checks.append((imported_module.get("object_artifact_path") == normalize_path_text(provider_obj), display_path(consumer_plan), "M258-D002-IMPORTED-OBJ-PATH", "unexpected imported object path"))
    checks.append((imported_module.get("discovery_artifact_path") == normalize_path_text(provider_discovery), display_path(consumer_plan), "M258-D002-IMPORTED-DISCOVERY-PATH", "unexpected imported discovery path"))
    checks.append((imported_module.get("linker_response_artifact_path") == normalize_path_text(provider_rsp), display_path(consumer_plan), "M258-D002-IMPORTED-RSP-PATH", "unexpected imported linker response path"))
    checks.append((imported_module.get("translation_unit_identity_model") == provider_manifest_payload.get("translation_unit_identity_model"), display_path(consumer_plan), "M258-D002-IMPORTED-IDENTITY-MODEL", "unexpected imported identity model"))
    checks.append((imported_module.get("translation_unit_identity_key") == provider_manifest_payload.get("translation_unit_identity_key"), display_path(consumer_plan), "M258-D002-IMPORTED-IDENTITY-KEY", "unexpected imported identity key"))
    checks.append((imported_module.get("translation_unit_registration_order_ordinal") == 1 == provider_manifest_payload.get("translation_unit_registration_order_ordinal"), display_path(consumer_plan), "M258-D002-IMPORTED-ORDINAL", "unexpected imported registration ordinal"))
    checks.append((imported_module.get("object_format") == "coff", display_path(consumer_plan), "M258-D002-IMPORTED-FORMAT", "unexpected imported object format"))
    checks.append((imported_module.get("runtime_support_library_archive_relative_path") == provider_manifest_payload.get("runtime_support_library_archive_relative_path"), display_path(consumer_plan), "M258-D002-IMPORTED-RUNTIME-LIB", "unexpected imported runtime library path"))
    checks.append((imported_module.get("driver_linker_flags") == provider_manifest_payload.get("driver_linker_flags"), display_path(consumer_plan), "M258-D002-IMPORTED-FLAGS", "unexpected imported linker flags"))
    checks.append((plan_payload.get("link_object_artifacts") == [normalize_path_text(provider_obj), normalize_path_text(consumer_obj)], display_path(consumer_plan), "M258-D002-PLAN-OBJECT-ORDER", "unexpected link object ordering"))
    expected_flags = list(provider_manifest_payload.get("driver_linker_flags", [])) + list(consumer_manifest_payload.get("driver_linker_flags", []))
    checks.append((plan_payload.get("driver_linker_flags") == expected_flags, display_path(consumer_plan), "M258-D002-PLAN-FLAGS", "unexpected merged driver linker flags"))
    checks.append((response_lines == expected_flags, display_path(consumer_plan_rsp), "M258-D002-PLAN-RSP-CONTENT", "response file contents must match merged driver linker flags"))
    checks.append((plan_payload.get("ready") is True, display_path(consumer_plan), "M258-D002-PLAN-READY", "plan must be marked ready"))
    checks_total += len(checks)
    for condition, artifact, check_id, detail in checks:
        checks_passed += require(condition, artifact, check_id, detail, failures)
    if failures:
        dynamic_summary.update(
            {
                "provider_compile": {"returncode": provider_compile.returncode},
                "consumer_compile": {"returncode": consumer_compile.returncode},
                "link_plan": plan_payload,
                "response_lines": response_lines,
            }
        )
        return {"static_contracts": static_summary, "dynamic_probes": dynamic_summary}, failures, checks_total, checks_passed

    clangxx = resolve_tool("clang++") or resolve_tool("clang++.exe")
    checks_total += 2
    checks_passed += require(clangxx is not None, "clang++", "M258-D002-LINK-TOOL", "unable to resolve clang++", failures)
    runtime_library = ROOT / str(plan_payload["runtime_support_library_archive_relative_path"])
    checks_passed += require(runtime_library.exists(), display_path(runtime_library), "M258-D002-RUNTIME-LIB-EXISTS", "runtime library missing", failures)
    if failures:
        return {"static_contracts": static_summary, "dynamic_probes": dynamic_summary}, failures, checks_total, checks_passed

    probe_dir = ROOT / "tmp" / "reports" / "m258" / "M258-D002" / "probe"
    probe_dir.mkdir(parents=True, exist_ok=True)
    probe_exe = probe_dir / "m258_d002_cross_module_runtime_packaging_probe.exe"
    link_command = [
        str(clangxx),
        "-std=c++20",
        "-Wall",
        "-Wextra",
        "-pedantic",
        f"-I{RUNTIME_INCLUDE_ROOT.resolve()}",
        str(PROBE_SOURCE.resolve()),
        *[str(Path(value)) for value in plan_payload["link_object_artifacts"]],
        str(runtime_library.resolve()),
        f"@{consumer_plan_rsp.resolve()}",
        "-o",
        str(probe_exe.resolve()),
    ]
    link_completed = run_command(link_command)
    link_output = (link_completed.stdout or "") + (link_completed.stderr or "")
    checks_total += 3
    checks_passed += require(link_completed.returncode == 0, display_path(probe_exe), "M258-D002-PROBE-LINK", f"probe link failed: {link_output.strip()}", failures)
    checks_passed += require(probe_exe.exists(), display_path(probe_exe), "M258-D002-PROBE-EXISTS", "probe executable missing after link", failures)
    checks_passed += require("LNK4078" not in link_output, display_path(probe_exe), "M258-D002-PROBE-LINK-WARNINGS", "probe link must not emit section-attribute mismatch warnings", failures)
    if failures:
        dynamic_summary.update(
            {
                "provider_compile": {"returncode": provider_compile.returncode},
                "consumer_compile": {"returncode": consumer_compile.returncode},
                "link_plan": plan_payload,
                "response_lines": response_lines,
                "link_command": link_command,
                "link_output": link_output,
            }
        )
        return {"static_contracts": static_summary, "dynamic_probes": dynamic_summary}, failures, checks_total, checks_passed

    probe_run = run_command([str(probe_exe.resolve())])
    probe_payload = json.loads(probe_run.stdout)
    probe_checks = []
    probe_checks.append((probe_run.returncode == 0, display_path(probe_exe), "M258-D002-PROBE-RUN", f"probe exited with {probe_run.returncode}"))
    probe_checks.append((probe_payload.get("startup_registration_copy_status") == 0, display_path(probe_exe), "M258-D002-PROBE-STARTUP-STATUS", "startup registration snapshot must succeed"))
    probe_checks.append((probe_payload.get("startup_registered_image_count") == 2, display_path(probe_exe), "M258-D002-PROBE-STARTUP-COUNT", "startup must register two images"))
    probe_checks.append((probe_payload.get("startup_next_expected_registration_order_ordinal") == 3, display_path(probe_exe), "M258-D002-PROBE-STARTUP-NEXT", "startup next expected ordinal must be 3"))
    probe_checks.append((probe_payload.get("startup_last_registered_translation_unit_identity_key") == consumer_manifest_payload.get("translation_unit_identity_key"), display_path(probe_exe), "M258-D002-PROBE-STARTUP-LAST", "startup last registered identity must be the consumer translation unit"))
    probe_checks.append((probe_payload.get("imported_entry_status") == 0 and probe_payload.get("imported_entry_found") == 1, display_path(probe_exe), "M258-D002-PROBE-IMPORTED-ENTRY", "imported provider entry must be realized"))
    probe_checks.append((probe_payload.get("local_entry_status") == 0 and probe_payload.get("local_entry_found") == 1, display_path(probe_exe), "M258-D002-PROBE-LOCAL-ENTRY", "local consumer entry must be realized"))
    probe_checks.append((probe_payload.get("imported_registration_order_ordinal") == 1, display_path(probe_exe), "M258-D002-PROBE-IMPORTED-ORDINAL", "imported provider ordinal must be 1"))
    probe_checks.append((probe_payload.get("local_registration_order_ordinal") == 2, display_path(probe_exe), "M258-D002-PROBE-LOCAL-ORDINAL", "local consumer ordinal must be 2"))
    probe_checks.append((probe_payload.get("imported_worker_query_status") == 0, display_path(probe_exe), "M258-D002-PROBE-PROTOCOL-STATUS", "imported protocol query must succeed"))
    probe_checks.append((probe_payload.get("imported_worker_conforms") == 1, display_path(probe_exe), "M258-D002-PROBE-PROTOCOL-CONFORMS", "imported provider must conform to ImportedWorker"))
    probe_checks.append((int(probe_payload.get("imported_provider_class_value", 0)) != 0, display_path(probe_exe), "M258-D002-PROBE-IMPORTED-CLASS-VALUE", "imported class dispatch must be nonzero"))
    probe_checks.append((int(probe_payload.get("imported_provider_protocol_value", 0)) != 0, display_path(probe_exe), "M258-D002-PROBE-IMPORTED-PROTOCOL-VALUE", "imported protocol dispatch must be nonzero"))
    probe_checks.append((int(probe_payload.get("local_consumer_class_value", 0)) != 0, display_path(probe_exe), "M258-D002-PROBE-LOCAL-CLASS-VALUE", "local class dispatch must be nonzero"))
    probe_checks.append((probe_payload.get("post_reset_registration_copy_status") == 0 and probe_payload.get("post_reset_replay_copy_status") == 0, display_path(probe_exe), "M258-D002-PROBE-RESET-STATUS", "reset snapshots must succeed"))
    probe_checks.append((probe_payload.get("post_reset_registered_image_count") == 0, display_path(probe_exe), "M258-D002-PROBE-RESET-COUNT", "reset must clear live registration state"))
    probe_checks.append((probe_payload.get("post_reset_retained_bootstrap_image_count") == 2, display_path(probe_exe), "M258-D002-PROBE-RESET-RETAINED", "reset must retain both bootstrap images for replay"))
    probe_checks.append((probe_payload.get("replay_status") == 0, display_path(probe_exe), "M258-D002-PROBE-REPLAY-STATUS", "replay must succeed"))
    probe_checks.append((probe_payload.get("post_replay_registration_copy_status") == 0 and probe_payload.get("post_replay_replay_copy_status") == 0, display_path(probe_exe), "M258-D002-PROBE-POST-REPLAY-STATUS", "post-replay snapshots must succeed"))
    probe_checks.append((probe_payload.get("post_replay_registered_image_count") == 2, display_path(probe_exe), "M258-D002-PROBE-POST-REPLAY-COUNT", "replay must restore both images"))
    probe_checks.append((probe_payload.get("post_replay_next_expected_registration_order_ordinal") == 3, display_path(probe_exe), "M258-D002-PROBE-POST-REPLAY-NEXT", "post-replay next expected ordinal must be 3"))
    probe_checks.append((probe_payload.get("post_replay_last_registered_translation_unit_identity_key") == consumer_manifest_payload.get("translation_unit_identity_key"), display_path(probe_exe), "M258-D002-PROBE-POST-REPLAY-LAST", "post-replay last registered identity must be the consumer translation unit"))
    probe_checks.append((probe_payload.get("post_replay_last_replayed_translation_unit_identity_key") == consumer_manifest_payload.get("translation_unit_identity_key"), display_path(probe_exe), "M258-D002-PROBE-POST-REPLAY-REPLAYED", "post-replay last replayed identity must be the consumer translation unit"))
    probe_checks.append((probe_payload.get("post_replay_imported_entry_status") == 0 and probe_payload.get("post_replay_imported_entry_found") == 1, display_path(probe_exe), "M258-D002-PROBE-POST-REPLAY-IMPORTED", "replay must restore the imported provider entry"))
    probe_checks.append((probe_payload.get("post_replay_local_entry_status") == 0 and probe_payload.get("post_replay_local_entry_found") == 1, display_path(probe_exe), "M258-D002-PROBE-POST-REPLAY-LOCAL", "replay must restore the local consumer entry"))
    probe_checks.append((probe_payload.get("post_replay_imported_provider_class_value") == probe_payload.get("imported_provider_class_value"), display_path(probe_exe), "M258-D002-PROBE-POST-REPLAY-IMPORTED-CLASS", "imported class dispatch must be replay-stable"))
    probe_checks.append((probe_payload.get("post_replay_imported_provider_protocol_value") == probe_payload.get("imported_provider_protocol_value"), display_path(probe_exe), "M258-D002-PROBE-POST-REPLAY-IMPORTED-PROTOCOL", "imported protocol dispatch must be replay-stable"))
    probe_checks.append((probe_payload.get("post_replay_local_consumer_class_value") == probe_payload.get("local_consumer_class_value"), display_path(probe_exe), "M258-D002-PROBE-POST-REPLAY-LOCAL-CLASS", "local class dispatch must be replay-stable"))
    checks_total += len(probe_checks)
    for condition, artifact, check_id, detail in probe_checks:
        checks_passed += require(condition, artifact, check_id, detail, failures)

    dynamic_summary.update(
        {
            "provider_compile": {
                "command": [str(NATIVE_EXE), str(PROVIDER_FIXTURE), "--out-dir", str(provider_dir), "--emit-prefix", "module", "--objc3-bootstrap-registration-order-ordinal", "1"],
                "returncode": provider_compile.returncode,
            },
            "consumer_compile": {
                "command": [str(NATIVE_EXE), str(CONSUMER_FIXTURE), "--out-dir", str(consumer_dir), "--emit-prefix", "module", "--objc3-bootstrap-registration-order-ordinal", "2", "--objc3-import-runtime-surface", str(provider_import)],
                "returncode": consumer_compile.returncode,
            },
            "provider": {
                "import_surface": provider_import_payload,
                "registration_manifest": provider_manifest_payload,
                "runtime_metadata_response": parse_response_lines(provider_rsp),
            },
            "consumer": {
                "import_surface": consumer_import_payload,
                "registration_manifest": consumer_manifest_payload,
                "cross_module_link_plan": plan_payload,
                "cross_module_linker_response": response_lines,
            },
            "probe": {
                "link_command": link_command,
                "link_returncode": link_completed.returncode,
                "link_output": link_output,
                "probe_payload": probe_payload,
            },
        }
    )
    return {"static_contracts": static_summary, "dynamic_probes": dynamic_summary}, failures, checks_total, checks_passed


def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    summary, failures, checks_total, checks_passed = build_summary(args.skip_dynamic_probes)
    payload = {
        "ok": not failures,
        "contract_id": CONTRACT_ID,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        **summary,
        "failures": [failure.__dict__ for failure in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    if failures:
        for failure in failures:
            print(f"[{failure.check_id}] {failure.artifact}: {failure.detail}", file=sys.stderr)
        return 1
    print(json.dumps(payload, indent=2, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
