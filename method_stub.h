#ifndef METHOD_STUB_H
#define METHOD_STUB_H

#include "method.h"

class MethodStub : public Method {
    int num;
public:
    MethodStub(int n) : num(n) {}
    
    // Изменено: принимает a, b и equation
    QString execute(double a, double b, const QString& equation) override {
        // a и b пока не используются, но они есть в сигнатуре
        return QString("Variant %1 not ready. Equation: %2").arg(num).arg(equation);
    }
};

#endif
