from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "materialize_objc3c_project_template.py"
SPEC = importlib.util.spec_from_file_location("materialize_objc3c_project_template", SCRIPT_PATH)
assert SPEC is not None and SPEC.loader is not None
materializer = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = materializer
SPEC.loader.exec_module(materializer)


def test_materializer_writes_template_and_harness(tmp_path: Path, monkeypatch) -> None:
    root = tmp_path
    portfolio_path = root / "showcase" / "portfolio.json"
    showcase_source = root / "showcase" / "auroraBoard" / "main.objc3"
    showcase_source.parent.mkdir(parents=True, exist_ok=True)
    showcase_source.write_text("module AuroraBoard;\n", encoding="utf-8")
    showcase_workspace = root / "showcase" / "auroraBoard" / "workspace.json"
    showcase_workspace.write_text('{"name":"auroraBoard"}\n', encoding="utf-8")
    portfolio_path.write_text(
        json.dumps(
            {
                "examples": [
                    {
                        "id": "auroraBoard",
                        "source": "showcase/auroraBoard/main.objc3",
                        "workspace_manifest": "showcase/auroraBoard/workspace.json",
                    }
                ]
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )

    monkeypatch.setattr(materializer, "ROOT", root)
    monkeypatch.setattr(materializer, "PORTFOLIO", portfolio_path)
    monkeypatch.setattr(
        materializer,
        "TEMPLATE_ARTIFACT_ROOT",
        root / "tmp" / "artifacts" / "project-template",
    )
    monkeypatch.setattr(
        materializer,
        "TEMPLATE_REPORT_ROOT",
        root / "tmp" / "reports" / "project-template",
    )

    def fake_run_step(name: str, command: list[str]) -> dict[str, object]:
        stdout = ""
        if name == "compile-template-source":
            assert command == [
                "npm",
                "run",
                "objc3c",
                "--",
                "compile-objc3c",
                "tmp/artifacts/project-template/auroraBoard/src/main.objc3",
                "--out-dir",
                "tmp/artifacts/project-template/auroraBoard/build",
                "--emit-prefix",
                "module",
            ]
        elif name == "inspect-bonus-tool-integration":
            stdout = "summary_path: tmp/reports/objc3c-public-workflow/bonus-tool-integration.json\n"
        elif name == "materialize-playground-workspace":
            stdout = "workspace_path: tmp/artifacts/playground/auroraBoard/workspace.json\n"
        elif name == "benchmark-runtime-inspector":
            stdout = "summary_path: tmp/reports/objc3c-public-workflow/runtime-inspector-benchmark.json\n"
        return {
            "name": name,
            "command": command,
            "exit_code": 0,
            "stdout": stdout,
        }

    monkeypatch.setattr(materializer, "run_step", fake_run_step)
    monkeypatch.setattr(
        sys,
        "argv",
        ["materialize_objc3c_project_template.py", "--example", "auroraBoard"],
    )

    exit_code = materializer.main()

    template_path = (
        root
        / "tmp"
        / "artifacts"
        / "project-template"
        / "auroraBoard"
        / "template.json"
    )
    harness_path = (
        root
        / "tmp"
        / "reports"
        / "project-template"
        / "auroraBoard"
        / "demo-harness.json"
    )
    assert exit_code == 0
    assert template_path.is_file()
    assert harness_path.is_file()
    template_payload = json.loads(template_path.read_text(encoding="utf-8"))
    harness_payload = json.loads(harness_path.read_text(encoding="utf-8"))
    assert template_payload["contract_id"] == "objc3c.project.template.surface.v1"
    assert template_payload["workspace_origin"] == "showcase/auroraBoard/workspace.json"
    assert template_payload["template_workspace_manifest"] == (
        "tmp/artifacts/project-template/auroraBoard/workspace.json"
    )
    assert (
        root / "tmp" / "artifacts" / "project-template" / "auroraBoard" / "workspace.json"
    ).is_file()
    assert template_payload["application_architecture_testing_contracts"] == {
        "first_party_testing": "tests/tooling/fixtures/application_architecture_testing/first_party_testing_semantics.json",
        "project_template_workspace": "tests/tooling/fixtures/application_architecture_testing/project_template_workspace_semantics.json",
        "canonical_application_architecture": "tests/tooling/fixtures/application_architecture_testing/canonical_application_architecture_semantics.json",
    }
    assert template_payload["template_compile_contract"] == {
        "compile_action": "compile-objc3c",
        "source": "tmp/artifacts/project-template/auroraBoard/src/main.objc3",
        "artifact_root": "tmp/artifacts/project-template/auroraBoard/build",
        "emit_prefix": "module",
        "public_command": (
            "npm run objc3c -- compile-objc3c "
            "tmp/artifacts/project-template/auroraBoard/src/main.objc3 "
            "--out-dir tmp/artifacts/project-template/auroraBoard/build "
            "--emit-prefix module"
        ),
        "expected_artifacts": [
            "module.obj",
            "module.ll",
            "module.manifest.json",
            "module.runtime-registration-manifest.json",
        ],
    }
    assert "compile-objc3c" in template_payload["public_actions"]
    assert template_payload["recommended_validation_actions"] == [
        "validate-showcase",
        "validate-runnable-showcase",
        "validate-stdlib-program",
        "validate-runnable-stdlib-program",
    ]
    clean_room = template_payload["clean_room_usability"]
    assert clean_room["support_claim"] == "objc3c.behavior.tooling.first-run-product-path"
    assert "showcase/auroraBoard/workspace.json" in clean_room["source_truth_paths"]
    assert all(
        path.startswith("tmp/") for path in clean_room["generated_output_paths"]
    )
    assert any(
        step["stage"] == "inspect"
        and step["command"]
        == "npm run objc3c -- inspect-compile-observability tmp/artifacts/project-template/auroraBoard/src/main.objc3"
        for step in clean_room["normal_developer_loop"]
    )
    assert harness_payload["contract_id"] == "objc3c.project.template.demo.harness.v1"
    assert harness_payload["ok"] is True
    assert harness_payload["template_workspace_manifest"] == (
        "tmp/artifacts/project-template/auroraBoard/workspace.json"
    )
    assert harness_payload["compile_step"]["name"] == "compile-template-source"
    assert harness_payload["compile_step"]["exit_code"] == 0
