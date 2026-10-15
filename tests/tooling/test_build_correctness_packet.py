from __future__ import annotations

import json
from pathlib import Path

from scripts.objc3c_workflow.actions.validation_timing_changed_paths import select_validation_profiles
from scripts.objc3c_workflow.composite_step_runtime_reuse import runtime_acceptance_reuse_step


def test_changed_paths_classify_new_frontend_source_areas() -> None:
    payload = select_validation_profiles(["native/objc3c/src/lex/new_lexer.cpp", "schemas/compiler.json"])
    assert "lowering" in {row["profile"] for row in payload["profiles"]}


def test_unknown_changed_paths_force_repo_profile() -> None:
    payload = select_validation_profiles(["*"])
    assert "repo" in {row["profile"] for row in payload["profiles"]}


def test_unknown_paths_widen_mixed_known_selection() -> None:
    payload = select_validation_profiles(["native/objc3c/src/lex/new_lexer.cpp",
                                          "unknown-source-area/new.contract"])
    assert "repo" in {row["profile"] for row in payload["profiles"]}


def test_runtime_acceptance_skip_requires_real_matching_report(monkeypatch, tmp_path: Path) -> None:
    monkeypatch.setenv("OBJC3C_SKIP_RUNTIME_ACCEPTANCE_RERUN", "1")
    monkeypatch.setenv("OBJC3C_RUNTIME_ACCEPTANCE_EXPECTED_FIXTURES", "fixture-a")
    step = runtime_acceptance_reuse_step("test-runtime-acceptance-fast", ["python", "runner.py"], 0.0)
    assert step is None


def test_runtime_report_without_identity_is_not_reused() -> None:
    # Missing build/runtime/options/fixture provenance is always a cache miss.
    from scripts.objc3c_workflow.composite_step_runtime_reuse import _valid_runtime_report
    import tempfile
    with tempfile.TemporaryDirectory() as directory:
        report = Path(directory) / "summary.json"
        report.write_text(json.dumps({"final_status": "passed", "fixture_ids": ["x"], "options_digest": "abc"}), encoding="utf-8")
        assert _valid_runtime_report(report, {"x"})[0] is False
