#!/usr/bin/env python3
"""Checker for M265-D002 live optional-send and key-path runtime support."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m265" / "M265-D002" / "live_optional_send_and_keypath_runtime_support_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m265_live_optional_send_and_key_path_runtime_support_core_feature_implementation_d002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m265" / "m265_d002_live_optional_send_and_key_path_runtime_support_core_feature_implementation_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_LOWERING = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
RUNTIME_README = ROOT / "native" / "objc3c" / "src" / "runtime" / "README.md"
LOWERING_H = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
FRONTEND_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
RUNTIME_INTERNAL_H = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
PACKAGE_JSON = ROOT / "package.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
OPTIONAL_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_optional_member_access_runtime_positive.objc3"
KEYPATH_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_typed_keypath_runtime_positive.objc3"
MODULE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_typed_keypath_runtime_module.objc3"
PROBE_CPP = ROOT / "tests" / "tooling" / "runtime" / "m265_d002_keypath_runtime_probe.cpp"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m265" / "d002"
CONTRACT_ID = "objc3c-part3-optional-keypath-runtime-live-support/m265-d002-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_part3_optional_keypath_runtime_helper_contract"
RUNTIME_SURFACE_CONTRACT_ID = "objc3c-part3-optional-keypath-runtime-helper-contract/m265-d001-v1"


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
        failures.append(Finding(display_path(path), "M265-D002-MISSING", f"missing artifact: {display_path(path)}"))
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


def validate_empty_diagnostics(path: Path, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    payload = load_json(path)
    diagnostics = payload.get("diagnostics")
    checks_total += 1
    checks_passed += require(isinstance(diagnostics, list), display_path(path), "M265-D002-DIAG-schema", "diagnostics payload must be a list", failures)
    if isinstance(diagnostics, list):
        checks_total += 1
        checks_passed += require(len(diagnostics) == 0, display_path(path), "M265-D002-DIAG-empty", "expected zero diagnostics", failures)
    return checks_total, checks_passed


def runtime_helper_packet(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    packet = manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {}).get("objc_part3_optional_keypath_runtime_helper_contract")
    if not isinstance(packet, dict):
        raise TypeError(f"missing {SURFACE_PATH} in {display_path(manifest_path)}")
    return packet


def compile_fixture(name: str, fixture: Path, failures: list[Finding]) -> tuple[int, int, Path, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    evidence: dict[str, Any] = {}
    out_dir = PROBE_ROOT / name
    out_dir.mkdir(parents=True, exist_ok=True)
    compile_result = run_command([str(NATIVE_EXE), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module"])
    checks_total += 1
    checks_passed += require(compile_result.returncode == 0, display_path(fixture), f"M265-D002-{name}-compile", f"compile failed: {compile_result.stderr or compile_result.stdout}", failures)
    artifacts = (
        out_dir / "module.manifest.json",
        out_dir / "module.ll",
        out_dir / "module.obj",
        out_dir / "module.object-backend.txt",
        out_dir / "module.diagnostics.json",
        out_dir / "module.runtime-registration-manifest.json",
        out_dir / "module.runtime-metadata-linker-options.rsp",
    )
    for artifact in artifacts:
        checks_total += 1
        checks_passed += require(artifact.exists(), display_path(artifact), f"M265-D002-{name}-artifact", f"missing artifact: {display_path(artifact)}", failures)
    diag_path = out_dir / "module.diagnostics.json"
    if diag_path.exists():
        sub_total, sub_passed = validate_empty_diagnostics(diag_path, failures)
        checks_total += sub_total
        checks_passed += sub_passed
    backend_path = out_dir / "module.object-backend.txt"
    if backend_path.exists():
        checks_total += 1
        checks_passed += require(backend_path.read_text(encoding="utf-8").strip() == "llvm-direct", display_path(backend_path), f"M265-D002-{name}-backend", "expected llvm-direct object backend", failures)
    manifest_path = out_dir / "module.manifest.json"
    if manifest_path.exists():
        packet = runtime_helper_packet(manifest_path)
        for key, expected in {
            "contract_id": RUNTIME_SURFACE_CONTRACT_ID,
            "frontend_surface_path": SURFACE_PATH,
            "public_lookup_selector_symbol": "objc3_runtime_lookup_selector",
            "public_dispatch_i32_symbol": "objc3_runtime_dispatch_i32",
            "keypath_descriptor_section": "objc3.runtime.keypath_descriptors",
            "runtime_library_consumption_ready": True,
            "optional_send_runtime_ready": True,
            "typed_keypath_descriptor_handles_ready": True,
            "typed_keypath_runtime_execution_helper_landed": True,
            "diagnostic_fallback_ready": True,
        }.items():
            checks_total += 1
            checks_passed += require(packet.get(key) == expected, display_path(manifest_path), f"M265-D002-{name}-{key}", f"{key} mismatch", failures)
        evidence["runtime_helper_packet"] = packet
    ir_path = out_dir / "module.ll"
    if ir_path.exists():
        ir_text = ir_path.read_text(encoding="utf-8")
        checks_total += 1
        checks_passed += require("typed_keypath_runtime_execution_helper_landed=true" in ir_text, display_path(ir_path), f"M265-D002-{name}-ir-helper", "runtime helper IR summary missing live keypath flag", failures)
        if name == "optional":
            checks_total += 1
            checks_passed += require("@objc3_runtime_dispatch_i32" in ir_text, display_path(ir_path), "M265-D002-optional-ir-dispatch", "optional runtime dispatch symbol missing", failures)
        if name in {"keypath", "probe-module"}:
            checks_total += 1
            checks_passed += require("@__objc3_sec_keypath_descriptors" in ir_text, display_path(ir_path), f"M265-D002-{name}-ir-keypath", "keypath descriptor aggregate missing", failures)
    return checks_total, checks_passed, out_dir, evidence


def link_executable(out_dir: Path, stem: str = "module") -> tuple[Path | None, subprocess.CompletedProcess[str] | None]:
    obj_path = out_dir / f"{stem}.obj"
    rsp_path = out_dir / f"{stem}.runtime-metadata-linker-options.rsp"
    registration_manifest_path = out_dir / f"{stem}.runtime-registration-manifest.json"
    exe_path = out_dir / f"{stem}.exe"
    if not obj_path.exists() or not rsp_path.exists() or not registration_manifest_path.exists():
        return None, None
    runtime_library_path = load_runtime_library_path(registration_manifest_path)
    if runtime_library_path is None:
        return None, None
    result = run_command([resolve_clang(), str(obj_path), str(runtime_library_path), f"@{rsp_path.resolve()}", "-o", str(exe_path)], cwd=out_dir)
    if result.returncode != 0 or not exe_path.exists():
        return None, result
    return exe_path, result


def run_fixture_executable(name: str, out_dir: Path, expected_exit_code: int, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    exe_path, link_result = link_executable(out_dir)
    checks_total += 1
    checks_passed += require(exe_path is not None, display_path(out_dir), f"M265-D002-{name}-link", f"link failed: {(link_result.stderr or link_result.stdout) if link_result else 'missing linker inputs'}", failures)
    if exe_path is not None:
        run_result = run_command([str(exe_path)], cwd=out_dir)
        checks_total += 1
        checks_passed += require(run_result.returncode == expected_exit_code, display_path(exe_path), f"M265-D002-{name}-run", f"expected exit code {expected_exit_code}, got {run_result.returncode}", failures)
    return checks_total, checks_passed


def run_probe(module_out_dir: Path, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    evidence: dict[str, Any] = {}
    obj_path = module_out_dir / "module.obj"
    rsp_path = module_out_dir / "module.runtime-metadata-linker-options.rsp"
    registration_manifest_path = module_out_dir / "module.runtime-registration-manifest.json"
    probe_exe = module_out_dir / "m265_d002_keypath_runtime_probe.exe"
    runtime_library_path = load_runtime_library_path(registration_manifest_path)
    checks_total += 1
    checks_passed += require(runtime_library_path is not None, display_path(registration_manifest_path), "M265-D002-probe-runtime-lib", "runtime support library missing", failures)
    if runtime_library_path is None:
        return checks_total, checks_passed, evidence
    compile_result = run_command([
        resolve_clangxx(),
        str(PROBE_CPP),
        str(obj_path),
        str(runtime_library_path),
        f"@{rsp_path.resolve()}",
        "-I",
        str((ROOT / "native" / "objc3c" / "src").resolve()),
        "-o",
        str(probe_exe),
    ], cwd=module_out_dir)
    checks_total += 1
    checks_passed += require(compile_result.returncode == 0 and probe_exe.exists(), display_path(PROBE_CPP), "M265-D002-probe-compile", f"probe compile failed: {compile_result.stderr or compile_result.stdout}", failures)
    if not probe_exe.exists():
        return checks_total, checks_passed, evidence
    run_result = run_command([str(probe_exe)], cwd=module_out_dir)
    checks_total += 1
    checks_passed += require(run_result.returncode == 0, display_path(probe_exe), "M265-D002-probe-run", f"probe failed: {run_result.stderr or run_result.stdout}", failures)
    if run_result.returncode != 0:
        return checks_total, checks_passed, evidence
    try:
        payload = json.loads(run_result.stdout)
    except json.JSONDecodeError as exc:
        failures.append(Finding(display_path(probe_exe), "M265-D002-probe-json", f"invalid probe JSON: {exc}"))
        return checks_total + 1, checks_passed, evidence
    evidence["probe"] = payload
    for key, expected in {
        "keypath_table_entry_count": 1,
        "image_backed_keypath_count": 1,
        "ambiguous_keypath_handle_count": 0,
        "last_materialized_handle": 1,
        "entry_found": 1,
        "entry_ambiguous": 0,
        "entry_root_is_self": 0,
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
        checks_passed += require(payload.get(key) == expected, display_path(probe_exe), f"M265-D002-probe-{key}", f"{key} mismatch", failures)
    return checks_total, checks_passed, evidence


def run_dynamic_probes(failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    evidence: dict[str, Any] = {}

    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m265-d002-readiness",
        "--summary-out",
        "tmp/reports/m265/M265-D002/ensure_build_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M265-D002-build", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    checks_total += 1
    checks_passed += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M265-D002-native-driver", "native driver missing after build", failures)

    optional_total, optional_passed, optional_out_dir, optional_evidence = compile_fixture("optional", OPTIONAL_FIXTURE, failures)
    checks_total += optional_total
    checks_passed += optional_passed
    run_total, run_passed = run_fixture_executable("optional", optional_out_dir, 9, failures)
    checks_total += run_total
    checks_passed += run_passed
    evidence["optional"] = optional_evidence

    keypath_total, keypath_passed, keypath_out_dir, keypath_evidence = compile_fixture("keypath", KEYPATH_FIXTURE, failures)
    checks_total += keypath_total
    checks_passed += keypath_passed
    run_total, run_passed = run_fixture_executable("keypath", keypath_out_dir, 11, failures)
    checks_total += run_total
    checks_passed += run_passed
    evidence["keypath"] = keypath_evidence

    module_total, module_passed, module_out_dir, module_evidence = compile_fixture("probe-module", MODULE_FIXTURE, failures)
    checks_total += module_total
    checks_passed += module_passed
    evidence["probe_module"] = module_evidence

    probe_total, probe_passed, probe_evidence = run_probe(module_out_dir, failures)
    checks_total += probe_total
    checks_passed += probe_passed
    evidence.update(probe_evidence)

    return checks_total, checks_passed, evidence


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippet_map: dict[Path, list[SnippetCheck]] = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M265-D002-EXP-01", "# M265-D002 Expectations"),
            SnippetCheck("M265-D002-EXP-02", CONTRACT_ID),
            SnippetCheck("M265-D002-EXP-03", "typed_keypath_runtime_execution_helper_landed = true"),
        ],
        PACKET_DOC: [
            SnippetCheck("M265-D002-PKT-01", "# M265-D002 Packet"),
            SnippetCheck("M265-D002-PKT-02", "private runtime registry"),
        ],
        LOWERING_H: [
            SnippetCheck("M265-D002-H-01", "kObjc3RuntimeBootstrapRegistrationTableAbiVersion ="),
            SnippetCheck("M265-D002-H-02", "kObjc3RuntimeBootstrapRegistrationTablePointerFieldCount = 12u"),
        ],
        LOWERING_CPP: [
            SnippetCheck("M265-D002-CPP-01", "M265-D002 live-optional-send-and-keypath-runtime-support anchor"),
            SnippetCheck("M265-D002-CPP-02", "typed_keypath_runtime_execution_helper_landed=true"),
        ],
        FRONTEND_ARTIFACTS_CPP: [
            SnippetCheck("M265-D002-FA-01", "typed_keypath_runtime_execution_helper_landed = true;"),
        ],
        IR_CPP: [
            SnippetCheck("M265-D002-IR-01", "keypath_descriptor_root_symbol"),
            SnippetCheck("M265-D002-IR-02", "i32 13"),
        ],
        PROCESS_CPP: [
            SnippetCheck("M265-D002-PROC-01", "M265-D002 live-optional-send-and-keypath-runtime-support anchor"),
        ],
        RUNTIME_INTERNAL_H: [
            SnippetCheck("M265-D002-RIH-01", "const objc3_runtime_pointer_aggregate *keypath_descriptor_root;"),
            SnippetCheck("M265-D002-RIH-02", "typedef struct objc3_runtime_keypath_registry_state_snapshot"),
            SnippetCheck("M265-D002-RIH-03", "int objc3_runtime_copy_keypath_registry_state_for_testing("),
            SnippetCheck("M265-D002-RIH-04", "int objc3_runtime_keypath_component_count_for_testing(int keypath_handle);"),
        ],
        RUNTIME_CPP: [
            SnippetCheck("M265-D002-RCPP-01", "registration_table->abi_version != 2"),
            SnippetCheck("M265-D002-RCPP-02", "registration_table->pointer_field_count != 12"),
            SnippetCheck("M265-D002-RCPP-03", "AggregateCount(registration_table->keypath_descriptor_root)"),
            SnippetCheck("M265-D002-RCPP-04", "int objc3_runtime_copy_keypath_registry_state_for_testing("),
            SnippetCheck("M265-D002-RCPP-05", "int objc3_runtime_keypath_component_count_for_testing(int keypath_handle)"),
        ],
        DOC_SOURCE: [
            SnippetCheck("M265-D002-DOCSRC-01", "Current runtime-helper boundary (`M265-D002`):"),
            SnippetCheck("M265-D002-DOCSRC-02", "private runtime registry"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M265-D002-DOCN-01", "Current runtime-helper boundary (`M265-D002`):"),
            SnippetCheck("M265-D002-DOCN-02", "private runtime registry"),
        ],
        SPEC_AM: [
            SnippetCheck("M265-D002-AM-01", "single-component typed key-path handles now feed a private runtime registry"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M265-D002-ATTR-01", "handles into a private runtime registry/helper surface"),
        ],
        SPEC_LOWERING: [
            SnippetCheck("M265-D002-SPECLOW-01", "selector-string-pools-keypath-descriptors-image-local-init-state"),
            SnippetCheck("M265-D002-SPECLOW-02", "registration-table ABI version `2`"),
            SnippetCheck("M265-D002-SPECLOW-03", "registration-table pointer-field count `12`"),
        ],
        ARCHITECTURE_DOC: [
            SnippetCheck("M265-D002-ARCH-01", "## M265 live optional-send and key-path runtime support (D002)"),
            SnippetCheck("M265-D002-ARCH-02", "private runtime-owned key-path registry support"),
        ],
        RUNTIME_README: [
            SnippetCheck("M265-D002-RREADME-01", "`M265-D002` keeps that runtime helper boundary narrow while landing the first"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M265-D002-PKG-01", "\"check:objc3c:m265-d002-live-optional-send-and-key-path-runtime-support-core-feature-implementation\""),
            SnippetCheck("M265-D002-PKG-02", "\"test:tooling:m265-d002-live-optional-send-and-key-path-runtime-support-core-feature-implementation\""),
            SnippetCheck("M265-D002-PKG-03", "\"check:objc3c:m265-d002-lane-d-readiness\""),
        ],
        PROBE_CPP: [
            SnippetCheck("M265-D002-PROBE-01", "objc3_runtime_copy_keypath_registry_state_for_testing"),
            SnippetCheck("M265-D002-PROBE-02", "registry.keypath_table_entry_count"),
        ],
    }
    for path, snippets in snippet_map.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    evidence: dict[str, Any] = {}
    dynamic_executed = False
    if not args.skip_dynamic_probes:
        dynamic_executed = True
        sub_total, sub_passed, evidence = run_dynamic_probes(failures)
        checks_total += sub_total
        checks_passed += sub_passed

    ok = not failures
    summary = {
        "ok": ok,
        "issue": "M265-D002",
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_executed,
        "failures": [asdict(item) for item in failures],
        "evidence": evidence,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    if ok:
        print(f"[ok] M265-D002 validated ({checks_passed}/{checks_total} checks)")
        return 0
    print(json.dumps(summary, indent=2), file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
