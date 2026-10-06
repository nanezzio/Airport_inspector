#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

Database::Database(QObject *parent) : QObject(parent) {}

Database::~Database()
{
    if (m_db.isOpen()) m_db.close();
}

bool Database::connect()
{
    if (QSqlDatabase::contains("airport_connection")) {
        m_db = QSqlDatabase::database("airport_connection");
        if (m_db.isOpen()) {
            m_connected = true;
            return true;
        }
    } else {
        m_db = QSqlDatabase::addDatabase("QPSQL", "airport_connection");
        m_db.setHostName("981757-ca08998.tmweb.ru");
        m_db.setPort(5432);
        m_db.setDatabaseName("demo");
        m_db.setUserName("netology_usr_cpp");
        m_db.setPassword("CppNeto3");
    }
    if (!m_db.open()) {
        m_lastError = m_db.lastError().text();
        m_connected = false;
        return false;
    }
    m_connected = true;
    return true;
}

QVector<Airport> Database::getAirports()
{
    QVector<Airport> list;
    if (!m_connected) return list;

    QSqlQuery q(m_db);
    q.prepare("SELECT airport_name->>'ru' AS \"airportName\", airport_code "
              "FROM bookings.airports_data ORDER BY \"airportName\"");
    if (q.exec()) {
        while (q.next()) {
            Airport a;
            a.name = q.value(0).toString();
            a.code = q.value(1).toString();
            list.append(a);
        }
    } else {
        m_lastError = q.lastError().text();
    }
    return list;
}

QVector<FlightInfo> Database::getArrivals(const QString &airportCode, const QDate &date)
{
    QVector<FlightInfo> list;
    if (!m_connected) return list;

    QSqlQuery q(m_db);
    q.prepare("SELECT flight_no, scheduled_arrival, ad.airport_name->>'ru' AS \"Name\" "
              "FROM bookings.flights f "
              "JOIN bookings.airports_data ad ON ad.airport_code = f.departure_airport "
              "WHERE f.arrival_airport = :code "
              "AND f.scheduled_arrival::date = :date "
              "ORDER BY scheduled_arrival");
    q.bindValue(":code", airportCode);
    q.bindValue(":date", date);
    if (q.exec()) {
        while (q.next()) {
            FlightInfo f;
            f.flightNo    = q.value(0).toString().trimmed();
            f.time        = q.value(1).toDateTime();
            f.airportName = q.value(2).toString();
            list.append(f);
        }
    } else {
        m_lastError = q.lastError().text();
    }
    return list;
}

QVector<FlightInfo> Database::getDepartures(const QString &airportCode, const QDate &date)
{
    QVector<FlightInfo> list;
    if (!m_connected) return list;

    QSqlQuery q(m_db);
    q.prepare("SELECT flight_no, scheduled_departure, ad.airport_name->>'ru' AS \"Name\" "
              "FROM bookings.flights f "
              "JOIN bookings.airports_data ad ON ad.airport_code = f.arrival_airport "
              "WHERE f.departure_airport = :code "
              "AND f.scheduled_departure::date = :date "
              "ORDER BY scheduled_departure");
    q.bindValue(":code", airportCode);
    q.bindValue(":date", date);
    if (q.exec()) {
        while (q.next()) {
            FlightInfo f;
            f.flightNo    = q.value(0).toString().trimmed();
            f.time        = q.value(1).toDateTime();
            f.airportName = q.value(2).toString();
            list.append(f);
        }
    } else {
        m_lastError = q.lastError().text();
    }
    return list;
}

QVector<StatPoint> Database::getYearStats(const QString &airportCode)
{
    QVector<StatPoint> res;
    for (int i = 1; i <= 12; ++i) {
        StatPoint p; p.index = i; res.append(p);
    }
    if (!m_connected) return res;

    QSqlQuery q(m_db);
    q.prepare("SELECT count(flight_no) AS cnt, "
              "EXTRACT(MONTH FROM scheduled_departure)::int AS m "
              "FROM bookings.flights f "
              "WHERE scheduled_departure::date >  DATE '2016-08-31' "
              "  AND scheduled_departure::date <= DATE '2017-08-31' "
              "  AND (departure_airport = :code OR arrival_airport = :code) "
              "GROUP BY m ORDER BY m");
    q.bindValue(":code", airportCode);
    if (q.exec()) {
        while (q.next()) {
            const int cnt = q.value(0).toInt();
            const int m   = q.value(1).toInt();
            if (m >= 1 && m <= 12) res[m - 1].count = cnt;
        }
    } else {
        m_lastError = q.lastError().text();
    }
    return res;
}

QVector<StatPoint> Database::getMonthStats(const QString &airportCode, int month)
{
    QVector<StatPoint> res;
    for (int i = 1; i <= 31; ++i) {
        StatPoint p; p.index = i; res.append(p);
    }
    if (!m_connected) return res;

    QSqlQuery q(m_db);
    q.prepare("SELECT count(flight_no) AS cnt, "
              "EXTRACT(DAY FROM scheduled_departure)::int AS d "
              "FROM bookings.flights f "
              "WHERE scheduled_departure::date >  DATE '2016-08-31' "
              "  AND scheduled_departure::date <= DATE '2017-08-31' "
              "  AND (departure_airport = :code OR arrival_airport = :code) "
              "  AND EXTRACT(MONTH FROM scheduled_departure)::int = :month "
              "GROUP BY d ORDER BY d");
    q.bindValue(":code", airportCode);
    q.bindValue(":month", month);
    if (q.exec()) {
        while (q.next()) {
            const int cnt = q.value(0).toInt();
            const int d   = q.value(1).toInt();
            if (d >= 1 && d <= 31) res[d - 1].count = cnt;
        }
    } else {
        m_lastError = q.lastError().text();
    }
    return res;
}