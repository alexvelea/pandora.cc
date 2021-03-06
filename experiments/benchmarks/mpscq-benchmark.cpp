#include "mpsc-queue.hpp"

#include <iostream>
#include <vector>
#include <thread>
#include <sys/time.h>
using std::cerr;

int num_threads = 0;

mpsc_queue_t<int> mps;


long int times[100];

void Produce(int num, int num_produce) {
    for (int _ = 0; _ < num_produce; _ += 1) {
        mps.enqueue(num);
    }

    mps.enqueue(0);

    struct timeval tp;
    gettimeofday(&tp, NULL);
    long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    times[num] = ms;
}

void Consume() {
    int num_finished = 0;
    int last = 0;
    int num_switch = 0;
    int num_consumed = 0;
    int num_miss = 0;

    while (num_finished != num_threads) {
        int query;
        if (not mps.can_dequeue()) {
            num_miss += 1;
            continue;
        } else {
            mps.dequeue(query);
        }

        if (query == 0) {
            num_finished += 1;
        } else {
            num_consumed += 1;
        }

        if (last != query) {
            num_switch += 1;
        }

        last = query;
    }

    cerr << "Consumed all queryes\n";
    cerr << num_consumed << " queries\n";
    cerr << num_switch << " switches\n";
    cerr << 100 * num_switch / num_consumed << " percentage switch\n";
    cerr << num_miss << " num misses\n";
}

using std::vector;
using std::thread;

int main(int argc, char** argv) {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    if (argc < 3) {
        cerr << "Usage: ./exe num_threads num_produce_per_thread\n";
        return 0;
    }

    num_threads = atoi(argv[1]);
    int num_produce = atoi(argv[2]);

    vector<thread> producers;

    for (int i = 1; i <= num_threads; i += 1) {
        producers.emplace_back(
                [=]() {
                    Produce(i, num_produce);
                });
    }

    Consume();
    for (auto& itr : producers) {
        itr.join();
    }

    long int mn = times[1];
    long int mx = times[1];
    long int sum = 0;
    for (int i = 1; i <= num_threads; i += 1) {
        mn = std::min(mn, times[i]);
        mx = std::max(mx, times[i]);
        sum += times[i] - ms;
    }

    mn -= ms;
    mx -= ms;


    std::cerr << "min: " << mn << '\t' << "max: " << mx << '\t' << "sum: " << sum << '\t' << "avg: " << sum / num_threads << '\n';
}
