"""Public option-token request construction for workflow arguments."""

from __future__ import annotations

from collections.abc import Callable, Sequence
from dataclasses import asdict, dataclass

from .argument_option_contracts import (
    ARGUMENT_OPTION_CONTRACT_ID,
    ARGUMENT_OPTION_OWNER_SURFACE,
    DESCRIBE_ACTION_OPTION,
    DESCRIBE_PACKAGE_SCRIPT_OPTION,
    LIST_ACTIONS_OPTION,
    workflow_argument_option,
    workflow_argument_option_usage,
)
from .argument_request_model import (
    DescribeActionRequest,
    DescribePackageScriptRequest,
    ListActionsRequest,
    WorkflowRequest,
)
from .argument_usage_error import WorkflowUsageError

ARGUMENT_OPTION_REQUEST_CONTRACT_ID = "objc3c-workflow-argument-option-requests-v1"
ARGUMENT_OPTION_REQUEST_OWNER_SURFACE = (
    "scripts/objc3c_workflow/argument_option_requests.py"
)


@dataclass(frozen=True)
class WorkflowArgumentRequestFactory:
    token: str
    request_kind: str
    build_request: Callable[[Sequence[str]], WorkflowRequest]


def _factory_payload(factory: WorkflowArgumentRequestFactory) -> dict[str, object]:
    fields = asdict(factory)
    fields["build_request"] = factory.build_request.__name__
    return fields


def _list_actions_request(_: Sequence[str]) -> WorkflowRequest:
    return ListActionsRequest()


def _describe_action_request(rest: Sequence[str]) -> WorkflowRequest:
    return DescribeActionRequest(rest[0])


def _describe_package_script_request(rest: Sequence[str]) -> WorkflowRequest:
    return DescribePackageScriptRequest(rest[0])


WORKFLOW_ARGUMENT_REQUEST_FACTORIES: dict[str, WorkflowArgumentRequestFactory] = {
    LIST_ACTIONS_OPTION: WorkflowArgumentRequestFactory(
        LIST_ACTIONS_OPTION,
        "list-actions",
        _list_actions_request,
    ),
    DESCRIBE_ACTION_OPTION: WorkflowArgumentRequestFactory(
        DESCRIBE_ACTION_OPTION,
        "describe-action",
        _describe_action_request,
    ),
    DESCRIBE_PACKAGE_SCRIPT_OPTION: WorkflowArgumentRequestFactory(
        DESCRIBE_PACKAGE_SCRIPT_OPTION,
        "describe-package-script",
        _describe_package_script_request,
    ),
}


def parse_workflow_option_request(
    action: str,
    rest: Sequence[str],
) -> WorkflowRequest | None:
    factory = WORKFLOW_ARGUMENT_REQUEST_FACTORIES.get(action)
    if factory is None:
        return None

    option = workflow_argument_option(action)
    if option is None:
        raise AssertionError(f"missing workflow option contract for {action}")
    if len(rest) != option.required_argument_count:
        raise WorkflowUsageError(workflow_argument_option_usage(action))
    return factory.build_request(rest)


def workflow_argument_request_factories_payload() -> dict[str, object]:
    return {
        "contract_id": ARGUMENT_OPTION_REQUEST_CONTRACT_ID,
        "owner_surface": ARGUMENT_OPTION_REQUEST_OWNER_SURFACE,
        "option_contract_id": ARGUMENT_OPTION_CONTRACT_ID,
        "option_owner_surface": ARGUMENT_OPTION_OWNER_SURFACE,
        "factories": [
            _factory_payload(factory)
            for factory in WORKFLOW_ARGUMENT_REQUEST_FACTORIES.values()
        ],
        "non_option_action_passthrough": True,
    }


__all__ = [
    "ARGUMENT_OPTION_REQUEST_CONTRACT_ID",
    "ARGUMENT_OPTION_REQUEST_OWNER_SURFACE",
    "WORKFLOW_ARGUMENT_REQUEST_FACTORIES",
    "WorkflowArgumentRequestFactory",
    "parse_workflow_option_request",
    "workflow_argument_request_factories_payload",
]
