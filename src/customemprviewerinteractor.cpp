#include "../include/customemprviewerinteractor.h"
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkImageProperty.h>
#include <vtkImageSlice.h>
#include <vtkImageSliceMapper.h>
#include <iostream>

#include "../include/MPRViewerWidget.h"

vtkStandardNewMacro(CustomeMPRViewerInteractor);

CustomeMPRViewerInteractor::CustomeMPRViewerInteractor()
{

}

void CustomeMPRViewerInteractor::setWindow(windowType* window){
    window_ = window;
}

void CustomeMPRViewerInteractor::OnMouseWheelForward(){
    std::cout << "mouse wheel forward on" << std::endl;
//    this->Interactor->GetMousePosition();
    window_->interactorCallBack(InteractorCallbackType::MouseWheelForward,1);

}

void CustomeMPRViewerInteractor::OnMouseWheelBackward(){
    std::cout << "mouse wheel backward on" << std::endl;
    window_->interactorCallBack(InteractorCallbackType::MouseWheelBackword,1);
}

void CustomeMPRViewerInteractor::OnLeftButtonUp(){
    std::cout << "LeftButtonUp on" << std::endl;
    mouse_on_ = false;
}

void CustomeMPRViewerInteractor::OnLeftButtonDown(){
    std::cout << "OnLeftButtonDown on" << std::endl;
    mouse_on_ = true;

    // [TODO]：映射鼠标点击位置到图像位置
    if(Interactor->GetControlKey()){
        std::cout << "select point" << std::endl;
        window_->interactorCallBack(InteractorCallbackType::MouseWheelBackword,1);
    }
}

void CustomeMPRViewerInteractor::OnMouseMove(){
    if(!mouse_on_) return; // 仅处理鼠标按下时的移动
    std::cout << "OnMouseMove on" << std::endl;

//    int currentX= this->Interactor->GetEventPosition()[0];
    int currentY= this->Interactor->GetEventPosition()[1];

//    int lastX= this->Interactor->GetLastEventPosition()[0];
    int lastY= this->Interactor->GetLastEventPosition()[1];

//    int deltaX = currentX -lastX;
    int deltaY = currentY -lastY;

    if(Interactor->GetControlKey()){
        window_->interactorCallBack(InteractorCallbackType::ChangeWindow,std::tuple<int>(deltaY));
    }

    if(window_){
        window_->interactorCallBack(InteractorCallbackType::OnMouseMove,std::tuple<int>(deltaY));
        std::cout << "level change" << std::endl;
    }

}
