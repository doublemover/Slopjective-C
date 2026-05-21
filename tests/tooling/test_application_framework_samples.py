from __future__ import annotations

import json
import shutil
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
    validate_dependency_evidence,
    validate_manifest,
)
from scripts.objc3c_application_framework_samples.models import FrameworkSample
from scripts.objc3c_application_framework_samples.runner import (
    run_framework_sample_validation,
)
from scripts.objc3c_tooling.subprocesses import command_text
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
    assert all((ROOT / sample.replay_contract).is_file() for sample in samples)
    assert all((ROOT / sample.tutorial).is_file() for sample in samples)
    assert manifest["dependency_evidence"] == (
        "showcase/applicationFrameworkSamples/dependency-evidence.json"
    )
    assert (ROOT / manifest["dependency_evidence"]).is_file()


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
        assert public_command.startswith("npm run objc3c -- compile-objc3c ")
        assert public_command == command_text(command)
        assert public_command == sample.public_compile_command


def test_application_framework_contract_has_no_generated_source_roots() -> None:
    manifest_payload = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))

    assert manifest_payload["dependency_evidence"].startswith(
        "showcase/applicationFrameworkSamples/"
    )
    assert not manifest_payload["dependency_evidence"].startswith(("tmp/", "artifacts/"))
    for sample in manifest_payload["samples"]:
        assert sample["source"].startswith("showcase/applicationFrameworkSamples/")
        assert not sample["source"].startswith(("tmp/", "artifacts/"))
        assert not sample["workspace_manifest"].startswith(("tmp/", "artifacts/"))
        assert sample["replay_contract"].startswith("showcase/applicationFrameworkSamples/")
        assert not sample["replay_contract"].startswith(("tmp/", "artifacts/"))
        assert sample["tutorial"].startswith("docs/tutorials/")
        assert not sample["tutorial"].startswith(("tmp/", "artifacts/"))

    for edge in manifest_payload["package_edges"]:
        assert edge["from"].startswith("showcase-framework:")
        assert edge["to"].startswith(("showcase-framework:", "stdlib:"))


def test_application_framework_dependency_evidence_fails_without_source_term(tmp_path: Path) -> None:
    sample_root = tmp_path / "showcase" / "applicationFrameworkSamples"
    sample_root.parent.mkdir(parents=True, exist_ok=True)
    shutil.copytree(ROOT / "showcase" / "applicationFrameworkSamples", sample_root)

    manifest = load_json(MANIFEST_PATH)
    contract = load_json(CONTRACT_PATH)
    samples = [FrameworkSample.from_payload(sample) for sample in manifest["samples"]]
    evidence_path = sample_root / "dependency-evidence.json"
    evidence = load_json(evidence_path)
    for record in evidence["sample_dependency_evidence"]:
        if record["sample_id"] == "workflowStdlibCLI":
            record["dependencies"][1]["source_terms"] = ["missing_route_model_dependency_term"]
    evidence_path.write_text(json.dumps(evidence, indent=2) + "\n", encoding="utf-8")

    failures = validate_dependency_evidence(
        root=tmp_path,
        samples=samples,
        manifest=manifest,
        contract=contract,
    )

    assert (
        "workflowStdlibCLI: dependency showcase-framework:routeModelKit source term "
        "missing: missing_route_model_dependency_term"
    ) in failures


def _sample_payload_by_source(source: str) -> dict[str, object]:
    manifest = load_json(MANIFEST_PATH)
    for sample in manifest["samples"]:
        if sample["source"] == source:
            return sample
    raise AssertionError(f"unknown sample source {source}")


def _write_fake_compile_artifacts(command: list[str], *, module_override: str | None = None) -> None:
    source = command[5]
    sample = _sample_payload_by_source(source)
    replay = load_json(ROOT / str(sample["replay_contract"]))
    out_dir = ROOT / command[command.index("--out-dir") + 1]
    out_dir.mkdir(parents=True, exist_ok=True)

    expected_symbols = replay["expected_symbols"]
    expected_runtime = replay["expected_runtime_registration"]
    required_artifacts = replay["required_emitted_artifacts"]
    for artifact in required_artifacts:
        artifact_path = out_dir / str(artifact)
        artifact_path.parent.mkdir(parents=True, exist_ok=True)
        artifact_path.write_text("fresh", encoding="utf-8")

    (out_dir / "module.manifest.json").write_text(
        json.dumps(
            {
                "source": source,
                "module": module_override or sample["module_name"],
                "functions": [
                    {"name": name} for name in expected_symbols.get("functions", [])
                ],
                "interfaces": [
                    {"name": name} for name in expected_symbols.get("interfaces", [])
                ],
                "protocols": [
                    {"name": name} for name in expected_symbols.get("protocols", [])
                ],
                "categories": expected_symbols.get("categories", []),
            }
        ),
        encoding="utf-8",
    )
    (out_dir / "module.runtime-registration-manifest.json").write_text(
        json.dumps(expected_runtime),
        encoding="utf-8",
    )
    (out_dir / "module.compile-provenance.json").write_text(
        json.dumps(
            {
                "input_source": source,
                "artifact_count": replay["minimum_artifact_count"],
                "compile_output_truthfulness": {
                    "truthful": True,
                    "runtime_dispatch_symbol": "objc3_runtime_dispatch_i32",
                },
                "emitted_artifacts": [
                    {"path": artifact, "byte_count": 5, "sha256": "fake"}
                    for artifact in required_artifacts
                ],
            }
        ),
        encoding="utf-8",
    )


def test_application_framework_sample_compile_starts_from_clean_artifact_root() -> None:
    stale_root = ROOT / "tmp" / "artifacts" / "application-framework-samples" / "routeModelKit"
    stale_root.mkdir(parents=True, exist_ok=True)
    stale_marker = stale_root / "stale-output.txt"
    stale_marker.write_text("stale", encoding="utf-8")

    def fake_compile(command: list[str]) -> CommandExecution:
        _write_fake_compile_artifacts(command)
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
    assert result["replay_failures"] == []
    assert not stale_marker.exists()


def test_application_framework_sample_compile_fails_on_replay_contract_drift() -> None:
    def fake_compile(command: list[str]) -> CommandExecution:
        _write_fake_compile_artifacts(command, module_override="DocsOnlyStub")
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

    assert exit_code == 1
    result = payload["compile_results"][0]
    assert result["status"] == "FAIL"
    assert "routeModelKit: compiled manifest module drifted" in result["replay_failures"]
