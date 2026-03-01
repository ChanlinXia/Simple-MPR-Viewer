#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile.h>
#include <QDebug>
#include "../include/MPRViewerWidget.h"
#include "../include/filePage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
//    void on_pushButton_clicked();

    void on_scrollArea_customContextMenuRequested(const QPoint &pos);

    void on_pushButton_file_dialog_clicked();

    void closeFile(const std::string& file_path);


private:
    Ui::MainWindow *ui;

    std::vector<MPRViewerWidget*> viewer_list_;
    std::vector<filePage*> filePage_list_;
//    MPRViewerWidget* mpr_viewer_widget_;

    void loadStyleSheet(const QString& qss_path){
        QFile qss_file(qss_path);

        if(qss_file.open(QFile::ReadOnly|QFile::Text)){
            QString styleSheet = QLatin1String(qss_file.readAll());
            this->setStyleSheet(styleSheet);  // 应用到整个应用程序
            qss_file.close();
        } else {
            qDebug() << "无法加载QSS文件:" << qss_file.errorString();
        }
    }
};
#endif // MAINWINDOW_H
