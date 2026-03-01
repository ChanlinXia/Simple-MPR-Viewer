#include "../include/MPRViewerWidget.h"

// ITK具体头文件
#include <itkImage.h>
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkImageSeriesReader.h>
#include "../include/itkImageToVTKImageFilter.h"
#include <itkRescaleIntensityImageFilter.h>
#include <itkMedianImageFilter.h>
#include <itkThresholdImageFilter.h>
#include <itkExtractImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkFlipImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkBSplineInterpolateImageFunction.h>
#include <itkMirrorPadImageFilter.h>

// VTK具体头文件
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <vtkImageActor.h>
#include <vtkImageMapper.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkCornerAnnotation.h>

#include <QSize>
#include <QGridLayout>
#include <QSlider>
#include <QLabel>
MPRViewerWidget::MPRViewerWidget(QWidget *parent,QSize size,const MPRViewerConfig& config):
    QWidget(parent),
    viewer_param_(config),
    opengl_widget_(new QVTKOpenGLNativeWidget(this))
//    opengl_renderwindow_(new vtkGenericOpenGLRenderWindow())
{
    init(size);
}


MPRViewerWidget::~MPRViewerWidget(){
//    delete opengl_widget_;
//    original_image_->Delete();

//    delete custome_interactor;
//    delete original_image_;
//    delete opengl_renderwindow_;
}

void MPRViewerWidget::init(QSize size){
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    QPalette pal = this->palette();
    pal.setColor(QPalette::Window, Qt::black);  // 设置背景色
    this->setAutoFillBackground(true);  // 关键！必须开启
    this->setPalette(pal);

    // 布局
    QHBoxLayout* internalLayout = new QHBoxLayout(this);
    internalLayout->setContentsMargins(0, 0, 0, 0);
    internalLayout->setSpacing(0);

    this->setContentsMargins(0,0,0,0);
//    opengl_widget_->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    internalLayout->addWidget(opengl_widget_);
    if(viewer_param_.enable_interact){


        // toolbar布局
        QVBoxLayout* toolBarLayout = new QVBoxLayout();
        toolBarLayout->setContentsMargins(0, 0, 0, 0);
        toolBarLayout->setSpacing(0);
        // 文本布局
        QVBoxLayout* labelLayout = new QVBoxLayout();
        labelLayout->setContentsMargins(0, 0, 0, 0);
        labelLayout->setSpacing(0);

        imageSizeLabel = new QLabel("2x2",this);
        imageSizeLabel->setFont(QFont("Arial", 10));
        imageSizeLabel->setStyleSheet("color: white;");

        indexLabel = new QLabel("2/12");
        indexLabel->setFont(QFont("Arial", 10));
        indexLabel->setStyleSheet("color: white;");

        labelLayout-> addWidget(imageSizeLabel);
        labelLayout-> addWidget(indexLabel);

        internalLayout ->addLayout(toolBarLayout);
        internalLayout ->addLayout(labelLayout);

        // 添加滚动条
        scrollBar = new QSlider(this);
        internalLayout->addWidget(scrollBar);

        connect(scrollBar, &QSlider::valueChanged, this, &MPRViewerWidget::onSliderChanged);
    }



//    internalLayout->setRowStretch(0, 1);
//    internalLayout->setColumnStretch(0, 1);

    opengl_widget_->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    opengl_widget_->setMinimumSize(size);
    opengl_widget_->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    // 连接pipeline
    opengl_renderwindow_ = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    opengl_render_ = vtkSmartPointer<vtkRenderer>::New();
    image_actor_ = vtkSmartPointer<vtkImageActor>::New();
    image_mapper_ = vtkSmartPointer<vtkImageMapToColors >::New();

    opengl_widget_->setRenderWindow(opengl_renderwindow_.Get());
    opengl_renderwindow_->AddRenderer(opengl_render_);
    opengl_render_->AddActor(image_actor_);

    opengl_renderwindow_->Finalize();
    original_image_ = ITKImageType::New();

    // 设置交互样式
    image_interactor_ = vtkSmartPointer<CustomeMPRViewerInteractor>::New();
    opengl_renderwindow_->GetInteractor()->SetInteractorStyle(image_interactor_);
    image_interactor_->setWindow(this);
    // 初始窗位
    viewer_param_.cur_window = 350;
    viewer_param_.cur_level = 40;

    setLayout(internalLayout);
//    updateCanvas();
}


void MPRViewerWidget::updateVTKDisplay(){

    if(!opengl_renderwindow_||!opengl_render_){
        return;
    }

    opengl_render_->ResetCamera();
    opengl_renderwindow_->Render();
}

void MPRViewerWidget::interactorCallBack(InteractorCallbackType type,std::tuple<int> value){
    if(!viewer_param_.enable_interact){ // 禁用交互时不处理交互
        return;
    }

    // 进主线程处理
    QMetaObject::invokeMethod(this,[=](){
        switch(type){
            case InteractorCallbackType::MouseON:
            break;
        case InteractorCallbackType::MouseOff:
            break;
        case InteractorCallbackType::MouseWheelForward:
            this->viewer_param_.cur_slice_ind++;    // 往下一层
            if(this->viewer_param_.cur_slice_ind < this->viewer_param_.file_param.slices)
                    loadSlice(this->viewer_param_.cur_slice_ind);
            else this->viewer_param_.cur_slice_ind = this->viewer_param_.file_param.slices -1;
            break;
        case InteractorCallbackType::MouseWheelBackword:
            this->viewer_param_.cur_slice_ind--; // 往上一层
            if(this->viewer_param_.cur_slice_ind >= 0){
                loadSlice(this->viewer_param_.cur_slice_ind);
            }
            else{
                this->viewer_param_.cur_slice_ind =0;
            }
            break;
        case InteractorCallbackType::OnMouseMove:
            viewer_param_.cur_level -= std::get<0>(value) * 2; // 调整窗宽
            loadSlice(viewer_param_.cur_slice_ind); // 重新载入
            break;
        case InteractorCallbackType::SelectPoint:
            break;
        case InteractorCallbackType::ChangeWindow:
            viewer_param_.cur_level -= std::get<0>(value) * 2; // 调整窗位
            loadSlice(viewer_param_.cur_slice_ind);
        default:
            break;
        }
    },Qt::QueuedConnection); // 连接队列
}

void MPRViewerWidget::setWindow(double window,double level){
    viewer_param_.cur_window = window;
    viewer_param_.cur_level = level;
}

void MPRViewerWidget::onSliderChanged(const int& value){


    if(viewer_param_.cur_slice_ind != value) loadSlice(value);
}


//void MPRViewerWidget::loadSequence(const std::string& path,imageFileParam& fileparam);


void MPRViewerWidget::loadSequence(const std::string& path,imageFileParam& file_param){
    try { // 读入数据
        // 1.创建DICOM IO对象
        using ImageIOType = itk::GDCMImageIO;
        ImageIOType::Pointer dicomIO = ImageIOType::New();

        // 2.获取DICOM系列文件名
        using NamesGeneratorType = itk::GDCMSeriesFileNames;
        NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
        nameGenerator->SetUseSeriesDetails(true);
        nameGenerator->SetDirectory(path);

        //
        using SeriesIdContainer = std::vector<std::string>;
        const SeriesIdContainer& seriesUID = nameGenerator->GetSeriesUIDs();
        if(seriesUID.empty()){
            return;
        }

        // 读取第一个系列
        std::string seriesIdentifier = seriesUID[0];
        std::cout << "Reading series: " << seriesIdentifier << std::endl;

        using FileNamesContainer = std::vector<std::string>;
        FileNamesContainer fileNames = nameGenerator->GetFileNames(seriesIdentifier);

        // 先尝试按CT读取
        auto reader = itk::ImageSeriesReader<CTImageType>::New();
        reader->SetImageIO(dicomIO);
        reader->SetFileNames(fileNames);
        reader->Update();
        original_image_ = reader->GetOutput();

        if(viewer_param_.image_type == 1){ // 冠状
            double y = original_image_->GetOrigin()[1] + 120 *  original_image_->GetSpacing()[1];
            generateCoronalPlane(y,mpr_image_,200);

//            index = 0;
        }
        else if(viewer_param_.image_type == 2){ // 失状
            double x = original_image_->GetOrigin()[1] + 120 *  original_image_->GetSpacing()[1];
            generateSagittalPlane(x,mpr_image_,200);
        }
        else {  // 截断
            mpr_image_ = original_image_;
        }

        CTImageType::RegionType region = mpr_image_->GetLargestPossibleRegion();
        CTImageType::SizeType size = region.GetSize();
        CTImageType::SpacingType spacing = mpr_image_->GetSpacing();
        CTImageType::PointType origin = mpr_image_->GetOrigin();


        std::cout << "\n=== DICOM Image Information ===" << std::endl;
        std::cout << "Dimensions: " << size[0] << " x "
                  << size[1] << " x " << size[2] << std::endl;
        std::cout << "Spacing: " << spacing[0] << ", "
                  << spacing[1] << ", " << spacing[2] << " mm" << std::endl;
        std::cout << "Origin: " << origin[0] << ", "
                  << origin[1] << ", " << origin[2] << std::endl;


        viewer_param_.file_param.image_type = "CT";
        viewer_param_.file_param.file_path = path;
        viewer_param_.file_param.rows = size[0];
        viewer_param_.file_param.cols = size[1];
        viewer_param_.file_param.slices = size[2];

        loadSlice(0);
        viewer_param_.cur_slice_ind = 0;



        std::cout <<"start to copy" << std::endl;
        std::copy_n(&viewer_param_.file_param,sizeof(imageFileParam),&file_param);

//        printImageInfo()

//        using ImageType = decltype(reader->GetOutput())::ObjectType;

        // 步骤3: 创建读取器
//       using ReaderType = itk::ImageSeriesReader<ITKImageType>;
//       ReaderType::Pointer reader = ReaderType::New();
//       using ImageType = decltype(reader->GetOutput())::ObjectType;
//       reader->SetImageIO(dicomIO);
//       reader->SetFileNames(fileNames);

//       std::cout << "Reading DICOM files..." << std::endl;
//       reader->Update();
//       original_image_ = reader->GetOutput();

    } catch (...) {
    }
}


void MPRViewerWidget::loadImage(const std::string& path){
//        vtkSmartPointer<vtkMPRImageReader> MPR_reader = vtkSmartPointer<vtkMPRImageReader>::New();


}

bool MPRViewerWidget::convert2DITKToVTK(const itk::Image<CTPixelType,2>::Pointer  itkImage,vtkImageData* vtkImage){
    try {
        using  ConnectorType = itk::ImageToVTKImageFilter<itk::Image<CTPixelType, 2>>;
        ConnectorType::Pointer connector = ConnectorType::New();
        connector->SetInput(itkImage);
        connector->Update();

        vtkImage->ShallowCopy(connector->GetOutput());
        return true;

    } catch (itk::ExceptionObject& ex) {
        std::cerr << "Error converting 2D ITK to VTK: " << ex << std::endl;
                return false;
    }
}

bool MPRViewerWidget::convertITKToVTK(ITKImageType::Pointer itkImage, vtkImageData* vtkImage) {
    try {
        // 使用ITK到VTK的转换过滤器
        using ConnectorType = itk::ImageToVTKImageFilter<ITKImageType>;
        ConnectorType::Pointer connector = ConnectorType::New();
        connector->SetInput(itkImage);
        connector->Update();

        // 获取VTK图像数据
        vtkImage->ShallowCopy(connector->GetOutput());
        return true;

    } catch (itk::ExceptionObject& ex) {
        std::cerr << "Error converting ITK to VTK: " << ex << std::endl;
        return false;
    }
}

void MPRViewerWidget::loadSlice(unsigned int sliceIndex){
//    if (!processed_image_) {
//        std::cerr << "No image loaded!" << std::endl;
//        return;
//    }

    if (sliceIndex >= viewer_param_.file_param.slices) {
        std::cerr << "Slice index out of range!" << std::endl;
        return;
    }

    if (!mpr_image_) {
            std::cerr << "CRITICAL ERROR: mpr_image_ is nullptr!" << std::endl;
            std::cerr << "This means loadSequence() failed or image was deleted." << std::endl;
            return;
    }

    try {
        std::cout << "Extracting slice: " << sliceIndex << std::endl;

//        int x_size=0,y_size=0,z_size=0;
        unsigned int index = sliceIndex;

        // 定义2D图像类型
        using Image2DType = itk::Image<CTPixelType, 2>;

        // 设置提取区域
        CTImageType::RegionType inputRegion = mpr_image_->GetLargestPossibleRegion();
        CTImageType::SizeType size = inputRegion.GetSize();
        CTImageType::IndexType start = inputRegion.GetIndex();

        if(viewer_param_.enable_interact){
            std::ostringstream ss;
            ss << size[0] << " x " << size[1] ;
//            QString size(ss.str().c_str());
            imageSizeLabel->setText(ss.str().c_str());

            ss.str("");
            ss.clear();
            ss << sliceIndex+1 << "/" << viewer_param_.file_param.slices ;
            indexLabel->setText(ss.str().c_str());
            scrollBar->setMinimum(1);
            scrollBar->setMaximum(viewer_param_.file_param.slices);
            if(scrollBar->value() != sliceIndex) scrollBar->setValue(sliceIndex);
//            scrollBar->setValue(sliceIndex+1);
        }


        std::cout << "Image size: " << size[0] << " x " << size[1] << " x " << size[2] << std::endl;
        std::cout << "Requested slice: " << index << std::endl;
        if (sliceIndex < 0 || index >= size[2]) {
            std::cerr << "ERROR: Slice index " << sliceIndex
                      << " is out of range [0, " << size[2]-1 << "]" << std::endl;
            return;
        }
        std::cout << "extract"<< std::endl;

        size[2] = 0;  // 在Z方向只取一层
        start[2] = sliceIndex;

        CTImageType::RegionType desiredRegion;
        desiredRegion.SetSize(0, size[0]);   // X方向保持原大小
        desiredRegion.SetSize(1, size[1]);   // Y方向保持原大小
        desiredRegion.SetSize(2, 0);         // Z方向大小为0（只取一层）

        desiredRegion.SetIndex(0, 0);        // X起始索引
        desiredRegion.SetIndex(1, 0);        // Y起始索引
        desiredRegion.SetIndex(2, index); // Z起始索引

        // 使用ExtractImageFilter提取2D切片
        using ExtractFilterType = itk::ExtractImageFilter<CTImageType, Image2DType>;
        ExtractFilterType::Pointer extractFilter = ExtractFilterType::New();

        extractFilter->SetExtractionRegion(desiredRegion);
        extractFilter->SetInput(mpr_image_);
        extractFilter->SetDirectionCollapseToIdentity();  // 保持方向
        extractFilter->Update();

        // 可能需要翻转图像以正确显示
        using FlipFilterType = itk::FlipImageFilter<Image2DType>;
        FlipFilterType::Pointer flipFilter = FlipFilterType::New();

        flipFilter->SetInput(extractFilter->GetOutput());

        // 设置翻转轴（通常需要垂直翻转）
        FlipFilterType::FlipAxesArrayType flipArray;
        flipArray[0] = false;
        flipArray[1] = true;  // 垂直翻转
        flipFilter->SetFlipAxes(flipArray);

        flipFilter->Update();

//        std::cout << "翻转"<< std::endl;
        auto itkimage = extractFilter->GetOutput();
        if(viewer_param_.image_type == 0)    itkimage =flipFilter->GetOutput();

        // 转换为VTK并显示
        auto vtkImage = vtkImageData::New();
        if (convert2DITKToVTK(itkimage, vtkImage)) {
            // 创建窗宽窗位映射
            vtkNew<vtkImageMapToWindowLevelColors> windowLevel;
            windowLevel->SetInputData(vtkImage);
            windowLevel->SetWindow(viewer_param_.cur_window);
            windowLevel->SetLevel(viewer_param_.cur_level);
            windowLevel->Update();

            // 更新Actor
//            image_actor_->addin
            image_actor_->SetInputData(windowLevel->GetOutput());
            viewer_param_.cur_slice_ind = sliceIndex;
            updateVTKDisplay();
        }

        vtkImage->Delete();

    } catch (itk::ExceptionObject& ex) {
        std::cerr << "Error extracting slice: " << ex << std::endl;
    }
}


void MPRViewerWidget::generateCoronalPlane(const double& y,CTImageType::Pointer& output,const int& sliceNum){
//    if(index < 0) return;

//    // 边界扩展大小
//    CTImageType::SizeType lowerBound;
//    lowerBound[0] = 16;  // X方向扩展2
//    lowerBound[1] = 16;  // Y方向扩展2
//    lowerBound[2] = 16;  // Z方向扩展2

//    CTImageType::SizeType upperBound;
//    upperBound[0] = 8;
//    upperBound[1] = 8;
//    upperBound[2] = 8;

//    using MirrorPadFilterType = itk::MirrorPadImageFilter<CTImageType, CTImageType>;
//    MirrorPadFilterType::Pointer mirrorPad = MirrorPadFilterType::New();
//    mirrorPad->SetInput(original_image_);
//    mirrorPad->SetPadLowerBound(lowerBound);
//    mirrorPad->SetPadUpperBound(upperBound);
//    mirrorPad->Update();

//    auto input_image = mirrorPad->GetOutput();

    // 1. 获取原图信息
    CTImageType::SpacingType inputSpacing = original_image_->GetSpacing();
    CTImageType::PointType inputOrigin = original_image_->GetOrigin();
    CTImageType::RegionType inputRegion = original_image_->GetLargestPossibleRegion();
//    CTImageType::SizeType inputSize = inputRegion.GetSize();

    // 2. 定义输出图像（冠状面）参数
    //    我们希望输出图像是各向同性的正方形 (512x512)
    const unsigned int outputDim = 512;
    double outputSpacing = inputSpacing[0]; // 使用 X 方向的间距 (0.703125 mm)

    // 输出图像尺寸 (X方向: 512, Z方向: 512)
    CTImageType::SizeType outputSize;
    outputSize[0] = outputDim; // X
    outputSize[1] = outputDim; // Z (注意：这里是冠状面的Y方向对应原图的Z)
    outputSize[2] = sliceNum;

    viewer_param_.file_param.slices = sliceNum;
    // 输出图像的原点 (物理坐标)
    // 固定 Y = fixedY_phys, X 和 Z 的范围与原图保持一致
    CTImageType::PointType outputOrigin;
    outputOrigin[0] = inputOrigin[0];                              // X 起点
    outputOrigin[1] = y;                                            // Y 固定 (冠状面的固定Y)
    outputOrigin[2] = inputOrigin[2];                              // Z 起点

    // 输出图像的间距 (我们希望X和Z方向都是各向同性的)
    CTImageType::SpacingType outputSpacingArray;
    outputSpacingArray[0] = outputSpacing;                         // X 方向间距
    outputSpacingArray[1] = outputSpacing;                         // Z 方向间距
    outputSpacingArray[2] = 1;                                     // 第三维无意义

    // 获取原图的方向矩阵
    CTImageType::DirectionType outputDirection;
    outputDirection.SetIdentity();
    CTImageType::DirectionType inputDirection = original_image_->GetDirection();

    // 设置新方向：
    // 第一列（X轴方向）保持不变
    outputDirection[0][0] = inputDirection[0][0];
    outputDirection[1][0] = inputDirection[1][0];
    outputDirection[2][0] = inputDirection[2][0];

    // 第二列（Y轴方向）应该是原图的Z轴方向
    outputDirection[0][1] = inputDirection[0][2];
    outputDirection[1][1] = inputDirection[1][2];
    outputDirection[2][1] = inputDirection[2][2];

    // 第三列（Z轴方向）保持不变但可能不需要
    outputDirection[0][2] = 0;
    outputDirection[1][2] = 1;
    outputDirection[2][2] = 0;

    // 3. 创建并配置 Resample Filter
    using ResampleFilterType = itk::ResampleImageFilter<CTImageType, CTImageType>;
    ResampleFilterType::Pointer resampler = ResampleFilterType::New();

    // 创建插值器
    using InterpolatorType = itk::BSplineInterpolateImageFunction<CTImageType,double>;
//    using InterpolatorType = itk::LinearInterpolateImageFunction<CTImageType, double>;
    InterpolatorType::Pointer interpolator = InterpolatorType::New();
    interpolator->SetSplineOrder(3);

    resampler->SetInput(original_image_);
    resampler->SetInterpolator(interpolator);
    resampler->SetSize(outputSize);
    resampler->SetOutputOrigin(outputOrigin);
    resampler->SetOutputSpacing(outputSpacingArray);
    resampler->SetOutputDirection(outputDirection); // 通常保持与原图一致
    resampler->SetDefaultPixelValue(0); // 边界外的默认值

    // 4. 执行重建
    try {
        resampler->Update();
    } catch (itk::ExceptionObject & err) {
        std::cerr << "ExceptionObject caught !" << std::endl;
        std::cerr << err << std::endl;
        return;
    }

    output = resampler->GetOutput();
}

void MPRViewerWidget::generateSagittalPlane(const double& x,CTImageType::Pointer& output,const int& sliceNum){
    // 1. 获取原图信息
    CTImageType::SpacingType inputSpacing = original_image_->GetSpacing();
    CTImageType::PointType inputOrigin = original_image_->GetOrigin();
    CTImageType::RegionType inputRegion = original_image_->GetLargestPossibleRegion();
    CTImageType::SizeType inputSize = inputRegion.GetSize();

    // 2. 定义输出图像（冠状面）参数
    //    我们希望输出图像是各向同性的正方形 (512x512)
    const unsigned int outputDim = 512;
    double outputSpacing = inputSpacing[0]; // 使用 X 方向的间距 (0.703125 mm)

    // 输出图像尺寸 (X方向: 512, Z方向: 512)
    CTImageType::SizeType outputSize;
    outputSize[0] = outputDim; // X
    outputSize[1] = outputDim; // Z (注意：这里是冠状面的Y方向对应原图的Z)
    outputSize[2] = sliceNum;         // 第三维设为1，因为是2D图像

    viewer_param_.file_param.slices = sliceNum;

    // 输出图像的原点 (物理坐标)
    // 固定 Y = fixedY_phys, X 和 Z 的范围与原图保持一致
    CTImageType::PointType outputOrigin;
    outputOrigin[0] = x;                              // X 起点
    outputOrigin[1] = inputOrigin[1];                                            // Y 固定 (冠状面的固定Y)
    outputOrigin[2] = inputOrigin[2];                              // Z 起点

    // 输出图像的间距 (我们希望X和Z方向都是各向同性的)
    CTImageType::SpacingType outputSpacingArray;
    outputSpacingArray[0] = outputSpacing;                         // X 方向间距
    outputSpacingArray[1] = outputSpacing;                         // Z 方向间距
    outputSpacingArray[2] = 1;                                     // 第三维无意义

    // 获取原图的方向矩阵
    CTImageType::DirectionType outputDirection;
    outputDirection.SetIdentity();
    CTImageType::DirectionType inputDirection = original_image_->GetDirection();

    // 设置新方向：
    // 第一列（X轴方向）保持不变
    outputDirection[0][0] = inputDirection[0][1];
    outputDirection[1][0] = inputDirection[1][1];
    outputDirection[2][0] = inputDirection[2][1];

    // 第二列（Y轴方向）应该是原图的Z轴方向
    outputDirection[0][1] = inputDirection[0][2];
    outputDirection[1][1] = inputDirection[1][2];
    outputDirection[2][1] = inputDirection[2][2];

    // 第三列（Z轴方向）保持不变但可能不需要
    outputDirection[0][2] = 1;
    outputDirection[1][2] = 0;
    outputDirection[2][2] = 0;

    // 3. 创建并配置 Resample Filter
    using ResampleFilterType = itk::ResampleImageFilter<CTImageType, CTImageType>;
    ResampleFilterType::Pointer resampler = ResampleFilterType::New();

    // 创建插值器
    using InterpolatorType = itk::BSplineInterpolateImageFunction<CTImageType,double>;
//    using InterpolatorType = itk::LinearInterpolateImageFunction<CTImageType, double>;
    InterpolatorType::Pointer interpolator = InterpolatorType::New();
    interpolator->SetSplineOrder(3);

    resampler->SetInput(original_image_);
    resampler->SetInterpolator(interpolator);
    resampler->SetSize(outputSize);
    resampler->SetOutputOrigin(outputOrigin);
    resampler->SetOutputSpacing(outputSpacingArray);
    resampler->SetOutputDirection(outputDirection); // 通常保持与原图一致
    resampler->SetDefaultPixelValue(0); // 边界外的默认值

    // 4. 执行重建
    try {
        resampler->Update();
    } catch (itk::ExceptionObject & err) {
        std::cerr << "ExceptionObject caught !" << std::endl;
        std::cerr << err << std::endl;
        return;
    }

    output = resampler->GetOutput();
}

