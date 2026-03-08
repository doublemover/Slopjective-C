#!/usr/bin/env python3
"""Fail-closed contract checker for M254-B002 live bootstrap semantics."""

from __future__ import annotations

import argparse
import json
import os
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m254-b002-live-bootstrap-semantics-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-runtime-startup-bootstrap-semantics/m254-b002-v1"
BOOTSTRAP_INVARIANT_CONTRACT_ID = (
    "objc3c-runtime-startup-bootstrap-invariants/m254-b001-v1"
)
REGISTRATION_MANIFEST_CONTRACT_ID = (
    "objc3c-translation-unit-registration-manifest/m254-a002-v1"
)
SURFACE_PATH = (
    "frontend.pipeline.semantic_surface.objc_runtime_startup_bootstrap_semantics"
)
DUPLICATE_REGISTRATION_POLICY = "fail-closed-by-translation-unit-identity-key"
REALIZATION_ORDER_POLICY = "constructor-root-then-registration-manifest-order"
FAILURE_MODE = "abort-before-user-main-no-partial-registration-commit"
IMAGE_LOCAL_INITIALIZATION_SCOPE = "runtime-owned-image-local-registration-state"
REGISTRATION_RESULT_MODEL = "zero-success-negative-fail-closed"
REGISTRATION_ORDER_ORDINAL_MODEL = (
    "strictly-monotonic-positive-registration-order-ordinal"
)
RUNTIME_LIBRARY_ARCHIVE = "artifacts/lib/objc3_runtime.lib"
RUNTIME_STATE_SNAPSHOT_SYMBOL = "objc3_runtime_copy_registration_state_for_testing"
SUCCESS_STATUS_CODE = 0
INVALID_DESCRIPTOR_STATUS_CODE = -1
DUPLICATE_REGISTRATION_STATUS_CODE = -2
OUT_OF_ORDER_STATUS_CODE = -3
TRANSLATION_UNIT_REGISTRATION_ORDER_ORDINAL = 1
REGISTRATION_MANIFEST_ARTIFACT = "module.runtime-registration-manifest.json"
PROBE_SOURCE_PATH = "tests/tooling/runtime/m254_b002_bootstrap_semantics_probe.cpp"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m254_duplicate_registration_order_and_failure_mode_semantics_core_feature_implementation_b002_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m254" / "m254_b002_duplicate_registration_order_and_failure_mode_semantics_core_feature_implementation_packet.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_AST_HEADER = ROOT / "native" / "objc3c" / "src" / "ast" / "objc3_ast.h"
DEFAULT_FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
DEFAULT_FRONTEND_ARTIFACTS_HEADER = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.h"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_PROCESS_HEADER = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.h"
DEFAULT_PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DEFAULT_FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
DEFAULT_RUNTIME_HEADER = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.h"
DEFAULT_RUNTIME_SOURCE = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
DEFAULT_NATIVE_RUNTIME_README = ROOT / "native" / "objc3c" / "src" / "runtime" / "README.md"
DEFAULT_TEST_RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
DEFAULT_RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m254_b002_bootstrap_semantics_probe.cpp"
DEFAULT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
DEFAULT_RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m254" / "b002-live-bootstrap-semantics"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m254/M254-B002/bootstrap_semantics_summary.json")


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
        "M254-B002-DOC-EXP-01",
        "# M254 Duplicate-Registration, Order, and Failure-Mode Semantics Core Feature Implementation Expectations (B002)",
    ),
    SnippetCheck("M254-B002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M254-B002-DOC-EXP-03", f"`{SURFACE_PATH}`"),
    SnippetCheck("M254-B002-DOC-EXP-04", f"`{RUNTIME_STATE_SNAPSHOT_SYMBOL}`"),
    SnippetCheck("M254-B002-DOC-EXP-05", f"`{PROBE_SOURCE_PATH}`"),
)
PACKET_SNIPPETS = (
    SnippetCheck(
        "M254-B002-DOC-PKT-01",
        "# M254-B002 Duplicate-Registration, Order, and Failure-Mode Semantics Core Feature Implementation Packet",
    ),
    SnippetCheck("M254-B002-DOC-PKT-02", "Packet: `M254-B002`"),
    SnippetCheck("M254-B002-DOC-PKT-03", "Dependencies: `M254-B001`, `M254-A002`"),
    SnippetCheck("M254-B002-DOC-PKT-04", f"- Contract id `{CONTRACT_ID}`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck(
        "M254-B002-ARCH-01",
        "M254 lane-B B002 startup-bootstrap semantics implementation extends that",
    ),
    SnippetCheck("M254-B002-ARCH-02", "runtime/objc3_runtime.cpp"),
    SnippetCheck("M254-B002-ARCH-03", "module.runtime-registration-manifest.json"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck(
        "M254-B002-NDOC-01",
        "## Live bootstrap semantics (M254-B002)",
    ),
    SnippetCheck("M254-B002-NDOC-02", f"`{SURFACE_PATH}`"),
    SnippetCheck("M254-B002-NDOC-03", f"`{RUNTIME_STATE_SNAPSHOT_SYMBOL}`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck(
        "M254-B002-SPC-01",
        "## M254 live startup bootstrap semantics (B002)",
    ),
    SnippetCheck("M254-B002-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M254-B002-SPC-03", f"`{REGISTRATION_RESULT_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck(
        "M254-B002-META-01",
        "## M254 live startup bootstrap metadata anchors (B002)",
    ),
    SnippetCheck("M254-B002-META-02", f"`{RUNTIME_STATE_SNAPSHOT_SYMBOL}`"),
    SnippetCheck(
        "M254-B002-META-03",
        "translation_unit_registration_order_ordinal",
    ),
)
AST_SNIPPETS = (
    SnippetCheck("M254-B002-AST-01", "kObjc3RuntimeBootstrapSemanticsContractId"),
    SnippetCheck("M254-B002-AST-02", "kObjc3RuntimeBootstrapResultModel"),
    SnippetCheck("M254-B002-AST-03", "kObjc3RuntimeBootstrapStateSnapshotSymbol"),
)
FRONTEND_TYPES_SNIPPETS = (
    SnippetCheck(
        "M254-B002-TYPES-01",
        "struct Objc3RuntimeBootstrapSemanticsSummary {",
    ),
    SnippetCheck(
        "M254-B002-TYPES-02",
        "std::string registration_result_model =",
    ),
    SnippetCheck(
        "M254-B002-TYPES-03",
        "int duplicate_registration_status_code =",
    ),
    SnippetCheck(
        "M254-B002-TYPES-04",
        "inline bool IsReadyObjc3RuntimeBootstrapSemanticsSummary(",
    ),
)
FRONTEND_ARTIFACTS_HEADER_SNIPPETS = (
    SnippetCheck(
        "M254-B002-ARTH-01",
        "Objc3RuntimeBootstrapSemanticsSummary runtime_bootstrap_semantics_summary;",
    ),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck(
        "M254-B002-ART-01",
        "BuildRuntimeBootstrapSemanticsSummary(",
    ),
    SnippetCheck(
        "M254-B002-ART-02",
        "BuildRuntimeBootstrapSemanticsSummaryJson(",
    ),
    SnippetCheck(
        "M254-B002-ART-03",
        "objc_runtime_startup_bootstrap_semantics",
    ),
    SnippetCheck(
        "M254-B002-ART-04",
        "runtime_bootstrap_semantics_contract_id",
    ),
)
DRIVER_SNIPPETS = (
    SnippetCheck("M254-B002-DRV-01", "M254-B002 bootstrap-semantics anchor"),
    SnippetCheck("M254-B002-DRV-02", "bootstrap_semantics_contract_id"),
)
PROCESS_HEADER_SNIPPETS = (
    SnippetCheck("M254-B002-PROCH-01", "std::string bootstrap_semantics_contract_id;"),
    SnippetCheck("M254-B002-PROCH-02", "std::string runtime_state_snapshot_symbol;"),
    SnippetCheck("M254-B002-PROCH-03", "std::uint64_t translation_unit_registration_order_ordinal = 0;"),
)
PROCESS_CPP_SNIPPETS = (
    SnippetCheck("M254-B002-PROC-01", "M254-B002 live bootstrap semantics anchor"),
    SnippetCheck("M254-B002-PROC-02", "bootstrap_semantics_contract_id"),
    SnippetCheck("M254-B002-PROC-03", "ready_for_runtime_bootstrap_enforcement"),
)
FRONTEND_ANCHOR_SNIPPETS = (
    SnippetCheck("M254-B002-FRONT-01", "runtime_bootstrap_semantics_summary"),
    SnippetCheck("M254-B002-FRONT-02", "manifest_inputs.bootstrap_semantics_contract_id ="),
)
RUNTIME_HEADER_SNIPPETS = (
    SnippetCheck("M254-B002-RTH-01", "OBJC3_RUNTIME_REGISTRATION_STATUS_DUPLICATE_TRANSLATION_UNIT_IDENTITY_KEY = -2"),
    SnippetCheck("M254-B002-RTH-02", "const char *translation_unit_identity_key;"),
    SnippetCheck("M254-B002-RTH-03", "typedef struct objc3_runtime_registration_state_snapshot {"),
    SnippetCheck("M254-B002-RTH-04", "int objc3_runtime_copy_registration_state_for_testing("),
)
RUNTIME_SOURCE_SNIPPETS = (
    SnippetCheck("M254-B002-RTS-01", "next_expected_registration_order_ordinal = 1;"),
    SnippetCheck(
        "M254-B002-RTS-02",
        "registration_order_by_identity_key;",
    ),
    SnippetCheck("M254-B002-RTS-03", "OBJC3_RUNTIME_REGISTRATION_STATUS_OUT_OF_ORDER_REGISTRATION"),
    SnippetCheck("M254-B002-RTS-04", "objc3_runtime_copy_registration_state_for_testing("),
    SnippetCheck(
        "M254-B002-RTS-05",
        "state.registration_order_by_identity_key.emplace(",
    ),
    SnippetCheck(
        "M254-B002-RTS-06",
        "state.registration_order_by_identity_key.clear();",
    ),
)
NATIVE_RUNTIME_README_SNIPPETS = (
    SnippetCheck("M254-B002-NRTR-01", "`M254-B002` extends the same runtime surface"),
    SnippetCheck("M254-B002-NRTR-02", "duplicate translation-unit identity keys fail closed with status `-2`"),
)
TEST_RUNTIME_README_SNIPPETS = (
    SnippetCheck("M254-B002-TRTR-01", "`M254-B002` turns that freeze into a live runtime contract:"),
    SnippetCheck("M254-B002-TRTR-02", f"`{RUNTIME_STATE_SNAPSHOT_SYMBOL}`"),
)
PROBE_SNIPPETS = (
    SnippetCheck("M254-B002-PRB-01", "objc3_runtime_register_image(&success_image)"),
    SnippetCheck("M254-B002-PRB-02", "objc3_runtime_copy_registration_state_for_testing("),
    SnippetCheck("M254-B002-PRB-03", "out_of_order_status"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck(
        "M254-B002-PKG-01",
        '"check:objc3c:m254-b002-duplicate-registration-order-and-failure-mode-semantics-core-feature-implementation": "python scripts/check_m254_b002_duplicate_registration_order_and_failure_mode_semantics_core_feature_implementation.py"',
    ),
    SnippetCheck(
        "M254-B002-PKG-02",
        '"test:tooling:m254-b002-duplicate-registration-order-and-failure-mode-semantics-core-feature-implementation": "python -m pytest tests/tooling/test_check_m254_b002_duplicate_registration_order_and_failure_mode_semantics_core_feature_implementation.py -q"',
    ),
    SnippetCheck(
        "M254-B002-PKG-03",
        '"check:objc3c:m254-b002-lane-b-readiness": "python scripts/run_m254_b002_lane_b_readiness.py"',
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
    parser.add_argument("--frontend-artifacts-header", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS_HEADER)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--process-header", type=Path, default=DEFAULT_PROCESS_HEADER)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--frontend-anchor-cpp", type=Path, default=DEFAULT_FRONTEND_ANCHOR_CPP)
    parser.add_argument("--runtime-header", type=Path, default=DEFAULT_RUNTIME_HEADER)
    parser.add_argument("--runtime-source", type=Path, default=DEFAULT_RUNTIME_SOURCE)
    parser.add_argument("--native-runtime-readme", type=Path, default=DEFAULT_NATIVE_RUNTIME_README)
    parser.add_argument("--test-runtime-readme", type=Path, default=DEFAULT_TEST_RUNTIME_README)
    parser.add_argument("--runtime-probe", type=Path, default=DEFAULT_RUNTIME_PROBE)
    parser.add_argument("--fixture", type=Path, default=DEFAULT_FIXTURE)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--runtime-library", type=Path, default=DEFAULT_RUNTIME_LIBRARY)
    parser.add_argument("--runtime-include-root", type=Path, default=DEFAULT_RUNTIME_INCLUDE_ROOT)
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


def run_command(command: Sequence[str], cwd: Path = ROOT) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def resolve_tool(executable: str) -> Path | None:
    env_bin = os.environ.get("LLVM_BIN_DIR")
    if env_bin:
        candidate = Path(env_bin) / executable
        if candidate.exists():
            return candidate
    default_candidate = Path("C:/Program Files/LLVM/bin") / executable
    if default_candidate.exists():
        return default_candidate
    which = shutil.which(executable)
    if which:
        return Path(which)
    return None


def run_dynamic_case(args: argparse.Namespace) -> tuple[int, list[Finding], dict[str, object] | None]:
    findings: list[Finding] = []
    checks_total = 0

    checks_total += require(args.fixture.exists(), display_path(args.fixture), "M254-B002-FIXTURE-EXISTS", "fixture is missing", findings)
    checks_total += require(args.native_exe.exists(), display_path(args.native_exe), "M254-B002-NATIVE-EXISTS", "native executable is missing", findings)
    checks_total += require(args.runtime_library.exists(), display_path(args.runtime_library), "M254-B002-RTLIB-EXISTS", "runtime library is missing", findings)
    if findings:
        return checks_total, findings, None

    out_dir = args.probe_root.resolve() / "hello-bootstrap-semantics"
    out_dir.mkdir(parents=True, exist_ok=True)
    completed = run_command([
        str(args.native_exe.resolve()),
        str(args.fixture.resolve()),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ])

    manifest_path = out_dir / "module.manifest.json"
    registration_manifest_path = out_dir / REGISTRATION_MANIFEST_ARTIFACT
    checks_total += require(completed.returncode == 0, display_path(out_dir), "M254-B002-NATIVE-EXIT", "native compile must succeed", findings)
    checks_total += require(manifest_path.exists(), display_path(manifest_path), "M254-B002-MANIFEST", "manifest is missing", findings)
    checks_total += require(registration_manifest_path.exists(), display_path(registration_manifest_path), "M254-B002-REG-MANIFEST", "registration manifest is missing", findings)
    if findings:
        return checks_total, findings, None

    manifest_payload = load_json(manifest_path)
    registration_manifest_payload = load_json(registration_manifest_path)
    frontend = manifest_payload.get("frontend")
    pipeline = frontend.get("pipeline") if isinstance(frontend, dict) else None
    semantic_surface = pipeline.get("semantic_surface") if isinstance(pipeline, dict) else None
    sema_pass_manager = pipeline.get("sema_pass_manager") if isinstance(pipeline, dict) else None
    bootstrap = semantic_surface.get("objc_runtime_startup_bootstrap_semantics") if isinstance(semantic_surface, dict) else None

    checks_total += require(isinstance(bootstrap, dict), display_path(manifest_path), "M254-B002-SURFACE-EXISTS", "bootstrap semantics packet missing from semantic surface", findings)
    checks_total += require(isinstance(sema_pass_manager, dict), display_path(manifest_path), "M254-B002-FLAT-EXISTS", "flattened sema summary missing", findings)
    if findings:
        return checks_total, findings, None

    checks_total += require(bootstrap.get("contract_id") == CONTRACT_ID, display_path(manifest_path), "M254-B002-CONTRACT", "contract id mismatch", findings)
    checks_total += require(bootstrap.get("bootstrap_invariant_contract_id") == BOOTSTRAP_INVARIANT_CONTRACT_ID, display_path(manifest_path), "M254-B002-UPSTREAM-B001", "bootstrap invariant contract mismatch", findings)
    checks_total += require(bootstrap.get("registration_manifest_contract_id") == REGISTRATION_MANIFEST_CONTRACT_ID, display_path(manifest_path), "M254-B002-UPSTREAM-A002", "registration manifest contract mismatch", findings)
    checks_total += require(bootstrap.get("bootstrap_surface_path") == SURFACE_PATH, display_path(manifest_path), "M254-B002-SURFACE-PATH", "surface path mismatch", findings)
    checks_total += require(bootstrap.get("duplicate_registration_policy") == DUPLICATE_REGISTRATION_POLICY, display_path(manifest_path), "M254-B002-DUPLICATE", "duplicate-registration policy mismatch", findings)
    checks_total += require(bootstrap.get("realization_order_policy") == REALIZATION_ORDER_POLICY, display_path(manifest_path), "M254-B002-ORDER", "realization-order policy mismatch", findings)
    checks_total += require(bootstrap.get("failure_mode") == FAILURE_MODE, display_path(manifest_path), "M254-B002-FAILURE", "failure-mode mismatch", findings)
    checks_total += require(bootstrap.get("image_local_initialization_scope") == IMAGE_LOCAL_INITIALIZATION_SCOPE, display_path(manifest_path), "M254-B002-IMAGE-LOCAL", "image-local scope mismatch", findings)
    checks_total += require(bootstrap.get("registration_result_model") == REGISTRATION_RESULT_MODEL, display_path(manifest_path), "M254-B002-RESULT-MODEL", "result model mismatch", findings)
    checks_total += require(bootstrap.get("registration_order_ordinal_model") == REGISTRATION_ORDER_ORDINAL_MODEL, display_path(manifest_path), "M254-B002-ORDINAL-MODEL", "registration order model mismatch", findings)
    checks_total += require(bootstrap.get("runtime_state_snapshot_symbol") == RUNTIME_STATE_SNAPSHOT_SYMBOL, display_path(manifest_path), "M254-B002-SNAPSHOT-SYMBOL", "snapshot symbol mismatch", findings)
    checks_total += require(bootstrap.get("runtime_library_archive_relative_path") == RUNTIME_LIBRARY_ARCHIVE, display_path(manifest_path), "M254-B002-ARCHIVE", "runtime library path mismatch", findings)
    checks_total += require(bootstrap.get("success_status_code") == SUCCESS_STATUS_CODE, display_path(manifest_path), "M254-B002-SUCCESS-CODE", "success status mismatch", findings)
    checks_total += require(bootstrap.get("invalid_descriptor_status_code") == INVALID_DESCRIPTOR_STATUS_CODE, display_path(manifest_path), "M254-B002-INVALID-CODE", "invalid status mismatch", findings)
    checks_total += require(bootstrap.get("duplicate_registration_status_code") == DUPLICATE_REGISTRATION_STATUS_CODE, display_path(manifest_path), "M254-B002-DUPLICATE-CODE", "duplicate status mismatch", findings)
    checks_total += require(bootstrap.get("out_of_order_status_code") == OUT_OF_ORDER_STATUS_CODE, display_path(manifest_path), "M254-B002-ORDER-CODE", "out-of-order status mismatch", findings)
    checks_total += require(bootstrap.get("translation_unit_registration_order_ordinal") == TRANSLATION_UNIT_REGISTRATION_ORDER_ORDINAL, display_path(manifest_path), "M254-B002-ORDINAL", "registration order ordinal mismatch", findings)
    checks_total += require(bootstrap.get("ready") is True, display_path(manifest_path), "M254-B002-READY", "bootstrap semantics surface must be ready", findings)
    checks_total += require(bootstrap.get("fail_closed") is True, display_path(manifest_path), "M254-B002-FAIL-CLOSED", "bootstrap semantics surface must be fail-closed", findings)
    checks_total += require(bootstrap.get("live_runtime_enforcement_landed") is True, display_path(manifest_path), "M254-B002-LIVE", "live runtime enforcement flag must be true", findings)
    checks_total += require(bootstrap.get("registration_manifest_bootstrap_semantics_published") is True, display_path(manifest_path), "M254-B002-MANIFEST-PUBLISHED", "registration-manifest bootstrap publication flag must be true", findings)
    checks_total += require(bootstrap.get("runtime_probe_required") is True, display_path(manifest_path), "M254-B002-PROBE-REQUIRED", "runtime probe flag must be true", findings)
    checks_total += require(bootstrap.get("no_partial_commit_on_failure") is True, display_path(manifest_path), "M254-B002-NO-PARTIAL", "no-partial-commit flag must be true", findings)
    checks_total += require(bootstrap.get("ready_for_constructor_root_implementation") is True, display_path(manifest_path), "M254-B002-READY-FOR-C001", "ready-for-constructor-root flag must be true", findings)

    checks_total += require(sema_pass_manager.get("runtime_bootstrap_semantics_contract_id") == CONTRACT_ID, display_path(manifest_path), "M254-B002-FLAT-CONTRACT", "flattened contract id mismatch", findings)
    checks_total += require(sema_pass_manager.get("runtime_bootstrap_semantics_duplicate_registration_policy") == DUPLICATE_REGISTRATION_POLICY, display_path(manifest_path), "M254-B002-FLAT-DUPLICATE", "flattened duplicate policy mismatch", findings)
    checks_total += require(sema_pass_manager.get("runtime_bootstrap_semantics_runtime_state_snapshot_symbol") == RUNTIME_STATE_SNAPSHOT_SYMBOL, display_path(manifest_path), "M254-B002-FLAT-SNAPSHOT", "flattened snapshot symbol mismatch", findings)

    checks_total += require(registration_manifest_payload.get("bootstrap_semantics_contract_id") == CONTRACT_ID, display_path(registration_manifest_path), "M254-B002-REGFILE-CONTRACT", "registration manifest bootstrap contract mismatch", findings)
    checks_total += require(registration_manifest_payload.get("duplicate_registration_policy") == DUPLICATE_REGISTRATION_POLICY, display_path(registration_manifest_path), "M254-B002-REGFILE-DUPLICATE", "registration manifest duplicate policy mismatch", findings)
    checks_total += require(registration_manifest_payload.get("realization_order_policy") == REALIZATION_ORDER_POLICY, display_path(registration_manifest_path), "M254-B002-REGFILE-ORDER", "registration manifest order policy mismatch", findings)
    checks_total += require(registration_manifest_payload.get("failure_mode") == FAILURE_MODE, display_path(registration_manifest_path), "M254-B002-REGFILE-FAILURE", "registration manifest failure mode mismatch", findings)
    checks_total += require(registration_manifest_payload.get("registration_result_model") == REGISTRATION_RESULT_MODEL, display_path(registration_manifest_path), "M254-B002-REGFILE-RESULT", "registration manifest result model mismatch", findings)
    checks_total += require(registration_manifest_payload.get("registration_order_ordinal_model") == REGISTRATION_ORDER_ORDINAL_MODEL, display_path(registration_manifest_path), "M254-B002-REGFILE-ORDINAL-MODEL", "registration manifest order model mismatch", findings)
    checks_total += require(registration_manifest_payload.get("runtime_state_snapshot_symbol") == RUNTIME_STATE_SNAPSHOT_SYMBOL, display_path(registration_manifest_path), "M254-B002-REGFILE-SNAPSHOT", "registration manifest snapshot symbol mismatch", findings)
    checks_total += require(registration_manifest_payload.get("success_status_code") == SUCCESS_STATUS_CODE, display_path(registration_manifest_path), "M254-B002-REGFILE-SUCCESS", "registration manifest success status mismatch", findings)
    checks_total += require(registration_manifest_payload.get("invalid_descriptor_status_code") == INVALID_DESCRIPTOR_STATUS_CODE, display_path(registration_manifest_path), "M254-B002-REGFILE-INVALID", "registration manifest invalid status mismatch", findings)
    checks_total += require(registration_manifest_payload.get("duplicate_registration_status_code") == DUPLICATE_REGISTRATION_STATUS_CODE, display_path(registration_manifest_path), "M254-B002-REGFILE-DUPLICATE-CODE", "registration manifest duplicate status mismatch", findings)
    checks_total += require(registration_manifest_payload.get("out_of_order_status_code") == OUT_OF_ORDER_STATUS_CODE, display_path(registration_manifest_path), "M254-B002-REGFILE-ORDER-CODE", "registration manifest out-of-order status mismatch", findings)
    checks_total += require(registration_manifest_payload.get("translation_unit_registration_order_ordinal") == TRANSLATION_UNIT_REGISTRATION_ORDER_ORDINAL, display_path(registration_manifest_path), "M254-B002-REGFILE-ORDINAL", "registration manifest order ordinal mismatch", findings)
    checks_total += require(registration_manifest_payload.get("ready_for_runtime_bootstrap_enforcement") is True, display_path(registration_manifest_path), "M254-B002-REGFILE-READY", "registration manifest runtime-bootstrap readiness mismatch", findings)

    counts = {
        "class_descriptor_count": registration_manifest_payload.get("class_descriptor_count"),
        "protocol_descriptor_count": registration_manifest_payload.get("protocol_descriptor_count"),
        "category_descriptor_count": registration_manifest_payload.get("category_descriptor_count"),
        "property_descriptor_count": registration_manifest_payload.get("property_descriptor_count"),
        "ivar_descriptor_count": registration_manifest_payload.get("ivar_descriptor_count"),
        "total_descriptor_count": registration_manifest_payload.get("total_descriptor_count"),
    }
    checks_total += require(all(isinstance(value, int) for value in counts.values()), display_path(registration_manifest_path), "M254-B002-REGFILE-COUNTS", "registration manifest descriptor counts must be integers", findings)
    checks_total += require(
        counts["total_descriptor_count"] == counts["class_descriptor_count"] + counts["protocol_descriptor_count"] + counts["category_descriptor_count"] + counts["property_descriptor_count"] + counts["ivar_descriptor_count"],
        display_path(registration_manifest_path),
        "M254-B002-REGFILE-COUNT-TOTAL",
        "registration manifest descriptor total mismatch",
        findings,
    )

    clangxx = resolve_tool("clang++.exe") or resolve_tool("clang++")
    checks_total += require(clangxx is not None, display_path(args.runtime_probe), "M254-B002-CLANGXX", "clang++ is required for the runtime probe", findings)
    if findings:
        return checks_total, findings, None

    probe_out_dir = args.probe_root.resolve() / "probe"
    probe_out_dir.mkdir(parents=True, exist_ok=True)
    probe_exe = probe_out_dir / "m254_b002_bootstrap_semantics_probe.exe"
    compile_command = [
        str(clangxx),
        "-std=c++20",
        "-Wall",
        "-Wextra",
        "-pedantic",
        f"-I{args.runtime_include_root.resolve()}",
        str(args.runtime_probe.resolve()),
        str(args.runtime_library.resolve()),
        "-o",
        str(probe_exe),
    ]
    compile_result = run_command(compile_command)
    checks_total += require(compile_result.returncode == 0, display_path(args.runtime_probe), "M254-B002-PROBE-COMPILE", f"runtime probe compile exited with {compile_result.returncode}", findings)
    checks_total += require(probe_exe.exists(), display_path(probe_exe), "M254-B002-PROBE-EXE", "runtime probe executable is missing", findings)
    if findings:
        return checks_total, findings, None

    run_command_args = [
        str(probe_exe),
        "bootstrap-manifest-case",
        str(registration_manifest_payload["translation_unit_identity_key"]),
        str(registration_manifest_payload["translation_unit_registration_order_ordinal"]),
        str(registration_manifest_payload["class_descriptor_count"]),
        str(registration_manifest_payload["protocol_descriptor_count"]),
        str(registration_manifest_payload["category_descriptor_count"]),
        str(registration_manifest_payload["property_descriptor_count"]),
        str(registration_manifest_payload["ivar_descriptor_count"]),
    ]
    probe_result = run_command(run_command_args)
    checks_total += require(probe_result.returncode == 0, display_path(probe_exe), "M254-B002-PROBE-RUN", f"runtime probe exited with {probe_result.returncode}", findings)
    if findings:
        return checks_total, findings, None

    try:
        probe_payload = json.loads(probe_result.stdout)
    except json.JSONDecodeError as exc:
        findings.append(Finding(display_path(probe_exe), "M254-B002-PROBE-JSON", f"invalid runtime probe JSON: {exc}"))
        return checks_total, findings, None

    checks_total += require(probe_payload.get("success_status") == SUCCESS_STATUS_CODE, display_path(probe_exe), "M254-B002-PROBE-SUCCESS", "runtime probe success status mismatch", findings)
    checks_total += require(probe_payload.get("duplicate_status") == DUPLICATE_REGISTRATION_STATUS_CODE, display_path(probe_exe), "M254-B002-PROBE-DUPLICATE", "runtime probe duplicate status mismatch", findings)
    checks_total += require(probe_payload.get("out_of_order_status") == OUT_OF_ORDER_STATUS_CODE, display_path(probe_exe), "M254-B002-PROBE-ORDER", "runtime probe out-of-order status mismatch", findings)
    checks_total += require(probe_payload.get("invalid_status") == INVALID_DESCRIPTOR_STATUS_CODE, display_path(probe_exe), "M254-B002-PROBE-INVALID", "runtime probe invalid status mismatch", findings)
    checks_total += require(probe_payload.get("translation_unit_identity_key") == registration_manifest_payload.get("translation_unit_identity_key"), display_path(probe_exe), "M254-B002-PROBE-TU", "runtime probe identity key drifted from registration manifest", findings)
    checks_total += require(probe_payload.get("translation_unit_registration_order_ordinal") == registration_manifest_payload.get("translation_unit_registration_order_ordinal"), display_path(probe_exe), "M254-B002-PROBE-ORDINAL", "runtime probe registration order drifted from registration manifest", findings)

    snapshots = probe_payload.get("snapshots") if isinstance(probe_payload, dict) else None
    checks_total += require(isinstance(snapshots, dict), display_path(probe_exe), "M254-B002-PROBE-SNAPSHOTS", "runtime probe snapshots missing", findings)
    if findings:
        return checks_total, findings, None

    after_success = snapshots.get("after_success")
    after_duplicate = snapshots.get("after_duplicate")
    after_out_of_order = snapshots.get("after_out_of_order")
    after_invalid = snapshots.get("after_invalid")
    checks_total += require(isinstance(after_success, dict), display_path(probe_exe), "M254-B002-PROBE-AFTER-SUCCESS", "after_success snapshot missing", findings)
    checks_total += require(isinstance(after_duplicate, dict), display_path(probe_exe), "M254-B002-PROBE-AFTER-DUPLICATE", "after_duplicate snapshot missing", findings)
    checks_total += require(isinstance(after_out_of_order, dict), display_path(probe_exe), "M254-B002-PROBE-AFTER-ORDER", "after_out_of_order snapshot missing", findings)
    checks_total += require(isinstance(after_invalid, dict), display_path(probe_exe), "M254-B002-PROBE-AFTER-INVALID", "after_invalid snapshot missing", findings)
    if findings:
        return checks_total, findings, None

    expected_total = counts["total_descriptor_count"]
    expected_next_order = TRANSLATION_UNIT_REGISTRATION_ORDER_ORDINAL + 1
    for snapshot_name, snapshot in (("after_success", after_success), ("after_duplicate", after_duplicate), ("after_out_of_order", after_out_of_order), ("after_invalid", after_invalid)):
        checks_total += require(snapshot.get("registered_image_count") == 1, display_path(probe_exe), f"M254-B002-{snapshot_name.upper()}-IMAGE-COUNT", f"{snapshot_name} registered image count mismatch", findings)
        checks_total += require(snapshot.get("registered_descriptor_total") == expected_total, display_path(probe_exe), f"M254-B002-{snapshot_name.upper()}-DESCRIPTOR-TOTAL", f"{snapshot_name} descriptor total mismatch", findings)
        checks_total += require(snapshot.get("next_expected_registration_order_ordinal") == expected_next_order, display_path(probe_exe), f"M254-B002-{snapshot_name.upper()}-NEXT-ORDER", f"{snapshot_name} next expected order mismatch", findings)
        checks_total += require(snapshot.get("last_successful_registration_order_ordinal") == TRANSLATION_UNIT_REGISTRATION_ORDER_ORDINAL, display_path(probe_exe), f"M254-B002-{snapshot_name.upper()}-LAST-SUCCESS", f"{snapshot_name} last successful order mismatch", findings)
        checks_total += require(snapshot.get("last_registered_translation_unit_identity_key") == registration_manifest_payload.get("translation_unit_identity_key"), display_path(probe_exe), f"M254-B002-{snapshot_name.upper()}-LAST-REGISTERED", f"{snapshot_name} last registered identity mismatch", findings)

    checks_total += require(after_success.get("last_registration_status") == SUCCESS_STATUS_CODE, display_path(probe_exe), "M254-B002-AFTER-SUCCESS-STATUS", "after_success status mismatch", findings)
    checks_total += require(after_duplicate.get("last_registration_status") == DUPLICATE_REGISTRATION_STATUS_CODE, display_path(probe_exe), "M254-B002-AFTER-DUPLICATE-STATUS", "after_duplicate status mismatch", findings)
    checks_total += require(after_duplicate.get("last_rejected_translation_unit_identity_key") == registration_manifest_payload.get("translation_unit_identity_key"), display_path(probe_exe), "M254-B002-AFTER-DUPLICATE-REJECTED", "after_duplicate rejected identity mismatch", findings)
    checks_total += require(after_out_of_order.get("last_registration_status") == OUT_OF_ORDER_STATUS_CODE, display_path(probe_exe), "M254-B002-AFTER-ORDER-STATUS", "after_out_of_order status mismatch", findings)
    checks_total += require(after_out_of_order.get("last_rejected_translation_unit_identity_key") == f"{registration_manifest_payload.get('translation_unit_identity_key')}-out-of-order", display_path(probe_exe), "M254-B002-AFTER-ORDER-REJECTED", "after_out_of_order rejected identity mismatch", findings)
    checks_total += require(after_out_of_order.get("last_rejected_registration_order_ordinal") == TRANSLATION_UNIT_REGISTRATION_ORDER_ORDINAL + 2, display_path(probe_exe), "M254-B002-AFTER-ORDER-ORDINAL", "after_out_of_order rejected ordinal mismatch", findings)
    checks_total += require(after_invalid.get("last_registration_status") == INVALID_DESCRIPTOR_STATUS_CODE, display_path(probe_exe), "M254-B002-AFTER-INVALID-STATUS", "after_invalid status mismatch", findings)
    checks_total += require(after_invalid.get("last_rejected_module_name") == "invalid-module", display_path(probe_exe), "M254-B002-AFTER-INVALID-MODULE", "after_invalid rejected module mismatch", findings)

    case_payload: dict[str, object] = {
        "fixture": display_path(args.fixture),
        "manifest_path": display_path(manifest_path),
        "registration_manifest_path": display_path(registration_manifest_path),
        "compile_command": compile_command,
        "probe_command": run_command_args,
        "translation_unit_identity_key": registration_manifest_payload.get("translation_unit_identity_key"),
        "translation_unit_registration_order_ordinal": registration_manifest_payload.get("translation_unit_registration_order_ordinal"),
        "descriptor_total": expected_total,
        "probe_payload": probe_payload,
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
        (args.expectations_doc, "M254-B002-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M254-B002-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M254-B002-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.native_doc, "M254-B002-NDOC-EXISTS", NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, "M254-B002-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M254-B002-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.ast_header, "M254-B002-AST-EXISTS", AST_SNIPPETS),
        (args.frontend_types, "M254-B002-TYPES-EXISTS", FRONTEND_TYPES_SNIPPETS),
        (args.frontend_artifacts_header, "M254-B002-ARTH-EXISTS", FRONTEND_ARTIFACTS_HEADER_SNIPPETS),
        (args.frontend_artifacts, "M254-B002-ART-EXISTS", FRONTEND_ARTIFACTS_SNIPPETS),
        (args.driver_cpp, "M254-B002-DRV-EXISTS", DRIVER_SNIPPETS),
        (args.process_header, "M254-B002-PROCH-EXISTS", PROCESS_HEADER_SNIPPETS),
        (args.process_cpp, "M254-B002-PROC-EXISTS", PROCESS_CPP_SNIPPETS),
        (args.frontend_anchor_cpp, "M254-B002-FRONT-EXISTS", FRONTEND_ANCHOR_SNIPPETS),
        (args.runtime_header, "M254-B002-RTH-EXISTS", RUNTIME_HEADER_SNIPPETS),
        (args.runtime_source, "M254-B002-RTS-EXISTS", RUNTIME_SOURCE_SNIPPETS),
        (args.native_runtime_readme, "M254-B002-NRTR-EXISTS", NATIVE_RUNTIME_README_SNIPPETS),
        (args.test_runtime_readme, "M254-B002-TRTR-EXISTS", TEST_RUNTIME_README_SNIPPETS),
        (args.runtime_probe, "M254-B002-PRB-EXISTS", PROBE_SNIPPETS),
        (args.package_json, "M254-B002-PKG-EXISTS", PACKAGE_SNIPPETS),
    )
    for path, exists_check_id, snippets in doc_checks:
        added_checks, added_failures = check_doc_contract(
            path=path, exists_check_id=exists_check_id, snippets=snippets
        )
        checks_total += added_checks
        failures.extend(added_failures)

    dynamic_probes_executed = not args.skip_dynamic_probes
    if dynamic_probes_executed:
        case_checks, case_failures, case_payload = run_dynamic_case(args)
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
        "failures": [failure.__dict__ for failure in failures],
    }

    summary_out = args.summary_out
    if not summary_out.is_absolute():
        summary_out = ROOT / summary_out
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(canonical_json(payload), encoding="utf-8")

    if failures:
        print(f"[FAIL] M254-B002 bootstrap semantics drift; summary: {display_path(summary_out)}", file=sys.stderr)
        return 1
    print(f"[PASS] M254-B002 bootstrap semantics preserved; summary: {display_path(summary_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(run())
