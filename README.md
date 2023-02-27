# games101 task

## task0 Transform

简单的二维旋转矩阵

>  旋转矩阵
> 

```cpp

    Eigen::Matrix3f Rotation{
        {(float)std::cos(45.0f / 180.f), -std::sin(45.0f / 180.f), 1.0f},
        {std::sin(45.0f / 180.f), std::cos(45.0f / 180.f), 2.0f},
        {0, 0 ,1}
    };
```
> 任意轴 n 旋转 
> $$
> \mathbf{R}(\mathbf{n}, \alpha)=\cos (\alpha) \mathbf{I}+(1-\cos (\alpha)) \mathbf{n} \mathbf{n}^{T}+\sin (\alpha) \underbrace{\left(\begin{array}{ccc}
> 0 & -n_{z} & n_{y} \\
> n_{z} & 0 & -n_{x} \\
> -n_{y} & n_{x} & 0
> \end{array}\right)}_{\mathbf{N}}
> $$
>  


> 
>
> 视图变换
> $$
> T_{\text {view }}=\left[\begin{array}{cccc}
> 1 & 0 & 0 & -x_{e} \\
> 0 & 1 & 0 & -y_{e} \\
> 0 & 0 & 1 & -z_{e} \\
> 0 & 0 & 0 & 1
> \end{array}\right]
> $$
>
> $$
> R_{\text {view }}=\left[\begin{array}{cccc}
> x_{\hat{g} \times \hat{t}} & y_{\hat{g} \times \hat{t}} & z_{\hat{g} \times \hat{t}} & 0 \\
> x_{t} & y_{t} & z_{t} & 0 \\
> x_{-g} & y_{-g} & z_{-g} & 0 \\
> 0 & 0 & 0 & 1
> \end{array}\right]
> $$
>
> $$
> View = R_{\text {view }} * T_{\text {view }}
> $$
>
> 

## task1 MVP

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

> 投影矩阵，这样得到的矩阵是反向的很奇怪需要得到的是倒三角
>
> 透视投影矩阵 先移动到中心
> $$
> M_{\text {persp }}=\left[\begin{array}{cccc}
> n & 0 & 0 & 0 \\
> 0 & n & 0 & 0 \\
> 0 & 0 & n+f & -nf \\
> 0 & 0 & 1 & 0
> \end{array}\right]
> $$
> 正交投影矩阵
> $$
> M_{\text {ortho }}=\left[\begin{array}{cccc}
> \frac{2}{r-l} & 0 & 0 & 0 \\
> 0 & \frac{2}{t-b} & 0 & 0 \\
> 0 & 0 & \frac{2}{n-f} & 0 \\
> 0 & 0 & 0 & 1
> \end{array}\right]\left[\begin{array}{cccc}
> 1 & 0 & 0 & -\frac{r+l}{2} \\
> 0 & 1 & 0 & -\frac{t+b}{2} \\
> 0 & 0 & 1 & -\frac{n+f}{2} \\
> 0 & 0 & 0 & 1
> \end{array}\right]
> $$
> 
> 进行透视变换之后再进行正交变换即可

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
        0, 0, 1, 0;// 0, 0, -1, 0是正三角 
    Eigen::Matrix4f orthogonalProjection;//正交投影
    orthogonalProjection <<
        2 / (r - l), 0, 0, 0,
        0, 2 / (t - b), 0, 0,
        0, 0, 2 / (zNear - zFar), 0,
        0, 0, 0, 1;
    projection = orthogonalProjection * perspectiveProjection * projection;
    return projection;
```

## task2 Rasterization

> clip 空间到屏幕空间

