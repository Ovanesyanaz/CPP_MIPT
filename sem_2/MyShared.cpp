#include <memory>
#include <iostream>
class OtherClass{};
OtherClass *ptr_global = new OtherClass;

class MyObject{
    std::unique_ptr<OtherClass> ptr = std::make_unique<OtherClass>();

};

template <typename T>
class MyShared {
    T* ptr_ = nullptr;

    unsigned int * counter_ = nullptr;

    public:
    //конструктор
    MyShared (T * && ptr):ptr_(ptr), counter_(new unsigned int){
        (*counter_)++;
        std::cout << "Ctr " << counter_ << " \n";
    }
    //копирующий конструктор
    MyShared(MyShared <T> &other){
        counter_ = other.counter_;
        (*counter_)++;
        ptr_ = other.ptr_;
        std::cout << "Cpy " << counter_ << " \n";
    }

    //деструктор
    ~MyShared(){
        *counter_ = *counter_ - 1;
        if ((*counter_) == 0){
            std::cout << "Dtr " << ptr_ << "\n";
            delete ptr_;
            delete counter_;
            std::cout << "Dtr\n";
        }
    }

};

int main(){
    MyShared<MyObject> ptr = new MyObject;
    auto simple_ptr = new MyObject;
    MyShared<MyObject> ptr2 = ptr;
    return 0;
}