import ForeignAsync
import Synchronization

private enum NativeError: Error {
    case checkedFailure
    case invalidCompletion
}

private typealias Completion = @convention(c) (
    UnsafeMutableRawPointer?, objc_async_completion_status_t, CInt,
    UnsafeMutableRawPointer?
) -> Void

private typealias VoidCompletion = @convention(c) (
    UnsafeMutableRawPointer?, objc_async_completion_status_t, UnsafeMutableRawPointer?
) -> Void

// The immutable pointer owns a retained service handle. The runtime permits
// cancellation from any thread, and ARC keeps the handle alive during it.
private final class NativeTask: @unchecked Sendable {
    let handle: OpaquePointer

    init(_ handle: OpaquePointer) { self.handle = handle }
    deinit { objc3_task_release(handle) }

    func cancel() { objc3_task_request_cancel(handle) }
}

private final class Cancellation: Sendable {
    private struct State {
        var task: NativeTask?
        var requested = false
        var finished = false
    }

    private let state = Mutex(State())

    func install(_ handle: OpaquePointer?) {
        guard let handle else { return }
        let task = NativeTask(handle)
        let requested = state.withLock { state in
            guard !state.finished else { return false }
            state.task = task
            return state.requested
        }
        if requested { task.cancel() }
    }

    func cancel() {
        let task = state.withLock { state in
            state.requested = true
            return state.task
        }
        // Cancellation can synchronously run a completion on dispatch failure.
        task?.cancel()
    }

    func finish() {
        let task = state.withLock { state in
            state.finished = true
            let task = state.task
            state.task = nil
            return task
        }
        withExtendedLifetime(task) {}
    }
}

private final class Pending: Sendable {
    let continuation: CheckedContinuation<CInt, any Error>

    init(_ continuation: CheckedContinuation<CInt, any Error>) {
        self.continuation = continuation
    }
}

private nonisolated(nonsending) func awaitResult(
    _ submit: (UnsafeMutableRawPointer, Completion) -> Void
) async throws -> CInt {
    try await withCheckedThrowingContinuation { continuation in
        let context = Unmanaged.passRetained(Pending(continuation)).toOpaque()
        submit(context) { context, status, value, error in
            guard let context else { preconditionFailure("missing native completion context") }
            let pending = Unmanaged<Pending>.fromOpaque(context).takeRetainedValue()
            switch (status, objc3_swift_error_code(error)) {
            case (OBJC_ASYNC_COMPLETION_SUCCESS, 0) where error == nil:
                pending.continuation.resume(returning: value)
            case (OBJC_ASYNC_COMPLETION_ERROR, 1):
                pending.continuation.resume(throwing: NativeError.checkedFailure)
            case (OBJC_ASYNC_COMPLETION_CANCELLED, 2):
                pending.continuation.resume(throwing: CancellationError())
            default:
                pending.continuation.resume(throwing: NativeError.invalidCompletion)
            }
        }
    }
}

private final class PendingCancellation: Sendable {
    let continuation: CheckedContinuation<Void, any Error>
    let cancellation: Cancellation

    init(_ continuation: CheckedContinuation<Void, any Error>, _ cancellation: Cancellation) {
        self.continuation = continuation
        self.cancellation = cancellation
    }
}

private func awaitNativeCancellation(
    _ submit: (UnsafeMutableRawPointer, VoidCompletion) -> OpaquePointer?,
    submitted: () -> Void
) async throws {
    let cancellation = Cancellation()
    try await withTaskCancellationHandler {
        try await withCheckedThrowingContinuation { continuation in
            let pending = PendingCancellation(continuation, cancellation)
            let context = Unmanaged.passRetained(pending).toOpaque()
            let handle = submit(context) { context, status, error in
                guard let context else { preconditionFailure("missing native completion context") }
                let pending = Unmanaged<PendingCancellation>.fromOpaque(context).takeRetainedValue()
                pending.cancellation.finish()
                if status == OBJC_ASYNC_COMPLETION_CANCELLED && objc3_swift_error_code(error) == 2 {
                    pending.continuation.resume(throwing: CancellationError())
                } else {
                    pending.continuation.resume(throwing: NativeError.invalidCompletion)
                }
            }
            precondition(handle != nil)
            cancellation.install(handle)
            submitted()
        }
    } onCancel: {
        cancellation.cancel()
    }
}

private func checkSwiftCancellation(afterSubmission: Bool) async throws {
    let (submitted, signal) = AsyncStream<Void>.makeStream()
    let task = Task {
        if !afterSubmission { withUnsafeCurrentTask { $0?.cancel() } }
        try await awaitNativeCancellation(swift_bridge_cancellable_async_c_task) {
            signal.finish()
        }
    }
    if afterSubmission {
        // The stream finishes only after the Swift handler owns the native handle.
        for await _ in submitted {}
        task.cancel()
    }
    do {
        try await task.value
        preconditionFailure("cancelled Swift task completed normally")
    } catch is CancellationError {}
}

private func checkGeneratedSynchronousError() throws {
    do {
        try ForeignAsyncNative.swift_bridge_checked_void(false)
        preconditionFailure("generated Swift wrapper returned after synchronous failure")
    } catch let error as ForeignAsyncNativeError {
        precondition(objc3_swift_sync_errors_retired() == 0)
        precondition(error.withBorrowedNativeError { objc3_swift_error_code($0) } == 3)
    }
}

@main
private struct Consumer {
    static func main() async throws {
        let value = try await awaitResult { swift_bridge_work_async_c(41, $0, $1) }
        do {
            _ = try await awaitResult { swift_bridge_work_async_c(-1, $0, $1) }
            preconditionFailure("native failure returned a value")
        } catch NativeError.checkedFailure {}
        do {
            _ = try await awaitResult { swift_bridge_cancelled_async_c($0, $1) }
            preconditionFailure("native cancellation returned a value")
        } catch is CancellationError {}
        let taskValue = try await awaitResult { context, completion in
            let handle = swift_bridge_work_async_c_task(41, context, completion)
            precondition(handle != nil)
            objc3_task_release(handle)
        }
        try await checkSwiftCancellation(afterSubmission: false)
        try await checkSwiftCancellation(afterSubmission: true)

        let first = objc3_swift_counter_create()
        let second = objc3_swift_counter_create()
        defer {
            objc3_swift_counter_release(first)
            objc3_swift_counter_release(second)
        }
        let firstValue = try await awaitResult { SwiftBridgeCounter_add_async_c(first, 40, $0, $1) }
        let secondValue = try await awaitResult { SwiftBridgeCounter_add_async_c(second, 7, $0, $1) }
        let finalValue = try await awaitResult { SwiftBridgeCounter_add_async_c(first, 2, $0, $1) }
        precondition(value == 42 && taskValue == 42 && firstValue == 40 && secondValue == 7 && finalValue == 42)

        let generatedValue = try await ForeignAsyncNative.swift_bridge_work(41)
        precondition(generatedValue == 42)
        let scalarABI = try await ForeignAsyncNative.swift_bridge_scalars(
            true, 42, -8, 250, -11, 60000, -1, 7, -42, 41, Int64.min, UInt64.max, 1.25, 0.5
        )
        precondition(scalarABI)
        try await ForeignAsyncNative.swift_bridge_void()
        let generatedEnum = try await ForeignAsyncNative.swift_bridge_enum(SwiftBridgeChoice(rawValue: -1))
        precondition(generatedEnum.rawValue == 42)
        let record = SwiftBridgeRecord(
            metadata: SwiftBridgeMetadata(choice: SwiftBridgeChoice(rawValue: -1), accepted: false),
            values: (-1, 7, 41, CLong.max)
        )
        let asyncRecord = try await ForeignAsyncNative.swift_bridge_record(record)
        precondition(asyncRecord.metadata.choice.rawValue == 42 && asyncRecord.metadata.accepted)
        precondition(asyncRecord.values == (42, 7, 41, CLong.max))
        let checkedRecord = try ForeignAsyncNative.swift_bridge_checked_record(asyncRecord)
        precondition(checkedRecord.metadata.choice.rawValue == 42 && checkedRecord.metadata.accepted)
        precondition(checkedRecord.values == (42, 7, 42, CLong.max))
        do {
            _ = try await ForeignAsyncNative.swift_bridge_work(-1)
            preconditionFailure("generated Swift wrapper returned after checked failure")
        } catch let error as ForeignAsyncNativeError {
            precondition(error.withBorrowedNativeError { objc3_swift_error_code($0) } == 1)
        }
        let cancelled = Task {
            withUnsafeCurrentTask { $0?.cancel() }
            try await ForeignAsyncNative.swift_bridge_cancellable()
        }
        do {
            try await cancelled.value
            preconditionFailure("generated Swift wrapper discarded cancellation")
        } catch is CancellationError {}
        let generatedActor = try await ForeignAsyncNative.SwiftBridgeCounter_add(first, 0)
        precondition(generatedActor == finalValue)
        let generatedChecked = try ForeignAsyncNative.swift_bridge_checked(41)
        precondition(generatedChecked == 42)
        try ForeignAsyncNative.swift_bridge_checked_void(true)
        try checkGeneratedSynchronousError()
        precondition(objc3_swift_sync_errors_retired() == 1)
        print(#"""
            {
                "value": \#(value), "checked_error": true, "cancellation": true,
                "pre_cancelled": true, "cancelled_after_submit": true,
                "actor": \#(finalValue), "other_actor": \#(secondValue),
                "generated_value": \#(generatedValue), "generated_error": true,
                "generated_cancellation": true, "generated_scalars": \#(scalarABI),
                "generated_void": true, "generated_enum": \#(generatedEnum.rawValue),
                "generated_checked_value": \#(generatedChecked), "generated_checked_error": true,
                "generated_async_record": \#(asyncRecord.values.0),
                "generated_checked_record": \#(checkedRecord.values.2)
            }
            """#)
    }
}
