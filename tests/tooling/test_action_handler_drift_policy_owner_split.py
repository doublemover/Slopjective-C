from __future__ import annotations

from scripts.objc3c_workflow.action_handler_drift import ActionHandlerDrift
from scripts.objc3c_workflow.action_handler_drift_policy import (
    ACTION_HANDLER_COMPLETENESS_POLICY_OWNER,
    ACTION_HANDLER_DRIFT_POLICY_OWNER,
    handler_registry_complete,
    sorted_missing_handlers,
    sorted_orphan_handlers,
)


def test_action_handler_drift_policy_owns_missing_orphan_and_complete_rules() -> None:
    catalog_actions = ["build", "lint", "test-full"]
    handler_actions = ["lint", "test-full", "orphan"]

    assert ACTION_HANDLER_DRIFT_POLICY_OWNER == "objc3c-workflow-action-handler-drift-policy"
    assert ACTION_HANDLER_COMPLETENESS_POLICY_OWNER == "objc3c-workflow-action-handler-completeness-policy"
    assert sorted_missing_handlers(catalog_actions, handler_actions) == ["build"]
    assert sorted_orphan_handlers(catalog_actions, handler_actions) == ["orphan"]
    assert handler_registry_complete([], []) is True
    assert handler_registry_complete(["build"], []) is False
    assert ActionHandlerDrift([], []).complete is True
    assert ActionHandlerDrift(["build"], []).complete is False
