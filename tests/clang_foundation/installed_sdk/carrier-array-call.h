#pragma once
#include "carrier-nested-call.h"
typedef Optional(CallAction __weak) ArrayWeakBlock;
typedef Optional(CallAction __unsafe_unretained) ArrayUnsafeBlock;
typedef int (^ArrayWeakState)(void);
ArrayWeakBlock array_weak_identity(ArrayWeakBlock value);
ArrayUnsafeBlock array_unsafe_identity(ArrayUnsafeBlock value);
ArrayWeakState array_capture_weak(ArrayWeakBlock value);
int array_provider(int key);
int array_weak_blocks(void);
