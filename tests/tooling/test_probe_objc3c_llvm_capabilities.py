from __future__ import annotations

from pathlib import Path

import pytest

from objc3c_llvm_capabilities_probe_assertions import (
    assert_clang_missing_payload,
    assert_filetype_unsupported_payload,
    assert_filetype_support_payload,
    assert_llc_launch_file_not_found_payload,
    assert_llc_missing_payload,
    assert_llvm_ar_missing_payload,
    assert_llvm_config_headers_missing_payload,
    assert_mismatched_llvm_tool_versions_payload,
    assert_mixed_toolchain_root_payload,
    assert_package_wires_llvm_capability_probe_script,
    assert_success_payload,
    assert_windows_install_root_header_library_payload,
)
from objc3c_llvm_capabilities_probe_json import load_json
from objc3c_llvm_capabilities_probe_subprocess import (
    fake_capabilities_detected_run,
    fake_clang_missing_run,
    fake_filetype_command_probe_run,
    fake_llc_filetype_unsupported_run,
    fake_llc_launch_file_not_found_run,
    fake_llc_missing_run,
    fake_llvm_ar_missing_run,
    fake_llvm_config_headers_missing_run,
    fake_windows_install_root_without_llvm_config_run,
    fake_mismatched_llvm_tool_versions_run,
    fake_mixed_toolchain_root_run,
)
from objc3c_llvm_capabilities_probe_support import PACKAGE_JSON, probe


@pytest.fixture(autouse=True)
def clear_llvm_root_default(monkeypatch) -> None:
    monkeypatch.delenv("LLVM_ROOT", raising=False)


def test_probe_passes_when_clang_and_llc_capabilities_are_detected(
    tmp_path: Path,
    monkeypatch,
) -> None:
    monkeypatch.setattr(probe.subprocess, "run", fake_capabilities_detected_run)
    summary_out = tmp_path / "summary.json"
    exit_code = probe.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    assert_success_payload(load_json(summary_out))


def test_probe_fails_when_llc_is_missing(tmp_path: Path, monkeypatch) -> None:
    monkeypatch.setattr(probe.subprocess, "run", fake_llc_missing_run)
    summary_out = tmp_path / "summary.json"
    exit_code = probe.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    assert_llc_missing_payload(load_json(summary_out))


def test_probe_accepts_filetype_support_from_command_probe(
    tmp_path: Path,
    monkeypatch,
) -> None:
    monkeypatch.setattr(probe.subprocess, "run", fake_filetype_command_probe_run)
    summary_out = tmp_path / "summary.json"
    exit_code = probe.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    assert_filetype_support_payload(load_json(summary_out))


def test_probe_fail_closes_when_llc_filetype_obj_is_unsupported(
    tmp_path: Path,
    monkeypatch,
) -> None:
    monkeypatch.setattr(probe.subprocess, "run", fake_llc_filetype_unsupported_run)
    summary_out = tmp_path / "summary.json"
    exit_code = probe.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    assert_filetype_unsupported_payload(load_json(summary_out))


def test_probe_flags_semantic_diagnostics_unavailable_when_clang_is_missing(
    tmp_path: Path,
    monkeypatch,
) -> None:
    monkeypatch.setattr(probe.subprocess, "run", fake_clang_missing_run)
    summary_out = tmp_path / "summary.json"
    exit_code = probe.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    assert_clang_missing_payload(load_json(summary_out))


def test_probe_fail_closes_when_subprocess_launch_raises_file_not_found(
    tmp_path: Path,
    monkeypatch,
) -> None:
    monkeypatch.setattr(probe.subprocess, "run", fake_llc_launch_file_not_found_run)
    summary_out = tmp_path / "summary.json"
    exit_code = probe.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    assert_llc_launch_file_not_found_payload(load_json(summary_out))


def test_probe_fail_closes_when_llvm_ar_is_missing(tmp_path: Path, monkeypatch) -> None:
    monkeypatch.setattr(probe.subprocess, "run", fake_llvm_ar_missing_run)
    summary_out = tmp_path / "summary.json"
    exit_code = probe.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    assert_llvm_ar_missing_payload(load_json(summary_out))


def test_probe_fail_closes_when_llvm_config_cannot_publish_headers_and_libs(
    tmp_path: Path,
    monkeypatch,
) -> None:
    monkeypatch.setattr(probe.subprocess, "run", fake_llvm_config_headers_missing_run)
    summary_out = tmp_path / "summary.json"
    exit_code = probe.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    assert_llvm_config_headers_missing_payload(load_json(summary_out))


def test_probe_fail_closes_when_required_llvm_tool_versions_mismatch(
    tmp_path: Path,
    monkeypatch,
) -> None:
    monkeypatch.setattr(probe.subprocess, "run", fake_mismatched_llvm_tool_versions_run)
    summary_out = tmp_path / "summary.json"
    exit_code = probe.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    assert_mismatched_llvm_tool_versions_payload(load_json(summary_out))


def test_probe_fail_closes_when_required_llvm_tools_resolve_from_mixed_roots(
    tmp_path: Path,
    monkeypatch,
) -> None:
    clang_root = tmp_path / "LLVM-A"
    llc_root = tmp_path / "LLVM-B"
    config_root = tmp_path / "LLVM-C"
    for root in (clang_root, llc_root, config_root):
        (root / "bin").mkdir(parents=True)
        (root / "include").mkdir()
        (root / "lib").mkdir()
    clang = clang_root / "bin" / "clang.exe"
    clangxx = clang_root / "bin" / "clang++.exe"
    llc = llc_root / "bin" / "llc.exe"
    llvm_ar = llc_root / "bin" / "llvm-ar.exe"
    llvm_config = config_root / "bin" / "llvm-config.exe"
    for path in (clang, clangxx, llc, llvm_ar, llvm_config):
        path.write_text("", encoding="utf-8")

    monkeypatch.setattr(probe.subprocess, "run", fake_mixed_toolchain_root_run)
    summary_out = tmp_path / "summary.json"
    exit_code = probe.run(
        [
            "--clang",
            str(clang),
            "--clangxx",
            str(clangxx),
            "--llc",
            str(llc),
            "--llvm-ar",
            str(llvm_ar),
            "--llvm-config",
            str(llvm_config),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    assert_mixed_toolchain_root_payload(load_json(summary_out))


def test_probe_accepts_official_windows_install_root_when_llvm_config_is_absent(
    tmp_path: Path,
    monkeypatch,
) -> None:
    install_root = tmp_path / "LLVM"
    bin_dir = install_root / "bin"
    include_dir = install_root / "include"
    lib_dir = install_root / "lib"
    bin_dir.mkdir(parents=True)
    include_dir.mkdir()
    lib_dir.mkdir()
    clang = bin_dir / "clang.exe"
    clangxx = bin_dir / "clang++.exe"
    llc = bin_dir / "llc.exe"
    llvm_ar = bin_dir / "llvm-ar.exe"
    llvm_config = bin_dir / "llvm-config.exe"
    for path in (clang, clangxx, llc, llvm_ar):
        path.write_text("", encoding="utf-8")

    monkeypatch.setattr(
        probe.subprocess,
        "run",
        fake_windows_install_root_without_llvm_config_run,
    )
    summary_out = tmp_path / "summary.json"
    exit_code = probe.run(
        [
            "--clang",
            str(clang),
            "--clangxx",
            str(clangxx),
            "--llc",
            str(llc),
            "--llvm-ar",
            str(llvm_ar),
            "--llvm-config",
            str(llvm_config),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 0
    assert_windows_install_root_header_library_payload(load_json(summary_out))


def test_probe_defaults_to_configured_llvm_root_when_present(
    tmp_path: Path,
    monkeypatch,
) -> None:
    install_root = tmp_path / "LLVM"
    bin_dir = install_root / "bin"
    include_dir = install_root / "include"
    lib_dir = install_root / "lib"
    bin_dir.mkdir(parents=True)
    include_dir.mkdir()
    lib_dir.mkdir()
    for tool in ("clang.exe", "clang++.exe", "llc.exe", "llvm-ar.exe", "llvm-config.exe"):
        (bin_dir / tool).write_text("", encoding="utf-8")

    monkeypatch.setenv("LLVM_ROOT", str(install_root))
    monkeypatch.setattr(probe.subprocess, "run", fake_mixed_toolchain_root_run)
    summary_out = tmp_path / "summary.json"
    exit_code = probe.run(["--summary-out", str(summary_out)])
    payload = load_json(summary_out)

    assert exit_code == 0
    assert payload["ok"] is True
    assert payload["toolchain_identity"]["claimable"] is True
    assert payload["toolchain_resolution"]["clang"]["configured_path"] == str(bin_dir / "clang.exe")
    assert payload["toolchain_resolution"]["clang++"]["configured_path"] == str(bin_dir / "clang++.exe")
    assert payload["toolchain_resolution"]["llc"]["configured_path"] == str(bin_dir / "llc.exe")
    assert payload["toolchain_resolution"]["llvm-ar"]["configured_path"] == str(bin_dir / "llvm-ar.exe")
    assert payload["toolchain_resolution"]["llvm-config"]["configured_path"] == str(bin_dir / "llvm-config.exe")


def test_package_wires_llvm_capability_probe_script() -> None:
    assert_package_wires_llvm_capability_probe_script(load_json(PACKAGE_JSON))
