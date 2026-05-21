"""Package public workflow action contract helpers."""

from __future__ import annotations

from dataclasses import dataclass

from .action_spec import ActionSpec

PACKAGE_PUBLIC_COMMAND_PREFIX = "npm run objc3c --"


@dataclass(frozen=True)
class PackageSchemaContract:
    contract_key: str
    schema_path: str
    schema_id: str
    document_contract_id: str


@dataclass(frozen=True)
class PackagePublicWorkflowAction:
    action: str
    summary: str
    script_path: str
    validation_tier: str
    guarantee_owner: str
    schema_contracts: tuple[PackageSchemaContract, ...] = ()
    source_paths: tuple[str, ...] = ()
    generated_paths: tuple[str, ...] = ()
    pass_through_args: bool = False

    @property
    def backend(self) -> str:
        return f"python:{self.script_path}"

    @property
    def public_invocation(self) -> str:
        return f"{PACKAGE_PUBLIC_COMMAND_PREFIX} {self.action}"

    def action_spec(self) -> ActionSpec:
        return ActionSpec(
            self.action,
            self.summary,
            self.backend,
            validation_tier=self.validation_tier,
            guarantee_owner=self.guarantee_owner,
            pass_through_args=self.pass_through_args,
        )


def package_action_specs(
    actions: tuple[PackagePublicWorkflowAction, ...],
) -> dict[str, ActionSpec]:
    return {action.action: action.action_spec() for action in actions}


__all__ = [
    "PACKAGE_PUBLIC_COMMAND_PREFIX",
    "PackagePublicWorkflowAction",
    "PackageSchemaContract",
    "package_action_specs",
]
