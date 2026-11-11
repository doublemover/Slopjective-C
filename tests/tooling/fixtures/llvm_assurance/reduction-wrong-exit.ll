target triple = "x86_64-unknown-linux-gnu"
define i32 @unused() { ret i32 99 }
define i32 @main() {
entry:
  %value = add i32 3, 4
  ret i32 %value
}
