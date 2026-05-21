@globalCounter = global i32 0

declare void @objc3_runtime_bootstrap()

define i32 @main() {
entry:
  %0 = load i32, ptr @globalCounter
  ret i32 %0
}
