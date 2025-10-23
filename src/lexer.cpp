#include "lexer.h"
using namespace std;

Lexer::Lexer(string_view code) : source(code), backCar(this), frontCar(this) {};

vector<Token> tokenize() {};