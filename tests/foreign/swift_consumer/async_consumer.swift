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
        print("{\"value\":\(value),\"checked_error\":true,\"cancellation\":true,\"pre_cancelled\":true,\"cancelled_after_submit\":true,\"actor\":\(finalValue),\"other_actor\":\(secondValue)}")
    }
}
