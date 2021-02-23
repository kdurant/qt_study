#ifndef EPOS2_H
#define EPOS2_H
#include <QObject>

class EPOS2 : public QObject
{
    Q_OBJECT
public:
    explicit EPOS2(QObject *parent = 0);

public:
    quint16          CalcFieldCRC(quint16 *pDataArray, quint16 numberofWords);  //CRC-CCITT
    QVector<quint16> WordPlusCRC(QVector<quint16> word);

public:
    QByteArray setDisableState();
    QByteArray clearFault();
    QByteArray setShutdown();
    QByteArray setEnableState();
    QByteArray setHalt();

    QByteArray setPorfileVelocityMode();
    QByteArray setMaximalProfileVelocity(quint16 value);
    QByteArray setQuickstopDeceleration(quint16 value);
    QByteArray setProfileAcceleration(quint16 value);
    QByteArray setProfileDeceleration(quint16 value);
    QByteArray setMaxAcceleration(quint16 value);

    QByteArray setTargetVelocity(quint16 velocity);
    QByteArray getActualVelocity();
    quint32    ReadVelocity(QByteArray array);

    QByteArray setPositionControlWord();
    QByteArray setPorfilePositionMode();
    QByteArray setProfileVelocity(quint16 value);
    QByteArray setTargetPosition(qint32 value);
    QByteArray getActualPosition();
    qint32     ReadPosition(QByteArray array);

    QByteArray setHomeMode();
    QByteArray startHoming();
    QByteArray stopHoming();
    QByteArray setPositiveSpeed();
};

#endif
