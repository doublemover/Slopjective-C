from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from objc3c_performance_dashboard.input_loading import contract_id
from objc3c_performance_dashboard.input_loading import require_json
from objc3c_performance_dashboard.paths import BUDGET_MODEL_PATH
from objc3c_performance_dashboard.paths import CLAIM_POLICY_PATH
from objc3c_performance_dashboard.paths import COMPARATIVE_SUMMARY_PATH
from objc3c_performance_dashboard.paths import COMPILER_INTEGRATION_PATH
from objc3c_performance_dashboard.paths import COMPILER_SUMMARY_PATH
from objc3c_performance_dashboard.paths import LAB_POLICY_PATH
from objc3c_performance_dashboard.paths import PERFORMANCE_INTEGRATION_PATH
from objc3c_performance_dashboard.paths import PERFORMANCE_SUMMARY_PATH
from objc3c_performance_dashboard.paths import RUNTIME_INTEGRATION_PATH
from objc3c_performance_dashboard.paths import RUNTIME_SUMMARY_PATH
from objc3c_performance_dashboard.paths import SOURCE_SURFACE_PATH
from objc3c_performance_dashboard.paths import TRIAGE_POLICY_PATH
from objc3c_performance_dashboard.paths import WAIVERS_PATH
from objc3c_performance_dashboard.paths import WORKFLOW_SURFACE_PATH


@dataclass(frozen=True)
class DashboardInputs:
    source_surface: dict[str, Any]
    budget_model: dict[str, Any]
    claim_policy: dict[str, Any]
    triage_policy: dict[str, Any]
    lab_policy: dict[str, Any]
    waivers_payload: dict[str, Any]
    workflow_surface: dict[str, Any]
    performance_summary: dict[str, Any]
    performance_integration: dict[str, Any]
    comparative_summary: dict[str, Any]
    compiler_summary: dict[str, Any]
    compiler_integration: dict[str, Any]
    runtime_summary: dict[str, Any]
    runtime_integration: dict[str, Any]


def load_dashboard_inputs() -> DashboardInputs:
    return DashboardInputs(
        source_surface=require_json(SOURCE_SURFACE_PATH, kind="source surface"),
        budget_model=require_json(BUDGET_MODEL_PATH, kind="budget model"),
        claim_policy=require_json(CLAIM_POLICY_PATH, kind="claim policy"),
        triage_policy=require_json(TRIAGE_POLICY_PATH, kind="breach triage policy"),
        lab_policy=require_json(LAB_POLICY_PATH, kind="lab policy"),
        waivers_payload=require_json(WAIVERS_PATH, kind="waiver registry"),
        workflow_surface=require_json(WORKFLOW_SURFACE_PATH, kind="workflow surface"),
        performance_summary=require_json(PERFORMANCE_SUMMARY_PATH, kind="performance benchmark summary"),
        performance_integration=require_json(PERFORMANCE_INTEGRATION_PATH, kind="performance integration summary"),
        comparative_summary=require_json(COMPARATIVE_SUMMARY_PATH, kind="comparative baseline summary"),
        compiler_summary=require_json(COMPILER_SUMMARY_PATH, kind="compiler throughput summary"),
        compiler_integration=require_json(COMPILER_INTEGRATION_PATH, kind="compiler throughput integration summary"),
        runtime_summary=require_json(RUNTIME_SUMMARY_PATH, kind="runtime performance summary"),
        runtime_integration=require_json(RUNTIME_INTEGRATION_PATH, kind="runtime performance integration summary"),
    )


def build_policy_contracts(inputs: DashboardInputs) -> dict[str, str]:
    return {
        "source_surface": contract_id(inputs.source_surface),
        "budget_model": contract_id(inputs.budget_model),
        "claim_policy": contract_id(inputs.claim_policy),
        "breach_triage_policy": contract_id(inputs.triage_policy),
        "lab_policy": contract_id(inputs.lab_policy),
        "waiver_registry": contract_id(inputs.waivers_payload),
        "workflow_surface": contract_id(inputs.workflow_surface),
    }


def build_upstream_report_contracts(inputs: DashboardInputs) -> dict[str, str]:
    return {
        "performance_summary": contract_id(inputs.performance_summary),
        "performance_integration": contract_id(inputs.performance_integration),
        "comparative_summary": contract_id(inputs.comparative_summary),
        "compiler_summary": contract_id(inputs.compiler_summary),
        "compiler_integration": contract_id(inputs.compiler_integration),
        "runtime_summary": contract_id(inputs.runtime_summary),
        "runtime_integration": contract_id(inputs.runtime_integration),
    }
