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
