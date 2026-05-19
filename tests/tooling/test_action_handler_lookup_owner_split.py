from __future__ import annotations

import pytest

from scripts.objc3c_workflow.action_handler_lookup import (
    registered_action_handler,
    require_registered_action_handler,
)
from scripts.objc3c_workflow.action_handler_lookup_policy import (
    ACTION_HANDLER_LOOKUP_OWNER,
    ACTION_HANDLER_REQUIRED_LOOKUP_OWNER,
    maybe_action_handler,
    required_action_handler,
)
from scripts.objc3c_workflow.action_handlers import ACTION_HANDLERS


def _handler(_: list[str]) -> int:
    return 0


def test_action_handler_lookup_policy_owns_optional_and_required_lookup() -> None:
    handlers = {"owned": _handler}

    assert ACTION_HANDLER_LOOKUP_OWNER == "objc3c-workflow-action-handler-lookup"
    assert ACTION_HANDLER_REQUIRED_LOOKUP_OWNER == "objc3c-workflow-action-handler-required-lookup"
    assert maybe_action_handler(handlers, "owned") is _handler
    assert maybe_action_handler(handlers, "missing") is None
    assert required_action_handler(handlers, "owned") is _handler
    with pytest.raises(KeyError):
        required_action_handler(handlers, "missing")


def test_registered_action_handler_facade_delegates_to_lookup_policy() -> None:
    assert registered_action_handler("lint") is ACTION_HANDLERS["lint"]
    assert require_registered_action_handler("lint") is ACTION_HANDLERS["lint"]
    assert registered_action_handler("missing-action") is None
