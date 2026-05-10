from scripts.objc3c_workflow.actions.runtime_acceptance_command_runner import (
    runtime_acceptance_command,
)
from scripts.objc3c_workflow.actions.runtime_acceptance_routes import (
    runtime_acceptance_command as public_runtime_acceptance_command,
)

from runtime_acceptance_route_owner_split_support import (
    custom_runtime_acceptance_script,
)


def runtime_acceptance_command_uses_explicit_script_path() -> None:
    script = custom_runtime_acceptance_script()

    command = runtime_acceptance_command(
        "test-runtime-acceptance-fast",
        runtime_acceptance_script=script,
    )

    assert command[-3:] == [str(script), "--suite", "fast"]
    assert public_runtime_acceptance_command is runtime_acceptance_command
