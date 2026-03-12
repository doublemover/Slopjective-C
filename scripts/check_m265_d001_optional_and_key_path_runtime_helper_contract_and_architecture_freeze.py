#!/usr/bin/env python3
"""Checker for M265-D001 optional/keypath runtime helper contract freeze."""

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
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m265" / "M265-D001" / "optional_keypath_runtime_helper_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m265_optional_and_key_path_runtime_helper_contract_and_architecture_freeze_d001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m265" / "m265_d001_optional_and_key_path_runtime_helper_contract_and_architecture_freeze_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
RUNTIME_README = ROOT / "native" / "objc3c" / "src" / "runtime" / "README.md"
LOWERING_H = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
RUNTIME_INTERNAL_H = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
RUNTIME_CPP = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
PACKAGE_JSON = ROOT / "package.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
OPTIONAL_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_optional_member_access_runtime_positive.objc3"
KEYPATH_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_typed_keypath_artifact_positive.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m265" / "d001"
CONTRACT_ID = "objc3c-part3-optional-keypath-runtime-helper-contract/m265-d001-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_part3_optional_keypath_runtime_helper_contract"


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
        failures.append(Finding(display_path(path), "M265-D001-MISSING", f"missing artifact: {display_path(path)}"))
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


def resolve_clang() -> str:
    candidates = (shutil.which("clang.exe"), shutil.which("clang"), r"C:\Program Files\LLVM\bin\clang.exe")
    for candidate in candidates:
        if candidate and Path(candidate).exists():
            return str(candidate)
    return "clang"


def link_executable(out_dir: Path) -> tuple[Path | None, subprocess.CompletedProcess[str] | None]:
    obj_path = out_dir / "module.obj"
    rsp_path = out_dir / "module.runtime-metadata-linker-options.rsp"
    registration_manifest_path = out_dir / "module.runtime-registration-manifest.json"
    exe_path = out_dir / "module.exe"
    if not obj_path.exists() or not rsp_path.exists() or not registration_manifest_path.exists():
        return None, None
    registration_manifest = load_json(registration_manifest_path)
    runtime_library_relative_path = registration_manifest.get("runtime_support_library_archive_relative_path")
    if not isinstance(runtime_library_relative_path, str) or not runtime_library_relative_path.strip():
        return None, None
    runtime_library_path = (ROOT / runtime_library_relative_path).resolve()
    if not runtime_library_path.exists():
        return None, None
    result = run_command([
        resolve_clang(),
        str(obj_path),
        str(runtime_library_path),
        f"@{rsp_path.resolve()}",
        "-o",
        str(exe_path),
    ], cwd=out_dir)
    if result.returncode != 0 or not exe_path.exists():
        return None, result
    return exe_path, result


def validate_empty_diagnostics(path: Path, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    payload = load_json(path)
    diagnostics = payload.get("diagnostics")
    checks_total += 1
    checks_passed += require(isinstance(diagnostics, list), display_path(path), "M265-D001-DIAG-schema", "diagnostics payload must be a list", failures)
    if isinstance(diagnostics, list):
        checks_total += 1
        checks_passed += require(len(diagnostics) == 0, display_path(path), "M265-D001-DIAG-empty", "expected zero diagnostics", failures)
    return checks_total, checks_passed


def runtime_helper_packet(manifest_path: Path) -> dict[str, Any]:
    manifest = load_json(manifest_path)
    packet = manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {}).get("objc_part3_optional_keypath_runtime_helper_contract")
    if not isinstance(packet, dict):
        raise TypeError(f"missing {SURFACE_PATH} in {display_path(manifest_path)}")
    return packet


def run_dynamic_probe(name: str, fixture: Path, expected_exit_code: int, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    evidence: dict[str, Any] = {}
    out_dir = PROBE_ROOT / name
    out_dir.mkdir(parents=True, exist_ok=True)
    compile_result = run_command([str(NATIVE_EXE), str(fixture), "--out-dir", str(out_dir), "--emit-prefix", "module"])
    checks_total += 1
    checks_passed += require(compile_result.returncode == 0, display_path(fixture), f"M265-D001-DYN-{name}-compile", f"compile failed: {compile_result.stderr or compile_result.stdout}", failures)
    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"
    diag_path = out_dir / "module.diagnostics.json"
    rsp_path = out_dir / "module.runtime-metadata-linker-options.rsp"
    reg_manifest_path = out_dir / "module.runtime-registration-manifest.json"
    for check_id, path in (
        (f"M265-D001-DYN-{name}-manifest", manifest_path),
        (f"M265-D001-DYN-{name}-ir", ir_path),
        (f"M265-D001-DYN-{name}-obj", obj_path),
        (f"M265-D001-DYN-{name}-backend", backend_path),
        (f"M265-D001-DYN-{name}-diag", diag_path),
        (f"M265-D001-DYN-{name}-rsp", rsp_path),
        (f"M265-D001-DYN-{name}-registration", reg_manifest_path),
    ):
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), check_id, f"missing artifact: {display_path(path)}", failures)
    if diag_path.exists():
        sub_total, sub_passed = validate_empty_diagnostics(diag_path, failures)
        checks_total += sub_total
        checks_passed += sub_passed
    if backend_path.exists():
        checks_total += 1
        checks_passed += require(backend_path.read_text(encoding="utf-8").strip() == "llvm-direct", display_path(backend_path), f"M265-D001-DYN-{name}-backend-value", "expected llvm-direct object backend", failures)
    if manifest_path.exists():
        packet = runtime_helper_packet(manifest_path)
        for key, expected in {
            "contract_id": CONTRACT_ID,
            "frontend_surface_path": SURFACE_PATH,
            "public_lookup_selector_symbol": "objc3_runtime_lookup_selector",
            "public_dispatch_i32_symbol": "objc3_runtime_dispatch_i32",
            "keypath_descriptor_section": "objc3.runtime.keypath_descriptors",
            "runtime_library_consumption_ready": True,
            "optional_send_runtime_ready": True,
            "typed_keypath_descriptor_handles_ready": True,
            "typed_keypath_runtime_execution_helper_landed": False,
            "diagnostic_fallback_ready": True,
        }.items():
            checks_total += 1
            checks_passed += require(packet.get(key) == expected, display_path(manifest_path), f"M265-D001-DYN-{name}-{key}", f"{key} mismatch", failures)
        evidence[f"{name}_packet"] = packet
    if ir_path.exists():
        ir_text = ir_path.read_text(encoding="utf-8")
        checks_total += 1
        checks_passed += require("part3_optional_keypath_runtime_helper_contract = contract_id=" in ir_text, display_path(ir_path), f"M265-D001-DYN-{name}-ir-contract", "runtime helper IR comment missing", failures)
        checks_total += 1
        checks_passed += require("lookup_selector_symbol=objc3_runtime_lookup_selector" in ir_text, display_path(ir_path), f"M265-D001-DYN-{name}-ir-lookup", "lookup selector symbol missing from runtime helper IR comment", failures)
        if name == "keypath":
            checks_total += 1
            checks_passed += require("@__objc3_sec_keypath_descriptors" in ir_text, display_path(ir_path), "M265-D001-DYN-keypath-ir-aggregate", "keypath descriptor aggregate missing", failures)
        if name == "optional":
            checks_total += 1
            checks_passed += require("@objc3_runtime_dispatch_i32" in ir_text, display_path(ir_path), "M265-D001-DYN-optional-ir-dispatch", "public dispatch symbol missing from optional IR", failures)
    exe_path, link_result = link_executable(out_dir)
    checks_total += 1
    checks_passed += require(exe_path is not None, display_path(out_dir), f"M265-D001-DYN-{name}-link", f"link failed: {(link_result.stderr or link_result.stdout) if link_result else 'missing linker inputs'}", failures)
    if exe_path is not None:
        run_result = run_command([str(exe_path)], cwd=out_dir)
        checks_total += 1
        checks_passed += require(run_result.returncode == expected_exit_code, display_path(exe_path), f"M265-D001-DYN-{name}-run", f"expected exit code {expected_exit_code}, got {run_result.returncode}", failures)
        evidence[f"{name}_exit_code"] = run_result.returncode
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
        "m265-d001-readiness",
        "--summary-out",
        "tmp/reports/m265/M265-D001/ensure_build_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M265-D001-DYN-build", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    checks_total += 1
    checks_passed += require(NATIVE_EXE.exists(), display_path(NATIVE_EXE), "M265-D001-DYN-native", "native driver missing after build", failures)
    for name, fixture, exit_code in (
        ("optional", OPTIONAL_FIXTURE, 9),
        ("keypath", KEYPATH_FIXTURE, 7),
    ):
        sub_total, sub_passed, sub_evidence = run_dynamic_probe(name, fixture, exit_code, failures)
        checks_total += sub_total
        checks_passed += sub_passed
        evidence.update(sub_evidence)
    return checks_total, checks_passed, evidence


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippet_map: dict[Path, list[SnippetCheck]] = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M265-D001-EXP-01", "# M265-D001 Expectations"),
            SnippetCheck("M265-D001-EXP-02", CONTRACT_ID),
            SnippetCheck("M265-D001-EXP-03", SURFACE_PATH),
        ],
        PACKET_DOC: [
            SnippetCheck("M265-D001-PKT-01", "# M265-D001 Packet"),
            SnippetCheck("M265-D001-PKT-02", "full typed key-path runtime evaluation"),
        ],
        LOWERING_H: [
            SnippetCheck("M265-D001-H-01", "kObjc3Part3OptionalKeypathRuntimeHelperContractId"),
            SnippetCheck("M265-D001-H-02", "kObjc3Part3OptionalKeypathRuntimeHelperSurfacePath"),
        ],
        LOWERING_CPP: [
            SnippetCheck("M265-D001-CPP-01", "Objc3Part3OptionalKeypathRuntimeHelperContractSummary"),
            SnippetCheck("M265-D001-CPP-02", "typed_keypath_runtime_execution_helper_landed=false"),
        ],
        IR_CPP: [
            SnippetCheck("M265-D001-IR-01", "part3_optional_keypath_runtime_helper_contract = "),
        ],
        FRONTEND_ARTIFACTS_CPP: [
            SnippetCheck("M265-D001-FA-01", "BuildPart3OptionalKeypathRuntimeHelperContractJson"),
            SnippetCheck("M265-D001-FA-02", "objc_part3_optional_keypath_runtime_helper_contract"),
        ],
        RUNTIME_INTERNAL_H: [
            SnippetCheck("M265-D001-RIH-01", "M265-D001 optional/key-path runtime-helper freeze anchor"),
        ],
        RUNTIME_CPP: [
            SnippetCheck("M265-D001-RCPP-01", "M265-D001 optional/key-path runtime-helper freeze anchor"),
        ],
        PROCESS_CPP: [
            SnippetCheck("M265-D001-PROC-01", "M265-D001 optional/key-path runtime-helper freeze anchor"),
        ],
        DOC_SOURCE: [
            SnippetCheck("M265-D001-DOCSRC-01", "objc_part3_optional_keypath_runtime_helper_contract"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M265-D001-DOCN-01", "objc_part3_optional_keypath_runtime_helper_contract"),
        ],
        SPEC_AM: [
            SnippetCheck("M265-D001-AM-01", "The current runtime/helper boundary for that subset still stays narrow"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M265-D001-ATTR-01", "the current runtime/helper contract for that subset keeps optional sends on"),
        ],
        ARCHITECTURE_DOC: [
            SnippetCheck("M265-D001-ARCH-01", "## M265 optional/key-path runtime helper boundary (D001)"),
        ],
        RUNTIME_README: [
            SnippetCheck("M265-D001-RREADME-01", "`M265-D001` keeps that runtime helper boundary narrow"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M265-D001-PKG-01", "\"check:objc3c:m265-d001-optional-and-key-path-runtime-helper-contract-and-architecture-freeze\""),
            SnippetCheck("M265-D001-PKG-02", "\"test:tooling:m265-d001-optional-and-key-path-runtime-helper-contract-and-architecture-freeze\""),
            SnippetCheck("M265-D001-PKG-03", "\"check:objc3c:m265-d001-lane-d-readiness\""),
        ],
    }
    for path, snippets in snippet_map.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_executed = False
    evidence: dict[str, Any] = {}
    if not args.skip_dynamic_probes:
        dynamic_executed = True
        sub_total, sub_passed, evidence = run_dynamic_probes(failures)
        checks_total += sub_total
        checks_passed += sub_passed

    ok = not failures
    summary = {
        "ok": ok,
        "issue": "M265-D001",
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_executed,
        "failures": [asdict(item) for item in failures],
        "evidence": evidence,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    if ok:
        print(f"[ok] M265-D001 validated ({checks_passed}/{checks_total} checks)")
        return 0
    print(json.dumps(summary, indent=2), file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
