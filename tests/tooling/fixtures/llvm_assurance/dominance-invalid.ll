target triple = "x86_64-unknown-linux-gnu"
define i32 @bad(i1 %condition) {
entry:
  br i1 %condition, label %left, label %right
left:
  %value = add i32 1, 2
  br label %right
right:
  ret i32 %value
}
