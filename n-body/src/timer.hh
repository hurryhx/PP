#include<queue>
#include<cstring>


typedef long long Time_t;

enum TimerPrecision
{
	TIMER_PRECISION_S,		// second
	TIMER_PRECISION_MS,		// millisecond
	TIMER_PRECISION_US		// microsecond
};

/*
 * Default timer precision is millisecond
 */
class Timer
{
	public:
		Timer();
		~Timer();
		Time_t time() const;
		Time_t begin();
		Time_t end();
		Time_t duration();
		void sleep(Time_t time);

	private:
		TimerPrecision precision;
		Time_t time_begin;
		Time_t time_end;
};

class FpsCounter
{
	protected:
		std::queue<Time_t> que;

	public:
		FpsCounter();
		~FpsCounter();
		void count();
		void reset();
		double fps();
};


