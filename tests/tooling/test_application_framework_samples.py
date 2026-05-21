from __future__ import annotations

import json
from pathlib import Path

from scripts.objc3c_application_framework_samples.constants import (
    CONTRACT_PATH,
    MANIFEST_PATH,
    ROOT,
)
from scripts.objc3c_application_framework_samples.validation import (
    build_compile_command,
    build_public_compile_command_text,
    load_json,
    validate_manifest,
)
from scripts.objc3c_application_framework_samples.runner import (
    run_framework_sample_validation,
)
from scripts.objc3c_tooling.subprocesses import CommandExecution


def test_application_framework_sample_manifest_is_real_source_backed() -> None:
    manifest = load_json(MANIFEST_PATH)
    contract = load_json(CONTRACT_PATH)

    samples, failures = validate_manifest(root=ROOT, manifest=manifest, contract=contract)

    assert failures == []
    assert [sample.sample_id for sample in samples] == [
        "routeModelKit",
        "interopAdapterKit",
        "workflowStdlibCLI",
        "asyncRuntimeConsole",
    ]
    assert all((ROOT / sample.source).is_file() for sample in samples)
    assert all((ROOT / sample.workspace_manifest).is_file() for sample in samples)


def test_application_framework_sample_compile_commands_use_public_bridge() -> None:
    manifest = load_json(MANIFEST_PATH)
    contract = load_json(CONTRACT_PATH)
    samples, failures = validate_manifest(root=ROOT, manifest=manifest, contract=contract)
    assert failures == []

    for sample in samples:
        command = build_compile_command(sample)
        assert command[:4] == ["npm", "run", "objc3c", "--"]
        assert command[4] == "compile-objc3c"
        assert command[5] == sample.source
        assert "--out-dir" in command
        assert "--emit-prefix" in command
        public_command = build_public_compile_command_text(sample)
        assert public_command.startswith("npm run objc3c -- compile-objc3c -- ")
        assert public_command == sample.public_compile_command


def test_application_framework_contract_has_no_generated_source_roots() -> None:
    manifest_payload = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))

    for sample in manifest_payload["samples"]:
        assert sample["source"].startswith("showcase/applicationFrameworkSamples/")
        assert not sample["source"].startswith(("tmp/", "artifacts/"))
        assert not sample["workspace_manifest"].startswith(("tmp/", "artifacts/"))

    for edge in manifest_payload["package_edges"]:
        assert edge["from"].startswith("showcase-framework:")
        assert edge["to"].startswith(("showcase-framework:", "stdlib:"))


def test_application_framework_sample_compile_starts_from_clean_artifact_root() -> None:
    stale_root = ROOT / "tmp" / "artifacts" / "application-framework-samples" / "routeModelKit"
    stale_root.mkdir(parents=True, exist_ok=True)
    stale_marker = stale_root / "stale-output.txt"
    stale_marker.write_text("stale", encoding="utf-8")

    def fake_compile(command: list[str]) -> CommandExecution:
        out_dir = ROOT / command[command.index("--out-dir") + 1]
        for artifact in load_json(CONTRACT_PATH)["required_artifacts"]:
            artifact_path = out_dir / artifact
            artifact_path.parent.mkdir(parents=True, exist_ok=True)
            artifact_path.write_text("fresh", encoding="utf-8")
        return CommandExecution(
            command=tuple(command),
            cwd=str(ROOT),
            returncode=0,
            stdout="",
            stderr="",
            duration_seconds=0.0,
        )

    exit_code, payload = run_framework_sample_validation(
        selected_sample_ids={"routeModelKit"},
        compile_samples=True,
        run_command=fake_compile,
    )

    assert exit_code == 0
    result = payload["compile_results"][0]
    assert result["sample_id"] == "routeModelKit"
    assert result["preexisting_artifact_root_removed"] is True
    assert result["stale_artifacts_allowed"] is False
    assert not stale_marker.exists()
