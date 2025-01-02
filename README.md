# ExTL - A No-Exception C++ Template Library

ExTL is a specialized C++ template library designed specifically for environments where exceptions are disabled or undesirable. It provides a lightweight, robust alternative to C++'s native exception handling mechanism, centering around the use of `expected<T, E>` to handle error conditions.

## Key Features

- **No-Exception Design**: ExTL assumes a no-exception environment, meaning that all critical operations are expected to be `noexcept`.
- `expected<T, E>`: A versatile type for representing either a value of type `T` or an error of type `E`. This enables expressive, type-safe error handling without relying on exceptions.
- **Lifecycle Function Assumptions**: ExTL assumes that lifecycle-related operations such as construction, destruction, move, and copy are guaranteed to succeed. For operations where construction or copying might fail, explicit alternatives like `T::create(...) -> expected<T, E>` and `T::copy(const T&) -> expected<T, E>` should be used. Violating these assumptions results in undefined behavior.

## Design Philosophy

ExTL is built on the premise that exceptions, while powerful, may not be suitable for certain performance-critical or low-level applications. By replacing exceptions with `expected<T, E>`, developers can achieve predictable error handling with minimal overhead.

### Assumptions

1. **No-Throw Lifecycle Functions**:

   - Constructors, destructors, move constructors, move assignments, copy constructors, and copy assignments are assumed to be `noexcept`. Any violation of this assumption leads to undefined behavior.

2. **Alternative Error Handling**:

   - For operations that may fail during construction or copying, ExTL provides explicit alternatives:
     - Use `T::create(...) -> expected<T, E>` for constructing objects.
     - Use `T::copy(const T&) -> expected<T, E>` for copying objects.
   - These methods return `expected<T, E>` to indicate success or failure explicitly.

3. **Guaranteed Move and Destruction**:

   - Move operations and destructors must always succeed. This assumption is critical for the library's design and guarantees.

## Why Use ExTL?

- **Predictability**: Avoids the non-deterministic behavior of exceptions in a no-exception environment.
- **Performance**: Eliminates the runtime cost of exceptions, making it ideal for performance-critical systems.
- **Type-Safe Error Handling**: `expected<T, E>` enforces explicit error handling, improving code readability and safety.

## Requirements

- A C++ compiler that supports C++20 or later.
- A no-exception build configuration.

## Limitations

- **Undefined Behavior**: If lifecycle functions (e.g., constructors, destructors, move constructors) throw exceptions or if move and destruction operations fail, the behavior is undefined.
## Contributing

Contributions to ExTL are welcome! If you find bugs or have feature suggestions, feel free to open an issue or submit a pull request.

## License

ExTL is licensed under the Apache License 2.0. See the [LICENSE](LICENSE) file for details.

