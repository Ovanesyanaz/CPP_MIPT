#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>

// Исходная реализация TypeList без изменений
namespace TypeListUtils {
    template<typename... Types> struct TypeList {};
    
    template<typename TList> struct Size;
    template<typename... Types> struct Size<TypeList<Types...>> {
        static constexpr std::size_t value = sizeof...(Types);
    };
    
    template<typename TList, std::size_t Index> struct TypeAt;
    template<std::size_t Index> struct TypeAt<TypeList<>, Index> {
        static_assert(Index < 0, "Index out of bounds");
    };
    template<typename Head, typename... Tail> struct TypeAt<TypeList<Head, Tail...>, 0> {
        using type = Head;
    };
    template<typename Head, typename... Tail, std::size_t Index> 
    struct TypeAt<TypeList<Head, Tail...>, Index> {
        using type = typename TypeAt<TypeList<Tail...>, Index - 1>::type;
    };
    
    template<typename TList, typename T> struct Contains;
    template<typename T> struct Contains<TypeList<>, T> {
        static constexpr bool value = false;
    };
    template<typename Head, typename... Tail, typename T> 
    struct Contains<TypeList<Head, Tail...>, T> {
        static constexpr bool value = std::is_same_v<Head, T> || Contains<TypeList<Tail...>, T>::value;
    };
    
    template<typename TList, typename T, std::size_t Index = 0> struct IndexOf;
    template<typename T, std::size_t Index> struct IndexOf<TypeList<>, T, Index> {
        static constexpr std::size_t value = static_cast<std::size_t>(-1);
    };
    template<typename Head, typename... Tail, typename T, std::size_t Index> 
    struct IndexOf<TypeList<Head, Tail...>, T, Index> {
        static constexpr std::size_t value = 
            std::is_same_v<Head, T> ? Index : IndexOf<TypeList<Tail...>, T, Index + 1>::value;
    };
} // namespace TypeListUtils

// Реализация TypeMap
template<typename... Keys>
class TypeMap {
private:
    // Хранилище значений для каждого типа ключа
    std::tuple<Keys...> values;
    
    // Получение индекса типа в списке ключей
    template<typename Key>
    static constexpr std::size_t getKeyIndex() {
        constexpr std::size_t index = TypeListUtils::IndexOf<TypeListUtils::TypeList<Keys...>, Key>::value;
        static_assert(index != static_cast<std::size_t>(-1), "Key not found in TypeMap");
        return index;
    }

public:
    // Добавление значения по типу ключа
    template<typename Key>
    void AddValue(Key value) {
        std::get<getKeyIndex<Key>()>(values) = value;
    }
    
    // Получение значения по типу ключа
    template<typename Key>
    Key& GetValue() {
        return std::get<getKeyIndex<Key>()>(values);
    }
    
    template<typename Key>
    const Key& GetValue() const {
        return std::get<getKeyIndex<Key>()>(values);
    }
    
    // Проверка наличия типа ключа
    template<typename Key>
    constexpr bool Contains() const {
        return TypeListUtils::Contains<TypeListUtils::TypeList<Keys...>, Key>::value;
    }
    
    // Удаление значения (сброс в значение по умолчанию)
    template<typename Key>
    void RemoveValue() {
        std::get<getKeyIndex<Key>()>(values) = Key{};
    }
};

// Пример использования
struct DataA {
    std::string value;
    friend std::ostream& operator<<(std::ostream& os, const DataA& data) {
        return os << data.value;
    }
};

struct DataB {
    int value;
    friend std::ostream& operator<<(std::ostream& os, const DataB& data) {
        return os << data.value;
    }
};

int main() {
    // Создаем TypeMap с указанием типов ключей
    TypeMap<int, double, DataA, DataB> myTypeMap;
    
    // Добавление элементов
    myTypeMap.AddValue<int>(42);
    myTypeMap.AddValue<double>(3.14);
    myTypeMap.AddValue<DataA>({"Hello, TypeMap!"});
    myTypeMap.AddValue<DataB>({10});
    
    // Получение и вывод значений
    std::cout << "Value for int: " << myTypeMap.GetValue<int>() << std::endl;
    std::cout << "Value for double: " << myTypeMap.GetValue<double>() << std::endl;
    std::cout << "Value for DataA: " << myTypeMap.GetValue<DataA>().value << std::endl;
    std::cout << "Value for DataB: " << myTypeMap.GetValue<DataB>().value << std::endl;
    
    // Проверка наличия
    std::cout << "Contains int? " << (myTypeMap.Contains<int>() ? "Yes" : "No") << std::endl;
    
    // Удаление элемента
    myTypeMap.RemoveValue<double>();
    
    // Попытка получения удаленного элемента
    std::cout << "Value for double after removal: " << myTypeMap.GetValue<double>() << std::endl;
    
    return 0;
}