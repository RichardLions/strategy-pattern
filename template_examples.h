#pragma once

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <random/random.h>

namespace Template
{
    class Value
    {
    public:
        virtual ~Value() = default;
        virtual void Operation() = 0;
    };

    // IntValue
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

    class DecrementIntValueOperationStrategy
    {
    public:
        void operator()(IntValue<DecrementIntValueOperationStrategy>& value)
        {
            value.SetValue(value.GetValue() - 1);
        }
    };

    // FloatValue
    template<typename TOperationStrategy>
    class FloatValue final : public Value
    {
    public:
        explicit FloatValue(const float_t value)
            : m_Value{value}
        {
        }

        void Operation() override
        {
            m_OperationStrategy(*this);
        }

        float_t GetValue() const { return m_Value; }
        void SetValue(const float_t value) { m_Value = value; }
    private:
        TOperationStrategy m_OperationStrategy{};
        float_t m_Value{0.0f};
    };

    class IncrementFloatValueOperationStrategy
    {
    public:
        void operator()(FloatValue<IncrementFloatValueOperationStrategy>& value)
        {
            value.SetValue(value.GetValue() + 1.0f);
        }
    };

    class DecrementFloatValueOperationStrategy
    {
    public:
        void operator()(FloatValue<DecrementFloatValueOperationStrategy>& value)
        {
            value.SetValue(value.GetValue() - 1.0f);
        }
    };

    std::unique_ptr<Value> CreateRandomValue()
    {
        if(Random::RandomBool())
        {
            if(Random::RandomBool())
                return std::make_unique<IntValue<IncrementIntValueOperationStrategy>>(0);

            return std::make_unique<IntValue<DecrementIntValueOperationStrategy>>(0);
        }

        if(Random::RandomBool())
            return std::make_unique<FloatValue<IncrementFloatValueOperationStrategy>>(0.0f);

        return std::make_unique<FloatValue<DecrementFloatValueOperationStrategy>>(0.0f);
    }

    TEST_CASE("Strategy - Template - Unit Tests")
    {
        SECTION("IntValue Increment Operation")
        {
            IntValue<IncrementIntValueOperationStrategy> intValue{0};
            REQUIRE(intValue.GetValue() == 0);

            Value* const value{&intValue};
            value->Operation();
            REQUIRE(intValue.GetValue() == 1);
            value->Operation();
            REQUIRE(intValue.GetValue() == 2);
        }

        SECTION("IntValue Decrement Operation")
        {
            IntValue<DecrementIntValueOperationStrategy> intValue{0};
            REQUIRE(intValue.GetValue() == 0);

            Value* const value{&intValue};
            value->Operation();
            REQUIRE(intValue.GetValue() == -1);
            value->Operation();
            REQUIRE(intValue.GetValue() == -2);
        }

        SECTION("FloatValue Increment Operation")
        {
            FloatValue<IncrementFloatValueOperationStrategy> floatValue{0.0f};
            REQUIRE(floatValue.GetValue() == 0.0f);

            Value* const value{&floatValue};
            value->Operation();
            REQUIRE(floatValue.GetValue() == 1.0f);
            value->Operation();
            REQUIRE(floatValue.GetValue() == 2.0f);
        }

        SECTION("IntValue Decrement Operation")
        {
            FloatValue<DecrementFloatValueOperationStrategy> floatValue{0.0f};
            REQUIRE(floatValue.GetValue() == 0.0f);

            Value* const value{&floatValue};
            value->Operation();
            REQUIRE(floatValue.GetValue() == -1.0f);
            value->Operation();
            REQUIRE(floatValue.GetValue() == -2.0f);
        }
    }

    TEST_CASE("Strategy - Template - Benchmark")
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
