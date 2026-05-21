from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_application_architecture_template_harness.py"
SPEC = importlib.util.spec_from_file_location(
    "check_application_architecture_template_harness",
    SCRIPT_PATH,
)
assert SPEC is not None and SPEC.loader is not None
checker = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = checker
SPEC.loader.exec_module(checker)


def test_template_harness_check_writes_summary(tmp_path: Path, monkeypatch) -> None:
    root = tmp_path
    contract_path = (
        root
        / "tests"
        / "tooling"
        / "fixtures"
        / "application_architecture_testing"
        / "project_template_workspace_semantics.json"
    )
    contract_path.parent.mkdir(parents=True, exist_ok=True)
    contract_path.write_text(
        json.dumps(
            {
                "template_contract_id": "objc3c.project.template.surface.v1",
                "template_harness_contract_id": "objc3c.project.template.demo.harness.v1",
                "required_actions": [
                    "materialize-project-template",
                    "compile-objc3c",
                    "materialize-playground-workspace",
                    "benchmark-runtime-inspector",
                    "inspect-bonus-tool-integration",
                ],
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )

    template_path = root / "tmp" / "artifacts" / "project-template" / "auroraBoard" / "template.json"
    harness_path = root / "tmp" / "reports" / "project-template" / "auroraBoard" / "demo-harness.json"
    for path in (
        root / "tmp" / "reports" / "objc3c-public-workflow" / "bonus-tool-integration.json",
        root / "tmp" / "artifacts" / "playground" / "auroraBoard" / "workspace.json",
        root / "tmp" / "reports" / "objc3c-public-workflow" / "runtime-inspector-benchmark.json",
    ):
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text("{}\n", encoding="utf-8")
    template_path.parent.mkdir(parents=True, exist_ok=True)
    harness_path.parent.mkdir(parents=True, exist_ok=True)
    template_path.write_text(
        json.dumps(
            {
                "contract_id": "objc3c.project.template.surface.v1",
                "application_architecture_testing_contracts": {
                    "project_template_workspace": "tests/tooling/fixtures/application_architecture_testing/project_template_workspace_semantics.json"
                },
                "source_origin": "showcase/auroraBoard/main.objc3",
                "template_compile_contract": {
                    "compile_action": "compile-objc3c",
                    "source": "tmp/artifacts/project-template/auroraBoard/src/main.objc3",
                    "artifact_root": "tmp/artifacts/project-template/auroraBoard/build",
                    "emit_prefix": "module",
                    "public_command": "npm run objc3c -- compile-objc3c tmp/artifacts/project-template/auroraBoard/src/main.objc3 --out-dir tmp/artifacts/project-template/auroraBoard/build --emit-prefix module",
                    "expected_artifacts": [
                        "module.obj",
                        "module.ll",
                        "module.manifest.json",
                        "module.runtime-registration-manifest.json",
                    ],
                },
                "public_actions": [
                    "materialize-project-template",
                    "compile-objc3c",
                    "materialize-playground-workspace",
                    "benchmark-runtime-inspector",
                    "inspect-bonus-tool-integration",
                ],
                "tutorial_guides": [
                    "docs/tutorials/getting_started.md",
                    "docs/tutorials/build_run_verify.md",
                    "docs/tutorials/guided_walkthrough.md",
                ],
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    harness_path.write_text(
        json.dumps(
            {
                "contract_id": "objc3c.project.template.demo.harness.v1",
                "ok": True,
                "compile_artifact_root": "tmp/artifacts/project-template/auroraBoard/build",
                "compile_step": {
                    "name": "compile-template-source",
                    "command": [
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
                    ],
                    "exit_code": 0,
                },
                "integration_report": "tmp/reports/objc3c-public-workflow/bonus-tool-integration.json",
                "playground_workspace": "tmp/artifacts/playground/auroraBoard/workspace.json",
                "benchmark_report": "tmp/reports/objc3c-public-workflow/runtime-inspector-benchmark.json",
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )

    monkeypatch.setattr(checker, "ROOT", root)
    monkeypatch.setattr(checker, "CONTRACT_PATH", contract_path)
    monkeypatch.setattr(
        checker,
        "SUMMARY_PATH",
        root / "tmp" / "reports" / "application-architecture-testing" / "template-harness-summary.json",
    )

    def fake_run_step(name: str, command: list[str]) -> dict[str, object]:
        assert command[:4] == ["npm", "run", "objc3c", "--"]
        return {
            "name": name,
            "command": command,
            "exit_code": 0,
            "stdout": (
                "template_path: tmp/artifacts/project-template/auroraBoard/template.json\n"
                "harness_path: tmp/reports/project-template/auroraBoard/demo-harness.json\n"
            ),
        }

    monkeypatch.setattr(checker, "run_step", fake_run_step)
    exit_code = checker.main()

    assert exit_code == 0
    summary_payload = json.loads(checker.SUMMARY_PATH.read_text(encoding="utf-8"))
    assert summary_payload["status"] == "PASS"
    assert summary_payload["template_contract_id"] == "objc3c.project.template.surface.v1"
