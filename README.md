# games101 task

## task0

简单的二维旋转矩阵

>  旋转矩阵

```cpp

    Eigen::Matrix3f Rotation{
        {(float)std::cos(45.0f / 180.f), -std::sin(45.0f / 180.f), 1.0f},
        {std::sin(45.0f / 180.f), std::cos(45.0f / 180.f), 2.0f},
        {0, 0 ,1}
    };
```



## task1

实现绕z轴的旋转矩阵和透视投影矩阵，注意三角形本身应该是底边在下，因为视锥在相机后面所以导致了一个翻转

> z轴旋转矩阵

```cpp
 Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
    
    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.
    Eigen::Matrix4f rotation;
    float radian = rotation_angle / 180;
    rotation << std::cos(radian), -std::sin(radian), 0, 0,
        std::sin(radian), std::cos(radian), 0, 0,
        0, 0, 1, 0, 
        0, 0, 0, 1;
    model = rotation * model;
    return model;
```

> 投影矩阵

```cpp

    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.

    float t = std::tan(eye_fov / 2) * std::abs(zNear);//top
    float b = -t;//bottom
    float r = aspect_ratio * t;//right
    float l = -r;//left
    Eigen::Matrix4f perspectiveProjection;//透视投影
    perspectiveProjection <<
        zNear, 0, 0, 0,
        0, zNear, 0, 0,
        0, 0, zNear + zFar, -zNear * zFar,
        0, 0, 1, 0;
    Eigen::Matrix4f orthogonalProjection;//正交投影
    orthogonalProjection <<
        2 / (r - l), 0, 0, 0,
        0, 2 / (t - b), 0, 0,
        0, 0, 2 / (zNear - zFar), 0,
        0, 0, 0, 1;
    projection = orthogonalProjection * perspectiveProjection * projection;
    return projection;
```

