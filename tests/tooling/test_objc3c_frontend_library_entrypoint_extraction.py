from __future__ import annotations

from objc3c_frontend_library_entrypoint_extraction_behavior import (
    assert_cli_frontend_exports_reusable_pipeline_compile_product,
    assert_frontend_anchor_compile_entrypoints_are_pipeline_backed,
    assert_public_api_documents_pipeline_backed_compile_behavior,
)


def test_frontend_anchor_compile_entrypoints_are_pipeline_backed() -> None:
    assert_frontend_anchor_compile_entrypoints_are_pipeline_backed()


def test_cli_frontend_exports_reusable_pipeline_compile_product() -> None:
    assert_cli_frontend_exports_reusable_pipeline_compile_product()


def test_public_api_documents_pipeline_backed_compile_behavior() -> None:
    assert_public_api_documents_pipeline_backed_compile_behavior()
