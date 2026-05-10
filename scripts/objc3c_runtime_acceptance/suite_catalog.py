"""Named runtime acceptance suite membership."""

from __future__ import annotations

RUNTIME_ACCEPTANCE_SUITE_CASES: dict[str, tuple[str, ...]] = {
    "full": (),
    "helpers": (
        "runtime-probe-helper-support",
    ),
    "fast": (
        "runtime-library",
        "runtime-probe-helper-support",
        "compile-backend-parity",
        "artifact-registry-key-isolation",
        "installation-lifecycle",
        "multi-image-registration-reset-replay",
        "metaprogramming-source-surface",
        "live-metaprogramming-cache-runtime-integration",
        "cross-module-runtime-package-interop-source-surface",
        "unified-concurrency-runtime-abi",
        "live-error-runtime-integration",
        "canonical-dispatch",
        "metaclass-graph-root-class",
        "live-dispatch-fast-path",
        "storage-ownership-reflection",
        "accessor-storage-lowering-metadata-surface",
        "property-layout",
        "instance-allocation-layout-runtime",
        "escaping-block-capture-legality",
        "block-arc-runtime-abi",
        "arc-property-helper",
    ),
    "diagnostics": (
        "metaprogramming-derive-property-behavior-semantics",
        "metaprogramming-macro-safety-cache-diagnostics",
        "import-version-feature-claim-diagnostics",
        "scaffold-retirement-deprecated-sidecar-compatibility-diagnostics",
        "executable-try-throw-do-catch-semantics",
        "bridging-filter-unwind-compatibility-diagnostics",
        "property-reflection-accessor-compatibility-diagnostics",
        "property-synthesis-storage-binding-semantics",
        "storage-legality-semantics",
        "escaping-block-capture-legality",
        "block-storage-arc-automation-semantics",
    ),
    "cross-module": (
        "cross-module-metaprogramming-artifact-preservation",
        "cross-module-runtime-package-interop-source-surface",
        "mixed-image-compatibility-interop-semantics",
        "c-cpp-swift-interop-boundary-semantics",
        "runtime-packaging-bridge-loader-artifact-surface",
        "mixed-image-package-lowering-bridge-emission",
        "cross-language-replay-import-surface-preservation",
        "live-package-loading-interop-runtime-implementation",
        "cross-module-error-metadata-replay-preservation",
        "cross-module-concurrency-actor-artifact-preservation",
        "cross-module-block-ownership-artifact-preservation",
        "cross-module-storage-reflection-artifact-preservation",
        "imported-runtime-packaging-replay",
        "multi-image-registration-reset-replay",
    ),
    "block-arc": (
        "escaping-block-capture-legality",
        "block-storage-arc-automation-semantics",
        "block-arc-runtime-abi",
        "block-helper-runtime-execution",
        "arc-property-helper",
        "cross-module-block-ownership-artifact-preservation",
    ),
    "concurrency": (
        "unified-concurrency-runtime-architecture",
        "async-task-actor-normalization-completion",
        "unified-concurrency-lowering-metadata-surface",
        "unified-concurrency-runtime-abi",
        "live-unified-concurrency-runtime-implementation",
        "cross-module-concurrency-actor-artifact-preservation",
    ),
}


def available_suite_payload(
    case_factories: list[tuple[str, object]],
) -> dict[str, list[str]]:
    all_case_labels = [label for label, _ in case_factories]
    return {
        suite: list(cases) if cases else all_case_labels
        for suite, cases in RUNTIME_ACCEPTANCE_SUITE_CASES.items()
    }


__all__ = ["RUNTIME_ACCEPTANCE_SUITE_CASES", "available_suite_payload"]
