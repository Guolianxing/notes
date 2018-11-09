#include <iostream>
#include <algorithm>
using namespace std;
struct Job {
    int idx;
    int time;
};

int main() {
    int n = 7;
    int m = 3;
    int machine[3] = {0};
    int time[] = {2, 14, 4, 16, 6, 5, 3};
    Job tasks[7];
    for (int i = 0; i < n; i++) {
        Job job;
        job.idx = i + 1;
        job.time = time[i];
        tasks[i] = job;
    }
    sort(tasks, tasks + n, [](const Job &j1, const Job &j2) -> bool {
        return j1.time > j2.time;
    });
    for (int i = 0; i < m; i++) {
        machine[i] = tasks[i].time;
    }
    int maxTime = machine[0];
    for (int i = m; i < n; i++) {
        int min = machine[0];
        int minidx = 0;
        for (int j = 1; j < m; j++) {
            if (machine[j] < machine[minidx]) {
                minidx = j;
            }
        }
        machine[minidx] += tasks[i].time;
        if (machine[minidx] > maxTime) {
            maxTime = machine[minidx];
        }
    }
    cout << maxTime << endl;
    return 0;
}