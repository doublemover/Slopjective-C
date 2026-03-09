#!/usr/bin/env python3
"""Fail-closed checker for M263-D002 live registration, replay, and discovery."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m263-d002-live-registration-replay-and-discovery-implementation-v1"
CONTRACT_ID = "objc3c-runtime-live-registration-discovery-replay/m263-d002-v1"
UPSTREAM_TABLE_CONTRACT_ID = (
    "objc3c-runtime-bootstrap-table-consumption-freeze/m263-d001-v1"
)
UPSTREAM_RESET_REPLAY_CONTRACT_ID = (
    "objc3c-runtime-bootstrap-reset-replay/m254-d003-v1"
)
LIVE_REGISTRATION_MODEL = (
    "emitted-metadata-images-register-through-native-runtime-and-retained-bootstrap-catalog"
)
LIVE_DISCOVERY_TRACKING_MODEL = (
    "image-walk-snapshot-tracks-last-discovered-root-and-descriptor-families"
)
LIVE_REPLAY_TRACKING_MODEL = (
    "reset-replay-state-snapshot-tracks-retained-images-reset-clears-and-last-replayed-identity"
)
INTERNAL_HEADER_PATH = "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h"
RUNTIME_SOURCE_PATH = "native/objc3c/src/runtime/objc3_runtime.cpp"
PROCESS_CPP_PATH = "native/objc3c/src/io/objc3_process.cpp"
IMAGE_WALK_SNAPSHOT_SYMBOL = "objc3_runtime_copy_image_walk_state_for_testing"
RESET_REPLAY_SNAPSHOT_SYMBOL = "objc3_runtime_copy_reset_replay_state_for_testing"
REPLAY_REGISTERED_IMAGES_SYMBOL = "objc3_runtime_replay_registered_images_for_testing"
REGISTRATION_MANIFEST_ARTIFACT = "module.runtime-registration-manifest.json"
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m263_live_registration_replay_and_discovery_implementation_d002_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m263" / "m263_d002_live_registration_replay_and_discovery_implementation_packet.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_INTERNAL_HEADER = ROOT / INTERNAL_HEADER_PATH
DEFAULT_RUNTIME_SOURCE = ROOT / RUNTIME_SOURCE_PATH
DEFAULT_PROCESS_CPP = ROOT / PROCESS_CPP_PATH
DEFAULT_RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m263_d002_live_registration_replay_tracking_probe.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
DEFAULT_RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
DEFAULT_PRIMARY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m254_c002_runtime_bootstrap_metadata_library.objc3"
DEFAULT_CATEGORY_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m254_c003_runtime_bootstrap_category_library.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m263" / "d002-live-registration-discovery-replay"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m263/M263-D002/live_registration_replay_and_discovery_summary.json"
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


@dataclass(frozen=True)
class DynamicCase:
    case_id: str
    fixture: Path


DYNAMIC_CASES = (
    DynamicCase("metadata-library", DEFAULT_PRIMARY_FIXTURE),
    DynamicCase("category-library", DEFAULT_CATEGORY_FIXTURE),
)

EXPECTATIONS_SNIPPETS = (
    SnippetCheck(
        "M263-D002-DOC-EXP-01",
        "# M263 Live Registration Replay And Discovery Implementation Expectations (D002)",
    ),
    SnippetCheck("M263-D002-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M263-D002-DOC-EXP-03", f"`{INTERNAL_HEADER_PATH}`"),
    SnippetCheck("M263-D002-DOC-EXP-04", f"`{RUNTIME_SOURCE_PATH}`"),
    SnippetCheck("M263-D002-DOC-EXP-05", f"`{PROCESS_CPP_PATH}`"),
)
PACKET_SNIPPETS = (
    SnippetCheck(
        "M263-D002-DOC-PKT-01",
        "# M263-D002 Live Registration, Replay, And Discovery Implementation Packet",
    ),
    SnippetCheck("M263-D002-DOC-PKT-02", "Packet: `M263-D002`"),
    SnippetCheck("M263-D002-DOC-PKT-03", f"`{CONTRACT_ID}`"),
    SnippetCheck(
        "M263-D002-DOC-PKT-04",
        "replay restores live registration state and republishes discovery-root evidence",
    ),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck(
        "M263-D002-NDOC-01",
        "## Live registration, discovery, and replay tracking (M263-D002)",
    ),
    SnippetCheck("M263-D002-NDOC-02", f"`{LIVE_DISCOVERY_TRACKING_MODEL}`"),
    SnippetCheck("M263-D002-NDOC-03", f"`{REPLAY_REGISTERED_IMAGES_SYMBOL}`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck(
        "M263-D002-SPC-01",
        "## M263 live registration, discovery, and replay implementation (D002)",
    ),
    SnippetCheck("M263-D002-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M263-D002-SPC-03", f"`{LIVE_REPLAY_TRACKING_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck(
        "M263-D002-META-01",
        "## M263 live registration, discovery, and replay anchors (D002)",
    ),
    SnippetCheck(
        "M263-D002-META-02",
        "`bootstrap_live_registration_contract_id`",
    ),
    SnippetCheck(
        "M263-D002-META-03",
        "`last_replayed_translation_unit_identity_key`",
    ),
)
INTERNAL_HEADER_SNIPPETS = (
    SnippetCheck(
        "M263-D002-INTH-01",
        "M263-D002 live-registration-discovery-replay anchor",
    ),
    SnippetCheck("M263-D002-INTH-02", REPLAY_REGISTERED_IMAGES_SYMBOL),
    SnippetCheck("M263-D002-INTH-03", RESET_REPLAY_SNAPSHOT_SYMBOL),
)
RUNTIME_SOURCE_SNIPPETS = (
    SnippetCheck(
        "M263-D002-RTS-01",
        "M263-D002 live-registration-discovery-replay anchor",
    ),
    SnippetCheck("M263-D002-RTS-02", "state.last_discovery_root_entry_count"),
    SnippetCheck(
        "M263-D002-RTS-03",
        "state.last_replayed_translation_unit_identity_key",
    ),
    SnippetCheck("M263-D002-RTS-04", "state.replay_generation"),
)
PROCESS_CPP_SNIPPETS = (
    SnippetCheck(
        "M263-D002-PROC-01",
        "kObjc3RuntimeLiveRegistrationDiscoveryReplayContractId",
    ),
    SnippetCheck(
        "M263-D002-PROC-02",
        "bootstrap_live_registration_contract_id",
    ),
    SnippetCheck(
        "M263-D002-PROC-03",
        "ready_for_live_registration_discovery_replay",
    ),
)
PROBE_SNIPPETS = (
    SnippetCheck(
        "M263-D002-PRB-01",
        '#include "runtime/objc3_runtime_bootstrap_internal.h"',
    ),
    SnippetCheck("M263-D002-PRB-02", "post_replay_last_replayed_image_count"),
    SnippetCheck(
        "M263-D002-PRB-03",
        "post_reset_last_reset_cleared_image_local_init_state_count",
    ),
)
PACKAGE_SNIPPETS = (
    SnippetCheck(
        "M263-D002-PKG-01",
        '"check:objc3c:m263-d002-live-registration-discovery-replay": "python scripts/check_m263_d002_live_registration_replay_and_discovery_implementation.py"',
    ),
    SnippetCheck(
        "M263-D002-PKG-02",
        '"test:tooling:m263-d002-live-registration-discovery-replay": "python -m pytest tests/tooling/test_check_m263_d002_live_registration_replay_and_discovery_implementation.py -q"',
    ),
    SnippetCheck(
        "M263-D002-PKG-03",
        '"check:objc3c:m263-d002-lane-d-readiness": "python scripts/run_m263_d002_lane_d_readiness.py"',
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
    parser.add_argument("--native-doc", type=Path, default=DEFAULT_NATIVE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--internal-header", type=Path, default=DEFAULT_INTERNAL_HEADER)
    parser.add_argument("--runtime-source", type=Path, default=DEFAULT_RUNTIME_SOURCE)
    parser.add_argument("--process-cpp", type=Path, default=DEFAULT_PROCESS_CPP)
    parser.add_argument("--runtime-probe", type=Path, default=DEFAULT_RUNTIME_PROBE)
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
        failures.append(
            Finding(
                display_path(path),
                "M263-D002-MISSING",
                f"required artifact is missing: {display_path(path)}",
            )
        )
        return len(snippets)
    text = read_text(path)
    checks_passed = 0
    for snippet in snippets:
        if snippet.snippet in text:
            checks_passed += 1
        else:
            failures.append(
                Finding(
                    display_path(path),
                    snippet.check_id,
                    f"missing snippet: {snippet.snippet}",
                )
            )
    return checks_passed


def require(
    condition: bool,
    artifact: str,
    check_id: str,
    detail: str,
    failures: list[Finding],
) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def load_json(path: Path) -> dict[str, Any]:
    return json.loads(read_text(path))


def resolve_tool(name: str) -> Path | None:
    which = shutil.which(name)
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


def run_dynamic_case(
    *,
    args: argparse.Namespace,
    case_input: DynamicCase,
    clangxx: str,
    failures: list[Finding],
) -> tuple[int, int, dict[str, object]]:
    checks_total = 0
    checks_passed = 0
    case: dict[str, object] = {
        "case_id": case_input.case_id,
        "fixture": display_path(case_input.fixture),
    }

    runtime_include_root = args.runtime_include_root.resolve()
    runtime_probe = args.runtime_probe.resolve()
    runtime_library = args.runtime_library.resolve()
    out_dir = args.probe_root.resolve() / case_input.case_id
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [
        str(args.native_exe.resolve()),
        str(case_input.fixture.resolve()),
        "--out-dir",
        str(out_dir),
        "--emit-prefix",
        "module",
    ]
    llc = resolve_tool("llc.exe")
    if llc is not None:
        command.extend(["--llc", str(llc)])
    compile_result = run_command(command)

    module_manifest_path = out_dir / "module.manifest.json"
    registration_manifest_path = out_dir / REGISTRATION_MANIFEST_ARTIFACT
    obj_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"

    checks_total += 1
    checks_passed += require(
        compile_result.returncode == 0,
        display_path(case_input.fixture),
        f"M263-D002-{case_input.case_id}-COMPILE",
        f"native compile exited with {compile_result.returncode}",
        failures,
    )
    for artifact_path, suffix in (
        (module_manifest_path, "MANIFEST"),
        (registration_manifest_path, "REG-MANIFEST"),
        (obj_path, "OBJ"),
    ):
        checks_total += 1
        checks_passed += require(
            artifact_path.exists(),
            display_path(artifact_path),
            f"M263-D002-{case_input.case_id}-{suffix}",
            f"required artifact is missing: {display_path(artifact_path)}",
            failures,
        )
    if any(
        f.check_id.startswith(f"M263-D002-{case_input.case_id}-") for f in failures
    ):
        case["compile_stdout"] = compile_result.stdout
        case["compile_stderr"] = compile_result.stderr
        return checks_total, checks_passed, case

    module_manifest_payload = load_json(module_manifest_path)
    registration_manifest_payload = load_json(registration_manifest_path)
    backend_text = read_text(backend_path).strip() if backend_path.exists() else ""

    checks_total += 1
    checks_passed += require(
        registration_manifest_payload.get("bootstrap_live_registration_contract_id")
        == CONTRACT_ID,
        display_path(registration_manifest_path),
        f"M263-D002-{case_input.case_id}-REG-CONTRACT",
        "registration manifest live registration contract mismatch",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        registration_manifest_payload.get("bootstrap_live_registration_model")
        == LIVE_REGISTRATION_MODEL,
        display_path(registration_manifest_path),
        f"M263-D002-{case_input.case_id}-REG-LIVE",
        "registration manifest live registration model mismatch",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        registration_manifest_payload.get("bootstrap_live_discovery_tracking_model")
        == LIVE_DISCOVERY_TRACKING_MODEL,
        display_path(registration_manifest_path),
        f"M263-D002-{case_input.case_id}-REG-DISCOVERY",
        "registration manifest discovery tracking model mismatch",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        registration_manifest_payload.get("bootstrap_live_replay_tracking_model")
        == LIVE_REPLAY_TRACKING_MODEL,
        display_path(registration_manifest_path),
        f"M263-D002-{case_input.case_id}-REG-REPLAY",
        "registration manifest replay tracking model mismatch",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        registration_manifest_payload.get(
            "bootstrap_live_replay_registered_images_symbol"
        )
        == REPLAY_REGISTERED_IMAGES_SYMBOL,
        display_path(registration_manifest_path),
        f"M263-D002-{case_input.case_id}-REG-REPLAY-SYM",
        "registration manifest replay symbol mismatch",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        registration_manifest_payload.get(
            "bootstrap_live_reset_replay_state_snapshot_symbol"
        )
        == RESET_REPLAY_SNAPSHOT_SYMBOL,
        display_path(registration_manifest_path),
        f"M263-D002-{case_input.case_id}-REG-RESET-SYM",
        "registration manifest reset/replay snapshot symbol mismatch",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        registration_manifest_payload.get("bootstrap_table_image_walk_snapshot_symbol")
        == IMAGE_WALK_SNAPSHOT_SYMBOL,
        display_path(registration_manifest_path),
        f"M263-D002-{case_input.case_id}-REG-WALK-SYM",
        "registration manifest image-walk snapshot symbol mismatch",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        registration_manifest_payload.get("ready_for_live_registration_discovery_replay")
        is True,
        display_path(registration_manifest_path),
        f"M263-D002-{case_input.case_id}-REG-READY",
        "registration manifest live registration readiness flag must be true",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        registration_manifest_payload.get("bootstrap_table_consumption_contract_id")
        == UPSTREAM_TABLE_CONTRACT_ID,
        display_path(registration_manifest_path),
        f"M263-D002-{case_input.case_id}-REG-UPSTREAM-TABLE",
        "registration manifest upstream table-consumption contract mismatch",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        registration_manifest_payload.get("bootstrap_reset_contract_id")
        == UPSTREAM_RESET_REPLAY_CONTRACT_ID,
        display_path(registration_manifest_path),
        f"M263-D002-{case_input.case_id}-REG-UPSTREAM-REPLAY",
        "registration manifest upstream replay contract mismatch",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        backend_text == "llvm-direct",
        display_path(backend_path),
        f"M263-D002-{case_input.case_id}-BACKEND",
        "object backend must remain llvm-direct",
        failures,
    )
    if any(
        f.check_id.startswith(f"M263-D002-{case_input.case_id}-") for f in failures
    ):
        return checks_total, checks_passed, case

    probe_out_dir = out_dir / "probe"
    probe_out_dir.mkdir(parents=True, exist_ok=True)
    probe_exe = probe_out_dir / (
        f"{case_input.case_id}-live-registration-replay-tracking-probe.exe"
    )
    probe_compile_command = [
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
    probe_compile = run_command(probe_compile_command)
    checks_total += 1
    checks_passed += require(
        probe_compile.returncode == 0,
        display_path(runtime_probe),
        f"M263-D002-{case_input.case_id}-PROBE-COMPILE",
        f"probe compile exited with {probe_compile.returncode}",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        probe_exe.exists(),
        display_path(probe_exe),
        f"M263-D002-{case_input.case_id}-PROBE-EXE",
        "probe executable is missing",
        failures,
    )
    if any(
        f.check_id.startswith(f"M263-D002-{case_input.case_id}-") for f in failures
    ):
        case["probe_compile_stdout"] = probe_compile.stdout
        case["probe_compile_stderr"] = probe_compile.stderr
        return checks_total, checks_passed, case

    probe_run = run_command([str(probe_exe)])
    checks_total += 1
    checks_passed += require(
        probe_run.returncode == 0,
        display_path(probe_exe),
        f"M263-D002-{case_input.case_id}-PROBE-RUN",
        f"probe exited with {probe_run.returncode}",
        failures,
    )
    if probe_run.returncode != 0:
        case["probe_run_stdout"] = probe_run.stdout
        case["probe_run_stderr"] = probe_run.stderr
        return checks_total, checks_passed, case

    try:
        probe_payload = json.loads(probe_run.stdout)
    except json.JSONDecodeError as exc:
        failures.append(
            Finding(
                display_path(probe_exe),
                f"M263-D002-{case_input.case_id}-PROBE-JSON",
                f"invalid probe JSON: {exc}",
            )
        )
        return checks_total + 1, checks_passed, case

    expected_identity_key = str(
        registration_manifest_payload.get("translation_unit_identity_key", "")
    )
    expected_module_name = str(module_manifest_payload.get("module", ""))
    expected_ordinal = int(
        registration_manifest_payload.get("translation_unit_registration_order_ordinal", 0)
    )

    def add(condition: bool, suffix: str, detail: str) -> None:
        nonlocal checks_total, checks_passed
        checks_total += 1
        checks_passed += require(
            condition,
            display_path(probe_exe),
            f"M263-D002-{case_input.case_id}-{suffix}",
            detail,
            failures,
        )

    add(
        probe_payload.get("startup_registration_copy_status") == 0,
        "STARTUP-REG-COPY",
        "startup registration snapshot copy must succeed",
    )
    add(
        probe_payload.get("startup_image_walk_copy_status") == 0,
        "STARTUP-WALK-COPY",
        "startup image-walk snapshot copy must succeed",
    )
    add(
        probe_payload.get("startup_reset_replay_copy_status") == 0,
        "STARTUP-RESET-COPY",
        "startup reset/replay snapshot copy must succeed",
    )
    add(
        probe_payload.get("startup_registered_image_count") == 1,
        "STARTUP-COUNT",
        "startup registered image count must be 1",
    )
    add(
        probe_payload.get("startup_next_expected_registration_order_ordinal")
        == expected_ordinal + 1,
        "STARTUP-NEXT",
        "startup next expected ordinal mismatch",
    )
    add(
        probe_payload.get("startup_walked_image_count") == 1,
        "STARTUP-WALKED",
        "startup walked image count must be 1",
    )
    add(
        int(probe_payload.get("startup_last_discovery_root_entry_count", 0)) >= 6,
        "STARTUP-DISCOVERY",
        "startup discovery-root entry count must be >= 6",
    )
    add(
        probe_payload.get("startup_last_registration_used_staged_table") == 1,
        "STARTUP-STAGED",
        "startup registration must consume the staged table",
    )
    add(
        probe_payload.get("startup_retained_bootstrap_image_count") == 1,
        "STARTUP-RETAINED",
        "startup retained bootstrap image count must be 1",
    )
    add(
        probe_payload.get("startup_last_registered_module_name") == expected_module_name,
        "STARTUP-MODULE",
        "startup registered module name mismatch",
    )
    add(
        probe_payload.get("startup_last_registered_translation_unit_identity_key")
        == expected_identity_key,
        "STARTUP-IDENTITY",
        "startup registered identity key mismatch",
    )
    add(
        probe_payload.get("post_reset_registration_copy_status") == 0,
        "POST-RESET-REG-COPY",
        "post-reset registration snapshot copy must succeed",
    )
    add(
        probe_payload.get("post_reset_reset_replay_copy_status") == 0,
        "POST-RESET-SNAPSHOT-COPY",
        "post-reset reset/replay snapshot copy must succeed",
    )
    add(
        probe_payload.get("post_reset_registered_image_count") == 0,
        "POST-RESET-COUNT",
        "post-reset registered image count must be 0",
    )
    add(
        probe_payload.get("post_reset_next_expected_registration_order_ordinal") == 1,
        "POST-RESET-NEXT",
        "post-reset next expected ordinal must reset to 1",
    )
    add(
        probe_payload.get("post_reset_retained_bootstrap_image_count") == 1,
        "POST-RESET-RETAINED",
        "post-reset retained bootstrap image count must remain 1",
    )
    add(
        int(
            probe_payload.get(
                "post_reset_last_reset_cleared_image_local_init_state_count", 0
            )
        )
        >= 1,
        "POST-RESET-CLEARED",
        "post-reset cleared init-state count must be >= 1",
    )
    add(
        probe_payload.get("replay_status") == 0,
        "REPLAY-STATUS",
        "replay must succeed",
    )
    add(
        probe_payload.get("post_replay_registration_copy_status") == 0,
        "POST-REPLAY-REG-COPY",
        "post-replay registration snapshot copy must succeed",
    )
    add(
        probe_payload.get("post_replay_image_walk_copy_status") == 0,
        "POST-REPLAY-WALK-COPY",
        "post-replay image-walk snapshot copy must succeed",
    )
    add(
        probe_payload.get("post_replay_reset_replay_copy_status") == 0,
        "POST-REPLAY-SNAPSHOT-COPY",
        "post-replay reset/replay snapshot copy must succeed",
    )
    add(
        probe_payload.get("post_replay_registered_image_count") == 1,
        "POST-REPLAY-COUNT",
        "post-replay registered image count must be 1",
    )
    add(
        probe_payload.get("post_replay_next_expected_registration_order_ordinal")
        == expected_ordinal + 1,
        "POST-REPLAY-NEXT",
        "post-replay next expected ordinal mismatch",
    )
    add(
        probe_payload.get("post_replay_walked_image_count") == 1,
        "POST-REPLAY-WALKED",
        "post-replay walked image count must be 1",
    )
    add(
        int(probe_payload.get("post_replay_last_discovery_root_entry_count", 0)) >= 6,
        "POST-REPLAY-DISCOVERY",
        "post-replay discovery-root entry count must be >= 6",
    )
    add(
        probe_payload.get("post_replay_last_registration_used_staged_table") == 1,
        "POST-REPLAY-STAGED",
        "post-replay registration must republish staged-table-backed discovery",
    )
    add(
        probe_payload.get("post_replay_retained_bootstrap_image_count") == 1,
        "POST-REPLAY-RETAINED",
        "post-replay retained bootstrap image count must remain 1",
    )
    add(
        probe_payload.get("post_replay_last_replayed_image_count") == 1,
        "POST-REPLAY-LAST-COUNT",
        "post-replay last replayed image count must be 1",
    )
    add(
        int(probe_payload.get("post_replay_replay_generation", 0)) >= 1,
        "POST-REPLAY-GENERATION",
        "post-replay replay generation must advance",
    )
    add(
        probe_payload.get("post_replay_last_replay_status") == 0,
        "POST-REPLAY-LAST-STATUS",
        "post-replay last replay status must be success",
    )
    add(
        probe_payload.get("post_replay_last_registered_module_name")
        == expected_module_name,
        "POST-REPLAY-REG-MODULE",
        "post-replay registered module name mismatch",
    )
    add(
        probe_payload.get("post_replay_last_walked_module_name") == expected_module_name,
        "POST-REPLAY-WALK-MODULE",
        "post-replay walked module name mismatch",
    )
    add(
        probe_payload.get("post_replay_last_replayed_module_name")
        == expected_module_name,
        "POST-REPLAY-REPLAY-MODULE",
        "post-replay replayed module name mismatch",
    )
    add(
        probe_payload.get("post_replay_last_registered_translation_unit_identity_key")
        == expected_identity_key,
        "POST-REPLAY-REG-IDENTITY",
        "post-replay registered identity key mismatch",
    )
    add(
        probe_payload.get("post_replay_last_walked_translation_unit_identity_key")
        == expected_identity_key,
        "POST-REPLAY-WALK-IDENTITY",
        "post-replay walked identity key mismatch",
    )
    add(
        probe_payload.get("post_replay_last_replayed_translation_unit_identity_key")
        == expected_identity_key,
        "POST-REPLAY-REPLAY-IDENTITY",
        "post-replay replayed identity key mismatch",
    )

    case.update(
        {
            "module_manifest": display_path(module_manifest_path),
            "registration_manifest": display_path(registration_manifest_path),
            "object_file": display_path(obj_path),
            "probe_exe": display_path(probe_exe),
            "runtime_library": display_path(runtime_library),
            "expected_translation_unit_identity_key": expected_identity_key,
            "expected_module_name": expected_module_name,
            "expected_registration_order_ordinal": expected_ordinal,
            "probe_payload": probe_payload,
        }
    )
    return checks_total, checks_passed, case


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for path, snippets in (
        (args.expectations_doc, EXPECTATIONS_SNIPPETS),
        (args.packet_doc, PACKET_SNIPPETS),
        (args.native_doc, NATIVE_DOC_SNIPPETS),
        (args.lowering_spec, LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, METADATA_SPEC_SNIPPETS),
        (args.internal_header, INTERNAL_HEADER_SNIPPETS),
        (args.runtime_source, RUNTIME_SOURCE_SNIPPETS),
        (args.process_cpp, PROCESS_CPP_SNIPPETS),
        (args.runtime_probe, PROBE_SNIPPETS),
        (args.package_json, PACKAGE_SNIPPETS),
    ):
        checks_total += len(snippets)
        checks_passed += ensure_snippets(path, snippets, failures)

    clangxx = shutil.which(args.clangxx)
    checks_total += 1
    checks_passed += require(
        clangxx is not None,
        args.clangxx,
        "M263-D002-TOOL-CLANGXX",
        f"required C++ compiler is not on PATH: {args.clangxx}",
        failures,
    )

    llc = resolve_tool("llc.exe")
    checks_total += 1
    checks_passed += require(
        llc is not None,
        "llc.exe",
        "M263-D002-TOOL-LLC",
        "llc.exe is required on PATH for dynamic probes",
        failures,
    )

    dynamic_cases: list[dict[str, object]] = []
    if not args.skip_dynamic_probes and clangxx is not None and llc is not None:
        for case_input in DYNAMIC_CASES:
            case_checks_total, case_checks_passed, case_payload = run_dynamic_case(
                args=args,
                case_input=case_input,
                clangxx=clangxx,
                failures=failures,
            )
            checks_total += case_checks_total
            checks_passed += case_checks_passed
            dynamic_cases.append(case_payload)

    summary = {
        "mode": MODE,
        "contract_id": CONTRACT_ID,
        "upstream_table_contract_id": UPSTREAM_TABLE_CONTRACT_ID,
        "upstream_reset_replay_contract_id": UPSTREAM_RESET_REPLAY_CONTRACT_ID,
        "checks_passed": checks_passed,
        "checks_total": checks_total,
        "dynamic_probes_skipped": bool(args.skip_dynamic_probes),
        "dynamic_case_count": len(dynamic_cases),
        "dynamic_cases": dynamic_cases,
        "failures": [finding.__dict__ for finding in failures],
    }

    summary_path = (ROOT / args.summary_out).resolve()
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}")
        print(f"[summary] wrote {display_path(summary_path)}")
        return 1

    print(
        f"[ok] {CONTRACT_ID} checks passed "
        f"({checks_passed}/{checks_total}); summary: {display_path(summary_path)}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
