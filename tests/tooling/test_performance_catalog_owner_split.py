from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow.action_catalog_performance_benchmarks import (
    PERFORMANCE_BENCHMARK_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_performance_core_benchmarks import (
    PERFORMANCE_CORE_BENCHMARK_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_performance_governance import (
    PERFORMANCE_GOVERNANCE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_performance_packaging import (
    PERFORMANCE_PACKAGING_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_performance_runtime import (
    PERFORMANCE_RUNTIME_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_performance_throughput import (
    PERFORMANCE_THROUGHPUT_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_public_conformance_reporting import (
    PUBLIC_CONFORMANCE_REPORTING_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_public_performance import (
    PUBLIC_REPORTING_AND_PERFORMANCE_ACTION_SPECS,
)

ROOT = Path(__file__).resolve().parents[2]
WORKFLOW_ROOT = ROOT / "scripts" / "objc3c_workflow"


def test_public_performance_catalog_is_owner_facade() -> None:
    facade_text = (WORKFLOW_ROOT / "action_catalog_public_performance.py").read_text(
        encoding="utf-8"
    )

    for module_name in (
        "action_catalog_public_conformance_reporting",
        "action_catalog_performance_governance",
    ):
        assert importlib.import_module(f"scripts.objc3c_workflow.{module_name}")
        assert f"from .{module_name} import" in facade_text
    assert "ActionSpec(" not in facade_text


def test_performance_benchmark_catalog_is_owner_facade() -> None:
    facade_text = (
        WORKFLOW_ROOT / "action_catalog_performance_benchmarks.py"
    ).read_text(encoding="utf-8")

    for module_name in (
        "action_catalog_performance_core_benchmarks",
        "action_catalog_performance_runtime",
        "action_catalog_performance_throughput",
        "action_catalog_performance_packaging",
    ):
        assert importlib.import_module(f"scripts.objc3c_workflow.{module_name}")
        assert f"from .{module_name} import" in facade_text
    assert "ActionSpec(" not in facade_text


def test_performance_catalogs_preserve_owner_aggregation() -> None:
    assert PUBLIC_REPORTING_AND_PERFORMANCE_ACTION_SPECS == {
        **PUBLIC_CONFORMANCE_REPORTING_ACTION_SPECS,
        **PERFORMANCE_GOVERNANCE_ACTION_SPECS,
    }
    assert PERFORMANCE_BENCHMARK_ACTION_SPECS == {
        **PERFORMANCE_CORE_BENCHMARK_ACTION_SPECS,
        **PERFORMANCE_RUNTIME_ACTION_SPECS,
        **PERFORMANCE_THROUGHPUT_ACTION_SPECS,
        **PERFORMANCE_PACKAGING_ACTION_SPECS,
    }
    assert "validate-performance-governance" in (
        PUBLIC_REPORTING_AND_PERFORMANCE_ACTION_SPECS
    )
    assert "validate-runtime-performance" in PERFORMANCE_BENCHMARK_ACTION_SPECS
