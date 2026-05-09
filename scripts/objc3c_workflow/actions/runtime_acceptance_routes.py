"""Runtime acceptance suite routing for workflow actions."""

from __future__ import annotations

import sys
from dataclasses import dataclass

from ..commands import run
from ..environment import ROOT

RUNTIME_ACCEPTANCE_PY = ROOT / "scripts" / "check_objc3c_runtime_acceptance.py"


@dataclass(frozen=True)
class RuntimeAcceptanceRoute:
    action: str
    suite: str
    title: str
    validation_tier: str
    guarantee_owner: str

    @property
    def target(self) -> str:
        return f"python:scripts/check_objc3c_runtime_acceptance.py --suite {self.suite}"

    def args(self) -> tuple[str, ...]:
        return ("--suite", self.suite)

    def command(self) -> list[str]:
        return [sys.executable, str(RUNTIME_ACCEPTANCE_PY), *self.args()]


RUNTIME_ACCEPTANCE_ROUTES: dict[str, RuntimeAcceptanceRoute] = {
    "test-runtime-acceptance": RuntimeAcceptanceRoute(
        action="test-runtime-acceptance",
        suite="full",
        title="full runtime acceptance suite",
        validation_tier="full",
        guarantee_owner="exhaustive runtime acceptance and ABI/accessor proof",
    ),
    "test-runtime-acceptance-fast": RuntimeAcceptanceRoute(
        action="test-runtime-acceptance-fast",
        suite="fast",
        title="fast runtime acceptance suite",
        validation_tier="fast",
        guarantee_owner="high-signal runtime acceptance slice for developer validation",
    ),
    "test-runtime-acceptance-diagnostics": RuntimeAcceptanceRoute(
        action="test-runtime-acceptance-diagnostics",
        suite="diagnostics",
        title="diagnostic runtime acceptance suite",
        validation_tier="fast",
        guarantee_owner="negative diagnostics and fail-closed runtime acceptance surfaces",
    ),
    "test-runtime-acceptance-cross-module": RuntimeAcceptanceRoute(
        action="test-runtime-acceptance-cross-module",
        suite="cross-module",
        title="cross-module runtime acceptance suite",
        validation_tier="fast",
        guarantee_owner=(
            "cross-module import, replay, package, and link-plan runtime acceptance surfaces"
        ),
    ),
    "test-runtime-acceptance-block-arc": RuntimeAcceptanceRoute(
        action="test-runtime-acceptance-block-arc",
        suite="block-arc",
        title="Block/ARC runtime acceptance suite",
        validation_tier="fast",
        guarantee_owner=(
            "Block, byref, ownership transfer, and ARC runtime acceptance surfaces"
        ),
    ),
    "test-runtime-acceptance-concurrency": RuntimeAcceptanceRoute(
        action="test-runtime-acceptance-concurrency",
        suite="concurrency",
        title="concurrency runtime acceptance suite",
        validation_tier="fast",
        guarantee_owner="async/task/actor runtime acceptance surfaces",
    ),
}


def runtime_acceptance_route(action: str) -> RuntimeAcceptanceRoute:
    return RUNTIME_ACCEPTANCE_ROUTES[action]


def runtime_acceptance_command(action: str) -> list[str]:
    return runtime_acceptance_route(action).command()


def run_runtime_acceptance_action(action: str) -> int:
    return run(runtime_acceptance_command(action))

