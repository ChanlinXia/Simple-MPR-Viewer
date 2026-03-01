#ifndef MPRVIEWER_H
#define MPRVIEWER_H

#include <iostream>
#include <vtkSmartPointer.h>
#include <vtkDICOMImageReader.h>
#include <vtkImageViewer2.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <QDebug>

#include <itkImageSeriesReader.h>
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>


class MPRViewer{
public:
    MPRViewer();
    ~MPRViewer();

    template<class ImageType>
    void loadMPRFile(const std::string& path){
//        vtkSmartPointer<vtkMPRImageReader> MPR_reader = vtkSmartPointer<vtkMPRImageReader>::New();
        vtkNew<vtkDICOMImageReader> MPR_reader;

        MPR_reader->SetDirectoryName(path.c_str());
        MPR_reader->Update();
        // 输出图像信息
            std::cout << "Image dimensions: "
                      << MPR_reader->GetOutput()->GetDimensions()[0] << " x "
                      << MPR_reader->GetOutput()->GetDimensions()[1] << " x "
                      << MPR_reader->GetOutput()->GetDimensions()[2] << std::endl;

        std::cout << "Pixel spacing: "
                  << MPR_reader->GetPixelSpacing()[0] << ", "
                  << MPR_reader->GetPixelSpacing()[1] << ", "
                  << MPR_reader->GetPixelSpacing()[2] << std::endl;

    }


private:


    // 列表显示


    // 栅格显示

};


#endif // MPRVIEWER_H
