"""Runtime packaging bridge-loader artifact-surface acceptance case."""

from __future__ import annotations

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


def check_runtime_packaging_bridge_loader_artifact_surface_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "runtime-packaging-bridge-loader-artifact-surface"
    provider_fixture = ROOT / Path(INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE)

    provider_compile_dir = case_dir / "provider"
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
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

    for artifact_name in (
        "module.interop-bridge.h",
        "module.interop-bridge.modulemap",
        "module.interop-bridge.json",
    ):
        expect(
            not (provider_compile_dir / artifact_name).is_file(),
            f"expected provider compile not to publish deferred {artifact_name}",
        )

    return CaseResult(
        case_id="runtime-packaging-bridge-loader-artifact-surface",
        probe="compile-artifact-and-fail-closed-import-inspection",
        fixture=INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "consumer_import_returncode": consumer_import_negative["returncode"],
            "bridge_artifact_paths_deferred": [
                "module.interop-bridge.h",
                "module.interop-bridge.modulemap",
                "module.interop-bridge.json",
            ],
        },
    )


__all__ = ["check_runtime_packaging_bridge_loader_artifact_surface_case"]
