#include <benchmark/benchmark.h>
#include <algorithm>
#include "AirArea.h"
#include <QtCore>
#include <QPolygonF>

QString path{"../../check_airway.txt"};

AirArea     m_designedAirArea;
static void CoverageDoSetup(const benchmark::State& state)
{
    m_designedAirArea.setFile(path);
    m_designedAirArea.parseFile();
}

static void InitSurveyArea(benchmark::State& state)
{
    double threshold = state.range(0);
    for(auto _ : state)
    {
        m_designedAirArea.setCoverageThreshold(threshold);
        m_designedAirArea.initSurveyArea(threshold);
    }
}
static void SetSurveyArea(benchmark::State& state)
{
    double threshold = state.range(0);
    for(auto _ : state)
    {
        m_designedAirArea.setCoverageThreshold(threshold);
        m_designedAirArea.initSurveyArea(threshold);
        m_designedAirArea.setSurverArea();
    }
}

BspConfig::Gps_Info prevPos{17476, 0, 0, 19.556985561977637, 109.44228145496466, 359.49531255476177, 359.49531255476177, 339.5817447173903, -2.8658673724470445, 0.8977233399813679};
BspConfig::Gps_Info currentPos{17476, 0, 0, 19.557024501297718, 109.44238916966063, 359.54851242434233, 359.54851242434233, 339.7008848804187, -2.8586711836017757, 1.1314910681129875};
AirArea             m_surveyArea(prevPos, currentPos);

static void PercentDoSetup(const benchmark::State& state)
{
    m_surveyArea.setFile(path);
    m_surveyArea.parseFile();

    double threshold = state.range(0);
    m_surveyArea.setCoverageThreshold(threshold);
    m_surveyArea.initSurveyArea(threshold);
    m_surveyArea.setSurverArea();
    m_surveyArea._getCurrentSpeed(currentPos);
}

static void GetCoveragePercent(benchmark::State& state)
{
    for(auto _ : state)
    {
        m_surveyArea.__getCoveragePercent();
    }
}

////////////////////////////////////////////////////////////////////////////
static void point2seg_distance(benchmark::State& state)
{
    QPointF p1{109.4562258, 19.56154218};
    QLineF  target{
        QPointF{109.4573788, 19.56213428},
        QPointF{109.4397899, 19.55589398},
    };
    for(auto _ : state)
    {
        m_surveyArea.point2seg_distance(p1, target);
    }
}

////////////////////////////////////////////////////////////////////////////
static void QPloygonF_Reverse(benchmark::State& state)
{
    double    len = state.range(0);
    QPolygonF polygon;
    for(int i = 0; i < len; i++)
    {
        polygon << QPointF(i, i);
    }
    for(auto _ : state)
    {
        std::reverse(polygon.begin(), polygon.end());
    }
}

#if 0
BENCHMARK(InitSurveyArea)
    ->Unit(benchmark::kMillisecond)
    ->Setup(CoverageDoSetup)
    ->Arg(5)
    ->Arg(10)
    ->Arg(20);
#endif

#if 0
BENCHMARK(SetSurveyArea)
    ->Unit(benchmark::kMillisecond)
    ->Setup(CoverageDoSetup)
    ->Arg(1)
    ->Arg(5)
    ->Arg(10)
    ->Arg(20);
#endif

#if 0
BENCHMARK(GetCoveragePercent)
    ->Unit(benchmark::kMillisecond)
    ->Setup(PercentDoSetup)
    ->Arg(1)
    ->Arg(5)
    ->Arg(10);
#endif

#if 0
BENCHMARK(point2seg_distance)
    ->Unit(benchmark::kMicrosecond);
#endif

#if 1
BENCHMARK(QPloygonF_Reverse)
    ->Unit(benchmark::kMicrosecond)
    ->Arg(10000)
    ->Arg(50000)
    ->Arg(100000);
#endif

BENCHMARK_MAIN();
