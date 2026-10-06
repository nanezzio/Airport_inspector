#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "database.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QLabel;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void tryConnect();
    void onSearchClicked();
    void onStatsClicked();

private:
    void updateStatusLabel(bool ok);
    void loadAirports();
    void fillTable(const QVector<FlightInfo> &rows, bool arrival);

    Ui::MainWindow *ui;
    Database       *m_db          = nullptr;
    QLabel         *m_statusLabel = nullptr;
    bool            m_connected   = false;
};

#endif