#include <QCoreApplication>
#include <sharedptr.h>
#include <QDebug>


template <size_t i>
void test(SharedPtr<int>& shp){
    auto l = shp;
    test<i-1>(l);
}


struct A{
    A(int& l): t(l){
        c++;
    }
    ~A(){c--;}
    int& t;
    static int c;
};
int A::c = 0;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    int t = 3;
    int ll = 8;
    while (t--){
        auto l = SharedPtr<A>::create(ll);
        auto g = l;
    }
    qDebug() << "a c" << A::c;
    return a.exec();
}
