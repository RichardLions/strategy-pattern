#pragma once

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <random/random.h>

namespace ReferenceSemantics
{
    class Value
    {
    public:
        virtual ~Value() = default;
        virtual void Operation() = 0;
    };

    template<typename TValue>
    class OperationStrategy
    {
    public:
        virtual ~OperationStrategy() = default;
        virtual void Operation(TValue&) = 0;
    };

    // IntValue
    class IntValue final : public Value
    {
    public:
        using OperationStrategy = OperationStrategy<IntValue>;

        explicit IntValue(
            const int32_t value, std::unique_ptr<OperationStrategy>&& operationStrategy)
            : m_OperationStrategy{std::move(operationStrategy)}
            , m_Value{value}
        {
        }

        void SetOperationStrategy(std::unique_ptr<OperationStrategy>&& operationStrategy)
        {
            m_OperationStrategy = std::move(operationStrategy);
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

    class DecrementIntValueOperationStrategy final : public IntValue::OperationStrategy
    {
    public:
        void Operation(IntValue& value) override
        {
            value.SetValue(value.GetValue() - 1);
        }
    };

    // FloatValue
    class FloatValue final : public Value
    {
    public:
        using OperationStrategy = OperationStrategy<FloatValue>;

        explicit FloatValue(
            const float_t value, std::unique_ptr<OperationStrategy>&& operationStrategy)
            : m_OperationStrategy{std::move(operationStrategy)}
            , m_Value{value}
        {
        }

        void SetOperationStrategy(std::unique_ptr<OperationStrategy>&& operationStrategy)
        {
            m_OperationStrategy = std::move(operationStrategy);
        }

        void Operation() override
        {
            m_OperationStrategy->Operation(*this);
        }

        float_t GetValue() const { return m_Value; }
        void SetValue(const float_t value) { m_Value = value; }
    private:
        std::unique_ptr<OperationStrategy> m_OperationStrategy{};
        float_t m_Value{0.0f};
    };

    class IncrementFloatValueOperationStrategy final : public FloatValue::OperationStrategy
    {
    public:
        void Operation(FloatValue& value) override
        {
            value.SetValue(value.GetValue() + 1.0f);
        }
    };

    class DecrementFloatValueOperationStrategy final : public FloatValue::OperationStrategy
    {
    public:
        void Operation(FloatValue& value) override
        {
            value.SetValue(value.GetValue() - 1.0f);
        }
    };

    std::unique_ptr<Value> CreateRandomValue()
    {
        if(Random::RandomBool())
        {
            std::unique_ptr<IntValue::OperationStrategy> operationStrategy{
                []() -> std::unique_ptr<IntValue::OperationStrategy>
                {
                    if(Random::RandomBool())
                        return std::make_unique<IncrementIntValueOperationStrategy>();

                    return std::make_unique<DecrementIntValueOperationStrategy>();
                }()};

            return std::make_unique<IntValue>(0, std::move(operationStrategy));
        }

        std::unique_ptr<FloatValue::OperationStrategy> operationStrategy{
            []() -> std::unique_ptr<FloatValue::OperationStrategy>
            {
                if(Random::RandomBool())
                    return std::make_unique<IncrementFloatValueOperationStrategy>();

                return std::make_unique<DecrementFloatValueOperationStrategy>();
            }()};

        return std::make_unique<FloatValue>(0.0f, std::move(operationStrategy));
    }

    TEST_CASE("Strategy - Reference Semantics - Unit Tests")
    {
        SECTION("IntValue Operations")
        {
            IntValue intValue{0, std::make_unique<IncrementIntValueOperationStrategy>()};
            REQUIRE(intValue.GetValue() == 0);

            Value* const value{&intValue};
            value->Operation();
            REQUIRE(intValue.GetValue() == 1);
            value->Operation();
            REQUIRE(intValue.GetValue() == 2);

            intValue.SetOperationStrategy(std::make_unique<DecrementIntValueOperationStrategy>());
            REQUIRE(intValue.GetValue() == 2);

            value->Operation();
            REQUIRE(intValue.GetValue() == 1);
            value->Operation();
            REQUIRE(intValue.GetValue() == 0);
        }

        SECTION("FloatValue Operations")
        {
            FloatValue floatValue{0.0f, std::make_unique<IncrementFloatValueOperationStrategy>()};
            REQUIRE(floatValue.GetValue() == 0.0f);

            Value* const value{&floatValue};
            value->Operation();
            REQUIRE(floatValue.GetValue() == 1.0f);
            value->Operation();
            REQUIRE(floatValue.GetValue() == 2.0f);

            floatValue.SetOperationStrategy(std::make_unique<DecrementFloatValueOperationStrategy>());
            REQUIRE(floatValue.GetValue() == 2.0f);

            value->Operation();
            REQUIRE(floatValue.GetValue() == 1.0f);
            value->Operation();
            REQUIRE(floatValue.GetValue() == 0.0f);
        }
    }

    TEST_CASE("Strategy - Reference Semantics - Benchmark")
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
