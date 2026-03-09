#!/usr/bin/env python3
"""Fail-closed checker for M255-D002 selector interning and lookup tables."""

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
MODE = "m255-d002-selector-interning-and-lookup-tables-core-feature-implementation-v1"
CONTRACT_ID = "objc3c-runtime-selector-lookup-tables/m255-d002-v1"
LOOKUP_DISPATCH_CONTRACT_ID = "objc3c-runtime-lookup-dispatch-freeze/m255-d001-v1"
BOOTSTRAP_REGISTRAR_CONTRACT_ID = "objc3c-runtime-bootstrap-registrar-image-walk/m254-d002-v1"
BOOTSTRAP_RESET_CONTRACT_ID = "objc3c-runtime-bootstrap-reset-replay/m254-d003-v1"
SELECTOR_TABLE_STATE_SYMBOL = "objc3_runtime_copy_selector_lookup_table_state_for_testing"
SELECTOR_ENTRY_STATE_SYMBOL = "objc3_runtime_copy_selector_lookup_entry_for_testing"
SELECTOR_INTERNING_MODEL = "registered-selector-pools-materialize-process-global-stable-id-table"
SELECTOR_MERGE_MODEL = "per-image-selector-pools-deduplicated-and-merged-across-registration-order"
SELECTOR_FALLBACK_MODEL = "unknown-selector-lookups-remain-dynamic-until-m255-d003"
SELECTOR_REPLAY_MODEL = "reset-replay-rebuilds-metadata-backed-selector-table-in-registration-order"
IR_SELECTOR_COMMENT_PREFIX = "; runtime_selector_lookup_tables = contract=" + CONTRACT_ID
MANUAL_IDENTITY_KEY = "m255-d002::manual-image"
EXPECTED_EMITTED_SELECTORS = ["currentValue", "setCurrentValue:", "shared", "tokenValue"]
EXPECTED_EMITTED_SELECTOR_IDS = {
    name: index + 1 for index, name in enumerate(EXPECTED_EMITTED_SELECTORS)
}
EXPECTED_MANUAL_SELECTOR_ID = len(EXPECTED_EMITTED_SELECTORS) + 1
EXPECTED_DYNAMIC_SELECTOR_ID = len(EXPECTED_EMITTED_SELECTORS) + 2

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m255_selector_interning_and_lookup_tables_core_feature_implementation_d002_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m255" / "m255_d002_selector_interning_and_lookup_tables_core_feature_implementation_packet.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_RUNTIME_README = ROOT / "native" / "objc3c" / "src" / "runtime" / "README.md"
DEFAULT_TOOLING_RUNTIME_README = ROOT / "tests" / "tooling" / "runtime" / "README.md"
DEFAULT_LOWERING_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
DEFAULT_IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
DEFAULT_PARSER = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
DEFAULT_RUNTIME_SOURCE = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
DEFAULT_BOOTSTRAP_HEADER = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime_bootstrap_internal.h"
DEFAULT_SHIM = ROOT / "tests" / "tooling" / "runtime" / "objc3_msgsend_i32_shim.c"
DEFAULT_RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m255_d002_selector_lookup_tables_probe.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
DEFAULT_RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
DEFAULT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m254_c002_runtime_bootstrap_metadata_library.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m255" / "d002-selector-lookup-tables"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m255/M255-D002/selector_lookup_tables_summary.json")


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
    SnippetCheck("M255-D002-DOC-EXP-01", "# M255 Selector Interning and Lookup Tables Core Feature Implementation Expectations (D002)"),
    SnippetCheck("M255-D002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M255-D002-DOC-EXP-03", f"`{SELECTOR_TABLE_STATE_SYMBOL}`"),
    SnippetCheck("M255-D002-DOC-EXP-04", f"`{SELECTOR_ENTRY_STATE_SYMBOL}`"),
)
PACKET_SNIPPETS = (
    SnippetCheck("M255-D002-PKT-01", "# M255-D002 Selector Interning and Lookup Tables Core Feature Implementation Packet"),
    SnippetCheck("M255-D002-PKT-02", "Packet: `M255-D002`"),
    SnippetCheck("M255-D002-PKT-03", f"`{BOOTSTRAP_REGISTRAR_CONTRACT_ID}`"),
    SnippetCheck("M255-D002-PKT-04", "real runtime capability backed by emitted startup selector pools"),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck("M255-D002-NDOC-01", "## Selector interning and lookup tables (M255-D002)"),
    SnippetCheck("M255-D002-NDOC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M255-D002-NDOC-03", f"`{SELECTOR_REPLAY_MODEL}`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck("M255-D002-SPC-01", "## M255 selector interning and lookup tables (D002)"),
    SnippetCheck("M255-D002-SPC-02", f"`{SELECTOR_MERGE_MODEL}`"),
    SnippetCheck("M255-D002-SPC-03", f"`{SELECTOR_FALLBACK_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck("M255-D002-META-01", "## M255 selector lookup table metadata anchors (D002)"),
    SnippetCheck("M255-D002-META-02", f"`{SELECTOR_TABLE_STATE_SYMBOL}`"),
    SnippetCheck("M255-D002-META-03", f"`{SELECTOR_ENTRY_STATE_SYMBOL}`"),
)
ARCHITECTURE_SNIPPETS = (
    SnippetCheck("M255-D002-ARCH-01", "## M255 selector interning and lookup tables (D002)"),
    SnippetCheck("M255-D002-ARCH-02", "duplicate selector spellings across images merge onto one stable ID"),
)
RUNTIME_README_SNIPPETS = (
    SnippetCheck("M255-D002-RTDOC-01", "`M255-D002` then turns selector pools into a real runtime-owned selector table"),
    SnippetCheck("M255-D002-RTDOC-02", f"`{SELECTOR_TABLE_STATE_SYMBOL}`"),
)
TOOLING_RUNTIME_README_SNIPPETS = (
    SnippetCheck("M255-D002-TRTDOC-01", "`M255-D002` adds the selector-table proof surface"),
    SnippetCheck("M255-D002-TRTDOC-02", f"`{SELECTOR_ENTRY_STATE_SYMBOL}`"),
)
LOWERING_HEADER_SNIPPETS = (
    SnippetCheck("M255-D002-LHC-01", "kObjc3RuntimeSelectorLookupTablesContractId"),
    SnippetCheck("M255-D002-LHC-02", "kObjc3RuntimeSelectorLookupTablesMergeModel"),
    SnippetCheck("M255-D002-LHC-03", SELECTOR_INTERNING_MODEL),
    SnippetCheck("M255-D002-LHC-04", SELECTOR_REPLAY_MODEL),
)
IR_EMITTER_SNIPPETS = (
    SnippetCheck("M255-D002-IR-01", "; runtime_selector_lookup_tables = contract="),
    SnippetCheck("M255-D002-IR-02", "kObjc3RuntimeSelectorLookupTablesContractId"),
)
PARSER_SNIPPETS = (
    SnippetCheck("M255-D002-PAR-01", "M255-D002 selector-table anchor"),
    SnippetCheck("M255-D002-PAR-02", "metadata-backed lookup tables"),
)
RUNTIME_SOURCE_SNIPPETS = (
    SnippetCheck("M255-D002-RTS-01", "objc3_runtime_copy_selector_lookup_table_state_for_testing"),
    SnippetCheck("M255-D002-RTS-02", "objc3_runtime_copy_selector_lookup_entry_for_testing"),
    SnippetCheck("M255-D002-RTS-03", "MaterializeSelectorLookupEntryUnlocked"),
)
BOOTSTRAP_HEADER_SNIPPETS = (
    SnippetCheck("M255-D002-BH-01", "typedef struct objc3_runtime_selector_lookup_table_state_snapshot"),
    SnippetCheck("M255-D002-BH-02", SELECTOR_TABLE_STATE_SYMBOL),
    SnippetCheck("M255-D002-BH-03", SELECTOR_ENTRY_STATE_SYMBOL),
)
SHIM_SNIPPETS = (
    SnippetCheck("M255-D002-SHIM-01", "M255-D002 selector lookup table boundary"),
    SnippetCheck("M255-D002-SHIM-02", "metadata-backed lookup tables now live behind the native"),
)
PROBE_SNIPPETS = (
    SnippetCheck("M255-D002-PRB-01", '#include "runtime/objc3_runtime_bootstrap_internal.h"'),
    SnippetCheck("M255-D002-PRB-02", MANUAL_IDENTITY_KEY),
    SnippetCheck("M255-D002-PRB-03", "manualOnly:"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck("M255-D002-PKG-01", '"check:objc3c:m255-d002-selector-interning-and-lookup-tables-core-feature-implementation": "python scripts/check_m255_d002_selector_interning_and_lookup_tables_core_feature_implementation.py"'),
    SnippetCheck("M255-D002-PKG-02", '"test:tooling:m255-d002-selector-interning-and-lookup-tables-core-feature-implementation": "python -m pytest tests/tooling/test_check_m255_d002_selector_interning_and_lookup_tables_core_feature_implementation.py -q"'),
    SnippetCheck("M255-D002-PKG-03", '"check:objc3c:m255-d002-lane-d-readiness": "python scripts/run_m255_d002_lane_d_readiness.py"'),
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
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--runtime-readme", type=Path, default=DEFAULT_RUNTIME_README)
    parser.add_argument("--tooling-runtime-readme", type=Path, default=DEFAULT_TOOLING_RUNTIME_README)
    parser.add_argument("--lowering-header", type=Path, default=DEFAULT_LOWERING_HEADER)
    parser.add_argument("--ir-emitter", type=Path, default=DEFAULT_IR_EMITTER)
    parser.add_argument("--parser-cpp", type=Path, default=DEFAULT_PARSER)
    parser.add_argument("--runtime-source", type=Path, default=DEFAULT_RUNTIME_SOURCE)
    parser.add_argument("--bootstrap-header", type=Path, default=DEFAULT_BOOTSTRAP_HEADER)
    parser.add_argument("--shim-source", type=Path, default=DEFAULT_SHIM)
    parser.add_argument("--runtime-probe", type=Path, default=DEFAULT_RUNTIME_PROBE)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--native-exe", type=Path, default=DEFAULT_NATIVE_EXE)
    parser.add_argument("--runtime-library", type=Path, default=DEFAULT_RUNTIME_LIBRARY)
    parser.add_argument("--runtime-include-root", type=Path, default=DEFAULT_RUNTIME_INCLUDE_ROOT)
    parser.add_argument("--fixture", type=Path, default=DEFAULT_FIXTURE)
    parser.add_argument("--probe-root", type=Path, default=DEFAULT_PROBE_ROOT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--clangxx", default="clang++.exe")
    parser.add_argument("--skip-dynamic-probes", action="store_true")
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def ensure_snippets(path: Path, snippets: Sequence[SnippetCheck], failures: list[Finding]) -> int:
    if not path.exists():
        failures.append(Finding(display_path(path), "M255-D002-MISSING", f"required artifact is missing: {display_path(path)}"))
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


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected JSON object in {display_path(path)}")
    return payload


def find_first_key(payload: Any, target_key: str) -> Any | None:
    if isinstance(payload, dict):
        if target_key in payload:
            return payload[target_key]
        for value in payload.values():
            found = find_first_key(value, target_key)
            if found is not None:
                return found
    elif isinstance(payload, list):
        for value in payload:
            found = find_first_key(value, target_key)
            if found is not None:
                return found
    return None


def extract_expected_selectors(manifest_payload: dict[str, Any]) -> list[str]:
    records = manifest_payload.get("runtime_metadata_source_records")
    if not isinstance(records, dict):
        fallback = find_first_key(manifest_payload, "runtime_metadata_source_records")
        records = fallback if isinstance(fallback, dict) else None
    if not isinstance(records, dict):
        return []
    selectors: set[str] = set()
    properties = records.get("properties")
    if isinstance(properties, list):
        for record in properties:
            if not isinstance(record, dict):
                continue
            if record.get("has_getter") and isinstance(record.get("getter_selector"), str):
                selector = record["getter_selector"].strip()
                if selector:
                    selectors.add(selector)
            if record.get("has_setter") and isinstance(record.get("setter_selector"), str):
                selector = record["setter_selector"].strip()
                if selector:
                    selectors.add(selector)
    methods = records.get("methods")
    if isinstance(methods, list):
        for record in methods:
            if not isinstance(record, dict):
                continue
            selector = record.get("selector")
            if isinstance(selector, str) and selector.strip():
                selectors.add(selector.strip())
    return sorted(selectors)


def check_selector_entry(
    *,
    payload: dict[str, Any],
    artifact: str,
    check_prefix: str,
    expected_found: int,
    expected_metadata_backed: int,
    expected_stable_id: int,
    expected_provider_count: int,
    expected_first_ordinal: int,
    expected_last_ordinal: int,
    expected_first_pool_index: int,
    expected_last_pool_index: int,
    expected_selector: str | None,
    failures: list[Finding],
) -> tuple[int, int]:
    checks_total = 0
    checks_passed = 0
    checks_total += 1
    checks_passed += require(int(payload.get("found", -1)) == expected_found, artifact, f"{check_prefix}-FOUND", "unexpected found flag", failures)
    checks_total += 1
    checks_passed += require(int(payload.get("metadata_backed", -1)) == expected_metadata_backed, artifact, f"{check_prefix}-METADATA", "unexpected metadata-backed flag", failures)
    checks_total += 1
    checks_passed += require(int(payload.get("stable_id", -1)) == expected_stable_id, artifact, f"{check_prefix}-STABLE-ID", "unexpected stable id", failures)
    checks_total += 1
    checks_passed += require(int(payload.get("metadata_provider_count", -1)) == expected_provider_count, artifact, f"{check_prefix}-PROVIDERS", "unexpected provider count", failures)
    checks_total += 1
    checks_passed += require(int(payload.get("first_registration_order_ordinal", -1)) == expected_first_ordinal, artifact, f"{check_prefix}-FIRST-ORD", "unexpected first registration ordinal", failures)
    checks_total += 1
    checks_passed += require(int(payload.get("last_registration_order_ordinal", -1)) == expected_last_ordinal, artifact, f"{check_prefix}-LAST-ORD", "unexpected last registration ordinal", failures)
    checks_total += 1
    checks_passed += require(int(payload.get("first_selector_pool_index", -1)) == expected_first_pool_index, artifact, f"{check_prefix}-FIRST-POOL", "unexpected first selector-pool index", failures)
    checks_total += 1
    checks_passed += require(int(payload.get("last_selector_pool_index", -1)) == expected_last_pool_index, artifact, f"{check_prefix}-LAST-POOL", "unexpected last selector-pool index", failures)
    if expected_selector is not None:
        checks_total += 1
        checks_passed += require(payload.get("canonical_selector") == expected_selector, artifact, f"{check_prefix}-SELECTOR", "unexpected canonical selector spelling", failures)
    return checks_total, checks_passed


def run_dynamic_probe(
    *,
    args: argparse.Namespace,
    clangxx: str,
    failures: list[Finding],
) -> tuple[int, int, dict[str, object]]:
    checks_total = 0
    checks_passed = 0
    case: dict[str, object] = {"fixture": display_path(args.fixture)}

    runtime_include_root = args.runtime_include_root.resolve()
    runtime_probe = args.runtime_probe.resolve()
    runtime_library = args.runtime_library.resolve()
    out_dir = args.probe_root.resolve() / "fixture"
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [
        str(args.native_exe.resolve()),
        str(args.fixture.resolve()),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ]
    llc = resolve_tool("llc.exe")
    if llc is not None:
        command.extend(["--llc", str(llc)])
    compile_result = run_command(command)

    manifest_path = out_dir / "module.manifest.json"
    registration_manifest_path = out_dir / "module.runtime-registration-manifest.json"
    ir_path = out_dir / "module.ll"
    obj_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"

    checks_total += 1
    checks_passed += require(compile_result.returncode == 0, display_path(args.fixture), "M255-D002-DYN-COMPILE", f"native compile exited with {compile_result.returncode}", failures)
    for suffix, path in (("MANIFEST", manifest_path), ("REG-MANIFEST", registration_manifest_path), ("IR", ir_path), ("OBJ", obj_path), ("BACKEND", backend_path)):
        checks_total += 1
        checks_passed += require(path.exists(), display_path(path), f"M255-D002-DYN-{suffix}", f"required artifact missing: {display_path(path)}", failures)
    if failures and any(f.check_id.startswith("M255-D002-DYN-") for f in failures):
        return checks_total, checks_passed, case

    manifest_payload = load_json(manifest_path)
    registration_manifest_payload = load_json(registration_manifest_path)
    backend_text = read_text(backend_path).strip()
    ir_text = read_text(ir_path)
    expected_selectors = extract_expected_selectors(manifest_payload)
    case["expected_selectors"] = expected_selectors

    checks_total += 1
    checks_passed += require(expected_selectors == EXPECTED_EMITTED_SELECTORS, display_path(manifest_path), "M255-D002-DYN-SELECTOR-SET", "unexpected emitted selector set", failures)
    checks_total += 1
    checks_passed += require(backend_text == "llvm-direct", display_path(backend_path), "M255-D002-DYN-BACKEND", "object backend must remain llvm-direct", failures)
    checks_total += 1
    checks_passed += require(IR_SELECTOR_COMMENT_PREFIX in ir_text, display_path(ir_path), "M255-D002-DYN-IR-COMMENT", "selector lookup table IR comment missing", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_registrar_contract_id") == BOOTSTRAP_REGISTRAR_CONTRACT_ID, display_path(registration_manifest_path), "M255-D002-DYN-REG-REGISTRAR", "bootstrap registrar contract mismatch", failures)
    checks_total += 1
    checks_passed += require(registration_manifest_payload.get("bootstrap_reset_contract_id") == BOOTSTRAP_RESET_CONTRACT_ID, display_path(registration_manifest_path), "M255-D002-DYN-REG-RESET", "bootstrap reset contract mismatch", failures)
    if failures and any(f.check_id.startswith("M255-D002-DYN-") for f in failures):
        return checks_total, checks_passed, case

    probe_out_dir = args.probe_root.resolve() / "probe"
    probe_out_dir.mkdir(parents=True, exist_ok=True)
    probe_exe = probe_out_dir / "m255-d002-selector-lookup-tables-probe.exe"
    probe_compile = run_command(
        [
            clangxx,
            "-std=c++20",
            "-Wall",
            "-Wextra",
            "-pedantic",
            f"-I{runtime_include_root}",
            str(runtime_probe),
            str(obj_path),
            str(runtime_library),
            "-o",
            str(probe_exe),
        ]
    )
    checks_total += 1
    checks_passed += require(probe_compile.returncode == 0, display_path(runtime_probe), "M255-D002-DYN-PROBE-COMPILE", f"probe compile exited with {probe_compile.returncode}", failures)
    checks_total += 1
    checks_passed += require(probe_exe.exists(), display_path(probe_exe), "M255-D002-DYN-PROBE-EXE", "probe executable missing", failures)
    if failures and any(f.check_id.startswith("M255-D002-DYN-") for f in failures):
        return checks_total, checks_passed, case

    probe_run = run_command([str(probe_exe)])
    checks_total += 1
    checks_passed += require(probe_run.returncode == 0, display_path(probe_exe), "M255-D002-DYN-PROBE-RUN", f"probe exited with {probe_run.returncode}", failures)
    if probe_run.returncode != 0:
        case["probe_stdout"] = probe_run.stdout
        case["probe_stderr"] = probe_run.stderr
        return checks_total, checks_passed, case

    try:
        probe_payload = json.loads(probe_run.stdout)
    except json.JSONDecodeError as exc:
        failures.append(Finding(display_path(probe_exe), "M255-D002-DYN-PROBE-JSON", f"invalid probe JSON: {exc}"))
        return checks_total + 1, checks_passed, case

    case["probe_payload"] = probe_payload
    emitted_identity_key = str(registration_manifest_payload.get("translation_unit_identity_key", ""))

    for key in (
        "startup_registration_status",
        "startup_image_walk_status",
        "startup_table_status",
        "startup_token_status",
        "startup_current_status",
        "startup_set_current_status",
        "startup_shared_status",
        "startup_manual_only_before_status",
        "manual_register_status",
        "after_manual_registration_status",
        "after_manual_image_walk_status",
        "after_manual_table_status",
        "after_manual_token_status",
        "after_manual_debug_name_status",
        "after_dynamic_table_status",
        "after_dynamic_manual_only_status",
        "after_reset_registration_status",
        "after_reset_table_status",
        "replay_status",
        "after_replay_registration_status",
        "after_replay_image_walk_status",
        "after_replay_reset_replay_status",
        "after_replay_table_status",
        "after_replay_token_status",
        "after_replay_debug_name_status",
        "after_replay_manual_only_before_status",
        "after_replay_dynamic_table_status",
        "after_replay_dynamic_manual_only_status",
    ):
        checks_total += 1
        checks_passed += require(int(probe_payload.get(key, -999)) == 0, display_path(probe_exe), f"M255-D002-DYN-{key.upper()}", f"expected zero status for {key}", failures)

    startup_registration = probe_payload.get("startup_registration", {})
    startup_image_walk = probe_payload.get("startup_image_walk", {})
    startup_table = probe_payload.get("startup_table", {})
    after_manual_registration = probe_payload.get("after_manual_registration", {})
    after_manual_image_walk = probe_payload.get("after_manual_image_walk", {})
    after_manual_table = probe_payload.get("after_manual_table", {})
    after_dynamic_table = probe_payload.get("after_dynamic_table", {})
    after_reset_registration = probe_payload.get("after_reset_registration", {})
    after_reset_table = probe_payload.get("after_reset_table", {})
    after_replay_registration = probe_payload.get("after_replay_registration", {})
    after_replay_image_walk = probe_payload.get("after_replay_image_walk", {})
    after_replay_reset_replay = probe_payload.get("after_replay_reset_replay", {})
    after_replay_table = probe_payload.get("after_replay_table", {})
    after_replay_dynamic_table = probe_payload.get("after_replay_dynamic_table", {})

    checks_total += 1
    checks_passed += require(int(startup_registration.get("registered_image_count", -1)) == 1, display_path(probe_exe), "M255-D002-DYN-STARTUP-IMAGE-COUNT", "startup must register the emitted fixture image", failures)
    checks_total += 1
    checks_passed += require(startup_registration.get("last_registered_translation_unit_identity_key") == emitted_identity_key, display_path(probe_exe), "M255-D002-DYN-STARTUP-IDENTITY", "startup identity key mismatch", failures)
    checks_total += 1
    checks_passed += require(int(startup_image_walk.get("last_walked_selector_pool_count", -1)) == len(EXPECTED_EMITTED_SELECTORS), display_path(probe_exe), "M255-D002-DYN-STARTUP-POOL-COUNT", "unexpected startup selector-pool count", failures)
    checks_total += 1
    checks_passed += require(int(startup_image_walk.get("last_registration_used_staged_table", -1)) == 1, display_path(probe_exe), "M255-D002-DYN-STARTUP-STAGED", "startup should use the staged registration table", failures)
    checks_total += 1
    checks_passed += require(startup_image_walk.get("last_walked_translation_unit_identity_key") == emitted_identity_key, display_path(probe_exe), "M255-D002-DYN-STARTUP-WALK-ID", "startup walked identity key mismatch", failures)

    checks_total += 1
    checks_passed += require(int(startup_table.get("selector_table_entry_count", -1)) == len(EXPECTED_EMITTED_SELECTORS), display_path(probe_exe), "M255-D002-DYN-STARTUP-TABLE-COUNT", "unexpected startup selector table size", failures)
    checks_total += 1
    checks_passed += require(int(startup_table.get("metadata_backed_selector_count", -1)) == len(EXPECTED_EMITTED_SELECTORS), display_path(probe_exe), "M255-D002-DYN-STARTUP-META-COUNT", "unexpected startup metadata-backed selector count", failures)
    checks_total += 1
    checks_passed += require(int(startup_table.get("dynamic_selector_count", -1)) == 0, display_path(probe_exe), "M255-D002-DYN-STARTUP-DYNAMIC-COUNT", "startup must not create dynamic selectors", failures)
    checks_total += 1
    checks_passed += require(int(startup_table.get("metadata_provider_edge_count", -1)) == len(EXPECTED_EMITTED_SELECTORS), display_path(probe_exe), "M255-D002-DYN-STARTUP-PROVIDER-COUNT", "unexpected startup provider-edge count", failures)
    checks_total += 1
    checks_passed += require(startup_table.get("last_materialized_selector") == "tokenValue", display_path(probe_exe), "M255-D002-DYN-STARTUP-LAST-SELECTOR", "unexpected last startup materialized selector", failures)
    checks_total += 1
    checks_passed += require(int(startup_table.get("last_materialized_stable_id", -1)) == EXPECTED_EMITTED_SELECTOR_IDS["tokenValue"], display_path(probe_exe), "M255-D002-DYN-STARTUP-LAST-ID", "unexpected startup last materialized stable id", failures)

    expected_entry_map = {
        "startup_current": ("currentValue", EXPECTED_EMITTED_SELECTOR_IDS["currentValue"], 1),
        "startup_set_current": ("setCurrentValue:", EXPECTED_EMITTED_SELECTOR_IDS["setCurrentValue:"], 2),
        "startup_shared": ("shared", EXPECTED_EMITTED_SELECTOR_IDS["shared"], 3),
        "startup_token": ("tokenValue", EXPECTED_EMITTED_SELECTOR_IDS["tokenValue"], 4),
    }
    for payload_key, (selector, stable_id, pool_index) in expected_entry_map.items():
        entry_total, entry_passed = check_selector_entry(
            payload=probe_payload.get(payload_key, {}),
            artifact=display_path(probe_exe),
            check_prefix=f"M255-D002-DYN-{payload_key.upper()}",
            expected_found=1,
            expected_metadata_backed=1,
            expected_stable_id=stable_id,
            expected_provider_count=1,
            expected_first_ordinal=1,
            expected_last_ordinal=1,
            expected_first_pool_index=pool_index,
            expected_last_pool_index=pool_index,
            expected_selector=selector,
            failures=failures,
        )
        checks_total += entry_total
        checks_passed += entry_passed

    entry_total, entry_passed = check_selector_entry(
        payload=probe_payload.get("startup_manual_only_before", {}),
        artifact=display_path(probe_exe),
        check_prefix="M255-D002-DYN-STARTUP-MANUAL-ONLY-BEFORE",
        expected_found=0,
        expected_metadata_backed=0,
        expected_stable_id=0,
        expected_provider_count=0,
        expected_first_ordinal=0,
        expected_last_ordinal=0,
        expected_first_pool_index=0,
        expected_last_pool_index=0,
        expected_selector=None,
        failures=failures,
    )
    checks_total += entry_total
    checks_passed += entry_passed

    checks_total += 1
    checks_passed += require(int(after_manual_registration.get("registered_image_count", -1)) == 2, display_path(probe_exe), "M255-D002-DYN-MANUAL-IMAGE-COUNT", "manual registration should bring image count to 2", failures)
    checks_total += 1
    checks_passed += require(after_manual_registration.get("last_registered_translation_unit_identity_key") == MANUAL_IDENTITY_KEY, display_path(probe_exe), "M255-D002-DYN-MANUAL-IDENTITY", "manual registration identity mismatch", failures)
    checks_total += 1
    checks_passed += require(int(after_manual_image_walk.get("last_walked_selector_pool_count", -1)) == 2, display_path(probe_exe), "M255-D002-DYN-MANUAL-POOL-COUNT", "manual selector-pool count mismatch", failures)
    checks_total += 1
    checks_passed += require(after_manual_image_walk.get("last_walked_translation_unit_identity_key") == MANUAL_IDENTITY_KEY, display_path(probe_exe), "M255-D002-DYN-MANUAL-WALK-ID", "manual walked identity mismatch", failures)
    checks_total += 1
    checks_passed += require(int(after_manual_table.get("selector_table_entry_count", -1)) == EXPECTED_MANUAL_SELECTOR_ID, display_path(probe_exe), "M255-D002-DYN-MANUAL-TABLE-COUNT", "unexpected selector count after manual registration", failures)
    checks_total += 1
    checks_passed += require(int(after_manual_table.get("metadata_backed_selector_count", -1)) == EXPECTED_MANUAL_SELECTOR_ID, display_path(probe_exe), "M255-D002-DYN-MANUAL-META-COUNT", "unexpected metadata-backed count after manual registration", failures)
    checks_total += 1
    checks_passed += require(int(after_manual_table.get("dynamic_selector_count", -1)) == 0, display_path(probe_exe), "M255-D002-DYN-MANUAL-DYNAMIC-COUNT", "manual registration should not create dynamic selectors yet", failures)
    checks_total += 1
    checks_passed += require(int(after_manual_table.get("metadata_provider_edge_count", -1)) == len(EXPECTED_EMITTED_SELECTORS) + 2, display_path(probe_exe), "M255-D002-DYN-MANUAL-PROVIDER-COUNT", "unexpected provider-edge count after manual registration", failures)
    checks_total += 1
    checks_passed += require(after_manual_table.get("last_materialized_selector") == "debugName", display_path(probe_exe), "M255-D002-DYN-MANUAL-LAST-SELECTOR", "manual registration should last materialize debugName", failures)
    checks_total += 1
    checks_passed += require(int(after_manual_table.get("last_materialized_stable_id", -1)) == EXPECTED_MANUAL_SELECTOR_ID, display_path(probe_exe), "M255-D002-DYN-MANUAL-LAST-ID", "unexpected manual debugName stable id", failures)
    checks_total += 1
    checks_passed += require(int(after_manual_table.get("last_materialized_registration_order_ordinal", -1)) == 2, display_path(probe_exe), "M255-D002-DYN-MANUAL-LAST-ORDINAL", "unexpected manual last registration ordinal", failures)
    checks_total += 1
    checks_passed += require(int(after_manual_table.get("last_materialized_selector_pool_index", -1)) == 2, display_path(probe_exe), "M255-D002-DYN-MANUAL-LAST-POOL", "unexpected manual last selector-pool index", failures)

    entry_total, entry_passed = check_selector_entry(
        payload=probe_payload.get("after_manual_token", {}),
        artifact=display_path(probe_exe),
        check_prefix="M255-D002-DYN-AFTER-MANUAL-TOKEN",
        expected_found=1,
        expected_metadata_backed=1,
        expected_stable_id=EXPECTED_EMITTED_SELECTOR_IDS["tokenValue"],
        expected_provider_count=2,
        expected_first_ordinal=1,
        expected_last_ordinal=2,
        expected_first_pool_index=4,
        expected_last_pool_index=1,
        expected_selector="tokenValue",
        failures=failures,
    )
    checks_total += entry_total
    checks_passed += entry_passed

    entry_total, entry_passed = check_selector_entry(
        payload=probe_payload.get("after_manual_debug_name", {}),
        artifact=display_path(probe_exe),
        check_prefix="M255-D002-DYN-AFTER-MANUAL-DEBUG-NAME",
        expected_found=1,
        expected_metadata_backed=1,
        expected_stable_id=EXPECTED_MANUAL_SELECTOR_ID,
        expected_provider_count=1,
        expected_first_ordinal=2,
        expected_last_ordinal=2,
        expected_first_pool_index=2,
        expected_last_pool_index=2,
        expected_selector="debugName",
        failures=failures,
    )
    checks_total += entry_total
    checks_passed += entry_passed

    checks_total += 1
    checks_passed += require(int(after_dynamic_table.get("selector_table_entry_count", -1)) == EXPECTED_DYNAMIC_SELECTOR_ID, display_path(probe_exe), "M255-D002-DYN-DYNAMIC-TABLE-COUNT", "unexpected selector count after dynamic lookup", failures)
    checks_total += 1
    checks_passed += require(int(after_dynamic_table.get("metadata_backed_selector_count", -1)) == EXPECTED_MANUAL_SELECTOR_ID, display_path(probe_exe), "M255-D002-DYN-DYNAMIC-META-COUNT", "metadata-backed count changed after dynamic lookup", failures)
    checks_total += 1
    checks_passed += require(int(after_dynamic_table.get("dynamic_selector_count", -1)) == 1, display_path(probe_exe), "M255-D002-DYN-DYNAMIC-COUNT", "dynamic lookup should add one dynamic selector", failures)
    checks_total += 1
    checks_passed += require(after_dynamic_table.get("last_materialized_selector") == "manualOnly:", display_path(probe_exe), "M255-D002-DYN-DYNAMIC-LAST-SELECTOR", "unexpected dynamic last selector", failures)
    checks_total += 1
    checks_passed += require(int(probe_payload.get("dynamic_handle_stable_id", -1)) == EXPECTED_DYNAMIC_SELECTOR_ID, display_path(probe_exe), "M255-D002-DYN-DYNAMIC-HANDLE-ID", "unexpected dynamic selector handle id", failures)
    entry_total, entry_passed = check_selector_entry(
        payload=probe_payload.get("after_dynamic_manual_only", {}),
        artifact=display_path(probe_exe),
        check_prefix="M255-D002-DYN-AFTER-DYNAMIC-MANUAL-ONLY",
        expected_found=1,
        expected_metadata_backed=0,
        expected_stable_id=EXPECTED_DYNAMIC_SELECTOR_ID,
        expected_provider_count=0,
        expected_first_ordinal=0,
        expected_last_ordinal=0,
        expected_first_pool_index=0,
        expected_last_pool_index=0,
        expected_selector="manualOnly:",
        failures=failures,
    )
    checks_total += entry_total
    checks_passed += entry_passed

    checks_total += 1
    checks_passed += require(int(after_reset_registration.get("registered_image_count", -1)) == 0, display_path(probe_exe), "M255-D002-DYN-RESET-IMAGE-COUNT", "reset must clear registered images", failures)
    checks_total += 1
    checks_passed += require(int(after_reset_table.get("selector_table_entry_count", -1)) == 0, display_path(probe_exe), "M255-D002-DYN-RESET-TABLE-COUNT", "reset must clear selector table", failures)
    checks_total += 1
    checks_passed += require(int(after_reset_table.get("metadata_backed_selector_count", -1)) == 0, display_path(probe_exe), "M255-D002-DYN-RESET-META-COUNT", "reset must clear metadata-backed selector count", failures)
    checks_total += 1
    checks_passed += require(int(after_reset_table.get("dynamic_selector_count", -1)) == 0, display_path(probe_exe), "M255-D002-DYN-RESET-DYNAMIC-COUNT", "reset must clear dynamic selector count", failures)

    checks_total += 1
    checks_passed += require(int(after_replay_registration.get("registered_image_count", -1)) == 2, display_path(probe_exe), "M255-D002-DYN-REPLAY-IMAGE-COUNT", "replay must restore both retained images", failures)
    checks_total += 1
    checks_passed += require(after_replay_registration.get("last_registered_translation_unit_identity_key") == MANUAL_IDENTITY_KEY, display_path(probe_exe), "M255-D002-DYN-REPLAY-LAST-IDENTITY", "replay should finish with the manual image", failures)
    checks_total += 1
    checks_passed += require(int(after_replay_image_walk.get("last_walked_selector_pool_count", -1)) == 2, display_path(probe_exe), "M255-D002-DYN-REPLAY-POOL-COUNT", "replay walked selector-pool count mismatch", failures)
    checks_total += 1
    checks_passed += require(int(after_replay_reset_replay.get("retained_bootstrap_image_count", -1)) == 2, display_path(probe_exe), "M255-D002-DYN-REPLAY-RETAINED", "retained bootstrap image count mismatch", failures)
    checks_total += 1
    checks_passed += require(int(after_replay_reset_replay.get("last_replayed_image_count", -1)) == 2, display_path(probe_exe), "M255-D002-DYN-REPLAY-COUNT", "replayed image count mismatch", failures)
    checks_total += 1
    checks_passed += require(after_replay_reset_replay.get("last_replayed_translation_unit_identity_key") == MANUAL_IDENTITY_KEY, display_path(probe_exe), "M255-D002-DYN-REPLAY-LAST-REPLAYED", "last replayed identity mismatch", failures)
    checks_total += 1
    checks_passed += require(int(after_replay_table.get("selector_table_entry_count", -1)) == EXPECTED_MANUAL_SELECTOR_ID, display_path(probe_exe), "M255-D002-DYN-REPLAY-TABLE-COUNT", "replay must rebuild metadata-backed selector table", failures)
    checks_total += 1
    checks_passed += require(int(after_replay_table.get("metadata_backed_selector_count", -1)) == EXPECTED_MANUAL_SELECTOR_ID, display_path(probe_exe), "M255-D002-DYN-REPLAY-META-COUNT", "unexpected replay metadata-backed selector count", failures)
    checks_total += 1
    checks_passed += require(int(after_replay_table.get("dynamic_selector_count", -1)) == 0, display_path(probe_exe), "M255-D002-DYN-REPLAY-DYNAMIC-COUNT", "replay should not restore dynamic selectors before lookup", failures)
    checks_total += 1
    checks_passed += require(int(after_replay_table.get("metadata_provider_edge_count", -1)) == len(EXPECTED_EMITTED_SELECTORS) + 2, display_path(probe_exe), "M255-D002-DYN-REPLAY-PROVIDER-COUNT", "unexpected replay provider-edge count", failures)
    checks_total += 1
    checks_passed += require(after_replay_table.get("last_materialized_selector") == "debugName", display_path(probe_exe), "M255-D002-DYN-REPLAY-LAST-SELECTOR", "unexpected replay last materialized selector", failures)
    checks_total += 1
    checks_passed += require(int(after_replay_table.get("last_materialized_stable_id", -1)) == EXPECTED_MANUAL_SELECTOR_ID, display_path(probe_exe), "M255-D002-DYN-REPLAY-LAST-ID", "unexpected replay last materialized stable id", failures)

    entry_total, entry_passed = check_selector_entry(
        payload=probe_payload.get("after_replay_token", {}),
        artifact=display_path(probe_exe),
        check_prefix="M255-D002-DYN-AFTER-REPLAY-TOKEN",
        expected_found=1,
        expected_metadata_backed=1,
        expected_stable_id=EXPECTED_EMITTED_SELECTOR_IDS["tokenValue"],
        expected_provider_count=2,
        expected_first_ordinal=1,
        expected_last_ordinal=2,
        expected_first_pool_index=4,
        expected_last_pool_index=1,
        expected_selector="tokenValue",
        failures=failures,
    )
    checks_total += entry_total
    checks_passed += entry_passed

    entry_total, entry_passed = check_selector_entry(
        payload=probe_payload.get("after_replay_debug_name", {}),
        artifact=display_path(probe_exe),
        check_prefix="M255-D002-DYN-AFTER-REPLAY-DEBUG-NAME",
        expected_found=1,
        expected_metadata_backed=1,
        expected_stable_id=EXPECTED_MANUAL_SELECTOR_ID,
        expected_provider_count=1,
        expected_first_ordinal=2,
        expected_last_ordinal=2,
        expected_first_pool_index=2,
        expected_last_pool_index=2,
        expected_selector="debugName",
        failures=failures,
    )
    checks_total += entry_total
    checks_passed += entry_passed

    entry_total, entry_passed = check_selector_entry(
        payload=probe_payload.get("after_replay_manual_only_before", {}),
        artifact=display_path(probe_exe),
        check_prefix="M255-D002-DYN-AFTER-REPLAY-MANUAL-ONLY-BEFORE",
        expected_found=0,
        expected_metadata_backed=0,
        expected_stable_id=0,
        expected_provider_count=0,
        expected_first_ordinal=0,
        expected_last_ordinal=0,
        expected_first_pool_index=0,
        expected_last_pool_index=0,
        expected_selector=None,
        failures=failures,
    )
    checks_total += entry_total
    checks_passed += entry_passed

    checks_total += 1
    checks_passed += require(int(after_replay_dynamic_table.get("selector_table_entry_count", -1)) == EXPECTED_DYNAMIC_SELECTOR_ID, display_path(probe_exe), "M255-D002-DYN-REPLAY-DYNAMIC-TABLE-COUNT", "unexpected selector count after replay dynamic lookup", failures)
    checks_total += 1
    checks_passed += require(int(after_replay_dynamic_table.get("metadata_backed_selector_count", -1)) == EXPECTED_MANUAL_SELECTOR_ID, display_path(probe_exe), "M255-D002-DYN-REPLAY-DYNAMIC-META-COUNT", "unexpected metadata-backed count after replay dynamic lookup", failures)
    checks_total += 1
    checks_passed += require(int(after_replay_dynamic_table.get("dynamic_selector_count", -1)) == 1, display_path(probe_exe), "M255-D002-DYN-REPLAY-DYNAMIC-COUNT", "replay dynamic lookup should add one dynamic selector", failures)
    checks_total += 1
    checks_passed += require(int(probe_payload.get("replayed_dynamic_handle_stable_id", -1)) == EXPECTED_DYNAMIC_SELECTOR_ID, display_path(probe_exe), "M255-D002-DYN-REPLAY-DYNAMIC-HANDLE-ID", "unexpected replay dynamic selector handle id", failures)
    entry_total, entry_passed = check_selector_entry(
        payload=probe_payload.get("after_replay_dynamic_manual_only", {}),
        artifact=display_path(probe_exe),
        check_prefix="M255-D002-DYN-AFTER-REPLAY-DYNAMIC-MANUAL-ONLY",
        expected_found=1,
        expected_metadata_backed=0,
        expected_stable_id=EXPECTED_DYNAMIC_SELECTOR_ID,
        expected_provider_count=0,
        expected_first_ordinal=0,
        expected_last_ordinal=0,
        expected_first_pool_index=0,
        expected_last_pool_index=0,
        expected_selector="manualOnly:",
        failures=failures,
    )
    checks_total += entry_total
    checks_passed += entry_passed

    return checks_total, checks_passed, case


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.architecture_doc, ARCHITECTURE_SNIPPETS),
        (args.runtime_readme, RUNTIME_README_SNIPPETS),
        (args.tooling_runtime_readme, TOOLING_RUNTIME_README_SNIPPETS),
        (args.lowering_header, LOWERING_HEADER_SNIPPETS),
        (args.ir_emitter, IR_EMITTER_SNIPPETS),
        (args.parser_cpp, PARSER_SNIPPETS),
        (args.runtime_source, RUNTIME_SOURCE_SNIPPETS),
        (args.bootstrap_header, BOOTSTRAP_HEADER_SNIPPETS),
        (args.shim_source, SHIM_SNIPPETS),
        (args.runtime_probe, PROBE_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    ):
        checks_total += 1 + len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    summary: dict[str, object] = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "lookup_dispatch_contract_id": LOOKUP_DISPATCH_CONTRACT_ID,
        "bootstrap_registrar_contract_id": BOOTSTRAP_REGISTRAR_CONTRACT_ID,
        "bootstrap_reset_contract_id": BOOTSTRAP_RESET_CONTRACT_ID,
        "ok": False,
        "checks_total": 0,
        "checks_passed": 0,
        "findings": [],
        "models": {
            "selector_interning_model": SELECTOR_INTERNING_MODEL,
            "selector_merge_model": SELECTOR_MERGE_MODEL,
            "selector_fallback_model": SELECTOR_FALLBACK_MODEL,
            "selector_replay_model": SELECTOR_REPLAY_MODEL,
        },
    }

    if not args.skip_dynamic_probes:
        clangxx = resolve_tool(args.clangxx)
        checks_total += 1
        checks_passed += require(clangxx is not None, display_path(args.runtime_probe), "M255-D002-TOOL-CLANGXX", "clang++ is required", failures)
        checks_total += 1
        checks_passed += require(resolve_tool("llc.exe") is not None, display_path(args.native_exe), "M255-D002-TOOL-LLC", "llc.exe is required", failures)
        if clangxx is not None:
            dynamic_total, dynamic_passed, dynamic_case = run_dynamic_probe(
                args=args,
                clangxx=str(clangxx),
                failures=failures,
            )
            checks_total += dynamic_total
            checks_passed += dynamic_passed
            summary["dynamic_case"] = dynamic_case
    else:
        summary["dynamic_case"] = {"skipped": True}

    summary["checks_total"] = checks_total
    summary["checks_passed"] = checks_passed
    summary["ok"] = not failures
    summary["findings"] = [finding.__dict__ for finding in failures]

    summary_out = (ROOT / args.summary_out).resolve()
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(canonical_json(summary), encoding="utf-8")
    print(canonical_json(summary), end="")
    return 0 if not failures else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
