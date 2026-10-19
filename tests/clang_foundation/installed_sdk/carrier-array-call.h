#pragma once
#include "carrier-nested-call.h"
typedef __objc3_optional(CallAction __weak) ArrayWeakBlock;
typedef __objc3_optional(CallAction __unsafe_unretained) ArrayUnsafeBlock;
typedef int (^ArrayWeakState)(void);
ArrayWeakBlock array_weak_identity(ArrayWeakBlock value);
ArrayUnsafeBlock array_unsafe_identity(ArrayUnsafeBlock value);
ArrayWeakState array_capture_weak(ArrayWeakBlock value);
int array_provider(int key);
int array_weak_blocks(void);
