"""Mixed-image interop packaging semantic acceptance case."""

from __future__ import annotations

import json
from pathlib import Path

from ..expectation_matching import expect
from ..case_result import CaseResult
from ..runtime_contract_interop import (
    INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
    INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE,
)
from ..fixture_compilation import compile_fixture_expect_failure
from ..fixture_compilation import compile_fixture_with_args
from ..paths import ROOT


def check_mixed_image_interop_semantics_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "mixed-image-interop-semantics"
    provider_fixture = ROOT / Path(INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE)

    provider_compile_dir = case_dir / "provider"
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_import_surface = provider_compile_dir / "module.runtime-import-surface.json"
    provider_bridge_json = json.loads(
        (provider_compile_dir / "module.interop-bridge.json").read_text(
            encoding="utf-8"
        )
    )
    provider_import_payload = json.loads(
        provider_import_surface.read_text(encoding="utf-8")
    )

    consumer_compile_dir = case_dir / "consumer"
    compile_fixture_with_args(
        consumer_fixture,
        consumer_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_import_surface),
        ],
    )
    consumer_registration_manifest = json.loads(
        (
            consumer_compile_dir / "module.runtime-registration-manifest.json"
        ).read_text(encoding="utf-8")
    )
    link_plan_path = consumer_compile_dir / "module.cross-module-runtime-link-plan.json"
    link_plan = json.loads(link_plan_path.read_text(encoding="utf-8"))

    expect(
        link_plan.get("interop_header_module_bridge_imported_module_count") == 1
        and link_plan.get("interop_ffi_imported_module_count") == 1,
        "expected mixed-image interop link plan to preserve one imported bridge/ffi provider module",
    )
    expect(
        link_plan.get("module_image_count") == 2
        and link_plan.get("ready") is True,
        "expected mixed-image interop link plan to publish a ready two-image package topology",
    )
    expect(
        link_plan.get("expected_interop_bridge_header_artifact_relative_path")
        == "module.interop-bridge.h"
        and link_plan.get("expected_interop_bridge_module_artifact_relative_path")
        == "module.interop-bridge.modulemap"
        and link_plan.get("expected_interop_bridge_artifact_relative_path")
        == "module.interop-bridge.json",
        "expected mixed-image interop link plan to preserve the canonical bridge artifact paths",
    )

    local_module = link_plan.get("local_module", {})
    imported_modules = link_plan.get("imported_modules", [])
    expect(
        isinstance(local_module, dict)
        and isinstance(imported_modules, list)
        and len(imported_modules) == 1,
        "expected mixed-image interop link plan to publish one local module and one imported module summary",
    )
    imported_module = imported_modules[0]
    expect(
        local_module.get("translation_unit_registration_order_ordinal") == 2
        and consumer_registration_manifest.get("translation_unit_registration_order_ordinal") == 2,
        "expected mixed-image interop consumer compile to preserve registration ordinal 2",
    )
    expect(
        imported_module.get("translation_unit_registration_order_ordinal") == 1,
        "expected mixed-image interop link plan to preserve the imported provider registration ordinal",
    )
    expect(
        imported_module.get("interop_header_module_bridge_contract_id")
        == link_plan.get("expected_interop_header_module_bridge_contract_id")
        == "objc3c.interop.header.module.and.bridge.generation.v1",
        "expected mixed-image interop link plan to preserve the imported bridge-generation contract",
    )
    expect(
        imported_module.get("interop_ffi_contract_id")
        == link_plan.get("expected_interop_ffi_contract_id")
        == "objc3c.interop.ffi.metadata.interface.preservation.v1",
        "expected mixed-image interop link plan to preserve the imported ffi metadata/interface preservation contract",
    )
    expect(
        imported_module.get("interop_ffi_preservation_contract_id")
        == link_plan.get("expected_interop_ffi_preservation_contract_id")
        == "objc3c.interop.foreign.surface.interface.preservation.v1",
        "expected mixed-image interop link plan to preserve the imported foreign surface/interface preservation contract",
    )
    expect(
        imported_module.get("interop_bridge_header_artifact_relative_path")
        == provider_bridge_json.get("header_artifact_relative_path")
        == "module.interop-bridge.h",
        "expected mixed-image interop link plan to preserve the provider header bridge path",
    )
    expect(
        imported_module.get("interop_header_module_bridge_cross_module_packaging_ready")
        is True
        and imported_module.get("interop_header_module_bridge_runtime_generation_ready")
        is True
        and provider_import_payload.get(
            "objc_interop_header_module_and_bridge_generation", {}
        ).get("cross_module_packaging_ready")
        is True,
        "expected mixed-image interop provider and consumer artifacts to agree on bridge packaging readiness",
    )

    duplicate_ordinal_negative = compile_fixture_expect_failure(
        consumer_fixture,
        case_dir / "negative-duplicate-registration-order",
        expected_snippets=[
            "cross-module runtime link-plan duplicate registration order ordinal: 1"
        ],
        expected_codes=[],
        extra_args=[
            "--objc3-bootstrap-registration-order-ordinal",
            "1",
            "--objc3-import-runtime-surface",
            str(provider_import_surface),
        ],
        allow_missing_structured_diagnostics=True,
    )

    return CaseResult(
        case_id="mixed-image-interop-semantics",
        probe="compile-runtime-import-surface-link-plan-and-fail-closed-diagnostics",
        fixture=INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "provider_runtime_import_surface_path": str(
                provider_import_surface.relative_to(ROOT)
            ).replace("\\", "/"),
            "link_plan_path": str(link_plan_path.relative_to(ROOT)).replace("\\", "/"),
            "module_image_count": link_plan.get("module_image_count"),
            "imported_module_count": len(imported_modules),
            "duplicate_registration_order_returncode": duplicate_ordinal_negative[
                "returncode"
            ],
            "duplicate_registration_order_diagnostics_path": duplicate_ordinal_negative[
                "diagnostics_path"
            ],
        },
    )


__all__ = ["check_mixed_image_interop_semantics_case"]
