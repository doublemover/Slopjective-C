#!/usr/bin/env python3
"""Fail-closed validator for M141 CMake targetization and linkage topology wiring."""

from __future__ import annotations

import json
import sys
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MODE = "m141-cmake-target-topology-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "cmake": ROOT / "native" / "objc3c" / "CMakeLists.txt",
    "main_cpp": ROOT / "native" / "objc3c" / "src" / "main.cpp",
    "driver_main_cpp": ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_driver_main.cpp",
    "build_script": ROOT / "scripts" / "build_objc3c_native.ps1",
    "driver_test": ROOT / "tests" / "tooling" / "test_objc3c_driver_cli_extraction.py",
    "topology_test": ROOT / "tests" / "tooling" / "test_objc3c_cmake_target_topology.py",
    "semantics_fragment": ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md",
    "artifacts_fragment": ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md",
    "tests_fragment": ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md",
    "contract_doc": ROOT / "docs" / "contracts" / "cmake_target_linkage_topology_expectations.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "cmake": (
        ("M141-CMK-01", "add_library(objc3c_parse STATIC"),
        ("M141-CMK-02", "target_link_libraries(objc3c_parse PUBLIC"),
        ("M141-CMK-03", "add_library(objc3c_sema_type_system INTERFACE)"),
        ("M141-CMK-04", "target_link_libraries(objc3c_lower PUBLIC"),
        ("M141-CMK-05", "target_link_libraries(objc3c_ir PUBLIC"),
        ("M141-CMK-06", "add_library(objc3c_runtime_abi STATIC"),
        ("M141-CMK-07", "target_link_libraries(objc3c_io PUBLIC"),
        ("M141-CMK-08", "src/driver/objc3_driver_main.cpp"),
        ("M141-CMK-09", "target_link_libraries(objc3c-native PRIVATE"),
        ("M141-CMK-10", "  objc3c_driver"),
    ),
    "main_cpp": (
        ("M141-DRV-01", '#include "driver/objc3_driver_main.h"'),
        ("M141-DRV-02", "RunObjc3DriverMain(argc, argv)"),
    ),
    "driver_main_cpp": (
        ("M141-DRV-03", "ParseObjc3CliOptions(argc, argv, cli_options, cli_error)"),
        ("M141-DRV-04", "RunObjc3CompilationDriver(cli_options)"),
    ),
    "build_script": (
        ("M141-BLD-01", '"native/objc3c/src/driver/objc3_driver_main.cpp"'),
    ),
    "driver_test": (
        ("M141-TST-01", "test_cmake_target_linkage_topology_is_split_by_stage"),
    ),
    "topology_test": (
        ("M141-TST-02", "test_stage_libraries_define_forward_only_linkage_topology"),
        ("M141-TST-03", "test_native_executable_links_through_driver_aggregate_target"),
    ),
    "semantics_fragment": (
        ("M141-DOCS-01", "## CMake targetization and linkage topology contract (M141-E001)"),
        ("M141-DOCS-02", "native/objc3c/CMakeLists.txt"),
    ),
    "artifacts_fragment": (
        ("M141-DOCA-01", "## CMake targetization and linkage topology validation artifacts (M141-E001)"),
        ("M141-DOCA-02", "npm run check:compiler-closeout:m141"),
    ),
    "tests_fragment": (
        ("M141-DOCT-01", "npm run test:objc3c:m141-target-topology"),
        ("M141-DOCT-02", "npm run check:compiler-closeout:m141"),
    ),
    "contract_doc": (
        ("M141-DOC-01", "# CMake Targetization and Linkage Topology Expectations (M141)"),
        ("M141-DOC-02", "Contract ID: `objc3c-cmake-target-topology-contract/m141-v1`"),
        ("M141-DOC-03", "`python scripts/check_m141_cmake_target_topology_contract.py`"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "main_cpp": (
        ("M141-FORB-01", "ParseObjc3CliOptions(argc, argv, cli_options, cli_error)"),
        ("M141-FORB-02", "RunObjc3CompilationDriver(cli_options)"),
    ),
}

REQUIRED_PACKAGE_SCRIPTS = {
    "test:objc3c:m141-target-topology": (
        "tests/tooling/test_objc3c_driver_cli_extraction.py",
        "tests/tooling/test_objc3c_cmake_target_topology.py",
        "tests/tooling/test_objc3c_process_io_extraction.py",
    ),
    "check:compiler-closeout:m141": (
        "python scripts/check_m141_cmake_target_topology_contract.py",
        "npm run test:objc3c:m141-target-topology",
        '--glob "docs/contracts/cmake_target_linkage_topology_expectations.md"',
    ),
    "check:task-hygiene": (
        "check:compiler-closeout:m141",
    ),
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def _display(path: Path) -> str:
    try:
        return path.resolve().relative_to(ROOT).as_posix()
    except ValueError:
        return path.resolve().as_posix()


def _load_text(path: Path, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {_display(path)}")
    return path.read_text(encoding="utf-8")


def _collect_text_findings(artifact: str, text: str) -> list[Finding]:
    findings: list[Finding] = []
    for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
        if snippet not in text:
            findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))
    for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
        if snippet in text:
            findings.append(Finding(artifact, check_id, f"forbidden snippet present: {snippet}"))
    return findings


def _collect_package_findings(package_path: Path) -> list[Finding]:
    payload = json.loads(_load_text(package_path, "package_json"))
    scripts = payload.get("scripts")
    findings: list[Finding] = []
    if not isinstance(scripts, dict):
        return [Finding("package_json", "M141-PKG-00", "package.json scripts field must be an object")]

    for script_name, required_tokens in REQUIRED_PACKAGE_SCRIPTS.items():
        script_value = scripts.get(script_name)
        if not isinstance(script_value, str):
            findings.append(Finding("package_json", "M141-PKG-01", f"missing scripts['{script_name}']"))
            continue
        for index, token in enumerate(required_tokens, start=1):
            if token not in script_value:
                findings.append(
                    Finding(
                        "package_json",
                        f"M141-PKG-{script_name}-{index}",
                        f"scripts['{script_name}'] missing token: {token}",
                    )
                )
    return findings


def check() -> int:
    findings: list[Finding] = []
    for artifact, path in ARTIFACTS.items():
        if artifact == "package_json":
            continue
        findings.extend(_collect_text_findings(artifact, _load_text(path, artifact)))
    findings.extend(_collect_package_findings(ARTIFACTS["package_json"]))
    findings.sort(key=lambda item: (item.artifact, item.check_id))

    if findings:
        print(
            f"m141-cmake-target-topology-contract: contract drift detected ({len(findings)} finding(s)).",
            file=sys.stderr,
        )
        for finding in findings:
            print(f"- {finding.artifact}:{finding.check_id} {finding.detail}", file=sys.stderr)
        print("remediation:", file=sys.stderr)
        print("1. Restore M141 CMake/linkage topology source/docs/package snippets.", file=sys.stderr)
        print("2. Re-run: python scripts/check_m141_cmake_target_topology_contract.py", file=sys.stderr)
        return 1

    checks_passed = sum(len(v) for v in REQUIRED_SNIPPETS.values())
    checks_passed += sum(len(v) for v in FORBIDDEN_SNIPPETS.values())
    checks_passed += sum(len(v) for v in REQUIRED_PACKAGE_SCRIPTS.values())

    print("m141-cmake-target-topology-contract: OK")
    print(f"- mode={MODE}")
    for artifact, path in ARTIFACTS.items():
        print(f"- {artifact}={_display(path)}")
    print(f"- checks_passed={checks_passed}")
    print("- fail_closed=true")
    return 0


def main() -> int:
    try:
        return check()
    except Exception as exc:  # noqa: BLE001
        print(f"m141-cmake-target-topology-contract: error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
