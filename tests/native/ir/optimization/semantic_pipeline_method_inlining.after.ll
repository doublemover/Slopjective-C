; objc3 semantic optimization method/function inlining after fixture
; exact callee identity preserved: method:InlineMath.addOne:i32->i32
; inlined callee body identity: body:InlineMath.addOne:v1
; inline frame id preserved: inline-frame:method-inline:caller+callee
; source-map inline frame preserved: sm:inline:caller:line:21 -> sm:inline:callee:line:7
; inlined callsite source span preserved: source-span:method-inline:inlined-callsite
; imported debug map inline frame preserved: dmap.optimization_transform_edge
; emitted debug map inline frame preserved: dmap.optimization_transform_edge
; diagnostic location preserved: diag:inline:caller:21:13
; stepping policy preserved: stepping-policy:method-inline:step-into-callee-step-out-caller
; optimized IR/source correlation preserved: ir-source-correlation:method-inline:optimized-ir-to-caller-callee
; debug stepping evidence: debug-step:method-inline:caller-frame+callee-inline-frame
; side-effect replay consumed: side-effect-replay:method-inline:pure-no-writes-no-calls-no-runtime-helpers
; receiver dispatch assumption consumed: dispatch-assumption:method-inline:receiver-static-type+final-target
; runtime invalidation replay consumed: runtime-replay:method-inline:stale-dispatch-cache-fail-closed
; semantic-optimization.invalidate-global-proof-state: callee_body_identity,callee_generation,local_value,ownership_transfer,source_map_inline_frame,diagnostic_location,debug_stepping,runtime_dispatch_assumption,runtime_cache_version,runtime_metadata_identity,side_effect_replay,invalidation_replay,package_import_abi_identity

define i32 @objc3_callsite_compute(i32 %value) {
entry:
  %inlined.add = add nsw i32 %value, 1, !dbg !7
  ret i32 %inlined.add, !dbg !22
}

!7 = !DILocation(line: 7, column: 10, scope: !2, inlinedAt: !21)
!21 = !DILocation(line: 21, column: 13, scope: !1)
!22 = !DILocation(line: 21, column: 3, scope: !1)
!1 = distinct !DISubprogram(name: "compute", linkageName: "objc3_callsite_compute")
!2 = distinct !DISubprogram(name: "addOne", linkageName: "objc3_inlineable_InlineMath_addOne")
