"""Developer tooling materialization action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

TOOLING_MATERIALIZATION_ACTION_SPECS: dict[str, ActionSpec] = {
    "materialize-project-template": ActionSpec(
        "materialize-project-template",
        "materialize a machine-owned project template from the checked-in showcase portfolio and drive the live bonus-tool demo harness against it",
        "python:scripts/materialize_objc3c_project_template.py",
        validation_tier="repo",
        guarantee_owner=(
            "starter-template and demo-harness outputs stay derived from checked-in "
            "showcase sources and executable public actions"
        ),
        pass_through_args=True,
    ),
}
