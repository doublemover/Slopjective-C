from __future__ import annotations

import sys

import pytest

from scripts.objc3c_workflow.actions import ci_llvm_install, developer_tooling_llvm_probe
from scripts.objc3c_workflow.environment import SCRIPT_ROOT


@pytest.mark.parametrize(
    "platform,host,suffix",
    [("linux-x64", "linux", "linux"), ("darwin-arm64", "darwin", "darwin")],
)
def test_posix_installer_routes_to_selected_host(monkeypatch, platform, host, suffix):
    commands = []
    monkeypatch.setattr(sys, "platform", host)
    monkeypatch.setattr(ci_llvm_install, "run", lambda command: commands.append(command) or 7)
    assert ci_llvm_install.action_materialize_ci_llvm(["--platform", platform]) == 7
    assert commands == [["bash", str(SCRIPT_ROOT / f"install_objc3c_ci_llvm_{suffix}.sh")]]


def test_windows_installer_uses_powershell_owner(monkeypatch):
    scripts = []
    monkeypatch.setattr(sys, "platform", "win32")
    monkeypatch.setattr(ci_llvm_install, "pwsh_file", lambda path: scripts.append(path) or 3)
    assert ci_llvm_install.action_materialize_ci_llvm(["--platform", "windows-x64"]) == 3
    assert scripts == [SCRIPT_ROOT / "install_objc3c_ci_llvm.ps1"]


@pytest.mark.parametrize("args", [[], ["--platform", "unknown"], ["--platform", "linux-x64"], ["--platform", "windows-x64", "--extra"]])
def test_invalid_installer_requests_do_not_launch(monkeypatch, args):
    monkeypatch.setattr(sys, "platform", "win32")
    monkeypatch.setattr(ci_llvm_install, "run", lambda _: pytest.fail("unexpected installer"))
    monkeypatch.setattr(ci_llvm_install, "pwsh_file", lambda _: pytest.fail("unexpected installer"))
    with pytest.raises(SystemExit) as exc:
        ci_llvm_install.action_materialize_ci_llvm(args)
    assert exc.value.code == 2


def test_capability_probe_preserves_default_and_explicit_destination(monkeypatch):
    commands = []
    monkeypatch.setattr(developer_tooling_llvm_probe, "run", lambda command: commands.append(command) or 9)
    assert developer_tooling_llvm_probe.action_check_llvm_capabilities([]) == 9
    assert commands[-1] == developer_tooling_llvm_probe.default_llvm_capabilities_command()
    path = "tmp/reports/platform-host-evidence/llvm-capabilities.json"
    assert developer_tooling_llvm_probe.action_check_llvm_capabilities(["--summary-out", path]) == 9
    assert commands[-1] == developer_tooling_llvm_probe.capability_probe_command(path)


def test_capability_probe_rejects_unknown_options(monkeypatch):
    monkeypatch.setattr(developer_tooling_llvm_probe, "run", lambda _: pytest.fail("unexpected probe"))
    with pytest.raises(SystemExit):
        developer_tooling_llvm_probe.action_check_llvm_capabilities(["--unknown"])
