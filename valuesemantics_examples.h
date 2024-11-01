#pragma once

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <random/random.h>
#include <functional>

namespace ValueSemantics
{
    class Value
    {
    public:
        virtual ~Value() = default;
        virtual void Operation() = 0;
    };

    // IntValue
    class IntValue final : public Value
    {
    public:
        using OperationStrategy = std::function<void(IntValue&)>;

        explicit IntValue(
            const int32_t value, OperationStrategy&& operationStrategy)
            : m_OperationStrategy{std::move(operationStrategy)}
            , m_Value{value}
        {
        }

        void SetOperationStrategy(OperationStrategy&& operationStrategy)
        {
            m_OperationStrategy = std::move(operationStrategy);
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

    IntValue::OperationStrategy GetDecrementIntValueOperationStrategy()
    {
        return
            [](IntValue& value)
            {
                value.SetValue(value.GetValue() - 1);
            };
    }

    // FloatValue
    class FloatValue final : public Value
    {
    public:
        using OperationStrategy = std::function<void(FloatValue&)>;

        explicit FloatValue(
            const float_t value, OperationStrategy&& operationStrategy)
            : m_OperationStrategy{std::move(operationStrategy)}
            , m_Value{value}
        {
        }

        void SetOperationStrategy(OperationStrategy&& operationStrategy)
        {
            m_OperationStrategy = std::move(operationStrategy);
        }

        void Operation() override
        {
            m_OperationStrategy(*this);
        }

        float_t GetValue() const { return m_Value; }
        void SetValue(const float_t value) { m_Value = value; }
    private:
        OperationStrategy m_OperationStrategy{};
        float_t m_Value{0.0f};
    };

    class IncrementFloatValueOperationStrategy
    {
    public:
        void operator()(FloatValue& value)
        {
            value.SetValue(value.GetValue() + 1.0f);
        }
    };

    FloatValue::OperationStrategy GetDecrementFloatValueOperationStrategy()
    {
        return
            [](FloatValue& value)
            {
                value.SetValue(value.GetValue() - 1.0f);
            };
    }

    std::unique_ptr<Value> CreateRandomValue()
    {
        if(Random::RandomBool())
        {
            return std::make_unique<IntValue>(0,
                Random::RandomBool() ?
                    IncrementIntValueOperationStrategy{} : GetDecrementIntValueOperationStrategy());
        }

        return std::make_unique<FloatValue>(0.0f,
            Random::RandomBool() ?
                IncrementFloatValueOperationStrategy{} : GetDecrementFloatValueOperationStrategy());
    }

    TEST_CASE("Strategy - Value Semantics - Unit Tests")
    {
        SECTION("IntValue Operations")
        {
            IntValue intValue{0, IncrementIntValueOperationStrategy{}};
            REQUIRE(intValue.GetValue() == 0);

            Value* const value{&intValue};
            value->Operation();
            REQUIRE(intValue.GetValue() == 1);
            value->Operation();
            REQUIRE(intValue.GetValue() == 2);

            intValue.SetOperationStrategy(GetDecrementIntValueOperationStrategy());
            REQUIRE(intValue.GetValue() == 2);

            value->Operation();
            REQUIRE(intValue.GetValue() == 1);
            value->Operation();
            REQUIRE(intValue.GetValue() == 0);
        }

        SECTION("FloatValue Operations")
        {
            FloatValue floatValue{0.0f, IncrementFloatValueOperationStrategy{}};
            REQUIRE(floatValue.GetValue() == 0.0f);

            Value* const value{&floatValue};
            value->Operation();
            REQUIRE(floatValue.GetValue() == 1.0f);
            value->Operation();
            REQUIRE(floatValue.GetValue() == 2.0f);

            floatValue.SetOperationStrategy(GetDecrementFloatValueOperationStrategy());
            REQUIRE(floatValue.GetValue() == 2.0f);

            value->Operation();
            REQUIRE(floatValue.GetValue() == 1.0f);
            value->Operation();
            REQUIRE(floatValue.GetValue() == 0.0f);
        }
    }

    TEST_CASE("Strategy - Value Semantics - Benchmark")
    {
        BENCHMARK("Benchmark")
        {
            constexpr uint32_t valueCount{50'000};
            std::vector<std::unique_ptr<Value>> values{};
            values.reserve(valueCount);

            for(uint32_t i{0}; i != valueCount; ++i)
            {
                values.push_back(CreateRandomValue());
            }

            for(const std::unique_ptr<Value>& value: values)
            {
                value->Operation();
            }
        };
    }
}
