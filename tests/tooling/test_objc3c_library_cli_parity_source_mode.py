from __future__ import annotations

from pathlib import Path

import pytest

from objc3c_library_cli_parity_assertions import (
    assert_capability_backend_routing,
    assert_capability_fail_closed_summary,
    assert_command_failure_summary,
    assert_failure_contains,
    assert_source_mode_success_summary,
    assert_work_keys_differ,
    assert_work_keys_match_and_are_hex,
    load_summary,
)
from objc3c_library_cli_parity_fixtures import (
    write_capability_summary,
    write_source_and_bins,
    write_text_fixture,
)
from objc3c_library_cli_parity_subprocess import (
    fake_command_failure_run,
    fake_observed_backend_run,
    fake_successful_source_mode_run,
    never_run_on_fail_closed,
)
from objc3c_library_cli_parity_support import parity


def test_parity_source_mode_generates_and_compares_cli_and_c_api_outputs(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    source, cli_bin, c_api_bin = write_source_and_bins(tmp_path)

    monkeypatch.setattr(parity.subprocess, "run", fake_successful_source_mode_run("m"))

    summary_out = tmp_path / "summary.json"
    exit_code = parity.run(
        [
            "--source",
            str(source),
            "--cli-bin",
            str(cli_bin),
            "--c-api-bin",
            str(c_api_bin),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 0
    assert_source_mode_success_summary(load_summary(summary_out))


def test_parity_source_mode_routes_backend_from_capabilities_when_enabled(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    source, cli_bin, c_api_bin = write_source_and_bins(tmp_path)
    summary_path = tmp_path / "capabilities.json"
    write_capability_summary(summary_path)

    observed_commands: list[list[str]] = []
    monkeypatch.setattr(
        parity.subprocess,
        "run",
        fake_observed_backend_run("route", observed_commands),
    )

    summary_out = tmp_path / "summary.json"
    exit_code = parity.run(
        [
            "--source",
            str(source),
            "--cli-bin",
            str(cli_bin),
            "--c-api-bin",
            str(c_api_bin),
            "--llvm-capabilities-summary",
            str(summary_path),
            "--route-cli-backend-from-capabilities",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 0
    assert_capability_backend_routing(load_summary(summary_out), observed_commands)


def test_parity_source_mode_fail_closes_when_capability_parity_is_unavailable(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    source, cli_bin, c_api_bin = write_source_and_bins(tmp_path)
    summary_path = tmp_path / "capabilities.json"
    write_capability_summary(
        summary_path,
        parity_ready=False,
        blockers=["llc executable missing"],
    )

    monkeypatch.setattr(parity.subprocess, "run", never_run_on_fail_closed)

    summary_out = tmp_path / "summary.json"
    exit_code = parity.run(
        [
            "--source",
            str(source),
            "--cli-bin",
            str(cli_bin),
            "--c-api-bin",
            str(c_api_bin),
            "--llvm-capabilities-summary",
            str(summary_path),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    assert_capability_fail_closed_summary(load_summary(summary_out))


def test_parity_source_mode_fail_closes_when_capability_routing_is_requested_without_summary(
    tmp_path: Path,
) -> None:
    source, cli_bin, c_api_bin = write_source_and_bins(tmp_path)

    summary_out = tmp_path / "summary.json"
    exit_code = parity.run(
        [
            "--source",
            str(source),
            "--cli-bin",
            str(cli_bin),
            "--c-api-bin",
            str(c_api_bin),
            "--route-cli-backend-from-capabilities",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    assert_failure_contains(
        load_summary(summary_out),
        "--route-cli-backend-from-capabilities requires --llvm-capabilities-summary",
    )


def test_parity_source_mode_requires_binaries(tmp_path: Path) -> None:
    source = tmp_path / "sample.objc3"
    source.write_text("fn main() -> i32 { return 0; }\n", encoding="utf-8")

    with pytest.raises(ValueError, match="--cli-bin is required"):
        parity.run(
            [
                "--source",
                str(source),
            ]
        )


@pytest.mark.parametrize(
    "work_key",
    [
        "",
        "../escape",
        "bad/key",
    ],
)
def test_parity_source_mode_rejects_invalid_work_key(
    tmp_path: Path,
    work_key: str,
) -> None:
    source, cli_bin, c_api_bin = write_source_and_bins(tmp_path)

    with pytest.raises(ValueError, match="--work-key"):
        parity.run(
            [
                "--source",
                str(source),
                "--cli-bin",
                str(cli_bin),
                "--c-api-bin",
                str(c_api_bin),
                "--work-key",
                work_key,
                "--allow-non-tmp-work-dir",
                "--work-dir",
                str(tmp_path / "work"),
            ]
        )


def test_parity_source_mode_rejects_non_tmp_work_dir_by_default(
    tmp_path: Path,
) -> None:
    source, cli_bin, c_api_bin = write_source_and_bins(tmp_path)

    with pytest.raises(ValueError, match="work-dir must be under tmp"):
        parity.run(
            [
                "--source",
                str(source),
                "--cli-bin",
                str(cli_bin),
                "--c-api-bin",
                str(c_api_bin),
                "--work-dir",
                str(tmp_path / "outside_tmp"),
            ]
        )


def test_parity_source_mode_can_allow_non_tmp_work_dir_when_opted_in(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    source, cli_bin, c_api_bin = write_source_and_bins(tmp_path)

    monkeypatch.setattr(
        parity.subprocess,
        "run",
        fake_successful_source_mode_run("tmp-override"),
    )

    summary_out = tmp_path / "summary.json"
    exit_code = parity.run(
        [
            "--source",
            str(source),
            "--cli-bin",
            str(cli_bin),
            "--c-api-bin",
            str(c_api_bin),
            "--work-dir",
            str(tmp_path / "outside_tmp"),
            "--allow-non-tmp-work-dir",
            "--summary-out",
            str(summary_out),
        ]
    )
    assert exit_code == 0


def test_parity_source_mode_work_key_changes_with_backend_and_runtime_contract(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    source, cli_bin, c_api_bin = write_source_and_bins(tmp_path)

    monkeypatch.setattr(
        parity.subprocess,
        "run",
        fake_successful_source_mode_run("key-variant"),
    )

    summary_a = tmp_path / "summary_a.json"
    exit_code_a = parity.run(
        [
            "--source",
            str(source),
            "--cli-bin",
            str(cli_bin),
            "--c-api-bin",
            str(c_api_bin),
            "--allow-non-tmp-work-dir",
            "--work-dir",
            str(tmp_path / "work"),
            "--summary-out",
            str(summary_a),
            "--objc3-runtime-dispatch-symbol",
            "objc3_runtime_dispatch_i32",
            "--cli-ir-object-backend",
            "clang",
        ]
    )
    assert exit_code_a == 0

    summary_b = tmp_path / "summary_b.json"
    exit_code_b = parity.run(
        [
            "--source",
            str(source),
            "--cli-bin",
            str(cli_bin),
            "--c-api-bin",
            str(c_api_bin),
            "--allow-non-tmp-work-dir",
            "--work-dir",
            str(tmp_path / "work"),
            "--summary-out",
            str(summary_b),
            "--objc3-runtime-dispatch-symbol",
            "objc3_runtime_dispatch_i32_alt",
            "--cli-ir-object-backend",
            "llvm-direct",
        ]
    )
    assert exit_code_b == 0

    assert_work_keys_differ(load_summary(summary_a), load_summary(summary_b))


def test_parity_source_mode_default_work_key_is_deterministic_for_same_inputs(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    source, cli_bin, c_api_bin = write_source_and_bins(tmp_path)

    monkeypatch.setattr(
        parity.subprocess,
        "run",
        fake_successful_source_mode_run("det-key"),
    )

    summary_a = tmp_path / "summary_a.json"
    summary_b = tmp_path / "summary_b.json"
    base_args = [
        "--source",
        str(source),
        "--cli-bin",
        str(cli_bin),
        "--c-api-bin",
        str(c_api_bin),
        "--objc3-runtime-dispatch-symbol",
        "objc3_runtime_dispatch_i32",
        "--cli-ir-object-backend",
        "clang",
    ]
    exit_code_a = parity.run(
        [
            *base_args,
            "--allow-non-tmp-work-dir",
            "--work-dir",
            str(tmp_path / "work_a"),
            "--summary-out",
            str(summary_a),
        ]
    )
    exit_code_b = parity.run(
        [
            *base_args,
            "--allow-non-tmp-work-dir",
            "--work-dir",
            str(tmp_path / "work_b"),
            "--summary-out",
            str(summary_b),
        ]
    )
    assert exit_code_a == 0
    assert exit_code_b == 0

    assert_work_keys_match_and_are_hex(load_summary(summary_a), load_summary(summary_b))


@pytest.mark.parametrize(
    "emit_prefix",
    [
        "",
        "../module",
        "bad/prefix",
        ".module",
    ],
)
def test_parity_source_mode_rejects_invalid_emit_prefix(
    tmp_path: Path,
    emit_prefix: str,
) -> None:
    source, cli_bin, c_api_bin = write_source_and_bins(tmp_path)

    with pytest.raises(ValueError, match="--emit-prefix"):
        parity.run(
            [
                "--source",
                str(source),
                "--cli-bin",
                str(cli_bin),
                "--c-api-bin",
                str(c_api_bin),
                "--allow-non-tmp-work-dir",
                "--work-dir",
                str(tmp_path / "work"),
                "--emit-prefix",
                emit_prefix,
            ]
        )


def test_parity_source_mode_fails_when_stale_generated_outputs_exist(
    tmp_path: Path,
) -> None:
    source, cli_bin, c_api_bin = write_source_and_bins(tmp_path)

    work_dir = tmp_path / "work"
    stale_dir = work_dir / "stale_key" / "library"
    stale_dir.mkdir(parents=True, exist_ok=True)
    write_text_fixture(stale_dir / "module.ll", "stale\n")

    with pytest.raises(ValueError, match="stale generated artifacts"):
        parity.run(
            [
                "--source",
                str(source),
                "--cli-bin",
                str(cli_bin),
                "--c-api-bin",
                str(c_api_bin),
                "--allow-non-tmp-work-dir",
                "--work-dir",
                str(work_dir),
                "--work-key",
                "stale_key",
            ]
        )


def test_parity_source_mode_fails_when_stale_proxy_outputs_exist(
    tmp_path: Path,
) -> None:
    source, cli_bin, c_api_bin = write_source_and_bins(tmp_path)

    work_dir = tmp_path / "work"
    stale_dir = work_dir / "stale_proxy_key" / "cli"
    stale_dir.mkdir(parents=True, exist_ok=True)
    write_text_fixture(stale_dir / "module.obj.sha256", "3" * 64 + "\n")

    with pytest.raises(ValueError, match="stale generated artifacts"):
        parity.run(
            [
                "--source",
                str(source),
                "--cli-bin",
                str(cli_bin),
                "--c-api-bin",
                str(c_api_bin),
                "--allow-non-tmp-work-dir",
                "--work-dir",
                str(work_dir),
                "--work-key",
                "stale_proxy_key",
            ]
        )



def test_parity_source_mode_reports_command_failures_with_execution_details(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
    capsys: pytest.CaptureFixture[str],
) -> None:
    source, cli_bin, c_api_bin = write_source_and_bins(tmp_path)

    monkeypatch.setattr(parity.subprocess, "run", fake_command_failure_run)

    summary_out = tmp_path / "summary.json"
    exit_code = parity.run(
        [
            "--source",
            str(source),
            "--cli-bin",
            str(cli_bin),
            "--c-api-bin",
            str(c_api_bin),
            "--summary-out",
            str(summary_out),
        ]
    )

    captured = capsys.readouterr()
    assert exit_code == 1
    assert "PARITY-FAIL: cli command failed with exit 17" in captured.err
    assert "PARITY-FAIL: c-api command failed with exit 23" in captured.err

    assert_command_failure_summary(load_summary(summary_out))
