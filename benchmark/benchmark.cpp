#include <benchmark/benchmark.h>
#include <qelapsedtimer.h>
#include "AirArea.h"

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
BENCHMARK(InitSurveyArea)
    ->Unit(benchmark::kMillisecond)
    ->Setup(CoverageDoSetup)
    // ->Arg(1)
    ->Arg(5)
    ->Arg(10)
    ->Arg(20);

BENCHMARK(SetSurveyArea)
    ->Unit(benchmark::kMillisecond)
    ->Setup(CoverageDoSetup)
    ->Arg(1)
    ->Arg(5)
    ->Arg(10)
    ->Arg(20);

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

BENCHMARK(GetCoveragePercent)
    ->Unit(benchmark::kMillisecond)
    ->Setup(PercentDoSetup)
    ->Arg(1)
    ->Arg(5)
    ->Arg(10);
// ->Arg(20);

BENCHMARK_MAIN();