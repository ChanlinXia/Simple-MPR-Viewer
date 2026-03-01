#ifndef MPRVIEWER_WIDGET_H
#define MPRVIEWER_WIDGET_H

#include <QWidget>
#include <QDebug>

#include <iostream>
#include <string>
#include <QVTKOpenGLNativeWidget.h>
//#include <any>  // C++17

#include <vtkSmartPointer.h>
#include <vtkDICOMImageReader.h>
#include <vtkImageViewer2.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <vtkMapper.h>
#include <vtkImageMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkImageMapToColors.h>

#include <itkImageSeriesReader.h>
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <QLabel>
#include <QSlider>

#include "../include/customemprviewerinteractor.h"

struct imageFileParam{
    std::string image_type;
    std::string file_path;
    unsigned int rows;
    unsigned int cols;
    unsigned int slices;

    bool is_valid() const{
        return rows>0||cols>0||slices>0;
    }
};

struct MPRViewerConfig{
    // 图像文件参数
    imageFileParam file_param;
    // 控件参数
    bool enable_interact=true;
    bool show_lable=false;
    bool enable_save=false;
    bool enable_copy=false;
    bool enable_MIP=false;

    // 显示参数
    double cur_window;  // 当前窗宽
    double cur_level;   // 当前窗位
    int cur_slice_ind;  // 当前切片索引
    int image_type;     // 0,1,2|横断,冠状,失状
};

class MPRViewerWidget : public QWidget{
    Q_OBJECT

public:
//    explicit MPRViewerWidget(QWidget *parent = nullptr,QSize size=QSize(200,200));
    explicit MPRViewerWidget(QWidget *parent = nullptr,QSize size=QSize(200,200),const MPRViewerConfig& config=MPRViewerConfig{});

    ~MPRViewerWidget();

    void loadSequence(const std::string& path,imageFileParam& fileparam);
//    void loadSequence(const std::string& path,imageFileParam& fileparam);


    void interactorCallBack(InteractorCallbackType type,std::tuple<int> value);

private:
    // CT图像类型定义
    using CTPixelType = short;
    static constexpr unsigned int Dimension = 3;
    using CTImageType = itk::Image<CTPixelType, Dimension>;

    // MRI图像类型定义
    using MRIPixelType = short;
    using MRIImageType = itk::Image<MRIPixelType, Dimension>;

    using ITKImageType = CTImageType;
    // 控件参数
    MPRViewerConfig viewer_param_;

    /* 控件 */
    // VTK控件
    QVTKOpenGLNativeWidget* opengl_widget_;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> opengl_renderwindow_;
    vtkSmartPointer<vtkRenderer> opengl_render_;
    vtkSmartPointer<vtkImageActor> image_actor_;
    vtkSmartPointer<vtkImageMapToColors> image_mapper_;

    vtkSmartPointer<CustomeMPRViewerInteractor> image_interactor_;
//    CustomeMPRViewerInteractor<QVTKOpenGLNativeWidget> image_interactor_;
//    std::vector<vtkSmartPointer<vtkActor>> opengl_actor_list_;

    CTImageType::Pointer original_image_;
    CTImageType::Pointer processed_image_;
    CTImageType::Pointer mpr_image_;


    CTImageType::Pointer coronal_image_;  // 冠状面
    CTImageType::Pointer saggital_image_;  // 矢状面
    CTImageType::Pointer transverse_image_;  // 截断面

    QLabel* imageSizeLabel;
    QLabel* indexLabel;
    QSlider* scrollBar;

    void generateCoronalPlane(const double& y,CTImageType::Pointer& output,const int& sliceNum);
    void generateSagittalPlane(const double& x,CTImageType::Pointer& output,const int& sliceNum);


    void init(QSize size);

    void updateVTKDisplay(/*const std::vector<vtkSmartPointer<vtkRenderer>>& renderers*/);

    void loadImage(const std::string& path); // 读入一个图片文件

    bool convert2DITKToVTK(itk::Image<CTPixelType,2>::Pointer itkimage,vtkImageData* vtkimage);
    bool convertITKToVTK(ITKImageType::Pointer itkImage, vtkImageData* vtkImage);

    void setWindow(double window,double level);

    void loadSlice(unsigned int sliceIndex);
    void onSliderChanged(const int& value);


    enum class logMsgType{info,warning,error};
    void logDebug(const std::string& msg,const logMsgType type){
        std::ostringstream oss;
        std::string msg_type;
        switch (type) {
        case logMsgType::info : msg_type ="[info]";break;
        case logMsgType::warning :msg_type ="[warning]";break;
        case logMsgType::error :msg_type ="[error]";break;
        default:
            break;
        }
        oss<< "[MPRViewerWidget]"<<"["<< this->objectName().toStdString() <<"]"
            <<msg_type<< msg <<std::endl;
        std::cout << oss.str();

        // 可写入指定log文件中
    }

    template<class T>
    void setParam(const std::string& key,const T& value){

        if(typeid(value) == typeid(std::string)){
            logDebug("[key]:"+key+"has been changed to " +value,logMsgType::info);
        }
        else{
            logDebug("[key]:"+key+"has been changed to " +std::string(value),logMsgType::info);
        }
    }

    // 列表显示


};


#endif // MPRVIEWER_H
