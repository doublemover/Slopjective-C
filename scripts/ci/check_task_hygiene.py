#!/usr/bin/env python3
"""Fail-closed task-hygiene wiring checks for M155/M156 lane-E closeout."""

from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read_package_scripts() -> dict[str, str]:
    payload = json.loads(PACKAGE_JSON.read_text(encoding="utf-8"))
    scripts = payload.get("scripts")
    if not isinstance(scripts, dict):
        raise ValueError("package.json scripts field must be an object")
    return {str(key): str(value) for key, value in scripts.items()}


def _check_package_contracts(scripts: dict[str, str]) -> list[str]:
    errors: list[str] = []

    task_hygiene = scripts.get("check:task-hygiene", "")
    if "check:compiler-closeout:m155" not in task_hygiene:
        errors.append(
            "package.json scripts.check:task-hygiene must include check:compiler-closeout:m155",
        )
    if "check:compiler-closeout:m156" not in task_hygiene:
        errors.append(
            "package.json scripts.check:task-hygiene must include check:compiler-closeout:m156",
        )

    m155_closeout = scripts.get("check:compiler-closeout:m155", "")
    if "npm run check:objc3c:m155-id-class-sel-object-pointer-typecheck-contracts" not in m155_closeout:
        errors.append(
            "package.json scripts.check:compiler-closeout:m155 must run check:objc3c:m155-id-class-sel-object-pointer-typecheck-contracts",
        )
    if "python scripts/ci/check_task_hygiene.py" not in m155_closeout:
        errors.append(
            "package.json scripts.check:compiler-closeout:m155 must run python scripts/ci/check_task_hygiene.py",
        )

    m155_gate = scripts.get("check:objc3c:m155-id-class-sel-object-pointer-typecheck-contracts", "")
    required_gate_tests = (
        "test_objc3c_m155_frontend_id_class_sel_object_pointer_typecheck_contract.py",
        "test_objc3c_m155_sema_id_class_sel_object_pointer_typecheck_contract.py",
        "test_objc3c_m155_lowering_id_class_sel_object_pointer_typecheck_contract.py",
        "test_objc3c_m155_validation_id_class_sel_object_pointer_typecheck_contract.py",
        "test_objc3c_m155_integration_id_class_sel_object_pointer_typecheck_contract.py",
    )
    for required_test in required_gate_tests:
        if required_test not in m155_gate:
            errors.append(
                "package.json scripts.check:objc3c:m155-id-class-sel-object-pointer-typecheck-contracts "
                f"must include {required_test}",
            )

    m156_closeout = scripts.get("check:compiler-closeout:m156", "")
    if "npm run check:objc3c:m156-message-send-selector-lowering-contracts" not in m156_closeout:
        errors.append(
            "package.json scripts.check:compiler-closeout:m156 must run check:objc3c:m156-message-send-selector-lowering-contracts",
        )
    if "python scripts/ci/check_task_hygiene.py" not in m156_closeout:
        errors.append(
            "package.json scripts.check:compiler-closeout:m156 must run python scripts/ci/check_task_hygiene.py",
        )

    m156_gate = scripts.get("check:objc3c:m156-message-send-selector-lowering-contracts", "")
    required_m156_gate_tests = (
        "test_objc3c_m156_frontend_message_send_selector_lowering_contract.py",
        "test_objc3c_m156_sema_message_send_selector_lowering_contract.py",
        "test_objc3c_m156_lowering_message_send_selector_lowering_contract.py",
        "test_objc3c_m156_validation_message_send_selector_lowering_contract.py",
        "test_objc3c_m156_integration_message_send_selector_lowering_contract.py",
    )
    for required_test in required_m156_gate_tests:
        if required_test not in m156_gate:
            errors.append(
                "package.json scripts.check:objc3c:m156-message-send-selector-lowering-contracts "
                f"must include {required_test}",
            )

    return errors


def _check_workflow_contracts(workflow_text: str) -> list[str]:
    errors: list[str] = []
    required_runs = (
        "run: npm run check:compiler-closeout:m155",
        "run: npm run check:objc3c:m155-id-class-sel-object-pointer-typecheck-contracts",
        "run: npm run check:compiler-closeout:m156",
        "run: npm run check:objc3c:m156-message-send-selector-lowering-contracts",
    )
    for required in required_runs:
        if required not in workflow_text:
            errors.append(
                f".github/workflows/task-hygiene.yml must include `{required}`",
            )
    return errors


def main() -> int:
    scripts = _read_package_scripts()
    workflow_text = TASK_HYGIENE_WORKFLOW.read_text(encoding="utf-8")

    errors = _check_package_contracts(scripts)
    errors.extend(_check_workflow_contracts(workflow_text))

    if errors:
        print("M155/M156 task-hygiene contract check failed:")
        for error in errors:
            print(f"- {error}")
        return 1

    print("M155/M156 task-hygiene contract check passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
