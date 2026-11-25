#pragma once

void * _Nonnull objc3_swift_counter_create(void);
void objc3_swift_counter_release(void * _Nonnull counter);
int objc3_swift_error_code(void * _Nullable error);
int objc3_swift_sync_errors_retired(void);
int objc3_swift_resource_is_live(void * _Nonnull resource);
int objc3_swift_resources_retired(void);
