#!/usr/bin/env python3
"""Fail-closed validator for M226-D001 frontend build/invocation integration freeze."""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-d001-frontend-build-invocation-contract-v1"

DEFAULT_BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"
DEFAULT_COMPILE_WRAPPER_SCRIPT = ROOT / "scripts" / "objc3c_native_compile.ps1"
DEFAULT_DRIVER_MAIN_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_driver_main.cpp"
DEFAULT_COMPILATION_DRIVER_CPP = (
    ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_compilation_driver.cpp"
)
DEFAULT_DRIVER_SHELL_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_driver_shell.cpp"
DEFAULT_OBJC3_PATH_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_CAPABILITY_ROUTING_CPP = (
    ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_llvm_capability_routing.cpp"
)
DEFAULT_CLI_OPTIONS_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.cpp"
DEFAULT_CONTRACT_DOC = ROOT / "docs" / "contracts" / "m226_frontend_build_invocation_expectations.md"

HIDDEN_FALLBACK_MARKERS = re.compile(
    r"\b(hidden_fallback|fallback_status|fallback_to_clang|fallback_clang)\b",
    re.IGNORECASE,
)


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build-script", type=Path, default=DEFAULT_BUILD_SCRIPT)
    parser.add_argument("--compile-wrapper-script", type=Path, default=DEFAULT_COMPILE_WRAPPER_SCRIPT)
    parser.add_argument("--driver-main-cpp", type=Path, default=DEFAULT_DRIVER_MAIN_CPP)
    parser.add_argument("--compilation-driver-cpp", type=Path, default=DEFAULT_COMPILATION_DRIVER_CPP)
    parser.add_argument("--driver-shell-cpp", type=Path, default=DEFAULT_DRIVER_SHELL_CPP)
    parser.add_argument("--objc3-path-cpp", type=Path, default=DEFAULT_OBJC3_PATH_CPP)
    parser.add_argument("--capability-routing-cpp", type=Path, default=DEFAULT_CAPABILITY_ROUTING_CPP)
    parser.add_argument("--cli-options-cpp", type=Path, default=DEFAULT_CLI_OPTIONS_CPP)
    parser.add_argument("--contract-doc", type=Path, default=DEFAULT_CONTRACT_DOC)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path("tmp/m226_d001_frontend_build_invocation_contract_summary.json"),
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {display_path(path)}")
    return path.read_text(encoding="utf-8")


def extract_braced_block(text: str, marker: str) -> str:
    start = text.find(marker)
    if start == -1:
        raise ValueError(f"missing marker: {marker}")
    brace_start = text.find("{", start)
    if brace_start == -1:
        raise ValueError(f"missing opening brace after marker: {marker}")

    depth = 0
    for idx in range(brace_start, len(text)):
        ch = text[idx]
        if ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
            if depth == 0:
                return text[brace_start : idx + 1]
    raise ValueError(f"unterminated braced block for marker: {marker}")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    texts = {
        "build_script": load_text(args.build_script, artifact="build_script"),
        "compile_wrapper_script": load_text(args.compile_wrapper_script, artifact="compile_wrapper_script"),
        "driver_main_cpp": load_text(args.driver_main_cpp, artifact="driver_main_cpp"),
        "compilation_driver_cpp": load_text(args.compilation_driver_cpp, artifact="compilation_driver_cpp"),
        "driver_shell_cpp": load_text(args.driver_shell_cpp, artifact="driver_shell_cpp"),
        "objc3_path_cpp": load_text(args.objc3_path_cpp, artifact="objc3_path_cpp"),
        "capability_routing_cpp": load_text(args.capability_routing_cpp, artifact="capability_routing_cpp"),
        "cli_options_cpp": load_text(args.cli_options_cpp, artifact="cli_options_cpp"),
        "contract_doc": load_text(args.contract_doc, artifact="contract_doc"),
    }

    checks: list[tuple[str, str, bool, str]] = []

    def add_check(artifact: str, check_id: str, passed: bool, detail: str) -> None:
        checks.append((artifact, check_id, passed, detail))

    build_script = texts["build_script"]
    compile_wrapper = texts["compile_wrapper_script"]
    driver_main = texts["driver_main_cpp"]
    compilation_driver = texts["compilation_driver_cpp"]
    driver_shell = texts["driver_shell_cpp"]
    objc3_path = texts["objc3_path_cpp"]
    capability_routing = texts["capability_routing_cpp"]
    cli_options = texts["cli_options_cpp"]
    contract_doc = texts["contract_doc"]

    add_check(
        "build_script",
        "M226-BLD-01",
        '$tmpOutDir = Join-Path $repoRoot "tmp/build-objc3c-native"' in build_script,
        "build script must stage transient build outputs under tmp/build-objc3c-native",
    )
    add_check(
        "build_script",
        "M226-BLD-02",
        '$outCapiExe = Join-Path $outDir "objc3c-frontend-c-api-runner.exe"' in build_script,
        "build script must emit deterministic c-api runner artifact path",
    )
    add_check(
        "build_script",
        "M226-BLD-03",
        '"native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp"' in build_script,
        "build script must compile frontend c-api runner source",
    )
    add_check(
        "build_script",
        "M226-BLD-04",
        build_script.count("-DOBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION=1") >= 2,
        "build script must pin llvm-direct object emission define for both binaries",
    )
    add_check(
        "build_script",
        "M226-BLD-05",
        "Publish-ArtifactWithRetry -StagedPath $stagedOutExe -FinalPath $outExe" in build_script
        and "Publish-ArtifactWithRetry -StagedPath $stagedOutCapiExe -FinalPath $outCapiExe" in build_script,
        "build script must publish both staged binaries through retry wrapper",
    )

    add_check(
        "compile_wrapper_script",
        "M226-BLD-06",
        "usage: objc3c_native_compile.ps1 <input>" in compile_wrapper,
        "compile wrapper must retain explicit invocation usage diagnostics",
    )
    add_check(
        "compile_wrapper_script",
        "M226-BLD-07",
        '$buildScript = Join-Path $RepoRoot "scripts/build_objc3c_native.ps1"' in compile_wrapper,
        "compile wrapper must invoke canonical native build script",
    )
    add_check(
        "compile_wrapper_script",
        "M226-BLD-08",
        '$exe = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"' in compile_wrapper,
        "compile wrapper must invoke deterministic objc3c-native binary path",
    )
    add_check(
        "compile_wrapper_script",
        "M226-BLD-09",
        '$cacheRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/cache"' in compile_wrapper,
        "compile wrapper cache artifacts must remain rooted under tmp/",
    )
    try:
        no_cache_block = extract_braced_block(compile_wrapper, "if (-not $parsed.use_cache)")
        no_cache_build_idx = no_cache_block.find("Invoke-BuildNativeCompiler -RepoRoot $repoRoot")
        no_cache_invoke_idx = no_cache_block.find("Invoke-NativeCompiler -ExePath $exe -Arguments $parsed.compile_args")
        no_cache_order_ok = (
            no_cache_build_idx != -1 and no_cache_invoke_idx != -1 and no_cache_build_idx < no_cache_invoke_idx
        )
        add_check(
            "compile_wrapper_script",
            "M226-BLD-10",
            no_cache_order_ok,
            "compile wrapper no-cache path must build before invoking objc3c-native",
        )
    except ValueError as exc:
        add_check(
            "compile_wrapper_script",
            "M226-BLD-10",
            False,
            f"compile wrapper no-cache block missing: {exc}",
        )
    add_check(
        "compile_wrapper_script",
        "M226-BLD-11",
        'Write-Output "cache_hit=true"' in compile_wrapper and 'Write-Output "cache_hit=false"' in compile_wrapper,
        "compile wrapper must emit explicit cache_hit diagnostics",
    )

    add_check(
        "driver_main_cpp",
        "M226-INV-01",
        "ParseObjc3CliOptions(argc, argv, cli_options, cli_error)" in driver_main,
        "driver main must parse objc3 cli options before dispatch",
    )
    add_check(
        "driver_main_cpp",
        "M226-INV-02",
        "ApplyObjc3LLVMCabilityRouting(cli_options, cli_error)" in driver_main,
        "driver main must apply capability routing before compilation dispatch",
    )
    parse_idx = driver_main.find("ParseObjc3CliOptions(argc, argv, cli_options, cli_error)")
    route_idx = driver_main.find("ApplyObjc3LLVMCabilityRouting(cli_options, cli_error)")
    dispatch_idx = driver_main.find("RunObjc3CompilationDriver(cli_options)")
    add_check(
        "driver_main_cpp",
        "M226-INV-03",
        parse_idx != -1 and route_idx != -1 and dispatch_idx != -1 and parse_idx < route_idx < dispatch_idx,
        "driver main must preserve parse -> capability-route -> dispatch order",
    )
    add_check(
        "driver_main_cpp",
        "M226-INV-04",
        'std::cerr << cli_error << "\\n";' in driver_main and driver_main.count("return 2;") >= 2,
        "driver main must surface parse/routing diagnostics to stderr with fail-closed exit code 2",
    )

    validate_idx = compilation_driver.find("ValidateObjc3DriverShellInputs(cli_options, input_kind, shell_error)")
    objc3_idx = compilation_driver.find("RunObjc3LanguagePath(cli_options)")
    objectivec_idx = compilation_driver.find("RunObjectiveCPath(cli_options)")
    add_check(
        "compilation_driver_cpp",
        "M226-INV-05",
        validate_idx != -1 and objc3_idx != -1 and objectivec_idx != -1 and validate_idx < objc3_idx < objectivec_idx,
        "compilation driver must validate shell inputs before deterministic objc3/objective-c path dispatch",
    )

    add_check(
        "driver_shell_cpp",
        "M226-INV-06",
        "input file not found: " in driver_shell,
        "driver shell validation must report missing input diagnostics",
    )
    add_check(
        "driver_shell_cpp",
        "M226-INV-07",
        "clang executable not found: " in driver_shell,
        "driver shell validation must report missing clang diagnostics",
    )
    add_check(
        "driver_shell_cpp",
        "M226-INV-08",
        "llc executable not found: " in driver_shell,
        "driver shell validation must report missing llc diagnostics",
    )

    add_check(
        "objc3_path_cpp",
        "M226-INV-09",
        "if (cli_options.ir_object_backend == Objc3IrObjectBackend::kClang) {" in objc3_path
        and "compile_status = RunIRCompile(cli_options.clang_path, ir_out, object_out);" in objc3_path,
        "objc3 invocation path must keep explicit clang backend compile branch",
    )
    add_check(
        "objc3_path_cpp",
        "M226-INV-10",
        "compile_status = RunIRCompileLLVMDirect(cli_options.llc_path, ir_out, object_out, backend_error);" in objc3_path
        and "if (!backend_error.empty()) {" in objc3_path
        and 'std::cerr << backend_error << "\\n";' in objc3_path,
        "objc3 invocation path must keep explicit llvm-direct branch with backend diagnostics",
    )
    add_check(
        "objc3_path_cpp",
        "M226-INV-11",
        'cli_options.ir_object_backend == Objc3IrObjectBackend::kClang ? "clang\\n" : "llvm-direct\\n"' in objc3_path,
        "objc3 invocation path must emit deterministic backend marker artifact contents",
    )
    add_check(
        "objc3_path_cpp",
        "M226-INV-12",
        HIDDEN_FALLBACK_MARKERS.search(objc3_path) is None,
        "objc3 invocation path must not contain hidden fallback ambiguity markers",
    )

    add_check(
        "capability_routing_cpp",
        "M226-INV-13",
        "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary" in capability_routing,
        "capability routing must fail-closed when backend routing is requested without summary input",
    )
    add_check(
        "capability_routing_cpp",
        "M226-INV-14",
        "summary.llc_supports_filetype_obj ? Objc3IrObjectBackend::kLLVMDirect : Objc3IrObjectBackend::kClang"
        in capability_routing,
        "capability routing must map backend deterministically from llc object capability",
    )
    add_check(
        "capability_routing_cpp",
        "M226-INV-15",
        "clang backend selected but capability summary reports clang unavailable" in capability_routing
        and "llvm-direct backend selected but llc --filetype=obj capability is unavailable" in capability_routing,
        "capability routing must keep explicit missing capability diagnostics for selected backend",
    )
    try:
        routing_body = extract_braced_block(capability_routing, "bool ApplyObjc3LLVMCabilityRouting(")
        add_check(
            "capability_routing_cpp",
            "M226-INV-16",
            "RunIRCompile(" not in routing_body and "RunIRCompileLLVMDirect(" not in routing_body,
            "capability routing layer must not invoke hidden compile fallback paths",
        )
    except ValueError as exc:
        add_check(
            "capability_routing_cpp",
            "M226-INV-16",
            False,
            f"capability routing function drifted: {exc}",
        )
    add_check(
        "capability_routing_cpp",
        "M226-INV-17",
        HIDDEN_FALLBACK_MARKERS.search(capability_routing) is None,
        "capability routing source must not include hidden fallback ambiguity markers",
    )

    add_check(
        "cli_options_cpp",
        "M226-INV-18",
        "--objc3-ir-object-backend <clang|llvm-direct>" in cli_options
        and "--llvm-capabilities-summary <path>" in cli_options
        and "--objc3-route-backend-from-capabilities" in cli_options,
        "cli options must advertise explicit backend and capability-routing switches",
    )
    add_check(
        "cli_options_cpp",
        "M226-INV-19",
        "invalid --objc3-ir-object-backend (expected clang|llvm-direct): " in cli_options,
        "cli parser must keep deterministic invalid backend diagnostic",
    )

    add_check(
        "contract_doc",
        "M226-DOC-01",
        "# Frontend Build and Invocation Integration Expectations (M226-D001)" in contract_doc,
        "contract doc heading must remain pinned for m226-d001",
    )
    add_check(
        "contract_doc",
        "M226-DOC-02",
        "Contract ID: `objc3c-frontend-build-invocation-contract/m226-d001-v1`" in contract_doc,
        "contract doc id must remain pinned",
    )
    add_check(
        "contract_doc",
        "M226-DOC-03",
        "`M226-INV-06`" in contract_doc,
        "contract doc must encode hidden fallback ambiguity freeze requirement",
    )
    add_check(
        "contract_doc",
        "M226-DOC-04",
        "`python scripts/check_m226_d001_frontend_build_invocation_contract.py`" in contract_doc
        and "`python -m pytest tests/tooling/test_check_m226_d001_frontend_build_invocation_contract.py -q`"
        in contract_doc,
        "contract doc must declare checker + pytest verification commands",
    )

    failures = [
        Finding(artifact=artifact, check_id=check_id, detail=detail)
        for artifact, check_id, passed, detail in checks
        if not passed
    ]
    failures.sort(key=lambda item: (item.artifact, item.check_id, item.detail))

    checks_total = len(checks)
    checks_passed = checks_total - len(failures)
    summary = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in failures
        ],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if failures:
        print(
            "m226-d001-frontend-build-invocation-contract: contract drift detected "
            f"({len(failures)} finding(s)).",
            file=sys.stderr,
        )
        for finding in failures:
            print(f"- {finding.artifact}:{finding.check_id} {finding.detail}", file=sys.stderr)
        print("remediation:", file=sys.stderr)
        print("1. Restore M226-D001 build/invocation freeze snippets and diagnostics surfaces.", file=sys.stderr)
        print("2. Re-run: python scripts/check_m226_d001_frontend_build_invocation_contract.py", file=sys.stderr)
        print(
            f"3. Inspect summary: {display_path(args.summary_out)}",
            file=sys.stderr,
        )
        return 1

    print("m226-d001-frontend-build-invocation-contract: OK")
    print(f"- mode={MODE}")
    print(f"- checks_passed={checks_passed}")
    print("- fail_closed=true")
    print(f"- summary={display_path(args.summary_out)}")
    return 0


def main() -> int:
    try:
        return run(sys.argv[1:])
    except Exception as exc:  # noqa: BLE001
        print(f"m226-d001-frontend-build-invocation-contract: error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
