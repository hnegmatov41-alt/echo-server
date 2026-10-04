#include "mytcpserver.h"
#include "database.h"
#include "method_factory.h"
#include <QDebug>
#include <memory> // для unique_ptr

MyTcpServer::MyTcpServer()
{
    Database::getInstance();
    connect(this, &QTcpServer::newConnection, this, &MyTcpServer::newConnection);
    listen(QHostAddress::Any, 33333);
    qDebug() << "Server on 33333";
}

void MyTcpServer::newConnection()
{
    QTcpSocket* s = nextPendingConnection();
    users[s] = "";
    connect(s, &QTcpSocket::readyRead, this, &MyTcpServer::readData);
    connect(s, &QTcpSocket::disconnected, this, &MyTcpServer::disconnect);
    sendTo(s, "Welcome! Commands: /reg user pass group, /login user pass, /history, /stats");
}

void MyTcpServer::readData()
{
    QTcpSocket* s = qobject_cast<QTcpSocket*>(sender());
    if (!s) return;

    QString data = QString::fromUtf8(s->readAll()).trimmed();
    qDebug() << "From" << users[s] << ":" << data;

    // --- Команды ---
    if (data == "/help") {
        QString help = "\r\n COMMANDS \r\n";
        help += "/reg user pass group - register\r\n";
        help += "/login user pass - login\r\n";
        help += "/history - your requests\r\n";
        help += "/stats - global statistics\r\n";
        help += "/help - this help\r\n";
        help += "Task format: variant a b function\r\n";
        help += "Example: 1 1 2 x^3 - x - 2\r\n";
        sendTo(s, help);
        return;
    }

    if (data.startsWith("/reg")) {
        QStringList p = data.split(' ');
        if (p.size() >= 4) {
            if (Database::getInstance()->registerUser(p[1], p[2], p[3]))
                sendTo(s, "Registered");
            else sendTo(s, "Error");
        }
        return;
    }

    if (data.startsWith("/login")) {
        QStringList p = data.split(' ');
        if (p.size() >= 3) {
            if (Database::getInstance()->loginUser(p[1], p[2])) {
                users[s] = p[1];
                sendTo(s, "OK");
            } else sendTo(s, "Fail");
        }
        return;
    }

    if (data == "/history") {
        if (users[s].isEmpty()) sendTo(s, "Login first");
        else sendTo(s, Database::getInstance()->getHistory(users[s]));
        return;
    }

    if (data == "/stats") {
        sendTo(s, Database::getInstance()->getStats());
        return;
    }

    // --- Проверка авторизации ---
    if (users[s].isEmpty()) {
        sendTo(s, "Login first");
        return;
    }

    // --- ПАРСИНГ ЗАДАЧИ (ИСПРАВЛЕНО) ---
    QStringList p = data.split(' ', Qt::SkipEmptyParts);

    // Минимум должно быть 4 части: вариант, a, b и хотя бы один символ уравнения
    if (p.size() < 4) {
        sendTo(s, "Error: format is 'variant a b equation'. Example: 1 1 2 x^3 - x - 2");
        return;
    }

    bool okVariant, okA, okB;
    int variant = p[0].toInt(&okVariant);
    double a = p[1].toDouble(&okA);
    double b = p[2].toDouble(&okB);

    if (!okVariant || !okA || !okB) {
        sendTo(s, "Error: variant, a and b must be numbers");
        return;
    }

    // Уравнение — это всё, что идёт после третьего элемента
    QString equation = p.mid(3).join(' ');

    // Используем unique_ptr для автоматического удаления
    std::unique_ptr<Method> m(MethodFactory::get(variant));
    if (!m) {
        sendTo(s, "Error: unknown variant");
        return;
    }

    // Передаём в метод границы и уравнение
    QString res = m->execute(a, b, equation);

    // Сохраняем запрос в БД (equation вместо inp)
    Database::getInstance()->saveRequest(users[s], variant, equation, res);
    sendTo(s, res);
}

void MyTcpServer::sendTo(QTcpSocket* s, QString msg)
{
    s->write((msg + "\r\n> ").toUtf8());
}

void MyTcpServer::disconnect()
{
    QTcpSocket* s = qobject_cast<QTcpSocket*>(sender());
    if (s) {
        users.remove(s);
        s->deleteLater();
    }
}
