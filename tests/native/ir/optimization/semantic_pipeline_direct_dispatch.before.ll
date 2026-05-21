; objc3 semantic optimization pipeline fixture: direct dispatch candidate before pass.
declare i32 @objc3_runtime_dispatch_i32(i32, ptr, i32, i32, i32, i32)
declare i1 @objc3_direct_Sample_isReady(i32)

define i32 @main() {
entry:
  %selector = getelementptr inbounds [8 x i8], ptr @.objc3.selector.isReady, i32 0, i32 0
  %value = call i32 @objc3_runtime_dispatch_i32(i32 1, ptr %selector, i32 0, i32 0, i32 0, i32 0)
  ret i32 %value
}
