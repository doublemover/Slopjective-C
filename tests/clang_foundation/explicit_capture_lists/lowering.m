// RUN: %clang -fobjc-version=3 -fobjc-runtime=gnustep-2.2 -fblocks -fobjc-arc -S -emit-llvm -O0 %s -o - | FileCheck %s
// Own lowering fixtures intentionally bind behavior, not capture layout offsets.
void capture_sink(void (^block)(void));
void capture_close_ir(int) __attribute__((nothrow));
void capture_lowering(id input) {
  id strong_source = input;
  id __weak weak_source = input;
  capture_sink(^[weak strong_source, strong weak_source] {
    (void)strong_source; (void)weak_source;
  });
  capture_sink(^[unowned strong_source] { (void)strong_source; });
  capture_sink(^[move strong_source] { (void)strong_source; });
  int handle __attribute__((objc_resource(close=capture_close_ir, invalid=-1))) = 9;
  capture_sink(^[move handle] { (void)handle; });
}
// CHECK-DAG: call{{.*}}@llvm.objc.initWeak
// CHECK-DAG: call{{.*}}@llvm.objc.loadWeakRetained
// CHECK-DAG: store ptr null
// CHECK-DAG: call{{.*}}@malloc
// CHECK-DAG: store i32 -1
// CHECK-DAG: atomicrmw
// CHECK-DAG: @__objc3_block_resource_destroy_v1
// CHECK-DAG: @free
