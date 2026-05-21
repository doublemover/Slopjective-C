from __future__ import annotations

from pathlib import Path
import sys

import pytest

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))

from scripts import check_getting_started_surface as checker


def test_extract_fenced_public_command_records_ignores_templates_and_inline_commands(
    tmp_path: Path, monkeypatch: pytest.MonkeyPatch
) -> None:
    surface = tmp_path / "surface.md"
    surface.write_text(
        "\n".join(
            [
                "`npm run objc3c -- inline-command`",
                "```sh",
                "npm run objc3c -- build-native-binaries",
                "npm run objc3c -- <action>",
                "```",
            ]
        ),
        encoding="utf-8",
    )
    monkeypatch.setattr(checker, "repo_rel", lambda path: Path(path).name)

    records = checker.extract_fenced_public_command_records((surface,))

    assert [(record.command, record.action) for record in records] == [
        ("npm run objc3c -- build-native-binaries", "build-native-binaries")
    ]


def test_extract_fenced_public_command_records_rejects_noncanonical_prefix(
    tmp_path: Path, monkeypatch: pytest.MonkeyPatch
) -> None:
    surface = tmp_path / "surface.md"
    surface.write_text(
        "\n".join(
            [
                "```sh",
                "npm run objc3c compile-objc3c showcase/auroraBoard/main.objc3",
                "```",
            ]
        ),
        encoding="utf-8",
    )
    monkeypatch.setattr(checker, "repo_rel", lambda path: Path(path).name)

    with pytest.raises(ValueError, match="canonical npm workflow prefix"):
        checker.extract_fenced_public_command_records((surface,))


def test_walkthrough_public_commands_reject_extra_separator_after_action() -> None:
    walkthrough = {
        "steps": [
            {
                "workflow_action": "compile-objc3c",
                "public_command": "npm run objc3c -- compile-objc3c -- showcase/auroraBoard/main.objc3",
            }
        ]
    }

    with pytest.raises(RuntimeError, match="extra separator"):
        checker.extract_walkthrough_public_command_records(walkthrough)


def test_public_command_parity_requires_registered_actions(
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    monkeypatch.setattr(
        checker,
        "public_workflow_action_names",
        lambda: ["build-native-binaries"],
    )
    records = [
        checker.PublicCommandRecord(
            source_path="README.md",
            line=12,
            command="npm run objc3c -- missing-action",
            action="missing-action",
            source_kind="fenced-doc-command",
        )
    ]

    with pytest.raises(RuntimeError, match="unregistered workflow actions"):
        checker.validate_public_command_parity(records)
