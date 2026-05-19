"""Package-script workflow request handlers."""

from __future__ import annotations

from .argument_request_model import DescribePackageScriptRequest
from .npm_surface import describe_package_script_payload
from .npm_surface_policy import package_script_is_registered
from .public_bridge import PACKAGE_BRIDGES
from .reports import emit_json
from .request_unknown_errors import emit_unknown_package_script


def handle_describe_package_script_request(
    request: DescribePackageScriptRequest,
) -> int:
    if not package_script_is_registered(PACKAGE_BRIDGES, request.package_script):
        return emit_unknown_package_script(request.package_script)
    return emit_json(describe_package_script_payload(request.package_script))


__all__ = ["handle_describe_package_script_request"]
