# Preview: 
A pipeline like image algorithm editor, in order to adjust algorithm parameters in real-time, adjust algorithm content, edit algorithms, and attempt to use it as a pipeline

[![Artifacts](https://github.com/GengGode/image-node-editor/actions/workflows/artifacts.yml/badge.svg)](https://github.com/GengGode/image-node-editor/actions/workflows/artifacts.yml)

# 关于节点

节点有多个输入输出，他们似乎都被定义成一种类型，但是在我看来，节点模型应该被叫做拼图模型  
因为输入的数据类型是确定的，就像拼图的固定的凹陷，而输出的数据类型是不确定的，就像拼图的凸起  
节点连接的时候，就像拼图一样，拼图的凹陷和凸起是吻合的才能搞进行连接  
所以数据也只有在输出节点那里，才能被真正的使用，而不是在输入节点也有一份  
一件事是可以确定的，那就是节点的输入只会连接一个输出，因为我们需要确定的数据   
而输出可以连接多个输入，因为输入只需要输出的引用就可以了
这就是为什么称之为拼图模型，而不是节点模型

# 关于拼图模型

拼图模型是一种抽象的说法，节点的输入输出就是一块拼图的边，而输入的是凹陷，输出的是凸起  
输入的凹陷本身不保存数据，只保存对数据的引用，而输出的凸起存有数据   
当输入的突起和一个输出的凹陷契合连接的时候，输入就会获得输出的数据的引用   
节点的连接就是拼图的正确拼接，而节点能否运行，就看它的所有输入是否都存有引用   
如果有一个输入没有引用，那么这个节点就不能运行，因为它缺少了数据

## 关于并行

每个节点都是独立的，所以只要它的输入是准备好的，那么它就可以运行
这就意味着，每个节点都可以在一个线程中运行，而不会相互影响
但是问题在于，如何通知更新
这就需要一个信号机制，当一个节点的输出更新的时候，它的所有连接的节点都会收到一个信号
通过这个信号触发节点的运行，此处还有一个问题在于，如何保证只有多个输入都更新了才会运行
这就需要一个屏障，当所有的输入都准备好的时候，才会运行

### 细节流程

一块拼图后台不运行线程，如果后台维护一个线程，则开销会很大，当拼图很多的时候
拼图只会在恰当的时候启动一个future
拼图会有持有一个future引用
上一块拼图的输出持有下一块拼图的输入引用
当上一块拼图的输出更新的时候，会调用下一块拼图的输入的更新函数
下一块拼图的输入更新函数会检查所有的输入是否都准备好

如果都准备好了，那么就会启动一个future
```
    if (all_input_ready()) {
        future = std::async(std::launch::async, [this] {
            run();
        });
    }
```
future会调用拼图的运行函数
拼图的运行函数会调用拼图的算法函数
算法运行完毕后，会更新拼图的输出
```
    void run() {
        outputs = algorithm(input);

        for (auto& output : outputs) {
            output->update();
        }
    }
```
拼图的输出更新后，会调用下一块拼图的输入的更新函数

``` mermaid
sequenceDiagram
    participant UI as 界面UI
    box 节点1
    participant 节点1的输入节点1
    participant 节点1
    participant 节点1的输入节点2
    participant 节点1的输出节点1
    end
    UI->>节点1的输入节点1: 起始节点的输出触发绑定的事件
    节点1的输入节点1->>节点1: 检测可行性 false
    UI->>节点1的输入节点2: 起始节点输出绑定的
    节点1的输入节点2->>节点1: 检测可用性 true
    create participant future as 节点1的future
    节点1->>future: 触发运行future
    destroy future
    future->>节点1: future运行结束后触发
    
    节点1->>节点1的输出节点1: 更新值
    节点1的输出节点1->>+节点2的输入节点: 节点1的输出节点触发

```




# Build

you need to release `\examples\imgui-node-editor\external\opencv-lib-Debug.zip` the opencv library in the `\examples\imgui-node-editor\external\opencv-lib-Debug\` directory, or you can download it from the official website and release it in the same directory.

now, you can cmake and build it.

# Wait Fix:
- [ ] Chaos!

# View

![预览图](doc/image.png)

# features:

- [ ] refactor: node and link classes 
- [ ] refactor: execute changed to mutiple threads
    as: every node has a thread to execute, but only some nodes are executed. 
        some nodes has all input parameters, it will be executed.
        nodes linked save the signal to child nodes.
        child nodes will be executed when all input parameters are ready.
- [ ] feature: some stl containers will as node input output parameters
    as: vector, list, map, set
- [ ] feature: some opencv type will as parameters
    as: Mat(over), Point, Rect, Size, Scalar, KeyPoint, DMatch
        and std::vector<cv::KeyPoint>
- [ ] feature: more and more opencv algorithms will be added
    as: cv::resize, cv::cvtColor, cv::threshold, cv::blur, cv::GaussianBlur, cv::medianBlur, cv::Sobel, cv::Scharr, cv::Laplacian, cv::Canny, cv::HoughLines, cv::HoughLinesP, cv::HoughCircles, cv::remap, cv::flip, cv::warpAffine, cv::warpPerspective, cv::integral
- [ ] ui: need support imgui-docking
- [ ] ui: node ui redraw, pin rect change

