#include <iostream>
#include <algorithm>
using namespace std;

struct Task {
    int idx;
    int s;
    int f;
};

int main() {
    int n = 11;
    int s[11] = {1, 3, 0, 5, 3, 5, 6, 8, 8, 2, 12};
    int f[11] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
    Task tasks[11];
    bool res[11];
    for (int i = 0; i < n; i++) {
        Task task;
        task.idx = i + 1;
        task.s = s[i];
        task.f = f[i];
        tasks[i] = task;
    }
    sort(tasks, tasks + n, [](const Task &t1, const Task &t2) -> bool {
        return t1.f < t2.f;
    });
    res[0] = true;
    int j = 0;
    for (int i = 1; i < n; i++) {
        if (tasks[i].s >= tasks[j].f) {
            res[i] = true;
            j = i;
        } else {
            res[i] = false;
        }
    }
    for (int i = 0; i < n; i++) {
        if (res[i]) {
            cout << tasks[i].idx << " ";
        }
    }
    cout << endl;
    return 0;
}