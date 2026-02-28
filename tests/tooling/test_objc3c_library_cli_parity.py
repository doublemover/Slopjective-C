from __future__ import annotations

import importlib.util
import json
import subprocess
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_objc3c_library_cli_parity.py"
FIXTURE_ROOT = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "library_cli_parity"
)
SPEC = importlib.util.spec_from_file_location("check_objc3c_library_cli_parity", SCRIPT_PATH)
assert SPEC is not None and SPEC.loader is not None
parity = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = parity
SPEC.loader.exec_module(parity)


def _write(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def _write_source_mode_artifacts(out_dir: Path, *, emit_prefix: str, marker: str) -> None:
    out_dir.mkdir(parents=True, exist_ok=True)
    _write(out_dir / f"{emit_prefix}.diagnostics.json", f'{{"marker":"{marker}"}}\n')
    _write(out_dir / f"{emit_prefix}.manifest.json", f'{{"module":"{marker}"}}\n')
    _write(out_dir / f"{emit_prefix}.ll", "define i32 @main() { ret i32 0 }\n")
    (out_dir / f"{emit_prefix}.obj").write_bytes(b"\x00OBJ")


def test_parity_pass(tmp_path: Path) -> None:
    library_dir = tmp_path / "library"
    cli_dir = tmp_path / "cli"

    _write(library_dir / "module.diagnostics.json", '{"ok":true}\n')
    _write(cli_dir / "module.diagnostics.json", '{"ok":true}\n')
    _write(library_dir / "module.manifest.json", '{"module":"m"}\n')
    _write(cli_dir / "module.manifest.json", '{"module":"m"}\n')

    summary_out = tmp_path / "summary.json"
    exit_code = parity.run(
        [
            "--library-dir",
            str(library_dir),
            "--cli-dir",
            str(cli_dir),
            "--artifacts",
            "module.diagnostics.json",
            "module.manifest.json",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "objc3c-library-cli-parity-v2"
    assert payload["ok"] is True
    assert payload["failures"] == []
    assert [item["dimension"] for item in payload["dimensions"]] == [
        "diagnostics",
        "manifest",
        "ir",
        "object",
    ]


def test_parity_fail_on_digest_mismatch(tmp_path: Path) -> None:
    library_dir = tmp_path / "library"
    cli_dir = tmp_path / "cli"

    _write(library_dir / "module.ll", "define i32 @main() { ret i32 1 }\n")
    _write(cli_dir / "module.ll", "define i32 @main() { ret i32 2 }\n")

    summary_out = tmp_path / "summary.json"
    exit_code = parity.run(
        [
            "--library-dir",
            str(library_dir),
            "--cli-dir",
            str(cli_dir),
            "--artifacts",
            "module.ll",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any("digest mismatch" in failure for failure in payload["failures"])


def test_parity_supports_sha256_proxy_for_missing_object_artifact(tmp_path: Path) -> None:
    library_dir = tmp_path / "library"
    cli_dir = tmp_path / "cli"

    digest = "3" * 64
    _write(library_dir / "module.o.sha256", digest + "\n")
    _write(cli_dir / "module.o.sha256", digest + "\n")

    summary_out = tmp_path / "summary.json"
    exit_code = parity.run(
        [
            "--library-dir",
            str(library_dir),
            "--cli-dir",
            str(cli_dir),
            "--artifacts",
            "module.o",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["comparisons"][0]["source_kind"] == "sha256-proxy"


def test_parity_check_golden_detects_drift(tmp_path: Path) -> None:
    library_dir = tmp_path / "library"
    cli_dir = tmp_path / "cli"

    _write(library_dir / "module.manifest.json", '{"module":"m"}\n')
    _write(cli_dir / "module.manifest.json", '{"module":"m"}\n')

    summary_out = tmp_path / "summary.json"
    golden = tmp_path / "golden.json"
    write_code = parity.run(
        [
            "--library-dir",
            str(library_dir),
            "--cli-dir",
            str(cli_dir),
            "--artifacts",
            "module.manifest.json",
            "--summary-out",
            str(summary_out),
            "--golden-summary",
            str(golden),
            "--write-golden",
        ]
    )
    assert write_code == 0
    assert golden.is_file()

    _write(cli_dir / "module.manifest.json", '{"module":"m2"}\n')
    check_code = parity.run(
        [
            "--library-dir",
            str(library_dir),
            "--cli-dir",
            str(cli_dir),
            "--artifacts",
            "module.manifest.json",
            "--summary-out",
            str(summary_out),
            "--golden-summary",
            str(golden),
            "--check-golden",
        ]
    )

    assert check_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any("golden summary drift detected" in failure for failure in payload["failures"])


def test_parity_source_mode_generates_and_compares_cli_and_c_api_outputs(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    source = tmp_path / "sample.objc3"
    source.write_text("fn main() -> i32 { return 0; }\n", encoding="utf-8")
    cli_bin = tmp_path / "objc3c-native.exe"
    c_api_bin = tmp_path / "objc3c-frontend-c-api-runner.exe"
    cli_bin.write_text("stub\n", encoding="utf-8")
    c_api_bin.write_text("stub\n", encoding="utf-8")

    def _write_artifacts(command: list[str]) -> None:
        out_dir = Path(command[command.index("--out-dir") + 1])
        emit_prefix = command[command.index("--emit-prefix") + 1]
        _write_source_mode_artifacts(out_dir, emit_prefix=emit_prefix, marker="m")

    def _fake_run(command: list[str], **_: object) -> subprocess.CompletedProcess[str]:
        _write_artifacts(command)
        return subprocess.CompletedProcess(command, 0, stdout="ok\n", stderr="")

    monkeypatch.setattr(parity.subprocess, "run", _fake_run)

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
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["artifacts"] == [
        "module.diagnostics.json",
        "module.ll",
        "module.manifest.json",
        "module.obj",
    ]
    assert payload["execution"]["source"].endswith("sample.objc3")
    assert [item["role"] for item in payload["execution"]["commands"]] == [
        "cli",
        "c-api",
    ]


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


def test_parity_fixture_contract_matches_golden_and_is_replay_deterministic(
    tmp_path: Path,
) -> None:
    summary_a = tmp_path / "fixture_summary_a.json"
    summary_b = tmp_path / "fixture_summary_b.json"
    golden = FIXTURE_ROOT / "golden_summary.json"

    argv = [
        "--library-dir",
        str(FIXTURE_ROOT / "library"),
        "--cli-dir",
        str(FIXTURE_ROOT / "cli"),
        "--summary-out",
        str(summary_a),
        "--golden-summary",
        str(golden),
        "--check-golden",
    ]
    first_code = parity.run(argv)
    assert first_code == 0
    first_payload = json.loads(summary_a.read_text(encoding="utf-8"))
    assert first_payload == json.loads(golden.read_text(encoding="utf-8"))

    second_code = parity.run(
        [
            "--library-dir",
            str(FIXTURE_ROOT / "library"),
            "--cli-dir",
            str(FIXTURE_ROOT / "cli"),
            "--summary-out",
            str(summary_b),
            "--golden-summary",
            str(golden),
            "--check-golden",
        ]
    )
    assert second_code == 0
    assert first_payload == json.loads(summary_b.read_text(encoding="utf-8"))


def test_parity_reports_source_kind_mismatch(tmp_path: Path) -> None:
    library_dir = tmp_path / "library"
    cli_dir = tmp_path / "cli"

    (library_dir / "module.o").parent.mkdir(parents=True, exist_ok=True)
    (library_dir / "module.o").write_bytes(b"\x00OBJ")
    _write(cli_dir / "module.o.sha256", "1" * 64 + "\n")

    summary_out = tmp_path / "summary.json"
    exit_code = parity.run(
        [
            "--library-dir",
            str(library_dir),
            "--cli-dir",
            str(cli_dir),
            "--artifacts",
            "module.o",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert any(
        "source-kind mismatch for module.o" in failure
        for failure in payload["failures"]
    )


def test_parity_reports_invalid_proxy_digest_failure(tmp_path: Path) -> None:
    library_dir = tmp_path / "library"
    cli_dir = tmp_path / "cli"

    _write(library_dir / "module.o.sha256", "NOT-A-DIGEST\n")
    _write(cli_dir / "module.o.sha256", "3" * 64 + "\n")

    summary_out = tmp_path / "summary.json"
    exit_code = parity.run(
        [
            "--library-dir",
            str(library_dir),
            "--cli-dir",
            str(cli_dir),
            "--artifacts",
            "module.o",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert any(
        "library module.o: invalid sha256 proxy digest" in failure
        for failure in payload["failures"]
    )


def test_parity_reports_missing_cli_artifact_or_proxy(tmp_path: Path) -> None:
    library_dir = tmp_path / "library"
    cli_dir = tmp_path / "cli"

    _write(library_dir / "module.manifest.json", '{"module":"present"}\n')
    cli_dir.mkdir(parents=True, exist_ok=True)

    summary_out = tmp_path / "summary.json"
    exit_code = parity.run(
        [
            "--library-dir",
            str(library_dir),
            "--cli-dir",
            str(cli_dir),
            "--artifacts",
            "module.manifest.json",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert any(
        "cli module.manifest.json: missing artifact and proxy digest" in failure
        for failure in payload["failures"]
    )


@pytest.mark.parametrize(
    ("dimension_map", "message"),
    [
        ("diagnostics", "DIMENSION=ARTIFACT format"),
        ("unknown=module.ll", "unsupported dimension"),
        ("ir=", "artifact path must be non-empty"),
    ],
)
def test_parity_rejects_invalid_dimension_map_entries(
    tmp_path: Path,
    dimension_map: str,
    message: str,
) -> None:
    library_dir = tmp_path / "library"
    cli_dir = tmp_path / "cli"
    _write(library_dir / "module.ll", "define i32 @main() { ret i32 0 }\n")
    _write(cli_dir / "module.ll", "define i32 @main() { ret i32 0 }\n")

    with pytest.raises(ValueError, match=message):
        parity.run(
            [
                "--library-dir",
                str(library_dir),
                "--cli-dir",
                str(cli_dir),
                "--artifacts",
                "module.ll",
                "--dimension-map",
                dimension_map,
                "--summary-out",
                str(tmp_path / "summary.json"),
            ]
        )


def test_parity_check_golden_reports_invalid_json(tmp_path: Path) -> None:
    library_dir = tmp_path / "library"
    cli_dir = tmp_path / "cli"
    _write(library_dir / "module.manifest.json", '{"module":"m"}\n')
    _write(cli_dir / "module.manifest.json", '{"module":"m"}\n')
    golden = tmp_path / "golden.json"
    _write(golden, "{invalid-json}\n")

    summary_out = tmp_path / "summary.json"
    exit_code = parity.run(
        [
            "--library-dir",
            str(library_dir),
            "--cli-dir",
            str(cli_dir),
            "--artifacts",
            "module.manifest.json",
            "--summary-out",
            str(summary_out),
            "--golden-summary",
            str(golden),
            "--check-golden",
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any("golden summary parse error" in failure for failure in payload["failures"])


def test_parity_source_mode_reports_command_failures_with_execution_details(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
    capsys: pytest.CaptureFixture[str],
) -> None:
    source = tmp_path / "sample.objc3"
    source.write_text("fn main() -> i32 { return 0; }\n", encoding="utf-8")
    cli_bin = tmp_path / "objc3c-native.exe"
    c_api_bin = tmp_path / "objc3c-frontend-c-api-runner.exe"
    cli_bin.write_text("stub\n", encoding="utf-8")
    c_api_bin.write_text("stub\n", encoding="utf-8")

    def _fake_run(command: list[str], **_: object) -> subprocess.CompletedProcess[str]:
        out_dir = Path(command[command.index("--out-dir") + 1])
        emit_prefix = command[command.index("--emit-prefix") + 1]
        _write_source_mode_artifacts(out_dir, emit_prefix=emit_prefix, marker="cmd-fail")
        role = "cli" if "objc3c-native" in command[0] else "c-api"
        exit_code = 17 if role == "cli" else 23
        return subprocess.CompletedProcess(
            command,
            exit_code,
            stdout=f"{role}-stdout\n",
            stderr=f"{role}-stderr\n",
        )

    monkeypatch.setattr(parity.subprocess, "run", _fake_run)

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

    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    failures = payload["failures"]
    assert len(failures) == 2
    assert "objc3c-native.exe" in failures[0]
    assert "objc3c-frontend-c-api-runner.exe" in failures[1]

    commands = payload["execution"]["commands"]
    assert [item["role"] for item in commands] == ["cli", "c-api"]
    assert [item["exit_code"] for item in commands] == [17, 23]
    assert [item["stdout"] for item in commands] == ["cli-stdout\n", "c-api-stdout\n"]
    assert [item["stderr"] for item in commands] == ["cli-stderr\n", "c-api-stderr\n"]
