"""Cross-language replay import-surface preservation acceptance case."""

from __future__ import annotations

import json
from pathlib import Path

from ..expectation_matching import expect
from ..case_result import CaseResult
from ..runtime_contract_interop import (
    INTEROP_HEADER_MODULE_CONSUMER_FIXTURE,
    INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
)
from ..fixture_compilation import compile_fixture_expect_failure
from ..fixture_compilation import compile_fixture_with_args
from ..paths import ROOT


def check_cross_language_replay_import_surface_preservation_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "cross-language-replay-import-surface-preservation"
    provider_fixture = ROOT / Path(INTEROP_HEADER_MODULE_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(INTEROP_HEADER_MODULE_CONSUMER_FIXTURE)

    provider_compile_dir = case_dir / "provider"
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_import_surface = json.loads(
        (provider_compile_dir / "module.runtime-import-surface.json").read_text(
            encoding="utf-8"
        )
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
            str(provider_compile_dir / "module.runtime-import-surface.json"),
        ],
        allow_missing_structured_diagnostics=True,
    )
    provider_ffi_surface = provider_import_surface.get(
        "objc_interop_foreign_surface_interface_and_module_preservation", {}
    )
    provider_bridge_surface = provider_import_surface.get(
        "objc_interop_header_module_and_bridge_generation", {}
    )

    expect(
        provider_ffi_surface.get("replay_key")
        and provider_ffi_surface.get("foreign_import_source_replay_key")
        and provider_ffi_surface.get("cpp_swift_source_replay_key"),
        "expected provider import surface to preserve foreign/C++/Swift replay keys",
    )
    expect(
        provider_bridge_surface.get("replay_key")
        and provider_bridge_surface.get("preservation_replay_key")
        and provider_bridge_surface.get("source_contract_id")
        == "objc3c.interop.bridge.packaging.and.toolchain.contract.v1",
        "expected provider import surface to preserve bridge replay and source contract keys",
    )
    expect(
        provider_bridge_surface.get("runtime_generation_ready") is False
        and provider_bridge_surface.get("cross_module_packaging_ready") is False
        and provider_bridge_surface.get("deterministic") is False,
        "expected replay preservation to remain deferred until bridge generation is active",
    )

    return CaseResult(
        case_id="cross-language-replay-import-surface-preservation",
        probe="compile-runtime-import-surface-replay-key-and-fail-closed-import-inspection",
        fixture=INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "provider_module_name": provider_import_surface.get("module_name"),
            "ffi_replay_key": provider_ffi_surface.get("replay_key"),
            "bridge_replay_key": provider_bridge_surface.get("replay_key"),
            "consumer_import_returncode": consumer_import_negative["returncode"],
        },
    )


__all__ = ["check_cross_language_replay_import_surface_preservation_case"]
