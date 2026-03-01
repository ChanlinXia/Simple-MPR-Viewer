#ifndef CUSTOMEMPRVIEWERINTERACTOR_H
#define CUSTOMEMPRVIEWERINTERACTOR_H

#include <vtkInteractorStyleImage.h>
#include <vtkSmartPointer.h>
#include <vtkObject.h>
#include <vtkObjectFactory.h>
#include <QObject>
#include <QVTKOpenGLNativeWidget.h>

enum class InteractorCallbackType
{
    MouseON=0,
    MouseOff,
    MouseWheelForward,
    MouseWheelBackword,
    OnMouseMove,
    ChangeWindow,
    SelectPoint
};

class MPRViewerWidget;

using windowType = MPRViewerWidget;

class CustomeMPRViewerInteractor:public vtkInteractorStyleImage{

public:
    // VTK 接口
    vtkTypeMacro(CustomeMPRViewerInteractor, vtkInteractorStyleImage);
    static CustomeMPRViewerInteractor* New();

    // VTK事件重写
    void OnMouseWheelForward() override;
    void OnMouseWheelBackward() override;
    void OnLeftButtonDown() override;
    void OnLeftButtonUp() override;
    void OnMouseMove() override;

    void setWindow(windowType* window);
signals:
    void sliceChanged(int delta);
    void windowLevelChanged(double window, double level);
    void positionClicked(int x, int y, int globalX, int globalY);

protected:
    CustomeMPRViewerInteractor();
    ~CustomeMPRViewerInteractor() override = default;

private:
    windowType* window_ = nullptr;

    bool mouse_on_ = false;
};

#endif // CUSTOMEMPRVIEWERINTERACTOR_H
