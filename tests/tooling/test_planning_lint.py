from __future__ import annotations

import importlib.util
import io
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

SCRIPT_PATH = Path(__file__).resolve().parents[2] / "scripts" / "planning_lint.py"
SCRIPTS_DIR = SCRIPT_PATH.parent
if str(SCRIPTS_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_DIR))

SPEC = importlib.util.spec_from_file_location("planning_lint_for_tests", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/planning_lint.py")

planning_lint = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = planning_lint
SPEC.loader.exec_module(planning_lint)


def run_main(args: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = planning_lint.main(args)
    return code, stdout.getvalue(), stderr.getvalue()


def test_main_returns_2_when_scope_is_empty(monkeypatch) -> None:
    monkeypatch.setattr(planning_lint, "resolve_scope", lambda *_: [])

    code, stdout, stderr = run_main([])

    assert code == 2
    assert stdout == ""
    assert "planning-lint: no files matched include/exclude scope" in stderr


def test_main_returns_0_when_all_checks_pass(monkeypatch) -> None:
    monkeypatch.setattr(
        planning_lint,
        "resolve_scope",
        lambda *_: [Path("spec/planning/example.md")],
    )
    monkeypatch.setattr(planning_lint, "run_structural_lint", lambda _paths: 0)
    monkeypatch.setattr(planning_lint, "run_placeholders_lint", lambda: 0)
    monkeypatch.setattr(planning_lint, "run_unchecked_checkboxes_lint", lambda *_: 0)

    code, stdout, stderr = run_main([])

    assert code == 0
    assert stderr == ""
    assert "planning-lint summary: structural=0 placeholders=0 unchecked_checkboxes=0" in stdout


def test_main_returns_1_when_any_check_reports_findings(monkeypatch) -> None:
    monkeypatch.setattr(
        planning_lint,
        "resolve_scope",
        lambda *_: [Path("spec/planning/example.md")],
    )
    monkeypatch.setattr(planning_lint, "run_structural_lint", lambda _paths: 0)
    monkeypatch.setattr(planning_lint, "run_placeholders_lint", lambda: 1)
    monkeypatch.setattr(planning_lint, "run_unchecked_checkboxes_lint", lambda *_: 0)

    code, stdout, stderr = run_main([])

    assert code == 1
    assert stderr == ""
    assert "planning-lint summary: structural=0 placeholders=1 unchecked_checkboxes=0" in stdout


def test_main_returns_2_when_any_check_returns_runtime_failure(monkeypatch) -> None:
    monkeypatch.setattr(
        planning_lint,
        "resolve_scope",
        lambda *_: [Path("spec/planning/example.md")],
    )
    monkeypatch.setattr(planning_lint, "run_structural_lint", lambda _paths: 0)
    monkeypatch.setattr(planning_lint, "run_placeholders_lint", lambda: 0)
    monkeypatch.setattr(planning_lint, "run_unchecked_checkboxes_lint", lambda *_: 2)

    code, stdout, stderr = run_main([])

    assert code == 2
    assert stderr == ""
    assert "planning-lint summary: structural=0 placeholders=0 unchecked_checkboxes=2" in stdout


def test_main_returns_2_when_any_check_raises_runtime_failure(monkeypatch) -> None:
    monkeypatch.setattr(
        planning_lint,
        "resolve_scope",
        lambda *_: [Path("spec/planning/example.md")],
    )
    monkeypatch.setattr(planning_lint, "run_structural_lint", lambda _paths: 0)
    monkeypatch.setattr(planning_lint, "run_placeholders_lint", lambda: 0)

    def raise_runtime_failure(*_args) -> int:
        raise RuntimeError("boom")

    monkeypatch.setattr(
        planning_lint,
        "run_unchecked_checkboxes_lint",
        raise_runtime_failure,
    )

    code, stdout, stderr = run_main([])

    assert code == 2
    assert (
        "planning-lint: planning unchecked checkbox lint raised an unexpected exception: boom"
        in stderr
    )
    assert "planning-lint summary: structural=0 placeholders=0 unchecked_checkboxes=2" in stdout


def test_run_placeholders_lint_uses_json_mode(monkeypatch) -> None:
    captured: list[list[str]] = []

    def fake_main(args: list[str]) -> int:
        captured.append(args)
        return 0

    monkeypatch.setattr(planning_lint.check_planning_placeholders, "main", fake_main)

    code = planning_lint.run_placeholders_lint()

    assert code == 0
    assert captured == [["--format", "json"]]


def test_run_unchecked_checkboxes_lint_uses_json_mode(monkeypatch) -> None:
    captured: list[list[str]] = []

    def fake_main(args: list[str]) -> int:
        captured.append(args)
        return 0

    monkeypatch.setattr(
        planning_lint.check_planning_unchecked_checkboxes,
        "main",
        fake_main,
    )

    code = planning_lint.run_unchecked_checkboxes_lint(
        ("spec/planning/custom/**/*.md",),
        ("spec/planning/archive/**", "spec/planning/generated/**"),
    )

    assert code == 0
    assert captured == [
        [
            "--format",
            "json",
            "--glob",
            "spec/planning/custom/**/*.md",
            "--exclude",
            "spec/planning/archive/**",
            "--exclude",
            "spec/planning/generated/**",
        ]
    ]
