from __future__ import annotations

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
WORKFLOW_ROOT = ROOT / ".github" / "workflows"
ALLOWED_NON_OBJC3C_RUN_COMMANDS = {
    "npm ci",
    "python -m pip install --upgrade jsonschema",
    "python -m pip install --upgrade pytest",
}


def _workflow_run_commands(path: Path) -> list[str]:
    commands: list[str] = []
    for line in path.read_text(encoding="utf-8").splitlines():
        stripped = line.strip()
        if stripped.startswith("run: "):
            commands.append(stripped.removeprefix("run: ").strip())
    return commands


def test_github_workflows_route_objc3c_commands_through_package_bridge() -> None:
    workflow_paths = sorted(WORKFLOW_ROOT.glob("*.yml"))
    assert workflow_paths, "expected checked-in GitHub workflows"

    for workflow_path in workflow_paths:
        for command in _workflow_run_commands(workflow_path):
            assert "cmd.exe" not in command.lower(), (workflow_path, command)
            assert "cmd /c" not in command.lower(), (workflow_path, command)
            assert "scripts/" not in command and "scripts\\" not in command, (
                workflow_path,
                command,
            )
            if command in ALLOWED_NON_OBJC3C_RUN_COMMANDS:
                continue
            assert command.startswith("npm run objc3c -- "), (
                workflow_path,
                command,
            )
            assert "lint-default" not in command, (workflow_path, command)
