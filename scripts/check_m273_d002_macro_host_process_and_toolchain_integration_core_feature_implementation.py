#!/usr/bin/env python3
"""Issue-local checker for M273-D002 macro host process and toolchain integration."""

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
MODE = "m273-d002-part10-macro-host-process-cache-runtime-integration-v1"
ISSUE = "M273-D002"
ISSUE_NUMBER = "#7357"
CONTRACT_ID = "objc3c-part10-macro-host-process-cache-runtime-integration/m273-d002-v1"
SOURCE_CONTRACT_ID = "objc3c-part10-expansion-host-runtime-boundary/m273-d001-v1"
PRESERVATION_CONTRACT_ID = "objc3c-part10-module-interface-replay-preservation/m273-c003-v1"
SURFACE_KEY = "objc_part10_macro_host_process_and_cache_runtime_integration"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m273" / "M273-D002" / "macro_host_process_cache_runtime_integration_summary.json"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m273" / "d002"
CACHE_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-native" / "cache" / "part10"
BACKUP_ROOT = ROOT / "tmp" / "reports" / "m273" / "M273-D002" / "backups"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m273_macro_host_process_and_toolchain_integration_core_feature_implementation_d002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m273" / "m273_d002_macro_host_process_and_toolchain_integration_core_feature_implementation_packet.md"
CHECKER_FILE = ROOT / "scripts" / "check_m273_d002_macro_host_process_and_toolchain_integration_core_feature_implementation.py"
READINESS_RUNNER = ROOT / "scripts" / "run_m273_d002_lane_d_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m273_d002_macro_host_process_and_toolchain_integration_core_feature_implementation.py"
PROVIDER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m273_d002_macro_host_process_provider.objc3"
CONSUMER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m273_d002_macro_host_process_consumer.objc3"
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
RUNTIME_LIB = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_TABLES = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
LOWER_H = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWER_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
PROCESS_H = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.h"
PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
FRONTEND_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
IMPORT_SURFACE_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.h"
IMPORT_SURFACE_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.cpp"
RUNTIME_H = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
IMPORT_ARTIFACT = "module.runtime-import-surface.json"
LINK_PLAN_ARTIFACT = "module.cross-module-runtime-link-plan.json"
CACHE_ARTIFACT = "module.part10-macro-host-cache.json"
MANIFEST_ARTIFACT = "module.manifest.json"
RUNNER_RELATIVE_PATH = "artifacts/bin/objc3c-frontend-c-api-runner.exe"
CACHE_ROOT_RELATIVE_PATH = "tmp/artifacts/objc3c-native/cache/part10"
PROVIDER_MODULE = "Part10HostProcessProvider"
CONSUMER_MODULE = "Part10HostProcessConsumer"


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
        SnippetCheck("M273-D002-EXP-01", CONTRACT_ID),
        SnippetCheck("M273-D002-EXP-02", SURFACE_KEY),
        SnippetCheck("M273-D002-EXP-03", RUNNER_RELATIVE_PATH),
        SnippetCheck("M273-D002-EXP-04", CACHE_ROOT_RELATIVE_PATH),
        SnippetCheck("M273-D002-EXP-05", CACHE_ARTIFACT),
        SnippetCheck("M273-D002-EXP-06", "The issue-local checker is fail-closed."),
    ),
    PACKET_DOC: (
        SnippetCheck("M273-D002-PKT-01", CONTRACT_ID),
        SnippetCheck("M273-D002-PKT-02", ISSUE_NUMBER),
        SnippetCheck("M273-D002-PKT-03", "M273-C003"),
        SnippetCheck("M273-D002-PKT-04", "M273-D001"),
        SnippetCheck("M273-D002-PKT-05", "M273-E001"),
        SnippetCheck("M273-D002-PKT-06", CACHE_ARTIFACT),
    ),
    CHECKER_FILE: (
        SnippetCheck("M273-D002-CHK-01", f'CONTRACT_ID = "{CONTRACT_ID}"'),
        SnippetCheck("M273-D002-CHK-02", f'SURFACE_KEY = "{SURFACE_KEY}"'),
        SnippetCheck("M273-D002-CHK-03", "compile_provider_with_native"),
        SnippetCheck("M273-D002-CHK-04", "allocate_backup_path"),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M273-D002-RUN-01", "M273-C003 + M273-D001 + M273-D002"),
        SnippetCheck("M273-D002-RUN-02", "build_objc3c_native_docs.py"),
        SnippetCheck("M273-D002-RUN-03", "pytest"),
    ),
    TEST_FILE: (
        SnippetCheck("M273-D002-TEST-01", "def test_checker_passes_static() -> None:"),
        SnippetCheck("M273-D002-TEST-02", "def test_checker_passes_dynamic() -> None:"),
        SnippetCheck("M273-D002-TEST-03", 'assert summary["implementation_ready"] is True'),
    ),
    PROVIDER_FIXTURE: (
        SnippetCheck("M273-D002-FIX-01", PROVIDER_MODULE),
        SnippetCheck("M273-D002-FIX-02", 'objc_macro(named("Trace"))'),
        SnippetCheck("M273-D002-FIX-03", "behavior=Observed"),
    ),
    CONSUMER_FIXTURE: (
        SnippetCheck("M273-D002-FIX-04", CONSUMER_MODULE),
        SnippetCheck("M273-D002-FIX-05", "fn main() -> i32"),
    ),
    DOC_SOURCE: (
        SnippetCheck("M273-D002-DOC-01", "M273 macro host process and cache integration"),
        SnippetCheck("M273-D002-DOC-02", SURFACE_KEY),
        SnippetCheck("M273-D002-DOC-03", CACHE_ARTIFACT),
    ),
    SPEC_ATTR: (
        SnippetCheck("M273-D002-SPEC-01", CONTRACT_ID),
        SnippetCheck("M273-D002-SPEC-02", SURFACE_KEY),
        SnippetCheck("M273-D002-SPEC-03", CACHE_ROOT_RELATIVE_PATH),
    ),
    SPEC_TABLES: (
        SnippetCheck("M273-D002-TBL-01", CONTRACT_ID),
        SnippetCheck("M273-D002-TBL-02", CACHE_ARTIFACT),
    ),
    LOWER_H: (
        SnippetCheck("M273-D002-LWRH-01", CONTRACT_ID),
        SnippetCheck("M273-D002-LWRH-02", RUNNER_RELATIVE_PATH),
        SnippetCheck("M273-D002-LWRH-03", CACHE_ROOT_RELATIVE_PATH),
    ),
    LOWER_CPP: (
        SnippetCheck("M273-D002-LWRC-01", "Objc3Part10MacroHostProcessCacheRuntimeIntegrationSummary"),
        SnippetCheck("M273-D002-LWRC-02", "next_issue=M273-D003"),
    ),
    PROCESS_H: (
        SnippetCheck("M273-D002-PROCH-01", "Objc3Part10MacroHostProcessCacheArtifactInputs"),
        SnippetCheck("M273-D002-PROCH-02", "TryBuildObjc3Part10MacroHostProcessCacheArtifact"),
    ),
    PROCESS_CPP: (
        SnippetCheck("M273-D002-PROCC-01", "ComputeFnv1a64Hex"),
        SnippetCheck("M273-D002-PROCC-02", "module.host-summary.json"),
        SnippetCheck("M273-D002-PROCC-03", "part10_host_cache_imported_module_names_lexicographic"),
    ),
    DRIVER_CPP: (
        SnippetCheck("M273-D002-DRV-01", "WritePart10MacroHostProcessCacheArtifact"),
        SnippetCheck("M273-D002-DRV-02", "expected_part10_host_cache_contract_id"),
    ),
    FRONTEND_CPP: (
        SnippetCheck("M273-D002-FRT-01", SURFACE_KEY),
        SnippetCheck("M273-D002-FRT-02", "BuildPart10MacroHostProcessCacheRuntimeIntegrationSummaryJson"),
    ),
    IMPORT_SURFACE_H: (
        SnippetCheck("M273-D002-IMPH-01", "part10_macro_host_process_cache_runtime_integration_present"),
        SnippetCheck("M273-D002-IMPH-02", "part10_macro_host_process_cache_replay_key"),
    ),
    IMPORT_SURFACE_CPP: (
        SnippetCheck("M273-D002-IMPC-01", "PopulateImportedPart10MacroHostProcessCacheRuntimeIntegration"),
        SnippetCheck("M273-D002-IMPC-02", "part10_macro_host_process_cache_runtime_ready"),
    ),
    RUNTIME_H: (
        SnippetCheck("M273-D002-RTH-01", "objc3_runtime_part10_macro_host_process_cache_integration_snapshot"),
        SnippetCheck("M273-D002-RTH-02", "objc3_runtime_copy_part10_macro_host_process_cache_integration_snapshot_for_testing"),
    ),
    RUNTIME_CPP: (
        SnippetCheck("M273-D002-RTC-01", "objc3_runtime_copy_part10_macro_host_process_cache_integration_snapshot_for_testing"),
        SnippetCheck("M273-D002-RTC-02", RUNNER_RELATIVE_PATH),
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


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected object in {display_path(path)}")
    return payload


def extract_surface_member(payload: dict[str, Any], artifact: Path, failures: list[Finding]) -> dict[str, Any] | None:
    surface = payload.get(SURFACE_KEY)
    if isinstance(surface, dict):
        return surface
    failures.append(Finding(display_path(artifact), "M273-D002-SURFACE-01", f"missing object member: {SURFACE_KEY}"))
    return None


def run_command(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return 0
    text = path.read_text(encoding="utf-8")
    passed = 1
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def ensure_native_build(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    if NATIVE_EXE.exists() and RUNNER_EXE.exists() and RUNTIME_LIB.exists():
        return 3, 3, {"reused_existing_binaries": True, "returncode": 0}
    ensure_summary = ROOT / "tmp" / "reports" / "m273" / "M273-D002" / "ensure_objc3c_native_build_summary.json"
    completed = run_command([
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m273-d002-issue-local-check",
        "--summary-out",
        str(ensure_summary),
    ])
    total = 4
    passed = 0
    passed += require(completed.returncode == 0, display_path(BUILD_HELPER), "M273-D002-BUILD-01", completed.stderr or completed.stdout or "native build failed", failures)
    passed += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M273-D002-BUILD-02", "native compiler missing after build", failures)
    passed += require(RUNNER_EXE.exists(), display_path(RUNNER_EXE), "M273-D002-BUILD-03", "frontend runner missing after build", failures)
    passed += require(RUNTIME_LIB.exists(), display_path(RUNTIME_LIB), "M273-D002-BUILD-04", "runtime library missing after build", failures)
    return total, passed, {"reused_existing_binaries": False, "returncode": completed.returncode, "ensure_summary": display_path(ensure_summary)}


def allocate_backup_path() -> Path:
    BACKUP_ROOT.mkdir(parents=True, exist_ok=True)
    index = 1
    while True:
        candidate = BACKUP_ROOT / f"cache-root-pre-d002-{index:02d}"
        if not candidate.exists():
            return candidate
        index += 1


def backup_existing_cache_root() -> Path | None:
    if not CACHE_ROOT.exists():
        return None
    backup_path = allocate_backup_path()
    backup_path.parent.mkdir(parents=True, exist_ok=True)
    shutil.move(str(CACHE_ROOT), str(backup_path))
    return backup_path


def compile_provider_with_native(out_dir: Path) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_command([
        str(NATIVE_EXE),
        str(PROVIDER_FIXTURE),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--objc3-bootstrap-registration-order-ordinal",
        "1",
    ])


def compile_consumer_with_native(out_dir: Path, provider_import_surface: Path) -> subprocess.CompletedProcess[str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_command([
        str(NATIVE_EXE),
        str(CONSUMER_FIXTURE),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
        "--objc3-bootstrap-registration-order-ordinal",
        "2",
        "--objc3-import-runtime-surface",
        str(provider_import_surface),
    ])


def validate_import_surface(payload: dict[str, Any], *, imported_module_count: int, local_macro_artifact_count: int, local_property_behavior_artifact_count: int, artifact: Path, failures: list[Finding]) -> tuple[int, int]:
    total = 11
    passed = 0
    passed += require(payload.get("contract_id") == CONTRACT_ID, display_path(artifact), "M273-D002-IMP-01", "unexpected contract id", failures)
    passed += require(payload.get("source_contract_id") == SOURCE_CONTRACT_ID, display_path(artifact), "M273-D002-IMP-02", "unexpected source contract id", failures)
    passed += require(payload.get("host_executable_relative_path") == RUNNER_RELATIVE_PATH, display_path(artifact), "M273-D002-IMP-03", "unexpected runner path", failures)
    passed += require(payload.get("cache_root_relative_path") == CACHE_ROOT_RELATIVE_PATH, display_path(artifact), "M273-D002-IMP-04", "unexpected cache root", failures)
    passed += require(payload.get("runtime_import_artifact_ready") is True, display_path(artifact), "M273-D002-IMP-05", "runtime_import_artifact_ready must be true", failures)
    passed += require(payload.get("separate_compilation_ready") is True, display_path(artifact), "M273-D002-IMP-06", "separate_compilation_ready must be true", failures)
    passed += require(payload.get("deterministic") is True, display_path(artifact), "M273-D002-IMP-07", "deterministic must be true", failures)
    passed += require(payload.get("imported_module_count") == imported_module_count, display_path(artifact), "M273-D002-IMP-08", "unexpected imported module count", failures)
    passed += require(payload.get("local_macro_artifact_count") == local_macro_artifact_count, display_path(artifact), "M273-D002-IMP-09", "unexpected local macro artifact count", failures)
    passed += require(payload.get("local_property_behavior_artifact_count") == local_property_behavior_artifact_count, display_path(artifact), "M273-D002-IMP-10", "unexpected local property behavior artifact count", failures)
    passed += require(isinstance(payload.get("replay_key"), str) and bool(payload.get("replay_key")), display_path(artifact), "M273-D002-IMP-11", "replay_key must be non-empty", failures)
    return total, passed


def validate_cache_sidecar(payload: dict[str, Any], *, expect_launch_attempted: bool | None, artifact: Path, failures: list[Finding]) -> tuple[int, int]:
    total = 13
    passed = 0
    passed += require(payload.get("contract_id") == CONTRACT_ID, display_path(artifact), "M273-D002-CACHE-01", "unexpected cache artifact contract id", failures)
    passed += require(payload.get("source_contract_id") == SOURCE_CONTRACT_ID, display_path(artifact), "M273-D002-CACHE-02", "unexpected cache artifact source contract id", failures)
    passed += require(payload.get("artifact") == CACHE_ARTIFACT, display_path(artifact), "M273-D002-CACHE-03", "unexpected cache artifact name", failures)
    passed += require(payload.get("host_executable_relative_path") == RUNNER_RELATIVE_PATH, display_path(artifact), "M273-D002-CACHE-04", "unexpected runner path", failures)
    passed += require(payload.get("cache_root_relative_path") == CACHE_ROOT_RELATIVE_PATH, display_path(artifact), "M273-D002-CACHE-05", "unexpected cache root", failures)
    if expect_launch_attempted is None:
        passed += require(isinstance(payload.get("launch_attempted"), bool), display_path(artifact), "M273-D002-CACHE-06", "launch_attempted must be boolean", failures)
    else:
        passed += require(payload.get("launch_attempted") is expect_launch_attempted, display_path(artifact), "M273-D002-CACHE-06", "unexpected launch_attempted state", failures)
    passed += require(payload.get("cache_hit") is True, display_path(artifact), "M273-D002-CACHE-07", "cache_hit must be true", failures)
    passed += require(payload.get("host_process_exit_code") == 0, display_path(artifact), "M273-D002-CACHE-08", "host process exit code must be zero", failures)
    passed += require(payload.get("deterministic") is True, display_path(artifact), "M273-D002-CACHE-09", "deterministic must be true", failures)
    passed += require(isinstance(payload.get("cache_key"), str) and len(payload.get("cache_key")) == 16, display_path(artifact), "M273-D002-CACHE-10", "cache_key must be a 16-digit hex string", failures)
    passed += require(Path(ROOT / payload.get("cache_summary_relative_path", "")).exists(), display_path(artifact), "M273-D002-CACHE-11", "cache summary missing", failures)
    passed += require(Path(ROOT / payload.get("cache_runtime_import_surface_relative_path", "")).exists(), display_path(artifact), "M273-D002-CACHE-12", "cache runtime import surface missing", failures)
    passed += require(Path(ROOT / payload.get("cache_manifest_relative_path", "")).exists(), display_path(artifact), "M273-D002-CACHE-13", "cache manifest missing", failures)
    return total, passed


def validate_link_plan(payload: dict[str, Any], artifact: Path, failures: list[Finding]) -> tuple[int, int]:
    total = 13
    passed = 0
    passed += require(payload.get("expected_part10_host_cache_contract_id") == CONTRACT_ID, display_path(artifact), "M273-D002-LINK-01", "unexpected expected_part10_host_cache_contract_id", failures)
    passed += require(payload.get("expected_part10_host_cache_source_contract_id") == SOURCE_CONTRACT_ID, display_path(artifact), "M273-D002-LINK-02", "unexpected expected_part10_host_cache_source_contract_id", failures)
    passed += require(payload.get("expected_part10_host_cache_executable_relative_path") == RUNNER_RELATIVE_PATH, display_path(artifact), "M273-D002-LINK-03", "unexpected link-plan runner path", failures)
    passed += require(payload.get("expected_part10_host_cache_root_relative_path") == CACHE_ROOT_RELATIVE_PATH, display_path(artifact), "M273-D002-LINK-04", "unexpected link-plan cache root", failures)
    passed += require(payload.get("part10_host_cache_imported_module_count") == 1, display_path(artifact), "M273-D002-LINK-05", "unexpected imported module count", failures)
    passed += require(payload.get("part10_host_cache_imported_module_names_lexicographic") == [PROVIDER_MODULE], display_path(artifact), "M273-D002-LINK-06", "unexpected imported module names", failures)
    passed += require(payload.get("part10_host_cache_cross_module_preservation_ready") is True, display_path(artifact), "M273-D002-LINK-07", "cross-module preservation must be true", failures)
    imported_modules = payload.get("imported_modules")
    passed += require(isinstance(imported_modules, list) and len(imported_modules) == 1, display_path(artifact), "M273-D002-LINK-08", "expected one imported module record", failures)
    if isinstance(imported_modules, list) and imported_modules:
        imported = imported_modules[0]
        passed += require(imported.get("module_name") == PROVIDER_MODULE, display_path(artifact), "M273-D002-LINK-09", "unexpected imported module name", failures)
        passed += require(imported.get("part10_macro_host_process_cache_runtime_integration_present") is True, display_path(artifact), "M273-D002-LINK-10", "imported Part 10 host/cache surface missing", failures)
        passed += require(imported.get("part10_macro_host_process_cache_runtime_ready") is True, display_path(artifact), "M273-D002-LINK-11", "imported Part 10 host/cache readiness missing", failures)
        passed += require(imported.get("part10_macro_host_process_cache_host_executable_relative_path") == RUNNER_RELATIVE_PATH, display_path(artifact), "M273-D002-LINK-12", "imported runner path mismatch", failures)
        passed += require(imported.get("part10_macro_host_process_cache_root_relative_path") == CACHE_ROOT_RELATIVE_PATH, display_path(artifact), "M273-D002-LINK-13", "imported cache root mismatch", failures)
    else:
        for check_id in ("M273-D002-LINK-09", "M273-D002-LINK-10", "M273-D002-LINK-11", "M273-D002-LINK-12", "M273-D002-LINK-13"):
            failures.append(Finding(display_path(artifact), check_id, "imported module record unavailable"))
    return total, passed


def run_dynamic_checks(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    total = 0
    passed = 0
    payload: dict[str, Any] = {}

    build_total, build_passed, build_payload = ensure_native_build(failures)
    total += build_total
    passed += build_passed
    payload["ensure_native_build"] = build_payload
    if build_passed != build_total:
        return total, passed, payload

    backup_path = backup_existing_cache_root()
    payload["cache_backup"] = display_path(backup_path) if backup_path else None

    provider_first_dir = PROBE_ROOT / "provider_first"
    provider_second_dir = PROBE_ROOT / "provider_second"
    consumer_dir = PROBE_ROOT / "consumer"

    provider_first = compile_provider_with_native(provider_first_dir)
    provider_first_import = provider_first_dir / IMPORT_ARTIFACT
    provider_first_cache = provider_first_dir / CACHE_ARTIFACT
    provider_first_manifest = provider_first_dir / MANIFEST_ARTIFACT
    total += 4
    passed += require(provider_first.returncode == 0, display_path(PROVIDER_FIXTURE), "M273-D002-DYN-01", provider_first.stderr or provider_first.stdout or "provider first compile failed", failures)
    passed += require(provider_first_import.exists(), display_path(provider_first_import), "M273-D002-DYN-02", "provider first runtime-import surface missing", failures)
    passed += require(provider_first_cache.exists(), display_path(provider_first_cache), "M273-D002-DYN-03", "provider first cache sidecar missing", failures)
    passed += require(provider_first_manifest.exists(), display_path(provider_first_manifest), "M273-D002-DYN-04", "provider first manifest missing", failures)
    if provider_first.returncode != 0:
        return total, passed, payload

    provider_first_surface = extract_surface_member(load_json(provider_first_import), provider_first_import, failures)
    provider_first_cache_payload = load_json(provider_first_cache)
    if provider_first_surface is not None:
        imp_total, imp_passed = validate_import_surface(provider_first_surface, imported_module_count=0, local_macro_artifact_count=1, local_property_behavior_artifact_count=1, artifact=provider_first_import, failures=failures)
        total += imp_total
        passed += imp_passed
    cache_total, cache_passed = validate_cache_sidecar(provider_first_cache_payload, expect_launch_attempted=True, artifact=provider_first_cache, failures=failures)
    total += cache_total
    passed += cache_passed

    provider_second = compile_provider_with_native(provider_second_dir)
    provider_second_import = provider_second_dir / IMPORT_ARTIFACT
    provider_second_cache = provider_second_dir / CACHE_ARTIFACT
    total += 3
    passed += require(provider_second.returncode == 0, display_path(PROVIDER_FIXTURE), "M273-D002-DYN-05", provider_second.stderr or provider_second.stdout or "provider second compile failed", failures)
    passed += require(provider_second_cache.exists(), display_path(provider_second_cache), "M273-D002-DYN-06", "provider second cache sidecar missing", failures)
    passed += require(provider_second_import.exists(), display_path(provider_second_import), "M273-D002-DYN-07", "provider second runtime-import surface missing", failures)
    if provider_second.returncode == 0 and provider_second_cache.exists():
        provider_second_cache_payload = load_json(provider_second_cache)
        cache_total, cache_passed = validate_cache_sidecar(provider_second_cache_payload, expect_launch_attempted=False, artifact=provider_second_cache, failures=failures)
        total += cache_total
        passed += cache_passed
        total += 1
        passed += require(provider_second_cache_payload.get("cache_key") == provider_first_cache_payload.get("cache_key"), display_path(provider_second_cache), "M273-D002-DYN-08", "provider cache key changed across identical builds", failures)

    consumer = compile_consumer_with_native(consumer_dir, provider_first_import)
    consumer_import = consumer_dir / IMPORT_ARTIFACT
    consumer_cache = consumer_dir / CACHE_ARTIFACT
    consumer_link_plan = consumer_dir / LINK_PLAN_ARTIFACT
    total += 5
    passed += require(consumer.returncode == 0, display_path(CONSUMER_FIXTURE), "M273-D002-DYN-09", consumer.stderr or consumer.stdout or "consumer compile failed", failures)
    passed += require(consumer_import.exists(), display_path(consumer_import), "M273-D002-DYN-10", "consumer runtime-import surface missing", failures)
    passed += require(consumer_cache.exists(), display_path(consumer_cache), "M273-D002-DYN-11", "consumer cache sidecar missing", failures)
    passed += require(consumer_link_plan.exists(), display_path(consumer_link_plan), "M273-D002-DYN-12", "consumer link plan missing", failures)
    passed += require(RUNTIME_LIB.exists(), display_path(RUNTIME_LIB), "M273-D002-DYN-13", "runtime library missing", failures)
    if consumer.returncode == 0 and consumer_import.exists() and consumer_cache.exists() and consumer_link_plan.exists():
        consumer_surface = extract_surface_member(load_json(consumer_import), consumer_import, failures)
        consumer_cache_payload = load_json(consumer_cache)
        consumer_plan_payload = load_json(consumer_link_plan)
        if consumer_surface is not None:
            imp_total, imp_passed = validate_import_surface(consumer_surface, imported_module_count=1, local_macro_artifact_count=0, local_property_behavior_artifact_count=0, artifact=consumer_import, failures=failures)
            total += imp_total
            passed += imp_passed
        cache_total, cache_passed = validate_cache_sidecar(consumer_cache_payload, expect_launch_attempted=None, artifact=consumer_cache, failures=failures)
        total += cache_total
        passed += cache_passed
        link_total, link_passed = validate_link_plan(consumer_plan_payload, consumer_link_plan, failures)
        total += link_total
        passed += link_passed

    payload["provider_first"] = display_path(provider_first_dir)
    payload["provider_second"] = display_path(provider_second_dir)
    payload["consumer"] = display_path(consumer_dir)
    return total, passed, payload


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    static_summary: dict[str, Any] = {}
    for path, snippets in STATIC_SNIPPETS.items():
        path_total = len(snippets) + 1
        path_passed = ensure_snippets(path, snippets, failures)
        checks_total += path_total
        checks_passed += path_passed
        static_summary[display_path(path)] = {
            "checks": path_total,
            "passed": path_passed,
            "ok": path_passed == path_total,
        }

    dynamic_summary: dict[str, Any] = {"executed": False}
    if not args.skip_dynamic_probes:
        dynamic_summary["executed"] = True
        dynamic_total, dynamic_passed, dynamic_payload = run_dynamic_checks(failures)
        checks_total += dynamic_total
        checks_passed += dynamic_passed
        dynamic_summary["payload"] = dynamic_payload

    summary = {
        "mode": MODE,
        "issue": ISSUE,
        "issue_number": ISSUE_NUMBER,
        "contract_id": CONTRACT_ID,
        "source_contract_id": SOURCE_CONTRACT_ID,
        "preservation_contract_id": PRESERVATION_CONTRACT_ID,
        "surface_key": SURFACE_KEY,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "checks_failed": len(failures),
        "ok": not failures,
        "implementation_ready": dynamic_summary["executed"] and not failures,
        "dynamic_probes_executed": dynamic_summary["executed"],
        "static_contracts": static_summary,
        "dynamic": dynamic_summary,
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
