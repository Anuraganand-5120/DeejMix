#include <QVariant>
#include <QString>
#include <iostream>

int main() {
    QVariant v1("true");
    QVariant v2("false");
    
    std::cout << "\"true\" toBool: " << v1.toBool() << std::endl;
    std::cout << "\"false\" toBool: " << v2.toBool() << std::endl;
    return 0;
}
