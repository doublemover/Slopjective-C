from pathlib import Path

import pytest

from scripts.objc3c_runtime_debug_trace import cli


def test_prototype_trace_reports_retirement_before_executing(tmp_path: Path, monkeypatch, capsys) -> None:
    monkeypatch.setattr(cli, "RUNTIME_TRACE_SOURCE_CONTRACT_HEADER", tmp_path / "missing.h")

    def unexpected_replay(**kwargs):
        pytest.fail(f"retired trace attempted to execute {kwargs}")

    monkeypatch.setattr(cli, "build_runtime_debug_trace_from_live_actions", unexpected_replay)
    trace = tmp_path / "trace.json"

    assert cli.main(["--trace-out", str(trace)]) == 1
    assert "retired with the prototype runtime" in capsys.readouterr().err
    assert not trace.exists()
