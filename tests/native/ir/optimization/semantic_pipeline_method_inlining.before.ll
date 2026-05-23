; objc3 semantic optimization method/function inlining before fixture
; call-site source-map: sm:inline:caller:line:21
; callee body identity: body:Math.addOne:v1
; callee generation: method-generation=G88

declare i32 @objc3_inlineable_Math_addOne(i32)

define i32 @objc3_callsite_compute(i32 %value) {
entry:
  %call = call i32 @objc3_inlineable_Math_addOne(i32 %value), !dbg !21
  ret i32 %call, !dbg !22
}

!21 = !DILocation(line: 21, column: 13, scope: !1)
!22 = !DILocation(line: 21, column: 3, scope: !1)
!1 = distinct !DISubprogram(name: "compute", linkageName: "objc3_callsite_compute")
