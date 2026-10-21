; objc3 semantic optimization method/function inlining before fixture
; call-site source-map: sm:inline:caller:line:21
; original call source span: source-span:method-inline:original-callsite
; callee source span: source-span:method-inline:callee-body
; inline frame id: inline-frame:method-inline:caller+callee
; inlined callsite source span: source-span:method-inline:inlined-callsite
; imported debug map inline frame: dmap.optimization_transform_edge
; emitted debug map inline frame: dmap.optimization_transform_edge
; stepping policy: stepping-policy:method-inline:step-into-callee-step-out-caller
; optimized IR/source correlation: ir-source-correlation:method-inline:optimized-ir-to-caller-callee
; receiver dispatch assumption: dispatch-assumption:method-inline:receiver-static-type+final-target
; side-effect replay: side-effect-replay:method-inline:pure-no-writes-no-calls-no-runtime-helpers
; exact callee identity: method:InlineMath.addOne:i32->i32
; callee body identity: body:InlineMath.addOne:v1
; callee generation: method-generation=G88
; runtime invalidation replay: runtime-replay:method-inline:stale-dispatch-cache-fail-closed

declare i32 @objc3_inlineable_InlineMath_addOne(i32)

define i32 @objc3_callsite_compute(i32 %value) {
entry:
  %call = call i32 @objc3_inlineable_InlineMath_addOne(i32 %value), !dbg !21
  ret i32 %call, !dbg !22
}

!21 = !DILocation(line: 21, column: 13, scope: !1)
!22 = !DILocation(line: 21, column: 3, scope: !1)
!1 = distinct !DISubprogram(name: "compute", linkageName: "objc3_callsite_compute")
