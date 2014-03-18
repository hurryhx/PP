#include"timer.hh"
#include<sys/time.h>
#include<unistd.h>

static Time_t time_prec_div[] = {1000000, 1000, 1};
static Time_t time_us();
static Time_t toMs(Time_t time, TimerPrecision prec);

Timer::Timer()
{
	precision = TIMER_PRECISION_MS;
}

Timer::~Timer()
{
}

Time_t Timer::time() const
{
	return time_us() / time_prec_div[precision];
}

Time_t Timer::begin()
{
	time_begin = time_us();
	return time_begin / time_prec_div[precision];
}

Time_t Timer::end()
{
	time_end = time_us();
	return duration();
}

Time_t Timer::duration()
{
	return (time_end - time_begin) / time_prec_div[precision];
}

void Timer::sleep(Time_t time)
{
	time = toMs(time, precision);
	usleep(time);
}


Time_t time_us()
{
	timeval tv;
	gettimeofday(&tv, 0);
	return (Time_t)tv.tv_sec * (Time_t)1000000 + (Time_t)tv.tv_usec;
}


Time_t toMs(Time_t time, TimerPrecision prec)
{
	if (prec == TIMER_PRECISION_S)
		return time * 1000;
	if (prec == TIMER_PRECISION_MS)
		return time;
	if (prec == TIMER_PRECISION_US)
		return time / 1000;
	return -1;
}


FpsCounter::FpsCounter()
{
}


FpsCounter::~FpsCounter()
{
}

void FpsCounter::count()
{
	que.push(Timer().time());
	while (que.back() - que.front() > 1000)
		que.pop();
}

void FpsCounter::reset()
{
	while (!que.empty())
		que.pop();
}

double FpsCounter::fps()
{
	return que.size();
}

