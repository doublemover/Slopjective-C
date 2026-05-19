"""Public argument surface for the objc3c workflow CLI."""

from __future__ import annotations

from .argument_option_contracts import (
    ARGUMENT_OPTION_CONTRACT_ID,
    ARGUMENT_OPTION_OWNER_SURFACE,
    DESCRIBE_ACTION_OPTION,
    DESCRIBE_PACKAGE_SCRIPT_OPTION,
    LIST_ACTIONS_OPTION,
    WORKFLOW_ARGUMENT_OPTIONS,
    WorkflowArgumentOption,
    workflow_argument_option,
    workflow_argument_option_usage,
    workflow_argument_options_payload,
)
from .argument_option_requests import (
    ARGUMENT_OPTION_REQUEST_CONTRACT_ID,
    ARGUMENT_OPTION_REQUEST_OWNER_SURFACE,
    WORKFLOW_ARGUMENT_REQUEST_FACTORIES,
    WorkflowArgumentRequestFactory,
    parse_workflow_option_request,
    workflow_argument_request_factories_payload,
)
from .argument_parser import parse_workflow_args
from .argument_requests import (
    DescribeActionRequest,
    DescribePackageScriptRequest,
    ExecuteActionRequest,
    ListActionsRequest,
    WorkflowRequest,
    WorkflowUsageError,
)
from .argument_usage import (
    ARGUMENT_USAGE_CONTRACT_ID,
    ARGUMENT_USAGE_OWNER_SURFACE,
    usage_contract_payload,
    usage_text,
)


__all__ = [
    "ARGUMENT_OPTION_CONTRACT_ID",
    "ARGUMENT_OPTION_OWNER_SURFACE",
    "ARGUMENT_OPTION_REQUEST_CONTRACT_ID",
    "ARGUMENT_OPTION_REQUEST_OWNER_SURFACE",
    "ARGUMENT_USAGE_CONTRACT_ID",
    "ARGUMENT_USAGE_OWNER_SURFACE",
    "DescribeActionRequest",
    "DescribePackageScriptRequest",
    "DESCRIBE_ACTION_OPTION",
    "DESCRIBE_PACKAGE_SCRIPT_OPTION",
    "ExecuteActionRequest",
    "LIST_ACTIONS_OPTION",
    "ListActionsRequest",
    "WORKFLOW_ARGUMENT_OPTIONS",
    "WORKFLOW_ARGUMENT_REQUEST_FACTORIES",
    "WorkflowArgumentOption",
    "WorkflowArgumentRequestFactory",
    "WorkflowRequest",
    "WorkflowUsageError",
    "parse_workflow_option_request",
    "parse_workflow_args",
    "usage_contract_payload",
    "usage_text",
    "workflow_argument_option",
    "workflow_argument_request_factories_payload",
    "workflow_argument_option_usage",
    "workflow_argument_options_payload",
]
