#if defined(_WIN32) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif
#if defined(_WIN32) && !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#if defined(_WIN32) && !defined(NOMINMAX)
#define NOMINMAX
#endif

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "runtime/concurrency/runtime_debug.h"
#include "runtime/dispatch/dispatch_result_state.h"
#include "runtime/dispatch/runtime_resolution_records.h"
#include "runtime/errors/error_bridge.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_registration_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/storage/runtime_instance_records.h"
#include "runtime/state/runtime_state_clear.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"
#include "runtime/state/runtime_thread_records.h"

#if defined(_WIN32)
#include <windows.h>
#endif

using objc3c::runtime::EmittedCategoryRecord;
using objc3c::runtime::EmittedClassBundle;
using objc3c::runtime::EmittedClassRecord;
using objc3c::runtime::EmittedIvarDescriptor;
using objc3c::runtime::EmittedIvarLayoutRecord;
using objc3c::runtime::EmittedKeyPathDescriptor;
using objc3c::runtime::EmittedMethodListEntry;
using objc3c::runtime::EmittedMethodListHeader;
using objc3c::runtime::EmittedMethodListRef;
using objc3c::runtime::EmittedPropertyDescriptor;
using objc3c::runtime::EmittedProtocolRecord;
using objc3c::runtime::ClearImageWalkSnapshotUnlocked;
using objc3c::runtime::FindRuntimePropertyAccessorByNameUnlocked;
using objc3c::runtime::HasTerminalStrictDispatchError;
using objc3c::runtime::InvokeRuntimeBuiltinMethod;
using objc3c::runtime::MethodCacheEntry;
using objc3c::runtime::PropertyLookupCacheEntry;
using objc3c::runtime::PropertyLookupCacheKey;
using objc3c::runtime::PropertyLookupCacheKeyHash;
using objc3c::runtime::ProcessRuntimeState;
using objc3c::runtime::RealizedClassNode;
using objc3c::runtime::RealizedPropertyAccessor;
using objc3c::runtime::RegisteredImageMetadata;
using objc3c::runtime::RuntimeInstanceRecord;
using objc3c::runtime::RuntimeState;
using objc3c::runtime::RuntimeDispatchFrame;
using objc3c::runtime::RuntimeWeakSlotRef;
using objc3c::runtime::ReplayRegisteredImagesForTestingUnlocked;
using objc3c::runtime::SelectorSlot;
using objc3c::runtime::SlowPathResolution;
namespace {

using objc3c::runtime::ClearRuntimeTestingDispatchFrame;
using objc3c::runtime::CountQueuedAutoreleaseValues;
using objc3c::runtime::CurrentRuntimeDispatchFrame;
using objc3c::runtime::EffectiveIvarOffset;
using objc3c::runtime::EffectiveIvarSize;
using objc3c::runtime::EnqueueAutoreleaseValue;
using objc3c::runtime::ExchangeRuntimeManagedPropertyValueUnlocked;
using objc3c::runtime::MethodCacheKey;
using objc3c::runtime::MethodCacheKeyHash;
using objc3c::runtime::BuildReceiverBaseIdentity;
using objc3c::runtime::ClassifyRuntimeReturnType;
using objc3c::runtime::DecodeReceiverIdentity;
using objc3c::runtime::DescribeResolvedImplementationKind;
using objc3c::runtime::DispatchFamily;
using objc3c::runtime::IsRuntimeReceiverBaseIdentity;
using objc3c::runtime::RuntimeBuiltinKind;
using objc3c::runtime::RuntimeMethodReturnKind;
using objc3c::runtime::RuntimeMethodShapeStatus;
using objc3c::runtime::RuntimeReceiverIdentityBase;
using objc3c::runtime::RuntimeReceiverIdentityStride;
using objc3c::runtime::PopRuntimeAutoreleasePoolFrameValues;
using objc3c::runtime::PopRuntimeDispatchFrameAutoreleaseValues;
using objc3c::runtime::PushRuntimeAutoreleasePoolFrame;
using objc3c::runtime::PushRuntimeDispatchFrame;
using objc3c::runtime::ReadRuntimeManagedPropertyValueUnlocked;
using objc3c::runtime::RecordRuntimeAutoreleasePoolDrainedValue;
using objc3c::runtime::ReleaseRuntimeValueUnlocked;
using objc3c::runtime::RetainRuntimeValueUnlocked;
using objc3c::runtime::RuntimeAutoreleasePoolDepth;
using objc3c::runtime::RuntimeAutoreleasePoolDrainedValueCount;
using objc3c::runtime::RuntimeAutoreleasePoolMaxDepth;
using objc3c::runtime::RuntimeLastAutoreleasedValue;
using objc3c::runtime::RuntimeLastDrainedAutoreleaseValue;
using objc3c::runtime::SetRuntimeTestingDispatchFrame;
using objc3c::runtime::RuntimeTypedDispatchFailure;
using objc3c::runtime::RuntimeTypedDispatchResult;
using objc3c::runtime::RuntimeTypedDispatchSuccess;
using objc3c::runtime::UsesStrongOwnedRuntimeHooks;
using objc3c::runtime::BorrowRuntimeCString;
using objc3c::runtime::ClearMethodCacheStateUnlocked;
using objc3c::runtime::ClearRealizedClassGraphUnlocked;
using objc3c::runtime::ClearRuntimeInstanceStateUnlocked;
using objc3c::runtime::LookupSelectorUnlocked;
using objc3c::runtime::MaterializeKeyPathDescriptorUnlocked;
using objc3c::runtime::MaterializeSelectorLookupEntryUnlocked;
using objc3c::runtime::RecordPostResolutionStrictDispatchFailure;
using objc3c::runtime::RecordTypedDispatchSuccess;
using objc3c::runtime::ResolveMethodSlowPathUnlocked;
using objc3c::runtime::SeedDispatchIntentFastPathCacheUnlocked;
using objc3c::runtime::StoreDispatchResultContractUnlocked;
using objc3c::runtime::WriteRuntimeManagedPropertyValueUnlocked;
