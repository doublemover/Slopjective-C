# objc3c Interop Surface

This runbook is the issue #8165 boundary for Objective-C 3.0 interop. It
separates implemented bridge metadata from broader language compatibility that
is still reserved or rejected.

Checked-in source of truth:

- `tests/tooling/fixtures/interop_surface_policy/objc3c_interop_surface_policy.json`
- `schemas/objc3c-interop-surface-policy-v1.schema.json`
- `scripts/check_objc3c_interop_surface_policy.py`

Related existing evidence:

- `tests/tooling/fixtures/module_interop_contracts/foundation_next_visibility_bridge_contract.json`
- `tests/tooling/fixtures/runtime_import_interop_bridge_metadata/valid_bridge_surface.json`
- `tests/tooling/fixtures/runtime_import_interop_bridge_metadata/tampered_bridge_surface.json`
- `tests/tooling/fixtures/package_ecosystem/mixed_image_interop_loader_metadata.json`

## Supported Lanes

| Lane | State | Scope |
| --- | --- | --- |
| `c.header-import-export` | supported | C header import/export and generated bridge metadata. |
| `objc2.metadata-only-migration` | supported | Objective-C 2.0-adjacent migration and bridge metadata only. Retired Objective-C 2 source syntax is not accepted as Objective-C 3. |
| `swift.annotation-metadata` | supported | `objc_swift_name` and `objc_swift_private` metadata preservation only. |
| `cpp.annotation-metadata` | supported | `objc_cxx_name` and `objc_header_name` metadata preservation only. |
| `package.mixed-image-loader-metadata` | supported | Local mixed-image package loader metadata with digest and payload drift checks. |

These lanes are claimable only through the existing public workflow evidence:

- `npm run objc3c -- validate-module-interop-contracts`
- `npm run objc3c -- validate-interop-conformance`
- `npm run objc3c -- validate-runnable-interop`
- `npm run objc3c -- validate-migration-workflow`
- `npm run objc3c -- validate-package-ecosystem`

## Reserved Or Rejected

| Lane | State | Diagnostic |
| --- | --- | --- |
| `swift.full-abi-callable-import` | reserved | `O3INT8166` |
| `cpp.template-and-abi-import` | reserved | `O3INT8167` |
| `objc2.retired-source-compatibility` | rejected | `O3INT8165` |
| `package.unchecked-abi-alignment-fallback` | rejected | `O3PKG8052` |

The policy does not claim complete Swift compatibility, complete C++
compatibility, or drop-in Objective-C 2 source compatibility. Swift and C++
metadata rows are supported only as checked annotation metadata. Swift ABI
import, C++ template import, C++ ABI import, Swift throws/async bridging, and
retired Objective-C 2 source acceptance stay reserved or rejected until
executable evidence exists.

## Fail-Closed Rules

Active bridge generation must have all of the following before it can be treated
as supported:

- ready foreign-surface and FFI preservation packets
- deterministic replay metadata
- matching foreign callable counts across bridge, foreign, and FFI packets
- import-module metadata
- header annotation coverage for every foreign callable
- at least one Swift-facing or C++-facing annotation metadata row
- canonical relative artifact paths for the bridge header, modulemap, and JSON

The tampered bridge fixture must continue to reject traversal paths and missing
Swift/C++ annotation metadata. Package metadata must continue to reject digest
drift, payload drift, unsafe mixed-image topology, malformed metadata, and
unchecked ABI alignment fallback.

## Intentional Gaps

This slice adds policy and validation around the existing interop evidence. It
does not implement full Swift ABI import, full C++ ABI import, C++ template
instantiation import, Objective-C 2 source compatibility, hosted registry
mixed-image restore, or network-resolved interop metadata.
