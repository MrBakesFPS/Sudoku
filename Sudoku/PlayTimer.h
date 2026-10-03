#ifndef PLAY_TIMER_H
#define PLAY_TIMER_H

#include <chrono>

// A monotonic play timer. Explicit time points make pause/resume deterministic
// to test without sleeping; normal callers use the steady clock defaults.
class PlayTimer {
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    void restart(TimePoint now = Clock::now()) {
        accumulated = Clock::duration::zero();
        started = now;
        running = true;
    }

    void pause(TimePoint now = Clock::now()) {
        if (running) {
            accumulated += now - started;
            running = false;
        }
    }

    void resume(TimePoint now = Clock::now()) {
        if (!running) {
            started = now;
            running = true;
        }
    }

    std::chrono::seconds elapsed(TimePoint now = Clock::now()) const {
        return std::chrono::duration_cast<std::chrono::seconds>(
            accumulated + (running ? now - started : Clock::duration::zero()));
    }

private:
    TimePoint started = Clock::now();
    Clock::duration accumulated = Clock::duration::zero();
    bool running = false;
};

#endif
