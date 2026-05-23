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
    provider_bridge_header = provider_compile_dir / "module.interop-bridge.h"
    provider_bridge_modulemap = provider_compile_dir / "module.interop-bridge.modulemap"
    provider_bridge_json = provider_compile_dir / "module.interop-bridge.json"
    provider_import_payload = json.loads(
        provider_import_surface.read_text(encoding="utf-8")
    )
    provider_bridge_packet = provider_import_payload.get(
        "objc_interop_header_module_and_bridge_generation", {}
    )
    provider_ffi_packet = provider_import_payload.get(
        "objc_interop_ffi_metadata_and_interface_preservation", {}
    )

    consumer_compile_dir = case_dir / "consumer"
    consumer_import_negative = compile_fixture_expect_failure(
        consumer_fixture,
        consumer_compile_dir,
        expected_snippets=[
            "cross-module runtime link-plan Part 11 ffi preservation surface incomplete"
        ],
        expected_codes=[],
        extra_args=[
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_import_surface),
        ],
        allow_missing_structured_diagnostics=True,
    )
    for artifact_path, label in (
        (provider_bridge_header, "bridge header"),
        (provider_bridge_modulemap, "bridge modulemap"),
        (provider_bridge_json, "bridge json"),
    ):
        expect(
            not artifact_path.exists(),
            f"expected mixed-image interop provider not to publish the deferred {label} artifact",
        )
    expect(
        provider_bridge_packet.get("contract_id")
        == "objc3c.interop.header.module.and.bridge.generation.v1",
        "expected mixed-image interop provider import surface to preserve the bridge-generation contract",
    )
    expect(
        provider_bridge_packet.get("header_artifact_relative_path")
        == "module.interop-bridge.h"
        and provider_bridge_packet.get("module_artifact_relative_path")
        == "module.interop-bridge.modulemap"
        and provider_bridge_packet.get("bridge_artifact_relative_path")
        == "module.interop-bridge.json",
        "expected mixed-image interop provider import surface to preserve canonical deferred bridge artifact paths",
    )
    expect(
        provider_bridge_packet.get("runtime_generation_ready") is False
        and provider_bridge_packet.get("cross_module_packaging_ready") is False
        and provider_bridge_packet.get("deterministic") is False
        and provider_ffi_packet.get("runtime_import_artifact_ready") is False
        and provider_ffi_packet.get("separate_compilation_preservation_ready") is False,
        "expected mixed-image interop provider import surface to fail closed until bridge and ffi artifacts are generated",
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
            "consumer_import_returncode": consumer_import_negative["returncode"],
            "consumer_import_diagnostics_path": consumer_import_negative[
                "diagnostics_path"
            ],
            "bridge_artifact_paths_deferred": [
                "module.interop-bridge.h",
                "module.interop-bridge.modulemap",
                "module.interop-bridge.json",
            ],
            "duplicate_registration_order_returncode": duplicate_ordinal_negative[
                "returncode"
            ],
            "duplicate_registration_order_diagnostics_path": duplicate_ordinal_negative[
                "diagnostics_path"
            ],
        },
    )


__all__ = ["check_mixed_image_interop_semantics_case"]
