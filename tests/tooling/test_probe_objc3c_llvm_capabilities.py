from __future__ import annotations

import importlib.util
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "probe_objc3c_llvm_capabilities.py"
PACKAGE_JSON = ROOT / "package.json"
SPEC = importlib.util.spec_from_file_location("probe_objc3c_llvm_capabilities", SCRIPT_PATH)
assert SPEC is not None and SPEC.loader is not None
probe = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = probe
SPEC.loader.exec_module(probe)


def _fake_completed(
    command: list[str],
    *,
    returncode: int,
    stdout: str = "",
    stderr: str = "",
) -> subprocess.CompletedProcess[str]:
    return subprocess.CompletedProcess(command, returncode, stdout=stdout, stderr=stderr)


def test_probe_passes_when_clang_and_llc_capabilities_are_detected(
    tmp_path: Path,
    monkeypatch,
) -> None:
    def fake_run(command: list[str], **_: object) -> subprocess.CompletedProcess[str]:
        cmd = tuple(command)
        if cmd == ("clang", "--version"):
            return _fake_completed(command, returncode=0, stdout="clang version 19.1.0\n")
        if cmd == ("llc", "--version"):
            return _fake_completed(command, returncode=0, stdout="Debian LLVM version 19.1.0\n")
        if cmd == ("llc", "--help"):
            return _fake_completed(command, returncode=0, stdout="--filetype=<type> ... obj ...\n")
        if cmd == ("llc", "--filetype=obj", "--version"):
            return _fake_completed(command, returncode=0, stdout="Debian LLVM version 19.1.0\n")
        raise AssertionError(f"unexpected command: {command}")

    monkeypatch.setattr(probe.subprocess, "run", fake_run)
    summary_out = tmp_path / "summary.json"
    exit_code = probe.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "objc3c-llvm-capabilities-v2"
    assert payload["ok"] is True
    assert payload["failures"] == []
    assert payload["llc_features"]["supports_filetype_obj"] is True
    assert payload["sema_type_system_parity"]["deterministic_semantic_diagnostics"] is True
    assert payload["sema_type_system_parity"]["deterministic_type_metadata_handoff"] is True
    assert payload["sema_type_system_parity"]["parity_ready"] is True
    assert payload["sema_type_system_parity"]["blockers"] == []


def test_probe_fails_when_llc_is_missing(tmp_path: Path, monkeypatch) -> None:
    def fake_run(command: list[str], **_: object) -> subprocess.CompletedProcess[str]:
        cmd = tuple(command)
        if cmd == ("clang", "--version"):
            return _fake_completed(command, returncode=0, stdout="clang version 19.1.0\n")
        if cmd == ("llc", "--version"):
            return _fake_completed(command, returncode=127, stderr="not found\n")
        raise AssertionError(f"unexpected command: {command}")

    monkeypatch.setattr(probe.subprocess, "run", fake_run)
    summary_out = tmp_path / "summary.json"
    exit_code = probe.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any("llc executable not found" in failure for failure in payload["failures"])
    assert payload["sema_type_system_parity"]["deterministic_semantic_diagnostics"] is True
    assert payload["sema_type_system_parity"]["deterministic_type_metadata_handoff"] is False
    assert payload["sema_type_system_parity"]["parity_ready"] is False
    assert "llc executable missing" in payload["sema_type_system_parity"]["blockers"]
    assert any("sema/type-system parity capability unavailable:" in failure for failure in payload["failures"])


def test_probe_accepts_filetype_support_from_command_probe(tmp_path: Path, monkeypatch) -> None:
    def fake_run(command: list[str], **_: object) -> subprocess.CompletedProcess[str]:
        cmd = tuple(command)
        if cmd == ("clang", "--version"):
            return _fake_completed(command, returncode=0, stdout="clang version 19.1.0\n")
        if cmd == ("llc", "--version"):
            return _fake_completed(command, returncode=0, stdout="Debian LLVM version 19.1.0\n")
        if cmd == ("llc", "--help"):
            return _fake_completed(command, returncode=0, stdout="llc help without obj token\n")
        if cmd == ("llc", "--filetype=obj", "--version"):
            return _fake_completed(command, returncode=0, stdout="Debian LLVM version 19.1.0\n")
        raise AssertionError(f"unexpected command: {command}")

    monkeypatch.setattr(probe.subprocess, "run", fake_run)
    summary_out = tmp_path / "summary.json"
    exit_code = probe.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["llc_features"]["supports_filetype_obj"] is True
    assert payload["sema_type_system_parity"]["parity_ready"] is True


def test_probe_flags_semantic_diagnostics_unavailable_when_clang_is_missing(tmp_path: Path, monkeypatch) -> None:
    def fake_run(command: list[str], **_: object) -> subprocess.CompletedProcess[str]:
        cmd = tuple(command)
        if cmd == ("clang", "--version"):
            return _fake_completed(command, returncode=127, stderr="not found\n")
        if cmd == ("llc", "--version"):
            return _fake_completed(command, returncode=0, stdout="Debian LLVM version 19.1.0\n")
        if cmd == ("llc", "--help"):
            return _fake_completed(command, returncode=0, stdout="--filetype=<type> ... obj ...\n")
        if cmd == ("llc", "--filetype=obj", "--version"):
            return _fake_completed(command, returncode=0, stdout="Debian LLVM version 19.1.0\n")
        raise AssertionError(f"unexpected command: {command}")

    monkeypatch.setattr(probe.subprocess, "run", fake_run)
    summary_out = tmp_path / "summary.json"
    exit_code = probe.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["clang"]["found"] is False
    assert payload["llc_features"]["supports_filetype_obj"] is True
    assert payload["sema_type_system_parity"]["deterministic_semantic_diagnostics"] is False
    assert payload["sema_type_system_parity"]["deterministic_type_metadata_handoff"] is False
    assert payload["sema_type_system_parity"]["parity_ready"] is False
    assert "clang executable missing" in payload["sema_type_system_parity"]["blockers"]
    assert any("sema/type-system parity capability unavailable:" in failure for failure in payload["failures"])


def test_probe_fail_closes_when_subprocess_launch_raises_file_not_found(tmp_path: Path, monkeypatch) -> None:
    def fake_run(command: list[str], **_: object) -> subprocess.CompletedProcess[str]:
        cmd = tuple(command)
        if cmd == ("clang", "--version"):
            return _fake_completed(command, returncode=0, stdout="clang version 19.1.0\n")
        if cmd[0] == "llc":
            raise FileNotFoundError("llc not found")
        raise AssertionError(f"unexpected command: {command}")

    monkeypatch.setattr(probe.subprocess, "run", fake_run)
    summary_out = tmp_path / "summary.json"
    exit_code = probe.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["llc"]["found"] is False
    assert payload["llc"]["version_exit_code"] == 127
    assert payload["llc"]["diagnostic"] == "llc executable not found: llc"
    assert payload["sema_type_system_parity"]["parity_ready"] is False
    assert "llc executable missing" in payload["sema_type_system_parity"]["blockers"]


def test_package_wires_llvm_capability_probe_script() -> None:
    payload = json.loads(PACKAGE_JSON.read_text(encoding="utf-8"))
    scripts = payload["scripts"]

    assert "check:objc3c:llvm-capabilities" in scripts
    command = scripts["check:objc3c:llvm-capabilities"]
    assert "scripts/probe_objc3c_llvm_capabilities.py" in command
    assert "tmp/artifacts/objc3c-native/m144/llvm_capabilities/summary.json" in command
