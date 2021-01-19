#ifndef WEAK_PTR_H
#define WEAK_PTR_H
#include <atomic>
#include <QDebug>
constexpr bool dbgShrPtr = true;
#define DBG if constexpr (dbgShrPtr) qDebug

template <class T> class SharedPtr;

template <class T> class SharedPtr{
public:
    template <typename ...Args>
    static SharedPtr<T> create(Args&&... args){
        DBG() << "create";
        return SharedPtr(new T(std::forward<Args>(args)...));
    }

    template <class D, std::enable_if_t<std::is_base_of_v<T, D>, bool> = true> void swap(SharedPtr<D>& ref){
        DBG() << "swap derived to T";
        std::swap(ref, *this);
    }
    template <class D, std::enable_if_t<std::is_base_of_v<D, T>, bool> = true> void swap(SharedPtr<D>& ref){
        DBG() << "swap T to D";
        std::swap(ref.counter, counter);
        D* temp = ref.raw;
        ref.raw = static_cast<D*>(raw); //static_cast or dynamic_cast, need to check it out
        ref = temp;
    }


    //copy c-tor for
    template <class D, typename std::enable_if_t<std::is_base_of_v<T,D>, bool> = true>
    explicit SharedPtr(SharedPtr<T>& ref){
        DBG() << "copy - ctor";
        counter = ref.counter;
        raw = ref.raw;
    }
    //    template <class D, typename std::enable_if_t<std::is_base_of_v<D,T>, bool> = true>
    //    explicit SharedPtr(SharedPtr<T>& ref){
    //        counter = ref.counter;
    //        raw = ref.raw;
    //    }

    //assign operator
    template <class D, typename std::enable_if_t<std::is_base_of_v<T,D>, bool> = true>
    SharedPtr<T>& operator =(SharedPtr<D> tempFromRef){  // same as (const SharedPtr<T>& ref ){ SharedPtr<T> temp(ref);...} need temp value to proper behaviour in d-tor for old data and counter.
        DBG() << "assign";
        swap(tempFromRef);
        return *this;
    }

    template <class D, typename std::enable_if_t<std::is_base_of_v<T,D>, bool> = true>
    SharedPtr<T>& operator =(std::nullptr_t){  // same as (const SharedPtr<T>& ref ){ SharedPtr<T> temp(ref);...} need temp value to proper behaviour in d-tor for old data and counter.
        DBG() << "assign nullptr";
        raw = nullptr;
        *counter--;
        return *this;
    }


    //move c-tor
    template <class D, typename std::enable_if_t<std::is_base_of_v<T,D>, bool> = true>
    explicit SharedPtr(SharedPtr<T>&& ref) noexcept{        //noexcept just for clearance
        DBG() << "move";
        swap(ref);
    }

    //assign move
    template <class D, typename std::enable_if_t<std::is_base_of_v<T,D>, bool> = true>
    SharedPtr<T>& operator =(SharedPtr<T>&& ref) noexcept{
        DBG() << "move assign";
        swap(ref);
        return *this;
    }

    T* get(){return raw;}
    ~SharedPtr(){
        DBG() << "d-tor";
        *counter--;
        if (*counter == 0){
            DBG() << "deleting raw and counter";
            delete raw;
            delete counter;
            raw = nullptr;
        }
    }
private:
    explicit SharedPtr(T* p)
    try                                             // yes if we pass ptr to shared pointer it should manage to delete it, even if we catch an exception
        : raw(p), counter(new std::atomic_int(1)){}
    catch(...){
    delete p;
    throw;
}
T* raw;
std::atomic_int* counter;
};



#endif // WEAK_PTR_H
