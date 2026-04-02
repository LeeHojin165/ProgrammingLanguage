#include <iostream>
#include <stack>
#include <deque>
#include <sstream>

using namespace std;

string input; // 입력 문자열
char current_token; // 현재 토큰
int position = 0;
int dec_index = 0;
string mix_result[3]; // 결과값 저장
char xyz; // 프린트할 변수가 뭔지
deque<string> operatr; // 연산자 리스트
deque<int> operand; // 피연산자 리스트
deque<string> items;
bool print; // statement가 print인지 아닌지
deque<char> decimal; // dec를 문자열로 저장

struct ret {
    bool tf;
    int result;
};

void get_token() {
    if (position >= input.length()) {
        current_token = '\0';
        return;
    }
    current_token = input[position++];
}

bool program();

bool statement();

bool expr();

bool bexpr();

bool relop();

bool aexpr();

bool term();

bool factor();

bool number();

bool dec();

bool var();

void print_result();

void save_result();

string boolToString(bool value);

int calculate(string expression);

int main() {
    while (true) {
        mix_result[0] = "";
        mix_result[1] = "";
        mix_result[2] = "";
        position = 0;
        dec_index = 0;
        items.clear();
        decimal.clear();
        operand.clear();
        operatr.clear();
        cout << ">> ";
        getline(cin, input);
        if (input == "terminate")
            break;
        // 공백문자 제거
        input.erase(remove_if(input.begin(), input.end(), ::isspace), input.end());
        get_token();
        stringstream ss(input);
        string token;
        while (getline(ss, token, ';')) {
            items.push_back(token);
        }
        if (!program()) {
            cout << "syntax error!!" << endl;
        }
    }
    return 0;
}

bool program() {
    while (statement()) {
        if (current_token == '\0')
            return true;
    }
    return false;
}

bool statement() {
    if (var()) {
        if (current_token == '=') {
            get_token();
            if (expr()) {
                if (current_token == ';') {
                    save_result();
                    dec_index = 0;
                    operand.clear();
                    operatr.clear();
                    get_token();
                    return true;
                }
            }
        }
    } else if (current_token == 'p') {
        get_token();
        if (current_token == 'r') {
            get_token();
            if (current_token == 'i') {
                get_token();
                if (current_token == 'n') {
                    get_token();
                    if (current_token == 't') {
                        get_token();
                        if (var()) {
                            if (current_token == ';') {
                                get_token();
                                print_result();
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool expr() {
    if (bexpr()) {
        return true;
    } else if (aexpr()) {
        return true;
    }

    return false;
}

bool bexpr() {
    if (number()) {
        if (current_token != '=' && current_token != '<' && current_token != '>' && current_token != '!') {
            position -= (dec_index + 1);
            operand.clear();
            get_token();
            return false;
        }
        if (relop()) {
            if (number()) {
                return true;
            }
        }
    }

    return false;
}

bool relop() {
    if (current_token == '=' || current_token == '!') {
        get_token();
        if (current_token == '=') {
            if (input[position - 2] == '=') {
                operatr.push_back("==");
            } else {
                operatr.push_back("!=");
            }
            get_token();
            return true;
        }
    } else if (current_token == '<' || current_token == '>') {
        get_token();
        if (current_token == '=') {
            if (input[position - 2] == '<') {
                operatr.push_back("<=");
            } else {
                operatr.push_back(">=");
            }
            get_token();
            return true;
        }
        if (input[position - 2] == '<') {
            operatr.push_back("<=");
        } else {
            operatr.push_back(">=");
        }
        return true;
    }
    return false;
}

bool aexpr() {
    if (term()) {
        while (current_token == '+' || current_token == '-') {
            if (current_token == '+') {
                operatr.push_back("+");
            } else if(current_token == '-'){
                operatr.push_back("-");
            }
            get_token();
            if (!term())
                return false;
        }

        return true;
    }
    return false;
}

bool term() {
    if (factor()) {
        while (current_token == '*' || current_token == '/') {
            if (current_token == '*') {
                operatr.push_back("*");
            } else if(current_token == '/') {
                operatr.push_back("/");
            }
            get_token();
            if (!factor())
                return false;
        }
        return true;
    }
    return false;
}

bool factor() {
    return number();
}

bool number() {
    if (dec()) {
        get_token();
        while (dec()) {
            get_token();
        }
        string numStr;
        while (!decimal.empty()) {
            numStr += decimal.front();
            decimal.pop_front();
        }
        int num = stoi(numStr);
        operand.push_back(num);
        return true;
    }
    return false;
}

bool dec() {
    if (isdigit(current_token)) {
        decimal.push_back(current_token);
        dec_index++;
        return true;
    }
    return false;
}

bool var() {
    if (current_token == 'x') {
        xyz = 'x';
        get_token();
        return true;
    }
    if (current_token == 'y') {
        xyz = 'y';
        get_token();
        return true;
    }
    if (current_token == 'z') {
        xyz = 'z';
        get_token();
        return true;
    }

    return false;
}

void print_result() {
    if (xyz == 'x') {
        cout << mix_result[0] << " ";
    } else if (xyz == 'y') {
        cout << mix_result[1] << " ";
    } else if (xyz == 'z') {
        cout << mix_result[2] << " ";
    }
}

void save_result() {
    // 연산자가 없을때
    if (operatr.empty()) {
        int num = operand.front();
        if (xyz == 'x') {
            mix_result[0] = to_string(num);
        } else if (xyz == 'y') {
            mix_result[1] = to_string(num);
        } else if (xyz == 'z') {
            mix_result[2] = to_string(num);
        }
    } else {
        string str = operatr.front();
        if (str != "+" && str != "-" && str != "*" && str != "/") {
            // 비교 연산자 일때
            bool result;
            int num1 = operand.front();
            operand.pop_front();
            int num2 = operand.front();
            operand.push_front(num1);
            operand.pop_front();
            switch (str[0]) {
                case '=':
                    result = num1 == num2;
                    break;
                case '!':
                    result = num1 != num2;
                    break;
                case '>':
                    result = num1 > num2;
                    break;
                case '<':
                    result = num1 < num2;
                    break;
                default:
                    result = false;
                    break;
            }

            if (xyz == 'x') {
                mix_result[0] = boolToString(result);
            } else if (xyz == 'y') {
                mix_result[1] = boolToString(result);
            } else {
                mix_result[2] = boolToString(result);
            }
        } else {// 사칙연산
            string str1 = items.front();
            items.pop_front();
            if(str1 == "print"){
                return;
            }
            string str2 = str1.substr(2);
            int num = calculate(str2);

            if (xyz == 'x') {
                mix_result[0] = to_string(num);
            } else if (xyz == 'y') {
                mix_result[1] = to_string(num);
            } else {
                mix_result[2] = to_string(num);
            }
        }
    }
}

string boolToString(bool value) {
    return value ? "TRUE" : "FALSE";
}

int calculate(string expression) {
    stack<int> stack;
    char op = '+';
    int num = 0;

    for (char token : expression) {
        if (isdigit(token)) {
            num = num * 10 + (token - '0');
        }
        if (!isdigit(token) || token == expression.back()) {
            if (op == '+') {
                stack.push(num);
            } else if (op == '-') {
                stack.push(-num);
            } else if (op == '*') {
                stack.push(stack.top() * num);
                stack.pop();
            } else if (op == '/') {
                stack.push(stack.top() / num);
                stack.pop();
            }
            op = token;
            num = 0;
        }
    }

    int result = 0;
    while (!stack.empty()) {
        result += stack.top();
        stack.pop();
    }
    return result;
}