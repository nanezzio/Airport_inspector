#ifndef STATSDIALOG_H
#define STATSDIALOG_H

#include <QDialog>
#include "database.h"

QT_BEGIN_NAMESPACE
namespace Ui { class StatsDialog; }
class QChartView;
QT_END_NAMESPACE

class StatsDialog : public QDialog
{
    Q_OBJECT
public:
    StatsDialog(Database *db,
                const QString &airportCode,
                const QString &airportName,
                QWidget *parent = nullptr);
    ~StatsDialog() override;

private slots:
    void onMonthChanged(int index);

private:
    void rebuildYearChart();
    void rebuildMonthChart(int month);

    Ui::StatsDialog *ui;
    Database    *m_db       = nullptr;
    QString      m_code;
    QString      m_name;
    QChartView  *m_yearView  = nullptr;
    QChartView  *m_monthView = nullptr;
};

#endif