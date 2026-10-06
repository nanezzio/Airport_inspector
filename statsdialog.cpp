#include "statsdialog.h"
#include "ui_statsdialog.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

StatsDialog::StatsDialog(Database *db,
                         const QString &airportCode,
                         const QString &airportName,
                         QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StatsDialog)
    , m_db(db)
    , m_code(airportCode)
    , m_name(airportName)
{
    ui->setupUi(this);

    ui->headerLabel->setText(QString("Аэропорт: %1 (%2)").arg(m_name, m_code));

    m_yearView = new QChartView(this);
    m_yearView->setRenderHint(QPainter::Antialiasing);
    auto *yearLayout = new QVBoxLayout(ui->yearChartPlaceholder);
    yearLayout->setContentsMargins(0, 0, 0, 0);
    yearLayout->addWidget(m_yearView);

    m_monthView = new QChartView(this);
    m_monthView->setRenderHint(QPainter::Antialiasing);
    auto *monthLayout = new QVBoxLayout(ui->monthChartPlaceholder);
    monthLayout->setContentsMargins(0, 0, 0, 0);
    monthLayout->addWidget(m_monthView);

    ui->monthCombo->addItems({"Январь","Февраль","Март","Апрель","Май","Июнь",
                              "Июль","Август","Сентябрь","Октябрь","Ноябрь","Декабрь"});

    connect(ui->monthCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StatsDialog::onMonthChanged);
    connect(ui->closeButton, &QPushButton::clicked, this, &QDialog::accept);

    rebuildYearChart();
    rebuildMonthChart(1);
}

StatsDialog::~StatsDialog()
{
    delete ui;
}

void StatsDialog::onMonthChanged(int index)
{
    rebuildMonthChart(index + 1);
}

void StatsDialog::rebuildYearChart()
{
    const auto data = m_db->getYearStats(m_code);

    static const QStringList monthNames = {
        "Янв","Фев","Мар","Апр","Май","Июн",
        "Июл","Авг","Сен","Окт","Ноя","Дек"
    };

    auto *set = new QBarSet("Рейсы");
    QStringList categories;
    int maxVal = 0;
    for (const auto &p : data) {
        *set << p.count;
        categories << monthNames.value(p.index - 1, QString::number(p.index));
        maxVal = qMax(maxVal, p.count);
    }

    auto *series = new QBarSeries();
    series->append(set);

    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(QString("Суммарное количество рейсов по месяцам — %1").arg(m_name));
    chart->setAnimationOptions(QChart::SeriesAnimations);

    auto *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    axisY->setTitleText("Количество рейсов");
    axisY->setRange(0, maxVal * 1.1 + 1);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChart *old = m_yearView->chart();
    m_yearView->setChart(chart);
    delete old;
}

void StatsDialog::rebuildMonthChart(int month)
{
    const auto data = m_db->getMonthStats(m_code, month);

    auto *series = new QLineSeries();
    series->setName("Рейсов в день");

    int maxVal = 0;
    for (const auto &p : data) {
        series->append(p.index, p.count);
        maxVal = qMax(maxVal, p.count);
    }

    static const QStringList monthTitles = {
        "январь","февраль","март","апрель","май","июнь",
        "июль","август","сентябрь","октябрь","ноябрь","декабрь"
    };

    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(QString("Загруженность аэропорта %1 — %2")
                        .arg(m_name, monthTitles.value(month - 1)));
    chart->setAnimationOptions(QChart::SeriesAnimations);

    auto *axisX = new QValueAxis();
    axisX->setTitleText("День месяца");
    axisX->setLabelFormat("%d");
    axisX->setRange(1, 31);
    axisX->setTickCount(7);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    axisY->setTitleText("Количество рейсов");
    axisY->setRange(0, maxVal * 1.1 + 1);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChart *old = m_monthView->chart();
    m_monthView->setChart(chart);
    delete old;
}