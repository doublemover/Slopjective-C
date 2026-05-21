; objc3 semantic optimization method/function inlining after fixture
; inlined callee body identity: body:Math.addOne:v1
; source-map inline frame preserved: sm:inline:caller:line:21 -> sm:inline:callee:line:7
; diagnostic location preserved: diag:inline:caller:21:13
; semantic-optimization.invalidate-global-proof-state: callee_body_identity,callee_generation,local_value,ownership_transfer,source_map_inline_frame,diagnostic_location,runtime_metadata_identity,package_import_abi_identity

define i32 @objc3_callsite_compute(i32 %value) {
entry:
  %inlined.add = add nsw i32 %value, 1, !dbg !7
  ret i32 %inlined.add, !dbg !22
}

!7 = !DILocation(line: 7, column: 10, scope: !2, inlinedAt: !21)
!21 = !DILocation(line: 21, column: 13, scope: !1)
!22 = !DILocation(line: 21, column: 3, scope: !1)
!1 = distinct !DISubprogram(name: "compute", linkageName: "objc3_callsite_compute")
!2 = distinct !DISubprogram(name: "addOne", linkageName: "objc3_inlineable_Math_addOne")
