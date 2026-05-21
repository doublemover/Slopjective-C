from __future__ import annotations

from pathlib import Path

from scripts.objc3c_workflow.action_catalog import ACTION_SPECS
from scripts.objc3c_workflow.action_handlers import ACTION_HANDLERS

ROOT = Path(__file__).resolve().parents[2]
CLEAN_ROOM_GATE = ROOT / "scripts" / "check_objc3c_native_clean_room_rebuild.py"
ENSURE_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"


def test_clean_room_rebuild_action_is_public_and_owned() -> None:
    spec = ACTION_SPECS["validate-native-clean-room-rebuild"]

    assert spec.backend == "python:scripts/check_objc3c_native_clean_room_rebuild.py"
    assert spec.validation_tier == "full"
    assert "without preexisting tmp or artifact outputs" in spec.guarantee_owner
    assert "validate-native-clean-room-rebuild" in ACTION_HANDLERS


def test_clean_room_gate_runs_two_forced_isolated_rebuilds() -> None:
    source = CLEAN_ROOM_GATE.read_text(encoding="utf-8")

    assert "run_rebuild(\"run-a\", args.mode, args.work_root, args.parallelism)" in source
    assert "run_rebuild(\"run-b\", args.mode, args.work_root, args.parallelism)" in source
    assert '"--clean-room-root"' in source
    assert '"--force-reconfigure"' in source
    assert '"--parallelism"' in source
    assert "default=4" in source
    assert "reset_work_root(args.work_root)" in source
    assert 'ROOT / "tmp" / "clean-room"' in source
    assert "shutil.rmtree(work_root)" in source


def test_clean_room_gate_compares_raw_and_normalized_artifacts() -> None:
    source = CLEAN_ROOM_GATE.read_text(encoding="utf-8")

    assert '"native_executable"' in source
    assert '"capi_runner"' in source
    assert '"runtime_library"' in source
    assert '"compile_commands"' in source
    assert '"build_fingerprint"' in source
    assert '"repo_superclean_surface"' in source
    assert "frontend_packet_digests" in source
    assert "normalized_json_digest" in source
    assert "normalized_value(payload, clean_root)" in source
    assert "<clean-room>" in source


def test_build_helper_uses_stable_logs_and_passes_clean_room_paths() -> None:
    source = ENSURE_HELPER.read_text(encoding="utf-8")

    assert "int(time.time())" not in source
    assert "safe_label(args.reason)" in source
    assert '"source-contracts": "contracts-source"' in source
    assert '"-NonInteractive"' in source
    assert '"-CleanRoomRoot", args.clean_room_root' in source
    assert '"-FrontendArtifactRoot", args.frontend_artifact_root' in source
    assert '"-SummaryPath", build_summary_path' in source
    assert '"-Parallelism", str(args.parallelism)' in source
    assert '"normalized_command"' in source
    assert '"saw_clean_room_root"' in source
    assert '"saw_native_build_summary"' in source
    assert '"saw_native_build_lock"' in source
    assert '"saw_native_build_lock_release"' in source
    assert '"saw_cmake_build_parallelism"' in source
