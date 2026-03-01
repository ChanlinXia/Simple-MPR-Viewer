#ifndef FILEPAGE_H
#define FILEPAGE_H

#include <QWidget>
#include <QtWidgets/QPushButton>

#include <qboxlayout.h>

//QT_BEGIN_NAMESPACE
//namespace Ui { class filePage; }
//QT_END_NAMESPACE

#include "../include/MPRViewerWidget.h"

class filePage : public QWidget
{
    Q_OBJECT
public:
    explicit filePage(QWidget *parent = nullptr,const imageFileParam& param={});
    ~filePage();

signals:
    void close(const std::string& file_path);

public slots:


private slots:
//    void onPushButtonFoldClicked();

    void on_pushButton_fold_clicked();

    void on_pushButton_close_clicked();

private:
//    Ui::filePage *ui;
    void hideShowLayout(bool is_show,QLayout* layout);

    QWidget* content_container_;
    QPushButton* collapes_button_;

    QHBoxLayout* content_layout_;
    imageFileParam param_config_;

    bool is_show = true;
};

#endif // FILEPAGE_H
