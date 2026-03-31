from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "materialize_objc3c_canonical_application_workspace.py"
SPEC = importlib.util.spec_from_file_location(
    "materialize_objc3c_canonical_application_workspace",
    SCRIPT_PATH,
)
assert SPEC is not None and SPEC.loader is not None
materializer = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = materializer
SPEC.loader.exec_module(materializer)


def test_materializer_writes_canonical_workspace_and_summary(
    tmp_path: Path, monkeypatch
) -> None:
    root = tmp_path
    contract_path = (
        root
        / "tests"
        / "tooling"
        / "fixtures"
        / "application_architecture_testing"
        / "canonical_application_architecture_semantics.json"
    )
    portfolio_path = root / "showcase" / "portfolio.json"
    stdlib_program_surface_path = root / "stdlib" / "program_surface.json"
    contract_path.parent.mkdir(parents=True, exist_ok=True)
    portfolio_path.parent.mkdir(parents=True, exist_ok=True)
    stdlib_program_surface_path.parent.mkdir(parents=True, exist_ok=True)

    contract_path.write_text(
        json.dumps(
            {
                "architecture_layers": [{"id": "entry"}, {"id": "domain"}],
                "required_evidence_actions": ["validate-showcase", "package-runnable-toolchain"],
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    portfolio_path.write_text(
        json.dumps(
            {
                "examples": [
                    {
                        "id": "auroraBoard",
                        "source": "showcase/auroraBoard/main.objc3",
                        "workspace_manifest": "showcase/auroraBoard/workspace.json",
                        "story_capabilities": ["reflection"],
                        "stdlib_followup_modules": ["objc3.core"],
                    }
                ]
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    stdlib_program_surface_path.write_text(
        json.dumps({"publish_model": "shared-runnable-toolchain"}, indent=2) + "\n",
        encoding="utf-8",
    )

    example_root = root / "showcase" / "auroraBoard"
    example_root.mkdir(parents=True, exist_ok=True)
    (example_root / "main.objc3").write_text("module AuroraBoard;\n", encoding="utf-8")
    (example_root / "workspace.json").write_text(
        json.dumps({"contract_id": "objc3c.showcase.example.workspace.v1"}, indent=2)
        + "\n",
        encoding="utf-8",
    )
    (root / "stdlib" / "workspace.json").write_text(
        json.dumps({"contract_id": "objc3c.stdlib.workspace.v1"}, indent=2) + "\n",
        encoding="utf-8",
    )

    monkeypatch.setattr(materializer, "ROOT", root)
    monkeypatch.setattr(materializer, "CONTRACT_PATH", contract_path)
    monkeypatch.setattr(materializer, "PORTFOLIO_PATH", portfolio_path)
    monkeypatch.setattr(materializer, "STDLIB_PROGRAM_SURFACE_PATH", stdlib_program_surface_path)
    monkeypatch.setattr(
        materializer,
        "DEFAULT_OUTPUT_ROOT",
        root / "tmp" / "artifacts" / "application-architecture-testing" / "canonical-workspace",
    )
    monkeypatch.setattr(
        materializer,
        "SUMMARY_PATH",
        root
        / "tmp"
        / "reports"
        / "application-architecture-testing"
        / "canonical-application-workspace-summary.json",
    )
    monkeypatch.setattr(sys, "argv", ["materialize_objc3c_canonical_application_workspace.py"])

    exit_code = materializer.main()

    assert exit_code == 0
    workspace_manifest = (
        root / "tmp" / "artifacts" / "application-architecture-testing" / "canonical-workspace" / "workspace.json"
    )
    assert workspace_manifest.is_file()
    summary_payload = json.loads(materializer.SUMMARY_PATH.read_text(encoding="utf-8"))
    workspace_payload = json.loads(workspace_manifest.read_text(encoding="utf-8"))
    assert summary_payload["status"] == "PASS"
    assert workspace_payload["contract_id"] == "objc3c.application.architecture.testing.canonical_workspace.v1"
    assert summary_payload["example_count"] == 1
