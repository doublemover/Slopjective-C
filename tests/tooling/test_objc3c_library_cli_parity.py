from __future__ import annotations

import importlib.util
import json
import subprocess
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_objc3c_library_cli_parity.py"
SPEC = importlib.util.spec_from_file_location("check_objc3c_library_cli_parity", SCRIPT_PATH)
assert SPEC is not None and SPEC.loader is not None
parity = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = parity
SPEC.loader.exec_module(parity)


def _write(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


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
        out_dir.mkdir(parents=True, exist_ok=True)
        _write(out_dir / f"{emit_prefix}.diagnostics.json", '{"diagnostics":[]}\n')
        _write(out_dir / f"{emit_prefix}.manifest.json", '{"module":"m"}\n')
        _write(out_dir / f"{emit_prefix}.ll", "define i32 @main() { ret i32 0 }\n")
        (out_dir / f"{emit_prefix}.obj").write_bytes(b"\x00OBJ")

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
