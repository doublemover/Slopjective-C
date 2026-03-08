#!/usr/bin/env python3
"""Fail-closed contract checker for M254-B001 bootstrap invariants."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m254-b001-bootstrap-invariants-contract-v1"
CONTRACT_ID = "objc3c-runtime-startup-bootstrap-invariants/m254-b001-v1"
REGISTRATION_MANIFEST_CONTRACT_ID = (
    "objc3c-translation-unit-registration-manifest/m254-a002-v1"
)
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_runtime_startup_bootstrap_invariants"
DUPLICATE_REGISTRATION_POLICY = "fail-closed-by-translation-unit-identity-key"
REALIZATION_ORDER_POLICY = "constructor-root-then-registration-manifest-order"
FAILURE_MODE = "abort-before-user-main-no-partial-registration-commit"
IMAGE_LOCAL_INITIALIZATION_SCOPE = "runtime-owned-image-local-registration-state"
CONSTRUCTOR_ROOT_UNIQUENESS_POLICY = (
    "one-startup-root-per-translation-unit-identity"
)
CONSTRUCTOR_ROOT_CONSUMPTION_MODEL = "startup-root-consumes-registration-manifest"
STARTUP_EXECUTION_MODE = "deferred-until-m254-c001"
CONSTRUCTOR_ROOT_SYMBOL = "__objc3_runtime_register_image_ctor"
REGISTRATION_ENTRYPOINT_SYMBOL = "objc3_runtime_register_image"
MANIFEST_AUTHORITY_MODEL = "registration-manifest-authoritative-for-constructor-root-shape"
TRANSLATION_UNIT_IDENTITY_MODEL = "input-path-plus-parse-and-lowering-replay"
REGISTRATION_MANIFEST_ARTIFACT = "module.runtime-registration-manifest.json"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m254_bootstrap_invariants_contract_and_architecture_freeze_b001_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m254" / "m254_b001_bootstrap_invariants_contract_and_architecture_freeze_packet.md"
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
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m254" / "b001-bootstrap-invariants"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m254/M254-B001/bootstrap_invariants_contract_summary.json"
)


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
    SnippetCheck(
        "M254-B001-DOC-EXP-01",
        "# M254 Bootstrap Invariants Contract and Architecture Freeze Expectations (B001)",
    ),
    SnippetCheck("M254-B001-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M254-B001-DOC-EXP-03", f"`{SURFACE_PATH}`"),
    SnippetCheck(
        "M254-B001-DOC-EXP-04",
        f"`{DUPLICATE_REGISTRATION_POLICY}`",
    ),
    SnippetCheck("M254-B001-DOC-EXP-05", "`M254-B002`"),
)
PACKET_SNIPPETS = (
    SnippetCheck(
        "M254-B001-DOC-PKT-01",
        "# M254-B001 Bootstrap Invariants Contract and Architecture Freeze Packet",
    ),
    SnippetCheck("M254-B001-DOC-PKT-02", "Packet: `M254-B001`"),
    SnippetCheck("M254-B001-DOC-PKT-03", "Dependencies: none"),
    SnippetCheck("M254-B001-DOC-PKT-04", f"- Contract id `{CONTRACT_ID}`"),
    SnippetCheck(
        "M254-B001-DOC-PKT-05",
        f"`{REGISTRATION_MANIFEST_CONTRACT_ID}`",
    ),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck(
        "M254-B001-ARCH-01",
        "M254 lane-B B001 startup-bootstrap invariant freeze publishes one semantic",
    ),
    SnippetCheck("M254-B001-ARCH-02", "duplicate\n  registration, realization order"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck(
        "M254-B001-NDOC-01",
        "## Bootstrap invariants (M254-B001)",
    ),
    SnippetCheck("M254-B001-NDOC-02", f"`{SURFACE_PATH}`"),
    SnippetCheck(
        "M254-B001-NDOC-03",
        f"`{DUPLICATE_REGISTRATION_POLICY}`",
    ),
    SnippetCheck("M254-B001-NDOC-04", f"`{STARTUP_EXECUTION_MODE}`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck(
        "M254-B001-SPC-01",
        "## M254 startup bootstrap invariants (B001)",
    ),
    SnippetCheck("M254-B001-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck(
        "M254-B001-SPC-03",
        f"`{FAILURE_MODE}`",
    ),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck(
        "M254-B001-META-01",
        "## M254 startup bootstrap metadata anchors (B001)",
    ),
    SnippetCheck("M254-B001-META-02", f"`{SURFACE_PATH}`"),
    SnippetCheck(
        "M254-B001-META-03",
        f"`{CONSTRUCTOR_ROOT_SYMBOL}`",
    ),
)
AST_SNIPPETS = (
    SnippetCheck(
        "M254-B001-AST-01",
        "kObjc3RuntimeStartupBootstrapInvariantContractId",
    ),
    SnippetCheck(
        "M254-B001-AST-02",
        "kObjc3RuntimeStartupBootstrapDuplicateRegistrationPolicy",
    ),
    SnippetCheck(
        "M254-B001-AST-03",
        "kObjc3RuntimeStartupBootstrapRealizationOrderPolicy",
    ),
    SnippetCheck(
        "M254-B001-AST-04",
        "kObjc3RuntimeStartupBootstrapExecutionMode",
    ),
)
FRONTEND_TYPES_SNIPPETS = (
    SnippetCheck(
        "M254-B001-TYPES-01",
        "struct Objc3RuntimeStartupBootstrapInvariantSummary {",
    ),
    SnippetCheck(
        "M254-B001-TYPES-02",
        "std::string duplicate_registration_policy =",
    ),
    SnippetCheck(
        "M254-B001-TYPES-03",
        "bool duplicate_registration_semantics_frozen = false;",
    ),
    SnippetCheck(
        "M254-B001-TYPES-04",
        "inline bool IsReadyObjc3RuntimeStartupBootstrapInvariantSummary(",
    ),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck(
        "M254-B001-ART-01",
        "BuildRuntimeStartupBootstrapInvariantSummary(",
    ),
    SnippetCheck(
        "M254-B001-ART-02",
        "BuildRuntimeStartupBootstrapInvariantSummaryJson(",
    ),
    SnippetCheck(
        "M254-B001-ART-03",
        "objc_runtime_startup_bootstrap_invariants",
    ),
    SnippetCheck(
        "M254-B001-ART-04",
        "runtime_startup_bootstrap_invariant_contract_id",
    ),
)
DRIVER_SNIPPETS = (
    SnippetCheck(
        "M254-B001-DRV-01",
        "M254-B001 bootstrap-invariant anchor",
    ),
    SnippetCheck(
        "M254-B001-DRV-02",
        "one constructor root per translation-unit identity",
    ),
)
PROCESS_SNIPPETS = (
    SnippetCheck(
        "M254-B001-PROC-01",
        "M254-B001 bootstrap-invariant anchor",
    ),
    SnippetCheck(
        "M254-B001-PROC-02",
        "duplicate registration on the same identity key",
    ),
)
RUNTIME_README_SNIPPETS = (
    SnippetCheck(
        "M254-B001-RUN-01",
        "`M254-B001` freezes the startup/bootstrap semantic contract",
    ),
    SnippetCheck(
        "M254-B001-RUN-02",
        f"`{FAILURE_MODE}`",
    ),
)
FIXTURE_SNIPPETS = (
    SnippetCheck("M254-B001-FIX-01", "module Demo;"),
    SnippetCheck("M254-B001-FIX-02", "fn main() {"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck(
        "M254-B001-PKG-01",
        '"check:objc3c:m254-b001-bootstrap-invariants-contract": "python scripts/check_m254_b001_bootstrap_invariants_contract.py"',
    ),
    SnippetCheck(
        "M254-B001-PKG-02",
        '"test:tooling:m254-b001-bootstrap-invariants-contract": "python -m pytest tests/tooling/test_check_m254_b001_bootstrap_invariants_contract.py -q"',
    ),
    SnippetCheck(
        "M254-B001-PKG-03",
        '"check:objc3c:m254-b001-lane-b-readiness": "npm run build:objc3c-native && npm run check:objc3c:m254-b001-bootstrap-invariants-contract && npm run test:tooling:m254-b001-bootstrap-invariants-contract"',
    ),
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
    checks_total += require(fixture_path.exists(), display_path(fixture_path), "M254-B001-FIXTURE-EXISTS", "fixture is missing", findings)
    checks_total += require(native_exe.exists(), display_path(native_exe), "M254-B001-NATIVE-EXISTS", "native executable is missing", findings)
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
    registration_manifest_path = out_dir / REGISTRATION_MANIFEST_ARTIFACT
    checks_total += require(completed.returncode == 0, display_path(out_dir), "M254-B001-NATIVE-EXIT", "native compile must succeed", findings)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "M254-B001-MANIFEST", "manifest is missing", findings)
    checks_total += require(registration_manifest_path.exists(), display_path(registration_manifest_path), "M254-B001-REG-MANIFEST", "registration manifest is missing", findings)
    if findings:
        return checks_total, findings, None

    manifest_payload = load_json(manifest_path)
    registration_manifest_payload = load_json(registration_manifest_path)
    frontend = manifest_payload.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    semantic_surface = pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    sema_pass_manager = pipeline.get("sema_pass_manager") if isinstance(pipeline, dict) else None
    bootstrap = semantic_surface.get("objc_runtime_startup_bootstrap_invariants") if isinstance(semantic_surface, dict) else None

    checks_total += require(isinstance(bootstrap, dict), display_path(manifest_path), "M254-B001-SURFACE-EXISTS", "bootstrap invariant packet missing from semantic surface", findings)
    checks_total += require(isinstance(sema_pass_manager, dict), display_path(manifest_path), "M254-B001-FLAT-EXISTS", "flattened sema pass manager summary missing", findings)
    if findings:
        return checks_total, findings, None

    checks_total += require(bootstrap.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M254-B001-CONTRACT", "contract id mismatch", findings)
    checks_total += require(bootstrap.get("registration_manifest_contract_id") == REGISTRATION_MANIFEST_CONTRACT_ID, display_path(manifest_path), "M254-B001-UPSTREAM", "upstream registration-manifest contract mismatch", findings)
    checks_total += require(bootstrap.get("bootstrap_surface_path") == SURFACE_PATH, display_path(manifest_path), "M254-B001-SURFACE-PATH", "surface path mismatch", findings)
    checks_total += require(bootstrap.get("duplicate_registration_policy") == DUPLICATE_REGISTRATION_POLICY, display_path(manifest_path), "M254-B001-DUPLICATE", "duplicate-registration policy mismatch", findings)
    checks_total += require(bootstrap.get("realization_order_policy") == REALIZATION_ORDER_POLICY, display_path(manifest_path), "M254-B001-ORDER", "realization-order policy mismatch", findings)
    checks_total += require(bootstrap.get("failure_mode") == FAILURE_MODE, display_path(manifest_path), "M254-B001-FAILURE", "failure-mode mismatch", findings)
    checks_total += require(bootstrap.get("image_local_initialization_scope") == IMAGE_LOCAL_INITIALIZATION_SCOPE, display_path(manifest_path), "M254-B001-IMAGE-LOCAL", "image-local initialization scope mismatch", findings)
    checks_total += require(bootstrap.get("constructor_root_uniqueness_policy") == CONSTRUCTOR_ROOT_UNIQUENESS_POLICY, display_path(manifest_path), "M254-B001-UNIQUENESS", "constructor-root uniqueness mismatch", findings)
    checks_total += require(bootstrap.get("constructor_root_consumption_model") == CONSTRUCTOR_ROOT_CONSUMPTION_MODEL, display_path(manifest_path), "M254-B001-CONSUMPTION", "constructor-root consumption model mismatch", findings)
    checks_total += require(bootstrap.get("startup_execution_mode") == STARTUP_EXECUTION_MODE, display_path(manifest_path), "M254-B001-EXECUTION", "startup execution mode mismatch", findings)
    checks_total += require(bootstrap.get("constructor_root_symbol") == CONSTRUCTOR_ROOT_SYMBOL, display_path(manifest_path), "M254-B001-CONSTRUCTOR", "constructor-root symbol mismatch", findings)
    checks_total += require(bootstrap.get("registration_entrypoint_symbol") == REGISTRATION_ENTRYPOINT_SYMBOL, display_path(manifest_path), "M254-B001-ENTRYPOINT", "registration entrypoint mismatch", findings)
    checks_total += require(bootstrap.get("manifest_authority_model") == MANIFEST_AUTHORITY_MODEL, display_path(manifest_path), "M254-B001-AUTHORITY", "manifest authority model mismatch", findings)
    checks_total += require(bootstrap.get("translation_unit_identity_model") == TRANSLATION_UNIT_IDENTITY_MODEL, display_path(manifest_path), "M254-B001-TU-MODEL", "translation-unit identity model mismatch", findings)
    checks_total += require(bootstrap.get("ready") is True, display_path(manifest_path), "M254-B001-READY", "bootstrap invariant surface must be ready", findings)
    checks_total += require(bootstrap.get("fail_closed") is True, display_path(manifest_path), "M254-B001-FAIL-CLOSED", "bootstrap invariant surface must be fail-closed", findings)
    checks_total += require(bootstrap.get("registration_manifest_contract_ready") is True, display_path(manifest_path), "M254-B001-MANIFEST-READY", "registration-manifest contract must be ready", findings)
    checks_total += require(bootstrap.get("duplicate_registration_semantics_frozen") is True, display_path(manifest_path), "M254-B001-DUPLICATE-FROZEN", "duplicate-registration semantics must be frozen", findings)
    checks_total += require(bootstrap.get("realization_order_semantics_frozen") is True, display_path(manifest_path), "M254-B001-ORDER-FROZEN", "realization-order semantics must be frozen", findings)
    checks_total += require(bootstrap.get("failure_mode_semantics_frozen") is True, display_path(manifest_path), "M254-B001-FAILURE-FROZEN", "failure-mode semantics must be frozen", findings)
    checks_total += require(bootstrap.get("image_local_initialization_scope_frozen") is True, display_path(manifest_path), "M254-B001-IMAGE-LOCAL-FROZEN", "image-local scope must be frozen", findings)
    checks_total += require(bootstrap.get("constructor_root_uniqueness_frozen") is True, display_path(manifest_path), "M254-B001-UNIQUENESS-FROZEN", "constructor-root uniqueness must be frozen", findings)
    checks_total += require(bootstrap.get("startup_execution_not_yet_landed") is True, display_path(manifest_path), "M254-B001-NONGOAL-EXECUTION", "startup execution non-goal must remain explicit", findings)
    checks_total += require(bootstrap.get("live_duplicate_registration_enforcement_not_yet_landed") is True, display_path(manifest_path), "M254-B001-NONGOAL-DUPLICATE", "duplicate-registration enforcement non-goal must remain explicit", findings)
    checks_total += require(bootstrap.get("image_local_realization_not_yet_landed") is True, display_path(manifest_path), "M254-B001-NONGOAL-REALIZATION", "image-local realization non-goal must remain explicit", findings)
    checks_total += require(bootstrap.get("ready_for_bootstrap_implementation") is True, display_path(manifest_path), "M254-B001-READY-FOR-B002", "ready-for-bootstrap-implementation must be true", findings)
    checks_total += require(bool(bootstrap.get("registration_manifest_replay_key")), display_path(manifest_path), "M254-B001-UPSTREAM-REPLAY", "registration-manifest replay key must be non-empty", findings)
    checks_total += require(bool(bootstrap.get("replay_key")), display_path(manifest_path), "M254-B001-REPLAY", "bootstrap invariant replay key must be non-empty", findings)
    checks_total += require(bootstrap.get("failure_reason") == "", display_path(manifest_path), "M254-B001-FAILURE-REASON", "failure reason must be empty", findings)

    checks_total += require(sema_pass_manager.get("runtime_startup_bootstrap_invariant_contract_id") == CONTRACT_ID, display_path(manifest_path), "M254-B001-FLAT-CONTRACT", "flattened contract id mismatch", findings)
    checks_total += require(sema_pass_manager.get("runtime_startup_bootstrap_invariant_duplicate_registration_policy") == DUPLICATE_REGISTRATION_POLICY, display_path(manifest_path), "M254-B001-FLAT-DUPLICATE", "flattened duplicate-registration policy mismatch", findings)
    checks_total += require(sema_pass_manager.get("runtime_startup_bootstrap_invariant_realization_order_policy") == REALIZATION_ORDER_POLICY, display_path(manifest_path), "M254-B001-FLAT-ORDER", "flattened realization-order policy mismatch", findings)
    checks_total += require(sema_pass_manager.get("runtime_startup_bootstrap_invariant_failure_mode") == FAILURE_MODE, display_path(manifest_path), "M254-B001-FLAT-FAILURE", "flattened failure mode mismatch", findings)
    checks_total += require(sema_pass_manager.get("runtime_startup_bootstrap_invariant_ready_for_bootstrap_implementation") is True, display_path(manifest_path), "M254-B001-FLAT-READY", "flattened ready flag mismatch", findings)

    checks_total += require(registration_manifest_payload.get("contract_id") == REGISTRATION_MANIFEST_CONTRACT_ID, display_path(registration_manifest_path), "M254-B001-REGFILE-CONTRACT", "registration-manifest contract drifted from A002", findings)
    checks_total += require(registration_manifest_payload.get("constructor_root_symbol") == CONSTRUCTOR_ROOT_SYMBOL, display_path(registration_manifest_path), "M254-B001-REGFILE-CONSTRUCTOR", "registration-manifest constructor-root symbol mismatch", findings)
    checks_total += require(registration_manifest_payload.get("registration_entrypoint_symbol") == REGISTRATION_ENTRYPOINT_SYMBOL, display_path(registration_manifest_path), "M254-B001-REGFILE-ENTRYPOINT", "registration-manifest entrypoint mismatch", findings)

    case_payload: dict[str, object] = {
        "fixture": display_path(fixture_path),
        "manifest_path": display_path(manifest_path),
        "registration_manifest_path": display_path(registration_manifest_path),
        "duplicate_registration_policy": bootstrap.get("duplicate_registration_policy"),
        "realization_order_policy": bootstrap.get("realization_order_policy"),
        "bootstrap_replay_key": bootstrap.get("replay_key"),
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
        (args.expectations_doc, "M254-B001-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M254-B001-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M254-B001-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.native_doc, "M254-B001-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M254-B001-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M254-B001-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.ast_header, "M254-B001-AST-EXISTS", AST_SNIPPETS),
        (args.frontend_types, "M254-B001-TYPES-EXISTS", FRONTEND_TYPES_SNIPPETS),
        (args.frontend_artifacts, "M254-B001-ART-EXISTS", FRONTEND_ARTIFACTS_SNIPPETS),
        (args.driver_cpp, "M254-B001-DRV-EXISTS", DRIVER_SNIPPETS),
        (args.process_cpp, "M254-B001-PROC-EXISTS", PROCESS_SNIPPETS),
        (args.runtime_readme, "M254-B001-RUN-EXISTS", RUNTIME_README_SNIPPETS),
        (args.fixture, "M254-B001-FIX-EXISTS", FIXTURE_SNIPPETS),
        (args.package_json, "M254-B001-PKG-EXISTS", PACKAGE_SNIPPETS),
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
            out_dir=args.probe_root.resolve() / "hello-bootstrap-invariants",
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
        "next_implementation_issue": "M254-B002",
        "failures": [failure.__dict__ for failure in failures],
    }

    summary_out = args.summary_out
    if not summary_out.is_absolute():
        summary_out = ROOT / summary_out
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(canonical_json(payload), encoding="utf-8")

    if failures:
        print(f"[FAIL] M254-B001 bootstrap invariant drift; summary: {display_path(summary_out)}", file=sys.stderr)
        return 1
    print(f"[PASS] M254-B001 bootstrap invariants preserved; summary: {display_path(summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run())
