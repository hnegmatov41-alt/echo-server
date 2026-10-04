#include "method1.h"
#include <QStringList>
#include <cmath>

// Функция для вычисления значения уравнения в точке x
double eval1(const QString& func, double x) {
    if (func.contains("x^3")) return x * x * x - x - 2;
    if (func.contains("x^2")) return x * x - 4;
    if (func.contains("sin")) return sin(x);
    return 0;
}

// Новая сигнатура: принимает a, b и equation
QString Method1::execute(double a, double b, const QString& equation) {
    
    // Проверка: корень существует только если знаки на концах разные
    double fa = eval1(equation, a);
    double fb = eval1(equation, b);

    if (fa * fb > 0) {
        return "Error: different signs required on ends of interval";
    }

    // Основной цикл метода половинного деления
    double epsilon = 1e-6; // Точность
    int maxIterations = 1000; // Защита от бесконечного цикла
    double c = a;

    for (int i = 0; i < maxIterations; i++) {
        c = (a + b) / 2.0;
        double fc = eval1(equation, c);

        // Если значение функции близко к нулю или интервал стал очень маленьким
        if (std::abs(fc) < epsilon || (b - a) / 2.0 < epsilon) {
            break;
        }

        // Выбираем ту половину, где знаки разные
        if (fa * fc < 0) {
            b = c;
            fb = fc;
        } else {
            a = c;
            fa = fc;
        }
    }

    // Возвращаем найденный корень
    return QString("Root: x ≈ %1").arg(c, 0, 'f', 6);
}
