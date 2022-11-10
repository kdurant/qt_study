@startuml

state idle #pink
state readyRead:udpSocket::
state parserFrame: ProtocolDispatch::

idle --> readyRead
readyRead --> processPendingDatagram
processPendingDatagram --> parserFrame
parserFrame --> onlineDataReady
onlineDataReady --> fullSampleDataReady
fullSampleDataReady --> sampleDataReady

@enduml

@startuml

state setFile: 选择航线规划文件
state parseFile: 分析文件，获得具体航线信息，以及航线矩形范围
state setCoverageThreshold: 设置测区栅格大小
state initSurveyArea: 设置测区栅格区域信息

state setSurverArea { 
    state iterateAirline : O(n) = n
    state compareSurvey : O(n) = n^2
    state interpolateAirLine : O(n) = n

    [*] --> iterateAirline
    iterateAirline --> interpolateAirLine
    interpolateAirLine --> compareSurvey
    compareSurvey -->iterateAirline

}

state SurveyArea: 测区信息结构体
state setCurrentPos: 其他模块更新GPS位置
state getCoveragePercent {
    state interpolatePoint : O(n) = n
    state calcRealSurvey: O(n) = n^2

    [*] --> getPrevPos
    getPrevPos --> getCurrPos
    getCurrPos --> interpolatePoint
    interpolatePoint --> getRadarScanExpression
    getRadarScanExpression --> calcRealSurvey
}

setFile --> parseFile
parseFile --> setCoverageThreshold
setCoverageThreshold --> initSurveyArea
initSurveyArea --> setSurverArea
setSurverArea --> SurveyArea

setCurrentPos --> getCoveragePercent
SurveyArea --> getCoveragePercent
@enduml


# 七连屿航线
测区矩形区域大概为20km*10km
20000/50*(10000/50) = 80000
20000/50=400
10000/50=200


# 水下预警雷达类雷达图
采样率：2000Hz(0.5ms), 电机转速：120r/min, 采样长度400, 数据不存硬盘
电机每秒2R, 2*163840/2000 = 163.84
电机计数值间隔大概163

## 显示雷达图流程
1. 先保存一圈数据，作为基本比较对象
```cpp 
struct WaveformInfo
{
    quint32         motorCnt;     // 电机位置计数
    quint32         maxPosition;  // 波形峰值的位置
    QVector<double> pos;          // 相当于x轴
    QVector<double> value;        // 相当于y轴
};
QVector<QVector<WaveExtract::WaveformInfo>> base;
```

- WaveformInfo, 某个通道某一段的数据
- QVector<WaveExtract::WaveformInfo>, 保存一次采样数据的全部信息.   
    如果有4个元素，说明没有第二段采样数据; 如果有8个元素，0，2，4，6为采样数据的第一段；1，3，5，7为采样数据的第二段
- QVector<QVector<WaveExtract::WaveformInfo>> base; 0-180度的全部数据

2. 


# 大连雷达
采样率：5000Hz(0.2ms), 电机转速：1200r/min
5000*(500*2*4) = 20000000/1024/1024=19.073486328125 MB/s

电机每秒20R, 20*163840/5000 = 655.36

## 新增功能
深度传感器

水面模式，只需要结算距离值, 第二段需要找到阈值然后上传一小段数据，用于上位机计算距离
起点：第一段主波
UI只显示距离，不现实波形

水下模式和海洋雷达一致(两段数据)
雷达图现实第二段数据
