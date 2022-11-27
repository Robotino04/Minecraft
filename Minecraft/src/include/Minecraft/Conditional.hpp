#pragma once

namespace Minecraft{

template<typename T>
struct Conditional{
    
    Conditional(bool const& condition): condition(condition){};
    Conditional(bool const& condition, T const& value): condition(condition), value(value){};

    auto operator = (T& newValue){
        return value = newValue;
    }

    bool isPresent() const{
        return condition;
    }

    bool operator ==(Conditional<T> const& other) const{
        return this->condition == other.condition && (!this->isPresent() || this->value == other.value);
    }

    T value;
    bool const& condition;
};

}