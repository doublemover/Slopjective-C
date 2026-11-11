import ForeignAPI

let value: Int32 = projectedValue(41)
let amount: Amount = makeAmount(42)
var input: Int32 = 42
let read: Int32 = projectedRead(&input)
if value != 42 || amount.rawValue != 42 || read != 42 {
  fatalError("primitive/newtype/nullability contract changed")
}
if objc3_swift_live() != 0 { fatalError("unexpected initial C resource") }
let token = objc3_swift_create(42)
if objc3_swift_live() != 1 || objc3_swift_token_value(token) != 42 {
  fatalError("opaque C resource was not created correctly")
}
objc3_swift_destroy(token)
if objc3_swift_live() != 0 { fatalError("opaque C resource did not retire") }
print("{\"value\":42,\"amount\":42,\"read\":42,\"live\":0}")
