# ExTL
A C++ template library that uses expected as an exception handling mechanism, tailored for environments lacking standard exception support.

## Key Features

- **No-Exception Design**: ExTL assumes a no-exception environment, meaning that it does not rely on exceptions for error handling. Instead, it uses `expected<T, E>` to represent errors and propagate them through the call stack.
- **`expected<T, E>`**: A versatile type for representing either a value of type `T` or an error of type `E`. This enables expressive, type-safe error handling without relying on exceptions.
- **Explicit Lifecycle Functions**: ExTL assumes that lifecycle-related operations such as construction, destruction, move, and copy are guaranteed to succeed. For operations where construction or copying might fail, explicit alternatives like `T::create(...)` and `T::copy(const T&)` should be used.

## Requirements

- A C++ compiler that supports C++20 or later.
- A no-exception build configuration.

## Contributing

Contributions to ExTL are welcome! If you find bugs or have feature suggestions, feel free to open an issue or submit a pull request.

## License

ExTL is licensed under the Apache License 2.0. See the [LICENSE](LICENSE) file for details.
