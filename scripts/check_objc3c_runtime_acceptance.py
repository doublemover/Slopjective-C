#!/usr/bin/env python3
"""Compile and run the live objc3 runtime acceptance workload."""

from __future__ import annotations

import json
import os
import shutil
import subprocess
import sys
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
TMP_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-runtime-acceptance"
REPORT_ROOT = ROOT / "tmp" / "reports" / "runtime" / "acceptance"
BUILD_PS1 = ROOT / "scripts" / "build_objc3c_native.ps1"
COMPILE_PS1 = ROOT / "scripts" / "objc3c_native_compile.ps1"
RUNTIME_LIB = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
PWSH = shutil.which("pwsh") or shutil.which("powershell") or "pwsh"


def run(command: list[str], *, cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        command,
        cwd=str(cwd or ROOT),
        text=True,
        capture_output=True,
        check=False,
    )


def ensure_native_binaries() -> None:
    if NATIVE_EXE.is_file() and RUNTIME_LIB.is_file():
        return
    result = run(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(BUILD_PS1),
            "-ExecutionMode",
            "binaries-only",
        ]
    )
    if result.returncode != 0:
        raise RuntimeError(
            "native build failed:\nSTDOUT:\n"
            + result.stdout
            + "\nSTDERR:\n"
            + result.stderr
        )
    if not NATIVE_EXE.is_file() or not RUNTIME_LIB.is_file():
        raise RuntimeError("native build completed without publishing the runtime executable/library")


def find_clangxx() -> str:
    llvm_root = os.environ.get("LLVM_ROOT")
    if llvm_root:
        candidate = Path(llvm_root) / "bin" / "clang++.exe"
        if candidate.is_file():
            return str(candidate)
    candidate = shutil.which("clang++")
    if candidate:
        return candidate
    raise RuntimeError("clang++ not found; set LLVM_ROOT or ensure clang++ is on PATH")


def compile_fixture(fixture: Path, out_dir: Path) -> Path:
    out_dir.mkdir(parents=True, exist_ok=True)
    result = run(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(COMPILE_PS1),
            str(fixture),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
        ]
    )
    if result.returncode != 0:
        raise RuntimeError(
            f"fixture compile failed for {fixture}:\nSTDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}"
        )
    obj_path = out_dir / "module.obj"
    manifest_path = out_dir / "module.manifest.json"
    registration_manifest_path = out_dir / "module.runtime-registration-manifest.json"
    provenance_path = out_dir / "module.compile-provenance.json"
    if not obj_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {obj_path}")
    if not manifest_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {manifest_path}")
    if not registration_manifest_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {registration_manifest_path}")
    if not provenance_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {provenance_path}")

    provenance = json.loads(provenance_path.read_text(encoding="utf-8"))
    registration_manifest = json.loads(registration_manifest_path.read_text(encoding="utf-8"))
    if provenance.get("contract_id") != "objc3c.native.compile.output.provenance.v1":
        raise RuntimeError("compiled fixture did not publish the native compile provenance contract")
    truthfulness = provenance.get("compile_output_truthfulness")
    if not isinstance(truthfulness, dict) or truthfulness.get("contract_id") != "objc3c.native.compile.output.truthfulness.v1":
        raise RuntimeError("compiled fixture did not publish the compile output truthfulness contract")
    if truthfulness.get("truthful") is not True:
        raise RuntimeError("compiled fixture did not certify truthful compile output")
    if registration_manifest.get("compile_output_provenance_artifact") != "module.compile-provenance.json":
        raise RuntimeError("runtime registration manifest did not bind compile provenance artifact")
    if registration_manifest.get("compile_output_truthful") is not True:
        raise RuntimeError("runtime registration manifest did not certify truthful compile output")
    if registration_manifest.get("compile_output_artifact_set_digest_sha256") != provenance.get("artifact_set_digest_sha256"):
        raise RuntimeError("runtime registration manifest compile output digest drifted from compile provenance")
    return obj_path


def compile_fixture_outputs(fixture: Path, out_dir: Path) -> tuple[Path, Path, Path]:
    obj_path = compile_fixture(fixture, out_dir)
    ll_path = out_dir / "module.ll"
    manifest_path = out_dir / "module.manifest.json"
    if not ll_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {ll_path}")
    return obj_path, ll_path, manifest_path


def compile_probe(clangxx: str, probe: Path, exe_path: Path, extra_objects: list[Path]) -> None:
    exe_path.parent.mkdir(parents=True, exist_ok=True)
    command = [
        clangxx,
        "-std=c++20",
        "-fms-runtime-lib=dll",
        "-I",
        str(ROOT / "native" / "objc3c" / "src"),
        str(probe),
        *[str(path) for path in extra_objects],
        str(RUNTIME_LIB),
        "-o",
        str(exe_path),
    ]
    result = run(command)
    if result.returncode != 0:
        raise RuntimeError(
            f"probe link failed for {probe}:\nSTDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}"
        )


def run_probe(exe_path: Path) -> subprocess.CompletedProcess[str]:
    result = run([str(exe_path)])
    if result.returncode != 0:
        raise RuntimeError(
            f"probe execution failed for {exe_path} (exit={result.returncode}):\n"
            f"STDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}"
        )
    return result


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def parse_json_output(result: subprocess.CompletedProcess[str], label: str) -> dict[str, Any]:
    stdout = result.stdout.strip()
    if not stdout:
        raise RuntimeError(f"{label} produced no JSON output")
    try:
        payload = json.loads(stdout)
    except json.JSONDecodeError as exc:
        raise RuntimeError(f"{label} produced invalid JSON: {exc}\nstdout:\n{stdout}") from exc
    if not isinstance(payload, dict):
        raise RuntimeError(f"{label} did not produce a JSON object")
    return payload


def parse_key_value_output(
    result: subprocess.CompletedProcess[str], label: str
) -> dict[str, Any]:
    stdout = result.stdout.strip()
    if not stdout:
        raise RuntimeError(f"{label} produced no key/value output")
    payload: dict[str, Any] = {}
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if not line:
            continue
        if "=" not in line:
            raise RuntimeError(f"{label} produced malformed line: {line}")
        key, value = line.split("=", 1)
        key = key.strip()
        value = value.strip()
        if value and (value.lstrip("-").isdigit()):
            payload[key] = int(value)
        else:
            payload[key] = value
    if not payload:
        raise RuntimeError(f"{label} produced no parseable key/value output")
    return payload


@dataclass(frozen=True)
class CaseResult:
    case_id: str
    probe: str
    fixture: str | None
    passed: bool
    summary: dict[str, Any]


def check_runtime_library_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "runtime-library"
    probe = ROOT / "tests" / "tooling" / "runtime" / "runtime_library_probe.cpp"
    exe_path = case_dir / "runtime_library_probe.exe"
    compile_probe(clangxx, probe, exe_path, [])
    run_probe(exe_path)
    return CaseResult(
        case_id="runtime-library",
        probe="tests/tooling/runtime/runtime_library_probe.cpp",
        fixture=None,
        passed=True,
        summary={"kind": "standalone-runtime-probe"},
    )


def check_canonical_dispatch_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "canonical-dispatch"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "runtime_canonical_runnable_object_runtime_library.objc3"
    obj_path = compile_fixture(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "runtime_canonical_runnable_object_probe.cpp"
    exe_path = case_dir / "runtime_canonical_runnable_object_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "canonical dispatch probe")

    expect(payload.get("traced_value") == 13, "expected category-backed tracedValue dispatch to return 13")
    expect(payload.get("inherited_value") == 7, "expected superclass inheritedValue dispatch to return 7")
    expect(payload.get("class_value") == 11, "expected class method dispatch to return 11")
    expect(payload.get("alloc_value", 0) != 0, "expected alloc dispatch to return a realized instance receiver")
    expect(payload.get("init_value") == payload.get("alloc_value"), "expected init to preserve the allocated receiver")
    expect(payload.get("new_value", 0) != 0, "expected new dispatch to materialize an instance receiver")

    worker_query = payload.get("worker_query", {})
    tracer_query = payload.get("tracer_query", {})
    method_state = payload.get("method_state", {})
    selector_handles = payload.get("selector_handles", {})
    traced_entry = payload.get("traced_entry", {})
    inherited_entry = payload.get("inherited_entry", {})
    class_entry = payload.get("class_entry", {})
    alloc_entry = payload.get("alloc_entry", {})
    init_entry = payload.get("init_entry", {})
    new_entry = payload.get("new_entry", {})

    expect(worker_query.get("conforms") == 1, "expected Widget to conform to Worker at runtime")
    expect(tracer_query.get("conforms") == 1, "expected Widget category attachment to satisfy Tracer at runtime")
    expect(method_state.get("live_dispatch_count", 0) >= 6, "expected live dispatch count to cover alloc/init/new/traced/inherited/class")
    expect(method_state.get("fallback_dispatch_count", 0) == 0, "did not expect canonical dispatch workload to use fallback dispatch")
    expect(method_state.get("last_selector_stable_id", 0) == selector_handles.get("classValue", 0),
           "expected last dispatch selector stable id to match the selector pool handle for classValue")
    expect(selector_handles.get("alloc", 0) != 0 and selector_handles.get("tracedValue", 0) != 0,
           "expected canonical dispatch selectors to be interned in the runtime selector pool")
    expect(alloc_entry.get("selector_stable_id", 0) == selector_handles.get("alloc", 0),
           "expected alloc cache entry to be keyed by the selector pool stable id")
    expect(init_entry.get("selector_stable_id", 0) == selector_handles.get("init", 0),
           "expected init cache entry to be keyed by the selector pool stable id")
    expect(new_entry.get("selector_stable_id", 0) == selector_handles.get("new", 0),
           "expected new cache entry to be keyed by the selector pool stable id")
    expect(traced_entry.get("selector_stable_id", 0) == selector_handles.get("tracedValue", 0),
           "expected tracedValue cache entry to be keyed by the selector pool stable id")
    expect(inherited_entry.get("selector_stable_id", 0) == selector_handles.get("inheritedValue", 0),
           "expected inheritedValue cache entry to be keyed by the selector pool stable id")
    expect(class_entry.get("selector_stable_id", 0) == selector_handles.get("classValue", 0),
           "expected classValue cache entry to be keyed by the selector pool stable id")
    expect(traced_entry.get("resolved") == 1, "expected tracedValue cache entry to resolve live")
    expect(inherited_entry.get("resolved") == 1, "expected inheritedValue cache entry to resolve live")
    expect(class_entry.get("resolved") == 1, "expected classValue cache entry to resolve live")

    return CaseResult(
        case_id="canonical-dispatch",
        probe="tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
        fixture="tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
        passed=True,
        summary={
            "traced_value": payload["traced_value"],
            "inherited_value": payload["inherited_value"],
            "class_value": payload["class_value"],
            "live_dispatch_count": method_state["live_dispatch_count"],
            "attached_category_count": payload.get("graph_state", {}).get("attached_category_count"),
        },
    )


def check_live_dispatch_fast_path_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "dispatch-fast-path"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "m272_d002_live_dispatch_fast_path_positive.objc3"
    )
    obj_path, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "m272_d002_live_dispatch_fast_path_probe.cpp"
    exe_path = case_dir / "m272_d002_live_dispatch_fast_path_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_key_value_output(run_probe(exe_path), "dispatch fast-path probe")
    ll_text = ll_path.read_text(encoding="utf-8")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    registration_manifest_path = case_dir / "compile" / "module.runtime-registration-manifest.json"
    registration_manifest = json.loads(registration_manifest_path.read_text(encoding="utf-8"))

    expect(payload.get("baseline_status") == 0, "expected baseline method-cache snapshot to succeed")
    expect(payload.get("dynamic_entry_status") == 0, "expected dynamic fast-path entry lookup to succeed")
    expect(payload.get("explicit_entry_status") == 0, "expected explicit fast-path entry lookup to succeed")
    expect(payload.get("fallback_entry_status") == 0, "expected fallback method-cache entry lookup to succeed")
    expect(payload.get("implicit_value") == 3, "expected implicit direct call to remain direct")
    expect(payload.get("explicit_value") == 5, "expected explicit direct call to remain direct")
    expect(payload.get("mixed_first") == 12 and payload.get("mixed_second") == 12,
           "expected mixed dispatch fixture to execute through the live runtime")
    expect(payload.get("fallback_first") == payload.get("fallback_expected") == payload.get("fallback_second"),
           "expected fallback dispatch to stay deterministic across cache miss/hit")
    expect(payload.get("baseline_cache_entry_count") == 4,
           "expected realized dispatch runtime to seed four method-cache entries")
    expect(payload.get("baseline_fast_path_seed_count") == 4,
           "expected realized dispatch runtime to publish seeded fast-path entries")
    expect(payload.get("dynamic_entry_found") == 1 and payload.get("dynamic_entry_resolved") == 1,
           "expected dynamicEscape entry to resolve live")
    expect(payload.get("dynamic_entry_fast_path_seeded") == 1,
           "expected dynamicEscape entry to be seeded for fast-path dispatch")
    expect(payload.get("dynamic_entry_effective_direct_dispatch") == 0,
           "expected dynamicEscape entry to stay runtime-dispatched")
    expect(payload.get("dynamic_entry_fast_path_reason") == "class-final",
           "expected dynamicEscape fast-path reason to remain class-final")
    expect(payload.get("explicit_entry_found") == 1 and payload.get("explicit_entry_resolved") == 1,
           "expected explicitDirect entry to resolve live")
    expect(payload.get("explicit_entry_fast_path_seeded") == 1,
           "expected explicitDirect entry to be seeded for direct dispatch")
    expect(payload.get("explicit_entry_effective_direct_dispatch") == 1,
           "expected explicitDirect entry to preserve direct dispatch semantics")
    expect(payload.get("explicit_entry_fast_path_reason") == "direct",
           "expected explicitDirect fast-path reason to remain direct")
    expect(payload.get("mixed_first_state_last_dispatch_used_cache") == 1,
           "expected first mixed dispatch runtime call to hit the seeded cache")
    expect(payload.get("mixed_first_state_last_dispatch_used_fast_path") == 1,
           "expected first mixed dispatch runtime call to use the seeded fast path")
    expect(payload.get("mixed_first_state_last_dispatch_resolved_live_method") == 1,
           "expected first mixed dispatch runtime call to resolve a live method")
    expect(payload.get("mixed_first_state_last_dispatch_fell_back") == 0,
           "did not expect first mixed dispatch runtime call to fall back")
    expect(payload.get("mixed_first_state_last_selector") == "dynamicEscape",
           "expected first mixed dispatch runtime call to target dynamicEscape")
    expect(payload.get("mixed_second_state_last_dispatch_used_cache") == 1,
           "expected repeated mixed dispatch runtime call to remain cached")
    expect(payload.get("mixed_second_state_last_dispatch_used_fast_path") == 1,
           "expected repeated mixed dispatch runtime call to remain on the fast path")
    expect(payload.get("mixed_second_state_last_dispatch_fell_back") == 0,
           "did not expect repeated mixed dispatch runtime call to fall back")
    expect(payload.get("fallback_first_state_last_dispatch_used_cache") == 0,
           "expected first missingDispatch: call to miss the cache")
    expect(payload.get("fallback_first_state_last_dispatch_used_fast_path") == 0,
           "expected first missingDispatch: call to avoid the fast path")
    expect(payload.get("fallback_first_state_last_dispatch_resolved_live_method") == 0,
           "did not expect first missingDispatch: call to resolve live")
    expect(payload.get("fallback_first_state_last_dispatch_fell_back") == 1,
           "expected first missingDispatch: call to fall back")
    expect(payload.get("fallback_second_state_last_dispatch_used_cache") == 1,
           "expected repeated missingDispatch: call to hit the fallback cache entry")
    expect(payload.get("fallback_second_state_last_dispatch_used_fast_path") == 0,
           "expected repeated missingDispatch: call to stay off the fast path")
    expect(payload.get("fallback_second_state_last_dispatch_fell_back") == 1,
           "expected repeated missingDispatch: call to remain a fallback dispatch")
    expect(
        "; method_dispatch_and_selector_thunk_lowering_surface = "
        "contract_id=objc3c.method.dispatch.selector.thunk.lowering.v1"
        in ll_text,
        "expected LLVM IR to publish authoritative method dispatch and selector thunk lowering surface",
    )
    expect("direct_dispatch_call_sites=5" in ll_text,
           "expected mixed dispatch fixture to emit five direct dispatch calls")
    expect("runtime_dispatch_call_sites=1" in ll_text,
           "expected mixed dispatch fixture to emit one live runtime dispatch call")
    expect("selector_pool_gep_sites=1" in ll_text,
           "expected mixed dispatch fixture to materialize one selector thunk gep")
    expect("selector_pool_count=4" in ll_text,
           "expected mixed dispatch fixture to publish four pooled selectors")
    expect("dynamic_opt_out_sites=2" in ll_text,
           "expected mixed dispatch fixture to preserve two objc_dynamic opt-out sites")
    expect("call i32 @objc3_method_PolicyBox_class_implicitDirect()" in ll_text,
           "expected implicit direct calls to lower as exact direct LLVM calls")
    expect("call i32 @objc3_method_PolicyBox_class_explicitDirect()" in ll_text,
           "expected explicit direct calls to lower as exact direct LLVM calls")
    expect("call i32 @objc3_method_PolicyBox_class_callers()" in ll_text,
           "expected runFixture to preserve direct class-method dispatch to callers")
    expect("call i32 @objc3_runtime_dispatch_i32(" in ll_text,
           "expected dynamicEscape lowering to retain the live runtime dispatch call")
    expect("@__objc3_sec_selector_pool" in ll_text,
           "expected mixed dispatch fixture to emit the selector pool section root")
    lowering_surface = manifest.get("dispatch_and_synthesized_accessor_lowering_surface", {})
    expect(isinstance(lowering_surface, dict),
           "expected compile manifest to publish the live lowering surface")
    expect(lowering_surface.get("runtime_dispatch_symbol_matches_lowering") is True,
           "expected compile manifest lowering surface to keep dispatch symbols aligned")
    expect(lowering_surface.get("message_send_sites") == 6,
           "expected compile manifest lowering surface to publish six message send sites")
    runtime_abi_surface = manifest.get("dispatch_accessor_runtime_abi_surface", {})
    expect(isinstance(runtime_abi_surface, dict),
           "expected compile manifest to publish dispatch/accessor runtime ABI surface")
    expect(runtime_abi_surface.get("contract_id") == "objc3c.runtime.dispatch_accessor.abi.surface.v1",
           "expected dispatch/accessor runtime ABI surface contract id in compile manifest")
    expect(runtime_abi_surface.get("runtime_dispatch_symbol") == "objc3_runtime_dispatch_i32",
           "expected runtime ABI surface to publish canonical runtime dispatch symbol")
    expect(runtime_abi_surface.get("method_cache_state_snapshot_symbol") == "objc3_runtime_copy_method_cache_state_for_testing",
           "expected runtime ABI surface to publish method cache state snapshot helper")
    expect(runtime_abi_surface.get("property_registry_state_snapshot_symbol") == "objc3_runtime_copy_property_registry_state_for_testing",
           "expected runtime ABI surface to publish property registry snapshot helper")
    expect(runtime_abi_surface.get("arc_debug_state_snapshot_symbol") == "objc3_runtime_copy_arc_debug_state_for_testing",
           "expected runtime ABI surface to publish ARC debug snapshot helper")
    expect(runtime_abi_surface.get("bind_current_property_context_symbol") == "objc3_runtime_bind_current_property_context_for_testing",
           "expected runtime ABI surface to publish property context bind helper")
    expect(runtime_abi_surface.get("clear_current_property_context_symbol") == "objc3_runtime_clear_current_property_context_for_testing",
           "expected runtime ABI surface to publish property context clear helper")
    expect(runtime_abi_surface.get("private_testing_surface_only") is True,
           "expected runtime ABI surface to remain on the private testing boundary")
    expect(runtime_abi_surface.get("deterministic") is True,
           "expected runtime ABI surface to report deterministic handoff")
    registration_runtime_abi_surface = registration_manifest.get("dispatch_accessor_runtime_abi_surface", {})
    expect(isinstance(registration_runtime_abi_surface, dict),
           "expected runtime registration manifest to publish dispatch/accessor runtime ABI surface")
    expect(registration_runtime_abi_surface.get("contract_id") == "objc3c.runtime.dispatch_accessor.abi.surface.v1",
           "expected dispatch/accessor runtime ABI surface contract id in runtime registration manifest")
    expect(registration_runtime_abi_surface.get("runtime_dispatch_symbol") == "objc3_runtime_dispatch_i32",
           "expected runtime registration manifest to publish canonical runtime dispatch symbol")
    expect(registration_runtime_abi_surface.get("current_property_read_symbol") == "objc3_runtime_read_current_property_i32",
           "expected runtime registration manifest to publish current-property read helper")
    expect(registration_runtime_abi_surface.get("current_property_exchange_symbol") == "objc3_runtime_exchange_current_property_i32",
           "expected runtime registration manifest to publish current-property exchange helper")
    expect(registration_runtime_abi_surface.get("weak_current_property_load_symbol") == "objc3_runtime_load_weak_current_property_i32",
           "expected runtime registration manifest to publish weak current-property load helper")
    expect(registration_runtime_abi_surface.get("autorelease_symbol") == "objc3_runtime_autorelease_i32",
           "expected runtime registration manifest to publish autorelease helper")
    expect(registration_runtime_abi_surface.get("private_testing_surface_only") is True,
           "expected runtime registration manifest ABI surface to remain private-testing only")
    expect(registration_runtime_abi_surface.get("deterministic") is True,
           "expected runtime registration manifest ABI surface to report deterministic handoff")

    return CaseResult(
        case_id="dispatch-fast-path",
        probe="tests/tooling/runtime/m272_d002_live_dispatch_fast_path_probe.cpp",
        fixture="tests/tooling/fixtures/native/m272_d002_live_dispatch_fast_path_positive.objc3",
        passed=True,
        summary={
            "llvm_ir": str(ll_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "registration_manifest": str(registration_manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "baseline_cache_entry_count": payload.get("baseline_cache_entry_count"),
            "baseline_fast_path_seed_count": payload.get("baseline_fast_path_seed_count"),
            "mixed_first_live_dispatch_count": payload.get("mixed_first_state_live_dispatch_count"),
            "mixed_second_live_dispatch_count": payload.get("mixed_second_state_live_dispatch_count"),
            "fallback_first_fallback_dispatch_count": payload.get("fallback_first_state_fallback_dispatch_count"),
            "fallback_second_fallback_dispatch_count": payload.get("fallback_second_state_fallback_dispatch_count"),
        },
    )


def check_property_reflection_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-reflection"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m257_d003_property_metadata_reflection_positive.objc3"
    obj_path = compile_fixture(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "runtime_property_metadata_reflection_probe.cpp"
    exe_path = case_dir / "runtime_property_metadata_reflection_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "property reflection probe")

    widget_entry = payload.get("widget_entry", {})
    token_property = payload.get("token_property", {})
    value_property = payload.get("value_property", {})
    count_property = payload.get("count_property", {})
    missing_property = payload.get("missing_property", {})
    missing_class_property = payload.get("missing_class_property", {})
    registry_after_count = payload.get("registry_state_after_count", {})

    expect(widget_entry.get("found") == 1, "expected Widget realized class entry to be present")
    expect(token_property.get("found") == 1, "expected token property to be reflectable")
    expect(token_property.get("setter_available") == 0, "expected readonly token property to have no setter")
    expect(token_property.get("has_runtime_getter") == 1, "expected token property getter to be runtime-backed")
    expect(value_property.get("found") == 1, "expected value property to be reflectable")
    expect(value_property.get("setter_available") == 1, "expected value property to expose a setter")
    expect(value_property.get("has_runtime_getter") == 1 and value_property.get("has_runtime_setter") == 1,
           "expected value property getter/setter to be runtime-backed")
    expect(count_property.get("found") == 1, "expected count property to be reflectable")
    expect(count_property.get("has_runtime_getter") == 1 and count_property.get("has_runtime_setter") == 1,
           "expected count property getter/setter to be runtime-backed")
    expect(registry_after_count.get("slot_backed_property_count", 0) >= 3,
           "expected slot-backed property registry to include the three Widget properties")
    expect(missing_property.get("found") == 0, "expected missing property lookup to fail closed")
    expect(missing_class_property.get("found") == 0, "expected missing class property lookup to fail closed")

    return CaseResult(
        case_id="property-reflection",
        probe="tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
        fixture="tests/tooling/fixtures/native/m257_d003_property_metadata_reflection_positive.objc3",
        passed=True,
        summary={
            "reflectable_property_count": registry_after_count.get("reflectable_property_count"),
            "slot_backed_property_count": registry_after_count.get("slot_backed_property_count"),
            "value_property_setter_available": value_property.get("setter_available"),
            "count_property_runtime_setter": count_property.get("has_runtime_setter"),
        },
    )


def check_property_execution_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-execution"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m257_property_ivar_execution_matrix_positive.objc3"
    obj_path = compile_fixture(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "m257_e002_property_ivar_execution_matrix_probe.cpp"
    exe_path = case_dir / "m257_e002_property_ivar_execution_matrix_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "property execution probe")

    widget_entry = payload.get("widget_entry", {})
    registry_state = payload.get("registry_state", {})
    count_property = payload.get("count_property", {})
    enabled_property = payload.get("enabled_property", {})
    value_property = payload.get("value_property", {})
    token_property = payload.get("token_property", {})
    count_method = payload.get("count_method", {})
    enabled_method = payload.get("enabled_method", {})
    value_method = payload.get("value_method", {})
    token_method = payload.get("token_method", {})

    expect(payload.get("widget_instance", 0) != 0, "expected alloc to materialize a Widget instance")
    expect(payload.get("count_value") == 37, "expected synthesized count getter to return the stored value")
    expect(payload.get("enabled_value") == 1, "expected synthesized enabled getter to return the stored value")
    expect(payload.get("value_result") == 55, "expected synthesized strong property getter to return the stored value")
    expect(widget_entry.get("found") == 1, "expected Widget to be realized during property execution")
    expect(widget_entry.get("runtime_property_accessor_count", 0) >= 4,
           "expected Widget to publish runtime-backed synthesized accessors")
    expect(registry_state.get("slot_backed_property_count", 0) >= 4,
           "expected property execution fixture to register four slot-backed properties")
    expect(count_property.get("has_runtime_getter") == 1 and count_property.get("has_runtime_setter") == 1,
           "expected count property to execute through runtime-backed synthesized accessors")
    expect(enabled_property.get("has_runtime_getter") == 1 and enabled_property.get("has_runtime_setter") == 1,
           "expected enabled property to execute through runtime-backed synthesized accessors")
    expect(value_property.get("has_runtime_getter") == 1 and value_property.get("has_runtime_setter") == 1,
           "expected value property to execute through runtime-backed synthesized accessors")
    expect(token_property.get("has_runtime_getter") == 1 and token_property.get("setter_available") == 0,
           "expected readonly token property to expose only the synthesized getter")
    expect(count_property.get("property_name") == "count",
           "expected count property reflection to stay coherent")
    expect(count_property.get("effective_getter_selector") == "count",
           "expected count getter selector reflection to stay coherent")
    expect(count_property.get("effective_setter_selector") == "setCount:",
           "expected count setter selector reflection to stay coherent")
    expect(enabled_property.get("effective_getter_selector") == "enabled",
           "expected enabled getter selector reflection to stay coherent")
    expect(enabled_property.get("effective_setter_selector") == "setEnabled:",
           "expected enabled setter selector reflection to stay coherent")
    expect(value_property.get("effective_getter_selector") == "currentValue",
           "expected value getter selector reflection to stay coherent")
    expect(value_property.get("effective_setter_selector") == "setCurrentValue:",
           "expected value setter selector reflection to stay coherent")
    expect(token_property.get("effective_getter_selector") == "tokenValue",
           "expected token getter selector reflection to stay coherent")
    expect(count_property.get("getter_owner_identity"), "expected count getter owner identity to be published")
    expect(count_property.get("setter_owner_identity"), "expected count setter owner identity to be published")
    expect(enabled_property.get("getter_owner_identity"), "expected enabled getter owner identity to be published")
    expect(enabled_property.get("setter_owner_identity"), "expected enabled setter owner identity to be published")
    expect(value_property.get("getter_owner_identity"), "expected value getter owner identity to be published")
    expect(value_property.get("setter_owner_identity"), "expected value setter owner identity to be published")
    expect(token_property.get("getter_owner_identity"), "expected token getter owner identity to be published")
    expect(token_property.get("setter_owner_identity") is None,
           "did not expect readonly token property to publish a setter owner identity")
    expect(count_property.get("base_identity") == widget_entry.get("base_identity"),
           "expected count property base identity to match the realized Widget class")
    expect(enabled_property.get("base_identity") == widget_entry.get("base_identity"),
           "expected enabled property base identity to match the realized Widget class")
    expect(value_property.get("base_identity") == widget_entry.get("base_identity"),
           "expected value property base identity to match the realized Widget class")
    expect(token_property.get("base_identity") == widget_entry.get("base_identity"),
           "expected token property base identity to match the realized Widget class")
    expect(registry_state.get("last_resolved_class_name") == "Widget",
           "expected property registry to resolve Widget during live accessor execution")
    expect(registry_state.get("last_resolved_owner_identity"),
           "expected property registry to publish the resolved owner identity")
    expect(count_method.get("resolved") == 1 and count_method.get("parameter_count") == 0,
           "expected count getter dispatch to resolve live through the runtime cache")
    expect(enabled_method.get("resolved") == 1 and enabled_method.get("parameter_count") == 0,
           "expected enabled getter dispatch to resolve live through the runtime cache")
    expect(value_method.get("resolved") == 1 and value_method.get("parameter_count") == 0,
           "expected currentValue getter dispatch to resolve live through the runtime cache")
    expect(token_method.get("resolved") == 1 and token_method.get("parameter_count") == 0,
           "expected tokenValue getter dispatch to resolve live through the runtime cache")
    expect(count_method.get("resolved_owner_identity") == count_property.get("getter_owner_identity"),
           "expected count getter cache ownership to match reflected property ownership")
    expect(enabled_method.get("resolved_owner_identity") == enabled_property.get("getter_owner_identity"),
           "expected enabled getter cache ownership to match reflected property ownership")
    expect(value_method.get("resolved_owner_identity") == value_property.get("getter_owner_identity"),
           "expected currentValue getter cache ownership to match reflected property ownership")
    expect(token_method.get("resolved_owner_identity") == token_property.get("getter_owner_identity"),
           "expected tokenValue getter cache ownership to match reflected property ownership")
    return CaseResult(
        case_id="property-execution",
        probe="tests/tooling/runtime/m257_e002_property_ivar_execution_matrix_probe.cpp",
        fixture="tests/tooling/fixtures/native/m257_property_ivar_execution_matrix_positive.objc3",
        passed=True,
        summary={
            "count_value": payload.get("count_value"),
            "enabled_value": payload.get("enabled_value"),
            "value_result": payload.get("value_result"),
            "runtime_property_accessor_count": widget_entry.get("runtime_property_accessor_count"),
            "slot_backed_property_count": registry_state.get("slot_backed_property_count"),
        },
    )


def check_arc_property_helper_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "arc-property-helper-abi"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "m262_arc_property_interaction_positive.objc3"
    )
    obj_path = compile_fixture(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "m262_d003_arc_debug_instrumentation_probe.cpp"
    exe_path = case_dir / "m262_d003_arc_debug_instrumentation_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "arc property helper probe")

    inside = payload.get("inside", {})
    after = payload.get("after", {})

    expect(payload.get("parent", 0) != 0 and payload.get("child", 0) != 0,
           "expected ArcBox runtime helper probe to allocate live receivers")
    expect(payload.get("bind_current_status") == 0,
           "expected strong-property current context binding to succeed")
    expect(payload.get("bind_weak_status") == 0,
           "expected weak-property current context binding to succeed")
    expect(payload.get("rebind_current_status") == 0 and payload.get("rebind_weak_status") == 0,
           "expected current-property helper rebinds to succeed")
    expect(payload.get("getter_value") == payload.get("child"),
           "expected current-property getter helper to read the stored child value")
    expect(payload.get("weak_set_result") == payload.get("child"),
           "expected weak-property helper write to preserve the child value")
    expect(payload.get("weak_inside_pool") == payload.get("child"),
           "expected weak-property helper read inside the pool to preserve the child value")
    expect(payload.get("weak_after_pool") == payload.get("child"),
           "expected weak-property helper read after pool pop to stay coherent")
    expect(payload.get("strong_set_result") == 0,
           "expected first strong-property exchange to replace an empty slot")
    expect(payload.get("clear_strong_result") == payload.get("child"),
           "expected clearing the strong property to return the previous child value")
    expect(inside.get("current_property_read_count", 0) >= 2,
           "expected live current-property reads to execute through the runtime helper ABI")
    expect(inside.get("current_property_write_count", 0) >= 1,
           "expected live current-property writes to execute through the runtime helper ABI")
    expect(inside.get("current_property_exchange_count", 0) >= 2,
           "expected strong ownership accessors to execute through exchange helper traffic")
    expect(inside.get("weak_current_property_load_count", 0) >= 1,
           "expected weak-property loads to execute through the runtime helper ABI")
    expect(inside.get("weak_current_property_store_count", 0) >= 1,
           "expected weak-property stores to execute through the runtime helper ABI")
    expect(inside.get("last_property_receiver") == payload.get("parent"),
           "expected helper ABI debug state to preserve the bound receiver")
    expect(inside.get("last_property_name") == "weakValue",
           "expected helper ABI debug state to report the bound weak property")
    expect(inside.get("last_property_owner_identity") == "implementation:ArcBox",
           "expected helper ABI debug state to report the ArcBox owner identity")
    expect(after.get("autoreleasepool_pop_count", 0) >= 1,
           "expected helper ABI probe to pop an autorelease pool")
    expect(after.get("release_call_count", 0) >= 3,
           "expected helper ABI probe to release the child, retained value, and parent")

    return CaseResult(
        case_id="arc-property-helper-abi",
        probe="tests/tooling/runtime/m262_d003_arc_debug_instrumentation_probe.cpp",
        fixture="tests/tooling/fixtures/native/m262_arc_property_interaction_positive.objc3",
        passed=True,
        summary={
            "parent": payload.get("parent"),
            "child": payload.get("child"),
            "getter_value": payload.get("getter_value"),
            "weak_after_pool": payload.get("weak_after_pool"),
            "inside_current_property_exchange_count": inside.get("current_property_exchange_count"),
            "after_release_call_count": after.get("release_call_count"),
        },
    )


def check_synthesized_accessor_codegen_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-codegen"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m257_synthesized_accessor_property_lowering_positive.objc3"
    _, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    registration_manifest_path = case_dir / "compile" / "module.runtime-registration-manifest.json"
    if not registration_manifest_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {registration_manifest_path}")

    ll_text = ll_path.read_text(encoding="utf-8")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    registration_manifest = json.loads(registration_manifest_path.read_text(encoding="utf-8"))

    required_ir_snippets = {
        "count getter": "define i32 @objc3_method_Widget_instance_count()",
        "count setter": "define void @objc3_method_Widget_instance_setCount_(i32 %arg0)",
        "enabled getter": "define i1 @objc3_method_Widget_instance_enabled()",
        "enabled setter": "define void @objc3_method_Widget_instance_setEnabled_(i1 %arg0)",
        "value getter": "define i32 @objc3_method_Widget_instance_value()",
        "value setter": "define void @objc3_method_Widget_instance_setValue_(i32 %arg0)",
        "getter runtime read": "call i32 @objc3_runtime_read_current_property_i32()",
        "setter runtime write": "call void @objc3_runtime_write_current_property_i32(i32 %arg0)",
        "bool setter coercion": "%objc3_property_value = zext i1 %arg0 to i32",
        "strong getter retain": "%objc3_property_retained = call i32 @objc3_runtime_retain_i32(i32 %objc3_property_slot)",
        "strong getter autorelease": "%objc3_property_autoreleased = call i32 @objc3_runtime_autorelease_i32(i32 %objc3_property_retained)",
        "strong setter exchange": "%objc3_property_previous = call i32 @objc3_runtime_exchange_current_property_i32(i32 %objc3_property_retained)",
        "strong setter release": "%objc3_property_release = call i32 @objc3_runtime_release_i32(i32 %objc3_property_previous)",
        "count descriptor getter binding": "ptr @objc3_method_Widget_instance_count, ptr @objc3_method_Widget_instance_setCount_",
        "enabled descriptor getter binding": "ptr @objc3_method_Widget_instance_enabled, ptr @objc3_method_Widget_instance_setEnabled_",
        "value descriptor getter binding": "ptr @objc3_method_Widget_instance_value, ptr @objc3_method_Widget_instance_setValue_",
    }
    for label, snippet in required_ir_snippets.items():
        expect(snippet in ll_text, f"expected synthesized accessor codegen to emit {label}")

    synthesis_summary = manifest.get("lowering_property_synthesis_ivar_binding", {})
    expect(isinstance(synthesis_summary, dict), "expected property synthesis lowering summary in compile manifest")
    expect(synthesis_summary.get("deterministic_handoff") is True,
           "expected property synthesis lowering summary to report deterministic handoff")
    replay_key = synthesis_summary.get("replay_key", "")
    expect("property_synthesis_sites=3" in replay_key,
           "expected property synthesis replay key to record the three synthesized properties")
    expect("property_synthesis_default_ivar_bindings=3" in replay_key,
           "expected property synthesis replay key to record the default ivar bindings")
    expect(registration_manifest.get("property_descriptor_count", 0) >= 6,
           "expected runtime registration manifest to publish synthesized property descriptors")
    lowering_surface = manifest.get("dispatch_and_synthesized_accessor_lowering_surface", {})
    expect(isinstance(lowering_surface, dict),
           "expected authoritative dispatch and synthesized-accessor lowering surface in compile manifest")
    expect(lowering_surface.get("contract_id") ==
           "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
           "expected lowering surface contract id for dispatch and synthesized accessors")
    expect(lowering_surface.get("runtime_dispatch_symbol") == "objc3_runtime_dispatch_i32",
           "expected lowering surface to publish canonical runtime dispatch symbol")
    expect(lowering_surface.get("runtime_dispatch_symbol_matches_lowering") is True,
           "expected lowering surface to bind lowering, shim, and runtime library dispatch symbols together")
    expect(lowering_surface.get("property_synthesis_sites") == 3,
           "expected lowering surface to publish three synthesized properties")
    expect(lowering_surface.get("property_synthesis_default_ivar_bindings") == 3,
           "expected lowering surface to publish three default ivar bindings")
    expect(lowering_surface.get("property_descriptor_count") == registration_manifest.get("property_descriptor_count"),
           "expected lowering surface property descriptor count to match runtime registration manifest")
    expect(lowering_surface.get("ivar_descriptor_count") == registration_manifest.get("ivar_descriptor_count"),
           "expected lowering surface ivar descriptor count to match runtime registration manifest")
    expect(lowering_surface.get("deterministic_handoff") is True,
           "expected lowering surface to report deterministic handoff")
    expect(
        "; dispatch_and_synthesized_accessor_lowering_surface = "
        "contract_id=objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1"
        in ll_text,
        "expected LLVM IR banner to publish dispatch and synthesized-accessor lowering surface",
    )
    expect("property_synthesis_sites=3" in ll_text,
           "expected LLVM IR banner to report three synthesized properties")
    expect("property_descriptor_count=6" in ll_text,
           "expected LLVM IR banner to report synthesized property descriptor count")
    expect("member_table_emission_ready=true" in ll_text,
           "expected LLVM IR banner to report member table emission readiness")
    expect(
        "; synthesized_getter_setter_llvm_ir_generation_surface = "
        "contract_id=objc3c.synthesized.getter.setter.llvm.ir.generation.v1"
        in ll_text,
        "expected LLVM IR to publish synthesized getter/setter generation surface",
    )
    expect("getter_definitions=3" in ll_text,
           "expected synthesized accessor fixture to emit three getter definitions")
    expect("setter_definitions=3" in ll_text,
           "expected synthesized accessor fixture to emit three setter definitions")
    expect("read_current_property_calls=3" in ll_text,
           "expected synthesized accessor fixture to emit three current-property reads")
    expect("write_current_property_calls=2" in ll_text,
           "expected synthesized accessor fixture to emit two current-property writes")
    expect("exchange_current_property_calls=1" in ll_text,
           "expected synthesized accessor fixture to emit one strong current-property exchange")
    expect("retain_calls=2" in ll_text,
           "expected synthesized accessor fixture to emit two retain helper calls")
    expect("release_calls=1" in ll_text,
           "expected synthesized accessor fixture to emit one release helper call")
    expect("autorelease_calls=1" in ll_text,
           "expected synthesized accessor fixture to emit one autorelease helper call")

    return CaseResult(
        case_id="property-codegen",
        probe="real-compile-llvm-inspection",
        fixture="tests/tooling/fixtures/native/m257_synthesized_accessor_property_lowering_positive.objc3",
        passed=True,
        summary={
            "llvm_ir": str(ll_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "registration_manifest": str(registration_manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "property_descriptor_count": registration_manifest.get("property_descriptor_count"),
        },
    )


def main() -> int:
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    run_dir = TMP_ROOT / run_id
    report_path = REPORT_ROOT / "summary.json"
    run_dir.mkdir(parents=True, exist_ok=True)
    report_path.parent.mkdir(parents=True, exist_ok=True)

    ensure_native_binaries()
    clangxx = find_clangxx()

    results = [
        check_runtime_library_case(clangxx, run_dir),
        check_canonical_dispatch_case(clangxx, run_dir),
        check_live_dispatch_fast_path_case(clangxx, run_dir),
        check_synthesized_accessor_codegen_case(run_dir),
        check_property_execution_case(clangxx, run_dir),
        check_property_reflection_case(clangxx, run_dir),
        check_arc_property_helper_case(clangxx, run_dir),
    ]

    summary = {
        "status": "PASS",
        "run_dir": str(run_dir.relative_to(ROOT)).replace("\\", "/"),
        "clangxx": clangxx,
        "runtime_library": str(RUNTIME_LIB.relative_to(ROOT)).replace("\\", "/"),
        "case_count": len(results),
        "cases": [
            {
                "case_id": result.case_id,
                "probe": result.probe,
                "fixture": result.fixture,
                "passed": result.passed,
                "summary": result.summary,
            }
            for result in results
        ],
        "dispatch_accessor_runtime_abi_surface": {
            "contract_id": "objc3c.runtime.dispatch_accessor.abi.surface.v1",
            "proof_cases": [
                "dispatch-fast-path",
                "property-execution",
                "arc-property-helper-abi",
            ],
            "runtime_dispatch_symbol": "objc3_runtime_dispatch_i32",
            "method_cache_state_snapshot_symbol": "objc3_runtime_copy_method_cache_state_for_testing",
            "property_registry_state_snapshot_symbol": "objc3_runtime_copy_property_registry_state_for_testing",
            "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
            "arc_debug_state_snapshot_symbol": "objc3_runtime_copy_arc_debug_state_for_testing",
            "current_property_read_symbol": "objc3_runtime_read_current_property_i32",
            "current_property_write_symbol": "objc3_runtime_write_current_property_i32",
            "current_property_exchange_symbol": "objc3_runtime_exchange_current_property_i32",
            "weak_current_property_load_symbol": "objc3_runtime_load_weak_current_property_i32",
            "weak_current_property_store_symbol": "objc3_runtime_store_weak_current_property_i32",
            "retain_symbol": "objc3_runtime_retain_i32",
            "release_symbol": "objc3_runtime_release_i32",
            "autorelease_symbol": "objc3_runtime_autorelease_i32",
            "private_testing_surface_only": True,
            "deterministic": True,
        },
    }
    report_path.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"runtime-acceptance: PASS ({report_path})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
