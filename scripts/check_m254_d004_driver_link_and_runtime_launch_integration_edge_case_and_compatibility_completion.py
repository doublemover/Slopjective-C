#!/usr/bin/env python3
"""Fail-closed checker for M254-D004 driver/link/runtime launch integration."""

from __future__ import annotations

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m254-d004-driver-link-and-runtime-launch-integration-edge-case-and-compatibility-completion-v1"
CONTRACT_ID = "objc3c-runtime-launch-integration/m254-d004-v1"
RUNTIME_LIBRARY_RESOLUTION_MODEL = "registration-manifest-runtime-archive-path-is-authoritative"
DRIVER_LINKER_FLAG_CONSUMPTION_MODEL = "registration-manifest-driver-linker-flags-feed-proof-and-smoke-link-commands"
COMPILE_WRAPPER_COMMAND_SURFACE = "scripts/objc3c_native_compile.ps1"
COMPILE_PROOF_COMMAND_SURFACE = "scripts/run_objc3c_native_compile_proof.ps1"
EXECUTION_SMOKE_COMMAND_SURFACE = "scripts/check_objc3c_native_execution_smoke.ps1"
REGISTRATION_MANIFEST_ARTIFACT = "module.runtime-registration-manifest.json"
MAIN_MANIFEST_ARTIFACT = "module.manifest.json"
HELLO_FIXTURE_REL = "tests/tooling/fixtures/native/hello.objc3"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m254_driver_link_and_runtime_launch_integration_edge_case_and_compatibility_completion_d004_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m254" / "m254_d004_driver_link_and_runtime_launch_integration_edge_case_and_compatibility_completion_packet.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_TOOLING_RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_PROCESS_HEADER = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.h"
DEFAULT_PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
DEFAULT_LAUNCH_CONTRACT_SCRIPT = ROOT / "scripts" / "objc3c_runtime_launch_contract.ps1"
DEFAULT_COMPILE_WRAPPER = ROOT / "scripts" / "objc3c_native_compile.ps1"
DEFAULT_COMPILE_PROOF = ROOT / "scripts" / "run_objc3c_native_compile_proof.ps1"
DEFAULT_EXECUTION_SMOKE = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_HELLO_FIXTURE = ROOT / HELLO_FIXTURE_REL
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m254" / "d004-launch-integration"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m254/M254-D004/runtime_launch_integration_summary.json")


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


EXPECTATIONS_SNIPPETS = (
    SnippetCheck("M254-D004-DOC-EXP-01", "# M254 Driver, Link, and Runtime Launch Integration Edge-case and Compatibility Completion Expectations (D004)"),
    SnippetCheck("M254-D004-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M254-D004-DOC-EXP-03", f"`{COMPILE_WRAPPER_COMMAND_SURFACE}`"),
    SnippetCheck("M254-D004-DOC-EXP-04", f"`{EXECUTION_SMOKE_COMMAND_SURFACE}`"),
    SnippetCheck("M254-D004-DOC-EXP-05", "tmp/reports/m254/M254-D004/runtime_launch_integration_summary.json"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M254-D004-DOC-PKT-01", "# M254-D004 Driver, Link, and Runtime Launch Integration Edge-case and Compatibility Completion Packet"),
    SnippetCheck("M254-D004-DOC-PKT-02", "Packet: `M254-D004`"),
    SnippetCheck("M254-D004-DOC-PKT-03", f"`{CONTRACT_ID}`"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M254-D004-NDOC-01", "## Driver, link, and runtime launch integration (M254-D004)"),
    SnippetCheck("M254-D004-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M254-D004-NDOC-03", f"`{COMPILE_PROOF_COMMAND_SURFACE}`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M254-D004-SPC-01", "## M254 driver, link, and runtime launch integration (D004)"),
    SnippetCheck("M254-D004-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M254-D004-SPC-03", f"`{DRIVER_LINKER_FLAG_CONSUMPTION_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M254-D004-META-01", "## M254 launch integration metadata anchors (D004)"),
    SnippetCheck("M254-D004-META-02", "`launch_integration_contract_id`"),
    SnippetCheck("M254-D004-META-03", "`execution_smoke_command_surface`"),
)
TOOLING_RUNTIME_README_SNIPPETS = (
    SnippetCheck("M254-D004-TRTDOC-01", "`M254-D004` then makes the operator launch surfaces consume the same emitted"),
    SnippetCheck("M254-D004-TRTDOC-02", f"`{COMPILE_WRAPPER_COMMAND_SURFACE}`"),
    SnippetCheck("M254-D004-TRTDOC-03", f"`{CONTRACT_ID}`"),
)
FRONTEND_TYPES_SNIPPETS = (
    SnippetCheck("M254-D004-TYPES-01", "std::string launch_integration_contract_id ="),
    SnippetCheck("M254-D004-TYPES-02", "std::string runtime_library_resolution_model ="),
    SnippetCheck("M254-D004-TYPES-03", "std::string driver_linker_flag_consumption_model ="),
    SnippetCheck("M254-D004-TYPES-04", 'launch_integration_ready = false'),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M254-D004-ART-01", "summary.launch_integration_ready ="),
    SnippetCheck("M254-D004-ART-02", '",\\"launch_integration_contract_id\\":\\"'),
    SnippetCheck("M254-D004-ART-03", '",\\"launch_integration_ready\\":'),
)
PROCESS_HEADER_SNIPPETS = (
    SnippetCheck("M254-D004-PROCH-01", 'std::string launch_integration_contract_id;'),
    SnippetCheck("M254-D004-PROCH-02", 'std::string execution_smoke_command_surface;'),
)
PROCESS_CPP_SNIPPETS = (
    SnippetCheck("M254-D004-PROC-01", 'inputs.launch_integration_contract_id'),
    SnippetCheck("M254-D004-PROC-02", '<< "  \\"execution_smoke_command_surface\\": \\""'),
    SnippetCheck("M254-D004-PROC-03", '<< "  \\"launch_integration_ready\\": true,\\n"'),
)
DRIVER_SNIPPETS = (
    SnippetCheck("M254-D004-DRV-01", 'manifest_inputs.launch_integration_contract_id ='),
    SnippetCheck("M254-D004-DRV-02", 'manifest_inputs.execution_smoke_command_surface ='),
)
FRONTEND_ANCHOR_SNIPPETS = (
    SnippetCheck("M254-D004-FRONT-01", 'manifest_inputs.launch_integration_contract_id ='),
    SnippetCheck("M254-D004-FRONT-02", 'manifest_inputs.compile_wrapper_command_surface ='),
)
HELPER_SNIPPETS = (
    SnippetCheck("M254-D004-HELP-01", '$script:Objc3cRuntimeLaunchIntegrationContractId = "objc3c-runtime-launch-integration/m254-d004-v1"'),
    SnippetCheck("M254-D004-HELP-02", 'function Get-Objc3cRuntimeLaunchContract'),
    SnippetCheck("M254-D004-HELP-03", 'driver_linker_flags = @($driverLinkerFlags)'),
)
COMPILE_WRAPPER_SNIPPETS = (
    SnippetCheck("M254-D004-CW-01", '. $runtimeLaunchContractScript'),
    SnippetCheck("M254-D004-CW-02", 'Assert-Objc3cRuntimeLaunchContract -CompileDir $parsed.out_dir -RepoRoot $repoRoot -EmitPrefix $parsed.emit_prefix'),
    SnippetCheck("M254-D004-CW-03", 'Write-Output "cache_hit=true"'),
)
COMPILE_PROOF_SNIPPETS = (
    SnippetCheck("M254-D004-CP-01", '$compileWrapper = Join-Path $repoRoot "scripts/objc3c_native_compile.ps1"'),
    SnippetCheck("M254-D004-CP-02", 'module.runtime-registration-manifest.json'),
    SnippetCheck("M254-D004-CP-03", 'runtime_registration_manifest = $true'),
)
EXECUTION_SMOKE_SNIPPETS = (
    SnippetCheck("M254-D004-SMK-01", '$compileWrapper = Join-Path $repoRoot "scripts/objc3c_native_compile.ps1"'),
    SnippetCheck("M254-D004-SMK-02", 'function Get-RuntimeLaunchLinkContract'),
    SnippetCheck("M254-D004-SMK-03", 'launch_integration_contract_id = $launchContract.launch_integration_contract_id'),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M254-D004-PKG-01", '"check:objc3c:m254-d004-driver-link-and-runtime-launch-integration-edge-case-and-compatibility-completion": "python scripts/check_m254_d004_driver_link_and_runtime_launch_integration_edge_case_and_compatibility_completion.py"'),
    SnippetCheck("M254-D004-PKG-02", '"test:tooling:m254-d004-driver-link-and-runtime-launch-integration-edge-case-and-compatibility-completion": "python -m pytest tests/tooling/test_check_m254_d004_driver_link_and_runtime_launch_integration_edge_case_and_compatibility_completion.py -q"'),
    SnippetCheck("M254-D004-PKG-03", '"check:objc3c:m254-d004-lane-d-readiness": "python scripts/run_m254_d004_lane_d_readiness.py"'),
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M254-D004-MISSING", f"required artifact is missing: {display_path(path)}"))
        return 1 + len(snippets)
    text = read_text(path)
    passed = 1
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def resolve_pwsh() -> str | None:
    return shutil.which("pwsh") or shutil.which("pwsh.exe") or shutil.which("powershell") or shutil.which("powershell.exe")


def run_command(command: Sequence[str], env: dict[str, str] | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=str(ROOT),
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        env=env,
        check=False,
    )


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--tooling-runtime-readme", type=Path, default=DEFAULT_TOOLING_RUNTIME_README)
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--process-header", type=Path, default=DEFAULT_PROCESS_HEADER)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--frontend-anchor-cpp", type=Path, default=DEFAULT_FRONTEND_ANCHOR_CPP)
    parser.add_argument("--launch-contract-script", type=Path, default=DEFAULT_LAUNCH_CONTRACT_SCRIPT)
    parser.add_argument("--compile-wrapper", type=Path, default=DEFAULT_COMPILE_WRAPPER)
    parser.add_argument("--compile-proof", type=Path, default=DEFAULT_COMPILE_PROOF)
    parser.add_argument("--execution-smoke", type=Path, default=DEFAULT_EXECUTION_SMOKE)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--hello-fixture", type=Path, default=DEFAULT_HELLO_FIXTURE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def validate_registration_manifest(
    registration_manifest: dict[str, Any],
    main_manifest: dict[str, Any],
    artifact: str,
    failures: list[Finding],
) -> int:
    semantic_registration_manifest = (
        main_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_runtime_translation_unit_registration_manifest", {})
    )
    checks = 0
    checks += require(
        registration_manifest.get("launch_integration_contract_id") == CONTRACT_ID,
        artifact,
        "M254-D004-DYN-REG-01",
        "registration manifest launch integration contract id mismatch",
        failures,
    )
    checks += require(
        registration_manifest.get("runtime_library_resolution_model") == RUNTIME_LIBRARY_RESOLUTION_MODEL,
        artifact,
        "M254-D004-DYN-REG-02",
        "registration manifest runtime library resolution model mismatch",
        failures,
    )
    checks += require(
        registration_manifest.get("driver_linker_flag_consumption_model") == DRIVER_LINKER_FLAG_CONSUMPTION_MODEL,
        artifact,
        "M254-D004-DYN-REG-03",
        "registration manifest driver linker flag consumption model mismatch",
        failures,
    )
    checks += require(
        registration_manifest.get("compile_wrapper_command_surface") == COMPILE_WRAPPER_COMMAND_SURFACE,
        artifact,
        "M254-D004-DYN-REG-04",
        "registration manifest compile wrapper command surface mismatch",
        failures,
    )
    checks += require(
        registration_manifest.get("compile_proof_command_surface") == COMPILE_PROOF_COMMAND_SURFACE,
        artifact,
        "M254-D004-DYN-REG-05",
        "registration manifest compile proof command surface mismatch",
        failures,
    )
    checks += require(
        registration_manifest.get("execution_smoke_command_surface") == EXECUTION_SMOKE_COMMAND_SURFACE,
        artifact,
        "M254-D004-DYN-REG-06",
        "registration manifest execution smoke command surface mismatch",
        failures,
    )
    checks += require(
        bool(registration_manifest.get("launch_integration_ready")) is True,
        artifact,
        "M254-D004-DYN-REG-07",
        "registration manifest launch integration ready flag must be true",
        failures,
    )
    checks += require(
        registration_manifest.get("manifest_artifact") == REGISTRATION_MANIFEST_ARTIFACT,
        artifact,
        "M254-D004-DYN-REG-08",
        "registration manifest artifact field mismatch",
        failures,
    )
    driver_linker_flags = registration_manifest.get("driver_linker_flags")
    checks += require(
        isinstance(driver_linker_flags, list) and len(driver_linker_flags) > 0,
        artifact,
        "M254-D004-DYN-REG-09",
        "registration manifest driver_linker_flags must be a non-empty list",
        failures,
    )
    checks += require(
        semantic_registration_manifest.get("launch_integration_contract_id") == CONTRACT_ID,
        artifact,
        "M254-D004-DYN-MAN-01",
        "semantic-surface registration manifest launch integration contract mismatch",
        failures,
    )
    checks += require(
        semantic_registration_manifest.get("runtime_library_resolution_model") == RUNTIME_LIBRARY_RESOLUTION_MODEL,
        artifact,
        "M254-D004-DYN-MAN-02",
        "semantic-surface registration manifest runtime library resolution model mismatch",
        failures,
    )
    checks += require(
        semantic_registration_manifest.get("driver_linker_flag_consumption_model") == DRIVER_LINKER_FLAG_CONSUMPTION_MODEL,
        artifact,
        "M254-D004-DYN-MAN-03",
        "semantic-surface registration manifest driver linker flag model mismatch",
        failures,
    )
    checks += require(
        semantic_registration_manifest.get("compile_wrapper_command_surface") == COMPILE_WRAPPER_COMMAND_SURFACE,
        artifact,
        "M254-D004-DYN-MAN-04",
        "semantic-surface registration manifest compile wrapper command mismatch",
        failures,
    )
    checks += require(
        semantic_registration_manifest.get("compile_proof_command_surface") == COMPILE_PROOF_COMMAND_SURFACE,
        artifact,
        "M254-D004-DYN-MAN-05",
        "semantic-surface registration manifest compile proof command mismatch",
        failures,
    )
    checks += require(
        semantic_registration_manifest.get("execution_smoke_command_surface") == EXECUTION_SMOKE_COMMAND_SURFACE,
        artifact,
        "M254-D004-DYN-MAN-06",
        "semantic-surface registration manifest execution smoke command mismatch",
        failures,
    )
    checks += require(
        semantic_registration_manifest.get("manifest_artifact_relative_path") == REGISTRATION_MANIFEST_ARTIFACT,
        artifact,
        "M254-D004-DYN-MAN-07",
        "semantic-surface registration manifest artifact path mismatch",
        failures,
    )
    checks += require(
        bool(semantic_registration_manifest.get("launch_integration_ready")) is True,
        artifact,
        "M254-D004-DYN-MAN-08",
        "semantic-surface registration manifest launch integration ready flag must be true",
        failures,
    )
    return checks


def extract_proof_dir(stdout: str) -> str | None:
    match = re.search(r"(?m)^proof_dir:\s+(.+)$", stdout)
    if match is None:
        return None
    return match.group(1).strip()


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_passed = 0
    checks_total = 0

    snippet_groups = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.tooling_runtime_readme, TOOLING_RUNTIME_README_SNIPPETS),
        (args.frontend_types, FRONTEND_TYPES_SNIPPETS),
        (args.frontend_artifacts, FRONTEND_ARTIFACTS_SNIPPETS),
        (args.process_header, PROCESS_HEADER_SNIPPETS),
        (args.process_cpp, PROCESS_CPP_SNIPPETS),
        (args.driver_cpp, DRIVER_SNIPPETS),
        (args.frontend_anchor_cpp, FRONTEND_ANCHOR_SNIPPETS),
        (args.launch_contract_script, HELPER_SNIPPETS),
        (args.compile_wrapper, COMPILE_WRAPPER_SNIPPETS),
        (args.compile_proof, COMPILE_PROOF_SNIPPETS),
        (args.execution_smoke, EXECUTION_SMOKE_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in snippet_groups:
        checks_total += 1 + len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_probes_executed = False
    compile_case: dict[str, Any] | None = None
    proof_case: dict[str, Any] | None = None
    smoke_case: dict[str, Any] | None = None

    if not args.skip_dynamic_probes:
        pwsh = resolve_pwsh()
        checks_total += 1
        checks_passed += require(pwsh is not None, display_path(args.compile_wrapper), "M254-D004-DYN-TOOL-01", "PowerShell is required for dynamic probes", failures)
        if pwsh is not None:
            dynamic_probes_executed = True
            args.probe_root.mkdir(parents=True, exist_ok=True)

            compile_dir = args.probe_root / "compile-wrapper"
            compile_dir.mkdir(parents=True, exist_ok=True)
            compile_result = run_command(
                [
                    pwsh,
                    "-NoProfile",
                    "-ExecutionPolicy",
                    "Bypass",
                    "-File",
                    str(args.compile_wrapper),
                    str(args.hello_fixture),
                    "--out-dir",
                    str(compile_dir),
                    "--emit-prefix",
                    "module",
                ]
            )
            checks_total += 4
            checks_passed += require(compile_result.returncode == 0, display_path(args.compile_wrapper), "M254-D004-DYN-COMPILE-01", f"compile wrapper exited with {compile_result.returncode}", failures)
            checks_passed += require("cache_hit=true" in compile_result.stdout or "cache_hit=false" in compile_result.stdout, display_path(args.compile_wrapper), "M254-D004-DYN-COMPILE-02", "compile wrapper must report cache_hit state", failures)
            reg_manifest_path = compile_dir / REGISTRATION_MANIFEST_ARTIFACT
            main_manifest_path = compile_dir / MAIN_MANIFEST_ARTIFACT
            checks_passed += require(reg_manifest_path.exists(), display_path(reg_manifest_path), "M254-D004-DYN-COMPILE-03", "registration manifest is missing after wrapper compile", failures)
            checks_passed += require(main_manifest_path.exists(), display_path(main_manifest_path), "M254-D004-DYN-COMPILE-04", "main manifest is missing after wrapper compile", failures)
            registration_manifest = load_json(reg_manifest_path) if reg_manifest_path.exists() else {}
            main_manifest = load_json(main_manifest_path) if main_manifest_path.exists() else {}
            checks_total += 17
            checks_passed += validate_registration_manifest(registration_manifest, main_manifest, display_path(reg_manifest_path), failures)
            compile_case = {
                "compile_dir": display_path(compile_dir),
                "stdout": compile_result.stdout.strip(),
                "stderr": compile_result.stderr.strip(),
                "registration_manifest": display_path(reg_manifest_path),
                "main_manifest": display_path(main_manifest_path),
                "driver_linker_flags": registration_manifest.get("driver_linker_flags", []),
            }

            proof_result = run_command(
                [
                    pwsh,
                    "-NoProfile",
                    "-ExecutionPolicy",
                    "Bypass",
                    "-File",
                    str(args.compile_proof),
                ]
            )
            checks_total += 8
            checks_passed += require(proof_result.returncode == 0, display_path(args.compile_proof), "M254-D004-DYN-PROOF-01", f"compile proof exited with {proof_result.returncode}", failures)
            checks_passed += require("status: PASS" in proof_result.stdout, display_path(args.compile_proof), "M254-D004-DYN-PROOF-02", "compile proof must report PASS", failures)
            proof_dir_rel = extract_proof_dir(proof_result.stdout)
            proof_dir = ROOT / proof_dir_rel if proof_dir_rel else None
            checks_passed += require(proof_dir is not None, display_path(args.compile_proof), "M254-D004-DYN-PROOF-03", "compile proof did not emit proof_dir", failures)
            digest_path = proof_dir / "digest.json" if proof_dir is not None else Path()
            checks_passed += require(proof_dir is not None and digest_path.exists(), display_path(digest_path if proof_dir is not None else args.compile_proof), "M254-D004-DYN-PROOF-04", "compile proof digest is missing", failures)
            proof_digest = load_json(digest_path) if proof_dir is not None and digest_path.exists() else {}
            deterministic = proof_digest.get("deterministic", {}) if isinstance(proof_digest, dict) else {}
            checks_passed += require(deterministic.get("runtime_registration_manifest") is True, display_path(digest_path), "M254-D004-DYN-PROOF-05", "compile proof must report deterministic runtime_registration_manifest", failures)
            checks_passed += require(deterministic.get("launch_contract") is True, display_path(digest_path), "M254-D004-DYN-PROOF-06", "compile proof must report deterministic launch_contract", failures)
            checks_passed += require(proof_digest.get("launch_integration_contract_id") == CONTRACT_ID, display_path(digest_path), "M254-D004-DYN-PROOF-07", "compile proof launch contract id mismatch", failures)
            checks_passed += require(isinstance(proof_digest.get("driver_linker_flags"), list) and len(proof_digest.get("driver_linker_flags", [])) > 0, display_path(digest_path), "M254-D004-DYN-PROOF-08", "compile proof driver linker flags must be recorded", failures)
            proof_case = {
                "proof_dir": display_path(proof_dir) if proof_dir is not None else "",
                "digest": display_path(digest_path) if proof_dir is not None else "",
                "stdout": proof_result.stdout.strip(),
                "stderr": proof_result.stderr.strip(),
            }

            smoke_run_id = "m254_d004_launch_contract"
            smoke_env = os.environ.copy()
            smoke_env["OBJC3C_NATIVE_EXECUTION_RUN_ID"] = smoke_run_id
            smoke_result = run_command(
                [
                    pwsh,
                    "-NoProfile",
                    "-ExecutionPolicy",
                    "Bypass",
                    "-File",
                    str(args.execution_smoke),
                ],
                env=smoke_env,
            )
            smoke_summary_path = ROOT / "tmp" / "artifacts" / "objc3c-native" / "execution-smoke" / smoke_run_id / "summary.json"
            checks_total += 10
            checks_passed += require(smoke_result.returncode == 0, display_path(args.execution_smoke), "M254-D004-DYN-SMOKE-01", f"execution smoke exited with {smoke_result.returncode}", failures)
            checks_passed += require(smoke_summary_path.exists(), display_path(smoke_summary_path), "M254-D004-DYN-SMOKE-02", "execution smoke summary is missing", failures)
            smoke_summary = load_json(smoke_summary_path) if smoke_summary_path.exists() else {}
            checks_passed += require(smoke_summary.get("status") == "PASS", display_path(smoke_summary_path), "M254-D004-DYN-SMOKE-03", "execution smoke summary must report PASS", failures)
            checks_passed += require(smoke_summary.get("compile_wrapper") == COMPILE_WRAPPER_COMMAND_SURFACE, display_path(smoke_summary_path), "M254-D004-DYN-SMOKE-04", "execution smoke must report compile wrapper command surface", failures)
            checks_passed += require(smoke_summary.get("runtime_launch_contract_script") == "scripts/objc3c_runtime_launch_contract.ps1", display_path(smoke_summary_path), "M254-D004-DYN-SMOKE-05", "execution smoke must report runtime launch contract helper", failures)
            results = smoke_summary.get("results", []) if isinstance(smoke_summary.get("results"), list) else []
            checks_passed += require(len(results) > 0, display_path(smoke_summary_path), "M254-D004-DYN-SMOKE-06", "execution smoke must record case results", failures)
            positive_results = [row for row in results if row.get("kind") == "positive"]
            checks_passed += require(len(positive_results) > 0, display_path(smoke_summary_path), "M254-D004-DYN-SMOKE-07", "execution smoke must include positive cases", failures)
            positive_with_contract = [row for row in positive_results if row.get("launch_integration_contract_id") == CONTRACT_ID and row.get("registration_manifest") == REGISTRATION_MANIFEST_ARTIFACT]
            checks_passed += require(len(positive_with_contract) == len(positive_results), display_path(smoke_summary_path), "M254-D004-DYN-SMOKE-08", "all positive smoke cases must record launch contract fields", failures)
            shim_positive_results = [row for row in positive_results if row.get("requires_runtime_shim")]
            checks_passed += require(all(isinstance(row.get("driver_linker_flags"), list) and len(row.get("driver_linker_flags")) > 0 for row in shim_positive_results), display_path(smoke_summary_path), "M254-D004-DYN-SMOKE-09", "runtime-shim positive cases must record emitted driver_linker_flags", failures)
            checks_passed += require(all(row.get("runtime_library_source") == "registration-manifest.runtime_support_library_archive_relative_path" for row in shim_positive_results), display_path(smoke_summary_path), "M254-D004-DYN-SMOKE-10", "runtime-shim positive cases must resolve runtime library from registration manifest", failures)
            smoke_case = {
                "run_dir": smoke_summary.get("run_dir", ""),
                "summary": display_path(smoke_summary_path),
                "stdout": smoke_result.stdout.strip(),
                "stderr": smoke_result.stderr.strip(),
            }

    summary_payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": len(failures) == 0,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_probes_executed,
        "compile_case": compile_case,
        "proof_case": proof_case,
        "smoke_case": smoke_case,
        "failures": [finding.__dict__ for finding in failures],
    }
    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(run())
