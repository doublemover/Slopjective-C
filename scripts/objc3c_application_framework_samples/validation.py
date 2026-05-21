from __future__ import annotations

import json
import re
from pathlib import Path
from typing import Any

from scripts.objc3c_tooling.subprocesses import command_text
from scripts.objc3c_workflow.public_command_api import public_workflow_command

from .constants import (
    COMPILE_EMIT_PREFIX,
    CONTRACT_ID,
    MANIFEST_CONTRACT_ID,
    REPLAY_CONTRACT_ID,
    WORKSPACE_CONTRACT_ID,
)
from .models import FrameworkSample

MODULE_RE = re.compile(r"^\s*module\s+([A-Za-z_][A-Za-z0-9_]*)\s*;", re.MULTILINE)
STDLIB_PACKAGE_IDS = {
    "stdlib:objc3.core",
    "stdlib:objc3.errors",
    "stdlib:objc3.concurrency",
    "stdlib:objc3.keypath",
    "stdlib:objc3.system",
}


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"expected object JSON at {path}")
    return payload


def build_compile_command(sample: FrameworkSample) -> list[str]:
    return public_workflow_command(
        "compile-objc3c",
        sample.source,
        "--out-dir",
        sample.artifact_root,
        "--emit-prefix",
        COMPILE_EMIT_PREFIX,
    )


def build_public_compile_command_text(sample: FrameworkSample) -> str:
    command = public_workflow_command(
        "compile-objc3c",
        sample.source,
        "--out-dir",
        sample.artifact_root,
        "--emit-prefix",
        COMPILE_EMIT_PREFIX,
    )
    return command_text(command)


def _module_name(source_text: str) -> str | None:
    match = MODULE_RE.search(source_text)
    return match.group(1) if match else None


def _required_values_missing(required: object, actual: set[str]) -> list[str]:
    if not isinstance(required, list):
        return []
    return sorted(str(value) for value in required if str(value) not in actual)


def _validate_workspace(
    *,
    root: Path,
    sample: FrameworkSample,
    workspace: dict[str, Any],
) -> list[str]:
    failures: list[str] = []
    expected_fields: dict[str, object] = {
        "contract_id": WORKSPACE_CONTRACT_ID,
        "schema_version": 1,
        "sample_id": sample.sample_id,
        "sample_kind": sample.kind,
        "package_id": sample.package_id,
        "module_name": sample.module_name,
        "entry_source": sample.source,
        "replay_contract": sample.replay_contract,
        "artifact_root": sample.artifact_root,
        "emit_prefix": COMPILE_EMIT_PREFIX,
        "public_compile_command": sample.public_compile_command,
    }
    for field_name, expected_value in expected_fields.items():
        if workspace.get(field_name) != expected_value:
            failures.append(f"{sample.sample_id}: workspace {field_name} drifted")

    if tuple(workspace.get("capabilities", [])) != sample.capabilities:
        failures.append(f"{sample.sample_id}: workspace capabilities drifted")
    if tuple(workspace.get("support_claims", [])) != sample.support_claims:
        failures.append(f"{sample.sample_id}: workspace support claims drifted")
    if tuple(workspace.get("package_dependencies", [])) != sample.package_dependencies:
        failures.append(f"{sample.sample_id}: workspace package dependencies drifted")

    workspace_root = workspace.get("workspace_root")
    if isinstance(workspace_root, str) and not (root / workspace_root).is_dir():
        failures.append(f"{sample.sample_id}: workspace root is missing")
    return failures


def _validate_replay_contract(
    *,
    root: Path,
    sample: FrameworkSample,
    replay_contract: dict[str, Any],
) -> list[str]:
    failures: list[str] = []
    expected_fields: dict[str, object] = {
        "contract_id": REPLAY_CONTRACT_ID,
        "schema_version": 1,
        "sample_id": sample.sample_id,
        "package_id": sample.package_id,
        "module_name": sample.module_name,
        "source": sample.source,
        "artifact_root": sample.artifact_root,
        "workspace_manifest": sample.workspace_manifest,
    }
    for field_name, expected_value in expected_fields.items():
        if replay_contract.get(field_name) != expected_value:
            failures.append(f"{sample.sample_id}: replay contract {field_name} drifted")

    if tuple(replay_contract.get("support_claims", [])) != sample.support_claims:
        failures.append(f"{sample.sample_id}: replay contract support claims drifted")
    if tuple(replay_contract.get("package_dependencies", [])) != sample.package_dependencies:
        failures.append(f"{sample.sample_id}: replay contract package dependencies drifted")

    required_artifacts = replay_contract.get("required_emitted_artifacts", [])
    if not isinstance(required_artifacts, list) or not required_artifacts:
        failures.append(f"{sample.sample_id}: replay contract must list required artifacts")
    else:
        for artifact in required_artifacts:
            artifact_text = str(artifact)
            if Path(artifact_text).is_absolute() or ".." in Path(artifact_text).parts:
                failures.append(
                    f"{sample.sample_id}: replay contract artifact escapes artifact root"
                )

    expected_runtime = replay_contract.get("expected_runtime_registration", {})
    if not isinstance(expected_runtime, dict):
        failures.append(f"{sample.sample_id}: replay runtime registration contract must be an object")
    else:
        for field_name in (
            "class_descriptor_count",
            "protocol_descriptor_count",
            "category_descriptor_count",
            "property_descriptor_count",
            "ivar_descriptor_count",
            "total_descriptor_count",
        ):
            value = expected_runtime.get(field_name)
            if not isinstance(value, int) or value < 0:
                failures.append(
                    f"{sample.sample_id}: replay runtime field {field_name} must be non-negative integer"
                )

    source_path = sample.source_path(root)
    if source_path.is_file():
        source_text = source_path.read_text(encoding="utf-8")
        required_source_terms = replay_contract.get("required_source_terms", [])
        if isinstance(required_source_terms, list):
            for term in required_source_terms:
                if str(term) not in source_text:
                    failures.append(
                        f"{sample.sample_id}: replay source term missing: {term}"
                    )
    return failures


def _names_from_records(payload: dict[str, Any], field_name: str) -> set[str]:
    records = payload.get(field_name, [])
    if not isinstance(records, list):
        return set()
    return {
        str(record.get("name"))
        for record in records
        if isinstance(record, dict) and record.get("name") is not None
    }


def _category_keys(payload: dict[str, Any]) -> set[tuple[str, str, str]]:
    categories = payload.get("categories", [])
    if not isinstance(categories, list):
        return set()
    return {
        (
            str(record.get("class_name")),
            str(record.get("category_name")),
            str(record.get("record_kind")),
        )
        for record in categories
        if isinstance(record, dict)
    }


def validate_compiled_replay_contract(
    *,
    root: Path,
    sample: FrameworkSample,
    replay_contract: dict[str, Any],
) -> list[str]:
    failures: list[str] = []
    artifact_root = sample.artifact_path(root)
    manifest_path = artifact_root / "module.manifest.json"
    registration_path = artifact_root / "module.runtime-registration-manifest.json"
    provenance_path = artifact_root / "module.compile-provenance.json"
    if not manifest_path.is_file():
        failures.append(f"{sample.sample_id}: compiled manifest missing")
    if not registration_path.is_file():
        failures.append(f"{sample.sample_id}: runtime registration manifest missing")
    if not provenance_path.is_file():
        failures.append(f"{sample.sample_id}: compile provenance missing")
    if failures:
        return failures

    compiled_manifest = load_json(manifest_path)
    registration_manifest = load_json(registration_path)
    provenance = load_json(provenance_path)

    if compiled_manifest.get("source") != sample.source:
        failures.append(f"{sample.sample_id}: compiled manifest source drifted")
    if compiled_manifest.get("module") != sample.module_name:
        failures.append(f"{sample.sample_id}: compiled manifest module drifted")
    if provenance.get("input_source") != sample.source:
        failures.append(f"{sample.sample_id}: compile provenance input source drifted")

    expected_symbols = replay_contract.get("expected_symbols", {})
    if not isinstance(expected_symbols, dict):
        failures.append(f"{sample.sample_id}: expected_symbols must be an object")
        expected_symbols = {}
    for field_name in ("functions", "interfaces", "protocols"):
        expected_names = {str(item) for item in expected_symbols.get(field_name, [])}
        actual_names = _names_from_records(compiled_manifest, field_name)
        missing = sorted(expected_names - actual_names)
        for name in missing:
            failures.append(f"{sample.sample_id}: compiled {field_name} missing {name}")

    expected_categories = expected_symbols.get("categories", [])
    if isinstance(expected_categories, list):
        actual_categories = _category_keys(compiled_manifest)
        for category in expected_categories:
            if not isinstance(category, dict):
                failures.append(f"{sample.sample_id}: category replay entry must be an object")
                continue
            expected_key = (
                str(category.get("class_name")),
                str(category.get("category_name")),
                str(category.get("record_kind")),
            )
            if expected_key not in actual_categories:
                failures.append(
                    f"{sample.sample_id}: compiled category missing {expected_key[0]}({expected_key[1]})/{expected_key[2]}"
                )

    expected_runtime = replay_contract.get("expected_runtime_registration", {})
    if isinstance(expected_runtime, dict):
        for field_name, expected_value in expected_runtime.items():
            if registration_manifest.get(field_name) != expected_value:
                failures.append(
                    f"{sample.sample_id}: runtime registration {field_name} drifted"
                )

    truthfulness = provenance.get("compile_output_truthfulness", {})
    if not isinstance(truthfulness, dict) or truthfulness.get("truthful") is not True:
        failures.append(f"{sample.sample_id}: compile output truthfulness is not true")
    expected_truthfulness = replay_contract.get("expected_compile_output_truthfulness", {})
    if isinstance(expected_truthfulness, dict):
        symbol = expected_truthfulness.get("runtime_dispatch_symbol")
        if symbol and truthfulness.get("runtime_dispatch_symbol") != symbol:
            failures.append(f"{sample.sample_id}: runtime dispatch symbol drifted")

    emitted = provenance.get("emitted_artifacts", [])
    emitted_paths = {
        str(artifact.get("path"))
        for artifact in emitted
        if isinstance(artifact, dict) and artifact.get("path") is not None
    }
    for artifact in replay_contract.get("required_emitted_artifacts", []):
        artifact_text = str(artifact)
        if artifact_text not in emitted_paths:
            failures.append(f"{sample.sample_id}: emitted artifact missing from provenance: {artifact_text}")
        if not (artifact_root / artifact_text).is_file():
            failures.append(f"{sample.sample_id}: emitted artifact file missing: {artifact_text}")

    minimum_artifact_count = replay_contract.get("minimum_artifact_count")
    if isinstance(minimum_artifact_count, int):
        artifact_count = provenance.get("artifact_count")
        if not isinstance(artifact_count, int) or artifact_count < minimum_artifact_count:
            failures.append(f"{sample.sample_id}: artifact count below replay minimum")
    return failures


def _validate_package_edges(
    *,
    samples: list[FrameworkSample],
    manifest: dict[str, Any],
    contract: dict[str, Any],
) -> list[str]:
    failures: list[str] = []
    sample_package_ids = {sample.package_id for sample in samples}
    known_package_ids = sample_package_ids | STDLIB_PACKAGE_IDS
    edge_pairs: set[tuple[str, str]] = set()
    package_edges = manifest.get("package_edges")
    if not isinstance(package_edges, list):
        return ["manifest package_edges must be a list"]

    for edge in package_edges:
        if not isinstance(edge, dict):
            failures.append("manifest package edge must be an object")
            continue
        from_package = str(edge.get("from", ""))
        to_package = str(edge.get("to", ""))
        edge_pairs.add((from_package, to_package))
        if from_package not in sample_package_ids:
            failures.append(f"package edge from unknown sample package {from_package}")
        if to_package not in known_package_ids:
            failures.append(f"package edge to unknown package {to_package}")

    for required_edge in contract.get("required_package_edges", []):
        if not isinstance(required_edge, dict):
            failures.append("contract required_package_edges entry must be an object")
            continue
        pair = (str(required_edge.get("from", "")), str(required_edge.get("to", "")))
        if pair not in edge_pairs:
            failures.append(f"missing required package edge {pair[0]} -> {pair[1]}")

    for sample in samples:
        for dependency in sample.package_dependencies:
            if dependency not in known_package_ids:
                failures.append(f"{sample.sample_id}: unknown package dependency {dependency}")
            if dependency not in STDLIB_PACKAGE_IDS and (sample.package_id, dependency) not in edge_pairs:
                failures.append(
                    f"{sample.sample_id}: missing manifest edge for dependency {dependency}"
                )
    return failures


def _manifest_edge_index(manifest: dict[str, Any]) -> dict[tuple[str, str], str]:
    package_edges = manifest.get("package_edges", [])
    if not isinstance(package_edges, list):
        return {}
    edge_index: dict[tuple[str, str], str] = {}
    for edge in package_edges:
        if not isinstance(edge, dict):
            continue
        edge_index[(str(edge.get("from", "")), str(edge.get("to", "")))] = str(
            edge.get("relationship", "")
        )
    return edge_index


def validate_dependency_evidence(
    *,
    root: Path,
    samples: list[FrameworkSample],
    manifest: dict[str, Any],
    contract: dict[str, Any],
) -> list[str]:
    failures: list[str] = []
    dependency_evidence_path = manifest.get("dependency_evidence")
    if dependency_evidence_path != contract.get("dependency_evidence"):
        failures.append("dependency evidence path drifted between manifest and contract")
    if not isinstance(dependency_evidence_path, str) or not dependency_evidence_path:
        return [*failures, "manifest dependency_evidence must be a path"]
    if not dependency_evidence_path.startswith("showcase/applicationFrameworkSamples/"):
        failures.append("manifest dependency_evidence escaped application framework sample root")
    evidence_path = root / dependency_evidence_path
    if not evidence_path.is_file():
        return [*failures, f"dependency evidence missing: {dependency_evidence_path}"]

    evidence = load_json(evidence_path)
    if evidence.get("contract_id") != "objc3c.application_framework_samples.dependency_evidence.v1":
        failures.append("dependency evidence contract_id drifted")
    if evidence.get("schema_version") != 1:
        failures.append("dependency evidence schema_version drifted")
    if evidence.get("issue") != 8178:
        failures.append("dependency evidence issue drifted")
    if evidence.get("manifest") != "showcase/applicationFrameworkSamples/manifest.json":
        failures.append("dependency evidence manifest path drifted")
    clean_policy = evidence.get("clean_root_replay_policy")
    if clean_policy != {
        "artifact_root_must_be_removed_before_compile": True,
        "stale_artifacts_allowed": False,
        "generated_outputs_are_source_truth": False,
    }:
        failures.append("dependency evidence clean-root replay policy drifted")

    forbidden_prefixes = evidence.get("forbidden_dependency_prefixes", [])
    if not isinstance(forbidden_prefixes, list) or not forbidden_prefixes:
        failures.append("dependency evidence must list forbidden dependency prefixes")
        forbidden_prefixes = []
    fail_closed_rules = evidence.get("fail_closed_rules", [])
    if not isinstance(fail_closed_rules, list) or not fail_closed_rules:
        failures.append("dependency evidence must declare fail-closed rules")
    else:
        for rule in fail_closed_rules:
            if not isinstance(rule, dict) or rule.get("blocks_sample_claim") is not True:
                failures.append("dependency evidence fail-closed rule must block sample claims")

    records = evidence.get("sample_dependency_evidence", [])
    if not isinstance(records, list):
        return [*failures, "dependency evidence sample_dependency_evidence must be a list"]
    evidence_by_sample: dict[str, dict[str, Any]] = {}
    for record in records:
        if not isinstance(record, dict):
            failures.append("dependency evidence sample record must be an object")
            continue
        sample_id = str(record.get("sample_id", ""))
        if not sample_id:
            failures.append("dependency evidence sample record missing sample_id")
            continue
        if sample_id in evidence_by_sample:
            failures.append(f"duplicate dependency evidence sample id {sample_id}")
        evidence_by_sample[sample_id] = record

    edge_index = _manifest_edge_index(manifest)
    sample_package_ids = {sample.package_id for sample in samples}
    known_package_ids = sample_package_ids | STDLIB_PACKAGE_IDS
    for sample in samples:
        record = evidence_by_sample.get(sample.sample_id)
        if record is None:
            failures.append(f"{sample.sample_id}: missing dependency evidence record")
            continue
        if record.get("package_id") != sample.package_id:
            failures.append(f"{sample.sample_id}: dependency evidence package id drifted")
        dependencies = record.get("dependencies", [])
        if not isinstance(dependencies, list):
            failures.append(f"{sample.sample_id}: dependency evidence dependencies must be a list")
            continue
        dependency_ids = [str(dependency.get("package_id", "")) for dependency in dependencies if isinstance(dependency, dict)]
        if tuple(dependency_ids) != sample.package_dependencies:
            failures.append(f"{sample.sample_id}: dependency evidence dependency list drifted")

        workspace = load_json(sample.workspace_path(root))
        replay_contract = load_json(sample.replay_contract_path(root))
        workspace_dependencies = {str(item) for item in workspace.get("package_dependencies", [])}
        replay_dependencies = {str(item) for item in replay_contract.get("package_dependencies", [])}
        source_text = sample.source_path(root).read_text(encoding="utf-8")

        for dependency in dependencies:
            if not isinstance(dependency, dict):
                failures.append(f"{sample.sample_id}: dependency evidence entry must be an object")
                continue
            package_id = str(dependency.get("package_id", ""))
            for prefix in forbidden_prefixes:
                if package_id.startswith(str(prefix)):
                    failures.append(f"{sample.sample_id}: unsupported dependency boundary {package_id}")
            if package_id not in known_package_ids:
                failures.append(f"{sample.sample_id}: dependency evidence references unknown package {package_id}")
            if package_id not in workspace_dependencies:
                failures.append(f"{sample.sample_id}: dependency {package_id} missing from workspace")
            if package_id not in replay_dependencies:
                failures.append(f"{sample.sample_id}: dependency {package_id} missing from replay contract")
            if dependency.get("requires_manifest_edge") is True:
                edge_key = (sample.package_id, package_id)
                relationship = edge_index.get(edge_key)
                if relationship is None:
                    failures.append(f"{sample.sample_id}: dependency {package_id} missing manifest edge")
                elif relationship != dependency.get("relationship"):
                    failures.append(f"{sample.sample_id}: dependency {package_id} relationship drifted")
            source_terms = dependency.get("source_terms", [])
            if not isinstance(source_terms, list) or not source_terms:
                failures.append(f"{sample.sample_id}: dependency {package_id} missing source terms")
                continue
            for term in source_terms:
                if str(term) not in source_text:
                    failures.append(
                        f"{sample.sample_id}: dependency {package_id} source term missing: {term}"
                    )
    return failures


def validate_manifest(
    *,
    root: Path,
    manifest: dict[str, Any],
    contract: dict[str, Any],
) -> tuple[list[FrameworkSample], list[str]]:
    failures: list[str] = []
    if manifest.get("contract_id") != MANIFEST_CONTRACT_ID:
        failures.append("manifest contract_id drifted")
    if manifest.get("schema_version") != 1:
        failures.append("manifest schema_version drifted")
    if manifest.get("issue") != 8178:
        failures.append("manifest issue drifted")
    if contract.get("contract_id") != CONTRACT_ID:
        failures.append("contract fixture contract_id drifted")
    if contract.get("manifest") != "showcase/applicationFrameworkSamples/manifest.json":
        failures.append("contract fixture manifest path drifted")
    required_tutorials = contract.get("required_tutorials", [])
    if not isinstance(required_tutorials, list):
        failures.append("contract required_tutorials must be a list")
        required_tutorials = []
    for tutorial in required_tutorials:
        tutorial_path = root / str(tutorial)
        if not str(tutorial).startswith("docs/tutorials/"):
            failures.append(f"contract required tutorial escaped tutorials root: {tutorial}")
        if not tutorial_path.is_file():
            failures.append(f"contract required tutorial missing: {tutorial}")
        else:
            tutorial_text = tutorial_path.read_text(encoding="utf-8")
            required_terms = contract.get("required_tutorial_terms", [])
            if isinstance(required_terms, list):
                for term in required_terms:
                    if str(term) not in tutorial_text:
                        failures.append(
                            f"contract required tutorial term missing from {tutorial}: {term}"
                        )

    samples_payload = manifest.get("samples")
    if not isinstance(samples_payload, list) or not samples_payload:
        return [], [*failures, "manifest samples must be a non-empty list"]

    samples: list[FrameworkSample] = []
    sample_ids: set[str] = set()
    package_ids: set[str] = set()
    for raw_sample in samples_payload:
        if not isinstance(raw_sample, dict):
            failures.append("sample entry must be an object")
            continue
        try:
            sample = FrameworkSample.from_payload(raw_sample)
        except KeyError as exc:
            failures.append(f"sample entry missing {exc.args[0]}")
            continue

        if sample.sample_id in sample_ids:
            failures.append(f"duplicate sample id {sample.sample_id}")
        if sample.package_id in package_ids:
            failures.append(f"duplicate sample package id {sample.package_id}")
        sample_ids.add(sample.sample_id)
        package_ids.add(sample.package_id)
        samples.append(sample)

        source_path = sample.source_path(root)
        workspace_path = sample.workspace_path(root)
        replay_contract_path = sample.replay_contract_path(root)
        tutorial_path = sample.tutorial_path(root)
        if not sample.source.startswith("showcase/applicationFrameworkSamples/"):
            failures.append(f"{sample.sample_id}: source escaped sample root")
        if not source_path.is_file():
            failures.append(f"{sample.sample_id}: source missing")
        else:
            module_name = _module_name(source_path.read_text(encoding="utf-8"))
            if module_name != sample.module_name:
                failures.append(f"{sample.sample_id}: module declaration drifted")
        if not workspace_path.is_file():
            failures.append(f"{sample.sample_id}: workspace manifest missing")
        else:
            failures.extend(
                _validate_workspace(
                    root=root,
                    sample=sample,
                    workspace=load_json(workspace_path),
                )
            )
        if not sample.replay_contract.startswith("showcase/applicationFrameworkSamples/"):
            failures.append(f"{sample.sample_id}: replay contract escaped sample root")
        if not replay_contract_path.is_file():
            failures.append(f"{sample.sample_id}: replay contract missing")
        else:
            failures.extend(
                _validate_replay_contract(
                    root=root,
                    sample=sample,
                    replay_contract=load_json(replay_contract_path),
                )
            )
        if not sample.tutorial.startswith("docs/tutorials/"):
            failures.append(f"{sample.sample_id}: tutorial escaped tutorials root")
        if not tutorial_path.is_file():
            failures.append(f"{sample.sample_id}: tutorial missing")
        else:
            tutorial_text = tutorial_path.read_text(encoding="utf-8")
            if sample.sample_id not in tutorial_text:
                failures.append(f"{sample.sample_id}: tutorial does not name sample id")
            if sample.public_compile_command not in tutorial_text:
                failures.append(f"{sample.sample_id}: tutorial missing public compile command")

        actual_compile_command = build_public_compile_command_text(sample)
        if actual_compile_command != sample.public_compile_command:
            failures.append(f"{sample.sample_id}: public compile command drifted")

    actual_kinds = {sample.kind for sample in samples}
    missing_kinds = _required_values_missing(contract.get("required_sample_kinds"), actual_kinds)
    for kind in missing_kinds:
        failures.append(f"missing required sample kind {kind}")

    actual_capabilities = {
        capability for sample in samples for capability in sample.capabilities
    }
    missing_capabilities = _required_values_missing(
        contract.get("required_capabilities"),
        actual_capabilities,
    )
    for capability in missing_capabilities:
        failures.append(f"missing required capability {capability}")

    actual_claims = {claim for sample in samples for claim in sample.support_claims}
    missing_claims = _required_values_missing(
        contract.get("required_support_claims"),
        actual_claims,
    )
    for claim in missing_claims:
        failures.append(f"missing required support claim {claim}")

    actual_tutorials = {sample.tutorial for sample in samples}
    missing_tutorials = _required_values_missing(required_tutorials, actual_tutorials)
    for tutorial in missing_tutorials:
        failures.append(f"missing required tutorial {tutorial}")

    failures.extend(
        _validate_package_edges(samples=samples, manifest=manifest, contract=contract)
    )
    failures.extend(
        validate_dependency_evidence(
            root=root,
            samples=samples,
            manifest=manifest,
            contract=contract,
        )
    )
    return samples, failures
