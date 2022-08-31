#include <benchmark/benchmark.h>
#include <QtCore>

////////////////////////////////////////////////////////////////////////////
// error: non-const lvalue reference to type 'QByteArray' cannot bind to a temporary of type 'QByteArray'
////////////////////////////////////////////////////////////////////////////
char getByteArray(QByteArray& data)
{
    char res = 0;
    for(auto& i : data)
        res += i;
    return res;
}

char getByteArray_const(const QByteArray& data)
{
    char res = 0;
    for(auto& i : data)
        res += i;
    return res;
}

static void non_const_lvalue(benchmark::State& state)
{
    double len = state.range(0);

    QByteArray array(len, 0x55);
    QByteArray mid;
    char       c;
    for(auto _ : state)
    {
        mid = array.mid(0, len / 2);
        c   = getByteArray(mid);
    }
}

static void const_lvalue(benchmark::State& state)
{
    double len = state.range(0);

    QByteArray array(len, 0x55);
    char       c;
    for(auto _ : state)
    {
        c = getByteArray_const(array.mid(0, len / 2));
    }
}

/*
 * 测试QByteArray转换到QVector<uint8_t>性能
 *
 */
static void QByteArray2QVector_1(benchmark::State& state)
{
    double len = state.range(0);

    QByteArray      source(len, 0x55);
    QVector<quint8> dest;
    for(auto _ : state)
    {
        dest.clear();
        for(auto& i : source)  // 数据格式转换
            dest.append(i);
    }
}

static void QByteArray2QVector_2(benchmark::State& state)
{
    double len = state.range(0);

    QByteArray      source(len, 0x55);
    QVector<quint8> dest;
    dest.reserve(len);
    for(auto _ : state)
    {
        dest.clear();
        for(auto& i : source)  // 数据格式转换
            dest.append(i);
    }
}

/*
 测试for(int i = 0; i < len; i++)
 for(auto i: data) 性能
 */

static void for_normal(benchmark::State& state)
{
    double len = state.range(0);

    std::vector<int> source(len, 0x55);
    int              res = 0;
    for(auto _ : state)
    {
        for(int i = 0; i < len; i++)
            res += source[i];
    }
}

static void for_auto(benchmark::State& state)
{
    double len = state.range(0);

    std::vector<int> source(len, 0x55);
    int              res = 0;
    for(auto _ : state)
    {
        for(auto& i : source)
            res += i;
    }
}

/*
 测试vector.size 在不在for循环里的区别
 */

static void do_in_for(benchmark::State& state)
{
    double len = state.range(0);

    std::vector<int> source(len, 0x55);
    for(auto _ : state)
    {
        for(int i = 0; i < source.size(); i++)
            source[i] = i;
    }
}

static void do_out_for(benchmark::State& state)
{
    double len = state.range(0);

    std::vector<int> source(len, 0x55);
    int              size = source.size();
    for(auto _ : state)
    {
        for(int i = 0; i < size; i++)
            source[i] = i;
    }
}

#if 0
BENCHMARK(non_const_lvalue)
    ->Arg(1024)
    ->Arg(10240)
    ->Arg(102400);
BENCHMARK(const_lvalue)
    ->Arg(1024)
    ->Arg(10240)
    ->Arg(102400);
#endif

#if 0
BENCHMARK(QByteArray2QVector_1)
    ->Arg(1024)
    ->Arg(10240)
    ->Arg(102400);
BENCHMARK(QByteArray2QVector_2)
    ->Arg(1024)
    ->Arg(10240)
    ->Arg(102400);
#endif

#if 0
BENCHMARK(for_normal)
    ->Arg(1024)
    ->Arg(10240);
BENCHMARK(for_auto)
    ->Arg(1024)
    ->Arg(10240);
#endif

#if 1
BENCHMARK(do_in_for)
    ->Arg(1024)
    ->Arg(10240);
BENCHMARK(do_out_for)
    ->Arg(1024)
    ->Arg(10240);
#endif
BENCHMARK_MAIN();
