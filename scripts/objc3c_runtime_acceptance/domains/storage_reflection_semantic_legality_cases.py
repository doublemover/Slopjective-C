"""Storage/reflection semantic legality acceptance facade."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_outputs
from objc3c_runtime_acceptance.fixture_compilation import compile_negative_diagnostic_batch

from .storage_reflection_semantic_legality_assertions import (
    expect_registration_manifest_published,
    expect_storage_legality_positive_evidence,
)
from .storage_reflection_semantic_legality_catalog import (
    STORAGE_LEGALITY_POSITIVE_FIXTURE,
    STORAGE_LEGALITY_REGISTRATION_MANIFEST_NAME,
    STORAGE_LEGALITY_SEMANTICS_CASE_ID,
    storage_legality_negative_expectations,
)
from .storage_reflection_semantic_legality_payloads import storage_legality_case_result
from .storage_reflection_semantic_legality_predicates import (
    diagnostic_results_by_key,
    read_json_object,
    semantic_pass_manager_manifest,
)


def check_storage_legality_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / STORAGE_LEGALITY_SEMANTICS_CASE_ID
    _, ll_path, manifest_path = compile_fixture_outputs(
        STORAGE_LEGALITY_POSITIVE_FIXTURE,
        case_dir / "positive",
    )
    registration_manifest_path = (
        case_dir / "positive" / STORAGE_LEGALITY_REGISTRATION_MANIFEST_NAME
    )
    expect_registration_manifest_published(registration_manifest_path)
    registration_manifest = read_json_object(registration_manifest_path)
    manifest = read_json_object(manifest_path)
    sema_manifest = semantic_pass_manager_manifest(manifest)
    expect_storage_legality_positive_evidence(
        registration_manifest,
        manifest,
        sema_manifest,
        ll_path.read_text(encoding="utf-8"),
    )

    negative_batch = compile_negative_diagnostic_batch(
        case_id=STORAGE_LEGALITY_SEMANTICS_CASE_ID,
        out_dir=case_dir / "negative-diagnostics-batch",
        expectations=storage_legality_negative_expectations(),
    )
    return storage_legality_case_result(
        registration_manifest,
        sema_manifest,
        diagnostic_results_by_key(negative_batch),
        negative_batch,
    )
