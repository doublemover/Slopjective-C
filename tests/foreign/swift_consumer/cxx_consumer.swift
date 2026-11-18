import ForeignCxx

let before: MeterAPI.Reading = MeterAPI.initialReading()
let after: MeterAPI.Reading = MeterAPI.adjustedReading(before, 2)
let count: Int32 = after.count
let total: Int64 = after.total
precondition(MemoryLayout<MeterAPI.Reading>.size == 16)
precondition(MemoryLayout<MeterAPI.Reading>.alignment == 8)
precondition(before.count == 40 && before.total == 2147483688)
precondition(count == 42 && total == 2147483690)
print("{\"before\":\(before.count),\"count\":\(count),\"total\":\(total)}")
