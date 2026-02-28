#!/usr/bin/env python3
"""Fail-closed validator for M142 CLI/C API frontend lowering parity wiring."""

from __future__ import annotations

import json
import sys
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MODE = "m142-frontend-lowering-parity-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "parity_script": ROOT / "scripts" / "check_objc3c_library_cli_parity.py",
    "c_api_runner": ROOT / "native" / "objc3c" / "src" / "tools" / "objc3c_frontend_c_api_runner.cpp",
    "cmake": ROOT / "native" / "objc3c" / "CMakeLists.txt",
    "build_script": ROOT / "scripts" / "build_objc3c_native.ps1",
    "parity_tooling_test": ROOT / "tests" / "tooling" / "test_objc3c_library_cli_parity.py",
    "runner_tooling_test": ROOT / "tests" / "tooling" / "test_objc3c_c_api_runner_extraction.py",
    "surface_tooling_test": ROOT / "tests" / "tooling" / "test_objc3c_frontend_lowering_parity_contract.py",
    "cli_fragment": ROOT / "docs" / "objc3c-native" / "src" / "10-cli.md",
    "semantics_fragment": ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md",
    "artifacts_fragment": ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md",
    "tests_fragment": ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md",
    "contract_doc": ROOT / "docs" / "contracts" / "frontend_lowering_parity_expectations.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "parity_script": (
        ("M142-PAR-01", 'MODE = "objc3c-library-cli-parity-v2"'),
        ("M142-PAR-02", '"--source"'),
        ("M142-PAR-03", '"--cli-bin"'),
        ("M142-PAR-04", '"--c-api-bin"'),
        ("M142-PAR-05", '"--cli-ir-object-backend"'),
        ("M142-PAR-06", 'choices=("clang", "llvm-direct")'),
        ("M142-PAR-07", 'run_command("c-api", c_api_command)'),
        ("M142-PAR-08", 'f"{emit_prefix}.obj"'),
    ),
    "c_api_runner": (
        ("M142-RUN-01", "objc3c-frontend-c-api-runner-v1"),
        ("M142-RUN-02", "--objc3-max-message-args"),
        ("M142-RUN-03", "--objc3-runtime-dispatch-symbol"),
        ("M142-RUN-04", 'options.out_dir / (options.emit_prefix + ".c_api_summary.json")'),
        ("M142-RUN-05", "ExitCodeFromStatus"),
    ),
    "cmake": (
        ("M142-CMK-01", "add_executable(objc3c-frontend-c-api-runner"),
        ("M142-CMK-02", "src/tools/objc3c_frontend_c_api_runner.cpp"),
        ("M142-CMK-03", "target_link_libraries(objc3c-frontend-c-api-runner PRIVATE"),
        ("M142-CMK-04", "  objc3c_frontend"),
    ),
    "build_script": (
        ("M142-BLD-01", 'objc3c-frontend-c-api-runner.exe'),
        ("M142-BLD-02", '"native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp"'),
    ),
    "parity_tooling_test": (
        ("M142-TST-01", "test_parity_source_mode_generates_and_compares_cli_and_c_api_outputs"),
    ),
    "runner_tooling_test": (
        ("M142-TST-02", "test_c_api_runner_uses_c_shim_compile_path"),
        ("M142-TST-03", "test_c_api_runner_reports_summary_and_cli_contract"),
    ),
    "surface_tooling_test": (
        ("M142-TST-04", "test_m142_package_and_docs_wiring_contract"),
    ),
    "cli_fragment": (
        ("M142-DOCCLI-01", "## C API parity runner usage (M142-E001)"),
        ("M142-DOCCLI-02", "objc3c-frontend-c-api-runner <input>"),
        ("M142-DOCCLI-03", "--no-emit-object"),
    ),
    "semantics_fragment": (
        ("M142-DOCS-01", "## Frontend lowering parity harness contract (M142-E001)"),
        ("M142-DOCS-02", "scripts/check_objc3c_library_cli_parity.py"),
        ("M142-DOCS-03", "objc3c-frontend-c-api-runner"),
    ),
    "artifacts_fragment": (
        ("M142-DOCA-01", "## Frontend lowering parity harness artifacts (M142-E001)"),
        ("M142-DOCA-02", "module.object-backend.txt"),
        ("M142-DOCA-03", "npm run check:objc3c:library-cli-parity:source"),
    ),
    "tests_fragment": (
        ("M142-DOCT-01", "npm run test:objc3c:m142-lowering-parity"),
        ("M142-DOCT-02", "npm run check:compiler-closeout:m142"),
    ),
    "contract_doc": (
        ("M142-DOC-01", "# Frontend Lowering Parity Harness Expectations (M142)"),
        ("M142-DOC-02", "Contract ID: `objc3c-frontend-lowering-parity-contract/m142-v1`"),
        ("M142-DOC-03", "`python scripts/check_m142_frontend_lowering_parity_contract.py`"),
    ),
}

REQUIRED_PACKAGE_SCRIPTS = {
    "check:objc3c:library-cli-parity:source": (
        "python scripts/check_objc3c_library_cli_parity.py",
        "--source",
        "--cli-bin artifacts/bin/objc3c-native.exe",
        "--c-api-bin artifacts/bin/objc3c-frontend-c-api-runner.exe",
        "--cli-ir-object-backend clang",
    ),
    "test:objc3c:m142-lowering-parity": (
        "tests/tooling/test_objc3c_library_cli_parity.py",
        "tests/tooling/test_objc3c_c_api_runner_extraction.py",
        "tests/tooling/test_objc3c_frontend_lowering_parity_contract.py",
        "tests/tooling/test_objc3c_sema_cli_c_api_parity_surface.py",
    ),
    "check:compiler-closeout:m142": (
        "python scripts/check_m142_frontend_lowering_parity_contract.py",
        "npm run test:objc3c:m142-lowering-parity",
        '--glob "docs/contracts/frontend_lowering_parity_expectations.md"',
    ),
    "check:task-hygiene": (
        "check:compiler-closeout:m142",
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
    return findings


def _collect_package_findings(package_path: Path) -> list[Finding]:
    payload = json.loads(_load_text(package_path, "package_json"))
    scripts = payload.get("scripts")
    findings: list[Finding] = []
    if not isinstance(scripts, dict):
        return [Finding("package_json", "M142-PKG-00", "package.json scripts field must be an object")]

    for script_name, required_tokens in REQUIRED_PACKAGE_SCRIPTS.items():
        script_value = scripts.get(script_name)
        if not isinstance(script_value, str):
            findings.append(Finding("package_json", "M142-PKG-01", f"missing scripts['{script_name}']"))
            continue
        for index, token in enumerate(required_tokens, start=1):
            if token not in script_value:
                findings.append(
                    Finding(
                        "package_json",
                        f"M142-PKG-{script_name}-{index}",
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
            f"m142-frontend-lowering-parity-contract: contract drift detected ({len(findings)} finding(s)).",
            file=sys.stderr,
        )
        for finding in findings:
            print(f"- {finding.artifact}:{finding.check_id} {finding.detail}", file=sys.stderr)
        print("remediation:", file=sys.stderr)
        print("1. Restore M142 parity harness source/docs/package snippets.", file=sys.stderr)
        print("2. Re-run: python scripts/check_m142_frontend_lowering_parity_contract.py", file=sys.stderr)
        return 1

    checks_passed = sum(len(v) for v in REQUIRED_SNIPPETS.values())
    checks_passed += sum(len(v) for v in REQUIRED_PACKAGE_SCRIPTS.values())

    print("m142-frontend-lowering-parity-contract: OK")
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
        print(f"m142-frontend-lowering-parity-contract: error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
