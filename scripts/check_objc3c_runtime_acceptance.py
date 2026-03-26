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
    if not obj_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {obj_path}")
    if not manifest_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {manifest_path}")
    return obj_path


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
        check_property_reflection_case(clangxx, run_dir),
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
    }
    report_path.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"runtime-acceptance: PASS ({report_path})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
