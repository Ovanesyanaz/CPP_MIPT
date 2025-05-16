#include <stdint.h>
#include <iostream>
template <typename T>
class MyShared {

    T * ptr_ = nullptr;
    unsigned int *counter_ = nullptr;
public:
    MyShared () {
        // new
        auto ptr = new uint8_t[sizeof (T) + sizeof(*counter_)];

        ptr_ = new(ptr + sizeof(*counter_)) T;
        std::cout << "ptr_\t" << (void *) ptr_ << std::endl; 
        counter_ = reinterpret_cast<unsigned int*>(ptr);
        std::cout << "counuter_\t" << counter_ << std::endl; 
        (*counter_)++;
    }

    ~MyShared () {
        if (counter_ ) {
            (*counter_)--;
            if ((*counter_) == 0) {
                std::cout << "удаляем" << std::endl;
                ptr_->~T();
                delete [] reinterpret_cast<uint8_t *>(counter_);
            }
        }
    }
    T * operator -> (){
        return ptr_;
    }
};
class MyClass{
public:
    float b = 5.7;
    int a;
    char c;
    MyClass () {
        std::cout << "MyClass создан" << std::endl;
    }
    ~MyClass () {
        std::cout << "MyClass удален" << std::endl;
    }
 };

int main(int argc, char const *argv[])
{
    std::cout << "Size of MyClass \t" << sizeof(MyClass) << std::endl;
    MyShared<MyClass> shared_ptr;
    std::cout << "some data \t" << shared_ptr->b << std::endl;
    return 0;
}
