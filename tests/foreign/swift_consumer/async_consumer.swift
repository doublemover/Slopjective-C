import ForeignAsync

private enum NativeError: Error {
    case checkedFailure
    case invalidCompletion
}

private typealias Completion = @convention(c) (
    UnsafeMutableRawPointer?, objc_async_completion_status_t, CInt,
    UnsafeMutableRawPointer?
) -> Void

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

        let first = objc3_swift_counter_create()
        let second = objc3_swift_counter_create()
        defer {
            objc3_swift_counter_release(first)
            objc3_swift_counter_release(second)
        }
        let firstValue = try await awaitResult { SwiftBridgeCounter_add_async_c(first, 40, $0, $1) }
        let secondValue = try await awaitResult { SwiftBridgeCounter_add_async_c(second, 7, $0, $1) }
        let finalValue = try await awaitResult { SwiftBridgeCounter_add_async_c(first, 2, $0, $1) }
        precondition(value == 42 && firstValue == 40 && secondValue == 7 && finalValue == 42)
        print("{\"value\":\(value),\"checked_error\":true,\"cancellation\":true,\"actor\":\(finalValue),\"other_actor\":\(secondValue)}")
    }
}
