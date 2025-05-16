#include <iostream>
int N = 10;
int& foo() {
    return N;
}

int baz(){
    return 42;
}

namespace temp{
    struct Base {
        double number = 42;
    };
    void foo(const Base *obj, double num){
        std::cout << "temp::foo\n " << "Base::number = " << obj->number 
        << "\nnum = " << num << std::endl;
    }
}

namespace temp2{
    int baz(temp::Base *obj) {
        return 1000 + obj->number;
    }
    
    struct Derived {
        temp::Base *m_obj;

        Derived(temp::Base *obj) : m_obj(obj) {}

        operator const  temp::Base() const {
            return *m_obj;
        }
    };
    template <class T>
    int foo (const T *obj, double num){
        std::cout << "temp2::foo" << typeid(T).name()
        << ">\nnum  = " << num << std::endl;
    }
    bool foo (const Derived *obj) {
        std::cout << "temp2::foo\n"
        << "Derived::m_obj::number = " << obj->m_obj->number << std::endl;
    }
}

namespace ca {
    template <class T>
    class ConstArray {
        public:
            ConstArray(std::size_t n, const T& value)
                m_data(new T[n]), m_size(n)
            {
                for (std::size_t i = 0; i < n; i++){
                    m_data[i] = value;
                }
            }
            ~ConstArray() {
                delete[] m_data;
            }

            std::size_t size() const { return m_size; }

            friend void swap(ConstArrey)
        private:
            const T* m_data;
            const std:;size_t m_size;
    };
}

int main(int argc, char const *argv[])
{
    //lvalue
    int a = 10; //переменная
    int &ref = a; //ссылка - тоже пременная
    int &&rref = 10; //правая ссылка - тоже переменная
    foo() = 20; //функция возвращающая левую ссылку
    //rvalue
    2, a+1; //литералы и выражениями над ними
    baz(); //функция возвращающая не ссылочкой тип
    static_cast<int&&>(a);
    //---------------------
    const char str[] = "simple string";
    std::cout << &str << std::endl;
    std::cout << &"string literal" << std::endl;

    temp::Base b;
    foo(&b, 10); //temp::foo
    temp2::Derived d(&b);
    foo(&d, 10); //temp2::foo<Derived>

    /*
    * ADL - argument dependent lookup
    * 1. Поиск имен (lookup)
        * Порядок поиска: 
            * текущая область видимости
            * глобальный область видимости
            * пространство имен аргументов
    * 2. Раскрытие концептов
    * 3. Раскрытие шаблонных аргуемнтов
    * 4. Разрешение перегрузок
    */

    return 0;
}
