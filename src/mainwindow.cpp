#include "../include/mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDebug>
#include <QFileSystemModel>
#include <QTreeView>
#include <QListView>
#include <QSplitter>
#include <QFileDialog>

#include <vtkSmartPointer.h>
#include <vtkDICOMImageReader.h>
#include <vtkImageViewer2.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <filesystem>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
//    mpr_viewer_widget_(new MPRViewerWidget(this))
{
    ui->setupUi(this);

    this->setWindowTitle("MPR Viewer");

    // 装载样式
    loadStyleSheet(":/style/styles.qss");

    // 控件初始化
//    mpr_viewer_widget_ = new MPRViewerWidget();

//    ui->gridLayout_file_viewer->setSpacing(0);
    ui->gridLayout_viewer->setContentsMargins(0,0,0,0);

    ui->gridLayout_viewer->setColumnStretch(0, 1);
    ui->gridLayout_viewer->setColumnStretch(1, 1);
    ui->gridLayout_viewer->setRowStretch(0, 1);
    ui->gridLayout_viewer->setRowStretch(1, 1);
//    ui->gridLayout_file_viewer->set

    // 控件添加
//    ui->gridLayout_file_viewer->addWidget(mpr_viewer_widget_);
}

MainWindow::~MainWindow()
{
    delete ui;
//    delete [] viewer_list_;
}



void MainWindow::on_scrollArea_customContextMenuRequested(const QPoint &pos)
{

}



/**
 *@brief 通过文件资源管理器读取一个文件夹或文件的path
 */
void MainWindow::on_pushButton_file_dialog_clicked()
{
    qDebug()<<"open the file" << Qt::endl;

    QFileDialog *dialog = new QFileDialog(this);
    dialog->setFileMode(QFileDialog::Directory);
    dialog->setOption(QFileDialog::DontUseNativeDialog, true);  // 不使用系统原生
    dialog->setViewMode(QFileDialog::Detail);

    // 模态显示，等待用户操作
    if (dialog->exec() == QDialog::Accepted) {
        QString selected_dir = dialog->selectedFiles().first();
        qDebug() << "选择的目录:" << selected_dir;

        std::filesystem::path selected_path(selected_dir.toStdString());
        // 检查是否为一个folder/不是folder也可以
//        if(!std::filesystem::is_directory(selected_path)){
//            qDebug() << "请选择一个文件夹目录";
//            return;
//        }
        // 设置控件参数
        MPRViewerConfig widget_param;
        widget_param.file_param.file_path = selected_dir.toStdString();

        imageFileParam file_param;
        for(int i=0;i<3;++i){
    //        auto widget = new MPRViewerWidget(this);
            widget_param.image_type = i; // 设置图片视角类型
            viewer_list_.emplace_back(new MPRViewerWidget(this,{},widget_param));
    //        viewer_list_[i] = widget;
    //        viewer_list_[i]->setsp
            ui->gridLayout_viewer->addWidget(viewer_list_[i],i/2,i%2);
            viewer_list_[i]->loadSequence(selected_dir.toStdString(),file_param);
        }
        std::cout<<file_param.image_type << std::endl;
        std::cout <<"try to push the filepage" << std::endl;

        filePage_list_.emplace_back(new filePage(this,file_param));
        ui->verticalLayout_file_list->insertWidget(0,filePage_list_.back());

        // 连接槽与信号
        connect(filePage_list_.back(), &filePage::close, this, &MainWindow::closeFile);

    } else {
        qDebug() << "用户取消了选择";
    }
}

void MainWindow::closeFile(const std::string& file_path){
    std::cout << "close the file " <<file_path <<std::endl;

//    for(size_t i=0;i<viewer_list_.size();++i){
//        if (!ui->gridLayout_viewer->indexOf()) {

//        }
//        ui->gridLayout_viewer->removeWidget(viewer_list_[i]);
//    }

    for(auto& widget:viewer_list_){
//        if (!ui->gridLayout_viewer->indexOf(widget) == -1) {
//                continue;
//        }
        std::cout << "ready to remove " <<file_path <<std::endl;
        ui->gridLayout_viewer->removeWidget(widget);
//        widget->hide();
//        delete widget;
        widget->setParent(nullptr);
        widget->deleteLater();
        widget = nullptr;
    }
    viewer_list_.clear();
}


