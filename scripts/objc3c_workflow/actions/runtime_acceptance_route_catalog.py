"""Runtime acceptance workflow route catalog."""

from __future__ import annotations

from .runtime_acceptance_route_model import RuntimeAcceptanceRoute

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
