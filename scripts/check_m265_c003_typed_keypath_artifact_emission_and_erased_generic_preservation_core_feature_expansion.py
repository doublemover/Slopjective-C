#!/usr/bin/env python3
"""Checker for M265-C003 typed keypath artifact emission."""

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
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m265" / "M265-C003" / "typed_keypath_artifact_emission_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m265_typed_keypath_artifact_emission_and_erased_generic_preservation_core_feature_expansion_c003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m265" / "m265_c003_typed_keypath_artifact_emission_and_erased_generic_preservation_core_feature_expansion_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
DOC_NATIVE = ROOT / "docs" / "objc3c-native.md"
SPEC_AM = ROOT / "spec" / "ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md"
SPEC_ATTR = ROOT / "spec" / "ATTRIBUTE_AND_SYNTAX_CATALOG.md"
SPEC_PART3 = ROOT / "spec" / "PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md"
LOWERING_H = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS_CPP = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
FRONTEND_TYPES_H = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
PACKAGE_JSON = ROOT / "package.json"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m265_typed_keypath_artifact_positive.objc3"
PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m265" / "c003"
CONTRACT_LINEAGE = "objc3c-part3-optional-keypath-lowering/m265-c001-v1"
KEYPATH_CONTRACT = "objc3c-part3-typed-keypath-artifact-emission/m265-c003-v1"


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
    parser.add_argument("--runner-exe", type=Path, default=RUNNER_EXE)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M265-C003-MISSING", f"missing artifact: {display_path(path)}"))
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


def validate_empty_diagnostics(path: Path, failures: list[Finding]) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    payload = load_json(path)
    diagnostics = payload.get("diagnostics")
    checks_total += 1
    checks_passed += require(isinstance(diagnostics, list), display_path(path), "M265-C003-DIAG-schema", "diagnostics payload must be a list", failures)
    if isinstance(diagnostics, list):
        checks_total += 1
        checks_passed += require(len(diagnostics) == 0, display_path(path), "M265-C003-DIAG-empty", "expected zero diagnostics", failures)
    return checks_total, checks_passed


def resolve_tool(explicit_name: str, absolute_fallback: str) -> str:
    candidates = (shutil.which(explicit_name), absolute_fallback)
    for candidate in candidates:
        if candidate and Path(candidate).exists():
            return str(candidate)
    return explicit_name


def resolve_clang() -> str:
    return resolve_tool("clang.exe", r"C:\Program Files\LLVM\bin\clang.exe")


def resolve_llvm_readobj() -> str:
    return resolve_tool("llvm-readobj.exe", r"C:\Program Files\LLVM\bin\llvm-readobj.exe")


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


def run_dynamic_probes(args: argparse.Namespace, failures: list[Finding]) -> tuple[int, int, dict[str, Any]]:
    checks_total = 0
    checks_passed = 0
    evidence: dict[str, Any] = {}

    ensure_build = run_command([
        sys.executable,
        "scripts/ensure_objc3c_native_build.py",
        "--mode",
        "fast",
        "--reason",
        "m265-c003-readiness",
        "--summary-out",
        "tmp/reports/m265/M265-C003/ensure_build_summary.json",
    ])
    checks_total += 1
    checks_passed += require(ensure_build.returncode == 0, "ensure_objc3c_native_build.py", "M265-C003-DYN-build", f"fast build failed: {ensure_build.stderr or ensure_build.stdout}", failures)
    checks_total += 1
    checks_passed += require(args.native_exe.exists(), display_path(args.native_exe), "M265-C003-DYN-native", "native driver missing after build", failures)
    checks_total += 1
    checks_passed += require(args.runner_exe.exists(), display_path(args.runner_exe), "M265-C003-DYN-runner", "frontend runner missing after build", failures)

    out_dir = PROBE_ROOT / "positive"
    out_dir.mkdir(parents=True, exist_ok=True)
    compile_result = run_command([str(args.native_exe), str(FIXTURE), "--out-dir", str(out_dir), "--emit-prefix", "module"])
    checks_total += 1
    checks_passed += require(compile_result.returncode == 0, display_path(FIXTURE), "M265-C003-DYN-compile", f"positive compile failed: {compile_result.stderr or compile_result.stdout}", failures)

    manifest_path = out_dir / "module.manifest.json"
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"
    diag_path = out_dir / "module.diagnostics.json"
    rsp_path = out_dir / "module.runtime-metadata-linker-options.rsp"
    reg_manifest_path = out_dir / "module.runtime-registration-manifest.json"
    for check_id, path in (
        ("M265-C003-DYN-manifest", manifest_path),
        ("M265-C003-DYN-ir", ir_path),
        ("M265-C003-DYN-obj", obj_path),
        ("M265-C003-DYN-backend", backend_path),
        ("M265-C003-DYN-diag", diag_path),
        ("M265-C003-DYN-rsp", rsp_path),
        ("M265-C003-DYN-registration", reg_manifest_path),
    ):
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), check_id, f"missing artifact: {display_path(path)}", failures)

    if diag_path.exists():
        sub_total, sub_passed = validate_empty_diagnostics(diag_path, failures)
        checks_total += sub_total
        checks_passed += sub_passed

    if backend_path.exists():
        checks_total += 1
        checks_passed += require(backend_path.read_text(encoding="utf-8").strip() == "llvm-direct", display_path(backend_path), "M265-C003-DYN-backend-value", "expected llvm-direct object backend", failures)

    lowering_packet: dict[str, Any] = {}
    generic_packet: dict[str, Any] = {}
    if manifest_path.exists():
        manifest = load_json(manifest_path)
        semantic_surface = manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {})
        if isinstance(semantic_surface, dict):
            lowering_packet = semantic_surface.get("objc_part3_optional_keypath_lowering_contract", {})
            generic_packet = semantic_surface.get("objc_generic_metadata_abi_lowering_surface", {})
        checks_total += 1
        checks_passed += require(isinstance(lowering_packet, dict), display_path(manifest_path), "M265-C003-DYN-lowering-packet", "missing lowering packet", failures)
        checks_total += 1
        checks_passed += require(isinstance(generic_packet, dict), display_path(manifest_path), "M265-C003-DYN-generic-packet", "missing generic metadata packet", failures)
        if isinstance(lowering_packet, dict):
            for key, expected in {
                "contract_id": CONTRACT_LINEAGE,
                "typed_keypath_literal_sites": 1,
                "typed_keypath_self_root_sites": 0,
                "typed_keypath_class_root_sites": 1,
                "live_typed_keypath_artifact_sites": 1,
                "deferred_typed_keypath_sites": 0,
                "ready_for_typed_keypath_artifact_emission": True,
                "typed_keypath_artifact_emission_deferred": False,
            }.items():
                checks_total += 1
                checks_passed += require(lowering_packet.get(key) == expected, display_path(manifest_path), f"M265-C003-DYN-lowering-{key}", f"{key} mismatch", failures)
            checks_total += 1
            checks_passed += require("canonical-runtime-descriptor-handles" in str(lowering_packet.get("typed_keypath_model", "")), display_path(manifest_path), "M265-C003-DYN-lowering-model", "typed keypath model must mention canonical runtime descriptor handles", failures)
            evidence["lowering_packet"] = {k: lowering_packet.get(k) for k in (
                "typed_keypath_literal_sites",
                "typed_keypath_class_root_sites",
                "live_typed_keypath_artifact_sites",
                "deferred_typed_keypath_sites",
                "ready_for_typed_keypath_artifact_emission",
            )}
        if isinstance(generic_packet, dict):
            for key, expected in {
                "generic_metadata_abi_sites": 1,
                "generic_suffix_sites": 1,
                "protocol_composition_sites": 1,
                "object_pointer_type_sites": 1,
                "deterministic_handoff": True,
            }.items():
                checks_total += 1
                checks_passed += require(generic_packet.get(key) == expected, display_path(manifest_path), f"M265-C003-DYN-generic-{key}", f"{key} mismatch", failures)
            checks_total += 1
            checks_passed += require("generic_metadata_abi_sites=1" in str(generic_packet.get("replay_key", "")), display_path(manifest_path), "M265-C003-DYN-generic-replay", "generic replay key missing generic_metadata_abi_sites=1", failures)
            evidence["generic_packet"] = {k: generic_packet.get(k) for k in (
                "generic_metadata_abi_sites",
                "generic_suffix_sites",
                "protocol_composition_sites",
                "object_pointer_type_sites",
                "replay_key",
            )}

    if ir_path.exists():
        ir_text = ir_path.read_text(encoding="utf-8")
        for check_id, needle, detail in (
            ("M265-C003-DYN-ir-lowering", "typed_keypath_artifact_emission = contract=" + KEYPATH_CONTRACT, "typed keypath artifact comment missing"),
            ("M265-C003-DYN-ir-generic", "generic_metadata_abi_lowering =", "generic metadata ABI comment missing"),
            ("M265-C003-DYN-ir-desc", "@__objc3_keypath_desc_0000", "keypath descriptor symbol missing"),
            ("M265-C003-DYN-ir-aggregate", "@__objc3_sec_keypath_descriptors", "keypath aggregate missing"),
            ("M265-C003-DYN-ir-section", "objc3.runtime.keypath_descriptors", "keypath descriptor section missing"),
        ):
            checks_total += 1
            checks_passed += require(needle in ir_text, display_path(ir_path), check_id, detail, failures)

    if obj_path.exists():
        readobj_result = run_command([resolve_llvm_readobj(), "--sections", str(obj_path)])
        checks_total += 1
        checks_passed += require(readobj_result.returncode == 0, display_path(obj_path), "M265-C003-DYN-readobj", f"llvm-readobj failed: {readobj_result.stderr or readobj_result.stdout}", failures)
        if readobj_result.returncode == 0:
            text = readobj_result.stdout
            checks_total += 1
            checks_passed += require("objc3.runtime.keypath_descriptors" in text, display_path(obj_path), "M265-C003-DYN-obj-section", "keypath descriptor section missing from object file", failures)
            evidence["llvm_readobj_sections"] = text

    exe_path, link_result = link_executable(out_dir)
    checks_total += 1
    checks_passed += require(exe_path is not None, display_path(out_dir), "M265-C003-DYN-link", f"link failed: {(link_result.stderr or link_result.stdout) if link_result else 'missing linker inputs'}", failures)
    if exe_path is not None:
        run_result = run_command([str(exe_path)], cwd=out_dir)
        checks_total += 1
        checks_passed += require(run_result.returncode == 7, display_path(exe_path), "M265-C003-DYN-run", f"expected exit code 7, got {run_result.returncode}", failures)
        evidence["linked_executable_exit_code"] = run_result.returncode

    return checks_total, checks_passed, evidence


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    snippet_map: dict[Path, list[SnippetCheck]] = {
        EXPECTATIONS_DOC: [
            SnippetCheck("M265-C003-EXP-01", "# M265-C003 Expectations"),
            SnippetCheck("M265-C003-EXP-02", CONTRACT_LINEAGE),
            SnippetCheck("M265-C003-EXP-03", "stable nonzero handles"),
            SnippetCheck("M265-C003-EXP-04", "execute the linked positive binary and require exit code `7`"),
        ],
        PACKET_DOC: [
            SnippetCheck("M265-C003-PKT-01", "# M265-C003 Packet"),
            SnippetCheck("M265-C003-PKT-02", "stable descriptor handles"),
            SnippetCheck("M265-C003-PKT-03", "objc3.runtime.keypath_descriptors"),
        ],
        LOWERING_H: [
            SnippetCheck("M265-C003-H-01", "kObjc3RuntimeKeypathDescriptorLogicalSection"),
            SnippetCheck("M265-C003-H-02", "live_typed_keypath_artifact_sites"),
        ],
        LOWERING_CPP: [
            SnippetCheck("M265-C003-CPP-01", "live_typed_keypath_artifact_sites"),
            SnippetCheck("M265-C003-CPP-02", "validated typed key-path descriptor emission and stable runtime handles"),
        ],
        IR_CPP: [
            SnippetCheck("M265-C003-IR-01", "typed_keypath_artifact_emission = "),
            SnippetCheck("M265-C003-IR-02", "@__objc3_sec_keypath_descriptors"),
            SnippetCheck("M265-C003-IR-03", "RegisterTypedKeyPathLiteral"),
            SnippetCheck("M265-C003-IR-04", "EmitTypedKeyPathLiteralValue"),
        ],
        FRONTEND_ARTIFACTS_CPP: [
            SnippetCheck("M265-C003-FA-01", "live_typed_keypath_artifact_sites"),
            SnippetCheck("M265-C003-FA-02", "ready_for_typed_keypath_artifact_emission"),
            SnippetCheck("M265-C003-FA-03", "contract.live_typed_keypath_artifact_sites"),
        ],
        FRONTEND_TYPES_H: [
            SnippetCheck("M265-C003-FT-01", "typed-keypath-literals-remain-source-sema-surfaces-while-native-lowering-now-emits-stable-descriptor-handles"),
        ],
        DOC_SOURCE: [
            SnippetCheck("M265-C003-DOCSRC-01", "validated single-component typed key-path literals now lower into retained"),
        ],
        DOC_NATIVE: [
            SnippetCheck("M265-C003-DOCN-01", "validated single-component typed key-path literals now lower into retained"),
        ],
        SPEC_AM: [
            SnippetCheck("M265-C003-AM-01", "validated single-component subset now lowers natively into retained"),
        ],
        SPEC_ATTR: [
            SnippetCheck("M265-C003-ATTR-01", "validated single-component subset now lowers on the"),
        ],
        SPEC_PART3: [
            SnippetCheck("M265-C003-P3-01", "single-component subset by emitting retained descriptor handles"),
        ],
        PACKAGE_JSON: [
            SnippetCheck("M265-C003-PKG-01", "\"check:objc3c:m265-c003-typed-keypath-artifact-emission-and-erased-generic-preservation-core-feature-expansion\""),
            SnippetCheck("M265-C003-PKG-02", "\"test:tooling:m265-c003-typed-keypath-artifact-emission-and-erased-generic-preservation-core-feature-expansion\""),
            SnippetCheck("M265-C003-PKG-03", "\"check:objc3c:m265-c003-lane-c-readiness\""),
        ],
    }
    for path, snippets in snippet_map.items():
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    evidence: dict[str, Any] = {}
    dynamic_executed = False
    if not args.skip_dynamic_probes:
        dynamic_executed = True
        sub_total, sub_passed, evidence = run_dynamic_probes(args, failures)
        checks_total += sub_total
        checks_passed += sub_passed

    ok = not failures
    summary = {
        "ok": ok,
        "issue": "M265-C003",
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_executed,
        "failures": [asdict(item) for item in failures],
        "evidence": evidence,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    if ok:
        print(f"[ok] M265-C003 validated ({checks_passed}/{checks_total} checks)")
        return 0
    print(json.dumps(summary, indent=2), file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
