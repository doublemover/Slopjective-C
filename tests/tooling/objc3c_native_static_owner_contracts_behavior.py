from __future__ import annotations

from pathlib import Path

from objc3c_native_static_owner_contracts_assertions import (
    assert_contains_all,
    assert_excludes_all,
    assert_header_excludes_include_shards,
    assert_same_paths,
)
from objc3c_native_static_owner_contracts_sources import (
    SRC_ROOT,
    header_paths,
    include_shard_aggregators,
    include_shards,
    ir_emitter_contract_texts,
    owner_cmake_contracts,
    read_text,
    runtime_public_result_texts,
)


def assert_native_owner_modules_are_registered_in_cmake_without_include_shards() -> None:
    for cmake_path, owner_sources in owner_cmake_contracts().items():
        cmake = read_text(cmake_path)
        assert_contains_all(cmake, owner_sources)
        assert_excludes_all(cmake, [".inc"])


def assert_native_include_shards_remain_private_to_owner_aggregators() -> None:
    shard_paths = include_shards()
    assert shard_paths

    included_by_owner: set[Path] = set()
    for aggregator_path, prefix in include_shard_aggregators().items():
        aggregator = read_text(aggregator_path)
        for shard_path in shard_paths:
            if not shard_path.name.startswith(prefix):
                continue
            relative_include = shard_path.relative_to(SRC_ROOT).as_posix()
            assert f'#include "{relative_include}"' in aggregator
            included_by_owner.add(shard_path)

    assert_same_paths(shard_paths, included_by_owner)

    for header_path in header_paths():
        assert_header_excludes_include_shards(header_path, read_text(header_path))


def assert_ir_emitter_static_contract_uses_split_context_and_active_artifact_api() -> None:
    ir_source, ir_header, artifacts_source = ir_emitter_contract_texts()

    assert_contains_all(
        ir_source,
        ['#include "ir/objc3_ir_emitter_context.h"'],
    )
    assert_excludes_all(ir_source, ["struct FunctionContext"])
    assert_excludes_all(ir_header, ["struct LoweredMessageSend"])
    assert_contains_all(
        artifacts_source,
        ["EmitObjc3IRText(pipeline_result.program.ast, options.lowering"],
    )
    assert_excludes_all(
        artifacts_source,
        ["EmitObjc3IRText(pipeline_result.program, options.lowering"],
    )


def assert_runtime_public_result_abi_uses_split_status_and_payload_headers() -> None:
    (
        runtime_cmake,
        aggregate,
        registration_status,
        dispatch_status,
        dispatch_result,
    ) = runtime_public_result_texts()

    assert_contains_all(
        aggregate,
        [
            '#include "runtime/public/objc3_runtime_registration_status.h"',
            '#include "runtime/public/objc3_runtime_dispatch_status.h"',
            '#include "runtime/public/objc3_runtime_dispatch_result.h"',
        ],
    )
    assert_excludes_all(
        aggregate,
        [
            "typedef enum objc3_runtime_registration_status_code",
            "typedef enum objc3_runtime_dispatch_status_code",
            "typedef struct objc3_runtime_dispatch_i32_result",
        ],
    )

    assert_contains_all(
        registration_status,
        [
            "typedef enum objc3_runtime_registration_status_code",
            "OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_REGISTRATION_ROOTS",
        ],
    )
    assert_contains_all(
        dispatch_status,
        [
            "typedef enum objc3_runtime_dispatch_status_code",
            "OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT",
        ],
    )
    assert_contains_all(
        dispatch_result,
        [
            "typedef struct objc3_runtime_dispatch_i32_result",
            "objc3_runtime_dispatch_status_code status_code;",
        ],
    )
    assert_contains_all(
        runtime_cmake,
        [
            "public/objc3_runtime_registration_status.h",
            "public/objc3_runtime_dispatch_status.h",
            "public/objc3_runtime_dispatch_result.h",
        ],
    )
