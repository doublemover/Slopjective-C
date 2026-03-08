#!/usr/bin/env python3
"""Fail-closed checker for M254-D003 deterministic reset/replay expansion."""

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
MODE = "m254-d003-realization-sequencing-and-deterministic-reset-hooks-core-feature-expansion-v1"
CONTRACT_ID = "objc3c-runtime-bootstrap-reset-replay/m254-d003-v1"
SURFACE_PATH = "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_reset_contract"
BOOTSTRAP_API_CONTRACT_ID = "objc3c-runtime-bootstrap-api-freeze/m254-d001-v1"
BOOTSTRAP_REGISTRAR_CONTRACT_ID = "objc3c-runtime-bootstrap-registrar-image-walk/m254-d002-v1"
INTERNAL_HEADER_PATH = "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h"
REPLAY_SYMBOL = "objc3_runtime_replay_registered_images_for_testing"
RESET_REPLAY_SNAPSHOT_SYMBOL = "objc3_runtime_copy_reset_replay_state_for_testing"
RESET_LIFECYCLE_MODEL = "reset-clears-live-runtime-state-and-zeroes-image-local-init-cells"
REPLAY_ORDER_MODEL = "replay-re-registers-retained-images-in-original-registration-order"
IMAGE_LOCAL_INIT_STATE_RESET_MODEL = "retained-bootstrap-image-local-init-cells-reset-to-zero-before-replay"
BOOTSTRAP_CATALOG_RETENTION_MODEL = "bootstrap-catalog-retained-across-reset-for-deterministic-replay"
KNOWN_SELECTOR = "tokenValue"
UNKNOWN_SELECTOR = "__objc3_d003_unknown_selector"
REGISTRATION_MANIFEST_ARTIFACT = "module.runtime-registration-manifest.json"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m254_realization_sequencing_and_deterministic_reset_hooks_core_feature_expansion_d003_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m254" / "m254_d003_realization_sequencing_and_deterministic_reset_hooks_core_feature_expansion_packet.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_RUNTIME_README = ROOT / "native" / "objc3c" / "src" / "runtime" / "README.md"
DEFAULT_TOOLING_RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
DEFAULT_INTERNAL_HEADER = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
DEFAULT_RUNTIME_SOURCE = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
DEFAULT_FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
DEFAULT_DRIVER_CPP = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"
DEFAULT_PROCESS_HEADER = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.h"
DEFAULT_PROCESS_CPP = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_process.cpp"
DEFAULT_FRONTEND_ANCHOR_CPP = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
DEFAULT_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m254_d003_deterministic_reset_replay_probe.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
DEFAULT_RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
DEFAULT_PRIMARY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m254_c002_runtime_bootstrap_metadata_library.objc3"
DEFAULT_CATEGORY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m254_c003_runtime_bootstrap_category_library.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m254" / "d003-reset-replay"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m254/M254-D003/deterministic_reset_replay_summary.json")


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


@dataclass(frozen=True)
class DynamicCase:
    case_id: str
    fixture: Path


DYNAMIC_CASES = (
    DynamicCase("metadata-library", DEFAULT_PRIMARY_FIXTURE),
    DynamicCase("category-library", DEFAULT_CATEGORY_FIXTURE),
)

EXPECTATIONS_SNIPPETS = (
    SnippetCheck("M254-D003-DOC-EXP-01", "# M254 Realization Sequencing and Deterministic Reset Hooks Core Feature Expansion Expectations (D003)"),
    SnippetCheck("M254-D003-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M254-D003-DOC-EXP-03", f"`{SURFACE_PATH}`"),
    SnippetCheck("M254-D003-DOC-EXP-04", f"`{REPLAY_SYMBOL}`"),
    SnippetCheck("M254-D003-DOC-EXP-05", f"`{RESET_REPLAY_SNAPSHOT_SYMBOL}`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M254-D003-DOC-PKT-01", "# M254-D003 Realization Sequencing and Deterministic Reset Hooks Core Feature Expansion Packet"),
    SnippetCheck("M254-D003-DOC-PKT-02", "Packet: `M254-D003`"),
    SnippetCheck("M254-D003-DOC-PKT-03", f"`{CONTRACT_ID}`"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M254-D003-NDOC-01", "## Realization sequencing and deterministic reset hooks (M254-D003)"),
    SnippetCheck("M254-D003-NDOC-02", f"`{REPLAY_SYMBOL}`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M254-D003-SPC-01", "## M254 realization sequencing and deterministic reset hooks (D003)"),
    SnippetCheck("M254-D003-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M254-D003-SPC-03", f"`{RESET_LIFECYCLE_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M254-D003-META-01", "## M254 reset/replay metadata anchors (D003)"),
    SnippetCheck("M254-D003-META-02", "`bootstrap_reset_contract_id`"),
    SnippetCheck("M254-D003-META-03", "`bootstrap_reset_replay_registered_images_symbol`"),
)
RUNTIME_README_SNIPPETS = (
    SnippetCheck("M254-D003-RTDOC-01", "`M254-D003` expands that same private bootstrap surface with deterministic reset"),
    SnippetCheck("M254-D003-RTDOC-02", f"`{BOOTSTRAP_CATALOG_RETENTION_MODEL}`"),
)
TOOLING_RUNTIME_README_SNIPPETS = (
    SnippetCheck("M254-D003-TRTDOC-01", "`M254-D003` extends the private bootstrap probe surface with deterministic"),
    SnippetCheck("M254-D003-TRTDOC-02", f"`{RESET_REPLAY_SNAPSHOT_SYMBOL}`"),
)
INTERNAL_HEADER_SNIPPETS = (
    SnippetCheck("M254-D003-INTH-01", "typedef struct objc3_runtime_reset_replay_state_snapshot {"),
    SnippetCheck("M254-D003-INTH-02", REPLAY_SYMBOL),
    SnippetCheck("M254-D003-INTH-03", RESET_REPLAY_SNAPSHOT_SYMBOL),
)
RUNTIME_SOURCE_SNIPPETS = (
    SnippetCheck("M254-D003-RTS-01", "retained_bootstrap_identity_order"),
    SnippetCheck("M254-D003-RTS-02", "ZeroRetainedBootstrapImageLocalInitStatesUnlocked"),
    SnippetCheck("M254-D003-RTS-03", REPLAY_SYMBOL),
    SnippetCheck("M254-D003-RTS-04", RESET_REPLAY_SNAPSHOT_SYMBOL),
)
FRONTEND_ARTIFACTS_SNIPPETS = (
    SnippetCheck("M254-D003-ART-01", "objc_runtime_bootstrap_reset_contract"),
    SnippetCheck("M254-D003-ART-02", "runtime_bootstrap_reset_contract_id"),
    SnippetCheck("M254-D003-ART-03", "runtime_bootstrap_reset_replay_registered_images_symbol"),
)
DRIVER_SNIPPETS = (
    SnippetCheck("M254-D003-DRV-01", "bootstrap_reset_contract_id"),
    SnippetCheck("M254-D003-DRV-02", "bootstrap_reset_replay_registered_images_symbol"),
)
PROCESS_HEADER_SNIPPETS = (
    SnippetCheck("M254-D003-PROCH-01", "std::string bootstrap_reset_contract_id;"),
    SnippetCheck("M254-D003-PROCH-02", "std::string bootstrap_reset_replay_registered_images_symbol;"),
)
PROCESS_CPP_SNIPPETS = (
    SnippetCheck("M254-D003-PROC-01", "bootstrap_reset_contract_id"),
    SnippetCheck("M254-D003-PROC-02", "bootstrap_reset_bootstrap_catalog_retention_model"),
)
FRONTEND_ANCHOR_SNIPPETS = (
    SnippetCheck("M254-D003-FRONT-01", "bootstrap_reset_contract_id"),
    SnippetCheck("M254-D003-FRONT-02", "bootstrap_reset_replay_registered_images_symbol"),
)
PROBE_SNIPPETS = (
    SnippetCheck("M254-D003-PRB-01", '#include "runtime/objc3_runtime_bootstrap_internal.h"'),
    SnippetCheck("M254-D003-PRB-02", REPLAY_SYMBOL),
    SnippetCheck("M254-D003-PRB-03", RESET_REPLAY_SNAPSHOT_SYMBOL),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M254-D003-PKG-01", '"check:objc3c:m254-d003-realization-sequencing-and-deterministic-reset-hooks-core-feature-expansion": "python scripts/check_m254_d003_realization_sequencing_and_deterministic_reset_hooks_core_feature_expansion.py"'),
    SnippetCheck("M254-D003-PKG-02", '"test:tooling:m254-d003-realization-sequencing-and-deterministic-reset-hooks-core-feature-expansion": "python -m pytest tests/tooling/test_check_m254_d003_realization_sequencing_and_deterministic_reset_hooks_core_feature_expansion.py -q"'),
    SnippetCheck("M254-D003-PKG-03", '"check:objc3c:m254-d003-lane-d-readiness": "python scripts/run_m254_d003_lane_d_readiness.py"'),
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
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--runtime-readme", type=Path, default=DEFAULT_RUNTIME_README)
    parser.add_argument("--tooling-runtime-readme", type=Path, default=DEFAULT_TOOLING_RUNTIME_README)
    parser.add_argument("--internal-header", type=Path, default=DEFAULT_INTERNAL_HEADER)
    parser.add_argument("--runtime-source", type=Path, default=DEFAULT_RUNTIME_SOURCE)
    parser.add_argument("--frontend-artifacts", type=Path, default=DEFAULT_FRONTEND_ARTIFACTS)
    parser.add_argument("--driver-cpp", type=Path, default=DEFAULT_DRIVER_CPP)
    parser.add_argument("--process-header", type=Path, default=DEFAULT_PROCESS_HEADER)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--frontend-anchor-cpp", type=Path, default=DEFAULT_FRONTEND_ANCHOR_CPP)
    parser.add_argument("--probe", type=Path, default=DEFAULT_PROBE)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--runtime-library", type=Path, default=DEFAULT_RUNTIME_LIBRARY)
    parser.add_argument("--runtime-include-root", type=Path, default=DEFAULT_RUNTIME_INCLUDE_ROOT)
    parser.add_argument("--primary-fixture", type=Path, default=DEFAULT_PRIMARY_FIXTURE)
    parser.add_argument("--category-fixture", type=Path, default=DEFAULT_CATEGORY_FIXTURE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--clangxx", default="clang++.exe")
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M254-D003-MISSING", f"required artifact is missing: {display_path(path)}"))
        return 1 + len(snippets)
    text = read_text(path)
    passed = 1
    for snippet in snippets:
        if snippet.snippet in text:
            passed += 1
        else:
            failures.append(Finding(display_path(path), snippet.check_id, f"missing snippet: {snippet.snippet}"))
    return passed


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


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


def run_command(command: Sequence[str], cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        list(command),
        cwd=str(cwd or ROOT),
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def stale_inputs(target: Path, inputs: Sequence[Path]) -> list[str]:
    if not target.exists():
        return [display_path(path) for path in inputs]
    target_mtime = target.stat().st_mtime_ns
    stale: list[str] = []
    for path in inputs:
        if path.exists() and path.stat().st_mtime_ns > target_mtime:
            stale.append(display_path(path))
    return stale


def extract_reset_packet(manifest_payload: dict[str, Any]) -> dict[str, Any] | None:
    frontend = manifest_payload.get("frontend")
    if not isinstance(frontend, dict):
        return None
    pipeline = frontend.get("pipeline")
    if not isinstance(pipeline, dict):
        return None
    semantic_surface = pipeline.get("semantic_surface")
    if not isinstance(semantic_surface, dict):
        return None
    packet = semantic_surface.get("objc_runtime_bootstrap_reset_contract")
    return packet if isinstance(packet, dict) else None


def run_dynamic_case(*, args: argparse.Namespace, case_input: DynamicCase, clangxx: str, failures: list[Finding]) -> tuple[int, int, dict[str, object]]:
    checks_total = 0
    checks_passed = 0
    case: dict[str, object] = {"case_id": case_input.case_id, "fixture": display_path(case_input.fixture)}

    runtime_include_root = args.runtime_include_root.resolve()
    probe_source = args.probe.resolve()
    runtime_library = args.runtime_library.resolve()
    out_dir = args.probe_root.resolve() / case_input.case_id
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [str(args.native_exe.resolve()), str(case_input.fixture.resolve()), "--out-dir", str(out_dir), "--emit-prefix", "module"]
    llc = resolve_tool("llc.exe")
    if llc is not None:
        command.extend(["--llc", str(llc)])
    compile_result = run_command(command)

    manifest_path = out_dir / "module.manifest.json"
    registration_manifest_path = out_dir / REGISTRATION_MANIFEST_ARTIFACT
    obj_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"

    checks_total += 1
    checks_passed += require(compile_result.returncode == 0, display_path(case_input.fixture), f"M254-D003-{case_input.case_id}-COMPILE", f"native compile exited with {compile_result.returncode}", failures)
    checks_total += 1
    checks_passed += require(manifest_path.exists(), display_path(manifest_path), f"M254-D003-{case_input.case_id}-MANIFEST", "manifest is missing", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_path.exists(), display_path(registration_manifest_path), f"M254-D003-{case_input.case_id}-REG-MANIFEST", "registration manifest is missing", failures)
    checks_total += 1
    checks_passed += require(obj_path.exists(), display_path(obj_path), f"M254-D003-{case_input.case_id}-OBJ", "object file is missing", failures)
    if failures and any(f.check_id.startswith(f"M254-D003-{case_input.case_id}-") for f in failures):
        return checks_total, checks_passed, case

    manifest_payload = load_json(manifest_path)
    registration_manifest_payload = load_json(registration_manifest_path)
    reset_packet = extract_reset_packet(manifest_payload)
    backend_text = backend_path.read_text(encoding="utf-8").strip() if backend_path.exists() else ""

    checks_total += 1
    checks_passed += require(reset_packet is not None, display_path(manifest_path), f"M254-D003-{case_input.case_id}-PACKET", "reset semantic packet is missing", failures)
    if reset_packet is not None:
        checks_total += 1
        checks_passed += require(reset_packet.get("contract_id") == CONTRACT_ID, display_path(manifest_path), f"M254-D003-{case_input.case_id}-PACKET-ID", "reset contract id mismatch", failures)
        checks_total += 1
        checks_passed += require(reset_packet.get("replay_registered_images_symbol") == REPLAY_SYMBOL, display_path(manifest_path), f"M254-D003-{case_input.case_id}-PACKET-REPLAY", "replay symbol mismatch", failures)
        checks_total += 1
        checks_passed += require(reset_packet.get("reset_replay_state_snapshot_symbol") == RESET_REPLAY_SNAPSHOT_SYMBOL, display_path(manifest_path), f"M254-D003-{case_input.case_id}-PACKET-SNAPSHOT", "snapshot symbol mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_reset_contract_id") == CONTRACT_ID, display_path(registration_manifest_path), f"M254-D003-{case_input.case_id}-REG-ID", "registration manifest reset contract mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_reset_replay_registered_images_symbol") == REPLAY_SYMBOL, display_path(registration_manifest_path), f"M254-D003-{case_input.case_id}-REG-REPLAY", "registration manifest replay symbol mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_reset_reset_replay_state_snapshot_symbol") == RESET_REPLAY_SNAPSHOT_SYMBOL, display_path(registration_manifest_path), f"M254-D003-{case_input.case_id}-REG-SNAPSHOT", "registration manifest snapshot symbol mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_reset_lifecycle_model") == RESET_LIFECYCLE_MODEL, display_path(registration_manifest_path), f"M254-D003-{case_input.case_id}-REG-RESET-MODEL", "registration manifest reset lifecycle mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_reset_replay_order_model") == REPLAY_ORDER_MODEL, display_path(registration_manifest_path), f"M254-D003-{case_input.case_id}-REG-REPLAY-MODEL", "registration manifest replay order mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_reset_image_local_init_state_reset_model") == IMAGE_LOCAL_INIT_STATE_RESET_MODEL, display_path(registration_manifest_path), f"M254-D003-{case_input.case_id}-REG-INIT-RESET", "registration manifest init-state reset model mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_reset_bootstrap_catalog_retention_model") == BOOTSTRAP_CATALOG_RETENTION_MODEL, display_path(registration_manifest_path), f"M254-D003-{case_input.case_id}-REG-CATALOG", "registration manifest catalog retention model mismatch", failures)
    checks_total += 1
    checks_passed += require(backend_text == "llvm-direct", display_path(backend_path), f"M254-D003-{case_input.case_id}-BACKEND", "object backend must remain llvm-direct", failures)
    if failures and any(f.check_id.startswith(f"M254-D003-{case_input.case_id}-") for f in failures):
        return checks_total, checks_passed, case

    probe_out_dir = args.probe_root.resolve() / f"{case_input.case_id}-probe"
    probe_out_dir.mkdir(parents=True, exist_ok=True)
    probe_exe = probe_out_dir / f"{case_input.case_id}-deterministic-reset-replay-probe.exe"
    probe_compile_command = [clangxx, "-std=c++20", "-Wall", "-Wextra", "-pedantic", f"-I{runtime_include_root}", str(probe_source), str(obj_path), str(runtime_library), "-o", str(probe_exe)]
    probe_compile = run_command(probe_compile_command)
    checks_total += 1
    checks_passed += require(probe_compile.returncode == 0, display_path(probe_source), f"M254-D003-{case_input.case_id}-PROBE-COMPILE", f"probe compile exited with {probe_compile.returncode}", failures)
    checks_total += 1
    checks_passed += require(probe_exe.exists(), display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-EXE", "probe executable is missing", failures)
    if failures and any(f.check_id.startswith(f"M254-D003-{case_input.case_id}-") for f in failures):
        return checks_total, checks_passed, case

    probe_run = run_command([str(probe_exe)])
    checks_total += 1
    checks_passed += require(probe_run.returncode == 0, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-RUN", f"probe exited with {probe_run.returncode}", failures)
    if probe_run.returncode != 0:
        return checks_total, checks_passed, case

    try:
        probe_payload = json.loads(probe_run.stdout)
    except json.JSONDecodeError as exc:
        failures.append(Finding(display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-JSON", f"invalid probe JSON: {exc}"))
        return checks_total + 1, checks_passed, case

    expected_identity_key = str(registration_manifest_payload.get("translation_unit_identity_key", ""))
    checks_total += 1
    checks_passed += require(probe_payload.get("startup_registration_copy_status") == 0, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-START-REG", "startup registration snapshot copy must succeed", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("startup_image_walk_copy_status") == 0, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-START-WALK", "startup image-walk snapshot copy must succeed", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("startup_reset_replay_copy_status") == 0, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-START-RESET", "startup reset/replay snapshot copy must succeed", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("startup_registered_image_count") == 1, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-START-COUNT", "startup registered image count mismatch", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("startup_walked_image_count") == 1, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-START-WALKED", "startup walked image count mismatch", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("startup_known_selector_stable_id", 0) > 0, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-START-SELECTOR", "startup known selector must be interned", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("post_reset_registration_copy_status") == 0, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-RESET-REG", "post-reset registration snapshot copy must succeed", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("post_reset_reset_replay_copy_status") == 0, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-RESET-SNAPSHOT", "post-reset reset/replay snapshot copy must succeed", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("post_reset_registered_image_count") == 0, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-RESET-COUNT", "post-reset registered image count must be zero", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("post_reset_next_expected_registration_order_ordinal") == 1, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-RESET-ORDINAL", "post-reset next expected ordinal must reset to 1", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("post_reset_last_reset_cleared_image_local_init_state_count", 0) >= 1, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-RESET-CLEARED", "reset must clear at least one image-local init-state cell", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("post_reset_retained_bootstrap_image_count") == 1, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-RESET-CATALOG", "retained bootstrap image count mismatch", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("post_reset_reset_generation", 0) >= 1, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-RESET-GEN", "reset generation must advance", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("replay_status") == 0, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-REPLAY-STATUS", "replay must succeed", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("post_replay_registration_copy_status") == 0, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-REPLAY-REG", "post-replay registration snapshot copy must succeed", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("post_replay_image_walk_copy_status") == 0, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-REPLAY-WALK", "post-replay image-walk snapshot copy must succeed", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("post_replay_reset_replay_copy_status") == 0, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-REPLAY-SNAPSHOT", "post-replay reset/replay snapshot copy must succeed", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("post_replay_registered_image_count") == 1, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-REPLAY-COUNT", "post-replay registered image count mismatch", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("post_replay_walked_image_count") == 1, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-REPLAY-WALKED", "post-replay walked image count mismatch", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("post_replay_last_registration_status") == 0, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-REPLAY-LAST-STATUS", "post-replay last registration status mismatch", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("post_replay_last_registration_used_staged_table") == 1, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-REPLAY-STAGED", "replay must use the staged-table path", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("post_replay_last_registered_translation_unit_identity_key") == expected_identity_key, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-REPLAY-REG-TU", "post-replay registered identity mismatch", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("post_replay_last_walked_translation_unit_identity_key") == expected_identity_key, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-REPLAY-WALK-TU", "post-replay walked identity mismatch", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("post_replay_last_replayed_translation_unit_identity_key") == expected_identity_key, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-REPLAY-LAST-TU", "post-replay replayed identity mismatch", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("post_replay_last_replayed_image_count") == 1, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-REPLAY-LAST-COUNT", "post-replay last replayed image count mismatch", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("post_replay_replay_generation", 0) >= 1, display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-REPLAY-GEN", "replay generation must advance", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("replay_known_selector_stable_id") == probe_payload.get("startup_known_selector_stable_id"), display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-REPLAY-SELECTOR", "known selector stable id must replay deterministically", failures)
    checks_total += 1
    checks_passed += require(probe_payload.get("replay_unknown_selector_stable_id", 0) > probe_payload.get("post_replay_last_walked_selector_pool_count", 0), display_path(probe_exe), f"M254-D003-{case_input.case_id}-PROBE-REPLAY-UNKNOWN", "unknown selector must allocate after the replayed selector pool", failures)

    case.update({
        "command": command,
        "probe_compile_command": probe_compile_command,
        "probe_run_command": [str(probe_exe)],
        "registration_manifest_path": display_path(registration_manifest_path),
        "probe_payload": probe_payload,
    })
    return checks_total, checks_passed, case


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    artifacts = (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.runtime_readme, RUNTIME_README_SNIPPETS),
        (args.tooling_runtime_readme, TOOLING_RUNTIME_README_SNIPPETS),
        (args.internal_header, INTERNAL_HEADER_SNIPPETS),
        (args.runtime_source, RUNTIME_SOURCE_SNIPPETS),
        (args.frontend_artifacts, FRONTEND_ARTIFACTS_SNIPPETS),
        (args.driver_cpp, DRIVER_SNIPPETS),
        (args.process_header, PROCESS_HEADER_SNIPPETS),
        (args.process_cpp, PROCESS_CPP_SNIPPETS),
        (args.frontend_anchor_cpp, FRONTEND_ANCHOR_SNIPPETS),
        (args.probe, PROBE_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    )
    for path, snippets in artifacts:
        checks_total += 1 + len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    dynamic_cases: list[dict[str, object]] = []
    dynamic_probes_executed = not args.skip_dynamic_probes
    if dynamic_probes_executed:
        native_stale = stale_inputs(
            args.native_exe.resolve(),
            (
                args.internal_header.resolve(),
                args.runtime_source.resolve(),
                args.frontend_artifacts.resolve(),
                args.driver_cpp.resolve(),
                args.process_header.resolve(),
                args.process_cpp.resolve(),
                args.frontend_anchor_cpp.resolve(),
            ),
        )
        runtime_library_stale = stale_inputs(
            args.runtime_library.resolve(),
            (args.internal_header.resolve(), args.runtime_source.resolve()),
        )
        clangxx = resolve_tool(args.clangxx)
        checks_total += 1
        checks_passed += require(not native_stale, display_path(args.native_exe), "M254-D003-STALE-NATIVE", "native executable is older than D003 inputs: " + ", ".join(native_stale), failures)
        checks_total += 1
        checks_passed += require(not runtime_library_stale, display_path(args.runtime_library), "M254-D003-STALE-RUNTIME", "runtime library is older than D003 inputs: " + ", ".join(runtime_library_stale), failures)
        checks_total += 1
        checks_passed += require(clangxx is not None, display_path(args.probe), "M254-D003-TOOL-CLANGXX", "clang++ is required", failures)
        if not failures:
            clangxx_text = str(clangxx)
            for case_input in DYNAMIC_CASES:
                case_total, case_passed, case_payload = run_dynamic_case(args=args, case_input=case_input, clangxx=clangxx_text, failures=failures)
                checks_total += case_total
                checks_passed += case_passed
                dynamic_cases.append(case_payload)

    checks_passed = checks_total - len(failures)
    summary = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "dynamic_probes_executed": dynamic_probes_executed,
        "dynamic_cases": dynamic_cases,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in failures
        ],
    }
    summary_out = args.summary_out
    if not summary_out.is_absolute():
        summary_out = ROOT / summary_out
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if not failures:
        return 0
    for finding in failures:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run())
