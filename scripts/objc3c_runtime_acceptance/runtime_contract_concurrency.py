"""Concurrency runtime acceptance contract ownership."""

from __future__ import annotations


RUNTIME_UNIFIED_CONCURRENCY_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.unified.concurrency.source.surface.v1"
)
RUNTIME_ASYNC_TASK_ACTOR_NORMALIZATION_COMPLETION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.async.task.actor.normalization.completion.surface.v1"
)
RUNTIME_UNIFIED_CONCURRENCY_LOWERING_METADATA_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.unified.concurrency.lowering.metadata.surface.v1"
)
RUNTIME_UNIFIED_CONCURRENCY_RUNTIME_ABI_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.unified.concurrency.runtime.abi.surface.v1"
)

CONTINUATION_RUNTIME_ABI_PROBE = (
    "tests/tooling/runtime/continuation_runtime_helper_probe.cpp"
)
TASK_RUNTIME_ABI_PROBE = (
    "tests/tooling/runtime/task_runtime_abi_completion_probe.cpp"
)
ACTOR_RUNTIME_ABI_PROBE = (
    "tests/tooling/runtime/actor_runtime_executor_contract_probe.cpp"
)
LIVE_CONTINUATION_RUNTIME_FIXTURE = (
    "tests/tooling/fixtures/native/live_continuation_runtime_integration_positive.objc3"
)
LIVE_CONTINUATION_RUNTIME_PROBE = (
    "tests/tooling/runtime/live_continuation_runtime_integration_probe.cpp"
)
LIVE_TASK_RUNTIME_FIXTURE = (
    "tests/tooling/fixtures/native/live_task_runtime_and_executor_implementation_positive.objc3"
)
LIVE_TASK_RUNTIME_PROBE = (
    "tests/tooling/runtime/live_task_runtime_and_executor_implementation_probe.cpp"
)
LIVE_ACTOR_RUNTIME_FIXTURE = (
    "tests/tooling/fixtures/native/actor_lowering_runtime_positive.objc3"
)
LIVE_ACTOR_RUNTIME_PROBE = (
    "tests/tooling/runtime/live_actor_mailbox_runtime_probe.cpp"
)
CONCURRENCY_ACTOR_PRESERVATION_PROVIDER_FIXTURE = (
    "tests/tooling/fixtures/native/cross_module_actor_isolation_provider.objc3"
)
CONCURRENCY_ACTOR_PRESERVATION_CONSUMER_FIXTURE = (
    "tests/tooling/fixtures/native/cross_module_actor_isolation_consumer.objc3"
)


__all__ = [name for name in globals() if name.isupper()]
