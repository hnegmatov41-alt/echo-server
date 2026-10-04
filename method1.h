#ifndef METHOD1_H
#define METHOD1_H

#include "method.h"
#include <cmath>

class Method1 : public Method {
public:
    // Изменено: теперь принимает границы и уравнение
    QString execute(double a, double b, const QString& equation) override;
};

#endif
