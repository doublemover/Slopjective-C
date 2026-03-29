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
    portfolio_path.write_text(
        json.dumps(
            {
                "examples": [
                    {
                        "id": "auroraBoard",
                        "source": "showcase/auroraBoard/main.objc3",
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
    monkeypatch.setattr(materializer, "PUBLIC_RUNNER", root / "scripts" / "objc3c_public_workflow_runner.py")
    monkeypatch.setattr(materializer, "TEMPLATE_ARTIFACT_ROOT", root / "tmp" / "artifacts" / "project-template")
    monkeypatch.setattr(materializer, "TEMPLATE_REPORT_ROOT", root / "tmp" / "reports" / "project-template")

    def fake_run_step(name: str, command: list[str]) -> dict[str, object]:
        stdout = ""
        if name == "inspect-bonus-tool-integration":
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
    monkeypatch.setattr(sys, "argv", ["materialize_objc3c_project_template.py", "--example", "auroraBoard"])

    exit_code = materializer.main()

    template_path = root / "tmp" / "artifacts" / "project-template" / "auroraBoard" / "template.json"
    harness_path = root / "tmp" / "reports" / "project-template" / "auroraBoard" / "demo-harness.json"
    assert exit_code == 0
    assert template_path.is_file()
    assert harness_path.is_file()
    template_payload = json.loads(template_path.read_text(encoding="utf-8"))
    harness_payload = json.loads(harness_path.read_text(encoding="utf-8"))
    assert template_payload["contract_id"] == "objc3c.project.template.surface.v1"
    assert harness_payload["contract_id"] == "objc3c.project.template.demo.harness.v1"
    assert harness_payload["ok"] is True
