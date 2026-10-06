#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QVector>
#include <QDate>
#include <QDateTime>
#include <QString>

struct Airport {
    QString name;
    QString code;
};

struct FlightInfo {
    QString flightNo;
    QDateTime time;
    QString airportName;
};

struct StatPoint {
    int index = 0;
    int count = 0;
};

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);
    ~Database() override;

    bool connect();
    bool isConnected() const { return m_connected; }
    QString lastError() const { return m_lastError; }

    QVector<Airport>    getAirports();
    QVector<FlightInfo> getArrivals(const QString &airportCode, const QDate &date);
    QVector<FlightInfo> getDepartures(const QString &airportCode, const QDate &date);
    QVector<StatPoint>  getYearStats(const QString &airportCode);
    QVector<StatPoint>  getMonthStats(const QString &airportCode, int month);

private:
    QSqlDatabase m_db;
    QString      m_lastError;
    bool         m_connected = false;
};

#endif