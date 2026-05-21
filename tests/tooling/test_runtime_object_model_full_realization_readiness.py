from __future__ import annotations

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[2]
CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "object_model_closure"
    / "full_realization_combined_readiness_contract.json"
)
UMBRELLA_READINESS_PATH = ROOT / "docs" / "support" / "umbrella_readiness.json"
PUBLIC_REFLECTION_HEADER = (
    ROOT
    / "native"
    / "objc3c"
    / "src"
    / "runtime"
    / "public"
    / "objc3_runtime_reflection.h"
)
PUBLIC_REFLECTION_PROBE = (
    ROOT / "tests" / "tooling" / "runtime" / "public_runtime_reflection_api_probe.cpp"
)


def _read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def _repo_path(path: str) -> Path:
    assert not path.startswith(("tmp/", "tmp\\"))
    return ROOT / path


def test_full_realization_combined_contract_is_checked_source_evidence() -> None:
    contract = _read_json(CONTRACT_PATH)

    assert contract["issue"] == 8198
    assert contract["capability_id"] == "runtime.object-model.full-realization"
    assert contract["public_status"] == "reserved"
    assert contract["support_claim_published"] is False
    assert "support_claim" not in contract
    assert contract["combined_axes"] == [
        "class",
        "metaclass",
        "category",
        "protocol",
        "property",
        "ivar",
        "selector",
        "public-reflection",
        "registration-replay",
    ]

    for key in (
        "combined_positive_fixture",
        "combined_positive_fixture_meta",
        "public_reflection_contract",
        "public_reflection_probe",
        "runbook",
        "umbrella_readiness",
    ):
        assert _repo_path(str(contract[key])).is_file(), key

    for command in contract["public_commands"]:
        assert str(command).startswith("npm run objc3c -- ")


def test_combined_fixture_covers_object_model_reflection_and_replay_axes() -> None:
    contract = _read_json(CONTRACT_PATH)
    fixture_path = _repo_path(str(contract["combined_positive_fixture"]))
    fixture_meta = _read_json(_repo_path(str(contract["combined_positive_fixture_meta"])))
    fixture = fixture_path.read_text(encoding="utf-8")

    assert fixture_meta["owner_phase"] == "runtime"
    assert fixture_meta["fixture_kind"] == "positive"
    assert fixture_meta["boundary"]["support_claim_published"] is False
    for token in (
        "@protocol RuntimeFullTraceable",
        "@interface RuntimeFullRoot",
        "+ (i32) rootTypeValue",
        "@interface RuntimeFullWidget : RuntimeFullRoot <RuntimeFullTraceable>",
        "@property (nonatomic, getter=value, setter=setValue:) i32 value",
        "@property (readonly, getter=token) id token",
        "@interface RuntimeFullWidget (ReplayReflection)",
        "- (i32) replayValue",
    ):
        assert token in fixture


def test_public_reflection_probe_preserves_lifetime_and_source_boundaries() -> None:
    contract = _read_json(CONTRACT_PATH)
    probe = PUBLIC_REFLECTION_PROBE.read_text(encoding="utf-8")
    header = PUBLIC_REFLECTION_HEADER.read_text(encoding="utf-8")
    lifetime = contract["reflection_lifetime_contract"]

    assert lifetime["snapshot_ownership"] == "caller-owned snapshot structs"
    assert lifetime["string_lifetime"] == "runtime-owned borrowed strings"
    assert lifetime["state_source"] == "runtime-owned realized state"
    assert "owner_identity;" not in header
    assert "const char *" in header
    assert "objc3_runtime_reflection_state_snapshot state{};" in probe
    assert "objc3_runtime_reflection_class_snapshot widget_class{};" in probe
    assert "objc3_runtime_reflection_property_snapshot value_property{};" in probe
    assert "objc3_runtime_reflection_selector_snapshot indexed_selector{};" in probe
    assert "objc3_runtime_stage_registration_table_for_bootstrap" in probe
    assert "objc3_runtime_register_image(&fixture.image)" in probe

    for source_anchor in contract["source_anchors"]:
        path = _repo_path(str(source_anchor["path"]))
        assert path.is_file(), source_anchor


def test_negative_boundaries_remain_non_public_and_fail_closed() -> None:
    contract = _read_json(CONTRACT_PATH)
    probe = PUBLIC_REFLECTION_PROBE.read_text(encoding="utf-8")
    header = PUBLIC_REFLECTION_HEADER.read_text(encoding="utf-8")

    boundaries = {
        str(boundary["boundary_id"]): boundary
        for boundary in contract["negative_boundaries"]
    }
    assert set(boundaries) == {
        "private-snapshots-not-public-reflection",
        "malformed-metadata-fails-closed",
        "stale-generation-replay-does-not-promote-support",
    }

    assert "_for_testing" not in probe
    for private_symbol in (
        "objc3_runtime_copy_realized_class_entry_for_testing",
        "objc3_runtime_copy_property_entry_for_testing",
        "objc3_runtime_copy_protocol_conformance_query_for_testing",
        "objc3_runtime_copy_selector_lookup_entry_for_testing",
    ):
        assert private_symbol not in header

    assert "OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA" in header
    assert "OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY" in header

    for boundary in boundaries.values():
        for evidence_path in boundary["evidence"]:
            assert _repo_path(str(evidence_path)).exists(), evidence_path


def test_umbrella_readiness_references_combined_evidence_without_closing_row() -> None:
    readiness = _read_json(UMBRELLA_READINESS_PATH)
    entries = {
        str(entry["umbrella_capability_id"]): entry
        for entry in readiness["entries"]
    }
    entry = entries["runtime.object-model.full-realization"]

    assert entry["current_state"] == "reserved"
    assert entry["readiness_state"] == "blocked"
    assert {
        blocker["blocker_id"] for blocker in entry["promotion_blockers"]
    } == {"object-model-debugger-source-identity"}

    required_paths = {
        requirement.get("path")
        for field in (
            "required_source_anchors",
            "required_positive_fixtures",
            "required_negative_fixtures",
            "required_runtime_probes",
            "required_docs",
        )
        for requirement in entry[field]
        if requirement.get("status") == "satisfied"
    }
    assert str(CONTRACT_PATH.relative_to(ROOT)).replace("\\", "/") in required_paths
    assert (
        "tests/native/runtime/object_model/"
        "full_realization_combined_reflection_replay_contract.objc3"
    ) in required_paths
    assert "tests/tooling/runtime/public_runtime_reflection_api_probe.cpp" in required_paths
