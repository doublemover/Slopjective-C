#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Clears runtime-owned state tables used by this public ABI.
 */
void objc3_runtime_reset_for_testing(void);

#ifdef __cplusplus
}
#endif
