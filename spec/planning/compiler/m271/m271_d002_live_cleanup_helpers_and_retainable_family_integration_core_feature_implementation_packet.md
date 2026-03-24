# M271-D002 Packet: Live Cleanup Helpers And Retainable-Family Integration - Core Feature Implementation

## Summary

Implement the supported runnable Part 8 cleanup/resource and retainable-family slice as an executable linked native path on top of the private helper/runtime boundary frozen in `M271-D001`.

## Scope

- publish one explicit executable Part 8 runtime-integration summary in emitted IR
- keep the helper/runtime slice on the existing private ARC/autorelease helpers and snapshots
- prove direct cleanup execution and retainable-family helper traffic with a linked native probe

## Required proof

- compile one positive fixture that emits the runnable `helperSurface` function
- link a runtime probe against the emitted object and `artifacts/lib/objc3_runtime.lib`
- route `CFRetain` / `CFRelease` / `CFAutorelease` through the private runtime helpers in the probe
- count `CloseFd` and `ReleaseTemp` directly in the probe
- validate ARC-debug and memory-management snapshots after execution

## Truth constraints

- keep `M271-C001` as the only Part 8 lowering contract
- consume `M271-C003` as the borrowed/retainable ABI packet
- consume `M271-D001` as the private runtime/helper freeze
- no dedicated borrowed-pointer runtime helper
- no escaping cleanup/resource ownership-transfer claim
- no public runtime header widening
