#!/usr/bin/env python3
"""Checker for M265-D003 cross-module type-surface preservation hardening."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
import time
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m265" / "M265-D003" / "cross_module_type_surface_preservation_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m265_cross_module_type_surface_preservation_hardening_edge_case_and_compatibility_completion_d003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m265" / "m265_d003_cross_module_type_surface_preservation_hardening_edge_case_and_compatibility_completion_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_LOWERING = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
RUNTIME_README = ROOT / "native" / "objc3c" / "src" / "runtime" / "README.md"
PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
RUNTIME_INTERNAL_H = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
FRONTEND_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
RUNTIME_IMPORT_SURFACE_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.h"
RUNTIME_IMPORT_SURFACE_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_runtime_import_surface.cpp"
FRONTEND_TYPES_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
PACKAGE_JSON = ROOT / "package.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
PROVIDER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_cross_module_type_surface_provider.objc3"
CONSUMER_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m258_d002_runtime_packaging_consumer.objc3"
PROBE_CPP = ROOT / "tests" / "tooling" / "runtime" / "m265_d002_keypath_runtime_probe.cpp"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m265" / "d003"
CONTRACT_ID = "objc3c-part3-cross-module-type-surface-preservation/m265-d003-v1"
IMPORTED_SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_imported_runtime_metadata_semantic_rules"
ORCHESTRATION_PATH = "frontend.pipeline.semantic_surface.objc_cross_module_build_runtime_orchestration_contract"
LOWERING_CONTRACT_ID = "objc3c-part3-optional-keypath-lowering/m265-c001-v1"
RUNTIME_HELPER_CONTRACT_ID = "objc3c-part3-optional-keypath-runtime-helper-contract/m265-d001-v1"


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    parser.add_argument("--native-exe", type=Path, default=NATIVE_EXE)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M265-D003-MISSING", f"missing artifact: {display_path(path)}"))
        return 0
    text = read_text(path)
    passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def run_command(command: Sequence[str], cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=ROOT if cwd is None else cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected object JSON in {display_path(path)}")
    return payload


def resolve_tool(names: Sequence[str], fallback: str) -> str:
    for name in names:
        resolved = shutil.which(name)
        if resolved and Path(resolved).exists():
            return resolved
    return fallback


def resolve_clang() -> str:
    return resolve_tool(("clang.exe", "clang"), r"C:\Program Files\LLVM\bin\clang.exe")


def resolve_clangxx() -> str:
    return resolve_tool(("clang++.exe", "clang++"), r"C:\Program Files\LLVM\bin\clang++.exe")


def load_runtime_library_path(registration_manifest_path: Path) -> Path | None:
    registration_manifest = load_json(registration_manifest_path)
    relative_path = registration_manifest.get("runtime_support_library_archive_relative_path")
    if not isinstance(relative_path, str) or not relative_path.strip():
        return None
    candidate = (ROOT / relative_path).resolve()
    return candidate if candidate.exists() else None


def move_existing_probe_dir(out_dir: Path) -> None:
    if not out_dir.exists():
        return
    backup_root = ROOT / "tmp" / "reports" / "m265" / "M265-D003" / "backups"
    backup_root.mkdir(parents=True, exist_ok=True)
    stamp = int(time.time() * 1000)
    backup_dir = backup_root / f"{out_dir.name}-{stamp}"
    shutil.move(str(out_dir), str(backup_dir))


def validate_empty_diagnostics(path: Path, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    payload = load_json(path)
    diagnostics = payload.get("diagnostics")
    checks_total += 1
    checks_passed += require(isinstance(diagnostics, list), display_path(path), "M265-D003-DIAG-schema", "diagnostics payload must be a list", failures)
    if isinstance(diagnostics, list):
        checks_total += 1
        checks_passed += require(len(diagnostics) == 0, display_path(path), "M265-D003-DIAG-empty", "expected zero diagnostics", failures)
    return checks_total, checks_passed


def ensure_fast_build(failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    build_result = run_command([sys.executable, "scripts/ensure_objc3c_native_build.py", "--mode", "fast"])
    checks_total += 1
    checks_passed += require(build_result.returncode == 0, "scripts/ensure_objc3c_native_build.py", "M265-D003-BUILD", f"fast build failed: {build_result.stderr or build_result.stdout}", failures)
    return checks_total, checks_passed


def compile_module(fixture: Path, out_dir: Path, ordinal: int, imported_surface: Path | None, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    evidence: dict[str, Any] = {}
    move_existing_probe_dir(out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [str(NATIVE_EXE), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module", "--objc3-bootstrap-registration-order-ordinal", str(ordinal)]
    if imported_surface is not None:
        command.extend(["--objc3-import-runtime-surface", str(imported_surface)])
    result = run_command(command)
    checks_total += 1
    checks_passed += require(result.returncode == 0, display_path(fixture), f"M265-D003-{out_dir.name}-compile", f"compile failed: {result.stderr or result.stdout}", failures)
    manifest_path = out_dir / "module.manifest.json"
    import_surface_path = out_dir / "module.runtime-import-surface.json"
    diagnostics_path = out_dir / "module.diagnostics.json"
    object_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"
    reg_manifest_path = out_dir / "module.runtime-registration-manifest.json"
    link_plan_path = out_dir / "module.cross-module-runtime-link-plan.json"
    link_rsp_path = out_dir / "module.cross-module-runtime-linker-options.rsp"
    for label, artifact in (("manifest", manifest_path), ("object", object_path), ("backend", backend_path), ("diagnostics", diagnostics_path), ("registration", reg_manifest_path)):
        checks_total += 1
        checks_passed += require(artifact.exists(), display_path(artifact), f"M265-D003-{out_dir.name}-{label}", f"missing artifact: {display_path(artifact)}", failures)
    if import_surface_path.exists() or imported_surface is None:
        checks_total += 1
        checks_passed += require(import_surface_path.exists(), display_path(import_surface_path), f"M265-D003-{out_dir.name}-import-surface", f"missing artifact: {display_path(import_surface_path)}", failures)
    if link_plan_path.exists() or imported_surface is not None:
        checks_total += 1
        checks_passed += require(link_plan_path.exists(), display_path(link_plan_path), f"M265-D003-{out_dir.name}-link-plan", f"missing artifact: {display_path(link_plan_path)}", failures)
        checks_total += 1
        checks_passed += require(link_rsp_path.exists(), display_path(link_rsp_path), f"M265-D003-{out_dir.name}-link-rsp", f"missing artifact: {display_path(link_rsp_path)}", failures)
    if diagnostics_path.exists():
        sub_total, sub_passed = validate_empty_diagnostics(diagnostics_path, failures)
        checks_total += sub_total
        checks_passed += sub_passed
    if backend_path.exists():
        checks_total += 1
        checks_passed += require(backend_path.read_text(encoding="utf-8").strip() == "llvm-direct", display_path(backend_path), f"M265-D003-{out_dir.name}-backend-value", "expected llvm-direct object backend", failures)
    if manifest_path.exists():
        evidence["manifest"] = load_json(manifest_path)
    if import_surface_path.exists():
        evidence["import_surface"] = load_json(import_surface_path)
    if link_plan_path.exists():
        evidence["link_plan"] = load_json(link_plan_path)
    return checks_total, checks_passed, evidence


def lookup_path(payload: dict[str, Any], path: Sequence[str]) -> Any:
    current: Any = payload
    for key in path:
        if not isinstance(current, dict):
            return None
        current = current.get(key)
    return current


def run_cross_module_probe(provider_dir: Path, consumer_dir: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    evidence: dict[str, Any] = {}
    provider_obj = provider_dir / "module.obj"
    consumer_obj = consumer_dir / "module.obj"
    link_rsp = consumer_dir / "module.cross-module-runtime-linker-options.rsp"
    registration_manifest = consumer_dir / "module.runtime-registration-manifest.json"
    checks_total += 1
    checks_passed += require(registration_manifest.exists(), display_path(registration_manifest), "M265-D003-probe-registration-manifest", "missing consumer registration manifest", failures)
    if not registration_manifest.exists():
        return checks_total, checks_passed, evidence
    runtime_library = load_runtime_library_path(registration_manifest)
    probe_exe = consumer_dir / "m265_d003_cross_module_probe.exe"
    if runtime_library is None:
        checks_total += 1
        checks_passed += require(False, display_path(registration_manifest), "M265-D003-probe-runtime-lib", "could not resolve runtime support library path", failures)
        return checks_total, checks_passed, evidence
    link_result = run_command([
        resolve_clangxx(),
        f"-I{(ROOT / 'native' / 'objc3c' / 'src').resolve()}",
        str(PROBE_CPP),
        str(provider_obj),
        str(consumer_obj),
        str(runtime_library),
        f"@{link_rsp.resolve()}",
        "-o",
        str(probe_exe),
    ], cwd=consumer_dir)
    checks_total += 1
    checks_passed += require(link_result.returncode == 0 and probe_exe.exists(), display_path(probe_exe), "M265-D003-probe-link", f"cross-module probe link failed: {link_result.stderr or link_result.stdout}", failures)
    if probe_exe.exists():
        run_result = run_command([str(probe_exe)], cwd=consumer_dir)
        checks_total += 1
        checks_passed += require(run_result.returncode == 0, display_path(probe_exe), "M265-D003-probe-run", f"probe execution failed: {run_result.stderr or run_result.stdout}", failures)
        if run_result.returncode == 0:
            payload = json.loads(run_result.stdout)
            if not isinstance(payload, dict):
                raise TypeError("cross-module probe payload must be an object")
            evidence["probe"] = payload
            for key, expected in {
                "entry_found": 1,
                "entry_ambiguous": 0,
                "entry_component_count": 1,
                "entry_metadata_provider_count": 1,
                "component_count_helper": 1,
                "root_is_self_helper": 0,
                "missing_found": 0,
                "root_name": "Person",
                "component_path": "name",
                "profile_present": 1,
                "generic_metadata_replay_key_present": 1,
            }.items():
                checks_total += 1
                checks_passed += require(payload.get(key) == expected, display_path(probe_exe), f"M265-D003-probe-{key}", f"{key} mismatch", failures)
            checks_total += 1
            checks_passed += require(int(payload.get("image_backed_keypath_count", 0)) >= 1, display_path(probe_exe), "M265-D003-probe-image-count", "expected at least one image-backed keypath", failures)
    return checks_total, checks_passed, evidence


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0
    evidence: dict[str, Any] = {}

    static_checks = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M265-D003-EXP-01", "# M265-D003 Expectations"),
            SnippetCheck("M265-D003-EXP-02", CONTRACT_ID),
            SnippetCheck("M265-D003-EXP-03", "cross-module"),
            SnippetCheck("M265-D003-EXP-04", "typed key-path runtime metadata survives module boundaries"),
        ],
        PACKET_DOC: [
            SnippetCheck("M265-D003-PKT-01", "# M265-D003 Packet"),
            SnippetCheck("M265-D003-PKT-02", "objc_part3_optional_keypath_runtime_helper_contract"),
            SnippetCheck("M265-D003-PKT-03", "module.cross-module-runtime-link-plan.json"),
        ],
        DOC_SOURCE: [
            SnippetCheck("M265-D003-DOCSRC-01", "Current cross-module preservation boundary (`M265-D003`):"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M265-D003-DOCN-01", "Current cross-module preservation boundary (`M265-D003`):"),
        ],
        SPEC_AM: [
            SnippetCheck("M265-D003-AM-01", "Imported runtime surfaces now preserve the live optional/key-path boundary"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M265-D003-ATTR-01", "Cross-module imports preserve optional/key-path runtime packets"),
        ],
        SPEC_LOWERING: [
            SnippetCheck("M265-D003-LOW-01", "M265-D003 cross-module type-surface preservation anchor"),
        ],
        ARCHITECTURE_DOC: [
            SnippetCheck("M265-D003-ARCH-01", "## M265 cross-module type-surface preservation hardening (D003)"),
        ],
        RUNTIME_README: [
            SnippetCheck("M265-D003-RUN-01", "`M265-D003` extends that proof across imported runtime surfaces"),
        ],
        PROCESS_CPP: [
            SnippetCheck("M265-D003-PROC-01", "M265-D003 cross-module type-surface preservation anchor"),
        ],
        RUNTIME_INTERNAL_H: [
            SnippetCheck("M265-D003-RINT-01", "M265-D003 cross-module type-surface preservation anchor"),
        ],
        RUNTIME_CPP: [
            SnippetCheck("M265-D003-RCPP-01", "M265-D003 cross-module type-surface preservation anchor"),
        ],
        FRONTEND_ARTIFACTS_CPP: [
            SnippetCheck("M265-D003-FA-01", "source_imported_runtime_metadata_semantic_rules_contract_id"),
            SnippetCheck("M265-D003-FA-02", "objc_part3_optional_keypath_runtime_helper_contract"),
        ],
        RUNTIME_IMPORT_SURFACE_H: [
            SnippetCheck("M265-D003-RIH-01", "part3_optional_keypath_lowering_contract_present"),
        ],
        RUNTIME_IMPORT_SURFACE_CPP: [
            SnippetCheck("M265-D003-RICPP-01", "PopulateImportedPart3OptionalKeypathSurface"),
        ],
        FRONTEND_TYPES_H: [
            SnippetCheck("M265-D003-FT-01", "imported_optional_send_site_count"),
            SnippetCheck("M265-D003-FT-02", "imported_part3_optional_keypath_module_count"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M265-D003-PKG-01", '"check:objc3c:m265-d003-cross-module-type-surface-preservation-hardening-edge-case-and-compatibility-completion"'),
            SnippetCheck("M265-D003-PKG-02", '"test:tooling:m265-d003-cross-module-type-surface-preservation-hardening-edge-case-and-compatibility-completion"'),
            SnippetCheck("M265-D003-PKG-03", '"check:objc3c:m265-d003-lane-d-readiness"'),
        ],
    }
    for path, snippets in static_checks.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    if args.skip_dynamic_probes:
        args.summary_out.parent.mkdir(parents=True, exist_ok=True)
        summary = {
            "ok": not failures,
            "issue": "M265-D003",
            "checks_total": checks_total,
            "checks_passed": checks_passed,
            "dynamic_probes_executed": False,
            "failures": [asdict(finding) for finding in failures],
            "evidence": evidence,
        }
        args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
        if failures:
            print(f"[fail] M265-D003 static validation failed ({checks_passed}/{checks_total} checks)")
            return 1
        print(f"[ok] M265-D003 static validation passed ({checks_passed}/{checks_total} checks)")
        return 0

    sub_total, sub_passed = ensure_fast_build(failures)
    checks_total += sub_total
    checks_passed += sub_passed
    checks_total += 1
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M265-D003-native-exe", "native driver missing after fast build", failures)

    provider_dir = PROBE_ROOT / "provider"
    consumer_dir = PROBE_ROOT / "consumer"
    sub_total, sub_passed, provider_evidence = compile_module(PROVIDER_FIXTURE, provider_dir, 1, None, failures)
    checks_total += sub_total
    checks_passed += sub_passed
    evidence["provider"] = provider_evidence

    provider_import_surface = provider_dir / "module.runtime-import-surface.json"
    sub_total, sub_passed, consumer_evidence = compile_module(CONSUMER_FIXTURE, consumer_dir, 2, provider_import_surface, failures)
    checks_total += sub_total
    checks_passed += sub_passed
    evidence["consumer"] = consumer_evidence

    provider_import_payload = provider_evidence.get("import_surface") if isinstance(provider_evidence, dict) else None
    if isinstance(provider_import_payload, dict):
        lowering_packet = provider_import_payload.get("objc_part3_optional_keypath_lowering_contract")
        runtime_packet = provider_import_payload.get("objc_part3_optional_keypath_runtime_helper_contract")
        checks_total += 1
        checks_passed += require(isinstance(lowering_packet, dict), display_path(provider_import_surface), "M265-D003-provider-lowering-packet", "provider import surface missing lowering packet", failures)
        checks_total += 1
        checks_passed += require(isinstance(runtime_packet, dict), display_path(provider_import_surface), "M265-D003-provider-runtime-packet", "provider import surface missing runtime helper packet", failures)
        if isinstance(lowering_packet, dict):
            for key, expected in {
                "contract_id": LOWERING_CONTRACT_ID,
                "optional_send_sites": 0,
                "typed_keypath_literal_sites": 1,
                "live_optional_lowering_sites": 0,
                "live_typed_keypath_artifact_sites": 1,
                "ready_for_native_optional_lowering": True,
            }.items():
                checks_total += 1
                checks_passed += require(lowering_packet.get(key) == expected, display_path(provider_import_surface), f"M265-D003-provider-lowering-{key}", f"provider lowering packet {key} mismatch", failures)
        if isinstance(runtime_packet, dict):
            for key, expected in {
                "contract_id": RUNTIME_HELPER_CONTRACT_ID,
                "optional_send_runtime_ready": True,
                "typed_keypath_descriptor_handles_ready": True,
                "typed_keypath_runtime_execution_helper_landed": True,
            }.items():
                checks_total += 1
                checks_passed += require(runtime_packet.get(key) == expected, display_path(provider_import_surface), f"M265-D003-provider-runtime-{key}", f"provider runtime packet {key} mismatch", failures)

    consumer_manifest = lookup_path(consumer_evidence.get("manifest", {}), ("frontend", "pipeline", "semantic_surface", "objc_imported_runtime_metadata_semantic_rules"))
    checks_total += 1
    checks_passed += require(isinstance(consumer_manifest, dict), display_path(consumer_dir / "module.manifest.json"), "M265-D003-consumer-imported-summary", f"missing {IMPORTED_SURFACE_PATH}", failures)
    if isinstance(consumer_manifest, dict):
        for key, expected in {
            "imported_part3_optional_keypath_module_count": 1,
            "optional_send_site_count": 0,
            "typed_keypath_literal_site_count": 1,
            "live_optional_lowering_site_count": 0,
            "live_typed_keypath_artifact_site_count": 1,
            "imported_optional_runtime_ready_module_count": 1,
            "imported_typed_keypath_runtime_ready_module_count": 1,
            "imported_part3_type_surface_landed": True,
            "imported_optional_runtime_semantics_landed": True,
            "imported_typed_keypath_runtime_semantics_landed": True,
        }.items():
            checks_total += 1
            checks_passed += require(consumer_manifest.get(key) == expected, display_path(consumer_dir / "module.manifest.json"), f"M265-D003-consumer-{key}", f"consumer imported summary {key} mismatch", failures)

    orchestration_manifest = lookup_path(consumer_evidence.get("manifest", {}), ("frontend", "pipeline", "semantic_surface", "objc_cross_module_build_runtime_orchestration_contract"))
    checks_total += 1
    checks_passed += require(isinstance(orchestration_manifest, dict), display_path(consumer_dir / "module.manifest.json"), "M265-D003-consumer-orchestration-summary", f"missing {ORCHESTRATION_PATH}", failures)
    if isinstance(orchestration_manifest, dict):
        for key, expected in {
            "source_imported_runtime_metadata_semantic_rules_ready": True,
            "imported_optional_send_site_count": 0,
            "imported_typed_keypath_literal_site_count": 1,
            "imported_live_optional_lowering_site_count": 0,
            "imported_live_typed_keypath_artifact_site_count": 1,
            "imported_part3_type_surface_landed": True,
            "imported_optional_runtime_semantics_landed": True,
            "imported_typed_keypath_runtime_semantics_landed": True,
        }.items():
            checks_total += 1
            checks_passed += require(orchestration_manifest.get(key) == expected, display_path(consumer_dir / "module.manifest.json"), f"M265-D003-orchestration-{key}", f"cross-module orchestration {key} mismatch", failures)

    link_plan = consumer_evidence.get("link_plan") if isinstance(consumer_evidence, dict) else None
    checks_total += 1
    checks_passed += require(isinstance(link_plan, dict), display_path(consumer_dir / "module.cross-module-runtime-link-plan.json"), "M265-D003-link-plan", "missing link-plan payload", failures)
    if isinstance(link_plan, dict):
        checks_total += 1
        checks_passed += require(link_plan.get("module_image_count") == 2, display_path(consumer_dir / "module.cross-module-runtime-link-plan.json"), "M265-D003-link-plan-image-count", "expected two images in link plan", failures)
        modules = link_plan.get("module_names_lexicographic")
        checks_total += 1
        checks_passed += require(isinstance(modules, list) and "typedKeyPathRuntimeModule" in modules and "runtimePackagingConsumer" in modules, display_path(consumer_dir / "module.cross-module-runtime-link-plan.json"), "M265-D003-link-plan-modules", "unexpected module names in link plan", failures)

    sub_total, sub_passed, probe_evidence = run_cross_module_probe(provider_dir, consumer_dir, failures)
    checks_total += sub_total
    checks_passed += sub_passed
    evidence["probe"] = probe_evidence

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "ok": not failures,
        "issue": "M265-D003",
        "contract_id": CONTRACT_ID,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": True,
        "failures": [asdict(finding) for finding in failures],
        "evidence": evidence,
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        print(f"[fail] M265-D003 validation failed ({checks_passed}/{checks_total} checks)")
        return 1
    print(f"[ok] M265-D003 validated ({checks_passed}/{checks_total} checks)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
