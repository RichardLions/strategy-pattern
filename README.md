# Strategy Pattern

This pattern was inspired by [C++ Software Design - Chapter 5](https://www.oreilly.com/library/view/c-software-design/9781098113155/).

## When To Use

The Strategy pattern is used to create an extension point on how something is done. This is similar to the Command pattern, however they have a different purpose. Strategy is to change how something is done, Command is more open and can do anything on a type. An example is std::sort(...) uses a Strategy to extend how to sort a collect, whereas std::for_each(...) uses a command to do anything on each item within a collect.

This pattern is best used when only a small number of functionality needs to be extended. It can become cumbersome to create an extension point on a type for many different pieces of functionality.

Strategies can be implemented via inheritance (Reference Semantics), callables (Value Semantics) and templates. Note: The template implementation has the best performance but the Strategy needs to be known at compile which may rule out this option depending on requirements.

## Examples

Examples are all based on:
```cpp
class Value
{
public:
    virtual ~Value() = default;
    virtual void Operation() = 0;
};
```

Inheritance (Reference Semantics):
```cpp
template<typename TValue>
class OperationStrategy
{
public:
    virtual ~OperationStrategy() = default;
    virtual void Operation(TValue&) = 0;
};

class IntValue final : public Value
{
public:
    using OperationStrategy = OperationStrategy<class IntValue>;

    explicit IntValue(
        const int32_t value, std::unique_ptr<OperationStrategy>&& operationStrategy)
        : m_OperationStrategy{std::move(operationStrategy)}
        , m_Value{value}
    {
    }

    void Operation() override
    {
        m_OperationStrategy->Operation(*this);
    }

    int32_t GetValue() const { return m_Value; }
    void SetValue(const int32_t value) { m_Value = value; }
private:
    std::unique_ptr<OperationStrategy> m_OperationStrategy{};
    int32_t m_Value{0};
};

class IncrementIntValueOperationStrategy final : public IntValue::OperationStrategy
{
public:
    void Operation(IntValue& value) override
    {
        value.SetValue(value.GetValue() + 1);
    }
};

{
    const std::unique_ptr<Value> value{
        std::make_unique<IntValue>(0, std::make_unique<IncrementIntValueOperationStrategy>()};
    value->Operation();
}
```

Callables (Value Semantics):
```cpp
class IntValue final : public Value
{
public:
    using OperationStrategy = std::function<void(class IntValue&)>;

    explicit IntValue(
        const int32_t value, OperationStrategy&& operationStrategy)
        : m_OperationStrategy{std::move(operationStrategy)}
        , m_Value{value}
    {
    }

    void Operation() override
    {
        m_OperationStrategy(*this);
    }

    int32_t GetValue() const { return m_Value; }
    void SetValue(const int32_t value) { m_Value = value; }
private:
    OperationStrategy m_OperationStrategy{};
    int32_t m_Value{0};
};

class IncrementIntValueOperationStrategy
{
public:
    void operator()(IntValue& value)
    {
        value.SetValue(value.GetValue() + 1);
    }
};

{
    const std::unique_ptr<Value> value{std::make_unique<IntValue>(0, IncrementIntValueOperationStrategy{})};
    value->Operation();
}
```

Template:
```cpp
template<typename TOperationStrategy>
class IntValue final : public Value
{
public:
    explicit IntValue(const int32_t value)
        : m_Value{value}
    {
    }

    void Operation() override
    {
        m_OperationStrategy(*this);
    }

    int32_t GetValue() const { return m_Value; }
    void SetValue(const int32_t value) { m_Value = value; }
private:
    TOperationStrategy m_OperationStrategy{};
    int32_t m_Value{0};
};

class IncrementIntValueOperationStrategy
{
public:
    void operator()(IntValue<IncrementIntValueOperationStrategy>& value)
    {
        value.SetValue(value.GetValue() + 1);
    }
};

{
    const std::unique_ptr<Value> value{std::make_unique<IntValue<IncrementIntValueOperationStrategy>>(0)};
    value->Operation();
}
```

## Setup

This repository uses the .sln/.proj files created by Visual Studio 2022 Community Edition.
Using MSVC compiler, Preview version(C++23 Preview). 

### Catch2
The examples for how to use the pattern are written as Unit Tests.

Launching the program in Debug or Release will run the Unit Tests.

Alternative:
Installing the Test Adapter for Catch2 Visual Studio extension enables running the Unit Tests via the Test Explorer Window. Setup the Test Explorer to use the project's .runsettings file.

### vcpkg
This repository uses vcpkg in manifest mode for it's dependencies. To interact with vcpkg, open a Developer PowerShell (View -> Terminal).

To setup vcpkg, install it via the Visual Studio installer. To enable/disable it run these commands from the Developer PowerShell:
```
vcpkg integrate install
vcpkg integrate remove
```

To add additional dependencies run:
```
vcpkg add port [dependency name]
```

To update the version of a dependency modify the overrides section of vcpkg.json. 

To create a clean vcpkg.json and vcpkg-configuration.json file run:
```
vcpkg new --application
```

### TODO
- [x] Reference Semantics Implementation
- [x] Reference Semantics Unit Tests/Benchmarking
- [x] Value Semantics Implementation Example
- [x] Value Semantics Unit Tests/Benchmarking
- [x] Template Implementation
- [x] Template Unit Tests/Benchmarking
