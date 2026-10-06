#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "statsdialog.h"

#include <QMessageBox>
#include <QTimer>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QStatusBar>
#include <QButtonGroup>
#include <QAction>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_db = new Database(this);

    auto *group = new QButtonGroup(this);
    group->addButton(ui->departureRadio);
    group->addButton(ui->arrivalRadio);
    group->setExclusive(true);

    ui->dateEdit->setDateRange(QDate(2016, 8, 15), QDate(2017, 9, 14));
    ui->dateEdit->setDate(QDate(2016, 9, 1));

    ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table->verticalHeader()->setVisible(false);
    ui->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table->setAlternatingRowColors(true);

    ui->searchButton->setEnabled(false);
    ui->statsButton->setEnabled(false);

    m_statusLabel = new QLabel("Отключено", this);
    m_statusLabel->setStyleSheet("color:#b00020; font-weight:bold;");
    statusBar()->addPermanentWidget(m_statusLabel);
    statusBar()->showMessage("Готово");

    connect(ui->actionExit,  &QAction::triggered, this, &QWidget::close);
    connect(ui->actionAbout, &QAction::triggered, this, [this]{
        QMessageBox::information(this, "О программе",
                                 "Инспектор аэропортов 1.0\nMVP-версия для Росавиации.\n"
                                 "Просмотр расписания и загруженности аэропортов.");
    });

    connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::onSearchClicked);
    connect(ui->statsButton,  &QPushButton::clicked, this, &MainWindow::onStatsClicked);

    QTimer::singleShot(0, this, &MainWindow::tryConnect);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::tryConnect()
{
    if (m_db->connect()) {
        m_connected = true;
        updateStatusLabel(true);
        loadAirports();
        return;
    }

    m_connected = false;
    updateStatusLabel(false);

    QMessageBox::critical(this, "Ошибка подключения",
                          "Не удалось подключиться к БД:\n" + m_db->lastError());

    QTimer::singleShot(5000, this, &MainWindow::tryConnect);
}

void MainWindow::updateStatusLabel(bool ok)
{
    if (ok) {
        m_statusLabel->setText("Подключено");
        m_statusLabel->setStyleSheet("color:#0a7a2f; font-weight:bold;");
    } else {
        m_statusLabel->setText("Отключено");
        m_statusLabel->setStyleSheet("color:#b00020; font-weight:bold;");
    }
}

void MainWindow::loadAirports()
{
    const auto airports = m_db->getAirports();
    ui->airportCombo->clear();
    for (const auto &a : airports)
        ui->airportCombo->addItem(QString("%1 (%2)").arg(a.name, a.code), a.code);

    const bool has = !airports.isEmpty();
    ui->searchButton->setEnabled(has);
    ui->statsButton->setEnabled(has);
    statusBar()->showMessage(QString("Загружено аэропортов: %1").arg(airports.size()), 4000);
}

void MainWindow::onSearchClicked()
{
    if (!m_connected) {
        QMessageBox::warning(this, "Нет подключения", "Подключение к БД отсутствует.");
        return;
    }
    const QString code = ui->airportCombo->currentData().toString();
    const QDate   date = ui->dateEdit->date();
    if (code.isEmpty()) return;

    const bool arrival = ui->arrivalRadio->isChecked();
    const auto rows = arrival ? m_db->getArrivals(code, date)
                              : m_db->getDepartures(code, date);
    fillTable(rows, arrival);
    statusBar()->showMessage(QString("Найдено рейсов: %1").arg(rows.size()), 4000);
}

void MainWindow::fillTable(const QVector<FlightInfo> &rows, bool arrival)
{
    ui->table->setRowCount(rows.size());
    ui->table->setHorizontalHeaderLabels({
        "Номер рейса",
        arrival ? "Время прилёта" : "Время вылета",
        arrival ? "Аэропорт отправления" : "Аэропорт назначения"
    });
    for (int i = 0; i < rows.size(); ++i) {
        const auto &f = rows[i];
        ui->table->setItem(i, 0, new QTableWidgetItem(f.flightNo));
        ui->table->setItem(i, 1, new QTableWidgetItem(f.time.toString("hh:mm dd.MM.yyyy")));
        ui->table->setItem(i, 2, new QTableWidgetItem(f.airportName));
    }
}

void MainWindow::onStatsClicked()
{
    if (!m_connected) return;
    const QString code = ui->airportCombo->currentData().toString();
    const QString name = ui->airportCombo->currentText();
    if (code.isEmpty()) return;

    StatsDialog dlg(m_db, code, name, this);
    dlg.exec();
}