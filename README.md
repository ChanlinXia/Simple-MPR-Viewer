# Simple-MPR-Viewer

A lightweight Multi-Planar Reconstruction (MPR) viewer developed with Qt, ITK, and VTK. This application supports DICOM visualization, multi-planar reconstruction, and interactive manipulation of medical image data.

## Development Environment

- **Qt**: 5.14
- **VTK**: 9.5  
- **ITK**: 5.4
- **Compiler**: Microsoft Visual C++ 2017 (MSVC 2017)

## Preview

<img width="1919" height="1079" alt="Application Interface" src="https://github.com/user-attachments/assets/70cf108a-403e-486a-a317-d7ac36fa71a4" />

**Test Dataset**: LIDC-IDRI-0001 (from the LIDC-IDRI collection, available at [The Cancer Imaging Archive](https://nbia.cancerimagingarchive.net/))

## Known Issues

1. **Rendering Artifacts**: A rectangular artifact appears in the upper region of the viewport, caused by the B-spline interpolation implementation.

2. **Widget Resizing**: The QVTKOpenGLNativeWidget does not resize responsively when the application window dimensions change, resulting in delayed or incorrect viewport updates.
