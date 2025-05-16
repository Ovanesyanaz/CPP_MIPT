#include <iostream>
#include <cassert>

// Реализация less_than_comparable
template <typename Derived>
class less_than_comparable {
public:
    friend bool operator>(const Derived& lhs, const Derived& rhs) {
        return rhs < lhs;
    }

    friend bool operator<=(const Derived& lhs, const Derived& rhs) {
        return !(rhs < lhs);
    }

    friend bool operator>=(const Derived& lhs, const Derived& rhs) {
        return !(lhs < rhs);
    }

    friend bool operator==(const Derived& lhs, const Derived& rhs) {
        return !(lhs < rhs) && !(rhs < lhs);
    }

    friend bool operator!=(const Derived& lhs, const Derived& rhs) {
        return (lhs < rhs) || (rhs < lhs);
    }
};

// Реализация counter
template <typename T>
class counter {
protected:
    counter() { ++count_; }
    counter(const counter&) { ++count_; }
    counter(counter&&) { ++count_; }
    ~counter() { --count_; }

public:
    static size_t count() { return count_; }

private:
    inline static size_t count_ = 0;
};

// Класс Number, использующий оба MixIn
class Number : public less_than_comparable<Number>, public counter<Number> {
public:
    Number(int value) : m_value{value} {}

    int value() const { return m_value; }

    bool operator<(const Number& other) const {
        return m_value < other.m_value;
    }

private:
    int m_value;
};

int main() {
    Number one{1};
    Number two{2};
    Number three{3};
    Number four{4};
    
    // Проверка операторов сравнения
    assert(one >= one);
    assert(three <= four);
    assert(two == two);
    assert(three > two);
    assert(one < two);
    
    // Проверка счетчика
    std::cout << "Count: " << counter<Number>::count() << std::endl; // Выведет 4
    
    Number five{5};
    
    std::cout << "Count after add number five: " << counter<Number>::count() << std::endl; // Теперь 5
    
    return 0;
}