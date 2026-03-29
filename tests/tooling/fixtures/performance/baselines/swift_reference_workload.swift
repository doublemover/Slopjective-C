struct BenchWidget {
    let name: String
    let count: Int

    func score() -> Int {
        var total = count
        for scalar in name.unicodeScalars {
            total += Int(scalar.value)
        }
        return total
    }
}

let widget = BenchWidget(name: "aurora", count: 7)
print(widget.score() % 31)
