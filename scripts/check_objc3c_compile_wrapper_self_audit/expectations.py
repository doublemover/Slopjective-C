"""Assertions for the objc3c compile-wrapper self-audit checker."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import require_json_object as load_json

from .config import PROVENANCE_CONTRACT_ID, TRUTHFULNESS_CONTRACT_ID
from .contracts import wrapper_truth_owner_contract


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def validate_compile_output(out_dir: Path) -> dict[str, Any]:
    required_artifacts = [
        "module.obj",
        "module.ll",
        "module.manifest.json",
        "module.runtime-registration-manifest.json",
        "module.runtime-registration-descriptor.json",
        "module.compile-provenance.json",
    ]
    missing = [name for name in required_artifacts if not (out_dir / name).is_file()]
    expect(not missing, "wrapper compile missed required artifacts: " + ", ".join(missing))

    provenance = load_json(out_dir / "module.compile-provenance.json")
    registration_manifest = load_json(out_dir / "module.runtime-registration-manifest.json")
    truthfulness = provenance.get("compile_output_truthfulness")
    expect(isinstance(truthfulness, dict), "provenance missing compile_output_truthfulness")
    expect(
        provenance.get("contract_id") == PROVENANCE_CONTRACT_ID,
        "compile provenance contract drifted",
    )
    expect(
        truthfulness.get("contract_id") == TRUTHFULNESS_CONTRACT_ID,
        "compile truthfulness contract drifted",
    )
    expect(truthfulness.get("truthful") is True, "compile truthfulness is not true")
    expect(
        truthfulness.get("runtime_dispatch_symbol") == "objc3_runtime_dispatch_i32",
        "runtime dispatch symbol drifted",
    )
    expect(
        truthfulness.get("property_descriptor_count_expected") == 6
        and truthfulness.get("property_descriptor_definition_count") == 6,
        "property descriptor truthfulness counts drifted",
    )
    expect(
        truthfulness.get("ivar_descriptor_count_expected") == 3
        and truthfulness.get("ivar_descriptor_definition_count") == 3,
        "ivar descriptor truthfulness counts drifted",
    )
    expect(
        truthfulness.get("property_descriptor_section_present") is True
        and truthfulness.get("ivar_descriptor_section_present") is True,
        "descriptor sections were not found in emitted IR",
    )
    expect(
        truthfulness.get("synthesized_property_surface_matches") is True,
        "synthesized property truthfulness surface drifted",
    )
    expect(truthfulness.get("failures") == [], "truthfulness failures are not empty")
    expect(
        registration_manifest.get("compile_output_provenance_contract_id")
        == PROVENANCE_CONTRACT_ID,
        "registration manifest provenance contract drifted",
    )
    expect(
        registration_manifest.get("compile_output_truthfulness_contract_id")
        == TRUTHFULNESS_CONTRACT_ID,
        "registration manifest truthfulness contract drifted",
    )
    expect(
        registration_manifest.get("compile_output_truthful") is True,
        "registration manifest did not certify truthful compile output",
    )
    expect(
        registration_manifest.get("compile_output_artifact_set_digest_sha256")
        == provenance.get("artifact_set_digest_sha256"),
        "registration manifest artifact digest drifted from provenance",
    )
    expect(
        provenance.get("artifact_count") == len(provenance.get("emitted_artifacts", [])),
        "provenance artifact count does not match emitted_artifacts",
    )
    return {
        "owner_contract": wrapper_truth_owner_contract(),
        "provenance_contract_id": provenance.get("contract_id"),
        "truthfulness_contract_id": truthfulness.get("contract_id"),
        "artifact_count": provenance.get("artifact_count"),
        "artifact_set_digest_sha256": provenance.get("artifact_set_digest_sha256"),
        "runtime_dispatch_symbol": truthfulness.get("runtime_dispatch_symbol"),
        "property_descriptor_count": truthfulness.get(
            "property_descriptor_definition_count"
        ),
        "ivar_descriptor_count": truthfulness.get("ivar_descriptor_definition_count"),
    }


__all__ = ["expect", "validate_compile_output"]
