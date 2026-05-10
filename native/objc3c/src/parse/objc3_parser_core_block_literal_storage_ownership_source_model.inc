  void ApplyBlockLiteralStorageOwnershipSourceModel(
      Expr &block,
      BlockLiteralSourceUseKind source_use_kind) {
    // block-source-storage-annotation anchor: parser now classifies
    // mutated/byref/helper/escape-shape source annotations directly from the
    // block body and surrounding expression context so later runnable block
    // lanes consume deterministic source truth instead of reconstructing it.
    // block-runtime-semantic-rules freeze anchor: these parser-owned
    // annotations are the complete current legality input for block runtime
    // semantics; no runnable byref/helper/heap-promotion behavior is implied
    // by their presence yet.
    // capture-legality/escape/invocation implementation anchor:
    // lane-B now consumes these exact parser-owned source facts as the live
    // sema input for capture resolution, truthful escape classification, and
    // local block invocation typing.
    // byref/copy-dispose/object-ownership anchor: parser still owns
    // only the deterministic source inventory here; sema layers
    // ownership-sensitive helper eligibility on top without widening this
    // source-only parser contract.
    // escaping-block runtime-hook anchor: parser continues to own
    // the exact escape-shape source classification, including which block
    // literal sites are truthful heap-promotion candidates for later lane-C
    // runtime-hook lowering.
    // block-runtime API/object-layout freeze anchor: parser does not
    // widen the runtime ABI here; it only preserves the source-truth escape
    // inventory that the frozen private runtime boundary consumes downstream.
    // block-runtime allocation/copy-dispose/invoke anchor: parser
    // still does not widen the source surface; runtime helper realization
    // remains an internal lowering/runtime concern layered on this same source
    // inventory.
    // byref-forwarding/heap-promotion/ownership-interop anchor:
    // parser still only publishes the truthful capture and escape inventory;
    // runtime-owned forwarding cells for escaping pointer-capture blocks are
    // layered later without widening this source contract.
    // runnable-block-runtime gate anchor: lane-E consumes this exact
    // parser-owned capture and escape inventory as the source side of the
    // runnable block proof chain and must fail closed if the source truth
    // drifts behind the integrated gate.
    // runnable-block execution-matrix anchor: lane-E closes the block-runtime
    // tranche on the same parser-owned capture and escape inventory together
    // with live executable block programs, without widening the supported
    // source surface.
    block.block_copy_helper_intent_required =
        block.block_byref_capture_count > 0u;
    block.block_dispose_helper_intent_required =
        block.block_byref_capture_count > 0u;
    block.block_escape_shape_symbol =
        BuildObjc3BlockEscapeShapeSymbol(source_use_kind);
    block.block_escape_shape_promotes_to_heap_candidate =
        Objc3BlockEscapeShapePromotesToHeapCandidate(source_use_kind);
  }
