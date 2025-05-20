#include <type_traits>
#include <string>
#include <vector>
#include <optional>
#include <any>
#include <iostream>

namespace TypeListDetail {
    template <typename... Types>
    struct TypeList {};

    template <typename... Types>
    struct Size;

    template <typename... Types>
    struct Size<TypeList<Types...>> {
        static constexpr std::size_t value = sizeof...(Types);
    };

    template <std::size_t N, typename TypeList>
    struct TypeAt;

    template <std::size_t N, typename Head, typename... Tail>
    struct TypeAt<N, TypeList<Head, Tail...>> {
        using type = typename TypeAt<N - 1, TypeList<Tail...>>::type;
    };

    template <typename Head, typename... Tail>
    struct TypeAt<0, TypeList<Head, Tail...>> {
        using type = Head;
    };

    template <typename T, typename TypeList>
    struct Contains;

    template <typename T, typename... Types>
    struct Contains<T, TypeList<Types...>> {
        static constexpr bool value = (std::is_same_v<T, Types> || ...);
    };

    template <typename T, typename TypeList, std::size_t Index = 0>
    struct IndexOf;

    template <typename T, typename Head, typename... Tail, std::size_t Index>
    struct IndexOf<T, TypeList<Head, Tail...>, Index> {
        static constexpr std::size_t value = std::is_same_v<T, Head> ? Index : IndexOf<T, TypeList<Tail...>, Index + 1>::value;
    };

    template <typename T, std::size_t Index>
    struct IndexOf<T, TypeList<>, Index> {
        static constexpr std::size_t value = static_cast<std::size_t>(-1);
    };

    template <typename T, typename TypeList>
    struct PushBack;

    template <typename T, typename... Types>
    struct PushBack<T, TypeList<Types...>> {
        using type = TypeList<Types..., T>;
    };

    template <typename T, typename TypeList>
    struct PushFront;

    template <typename T, typename... Types>
    struct PushFront<T, TypeList<Types...>> {
        using type = TypeList<T, Types...>;
    };
}

template <typename... Types>
using TypeList = TypeListDetail::TypeList<Types...>;

template<typename... Args>
struct TypeMap;

template<typename Head, typename... Tail>
struct TypeMap<Head, Tail...>
{
    TypeMap()
    {
        using keys = TypeList<Head, Tail...>;
        values.resize(sizeof...(Tail)+1);
    };

    template<typename T> void addValue(T value)
    {
        constexpr int i = TypeListDetail::IndexOf<T, keys>::value;
        values[i] = std::make_optional(std::any(value));
    };

    template<typename T> T getValue()
    {
        int i = TypeListDetail::IndexOf<T, keys>::value;
        if (values[i]) {
            T from_any = std::any_cast<T>(values[i].value());
            return from_any;
        }
        
        throw std::invalid_argument( "getValue(): value for this key doesn`t exists" );
    };
    
    template<typename T> bool valueExists()
    {
        int i = TypeListDetail::IndexOf<T, keys>::value;
        return values[i].has_value();
    }

    template<typename T> bool Contains()
    {
        return TypeListDetail::Contains<T, keys>::value;
    }
    
    template<typename T> void removeValue()
    {
        constexpr int i = TypeListDetail::IndexOf<T, keys>::value;
        values[i] = std::nullopt;
    };

    typedef TypeList<Head, Tail...> keys;
    std::vector<std::optional<std::any>> values;
};

int main()
{
    struct MyType1 {
        std::string value;
    };
    struct MyType2 {
        int value;
    };
    
    TypeMap<int, MyType1, double, MyType2> myTypeMap;
    
    myTypeMap.addValue<double>(2.72);
    myTypeMap.addValue<MyType1>({"Hello, World!"});
    myTypeMap.addValue<int>(52);
    myTypeMap.addValue<MyType2>({13});

    std::cout << "Value for int: "<<myTypeMap.getValue<int>() << std::endl;
    std::cout << "Value for double: "
              << myTypeMap.getValue<double>() << std::endl;
    std::cout << "Value for MyType1: "
              << myTypeMap.getValue<MyType1>().value << std::endl;
    std::cout << "Value for MyType2: "
              << myTypeMap.getValue<MyType2>().value << std::endl;

    std::cout << "Contains int? "
              << (myTypeMap.Contains<int>() ? "Yes" : "No") << std::endl;
              
    std::cout << "Value for double exists before removal: "
              << (myTypeMap.valueExists<double>() ? "Yes" : "No") << std::endl;

    myTypeMap.removeValue<double>();
    
    std::cout << "Value for double exists after removal: "
              << (myTypeMap.valueExists<double>() ? "Yes" : "No") << std::endl;

    std::cout << "Try get value for double after removal: "
              << myTypeMap.getValue<double>() << std::endl;
    return 0;
};