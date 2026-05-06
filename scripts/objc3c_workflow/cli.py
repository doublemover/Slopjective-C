"""Command-line entrypoint for the objc3c workflow action registry."""

from __future__ import annotations

import sys
from collections.abc import Sequence

from .environment import WORKFLOW_COMMAND_TEXT
from .npm_surface import describe_package_script_payload
from .registry import ACTION_SPECS
from .reports import emit_json
from .runner import describe_action_payload, execute_registered_action, list_actions_payload


def main(argv: Sequence[str] | None = None) -> int:
    args = list(sys.argv[1:] if argv is None else argv)
    if not args:
        print(
            f"usage: {WORKFLOW_COMMAND_TEXT} <action> [args...]\n"
            f"       {WORKFLOW_COMMAND_TEXT} --list-json\n"
            f"       {WORKFLOW_COMMAND_TEXT} --describe <action>\n"
            f"       {WORKFLOW_COMMAND_TEXT} --describe-script <package-script>",
            file=sys.stderr,
        )
        return 2

    action, *rest = args
    if action == "--list-json":
        return emit_json(list_actions_payload())
    if action == "--describe":
        if len(rest) != 1:
            print(f"usage: {WORKFLOW_COMMAND_TEXT} --describe <action>", file=sys.stderr)
            return 2
        describe_action = rest[0]
        if describe_action not in ACTION_SPECS:
            print(f"unknown action: {describe_action}", file=sys.stderr)
            return 2
        return emit_json(describe_action_payload(describe_action))
    if action == "--describe-script":
        if len(rest) != 1:
            print(f"usage: {WORKFLOW_COMMAND_TEXT} --describe-script <package-script>", file=sys.stderr)
            return 2
        describe_script = rest[0]
        if describe_script != "objc3c":
            print(f"unknown package script: {describe_script}", file=sys.stderr)
            return 2
        return emit_json(describe_package_script_payload(describe_script))
    return execute_registered_action(action, rest)
