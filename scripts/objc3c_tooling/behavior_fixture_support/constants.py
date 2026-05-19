from __future__ import annotations

from objc3c_tooling.paths import ROOT


NATIVE_ROOT = ROOT / "tests" / "native"
FIXTURE_ROOT = ROOT / "tests" / "fixtures"

PHASE_ORDER: tuple[str, ...] = ("parser", "sema", "lowering", "ir", "runtime", "e2e")
REQUIRED_TREE: dict[str, tuple[str, ...]] = {
    "parser": ("positive", "negative", "snapshots"),
    "sema": (
        "types",
        "ownership",
        "objc",
        "control_flow",
        "errors",
        "concurrency",
        "negative",
    ),
    "lowering": ("expressions", "statements", "objc_runtime", "ownership", "errors"),
    "ir": ("module", "function", "metadata", "runtime_calls"),
    "runtime": (
        "dispatch",
        "object_model",
        "storage",
        "arc",
        "blocks",
        "errors",
        "concurrency",
    ),
    "e2e": ("smoke", "feature_matrix", "negative_execution"),
}

STRICT_KINDS = {"negative", "strict-error", "rejection"}
FIXTURE_KINDS = {"positive", *STRICT_KINDS}
EXPECTED_STAGES = {"parse", "compile", "link", "run"}
RETIRED_SURFACE_TAGS = frozenset(
    {
        "compatibility-gate",
        "old-mode",
        "retired-route",
        "runtime-adapter",
        "runtime-dispatch",
    }
)
