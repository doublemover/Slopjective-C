#!/usr/bin/env python3
"""Fail-closed checker for M263-D003 live restart hardening."""

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
MODE = "m263-d003-live-restart-hardening-edge-case-and-compatibility-completion-v1"
CONTRACT_ID = "objc3c-runtime-live-restart-hardening/m263-d003-v1"
UPSTREAM_LIVE_REGISTRATION_CONTRACT_ID = (
    "objc3c-runtime-live-registration-discovery-replay/m263-d002-v1"
)
UPSTREAM_FAILURE_RESTART_CONTRACT_ID = (
    "objc3c-runtime-bootstrap-failure-restart-semantics/m263-b003-v1"
)
UPSTREAM_RESET_REPLAY_CONTRACT_ID = (
    "objc3c-runtime-bootstrap-reset-replay/m254-d003-v1"
)
IDEMPOTENCE_MODEL = (
    "second-live-replay-without-reset-fails-closed-and-preserves-live-runtime-state"
)
TEARDOWN_MODEL = (
    "reset-clears-live-state-zeroes-image-local-init-cells-and-retains-bootstrap-catalog"
)
RESTART_EVIDENCE_MODEL = (
    "repeated-reset-replay-cycles-publish-monotonic-reset-and-replay-generations"
)
RESET_FOR_TESTING_SYMBOL = "objc3_runtime_reset_for_testing"
REPLAY_REGISTERED_IMAGES_SYMBOL = "objc3_runtime_replay_registered_images_for_testing"
RESET_REPLAY_SNAPSHOT_SYMBOL = "objc3_runtime_copy_reset_replay_state_for_testing"
INVALID_DESCRIPTOR_STATUS_CODE = -1
INTERNAL_HEADER_PATH = "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h"
RUNTIME_SOURCE_PATH = "native/objc3c/src/runtime/objc3_runtime.cpp"
PROCESS_CPP_PATH = "native/objc3c/src/io/objc3_process.cpp"
REGISTRATION_MANIFEST_ARTIFACT = "module.runtime-registration-manifest.json"
DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m263_live_restart_hardening_edge_case_and_compatibility_completion_d003_expectations.md"
DEFAULT_PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m263" / "m263_d003_live_restart_hardening_edge_case_and_compatibility_completion_packet.md"
DEFAULT_NATIVE_DOC = ROOT / "docs" / "objc3c-native.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_INTERNAL_HEADER = ROOT / INTERNAL_HEADER_PATH
DEFAULT_RUNTIME_SOURCE = ROOT / RUNTIME_SOURCE_PATH
DEFAULT_PROCESS_CPP = ROOT / PROCESS_CPP_PATH
DEFAULT_RUNTIME_PROBE = ROOT / "tests" / "tooling" / "runtime" / "m263_d003_live_restart_hardening_probe.cpp"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_RUNTIME_LIBRARY = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
DEFAULT_RUNTIME_INCLUDE_ROOT = ROOT / "native" / "objc3c" / "src"
DEFAULT_EXPLICIT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m263_bootstrap_failure_restart_explicit.objc3"
DEFAULT_DEFAULT_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m263_bootstrap_failure_restart_default.objc3"
DEFAULT_PROBE_ROOT = ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "m263" / "d003-live-restart-hardening"
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m263/M263-D003/live_restart_hardening_summary.json")


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
    DynamicCase("default", DEFAULT_DEFAULT_FIXTURE),
    DynamicCase("explicit", DEFAULT_EXPLICIT_FIXTURE),
)

EXPECTATIONS_SNIPPETS = (
    SnippetCheck(
        "M263-D003-DOC-EXP-01",
        "# M263 Live Restart Hardening Edge Case And Compatibility Completion Expectations (D003)",
    ),
    SnippetCheck("M263-D003-DOC-EXP-02", f"Contract ID: `{CONTRACT_ID}`"),
    SnippetCheck("M263-D003-DOC-EXP-03", f"`{INTERNAL_HEADER_PATH}`"),
    SnippetCheck("M263-D003-DOC-EXP-04", f"`{RUNTIME_SOURCE_PATH}`"),
    SnippetCheck("M263-D003-DOC-EXP-05", f"`{PROCESS_CPP_PATH}`"),
)
PACKET_SNIPPETS = (
    SnippetCheck(
        "M263-D003-DOC-PKT-01",
        "# M263-D003 Live Restart Hardening Edge Case And Compatibility Completion Packet",
    ),
    SnippetCheck("M263-D003-DOC-PKT-02", "Packet: `M263-D003`"),
    SnippetCheck("M263-D003-DOC-PKT-03", f"`{CONTRACT_ID}`"),
    SnippetCheck(
        "M263-D003-DOC-PKT-04",
        "prove repeated reset/replay cycles advance reset and replay generation evidence monotonically",
    ),
)
NATIVE_DOC_SNIPPETS = (
    SnippetCheck(
        "M263-D003-NDOC-01",
        "## Live restart hardening (M263-D003)",
    ),
    SnippetCheck("M263-D003-NDOC-02", f"`{IDEMPOTENCE_MODEL}`"),
    SnippetCheck("M263-D003-NDOC-03", f"`{RESET_FOR_TESTING_SYMBOL}`"),
)
LOWERING_SPEC_SNIPPETS = (
    SnippetCheck(
        "M263-D003-SPC-01",
        "## M263 live restart hardening (D003)",
    ),
    SnippetCheck("M263-D003-SPC-02", f"`{CONTRACT_ID}`"),
    SnippetCheck("M263-D003-SPC-03", f"`{RESTART_EVIDENCE_MODEL}`"),
)
METADATA_SPEC_SNIPPETS = (
    SnippetCheck(
        "M263-D003-META-01",
        "## M263 live restart hardening anchors (D003)",
    ),
    SnippetCheck(
        "M263-D003-META-02",
        "`bootstrap_live_restart_hardening_contract_id`",
    ),
    SnippetCheck("M263-D003-META-03", "`reset_generation`"),
)
INTERNAL_HEADER_SNIPPETS = (
    SnippetCheck(
        "M263-D003-INTH-01",
        "M263-D003 live-restart-hardening anchor",
    ),
    SnippetCheck("M263-D003-INTH-02", REPLAY_REGISTERED_IMAGES_SYMBOL),
    SnippetCheck("M263-D003-INTH-03", RESET_REPLAY_SNAPSHOT_SYMBOL),
)
RUNTIME_SOURCE_SNIPPETS = (
    SnippetCheck(
        "M263-D003-RTS-01",
        "M263-D003 live-restart-hardening anchor",
    ),
    SnippetCheck("M263-D003-RTS-02", "state.replay_generation"),
    SnippetCheck("M263-D003-RTS-03", "state.reset_generation"),
    SnippetCheck("M263-D003-RTS-04", "state.last_reset_cleared_image_local_init_state_count"),
)
PROCESS_CPP_SNIPPETS = (
    SnippetCheck(
        "M263-D003-PROC-01",
        "kObjc3RuntimeLiveRestartHardeningContractId",
    ),
    SnippetCheck(
        "M263-D003-PROC-02",
        "bootstrap_live_restart_hardening_contract_id",
    ),
    SnippetCheck(
        "M263-D003-PROC-03",
        "ready_for_live_restart_hardening",
    ),
)
PROBE_SNIPPETS = (
    SnippetCheck(
        "M263-D003-PRB-01",
        '#include "runtime/objc3_runtime_bootstrap_internal.h"',
    ),
    SnippetCheck("M263-D003-PRB-02", "second_unsupported_replay_status"),
    SnippetCheck("M263-D003-PRB-03", "second_restart_replay_generation"),
)
PACKAGE_SNIPPETS = (
    SnippetCheck(
        "M263-D003-PKG-01",
        '"check:objc3c:m263-d003-live-restart-hardening": "python scripts/check_m263_d003_live_restart_hardening_edge_case_and_compatibility_completion.py"',
    ),
    SnippetCheck(
        "M263-D003-PKG-02",
        '"test:tooling:m263-d003-live-restart-hardening": "python -m pytest tests/tooling/test_check_m263_d003_live_restart_hardening_edge_case_and_compatibility_completion.py -q"',
    ),
    SnippetCheck(
        "M263-D003-PKG-03",
        '"check:objc3c:m263-d003-lane-d-readiness": "python scripts/run_m263_d003_lane_d_readiness.py"',
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
    parser.add_argument("--default-fixture", type=Path, default=DEFAULT_DEFAULT_FIXTURE)
    parser.add_argument("--explicit-fixture", type=Path, default=DEFAULT_EXPLICIT_FIXTURE)
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
                "M263-D003-MISSING",
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


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
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


def run_dynamic_case(*, args: argparse.Namespace, case_input: DynamicCase, clangxx: str, failures: list[Finding]) -> tuple[int, int, dict[str, object]]:
    checks_total = 0
    checks_passed = 0
    case: dict[str, object] = {"case_id": case_input.case_id, "fixture": display_path(case_input.fixture)}

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

    registration_manifest_path = out_dir / REGISTRATION_MANIFEST_ARTIFACT
    obj_path = out_dir / "module.obj"
    backend_path = out_dir / "module.object-backend.txt"

    checks_total += 1
    checks_passed += require(
        compile_result.returncode == 0,
        display_path(case_input.fixture),
        f"M263-D003-{case_input.case_id}-COMPILE",
        f"native compile exited with {compile_result.returncode}",
        failures,
    )
    for artifact_path, suffix in (
        (registration_manifest_path, "REG-MANIFEST"),
        (obj_path, "OBJ"),
    ):
        checks_total += 1
        checks_passed += require(
            artifact_path.exists(),
            display_path(artifact_path),
            f"M263-D003-{case_input.case_id}-{suffix}",
            f"required artifact is missing: {display_path(artifact_path)}",
            failures,
        )
    if any(f.check_id.startswith(f"M263-D003-{case_input.case_id}-") for f in failures):
        case["compile_stdout"] = compile_result.stdout
        case["compile_stderr"] = compile_result.stderr
        return checks_total, checks_passed, case

    registration_manifest_payload = load_json(registration_manifest_path)
    backend_text = read_text(backend_path).strip() if backend_path.exists() else ""

    for condition, check_id, detail in (
        (
            registration_manifest_payload.get("bootstrap_live_restart_hardening_contract_id") == CONTRACT_ID,
            f"M263-D003-{case_input.case_id}-REG-CONTRACT",
            "registration manifest D003 contract mismatch",
        ),
        (
            registration_manifest_payload.get("bootstrap_live_idempotence_model") == IDEMPOTENCE_MODEL,
            f"M263-D003-{case_input.case_id}-REG-IDEMPOTENCE",
            "registration manifest idempotence model mismatch",
        ),
        (
            registration_manifest_payload.get("bootstrap_live_teardown_model") == TEARDOWN_MODEL,
            f"M263-D003-{case_input.case_id}-REG-TEARDOWN",
            "registration manifest teardown model mismatch",
        ),
        (
            registration_manifest_payload.get("bootstrap_live_restart_evidence_model") == RESTART_EVIDENCE_MODEL,
            f"M263-D003-{case_input.case_id}-REG-EVIDENCE",
            "registration manifest restart evidence model mismatch",
        ),
        (
            registration_manifest_payload.get("bootstrap_live_restart_reset_for_testing_symbol") == RESET_FOR_TESTING_SYMBOL,
            f"M263-D003-{case_input.case_id}-REG-RESET-SYM",
            "registration manifest reset symbol mismatch",
        ),
        (
            registration_manifest_payload.get("bootstrap_live_restart_replay_registered_images_symbol") == REPLAY_REGISTERED_IMAGES_SYMBOL,
            f"M263-D003-{case_input.case_id}-REG-REPLAY-SYM",
            "registration manifest replay symbol mismatch",
        ),
        (
            registration_manifest_payload.get("bootstrap_live_restart_reset_replay_state_snapshot_symbol") == RESET_REPLAY_SNAPSHOT_SYMBOL,
            f"M263-D003-{case_input.case_id}-REG-SNAPSHOT-SYM",
            "registration manifest reset/replay snapshot symbol mismatch",
        ),
        (
            registration_manifest_payload.get("bootstrap_live_registration_contract_id") == UPSTREAM_LIVE_REGISTRATION_CONTRACT_ID,
            f"M263-D003-{case_input.case_id}-REG-UPSTREAM-D002",
            "registration manifest upstream D002 contract mismatch",
        ),
        (
            registration_manifest_payload.get("bootstrap_reset_contract_id") == UPSTREAM_RESET_REPLAY_CONTRACT_ID,
            f"M263-D003-{case_input.case_id}-REG-UPSTREAM-RESET",
            "registration manifest upstream reset/replay contract mismatch",
        ),
        (
            registration_manifest_payload.get("ready_for_live_restart_hardening") is True,
            f"M263-D003-{case_input.case_id}-REG-READY",
            "registration manifest live restart hardening readiness flag must be true",
        ),
        (
            backend_text == "llvm-direct",
            f"M263-D003-{case_input.case_id}-BACKEND",
            "object backend must remain llvm-direct",
        ),
    ):
        checks_total += 1
        checks_passed += require(condition, display_path(registration_manifest_path), check_id, detail, failures)
    if any(f.check_id.startswith(f"M263-D003-{case_input.case_id}-") for f in failures):
        return checks_total, checks_passed, case

    probe_out_dir = out_dir / "probe"
    probe_out_dir.mkdir(parents=True, exist_ok=True)
    probe_exe = probe_out_dir / f"{case_input.case_id}-live-restart-hardening-probe.exe"
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
        f"M263-D003-{case_input.case_id}-PROBE-COMPILE",
        f"probe compile exited with {probe_compile.returncode}",
        failures,
    )
    checks_total += 1
    checks_passed += require(
        probe_exe.exists(),
        display_path(probe_exe),
        f"M263-D003-{case_input.case_id}-PROBE-EXE",
        "probe executable is missing",
        failures,
    )
    if any(f.check_id.startswith(f"M263-D003-{case_input.case_id}-") for f in failures):
        case["probe_compile_stdout"] = probe_compile.stdout
        case["probe_compile_stderr"] = probe_compile.stderr
        return checks_total, checks_passed, case

    probe_run = run_command([str(probe_exe)])
    checks_total += 1
    checks_passed += require(
        probe_run.returncode == 0,
        display_path(probe_exe),
        f"M263-D003-{case_input.case_id}-PROBE-RUN",
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
                f"M263-D003-{case_input.case_id}-PROBE-JSON",
                f"invalid probe JSON: {exc}",
            )
        )
        return checks_total + 1, checks_passed, case

    expected_identity_key = str(registration_manifest_payload.get("translation_unit_identity_key", ""))

    for condition, check_id, detail in (
        (probe_payload.get("startup_registration_copy_status") == 0, f"M263-D003-{case_input.case_id}-START-REG", "startup registration snapshot copy must succeed"),
        (probe_payload.get("startup_reset_replay_copy_status") == 0, f"M263-D003-{case_input.case_id}-START-RESET", "startup reset snapshot copy must succeed"),
        (probe_payload.get("startup_registered_image_count") == 1, f"M263-D003-{case_input.case_id}-START-COUNT", "startup registered image count mismatch"),
        (probe_payload.get("startup_last_registered_translation_unit_identity_key") == expected_identity_key, f"M263-D003-{case_input.case_id}-START-ID", "startup identity mismatch"),
        (probe_payload.get("unsupported_replay_status") == INVALID_DESCRIPTOR_STATUS_CODE, f"M263-D003-{case_input.case_id}-UNSUPPORTED-STATUS", "unsupported replay must fail closed"),
        (probe_payload.get("unsupported_replay_registration_copy_status") == 0, f"M263-D003-{case_input.case_id}-UNSUPPORTED-REG", "unsupported replay registration snapshot copy must succeed"),
        (probe_payload.get("unsupported_replay_reset_replay_copy_status") == 0, f"M263-D003-{case_input.case_id}-UNSUPPORTED-RESET", "unsupported replay reset snapshot copy must succeed"),
        (probe_payload.get("unsupported_replay_registered_image_count") == 1, f"M263-D003-{case_input.case_id}-UNSUPPORTED-COUNT", "unsupported replay must not clear live state"),
        (probe_payload.get("unsupported_replay_last_replay_status") == INVALID_DESCRIPTOR_STATUS_CODE, f"M263-D003-{case_input.case_id}-UNSUPPORTED-LAST", "unsupported replay status snapshot mismatch"),
        (probe_payload.get("unsupported_replay_last_replayed_image_count") == 0, f"M263-D003-{case_input.case_id}-UNSUPPORTED-REPLAYED", "unsupported replay must not replay any image"),
        (probe_payload.get("post_reset_registration_copy_status") == 0, f"M263-D003-{case_input.case_id}-RESET-REG", "post-reset registration snapshot copy must succeed"),
        (probe_payload.get("post_reset_reset_replay_copy_status") == 0, f"M263-D003-{case_input.case_id}-RESET-SNAPSHOT", "post-reset reset snapshot copy must succeed"),
        (probe_payload.get("post_reset_registered_image_count") == 0, f"M263-D003-{case_input.case_id}-RESET-COUNT", "post-reset registered image count must be zero"),
        (probe_payload.get("post_reset_next_expected_registration_order_ordinal") == 1, f"M263-D003-{case_input.case_id}-RESET-ORDINAL", "post-reset next ordinal must be 1"),
        (probe_payload.get("post_reset_retained_bootstrap_image_count") == 1, f"M263-D003-{case_input.case_id}-RESET-RETAINED", "post-reset retained image count mismatch"),
        (probe_payload.get("post_reset_last_reset_cleared_image_local_init_state_count", 0) >= 1, f"M263-D003-{case_input.case_id}-RESET-CLEARED", "reset must clear at least one image-local init cell"),
        (probe_payload.get("post_reset_reset_generation", 0) >= 1, f"M263-D003-{case_input.case_id}-RESET-GEN", "reset generation must advance"),
        (probe_payload.get("first_restart_status") == 0, f"M263-D003-{case_input.case_id}-FIRST-RESTART-STATUS", "first restart replay must succeed"),
        (probe_payload.get("first_restart_registration_copy_status") == 0, f"M263-D003-{case_input.case_id}-FIRST-RESTART-REG", "first restart registration snapshot copy must succeed"),
        (probe_payload.get("first_restart_image_walk_copy_status") == 0, f"M263-D003-{case_input.case_id}-FIRST-RESTART-WALK", "first restart image-walk snapshot copy must succeed"),
        (probe_payload.get("first_restart_reset_replay_copy_status") == 0, f"M263-D003-{case_input.case_id}-FIRST-RESTART-RESET", "first restart reset snapshot copy must succeed"),
        (probe_payload.get("first_restart_registered_image_count") == 1, f"M263-D003-{case_input.case_id}-FIRST-RESTART-COUNT", "first restart must restore one image"),
        (probe_payload.get("first_restart_last_registration_status") == 0, f"M263-D003-{case_input.case_id}-FIRST-RESTART-LAST", "first restart registration status mismatch"),
        (probe_payload.get("first_restart_last_replayed_image_count") == 1, f"M263-D003-{case_input.case_id}-FIRST-RESTART-REPLAYED", "first restart replayed image count mismatch"),
        (probe_payload.get("first_restart_replay_generation", 0) >= 1, f"M263-D003-{case_input.case_id}-FIRST-RESTART-GEN", "first restart replay generation must advance"),
        (probe_payload.get("first_restart_last_registered_translation_unit_identity_key") == expected_identity_key, f"M263-D003-{case_input.case_id}-FIRST-RESTART-REG-ID", "first restart registered identity mismatch"),
        (probe_payload.get("first_restart_last_replayed_translation_unit_identity_key") == expected_identity_key, f"M263-D003-{case_input.case_id}-FIRST-RESTART-REPLAY-ID", "first restart replayed identity mismatch"),
        (probe_payload.get("first_restart_last_walked_translation_unit_identity_key") == expected_identity_key, f"M263-D003-{case_input.case_id}-FIRST-RESTART-WALK-ID", "first restart walked identity mismatch"),
        (probe_payload.get("second_unsupported_replay_status") == INVALID_DESCRIPTOR_STATUS_CODE, f"M263-D003-{case_input.case_id}-SECOND-UNSUPPORTED-STATUS", "second unsupported replay must fail closed"),
        (probe_payload.get("second_unsupported_replay_registration_copy_status") == 0, f"M263-D003-{case_input.case_id}-SECOND-UNSUPPORTED-REG", "second unsupported replay registration snapshot copy must succeed"),
        (probe_payload.get("second_unsupported_replay_reset_replay_copy_status") == 0, f"M263-D003-{case_input.case_id}-SECOND-UNSUPPORTED-RESET", "second unsupported replay reset snapshot copy must succeed"),
        (probe_payload.get("second_unsupported_replay_registered_image_count") == 1, f"M263-D003-{case_input.case_id}-SECOND-UNSUPPORTED-COUNT", "second unsupported replay must not clear live state"),
        (probe_payload.get("second_unsupported_replay_last_replay_status") == INVALID_DESCRIPTOR_STATUS_CODE, f"M263-D003-{case_input.case_id}-SECOND-UNSUPPORTED-LAST", "second unsupported replay status snapshot mismatch"),
        (probe_payload.get("second_reset_registration_copy_status") == 0, f"M263-D003-{case_input.case_id}-SECOND-RESET-REG", "second reset registration snapshot copy must succeed"),
        (probe_payload.get("second_reset_reset_replay_copy_status") == 0, f"M263-D003-{case_input.case_id}-SECOND-RESET-SNAPSHOT", "second reset snapshot copy must succeed"),
        (probe_payload.get("second_reset_registered_image_count") == 0, f"M263-D003-{case_input.case_id}-SECOND-RESET-COUNT", "second reset registered image count must be zero"),
        (probe_payload.get("second_reset_next_expected_registration_order_ordinal") == 1, f"M263-D003-{case_input.case_id}-SECOND-RESET-ORDINAL", "second reset next ordinal must be 1"),
        (probe_payload.get("second_reset_retained_bootstrap_image_count") == 1, f"M263-D003-{case_input.case_id}-SECOND-RESET-RETAINED", "second reset retained image count mismatch"),
        (probe_payload.get("second_reset_last_reset_cleared_image_local_init_state_count", 0) >= 1, f"M263-D003-{case_input.case_id}-SECOND-RESET-CLEARED", "second reset must clear at least one image-local init cell"),
        (probe_payload.get("second_reset_reset_generation", 0) > probe_payload.get("post_reset_reset_generation", 0), f"M263-D003-{case_input.case_id}-SECOND-RESET-GEN", "second reset generation must advance again"),
        (probe_payload.get("second_restart_status") == 0, f"M263-D003-{case_input.case_id}-SECOND-RESTART-STATUS", "second restart replay must succeed"),
        (probe_payload.get("second_restart_registration_copy_status") == 0, f"M263-D003-{case_input.case_id}-SECOND-RESTART-REG", "second restart registration snapshot copy must succeed"),
        (probe_payload.get("second_restart_image_walk_copy_status") == 0, f"M263-D003-{case_input.case_id}-SECOND-RESTART-WALK", "second restart image-walk snapshot copy must succeed"),
        (probe_payload.get("second_restart_reset_replay_copy_status") == 0, f"M263-D003-{case_input.case_id}-SECOND-RESTART-RESET", "second restart reset snapshot copy must succeed"),
        (probe_payload.get("second_restart_registered_image_count") == 1, f"M263-D003-{case_input.case_id}-SECOND-RESTART-COUNT", "second restart must restore one image"),
        (probe_payload.get("second_restart_last_registration_status") == 0, f"M263-D003-{case_input.case_id}-SECOND-RESTART-LAST", "second restart registration status mismatch"),
        (probe_payload.get("second_restart_last_replayed_image_count") == 1, f"M263-D003-{case_input.case_id}-SECOND-RESTART-REPLAYED", "second restart replayed image count mismatch"),
        (probe_payload.get("second_restart_replay_generation", 0) > probe_payload.get("first_restart_replay_generation", 0), f"M263-D003-{case_input.case_id}-SECOND-RESTART-GEN", "second restart replay generation must advance again"),
        (probe_payload.get("second_restart_last_registered_translation_unit_identity_key") == expected_identity_key, f"M263-D003-{case_input.case_id}-SECOND-RESTART-REG-ID", "second restart registered identity mismatch"),
        (probe_payload.get("second_restart_last_replayed_translation_unit_identity_key") == expected_identity_key, f"M263-D003-{case_input.case_id}-SECOND-RESTART-REPLAY-ID", "second restart replayed identity mismatch"),
        (probe_payload.get("second_restart_last_walked_translation_unit_identity_key") == expected_identity_key, f"M263-D003-{case_input.case_id}-SECOND-RESTART-WALK-ID", "second restart walked identity mismatch"),
    ):
        checks_total += 1
        checks_passed += require(condition, display_path(probe_exe), check_id, detail, failures)

    case.update(
        {
            "registration_manifest": display_path(registration_manifest_path),
            "object_file": display_path(obj_path),
            "probe_exe": display_path(probe_exe),
            "runtime_library": display_path(runtime_library),
            "expected_translation_unit_identity_key": expected_identity_key,
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
        "M263-D003-TOOL-CLANGXX",
        f"required C++ compiler is not on PATH: {args.clangxx}",
        failures,
    )

    llc = resolve_tool("llc.exe")
    checks_total += 1
    checks_passed += require(
        llc is not None,
        "llc.exe",
        "M263-D003-TOOL-LLC",
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
        "upstream_live_registration_contract_id": UPSTREAM_LIVE_REGISTRATION_CONTRACT_ID,
        "upstream_failure_restart_contract_id": UPSTREAM_FAILURE_RESTART_CONTRACT_ID,
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
