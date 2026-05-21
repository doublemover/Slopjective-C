; Cache-aware dispatch optimization fixture: eligible i32 message sends lower
; to the runtime-owned checked helper and preserve strict dispatch by aborting
; when the checked status envelope reports a non-value strict error.

declare void @abort()
declare { i32, i32, i32, i32, i32, ptr, ptr, ptr, ptr, ptr } @objc3_runtime_cache_aware_dispatch_i32_checked(i32, ptr, i32, i32, i32, i32)

@.objc3.selector.isReady = private unnamed_addr constant [8 x i8] c"isReady\00"

define i32 @semantic_pipeline_cache_aware_dispatch(i32 %receiver) {
entry:
  %selector = getelementptr inbounds [8 x i8], ptr @.objc3.selector.isReady, i32 0, i32 0
  ; semantic-optimization.cache-aware-dispatch: runtime-owned helper, strict checked status envelope
  ; source-map.cache-aware-dispatch: line=12;column=7;debug-visible=true;preserves-source-map=true
  %objc3.cache_aware.dispatch.descriptor.0 = alloca { i32, i32, ptr, i64, i64, i64, i64, i64, i64, ptr, i32, i32 }, align 8
  %objc3.cache_aware.dispatch.descriptor.0.field0 = getelementptr inbounds { i32, i32, ptr, i64, i64, i64, i64, i64, i64, ptr, i32, i32 }, ptr %objc3.cache_aware.dispatch.descriptor.0, i32 0, i32 0
  store i32 1, ptr %objc3.cache_aware.dispatch.descriptor.0.field0
  %objc3.cache_aware.dispatch.descriptor.0.field1 = getelementptr inbounds { i32, i32, ptr, i64, i64, i64, i64, i64, i64, ptr, i32, i32 }, ptr %objc3.cache_aware.dispatch.descriptor.0, i32 0, i32 1
  store i32 7, ptr %objc3.cache_aware.dispatch.descriptor.0.field1
  %objc3.cache_aware.dispatch.descriptor.0.field2 = getelementptr inbounds { i32, i32, ptr, i64, i64, i64, i64, i64, i64, ptr, i32, i32 }, ptr %objc3.cache_aware.dispatch.descriptor.0, i32 0, i32 2
  store ptr %selector, ptr %objc3.cache_aware.dispatch.descriptor.0.field2
  %objc3.cache_aware.dispatch.descriptor.0.field3 = getelementptr inbounds { i32, i32, ptr, i64, i64, i64, i64, i64, i64, ptr, i32, i32 }, ptr %objc3.cache_aware.dispatch.descriptor.0, i32 0, i32 3
  store i64 0, ptr %objc3.cache_aware.dispatch.descriptor.0.field3
  %objc3.cache_aware.dispatch.descriptor.0.field4 = getelementptr inbounds { i32, i32, ptr, i64, i64, i64, i64, i64, i64, ptr, i32, i32 }, ptr %objc3.cache_aware.dispatch.descriptor.0, i32 0, i32 4
  store i64 0, ptr %objc3.cache_aware.dispatch.descriptor.0.field4
  %objc3.cache_aware.dispatch.descriptor.0.field5 = getelementptr inbounds { i32, i32, ptr, i64, i64, i64, i64, i64, i64, ptr, i32, i32 }, ptr %objc3.cache_aware.dispatch.descriptor.0, i32 0, i32 5
  store i64 0, ptr %objc3.cache_aware.dispatch.descriptor.0.field5
  %objc3.cache_aware.dispatch.descriptor.0.field6 = getelementptr inbounds { i32, i32, ptr, i64, i64, i64, i64, i64, i64, ptr, i32, i32 }, ptr %objc3.cache_aware.dispatch.descriptor.0, i32 0, i32 6
  store i64 0, ptr %objc3.cache_aware.dispatch.descriptor.0.field6
  %objc3.cache_aware.dispatch.descriptor.0.field7 = getelementptr inbounds { i32, i32, ptr, i64, i64, i64, i64, i64, i64, ptr, i32, i32 }, ptr %objc3.cache_aware.dispatch.descriptor.0, i32 0, i32 7
  store i64 0, ptr %objc3.cache_aware.dispatch.descriptor.0.field7
  %objc3.cache_aware.dispatch.descriptor.0.field8 = getelementptr inbounds { i32, i32, ptr, i64, i64, i64, i64, i64, i64, ptr, i32, i32 }, ptr %objc3.cache_aware.dispatch.descriptor.0, i32 0, i32 8
  store i64 0, ptr %objc3.cache_aware.dispatch.descriptor.0.field8
  %objc3.cache_aware.dispatch.descriptor.0.field9 = getelementptr inbounds { i32, i32, ptr, i64, i64, i64, i64, i64, i64, ptr, i32, i32 }, ptr %objc3.cache_aware.dispatch.descriptor.0, i32 0, i32 9
  store ptr null, ptr %objc3.cache_aware.dispatch.descriptor.0.field9
  %objc3.cache_aware.dispatch.descriptor.0.field10 = getelementptr inbounds { i32, i32, ptr, i64, i64, i64, i64, i64, i64, ptr, i32, i32 }, ptr %objc3.cache_aware.dispatch.descriptor.0, i32 0, i32 10
  store i32 12, ptr %objc3.cache_aware.dispatch.descriptor.0.field10
  %objc3.cache_aware.dispatch.descriptor.0.field11 = getelementptr inbounds { i32, i32, ptr, i64, i64, i64, i64, i64, i64, ptr, i32, i32 }, ptr %objc3.cache_aware.dispatch.descriptor.0, i32 0, i32 11
  store i32 7, ptr %objc3.cache_aware.dispatch.descriptor.0.field11
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
