from __future__ import annotations

from pathlib import Path

from objc3c_llvm_capabilities_probe_assertions import (
    assert_clang_missing_payload,
    assert_filetype_unsupported_payload,
    assert_filetype_support_payload,
    assert_llc_launch_file_not_found_payload,
    assert_llc_missing_payload,
    assert_llvm_ar_missing_payload,
    assert_llvm_config_headers_missing_payload,
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
)
from objc3c_llvm_capabilities_probe_support import PACKAGE_JSON, probe


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


def test_package_wires_llvm_capability_probe_script() -> None:
    assert_package_wires_llvm_capability_probe_script(load_json(PACKAGE_JSON))
