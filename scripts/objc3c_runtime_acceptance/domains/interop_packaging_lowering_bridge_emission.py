"""Mixed-image package lowering bridge-emission acceptance case."""

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
from ..fixture_compilation import compile_fixture_outputs_with_args
from ..paths import ROOT


def check_mixed_image_package_lowering_bridge_emission_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "mixed-image-package-lowering-bridge-emission"
    provider_fixture = ROOT / Path(INTEROP_HEADER_MODULE_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(INTEROP_HEADER_MODULE_CONSUMER_FIXTURE)

    _, provider_ll_path, _ = compile_fixture_outputs_with_args(
        provider_fixture,
        case_dir / "provider",
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    consumer_import_negative = compile_fixture_expect_failure(
        consumer_fixture,
        case_dir / "consumer",
        expected_snippets=[
            "cross-module runtime link-plan Part 11 ffi preservation surface incomplete"
        ],
        expected_codes=[],
        extra_args=[
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(case_dir / "provider" / "module.runtime-import-surface.json"),
        ],
        allow_missing_structured_diagnostics=True,
    )

    provider_ll = provider_ll_path.read_text(encoding="utf-8")
    provider_import_surface = json.loads(
        ((case_dir / "provider") / "module.runtime-import-surface.json").read_text(
            encoding="utf-8"
        )
    )
    provider_bridge_surface = provider_import_surface.get(
        "objc_interop_header_module_and_bridge_generation", {}
    )

    for needle, label in (
        ("interop_interop_lowering_abi_contract", "interop ABI lowering summary"),
        (
            "interop_ffi_metadata_interface_preservation",
            "ffi metadata/interface preservation summary",
        ),
        (
            "interop_header_module_and_bridge_generation",
            "bridge-generation lowering summary",
        ),
        ("declare i32 @ffiInbound(i32)", "imported bridge declaration"),
        ("declare i32 @ffiHeaderBridge(i32)", "local bridge declaration"),
    ):
        expect(
            needle in provider_ll,
            f"expected provider lowering to publish the {label} in LLVM IR",
        )
    expect(
        provider_bridge_surface.get("local_foreign_callable_count") == 2
        and provider_bridge_surface.get("header_artifact_relative_path")
        == "module.interop-bridge.h"
        and provider_bridge_surface.get("module_artifact_relative_path")
        == "module.interop-bridge.modulemap"
        and provider_bridge_surface.get("bridge_artifact_relative_path")
        == "module.interop-bridge.json",
        "expected provider bridge-generation surface to preserve both interop callables and deferred bridge paths",
    )
    expect(
        provider_bridge_surface.get("runtime_generation_ready") is False
        and provider_bridge_surface.get("cross_module_packaging_ready") is False
        and provider_bridge_surface.get("deterministic") is False,
        "expected provider lowering to preserve deferred bridge metadata without claiming emitted bridge artifacts",
    )

    return CaseResult(
        case_id="mixed-image-package-lowering-bridge-emission",
        probe="compile-llvm-ir-runtime-import-surface-and-fail-closed-consumer-import",
        fixture=INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "provider_ll_path": str(provider_ll_path.relative_to(ROOT)).replace(
                "\\", "/"
            ),
            "foreign_callable_count": provider_bridge_surface.get(
                "local_foreign_callable_count"
            ),
            "consumer_import_returncode": consumer_import_negative["returncode"],
        },
    )


__all__ = ["check_mixed_image_package_lowering_bridge_emission_case"]
