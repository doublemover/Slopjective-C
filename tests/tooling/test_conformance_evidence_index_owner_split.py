from __future__ import annotations

import importlib.util
import sys
from pathlib import Path

import pytest

from conformance_evidence_index.builder import (
    build_index_payload,
    build_profiles_index,
    build_releases_index,
)
from conformance_evidence_index.cli import build_parser
from conformance_evidence_index.manifest import infer_profile_release
from conformance_evidence_index.model import ArtifactRecord
from conformance_evidence_index.timestamps import StrictGeneratedAtError

ROOT = Path(__file__).resolve().parents[2]
LEGACY_ENTRYPOINT = ROOT / "scripts" / "generate_conformance_evidence_index.py"


def _record(
    *,
    profile_id: str,
    release_id: str,
    artifact_path: str,
) -> ArtifactRecord:
    return ArtifactRecord(
        artifact_path=artifact_path,
        file_sha256="sha256:" + "0" * 64,
        size_bytes=1,
        media_type="application/json",
        profile_id=profile_id,
        release_id=release_id,
        artifact_id=Path(artifact_path).stem,
        manifest_kind="test-manifest",
        schema_ref=None,
        source_generated_at=None,
        issue_ref=None,
    )


def test_legacy_entrypoint_reexports_split_owner_modules() -> None:
    spec = importlib.util.spec_from_file_location(
        "generate_conformance_evidence_index", LEGACY_ENTRYPOINT
    )
    assert spec is not None
    assert spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)

    assert module.build_parser is build_parser
    assert module.build_index_payload is build_index_payload
    assert module.ArtifactRecord is ArtifactRecord
    assert (
        module.SUPPORT_CLAIM_RUNNABLE_EVIDENCE_CATALOG.as_posix()
        == "tests/conformance/support_claim_runnable_evidence_catalog.json"
    )


def test_manifest_inference_stays_isolated_from_payload_grouping() -> None:
    payload = {
        "schema_id": "objc3-conformance-evidence-bundle-v1",
        "profile_claim": {"profile": "runtime-core-2026Q1"},
        "generated_at": "2026-05-09T12:34:56-04:00",
        "issue_ref": "#8148",
    }

    profile, release, artifact_id, manifest_kind, schema_ref, generated_at, issue_ref = (
        infer_profile_release(
            rel_path="reports/conformance/bundles/runtime-core-2026Q1.example.json",
            payload=payload,
            release_retired_route=None,
            strict_generated_at=True,
        )
    )

    assert profile == "runtime-core"
    assert release == "2026Q1"
    assert artifact_id == "runtime-core-2026Q1"
    assert manifest_kind == "objc3-conformance-evidence-bundle-v1"
    assert schema_ref == "objc3-conformance-evidence-bundle-v1"
    assert generated_at == "2026-05-09T16:34:56Z"
    assert issue_ref == "#8148"


def test_strict_generated_at_failure_is_owned_by_manifest_inference() -> None:
    with pytest.raises(StrictGeneratedAtError, match="invalid generated_at"):
        infer_profile_release(
            rel_path="reports/conformance/bad.json",
            payload={"generated_at": "2026-05-09 12:34:56"},
            release_retired_route="v0.11",
            strict_generated_at=True,
        )


def test_bidirectional_grouping_is_payload_builder_owned() -> None:
    records = [
        _record(
            profile_id="runtime",
            release_id="v0.11",
            artifact_path="reports/conformance/runtime.json",
        ),
        _record(
            profile_id="parser",
            release_id="v0.11",
            artifact_path="reports/conformance/parser.json",
        ),
        _record(
            profile_id="runtime",
            release_id="v0.12",
            artifact_path="reports/conformance/runtime-next.json",
        ),
    ]

    assert build_profiles_index(records) == [
        {
            "profile_id": "parser",
            "artifact_count": 1,
            "releases": [
                {
                    "release_id": "v0.11",
                    "artifact_count": 1,
                    "artifact_paths": ["reports/conformance/parser.json"],
                }
            ],
        },
        {
            "profile_id": "runtime",
            "artifact_count": 2,
            "releases": [
                {
                    "release_id": "v0.11",
                    "artifact_count": 1,
                    "artifact_paths": ["reports/conformance/runtime.json"],
                },
                {
                    "release_id": "v0.12",
                    "artifact_count": 1,
                    "artifact_paths": ["reports/conformance/runtime-next.json"],
                },
            ],
        },
    ]
    assert build_releases_index(records)[0]["profiles"][0]["profile_id"] == "parser"
    payload = build_index_payload(
        records=records,
        input_root=ROOT / "reports" / "conformance",
        output_path=None,
        release_label=None,
        generated_at="2026-05-20T00:00:00Z",
    )
    assert payload["support_claim_traceability"]["row_count"] >= 1
    assert (
        "objc3c.behavior.runtime.object-model-interface-method-table"
        in payload["support_claim_traceability"]["support_claims"]
    )
