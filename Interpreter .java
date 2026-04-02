import java.util.*;
import java.util.regex.*;

public class Interpreter {
    private Map<String, Integer> variables = new HashMap<>();
    private List<String> tokens;
    private int currentTokenIndex = 0;

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        Interpreter interpreter = new Interpreter();

        while (true) {
            System.out.print(">> ");
            String code = scanner.nextLine();
            if (code.equals("terminate")) {
                break;
            }
            try {
                interpreter.run(code);
                System.out.println();
            } catch (Exception e) {
                System.out.print("Syntax Error!!");
                System.out.println();
            }
        }
        scanner.close();
    }

    private String getToken() {
        if (currentTokenIndex < tokens.size()) {
            return tokens.get(currentTokenIndex++);
        }
        return null;
    }


    public void run(String code) {
        tokenize(code);
        currentTokenIndex = 0;
        try {
            parseProgram();
        } catch (Exception e) {
            throw new RuntimeException("Syntax Error!!");
        }
    }

    private void tokenize(String code) {
        String[] tokenlist = code.split(" ");
        tokens = Arrays.asList(tokenlist);
    }


    private void parseProgram() {
        parseDeclarations();
        parseStatements();
    }

    private void parseDeclarations() {
        while (currentTokenIndex < tokens.size()) {
            String token = tokens.get(currentTokenIndex);
            if (token.equals("int")) {
                parseDeclaration();
            } else {
                return; // 선언부 종료
            }
        }
    }

    private void parseStatements() {
        while (currentTokenIndex < tokens.size()) {
            String token = tokens.get(currentTokenIndex);
            if (token.equals("print") || var(token) || token.equals("do")) {
                parseStatement();
            } else {
                throw new RuntimeException("Syntax Error!!");
            }
        }
    }
    private void parseDeclaration() {
        currentTokenIndex++;
        String varName = getToken();
        if (!var(varName)) {
            throw new RuntimeException("Syntax Error!!");
        }
        if (!getToken().equals(";")) {
            throw new RuntimeException("Syntax Error!!");
        }
        variables.put(varName, 0);
    }

    private void parseStatement() {
        String token = getToken();
        if (token.equals("print")) {
            Object val;
            Boolean ret;
            String next = tokens.get(currentTokenIndex);
            if (relop(next)) {
                ret = parseBexpr();
                val = ret ? "TRUE" : "FALSE";
            } else {
                val = parseAexpr();

            }
            if (!getToken().equals(";")) {
                throw new RuntimeException("Syntax Error!!");
            }
            System.out.print(val+" ");
        } else if(var(token) && !token.equals("do")){
            String varName = token;
            if (!variables.containsKey(varName)) {
                throw new RuntimeException("Syntax Error!!");
            }
            if (!getToken().equals("=")) {
                throw new RuntimeException("Syntax Error!!");
            }
            int value = parseAexpr();
            if (!getToken().equals(";")) {
                throw new RuntimeException("Syntax Error!!");
            }
            variables.put(varName, value);
        } else if(token.equals("do")){
            parseDoWhile();
        }
    }

    private void parseDoWhile() {
        if (!getToken().equals("{")) {
            throw new RuntimeException("Syntax Error!!");
        }
        List<String> doTokens = new ArrayList<>();
        while (!tokens.get(currentTokenIndex).equals("}")) {
            doTokens.add(getToken());
        }
        getToken(); // '}' 빼기
        //원래의 토큰 인덱스와 토큰을 저장
        int tempIndex = currentTokenIndex;
        List<String> originTokens = tokens;
        //do 안의 코드를 실행
        currentTokenIndex = 0;
        tokens = doTokens;

        parseStatements();
        //원래대로 복귀
        currentTokenIndex = tempIndex;
        tokens = originTokens;
        if (!getToken().equals("while")) {
            throw new RuntimeException("Syntax Error!!");
        }
        if (!getToken().equals("(")) {
            throw new RuntimeException("Syntax Error!!");
        }
        List<String> whileTokens = new ArrayList<>();
        while(!tokens.get(currentTokenIndex).equals(";")){
            whileTokens.add(getToken());
        }
        whileTokens.remove(whileTokens.size()-1);
        if (!getToken().equals(";")) {
            throw new RuntimeException("Syntax Error!!");
        }
        int afterWhileIndex = currentTokenIndex;
        while (true) {
            currentTokenIndex = 0;
            tokens = whileTokens;
            if(!parseBexpr()){
                break;
            }
            currentTokenIndex = 0;
            tokens = doTokens;
            parseStatements();
        }
        //토큰과 인덱스를 원래대로 복귀
        currentTokenIndex = afterWhileIndex;
        tokens = originTokens;
    }

    private Boolean parseBexpr() {
        String relop = getToken();
        int left = parseAexpr();
        int right = parseAexpr();
        boolean result = evaluateBexpr(relop, left, right);
        return result;
    }

    private int parseAexpr() {
        int value = parseTerm();
        while (currentTokenIndex < tokens.size()) {
            String operator = tokens.get(currentTokenIndex);
            if (operator.equals("+") || operator.equals("-") || operator.equals("*") || operator.equals("/")) {
                currentTokenIndex++;
                int nextValue = parseTerm();
                value = evaluateAexpr(value, operator, nextValue);
            } else {
                break;
            }
        }
        return value;
    }

    private int parseTerm() {
        String token = getToken();
        //number일때
        if (token.matches("\\d+")) {
            return Integer.parseInt(token);
            //var 일때
        } else if (var(token)) {
            if (!variables.containsKey(token)) {
                throw new RuntimeException("Syntax Error!!");
            }
            return variables.get(token);
            //
        } else if (token.equals("(")) {
            int value = parseAexpr();
            if (!getToken().equals(")")) {
                throw new RuntimeException("Syntax Error!!");
            }
            return value;
        } else {
            throw new RuntimeException("Syntax Error!!");
        }
    }

    private boolean evaluateBexpr(String relop, int left, int right) {
        switch (relop) {
            case "==": return left == right;
            case "!=": return left != right;
            case "<": return left < right;
            case ">": return left > right;
            case "<=": return left <= right;
            case ">=": return left >= right;
            default: throw new RuntimeException("Invalid relational operator");
        }
    }

    private int evaluateAexpr(int left, String operator, int right) {
        switch (operator) {
            case "+": return left + right;
            case "-": return left - right;
            case "*": return left * right;
            case "/": return left / right;
            default: throw new RuntimeException("Invalid arithmetic operator");
        }
    }

    private boolean var(String token) {
        return token.matches("[a-z]+");
    }

    private boolean relop(String token) {
        return token.equals("==") || token.equals("!=") || token.equals("<") ||
                token.equals(">") || token.equals("<=") || token.equals(">=");
    }
}
