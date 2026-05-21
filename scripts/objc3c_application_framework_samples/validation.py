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
        "--",
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
    return samples, failures
