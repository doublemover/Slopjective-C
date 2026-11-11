target triple = "x86_64-unknown-linux-gnu"
define i32 @main() {
entry:
  %value = add i32 20, 22
  %status = sub i32 %value, 42
  ret i32 %status
}
