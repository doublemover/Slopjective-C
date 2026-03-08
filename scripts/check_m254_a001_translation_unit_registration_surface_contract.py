#!/usr/bin/env python3
"""Fail-closed contract checker for M254-A001 translation-unit registration surface."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m254-a001-translation-unit-registration-surface-contract-v1"
CONTRACT_ID = "objc3c-translation-unit-registration-surface-freeze/m254-a001-v1"
BINARY_BOUNDARY_CONTRACT_ID = "objc3c-executable-metadata-runtime-ingest-binary-boundary/m252-d002-v1"
ARCHIVE_STATIC_LINK_CONTRACT_ID = "objc3c-runtime-metadata-archive-and-static-link-discovery/m253-d003-v1"
OBJECT_EMISSION_CLOSEOUT_CONTRACT_ID = "objc3c-runtime-cross-lane-object-emission-closeout/m253-e002-v1"
RUNTIME_SUPPORT_LINK_WIRING_CONTRACT_ID = "objc3c-runtime-support-library-link-wiring/m251-d003-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_runtime_translation_unit_registration_contract"
PAYLOAD_MODEL = "runtime-metadata-binary-plus-linker-retention-sidecars-v1"
PAYLOAD_ARTIFACT = "module.runtime-metadata.bin"
LINKER_RESPONSE_ARTIFACT = "module.runtime-metadata-linker-options.rsp"
DISCOVERY_ARTIFACT = "module.runtime-metadata-discovery.json"
CONSTRUCTOR_ROOT_SYMBOL = "__objc3_runtime_register_image_ctor"
CONSTRUCTOR_ROOT_OWNERSHIP_MODEL = "compiler-emits-constructor-root-runtime-owns-registration-state"
CONSTRUCTOR_EMISSION_MODE = "reserved-not-emitted-yet"
CONSTRUCTOR_PRIORITY_POLICY = "deferred-until-m254-a002"
REGISTRATION_ENTRYPOINT_SYMBOL = "objc3_runtime_register_image"
TRANSLATION_UNIT_IDENTITY_MODEL = "input-path-plus-parse-and-lowering-replay"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m254_translation_unit_registration_surface_contract_and_architecture_freeze_a001_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m254" / "m254_a001_translation_unit_registration_surface_contract_and_architecture_freeze_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DEFAULT_RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
DEFAULT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m254" / "a001-translation-unit-registration-surface"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m254/M254-A001/translation_unit_registration_surface_contract_summary.json")


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
    SnippetCheck("M254-A001-DOC-EXP-01", "# M254 Translation-Unit Registration Surface Contract and Architecture Freeze Expectations (A001)"),
    SnippetCheck("M254-A001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M254-A001-DOC-EXP-03", f"`{SURFACE_PATH}`"),
    SnippetCheck("M254-A001-DOC-EXP-04", f"`{PAYLOAD_MODEL}`"),
    SnippetCheck("M254-A001-DOC-EXP-05", f"`{CONSTRUCTOR_ROOT_SYMBOL}`"),
    SnippetCheck("M254-A001-DOC-EXP-06", f"`{CONSTRUCTOR_ROOT_OWNERSHIP_MODEL}`"),
    SnippetCheck("M254-A001-DOC-EXP-07", f"`{REGISTRATION_ENTRYPOINT_SYMBOL}`"),
    SnippetCheck("M254-A001-DOC-EXP-08", "`M254-A002`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M254-A001-DOC-PKT-01", "# M254-A001 Translation-Unit Registration Surface Contract and Architecture Freeze Packet"),
    SnippetCheck("M254-A001-DOC-PKT-02", "Packet: `M254-A001`"),
    SnippetCheck("M254-A001-DOC-PKT-03", f"- Contract id `{CONTRACT_ID}`"),
    SnippetCheck("M254-A001-DOC-PKT-04", f"`{PAYLOAD_MODEL}`"),
    SnippetCheck("M254-A001-DOC-PKT-05", f"`{TRANSLATION_UNIT_IDENTITY_MODEL}`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M254-A001-ARCH-01", "M254 lane-A A001 translation-unit registration freeze anchors the"),
    SnippetCheck("M254-A001-ARCH-02", "runtime-metadata binary, linker-retention"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M254-A001-NDOC-01", "## Translation-unit registration surface freeze (M254-A001)"),
    SnippetCheck("M254-A001-NDOC-02", f"`{SURFACE_PATH}`"),
    SnippetCheck("M254-A001-NDOC-03", f"`{CONSTRUCTOR_ROOT_SYMBOL}`"),
    SnippetCheck("M254-A001-NDOC-04", f"`{REGISTRATION_ENTRYPOINT_SYMBOL}`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M254-A001-SPC-01", "## M254 translation-unit registration surface freeze (A001)"),
    SnippetCheck("M254-A001-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M254-A001-SPC-03", f"`{PAYLOAD_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M254-A001-META-01", "## M254 translation-unit registration metadata anchors (A001)"),
    SnippetCheck("M254-A001-META-02", f"`{SURFACE_PATH}`"),
    SnippetCheck("M254-A001-META-03", f"`{CONSTRUCTOR_ROOT_SYMBOL}`"),
)
AST_SNIPPETS = (
    SnippetCheck("M254-A001-AST-01", "kObjc3RuntimeTranslationUnitRegistrationContractId"),
    SnippetCheck("M254-A001-AST-02", "kObjc3RuntimeTranslationUnitRegistrationSurfacePath"),
    SnippetCheck("M254-A001-AST-03", "kObjc3RuntimeTranslationUnitRegistrationPayloadModel"),
    SnippetCheck("M254-A001-AST-04", "kObjc3RuntimeTranslationUnitRegistrationConstructorRootSymbol"),
)
FRONTEND_TYPES_SNIPPETS = (
    SnippetCheck("M254-A001-TYPES-01", "struct Objc3RuntimeTranslationUnitRegistrationContractSummary {"),
    SnippetCheck("M254-A001-TYPES-02", "std::array<std::string, 3u> runtime_owned_payload_artifacts = {"),
    SnippetCheck("M254-A001-TYPES-03", "std::string constructor_root_symbol ="),
    SnippetCheck("M254-A001-TYPES-04", "inline bool IsReadyObjc3RuntimeTranslationUnitRegistrationContractSummary("),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M254-A001-ART-01", "BuildRuntimeTranslationUnitRegistrationContractSummary("),
    SnippetCheck("M254-A001-ART-02", "BuildRuntimeTranslationUnitRegistrationContractSummaryJson("),
    SnippetCheck("M254-A001-ART-03", "objc_runtime_translation_unit_registration_contract"),
    SnippetCheck("M254-A001-ART-04", "runtime_translation_unit_registration_contract_id"),
)
DRIVER_SNIPPETS = (
    SnippetCheck("M254-A001-DRV-01", "M254-A001 translation-unit registration surface freeze"),
    SnippetCheck("M254-A001-DRV-02", "objc3_runtime_register_image"),
)
PROCESS_SNIPPETS = (
    SnippetCheck("M254-A001-PROC-01", "M254-A001 translation-unit registration surface freeze"),
    SnippetCheck("M254-A001-PROC-02", "translation-unit identity"),
)
RUNTIME_README_SNIPPETS = (
    SnippetCheck("M254-A001-RUN-01", "`M254-A001` freezes the next startup-registration handoff"),
    SnippetCheck("M254-A001-RUN-02", f"`{PAYLOAD_ARTIFACT}`"),
    SnippetCheck("M254-A001-RUN-03", f"`{CONSTRUCTOR_ROOT_OWNERSHIP_MODEL}`"),
)
FIXTURE_SNIPPETS = (
    SnippetCheck("M254-A001-FIX-01", "module Demo;"),
    SnippetCheck("M254-A001-FIX-02", "fn main() {"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M254-A001-PKG-01", '"check:objc3c:m254-a001-translation-unit-registration-surface-contract": "python scripts/check_m254_a001_translation_unit_registration_surface_contract.py"'),
    SnippetCheck("M254-A001-PKG-02", '"test:tooling:m254-a001-translation-unit-registration-surface-contract": "python -m pytest tests/tooling/test_check_m254_a001_translation_unit_registration_surface_contract.py -q"'),
    SnippetCheck("M254-A001-PKG-03", '"check:objc3c:m254-a001-lane-a-readiness": "npm run build:objc3c-native && npm run check:objc3c:m254-a001-translation-unit-registration-surface-contract && npm run test:tooling:m254-a001-translation-unit-registration-surface-contract"'),
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--ast-header", type=Path, default=DEFAULT_AST_HEADER)
    parser.add_argument("--frontend-types", type=Path, default=DEFAULT_FRONTEND_TYPES)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--runtime-readme", type=Path, default=DEFAULT_RUNTIME_README)
    parser.add_argument("--fixture", type=Path, default=DEFAULT_FIXTURE)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def check_doc_contract(*, path: Path, exists_check_id: str, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists():
        findings.append(Finding(display_path(path), exists_check_id, f"required artifact is missing: {display_path(path)}"))
        return checks_total, findings
    if not path.is_file():
        findings.append(Finding(display_path(path), exists_check_id, f"required path is not a file: {display_path(path)}"))
        return checks_total, findings
    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 1


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def run_native(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(command, capture_output=True, text=True, check=False)


def run_manifest_case(*, native_exe: Path, fixture_path: Path, out_dir: Path) -> tuple[int, list[Finding], dict[str, object] | None]:
    findings: list[Finding] = []
    checks_total = 0
    checks_total += require(fixture_path.exists(), display_path(fixture_path), "M254-A001-FIXTURE-EXISTS", "fixture is missing", findings)
    checks_total += require(native_exe.exists(), display_path(native_exe), "M254-A001-NATIVE-EXISTS", "native executable is missing", findings)
    if findings:
        return checks_total, findings, None

    out_dir.mkdir(parents=True, exist_ok=True)
    command = [
        str(native_exe),
        str(fixture_path),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ]
    completed = run_native(command)

    manifest_path = out_dir / "module.manifest.json"
    payload_path = out_dir / PAYLOAD_ARTIFACT
    linker_response_path = out_dir / LINKER_RESPONSE_ARTIFACT
    discovery_path = out_dir / DISCOVERY_ARTIFACT
    object_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"

    checks_total += require(completed.returncode == 0, display_path(out_dir), "M254-A001-NATIVE-EXIT", "native compile must succeed", findings)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "M254-A001-MANIFEST", "manifest is missing", findings)
    checks_total += require(payload_path.exists(), display_path(payload_path), "M254-A001-PAYLOAD", "runtime metadata binary is missing", findings)
    checks_total += require(linker_response_path.exists(), display_path(linker_response_path), "M254-A001-LINKER-RSP", "linker-response artifact is missing", findings)
    checks_total += require(discovery_path.exists(), display_path(discovery_path), "M254-A001-DISCOVERY", "discovery artifact is missing", findings)
    checks_total += require(object_path.exists(), display_path(object_path), "M254-A001-OBJECT", "object artifact is missing", findings)
    checks_total += require(backend_path.exists(), display_path(backend_path), "M254-A001-BACKEND", "backend marker is missing", findings)
    if findings:
        return checks_total, findings, None

    manifest_payload = load_json(manifest_path)
    discovery_payload = load_json(discovery_path)
    backend_text = backend_path.read_text(encoding="utf-8").strip()
    linker_response_text = linker_response_path.read_text(encoding="utf-8").strip()

    frontend = manifest_payload.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    semantic_surface = pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    sema_pass_manager = pipeline.get("sema_pass_manager") if isinstance(pipeline, dict) else None
    registration = semantic_surface.get("objc_runtime_translation_unit_registration_contract") if isinstance(semantic_surface, dict) else None

    checks_total += require(isinstance(registration, dict), display_path(manifest_path), "M254-A001-SURFACE-EXISTS", "translation-unit registration packet missing from semantic surface", findings)
    checks_total += require(isinstance(sema_pass_manager, dict), display_path(manifest_path), "M254-A001-FLAT-EXISTS", "flattened sema pass manager summary missing", findings)
    if findings:
        return checks_total, findings, None

    checks_total += require(registration.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M254-A001-CONTRACT", "contract id mismatch", findings)
    checks_total += require(registration.get("binary_boundary_contract_id") == BINARY_BOUNDARY_CONTRACT_ID, display_path(manifest_path), "M254-A001-BINARY-CONTRACT", "binary-boundary contract mismatch", findings)
    checks_total += require(registration.get("archive_static_link_contract_id") == ARCHIVE_STATIC_LINK_CONTRACT_ID, display_path(manifest_path), "M254-A001-ARCHIVE-CONTRACT", "archive/static-link contract mismatch", findings)
    checks_total += require(registration.get("object_emission_closeout_contract_id") == OBJECT_EMISSION_CLOSEOUT_CONTRACT_ID, display_path(manifest_path), "M254-A001-CLOSEOUT-CONTRACT", "closeout contract mismatch", findings)
    checks_total += require(registration.get("runtime_support_library_link_wiring_contract_id") == RUNTIME_SUPPORT_LINK_WIRING_CONTRACT_ID, display_path(manifest_path), "M254-A001-RUNTIME-CONTRACT", "runtime support link-wiring contract mismatch", findings)
    checks_total += require(registration.get("registration_surface_path") == SURFACE_PATH, display_path(manifest_path), "M254-A001-SURFACE-PATH", "surface path mismatch", findings)
    checks_total += require(registration.get("registration_payload_model") == PAYLOAD_MODEL, display_path(manifest_path), "M254-A001-PAYLOAD-MODEL", "payload model mismatch", findings)
    checks_total += require(registration.get("runtime_owned_payload_artifact_count") == 3, display_path(manifest_path), "M254-A001-PAYLOAD-COUNT", "payload artifact count mismatch", findings)
    checks_total += require(registration.get("runtime_owned_payload_artifacts") == [PAYLOAD_ARTIFACT, LINKER_RESPONSE_ARTIFACT, DISCOVERY_ARTIFACT], display_path(manifest_path), "M254-A001-PAYLOAD-LIST", "payload artifact list mismatch", findings)
    checks_total += require(registration.get("constructor_root_symbol") == CONSTRUCTOR_ROOT_SYMBOL, display_path(manifest_path), "M254-A001-CONSTRUCTOR-SYMBOL", "constructor-root symbol mismatch", findings)
    checks_total += require(registration.get("constructor_root_ownership_model") == CONSTRUCTOR_ROOT_OWNERSHIP_MODEL, display_path(manifest_path), "M254-A001-CONSTRUCTOR-OWNERSHIP", "constructor-root ownership mismatch", findings)
    checks_total += require(registration.get("constructor_emission_mode") == CONSTRUCTOR_EMISSION_MODE, display_path(manifest_path), "M254-A001-CONSTRUCTOR-MODE", "constructor emission mode mismatch", findings)
    checks_total += require(registration.get("constructor_priority_policy") == CONSTRUCTOR_PRIORITY_POLICY, display_path(manifest_path), "M254-A001-CONSTRUCTOR-PRIORITY", "constructor priority policy mismatch", findings)
    checks_total += require(registration.get("registration_entrypoint_symbol") == REGISTRATION_ENTRYPOINT_SYMBOL, display_path(manifest_path), "M254-A001-ENTRYPOINT", "registration entrypoint mismatch", findings)
    checks_total += require(registration.get("translation_unit_identity_model") == TRANSLATION_UNIT_IDENTITY_MODEL, display_path(manifest_path), "M254-A001-TU-MODEL", "translation-unit identity model mismatch", findings)
    checks_total += require(registration.get("ready") is True, display_path(manifest_path), "M254-A001-READY", "registration surface must be ready", findings)
    checks_total += require(registration.get("boundary_frozen") is True, display_path(manifest_path), "M254-A001-BOUNDARY", "boundary must be frozen", findings)
    checks_total += require(registration.get("fail_closed") is True, display_path(manifest_path), "M254-A001-FAIL-CLOSED", "surface must be fail-closed", findings)
    checks_total += require(registration.get("constructor_root_reserved_not_emitted") is True, display_path(manifest_path), "M254-A001-CONSTRUCTOR-RESERVED", "constructor root must remain reserved-not-emitted", findings)
    checks_total += require(registration.get("startup_registration_not_yet_landed") is True, display_path(manifest_path), "M254-A001-STARTUP-NONGOAL", "startup non-goal must remain explicit", findings)
    checks_total += require(registration.get("runtime_bootstrap_not_yet_landed") is True, display_path(manifest_path), "M254-A001-BOOTSTRAP-NONGOAL", "runtime bootstrap non-goal must remain explicit", findings)
    checks_total += require(registration.get("ready_for_registration_manifest_implementation") is True, display_path(manifest_path), "M254-A001-READY-FOR-A002", "ready-for-registration-manifest-implementation must be true", findings)
    checks_total += require(bool(registration.get("binary_boundary_replay_key")), display_path(manifest_path), "M254-A001-BINARY-REPLAY", "binary-boundary replay key must be non-empty", findings)
    checks_total += require(bool(registration.get("replay_key")), display_path(manifest_path), "M254-A001-REPLAY", "registration replay key must be non-empty", findings)
    checks_total += require(registration.get("failure_reason") == "", display_path(manifest_path), "M254-A001-FAILURE-REASON", "failure reason must be empty", findings)

    checks_total += require(sema_pass_manager.get("runtime_translation_unit_registration_contract_id") == CONTRACT_ID, display_path(manifest_path), "M254-A001-FLAT-CONTRACT", "flattened contract id mismatch", findings)
    checks_total += require(sema_pass_manager.get("runtime_translation_unit_registration_payload_model") == PAYLOAD_MODEL, display_path(manifest_path), "M254-A001-FLAT-PAYLOAD", "flattened payload model mismatch", findings)
    checks_total += require(sema_pass_manager.get("runtime_translation_unit_registration_constructor_root_symbol") == CONSTRUCTOR_ROOT_SYMBOL, display_path(manifest_path), "M254-A001-FLAT-CONSTRUCTOR", "flattened constructor-root symbol mismatch", findings)
    checks_total += require(sema_pass_manager.get("runtime_translation_unit_registration_entrypoint_symbol") == REGISTRATION_ENTRYPOINT_SYMBOL, display_path(manifest_path), "M254-A001-FLAT-ENTRYPOINT", "flattened registration entrypoint mismatch", findings)
    checks_total += require(sema_pass_manager.get("runtime_translation_unit_registration_ready_for_manifest_implementation") is True, display_path(manifest_path), "M254-A001-FLAT-READY", "flattened ready flag mismatch", findings)

    checks_total += require(discovery_payload.get("translation_unit_identity_model") == TRANSLATION_UNIT_IDENTITY_MODEL, display_path(discovery_path), "M254-A001-DISCOVERY-TU-MODEL", "discovery translation-unit identity model mismatch", findings)
    driver_flags = discovery_payload.get("driver_linker_flags")
    checks_total += require(isinstance(driver_flags, list) and len(driver_flags) == 1 and isinstance(driver_flags[0], str), display_path(discovery_path), "M254-A001-DISCOVERY-FLAGS", "discovery linker flags must contain one string flag", findings)
    if isinstance(driver_flags, list) and driver_flags and isinstance(driver_flags[0], str):
        checks_total += require(driver_flags[0] in linker_response_text, display_path(linker_response_path), "M254-A001-RSP-FLAG", "linker response must contain discovery linker flag", findings)
    checks_total += require(backend_text == "llvm-direct", display_path(backend_path), "M254-A001-BACKEND-TEXT", "backend marker must remain llvm-direct", findings)

    case_payload: dict[str, object] = {
        "fixture": display_path(fixture_path),
        "manifest_path": display_path(manifest_path),
        "payload_artifact": display_path(payload_path),
        "linker_response_artifact": display_path(linker_response_path),
        "discovery_artifact": display_path(discovery_path),
        "backend": backend_text,
        "translation_unit_identity_key": discovery_payload.get("translation_unit_identity_key"),
        "registration_replay_key": registration.get("replay_key"),
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }
    return checks_total, findings, case_payload


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(list(argv) if argv is not None else sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    dynamic_cases: list[dict[str, object]] = []

    doc_checks: tuple[tuple[Path, str, tuple[SnippetCheck, ...]], ...] = (
        (args.expectations_doc, "M254-A001-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M254-A001-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M254-A001-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.native_doc, "M254-A001-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M254-A001-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M254-A001-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.ast_header, "M254-A001-AST-EXISTS", AST_SNIPPETS),
        (args.frontend_types, "M254-A001-TYPES-EXISTS", FRONTEND_TYPES_SNIPPETS),
        (args.frontend_artifacts, "M254-A001-ART-EXISTS", FRONTEND_ARTIFACTS_SNIPPETS),
        (args.driver_cpp, "M254-A001-DRV-EXISTS", DRIVER_SNIPPETS),
        (args.process_cpp, "M254-A001-PROC-EXISTS", PROCESS_SNIPPETS),
        (args.runtime_readme, "M254-A001-RUN-EXISTS", RUNTIME_README_SNIPPETS),
        (args.fixture, "M254-A001-FIX-EXISTS", FIXTURE_SNIPPETS),
        (args.package_json, "M254-A001-PKG-EXISTS", PACKAGE_SNIPPETS),
    )
    for path, exists_check_id, snippets in doc_checks:
        added_checks, added_failures = check_doc_contract(
            path=path, exists_check_id=exists_check_id, snippets=snippets
        )
        checks_total += added_checks
        failures.extend(added_failures)

    dynamic_probes_executed = not args.skip_dynamic_probes
    if dynamic_probes_executed:
        case_checks, case_failures, case_payload = run_manifest_case(
            native_exe=args.native_exe.resolve(),
            fixture_path=args.fixture.resolve(),
            out_dir=args.probe_root.resolve() / "hello-translation-unit-registration-surface",
        )
        checks_total += case_checks
        failures.extend(case_failures)
        if case_payload is not None:
            dynamic_cases.append(case_payload)

    checks_passed = checks_total - len(failures)
    payload = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_probes_executed,
        "dynamic_cases": dynamic_cases,
        "next_implementation_issue": "M254-A002",
        "failures": [failure.__dict__ for failure in failures],
    }

    summary_out = args.summary_out
    if not summary_out.is_absolute():
        summary_out = ROOT / summary_out
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(canonical_json(payload), encoding="utf-8")

    if failures:
        print(f"[FAIL] M254-A001 translation-unit registration surface drift; summary: {display_path(summary_out)}", file=sys.stderr)
        return 1
    print(f"[PASS] M254-A001 translation-unit registration surface preserved; summary: {display_path(summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run())
