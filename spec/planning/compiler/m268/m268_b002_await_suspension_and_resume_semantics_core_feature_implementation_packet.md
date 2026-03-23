# M268-B002 Packet: Await Suspension And Resume Semantics - Core Feature Implementation

Issue: `#7285`

## Intent

Implement the truthful Part 7 sema boundary that rejects illegal `await`
placement and publishes a dedicated deterministic packet for await suspension
and resume semantics.

## Scope

- reject `await` outside async functions and Objective-C methods
- publish a dedicated semantic surface for await placement and resume-profile
  legality
- keep the lane-B boundary truthful: sema enforcement is live, runnable async
  frame/runtime execution remains later work

## Required code anchors

- `native/objc3c/src/sema/objc3_sema_contract.h`
- `native/objc3c/src/sema/objc3_semantic_passes.h`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

## Required docs/spec anchors

- `docs/objc3c-native.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`

## Truth boundary

- await placement is now enforced live in sema
- non-async `await` sites fail closed with deterministic diagnostic `O3S223`
- the semantic packet is deterministic and ready for later lowering/runtime
  expansion
- runnable async frame layout, resume lowering, suspension cleanup, and
  executor scheduling remain later `M268` work

## Validation

- static checker verifies docs/spec/code/package anchors
- dynamic checker proves the positive fixture writes
  `frontend.pipeline.semantic_surface.objc_part7_await_suspension_and_resume_semantics`
  into the manifest with exact replay-stable counts
- dynamic checker proves a negative fixture with non-async `await` fails closed
  with `O3S223`
