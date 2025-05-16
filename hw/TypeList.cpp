#include <type_traits>

namespace TypeListUtils {

// Базовый шаблон TypeList
template<typename... Types>
struct TypeList {};

// Пустой TypeList
using EmptyTypeList = TypeList<>;

// Получение размера списка
template<typename TList>
struct Size;

template<typename... Types>
struct Size<TypeList<Types...>> {
    static constexpr std::size_t value = sizeof...(Types);
};

// Получение элемента по индексу
template<typename TList, std::size_t Index>
struct TypeAt;

// Ошибка - индекс за пределами
template<std::size_t Index>
struct TypeAt<EmptyTypeList, Index> {
    static_assert(Index < 0, "Index out of bounds");
};

// Базовый случай - индекс 0
template<typename Head, typename... Tail>
struct TypeAt<TypeList<Head, Tail...>, 0> {
    using type = Head;
};

// Рекурсивный случай
template<typename Head, typename... Tail, std::size_t Index>
struct TypeAt<TypeList<Head, Tail...>, Index> {
    using type = typename TypeAt<TypeList<Tail...>, Index - 1>::type;
};

// Проверка наличия типа в списке
template<typename TList, typename T>
struct Contains;

template<typename T>
struct Contains<EmptyTypeList, T> {
    static constexpr bool value = false;
};

template<typename Head, typename... Tail, typename T>
struct Contains<TypeList<Head, Tail...>, T> {
    static constexpr bool value = std::is_same_v<Head, T> || Contains<TypeList<Tail...>, T>::value;
};

// Получение индекса типа в списке
template<typename TList, typename T, std::size_t Index = 0>
struct IndexOf;

// Специализация для случая, когда тип не найден
template<typename T, std::size_t Index>
struct IndexOf<EmptyTypeList, T, Index> {
    static constexpr std::size_t value = static_cast<std::size_t>(-1);
};

template<typename Head, typename... Tail, typename T, std::size_t Index>
struct IndexOf<TypeList<Head, Tail...>, T, Index> {
    static constexpr std::size_t value = 
        std::is_same_v<Head, T> ? Index : IndexOf<TypeList<Tail...>, T, Index + 1>::value;
};

// Вспомогательная структура для проверки наличия типа перед получением индекса
template<typename TList, typename T>
struct SafeIndexOf {
private:
    static constexpr std::size_t index = IndexOf<TList, T>::value;
    static_assert(index != static_cast<std::size_t>(-1), "Type not found in TypeList");
public:
    static constexpr std::size_t value = index;
};

// Добавление типа в конец списка
template<typename TList, typename T>
struct Append;

template<typename... Types, typename T>
struct Append<TypeList<Types...>, T> {
    using type = TypeList<Types..., T>;
};

// Добавление типа в начало списка
template<typename TList, typename T>
struct Prepend;

template<typename... Types, typename T>
struct Prepend<TypeList<Types...>, T> {
    using type = TypeList<T, Types...>;
};

} // namespace TypeListUtils

// Тесты
using namespace TypeListUtils;

// Создаем тестовый TypeList
using MyList = TypeList<int, double, char, float>;

// Тест размера списка
static_assert(Size<MyList>::value == 4, "Size test failed");

// Тест получения элемента по индексу
static_assert(std::is_same_v<typename TypeAt<MyList, 0>::type, int>, "TypeAt 0 failed");
static_assert(std::is_same_v<typename TypeAt<MyList, 1>::type, double>, "TypeAt 1 failed");
static_assert(std::is_same_v<typename TypeAt<MyList, 2>::type, char>, "TypeAt 2 failed");
static_assert(std::is_same_v<typename TypeAt<MyList, 3>::type, float>, "TypeAt 3 failed");

// Тест проверки наличия типа
static_assert(Contains<MyList, int>::value, "Contains int failed");
static_assert(Contains<MyList, double>::value, "Contains double failed");
static_assert(!Contains<MyList, bool>::value, "Contains bool failed");

// Тест получения индекса типа
static_assert(SafeIndexOf<MyList, int>::value == 0, "IndexOf int failed");
static_assert(SafeIndexOf<MyList, double>::value == 1, "IndexOf double failed");
static_assert(SafeIndexOf<MyList, char>::value == 2, "IndexOf char failed");
static_assert(SafeIndexOf<MyList, float>::value == 3, "IndexOf float failed");

// Тест добавления в конец
using AppendedList = typename Append<MyList, bool>::type;
static_assert(std::is_same_v<typename TypeAt<AppendedList, 4>::type, bool>, "Append failed");
static_assert(Size<AppendedList>::value == 5, "Appended size failed");

// Тест добавления в начало
using PrependedList = typename Prepend<MyList, bool>::type;
static_assert(std::is_same_v<typename TypeAt<PrependedList, 0>::type, bool>, "Prepend failed");
static_assert(Size<PrependedList>::value == 5, "Prepended size failed");

int main() {
    return 0;
}