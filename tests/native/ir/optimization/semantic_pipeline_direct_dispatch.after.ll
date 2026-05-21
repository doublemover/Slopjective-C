; objc3 semantic optimization pipeline fixture: exact direct dispatch after pass.
declare i1 @objc3_direct_Sample_isReady(i32)

define i32 @main() {
entry:
  %direct = call i1 @objc3_direct_Sample_isReady(i32 1)
  %value = zext i1 %direct to i32
  ; semantic-optimization.invalidate-global-proof-state = direct-dispatch-exact-call
  ret i32 %value
}
