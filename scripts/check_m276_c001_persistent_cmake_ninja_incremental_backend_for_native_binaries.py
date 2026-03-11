#!/usr/bin/env python3
"""Fail-closed checker for M276-C001 persistent CMake/Ninja backend."""

from __future__ import annotations

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
import time
from dataclasses import dataclass
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
MODE = "m276-c001-persistent-cmake-ninja-incremental-backend-v1"
CONTRACT_ID = "objc3c-persistent-cmake-ninja-native-build-backend/m276-c001-v1"
BACKEND_MODEL = "wrapper-owned-entrypoint-with-persistent-cmake-ninja-native-binary-backend"
PUBLICATION_MODEL = "cmake-build-tree-under-tmp-with-stable-final-artifacts-under-artifacts"
INCREMENTAL_PROOF_MODEL = "warm-reuse-plus-single-source-timestamp-bump-rebuild-scope-proof"
NEXT_ISSUE = "M276-C002"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "m276" / "M276-C001" / "persistent_cmake_ninja_incremental_backend_summary.json"
LOG_OUT = ROOT / "tmp" / "reports" / "m276" / "M276-C001" / "incremental_rebuild_verbose.log"

EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m276_persistent_cmake_ninja_incremental_backend_for_native_binaries_c001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m276" / "m276_c001_persistent_cmake_ninja_incremental_backend_for_native_binaries_packet.md"
DOC_SOURCE = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
README = ROOT / "README.md"
ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"
PACKAGE_JSON = ROOT / "package.json"
READINESS_RUNNER = ROOT / "scripts" / "run_m276_c001_lane_c_readiness.py"
TEST_FILE = ROOT / "tests" / "tooling" / "test_check_m276_c001_persistent_cmake_ninja_incremental_backend_for_native_binaries.py"

BUILD_DIR = ROOT / "tmp" / "build-objc3c-native"
COMPILE_COMMANDS = BUILD_DIR / "compile_commands.json"
FINGERPRINT = BUILD_DIR / "native_build_backend_fingerprint.json"
ARTIFACT_NATIVE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
ARTIFACT_CAPI = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
ARTIFACT_RUNTIME = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
TIMESTAMP_BUMP_SOURCE = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parse_support.cpp"

COMPILE_LINE_RE = re.compile(r"-c\s+(?P<source>[^\s]+)")


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
        SnippetCheck("M276-C001-EXP-01", "# M276 Persistent CMake/Ninja Incremental Backend For Native Binaries Expectations (C001)"),
        SnippetCheck("M276-C001-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
        SnippetCheck("M276-C001-EXP-03", "The contract must explicitly hand off to `M276-C002`."),
    ),
    PACKET_DOC: (
        SnippetCheck("M276-C001-PKT-01", "# M276-C001 Persistent CMake/Ninja Incremental Backend For Native Binaries Packet"),
        SnippetCheck("M276-C001-PKT-02", "Issue: `#7387`"),
        SnippetCheck("M276-C001-PKT-03", "`M276-C002` is the explicit next handoff after this implementation closes."),
    ),
    DOC_SOURCE: (
        SnippetCheck("M276-C001-DOC-01", "## Persistent CMake/Ninja incremental backend (M276-C001)"),
        SnippetCheck("M276-C001-DOC-02", "`objc3c-persistent-cmake-ninja-native-build-backend/m276-c001-v1`"),
        SnippetCheck("M276-C001-DOC-03", "`tmp/build-objc3c-native/compile_commands.json`"),
        SnippetCheck("M276-C001-DOC-04", "packet-family decomposition remains deferred to `M276-C002` and `M276-C003`"),
    ),
    NATIVE_DOC: (
        SnippetCheck("M276-C001-NDOC-01", "## Persistent CMake/Ninja incremental backend (M276-C001)"),
        SnippetCheck("M276-C001-NDOC-02", "`tmp/build-objc3c-native/compile_commands.json`"),
        SnippetCheck("M276-C001-NDOC-03", "`M276-C002`"),
    ),
    README: (
        SnippetCheck("M276-C001-README-01", "PowerShell wrapper over a persistent"),
        SnippetCheck("M276-C001-README-02", "CMake/Ninja build tree under `tmp/build-objc3c-native`"),
        SnippetCheck("M276-C001-README-03", "native binary builds route through CMake/Ninja"),
        SnippetCheck("M276-C001-README-04", "`M276-C002` is the next issue."),
    ),
    ARCHITECTURE_DOC: (
        SnippetCheck("M276-C001-ARCH-01", "## M276 Persistent CMake/Ninja Incremental Backend (C001)"),
        SnippetCheck("M276-C001-ARCH-02", "`tmp/build-objc3c-native`"),
        SnippetCheck("M276-C001-ARCH-03", "`M276-C002` is the next issue"),
    ),
    BUILD_SCRIPT: (
        SnippetCheck("M276-C001-BUILD-01", "M276-C001 native-binary-backend anchor"),
        SnippetCheck("M276-C001-BUILD-02", "native binary compilation now routes through a persistent CMake/Ninja build"),
        SnippetCheck("M276-C001-BUILD-03", "wrapper still owns frontend packet generation"),
        SnippetCheck("M276-C001-BUILD-04", "cmake_build_start=native-binaries"),
    ),
    CMAKE_FILE: (
        SnippetCheck("M276-C001-CMAKE-01", "M276-C001 native-binary-backend anchor"),
        SnippetCheck("M276-C001-CMAKE-02", "persistent native binary backend under"),
        SnippetCheck("M276-C001-CMAKE-03", "canonical executables publish to `artifacts/bin`"),
        SnippetCheck("M276-C001-CMAKE-04", "canonical runtime archives publish to `artifacts/lib`"),
        SnippetCheck("M276-C001-CMAKE-05", "set(CMAKE_EXPORT_COMPILE_COMMANDS ON)"),
        SnippetCheck("M276-C001-CMAKE-06", "add_library(objc3c_libclang UNKNOWN IMPORTED)"),
    ),
    PACKAGE_JSON: (
        SnippetCheck("M276-C001-PKG-01", '"check:objc3c:m276-c001-persistent-cmake-ninja-incremental-backend": "python scripts/check_m276_c001_persistent_cmake_ninja_incremental_backend_for_native_binaries.py"'),
        SnippetCheck("M276-C001-PKG-02", '"test:tooling:m276-c001-persistent-cmake-ninja-incremental-backend": "python -m pytest tests/tooling/test_check_m276_c001_persistent_cmake_ninja_incremental_backend_for_native_binaries.py -q"'),
        SnippetCheck("M276-C001-PKG-03", '"check:objc3c:m276-c001-lane-c-readiness": "python scripts/run_m276_c001_lane_c_readiness.py"'),
    ),
    READINESS_RUNNER: (
        SnippetCheck("M276-C001-RUN-01", "scripts/build_objc3c_native_docs.py"),
        SnippetCheck("M276-C001-RUN-02", "check_m276_c001_persistent_cmake_ninja_incremental_backend_for_native_binaries.py"),
        SnippetCheck("M276-C001-RUN-03", "test_check_m276_c001_persistent_cmake_ninja_incremental_backend_for_native_binaries.py"),
    ),
    TEST_FILE: (
        SnippetCheck("M276-C001-TEST-01", "def test_m276_c001_checker_emits_summary() -> None:"),
        SnippetCheck("M276-C001-TEST-02", CONTRACT_ID),
    ),
}


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def check_static_contract(path: Path, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    findings: list[Finding] = []
    checks_total = len(snippets) + 1
    if not path.exists():
        findings.append(Finding(display_path(path), "STATIC-EXISTS", f"missing required artifact: {display_path(path)}"))
        return checks_total, findings
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def run(cmd: list[str], cwd: Path = ROOT) -> subprocess.CompletedProcess[str]:
    return subprocess.run(cmd, cwd=cwd, capture_output=True, text=True, check=False)


def resolve_llvm_root() -> Path:
    candidate = os.environ.get("LLVM_ROOT")
    if candidate:
        path = Path(candidate)
        if path.exists():
            return path
    fallback = Path(r"C:\Program Files\LLVM")
    if fallback.exists():
        return fallback
    raise FileNotFoundError("LLVM_ROOT not set and C:/Program Files/LLVM not found")


def resolve_tool(name: str) -> Path:
    found = shutil.which(name)
    if found:
        return Path(found)
    raise FileNotFoundError(f"required tool not found on PATH: {name}")


def resolve_clangxx(llvm_root: Path) -> Path:
    candidate = llvm_root / "bin" / "clang++.exe"
    if candidate.exists():
        return candidate
    found = shutil.which("clang++")
    if found:
        return Path(found)
    raise FileNotFoundError("clang++ not found")


def resolve_libclang(llvm_root: Path) -> Path:
    for name in ("libclang.lib", "clang.lib"):
        candidate = llvm_root / "lib" / name
        if candidate.exists():
            return candidate
    raise FileNotFoundError("libclang import library not found")


def analyze_incremental_log(log_text: str) -> tuple[list[str], list[str]]:
    compile_sources: list[str] = []
    link_steps: list[str] = []
    for line in log_text.splitlines():
        if " -c " in line:
            match = COMPILE_LINE_RE.search(line)
            if match:
                compile_sources.append(match.group("source").replace("\\", "/"))
        if "objc3c-native.exe" in line or "objc3c_parse.lib" in line:
            link_steps.append(line)
    return compile_sources, link_steps


def run_cold_proof(summary_dir: Path) -> tuple[int, list[Finding], dict[str, Any]]:
    findings: list[Finding] = []
    checks_total = 0

    llvm_root = resolve_llvm_root()
    clangxx = resolve_clangxx(llvm_root)
    cmake = resolve_tool("cmake")
    ninja = resolve_tool("ninja")
    libclang = resolve_libclang(llvm_root)
    include_dir = llvm_root / "include"
    if not include_dir.exists():
        raise FileNotFoundError(f"LLVM include dir missing: {include_dir}")

    cold_root = summary_dir / f"cold-build-{int(time.time())}"
    cold_build_dir = cold_root / "build"
    cold_bin_dir = cold_root / "artifacts" / "bin"
    cold_lib_dir = cold_root / "artifacts" / "lib"
    cold_bin_dir.mkdir(parents=True, exist_ok=True)
    cold_lib_dir.mkdir(parents=True, exist_ok=True)

    configure = run([
        str(cmake), "-S", str(ROOT / "native" / "objc3c"), "-B", str(cold_build_dir), "-G", "Ninja",
        f"-DCMAKE_MAKE_PROGRAM={ninja}",
        f"-DCMAKE_CXX_COMPILER={clangxx}",
        "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
        "-DOBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION=ON",
        f"-DOBJC3C_LLVM_INCLUDE_DIR={include_dir}",
        f"-DOBJC3C_LIBCLANG_LIBRARY={libclang}",
        f"-DOBJC3C_RUNTIME_OUTPUT_DIR={cold_bin_dir}",
        f"-DOBJC3C_LIBRARY_OUTPUT_DIR={cold_lib_dir}",
    ])
    (summary_dir / "cold_configure.log").write_text(configure.stdout + configure.stderr, encoding="utf-8")
    checks_total += 1
    if configure.returncode != 0:
        findings.append(Finding("dynamic/cold-configure", "M276-C001-DYN-COLD-CONFIGURE", f"cold configure failed: {configure.returncode}"))
        return checks_total, findings, {}

    build = run([
        str(cmake), "--build", str(cold_build_dir), "--parallel", "--target", "objc3c-native", "objc3c-frontend-c-api-runner", "objc3_runtime",
    ])
    (summary_dir / "cold_build.log").write_text(build.stdout + build.stderr, encoding="utf-8")
    checks_total += 1
    if build.returncode != 0:
        findings.append(Finding("dynamic/cold-build", "M276-C001-DYN-COLD-BUILD", f"cold build failed: {build.returncode}"))

    checks_total += 4
    for label, path in {
        "cold_native": cold_bin_dir / "objc3c-native.exe",
        "cold_capi": cold_bin_dir / "objc3c-frontend-c-api-runner.exe",
        "cold_runtime": cold_lib_dir / "objc3_runtime.lib",
        "cold_compile_commands": cold_build_dir / "compile_commands.json",
    }.items():
        if not path.exists():
            findings.append(Finding("dynamic/cold-build", f"M276-C001-DYN-{label.upper()}", f"missing cold-proof artifact: {display_path(path)}"))

    return checks_total, findings, {
        "cold_build_dir": display_path(cold_build_dir),
        "cold_compile_commands": display_path(cold_build_dir / "compile_commands.json"),
        "cold_native": display_path(cold_bin_dir / "objc3c-native.exe"),
        "cold_capi": display_path(cold_bin_dir / "objc3c-frontend-c-api-runner.exe"),
        "cold_runtime": display_path(cold_lib_dir / "objc3_runtime.lib"),
        "cold_cmake": display_path(cmake),
        "cold_target_source": display_path(TIMESTAMP_BUMP_SOURCE),
    }


def dynamic_proof(summary_dir: Path) -> tuple[int, list[Finding], dict[str, Any]]:
    findings: list[Finding] = []
    checks_total = 0

    wrapper_cmd = ["pwsh", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(BUILD_SCRIPT)]
    wrapper_first = run(wrapper_cmd)
    (summary_dir / "wrapper_first.log").write_text(wrapper_first.stdout + wrapper_first.stderr, encoding="utf-8")
    checks_total += 1
    if wrapper_first.returncode != 0:
        findings.append(Finding("dynamic/wrapper-first", "M276-C001-DYN-WRAPPER-FIRST", f"wrapper build failed: {wrapper_first.returncode}"))
    else:
        if "cmake_build_start=native-binaries" not in wrapper_first.stdout:
            findings.append(Finding("dynamic/wrapper-first", "M276-C001-DYN-WRAPPER-FIRST-CMAKE", "wrapper did not report native CMake/Ninja build start"))
        if "compile_commands=tmp/build-objc3c-native/compile_commands.json" not in wrapper_first.stdout:
            findings.append(Finding("dynamic/wrapper-first", "M276-C001-DYN-WRAPPER-FIRST-COMPILE-COMMANDS", "wrapper did not report compile_commands publication"))

    cold_total, cold_findings, cold_summary = run_cold_proof(summary_dir)
    checks_total += cold_total
    findings.extend(cold_findings)

    wrapper_second = run(wrapper_cmd)
    (summary_dir / "wrapper_second.log").write_text(wrapper_second.stdout + wrapper_second.stderr, encoding="utf-8")
    checks_total += 1
    if wrapper_second.returncode != 0:
        findings.append(Finding("dynamic/wrapper-second", "M276-C001-DYN-WRAPPER-SECOND", f"second wrapper build failed: {wrapper_second.returncode}"))
    else:
        if "cmake_configure=reuse" not in wrapper_second.stdout:
            findings.append(Finding("dynamic/wrapper-second", "M276-C001-DYN-WRAPPER-REUSE", "wrapper did not reuse the persistent build tree"))

    checks_total += 5
    for label, path in {
        "native": ARTIFACT_NATIVE,
        "capi": ARTIFACT_CAPI,
        "runtime": ARTIFACT_RUNTIME,
        "compile_commands": COMPILE_COMMANDS,
        "fingerprint": FINGERPRINT,
    }.items():
        if not path.exists():
            findings.append(Finding("dynamic/published-artifacts", f"M276-C001-DYN-{label.upper()}", f"missing required output: {display_path(path)}"))

    scratch_build_dir = ROOT / cold_summary["cold_build_dir"]
    scratch_cmake = Path(cold_summary["cold_cmake"])
    warm = run([str(scratch_cmake), "--build", str(scratch_build_dir), "--parallel", "--target", "objc3c-native", "--verbose"])
    (summary_dir / "warm_noop.log").write_text(warm.stdout + warm.stderr, encoding="utf-8")
    checks_total += 1
    if warm.returncode != 0:
        findings.append(Finding("dynamic/warm-noop", "M276-C001-DYN-WARM-NOOP", f"scratch warm rebuild failed: {warm.returncode}"))
    elif "ninja: no work to do." not in warm.stdout + warm.stderr:
        findings.append(Finding("dynamic/warm-noop", "M276-C001-DYN-WARM-NOOP-MISS", "scratch warm rebuild did not collapse to a no-op ninja invocation"))

    timestamp_now = time.time()
    os.utime(TIMESTAMP_BUMP_SOURCE, (timestamp_now, timestamp_now))
    incremental = run([str(scratch_cmake), "--build", str(scratch_build_dir), "--parallel", "--verbose", "--target", "objc3c-native"])
    LOG_OUT.parent.mkdir(parents=True, exist_ok=True)
    LOG_OUT.write_text(incremental.stdout + incremental.stderr, encoding="utf-8")
    checks_total += 1
    if incremental.returncode != 0:
        findings.append(Finding("dynamic/incremental", "M276-C001-DYN-INCREMENTAL", f"incremental rebuild failed: {incremental.returncode}"))

    compile_sources, link_steps = analyze_incremental_log(incremental.stdout + incremental.stderr)
    target_source = TIMESTAMP_BUMP_SOURCE.resolve().as_posix()
    checks_total += 3
    if compile_sources.count(target_source) != 1:
        findings.append(Finding("dynamic/incremental", "M276-C001-DYN-INCREMENTAL-SOURCE", f"expected exactly one compile of {target_source}, observed {compile_sources.count(target_source)}"))
    unrelated = [src for src in compile_sources if src != target_source]
    if unrelated:
        findings.append(Finding("dynamic/incremental", "M276-C001-DYN-INCREMENTAL-UNRELATED", f"unexpected unrelated compile(s): {', '.join(unrelated)}"))
    if not any("objc3c_parse.lib" in step or "objc3c-native.exe" in step for step in link_steps):
        findings.append(Finding("dynamic/incremental", "M276-C001-DYN-INCREMENTAL-LINK", "incremental rebuild did not show the expected narrow archive/link follow-up"))

    return checks_total, findings, {
        "wrapper_first_log": display_path(summary_dir / "wrapper_first.log"),
        "wrapper_second_log": display_path(summary_dir / "wrapper_second.log"),
        "warm_noop_log": display_path(summary_dir / "warm_noop.log"),
        "incremental_log": display_path(LOG_OUT),
        "compile_commands_path": display_path(COMPILE_COMMANDS),
        "fingerprint_path": display_path(FINGERPRINT),
        "incremental_compile_sources": compile_sources,
        "incremental_compile_hits": compile_sources.count(target_source),
        "incremental_unrelated_sema_hits": len(unrelated),
        "incremental_link_steps": link_steps,
        **cold_summary,
    }


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    failures: list[Finding] = []
    checks_total = 0
    for path, snippets in STATIC_SNIPPETS.items():
        total, local_failures = check_static_contract(path, snippets)
        checks_total += total
        failures.extend(local_failures)

    dynamic_total = 0
    dynamic_summary: dict[str, Any] = {}
    if not failures:
        summary_dir = args.summary_out.parent
        dynamic_total, dynamic_failures, dynamic_summary = dynamic_proof(summary_dir)
        checks_total += dynamic_total
        failures.extend(dynamic_failures)

    summary: dict[str, Any] = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "backend_model": BACKEND_MODEL,
        "publication_model": PUBLICATION_MODEL,
        "incremental_proof_model": INCREMENTAL_PROOF_MODEL,
        "next_issue": NEXT_ISSUE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
        "dynamic_summary": dynamic_summary,
        "findings": [finding.__dict__ for finding in failures],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        print(f"[info] wrote summary to {display_path(args.summary_out)}", file=sys.stderr)
        return 1
    print(f"[ok] M276-C001 persistent CMake/Ninja backend validated ({checks_total} checks)")
    print(f"[info] wrote summary to {display_path(args.summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
