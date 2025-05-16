#include <iostream>


namespace temp {
    struct Base {
        double number = 42;
    };

    void foo(const Base *obj, double num) {
        std::cout << "temp::foo\n" << "Base::number = " << obj->number
                  << "\nnum = " << num << std::endl;
    }
}


namespace temp2 {
    int baz(temp::Base *obj) {
        return 100 + obj->number;
    }

    struct Derived {
        temp::Base *m_obj;

        Derived(temp::Base *obj) : m_obj(obj) {}

        operator temp::Base &() {  // ОПЕРАТОР КАСТА
            return *m_obj;
        }
    };

    template<class T>
    int foo(const T &obj, double num) {
        std::cout << "temp2::foo<" << typeid(T).name()
                  << ">\nnum = " << num << std::endl;
        return 0;
    }

    bool foo(const Derived &obj) {
        std::cout << "temp2::foo\n" << "Derived::m_obj::number = " <<
        obj.m_obj->number << std::endl;
        return true;
    }
}


namespace ca {
    template<class T>
    class ConstArray {
    public:
        ConstArray(std::size_t n, const T& value) : m_data(new T[n]), m_size(n) {
            for (std::size_t i = 0; i < n; i++) {
                m_data[i] = value;
            }
        }

        ~ConstArray() {
            delete[] m_data;
        }

        std::size_t size() const { return m_size; }
        const T &operator[](std::size_t i) const { return m_data[i]; }

        friend void swap(ConstArray<T> &a, ConstArray<T> &b) {
            T *tmp = a.m_data;
            std::size_t tmp_size = a.m_size;
            a.m_data = b.m_data;
            a.m_size = b.m_size;
            b.m_data = tmp;
            b.m_size = tmp_size;
        }
    private:
        T *m_data;
        std::size_t m_size;
    };
}

// ХИТРЫЙ ТРЮЮЮЮК
template<class T>
void my_swap(T& a, T& b) {
    using std::swap;
    std::swap(a, b);
}


int main() {
    std::cout << "hi";  // std::operator<<(std::basic_ostream&, const char *)
    // тут короче ADL
    temp::Base b;
    temp::foo(&b, 10);  // OK
    foo(&b, 10);  // OK, хз почему :(

    temp2::Derived d(&b);
    foo(d, 10);


    ca::ConstArray<int> arr(10, 1), brr(20, 10);
    std::cout << arr.size() << " " << arr[0] << std::endl;
    std::cout << brr.size() << " " << brr[0] << std::endl;
    swap(arr, brr);
    std::cout << arr.size() << " " << arr[0] << std::endl;
    std::cout << brr.size() << " " << brr[0] << std::endl;
    return 0;
}
