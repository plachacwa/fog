#include <optional>
#include <string>
using namespace std;

optional<string> getPath(int argc, char* argv[]) {
    if (argc < 2) return nullopt;
    return argv[1];
};