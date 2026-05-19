from __future__ import annotations

from pathlib import Path

from hard_cutover_gate_support import (
    active_path_patterns,
    active_pattern_ids,
    write_fixture,
)
from scripts.source_hygiene.scanner import build_report


def assert_retired_public_workflow_runner_path_rejected(tmp_path: Path) -> None:
    retired_runner = "objc3c_public" + "_workflow_runner.py"
    write_fixture(
        tmp_path / "docs/runbooks/commands.md",
        f"Run python scripts/{retired_runner} test-fast.\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == (
        "retired-public-workflow-runner"
    )


def assert_retired_npm_workflow_aliases_rejected(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "docs/runbooks/commands.md",
        "Run npm run test:fast.\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "retired-npm-workflow-command"


def assert_direct_native_compile_wrapper_commands_rejected(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "docs/objc3c-native.md",
        "Run pwsh -NoProfile -File scripts/objc3c_native_compile.ps1 sample.objc3.\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == (
        "direct-native-compile-wrapper-command"
    )


def assert_native_compile_wrapper_as_source_anchor_allowed(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "docs/objc3c-native.md",
        "- implementation anchor: `scripts/objc3c_native_compile.ps1`\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["ok"] is True
    assert report["stats"]["active_finding_count"] == 0


def assert_retired_package_alias_metadata_rejected(tmp_path: Path) -> None:
    snake_alias = "public" + "_scripts"
    camel_alias = "public" + "Scripts"
    camel_alias_list = "public" + "ScriptAliases"
    pattern_id = "retired-" + "public" + "-script" + "-alias-metadata"

    write_fixture(
        tmp_path / "docs/support/capability_matrix.md",
        f"The {snake_alias} alias table remains authoritative.\n",
    )
    write_fixture(
        tmp_path / "site/src/index.body.md",
        f"The {camel_alias} metadata field is still displayed.\n",
    )
    write_fixture(
        tmp_path / "stdlib/workspace.json",
        '{"' + camel_alias_list + '": ["test:fast"]}\n',
    )

    report = build_report(
        root=tmp_path,
        scan_roots=("docs", "site", "stdlib"),
        excludes=(),
    )

    assert report["ok"] is False
    assert active_pattern_ids(report) == [
        pattern_id,
        pattern_id,
        pattern_id,
    ]


def assert_direct_runner_command_variables_rejected(tmp_path: Path) -> None:
    workflow_directory = '"scripts" / "' + "objc3c_workflow" + '"'
    runner_name = '"runner' + '.py"'
    write_fixture(
        tmp_path / "scripts/check_packaged_surface.py",
        f"PUBLIC_RUNNER = ROOT / {workflow_directory} / {runner_name}\n",
    )

    report = build_report(root=tmp_path, scan_roots=("scripts",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == (
        "direct-retired-workflow-runner-command-source"
    )


def assert_packaged_runner_direct_commands_rejected(tmp_path: Path) -> None:
    workflow_directory = '"scripts" / "' + "objc3c_workflow" + '"'
    runner_name = '"runner' + '.py"'
    write_fixture(
        tmp_path / "tests/tooling/test_packaged_surface.py",
        (
            f"packaged_runner = package_root / {workflow_directory} / {runner_name}\n"
            'run_capture([sys.executable, str(packaged_runner), "lint"])\n'
        ),
    )

    report = build_report(root=tmp_path, scan_roots=("tests",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == (
        "direct-retired-workflow-runner-command-source"
    )


def assert_retired_workflow_registry_facade_rejected(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "docs/runbooks/commands.md",
        "Action source: scripts/objc3c_workflow/registry.py\n",
    )
    write_fixture(
        tmp_path / "scripts/objc3c_workflow/actions/docs.py",
        "from ..registry import ACTION_SPECS\n",
    )
    write_fixture(
        tmp_path / "tests/tooling/test_objc3c_workflow_runner_decomposition.py",
        "from scripts.objc3c_workflow.registry import ACTION_SPECS\n",
    )

    report = build_report(
        root=tmp_path,
        scan_roots=("docs", "scripts", "tests"),
        excludes=(),
    )

    assert report["ok"] is False
    assert active_path_patterns(report) == {
        "docs/runbooks/commands.md": "retired-workflow-action-registry-facade",
        "scripts/objc3c_workflow/actions/docs.py": (
            "retired-workflow-action-registry-facade"
        ),
        "tests/tooling/test_objc3c_workflow_runner_decomposition.py": (
            "retired-workflow-action-registry-facade"
        ),
    }
