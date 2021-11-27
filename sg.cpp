#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

struct condition {
    string start = "    ----\n   /  |_\\\n   \\    /\n    ----\n     |\n     |\\\n     | \\\n";
    string sstep = "    ----\n   /  |_\\\n   \\    /\n    ----\n     |\n     |\n     |\n";
    void move() {
        bool flag = true;
        for (int i = 0; i < start.length(); ++i) {
            if (flag) {
                start.replace(i, 1, "  ");
                flag = false;
            }
            if (start[i] == '\n') {
                flag = true;
            }
        }
    }
    void step() {
        bool flag = true;
        for (int i = 0; i < sstep.length(); ++i) {
            if (flag) {
                sstep.replace(i, 1, "  ");
                flag = false;
            }
            if (sstep[i] == '\n') {
                flag = true;
            }
        }
    }
};
    
int main() {
    condition s;
    for (int i = 0; i < 100000; ++i) {
        system("clear");
        if (i % 2 == 0) {
            cout << s.sstep;
            s.step();
        }
        else {
            cout << s.start;
            s.move();
        }
        usleep(500000);
    }
    return 0;
}
