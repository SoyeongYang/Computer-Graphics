# Computer-Graphics
Pictures style I/O technology involving the Creation, manipulation, storing and display

+ a computer is used to create a picture
+ model based approach
+ concerns pictorial synthesis of real or imagery objects from computer based model
+ deals with vector data

## Overview of Rendering Pipeline
1. **Input**: 3D Primitives
2. **Modeling Transformation**: Transform into 3D world coordinate system
3. **Lighting**: Illuminate according to lighting and reflection
4. **Viewing Transformation**: Transform into 3D camera coordinate system<br><br>
  First move the viewing point (P<sub>0</sub>) to the world coordinate system, then rotate it according to the camera coordinates.<br>
  ![matrix](https://user-images.githubusercontent.com/88317168/128609943-0365a10f-d025-486e-bf87-a7ee6a69569b.png)<br>
  (R: Rotate Matrix, T: Translate Matrix)
  ![matrix2](https://user-images.githubusercontent.com/88317168/128609957-01f3ace6-b5b2-457b-b0e2-e2c0a06145c3.png)<br>
  (M: Viewing Matrix)


5. **Projection Transformation**: Transform into 2D camera coordinate system<br>
    + Perspective Projection Matrix<br>
  ![persp](https://user-images.githubusercontent.com/88317168/128609960-46f4fef4-949c-466d-9842-1e9c4ffe6a83.png)<br>
    + Orthographic Projection Matrix<br>
  ![ortho](https://user-images.githubusercontent.com/88317168/128609966-48c8b6b2-e0b0-417b-bea9-fa3d13eff831.png)<br>
6. **Clipping**: Clipping the basic shape out of the camera point of view
7. **Viewport Transformation**: Transform into image coordinate system<br>
    + Window-to-Viewport mapping<br>
  ![viewport](https://user-images.githubusercontent.com/88317168/128610003-2ffce9d3-4879-48f1-a82d-ebacd6bdc804.png)
8. **Scan Conversion**: Pixel Drawing (includes texturing, hidden surface etc)
9. **Output**: Image

## Final Project
### Rendering virtual space with SOR Modeling Data and Navigation<br>
Develop a program that stores various objects created using the SOR modeler and uses it to organize virtual spaces and change perspectives and rendering styles.
