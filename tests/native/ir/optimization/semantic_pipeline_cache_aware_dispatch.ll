; Cache-aware dispatch optimization fixture: eligible i32 message sends lower
; to the runtime-owned checked helper and preserve strict dispatch by aborting
; when the checked status envelope reports a non-value strict error.

declare void @abort()
declare i32 @objc3_runtime_prepare_cache_aware_dispatch_descriptor(ptr, ptr, ptr, i32, i32)
declare { i32, i32, i32, i32, i32, ptr, ptr, ptr, ptr, ptr } @objc3_runtime_cache_aware_dispatch_i32_checked(i32, ptr, i32, i32, i32, i32)

@.objc3.selector.isReady = private unnamed_addr constant [8 x i8] c"isReady\00"

define i32 @semantic_pipeline_cache_aware_dispatch(i32 %receiver) {
entry:
  %selector = getelementptr inbounds [8 x i8], ptr @.objc3.selector.isReady, i32 0, i32 0
  ; semantic-optimization.cache-aware-dispatch: runtime-owned helper, strict checked status envelope
  ; source-map.cache-aware-dispatch: line=12;column=7;debug-visible=true;preserves-source-map=true
  %objc3.cache_aware.dispatch.descriptor.0 = alloca { i32, i32, ptr, i64, i64, i64, i64, i64, i64, ptr, i32, i32 }, align 8
  %prepare_status = call i32 @objc3_runtime_prepare_cache_aware_dispatch_descriptor(ptr %objc3.cache_aware.dispatch.descriptor.0, ptr %selector, ptr null, i32 12, i32 7)
  %prepare_status.ok = icmp sge i32 %prepare_status, 0
  br i1 %prepare_status.ok, label %cache_dispatch_value.dispatch, label %cache_dispatch_strict_fail

cache_dispatch_value.dispatch:
  %result = call { i32, i32, i32, i32, i32, ptr, ptr, ptr, ptr, ptr } @objc3_runtime_cache_aware_dispatch_i32_checked(i32 %receiver, ptr %objc3.cache_aware.dispatch.descriptor.0, i32 0, i32 0, i32 0, i32 0)
  %status = extractvalue { i32, i32, i32, i32, i32, ptr, ptr, ptr, ptr, ptr } %result, 2
  %status.ok = icmp sge i32 %status, 0
  br i1 %status.ok, label %cache_dispatch_value, label %cache_dispatch_strict_fail

cache_dispatch_strict_fail:
  call void @abort()
  unreachable

cache_dispatch_value:
  %value = extractvalue { i32, i32, i32, i32, i32, ptr, ptr, ptr, ptr, ptr } %result, 4
  ret i32 %value
}
