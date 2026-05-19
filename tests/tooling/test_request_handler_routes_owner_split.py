from __future__ import annotations

from scripts.objc3c_workflow.argument_request_model import (
    DescribeActionRequest,
    DescribePackageScriptRequest,
    ExecuteActionRequest,
    ListActionsRequest,
)
from scripts.objc3c_workflow.request_handler_actions import (
    handle_describe_action_request,
    handle_execute_action_request,
    handle_list_actions_request,
)
from scripts.objc3c_workflow.request_handler_package_scripts import handle_describe_package_script_request
from scripts.objc3c_workflow.request_handler_routes import (
    REQUEST_HANDLER_ROUTE_OWNER,
    REQUEST_HANDLER_ROUTES,
    REQUEST_HANDLER_UNHANDLED_OWNER,
    handler_for_request,
    unhandled_workflow_request_message,
)


def test_request_handler_routes_own_dispatch_order_and_handlers() -> None:
    assert REQUEST_HANDLER_ROUTE_OWNER == "objc3c-workflow-request-handler-routes"
    assert REQUEST_HANDLER_UNHANDLED_OWNER == "objc3c-workflow-request-handler-unhandled"
    assert [(route.request_type, route.handler, route.owner) for route in REQUEST_HANDLER_ROUTES] == [
        (ListActionsRequest, handle_list_actions_request, REQUEST_HANDLER_ROUTE_OWNER),
        (DescribeActionRequest, handle_describe_action_request, REQUEST_HANDLER_ROUTE_OWNER),
        (DescribePackageScriptRequest, handle_describe_package_script_request, REQUEST_HANDLER_ROUTE_OWNER),
        (ExecuteActionRequest, handle_execute_action_request, REQUEST_HANDLER_ROUTE_OWNER),
    ]

    assert handler_for_request(ListActionsRequest()) is handle_list_actions_request
    assert handler_for_request(DescribeActionRequest("lint")) is handle_describe_action_request
    assert handler_for_request(DescribePackageScriptRequest("objc3c")) is handle_describe_package_script_request
    assert handler_for_request(ExecuteActionRequest("lint", [])) is handle_execute_action_request


def test_unhandled_request_message_is_owned() -> None:
    assert unhandled_workflow_request_message(ListActionsRequest()) == (
        "unhandled workflow request: ListActionsRequest()"
    )
