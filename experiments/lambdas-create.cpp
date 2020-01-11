#include <functional>
#include <iostream>
using namespace std;

long long ans;

function<void()> f;

void hello(int val) {
    ans += val;
}

int main() {
    for (int i = 0; i < 1e7; i += 1) {
        f = [=]() {
            hello(i);
        };

        f();
    }

    cout << ans << '\n';

    return 0;
}