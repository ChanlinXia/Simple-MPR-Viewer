#include "../include/filePage.h"
#include "./ui_filePage.h"

#include <QDebug>
#include <sstream>

filePage::filePage(QWidget *parent,const imageFileParam& param)
    : QWidget(parent)
//    , ui(new Ui::filePage)
    , param_config_(param)
{
//    ui->setupUi(this);

    // 设置参数
    if(!param_config_.is_valid()){
        qDebug() << "[filepage][error] the param is invalid" <<endl;
        return;
    }

    std::cout << "start to construct filepage" << std::endl;

    MPRViewerConfig widget_param;
    widget_param.enable_interact=false;

    // 1.创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(2);

    // 1.1添加折叠按钮
    collapes_button_ = new QPushButton("-",this);
    mainLayout->addWidget(collapes_button_);

    // 1.2添加内容容器
    content_container_ = new QWidget(this);
    mainLayout->addWidget(content_container_);

    // 2.创建内容布局
    QVBoxLayout* contentLayout =new QVBoxLayout(content_container_);

    // 2.1创建file信息布局
    QHBoxLayout* infoLayout = new QHBoxLayout(content_container_);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(5);
    this->setContentsMargins(0,0,0,0);

    // 2.2创建MPR控件
    MPRViewerWidget* mpr_viewer = new MPRViewerWidget(this,QSize(20,20),widget_param);
    imageFileParam image_param;
    mpr_viewer->loadSequence(param_config_.file_path,image_param);

    infoLayout->addWidget(mpr_viewer);

    // 2.3创建标签标签
    QVBoxLayout* lableLayout = new QVBoxLayout();
    QLabel* typeLable = new QLabel(param_config_.image_type.c_str());
    std::ostringstream ss;
    ss << param_config_.rows << "*" << param_config_.cols << "*" << param_config_.slices;
    QLabel* shapeLable = new QLabel(ss.str().c_str());
    lableLayout->addWidget(typeLable);
    lableLayout->addWidget(shapeLable);
    infoLayout->addLayout(lableLayout);

    // 2.4添加info布局
    contentLayout->addLayout(infoLayout);

    // 2.5添加按钮
    QPushButton* closedButton = new QPushButton("close",this);
    contentLayout->addWidget(closedButton);
//    QPushButton* collapseButton = new QPushButton("-",this);

//    mainLayout->addWidget(content_container_);
//    mainLayout->addLayout(infoLayout);
//    mainLayout->addWidget(closedButton);

//    content_container_->show();

    connect(collapes_button_, &QPushButton::clicked, this, &filePage::on_pushButton_fold_clicked);
    connect(closedButton, &QPushButton::clicked, this,&filePage::on_pushButton_close_clicked);


//    internalLayout->addWidget()

//    ui->label_imagetype_val->setText(QString::fromStdString(param_config_.image_type));
//    ui->label_path->setText(QString::fromStdString(param_config_.file_path));
//    std::ostringstream oss;
//    oss << param_config_.rows << "x" << param_config_.cols << "x" << param_config_.slices;
//    ui->label_size_val->setText(QString::fromStdString(oss.str()));

    // 设置参数显示
    std::cout << "---- filepage init----"<< std::endl;
     std::cout << "type:" <<param_config_.image_type << std::endl;
     std::cout << "file_path:" <<param_config_.file_path << std::endl;
     std::cout << "file_path:" <<ss.str() << std::endl;
}

filePage::~filePage()
{
//    delete ui;
}

void filePage::hideShowLayout(bool is_show,QLayout* layout){
    if(is_show){
        // 除第一个控件（折叠按钮外）全部隐藏
        for(int i=1;i<layout->count();++i){
            QLayoutItem* item = layout->itemAt(i);
            if(item->widget()) item->widget()->hide();
            else if(item->layout()) hideShowLayout(is_show,item->layout());
        }
    }
    else{
        // 除第一个控件（折叠按钮外）全部显示
        for(int i=1;i<layout->count();++i){
            QLayoutItem* item = layout->itemAt(i);
            if(item->widget()) item->widget()->show();
            else if(item->layout()) hideShowLayout(is_show,item->layout());
        }
    }
}

void filePage::on_pushButton_fold_clicked()
{
//    static bool is_show=true;
    std::cout << "fold the filepage" <<std::endl;

    is_show = !is_show; // 取反
    if(is_show) content_container_->show();
    else content_container_->hide();
//    hideShowLayout(is_show,content_layout_);
}


void filePage::on_pushButton_close_clicked()
{
    std::cout << "close the filepage" <<std::endl;
    emit close(param_config_.file_path);
    this->deleteLater();
}



