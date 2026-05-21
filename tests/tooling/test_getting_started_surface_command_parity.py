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

    with pytest.raises(RuntimeError, match="canonical npm workflow prefix"):
        checker.extract_fenced_public_command_records((surface,))


def test_fenced_public_commands_reject_extra_separator_after_action(
    tmp_path: Path, monkeypatch: pytest.MonkeyPatch
) -> None:
    surface = tmp_path / "surface.md"
    surface.write_text(
        "\n".join(
            [
                "```sh",
                "npm run objc3c -- compile-objc3c -- showcase/auroraBoard/main.objc3",
                "```",
            ]
        ),
        encoding="utf-8",
    )
    monkeypatch.setattr(checker, "repo_rel", lambda path: Path(path).name)

    with pytest.raises(RuntimeError, match="extra separator after the workflow action"):
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

    with pytest.raises(RuntimeError, match="extra separator after the workflow action"):
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


def test_first_run_workflow_contract_validates_public_docs_and_walkthrough(
    tmp_path: Path, monkeypatch: pytest.MonkeyPatch
) -> None:
    source = tmp_path / "showcase" / "auroraBoard" / "main.objc3"
    source.parent.mkdir(parents=True)
    source.write_text("module AuroraBoard;\n", encoding="utf-8")
    contract = {
        "contract_id": "objc3c.developer.first-run.workflow.v1",
        "schema_version": 1,
        "package_bridge": {
            "expected_script": "python -m scripts.objc3c_workflow",
            "public_command_prefix": ["npm", "run", "objc3c", "--"],
        },
        "required_public_actions": [
            "build-native-binaries",
            "compile-objc3c",
            "materialize-project-template",
        ],
        "required_fenced_doc_commands": [
            {
                "source_path": "docs/tutorials/getting_started.md",
                "command": "npm run objc3c -- compile-objc3c showcase/auroraBoard/main.objc3",
            }
        ],
        "required_walkthrough_steps": ["compile-auroraBoard"],
        "first_compile": {
            "example_id": "auroraBoard",
            "source": "showcase/auroraBoard/main.objc3",
            "workflow_action": "compile-objc3c",
            "walkthrough_step_id": "compile-auroraBoard",
            "artifact_root": "tmp/artifacts/showcase/auroraBoard",
        },
    }
    package_payload = {"scripts": {"objc3c": "python -m scripts.objc3c_workflow"}}
    records = [
        checker.PublicCommandRecord(
            source_path="docs/tutorials/getting_started.md",
            line=12,
            command="npm run objc3c -- compile-objc3c showcase/auroraBoard/main.objc3",
            action="compile-objc3c",
            source_kind="fenced-doc-command",
        )
    ]
    walkthrough = {
        "steps": [
            {
                "id": "compile-auroraBoard",
                "workflow_action": "compile-objc3c",
                "public_command": "npm run objc3c -- compile-objc3c showcase/auroraBoard/main.objc3",
                "artifact_root": "tmp/artifacts/showcase/auroraBoard",
            }
        ]
    }
    monkeypatch.setattr(
        checker,
        "public_workflow_action_names",
        lambda: [
            "build-native-binaries",
            "compile-objc3c",
            "materialize-project-template",
        ],
    )
    monkeypatch.setattr(checker, "repo_rel", lambda path, root=None: str(Path(path)))

    payload = checker.validate_first_run_workflow_contract(
        contract=contract,
        package_payload=package_payload,
        public_command_records=records,
        walkthrough=walkthrough,
        root=tmp_path,
    )

    assert payload["status"] == "PASS"
    assert payload["required_fenced_doc_command_count"] == 1


def test_first_run_workflow_contract_reports_missing_doc_commands(tmp_path: Path) -> None:
    source = tmp_path / "showcase" / "auroraBoard" / "main.objc3"
    source.parent.mkdir(parents=True)
    source.write_text("module AuroraBoard;\n", encoding="utf-8")
    contract = {
        "contract_id": "objc3c.developer.first-run.workflow.v1",
        "schema_version": 1,
        "package_bridge": {
            "expected_script": "python -m scripts.objc3c_workflow",
            "public_command_prefix": ["npm", "run", "objc3c", "--"],
        },
        "required_public_actions": [],
        "required_fenced_doc_commands": [
            {
                "source_path": "README.md",
                "command": "npm run objc3c -- build-native-binaries",
            }
        ],
        "required_walkthrough_steps": ["compile-auroraBoard"],
        "first_compile": {
            "example_id": "auroraBoard",
            "source": "showcase/auroraBoard/main.objc3",
            "workflow_action": "compile-objc3c",
            "walkthrough_step_id": "compile-auroraBoard",
            "artifact_root": "tmp/artifacts/showcase/auroraBoard",
        },
    }
    walkthrough = {
        "steps": [
            {
                "id": "compile-auroraBoard",
                "workflow_action": "compile-objc3c",
                "public_command": "npm run objc3c -- compile-objc3c showcase/auroraBoard/main.objc3",
                "artifact_root": "tmp/artifacts/showcase/auroraBoard",
            }
        ]
    }

    with pytest.raises(RuntimeError, match="required doc commands are missing"):
        checker.validate_first_run_workflow_contract(
            contract=contract,
            package_payload={"scripts": {"objc3c": "python -m scripts.objc3c_workflow"}},
            public_command_records=[],
            walkthrough=walkthrough,
            root=tmp_path,
        )


def test_developer_experience_completion_contract_validates_examples_and_diagnostics() -> None:
    contract = checker.load_json(checker.DEVELOPER_EXPERIENCE_CONTRACT_PATH)
    migration_commands = contract["migration_examples"]["required_fenced_doc_commands"]
    records = [
        checker.PublicCommandRecord(
            source_path=entry["source_path"],
            line=1,
            command=entry["command"],
            action=checker._parse_public_command(entry["command"])[0],
            source_kind="fenced-doc-command",
        )
        for entry in migration_commands
    ]

    payload = checker.validate_developer_experience_completion_contract(
        contract=contract,
        public_command_records=records,
    )

    assert payload["status"] == "PASS"
    assert payload["issue_ids"] == [8157]
    assert payload["template_compile_contract"]["compile_action"] == "compile-objc3c"
    assert payload["migration_examples"]["automatic_edit_count"] >= 6
    assert payload["diagnostic_fixit_metadata"]["machine_applicable_fixit_count"] >= 2
