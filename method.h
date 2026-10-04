#ifndef METHOD_H
#define METHOD_H

#include <QString>

class Method {
public:
    virtual ~Method() {}
    // Изменено: теперь принимает границы и уравнение отдельно
    virtual QString execute(double a, double b, const QString& equation) = 0;
};

#endif
