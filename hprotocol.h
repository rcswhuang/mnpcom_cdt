#ifndef HPROTOCOL_H
#define HPROTOCOL_H

#include <QObject>

class HProtocol : public QObject
{
    Q_OBJECT
public:
    explicit HProtocol(QObject *parent = 0);

public:
    void sendAllYx();
    void sendAllYc();
    void sendChangeYx();
    void processFrame(const QByteArray& data);

signals:
    void sendData(const QByteArray& data);
public slots:
};

#endif // HPROTOCOL_H
