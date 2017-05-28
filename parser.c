#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
char* token = 0;
int lastChar = 0;
int isTokenIntegral = 0;
enum datatype{SPHERE, LIGHT, MATH};
typedef enum datatype datatype;
int linenum = 1;
int isXYZexpression = 0;
int isRGBexpression = 0;
int isTexpression = 0;
uint64_t T_START = 0;
uint64_t T_END = 0;
typedef struct STNode{
    char* name;
    datatype type;
    struct STNode* next;
} STNode;
STNode* ST = 0;
void terminate(){
    fprintf(stderr, "TERMINATING ON LINE %d\n", linenum);
    exit(0);
}
typedef struct TTNode{
    char* name;
    struct TTNode* next;
} TTNode;
TTNode* textures = 0;
void addToTT(char* name){
    fprintf(stderr, "Adding %s to list of textures\n", name);
    TTNode* newguy = malloc(sizeof(TTNode));
    (newguy -> name) = strdup(name);
    (newguy -> next) = textures;
    textures = newguy;
}
void addToST(char* name, datatype dt){
    STNode* current = malloc(sizeof(STNode));
    (current -> name) = name;
    (current -> type) = dt;
    (current -> next) = ST;
    ST = current;
}
typedef struct TypeNode{
    char* name;
    datatype type;
    struct TypeNode* next;
} TypeNode;
TypeNode* types = 0;

void addToTypes(char* name, datatype type){
    TypeNode* current = malloc(sizeof(TypeNode));
    (current -> name) = name;
    (current -> type) = type;
    (current -> next) = types;
    types = current;
}

void pushXMM(int id){
    /*printf("mov $16, %%r9\n");
      printf("sub %%rsp, %%r9\n");
      printf("movdqu %%xmm%d, (%%rsp)\n", id);*/
    printf("pushq %%rax\n");
    printf("pushq %%rax\n");
    printf("movsd %%xmm%d, 8(%%rsp)\n", id);
    printf("movsd %%xmm%d, (%%rsp)\n", id);
}

void popXMM(int id){
    /*printf("movdqu (%%rsp), %%xmm%d\n", id);
      printf("mov $16, %%r9\n");
      printf("add %%rsp, %%r9\n");*/
    printf("movsd 8(%%rsp), %%xmm%d\n", id);
    printf("popq %%rax\n");
    printf("popq %%rax\n");
    //printf("popq DOUBLE_STORE\n");
    //printf("movsd DOUBLE_STORE, %%xmm%d\n", id);
}

int getCharBuffer(){
    return lastChar;
}

int getNextChar(){
    if(lastChar == 0){
        return getchar();
    } else {
        int store = lastChar;
        lastChar = 0;
        return store;
    }
}

int isWhiteSpace(char data){
    if(data == '\n'){
        linenum++;
    }
    return isspace(data);
}

int isAlphabetic(char data){
    return ('a' <= data && data <= 'z') || ('A' <= data && data <= 'Z');
}

int isNumeric(char data){
    return ('0' <= data && data <= '9');
}

int isAlphanumeric(char data){
    return isAlphabetic(data) | isNumeric(data);
}
int isInit(){
    return strcmp(token, "init") == 0;
}
int isFrames(){
    return strcmp(token, "frames") == 0;
}
int isTexture(){
    return strcmp(token, "Texture") == 0;
}
int isInt(){
    return isTokenIntegral;
}
int isSphere(){
    return strcmp(token, "Sphere") == 0;
}
int isSETTEX(){
    return strcmp(token, "SETTEX") == 0;
}
int isMAP(){
    return strcmp(token, "MAP") == 0;
}
int isROTCAM(){
    return strcmp(token, "ROTCAM") == 0;
}
int isMath(){
    return strcmp(token, "Math") == 0;
}
int isShape(){
    return strcmp(token, "Shape") == 0;
}
int isCamera(){
    return strcmp(token, "CAMERA") == 0;
}
int isRet(){
    return strcmp(token, "return") == 0;
}
int isLight(){
    return strcmp(token, "Light") == 0;
}
int isX(){
    return strcmp(token, "x") == 0;
}
int isY(){
    return strcmp(token, "y") == 0;
}
int isZ(){
    return strcmp(token, "z") == 0;
}
int isR(){
    return strcmp(token, "r") == 0;
}
int isG(){
    return strcmp(token, "g") == 0;
}
int isB(){
    return strcmp(token, "b") == 0;
}
int isA(){
    return strcmp(token, "a") == 0;
}
int isT(){
    return strcmp(token, "t") == 0;
}
STNode* getVar(){
    STNode* current = ST;
    while(current != 0){
        if(strcmp(current -> name, token) == 0){
            return current;
        }
        current = current -> next;
    }
    return 0;
}

TypeNode* getType(){
    TypeNode* current = types;
    while(current != 0){
        if(strcmp(current -> name, token) == 0){
            return current;
        }
        current = current -> next;
    }
    return 0;
}

int isLeftCurl(){
    return strcmp(token, "{") == 0;
}
int isRightCurl(){
    return strcmp(token, "}") == 0;
}
int isLeftSquare(){
    return strcmp(token, "[") == 0;
}
int isRightSquare(){
    return strcmp(token, "]") == 0;
}
int isComma(){
    return strcmp(token, ",") == 0;
}
int isDot(){
    return strcmp(token, ".") == 0;
}
int isEq(){
    return strcmp(token, "=") == 0;
}
int isLT(){
    return strcmp(token, "<") == 0;
}
int isGT(){
    return strcmp(token, ">") == 0;
}
int isPlus(){
    return strcmp(token, "+") == 0;
}
int isSub(){
    return strcmp(token, "-") == 0;
}
int isMul(){
    return strcmp(token, "*") == 0;
}
int isDiv(){
    return strcmp(token, "/") == 0;
}
int isAnd(){
    return strcmp(token, "&") == 0;
}
int isOr(){
    return strcmp(token, "|") == 0;
}
int isLeftParen(){
    return strcmp(token, "(") == 0;
}
int isRightParen(){
    return strcmp(token, ")") == 0;
}
int isMod(){
    return strcmp(token, "%") == 0;
}
int isSemi(){
    return strcmp(token, ";") == 0;
}
int isSingletonToken(int current){
    return (current == '|' || current == '/' || current == '>' || current == '&' || current == '%' || current == '(' || current == ')' || current == '*' || current == '+' || current == '{' || current == '}' || current == '.' || current == '[' || current == ']' || current == ',' || current == '.' || current == ';' || current == '<');
}
int parseNumeric(int current, int count){
    while(isNumeric(current) || current == '.'){
        token[count] = current;
        token = realloc(token, count + 2);
        count++;
        current = getNextChar();
    }
    isTokenIntegral = 1;
    lastChar = current;
    return count;
}
void expression();
void getNextToken(){
    if(token != 0){
        free(token);
    }
    token = malloc(1);
    int current = getNextChar();
    while(isWhiteSpace(current)){
        current = getNextChar();
    }
    int count = 0;
    isTokenIntegral = 0;
    if(isAlphabetic(current)){
        while(isAlphanumeric(current)){
            token[count] = current;
            token = realloc(token, count + 2);
            count++;
            current = getNextChar();
        }
        lastChar = current;
    } else if(isNumeric(current)){
        count = parseNumeric(current, count);
    } else if(current == '=') {
        current = getNextChar();
        token[0] = '=';
        if(current == '='){
            token = realloc(token, 3);
            token[1] = '=';
            count = 2;
        } else {
            token = realloc(token, 2);
            count = 1;
            lastChar = current;
        }
    } else if(current == '-') {
        //TODO account for subtraction operation
        token[count] = '-';
        token = realloc(token, count + 2);
        count++;
        current = getNextChar();
        if(isNumeric(current)){
            count = parseNumeric(current, count);
        }
    } else if (isSingletonToken(current)){
        token = realloc(token, 2);
        token[0] = current;
        count++;  
    } else if (current == -1) {
        token[0] = 0;
        return;
    } else {
        fprintf(stderr,"UNKNOWN TOKEN~TERMINATING: %c\n",current);
        token[0] = 0;
        return;
    }
    token[count] = 0;
}
void e1(){
    if(isLeftParen()){
        getNextToken();
        expression();
        printf("movsd %%xmm0, %%xmm7\n");
        if(!isRightParen()){
            terminate();
        }
    } else if(isT() && isTexpression){
        printf("movsd TIME_DOUBLE, %%xmm7\n");
    } else if(isX() && isXYZexpression){
        printf("movsd -8(%%rbp), %%xmm7\n");
    } else if(isY() && isXYZexpression){
        printf("movsd -24(%%rbp), %%xmm7\n");
    } else if(isZ() && isXYZexpression){
        printf("movsd -40(%%rbp), %%xmm7\n");
    } else if(isR() && isRGBexpression){
        printf("movsd (%%r9), %%xmm7\n");  
    } else if(isG() && isRGBexpression){
        printf("movsd 8(%%r9), %%xmm7\n");  
    } else if(isB() && isRGBexpression){
        printf("movsd 16(%%r9), %%xmm7\n");
    } else if(isA() && isRGBexpression){
        printf("movsd 24(%%r9), %%xmm7\n");  
    } else if(isTokenIntegral) {
        double val = 0;
        int current = 0;
        int mul = 1;
        if(token[current] == '-'){
            mul = -1;
            current++;
        }
        double p = 1;
        int dec = 0;
        while(token[current] != 0){
            if(token[current] == '.'){
                dec = 1;
                p /= 10;
            } else if(dec == 1){
                val = val + (token[current] - '0') * p;
                p /= 10;  
            } else {
                val = val * 10 + (token[current] - '0');
            }
            current++;
        }
        val *= mul;
        uint64_t* dval = (uint64_t*) (&val);
        printf("mov $%lu, %%rax\n", dval[0]);
        printf("mov %%rax, -16(%%rsp)\n");
        printf("movsd -16(%%rsp), %%xmm7\n");
    } else {
        if(getCharBuffer() == '('){
            char* name = strdup(token);
            getNextToken();
            getNextToken();
            int ind = 0;
            pushXMM(0);
            pushXMM(1);
            pushXMM(2);
            while(!isRightParen() && ind < 3){
                expression();
                pushXMM(ind);
                if(!isRightParen()){
                    getNextToken();
                }
                ind++;
            }
            ind--;
            while(ind >= 0){
                popXMM(ind);
                ind--;
            }
            printf("call USER_FUNC_%s\n", name);
            printf("movsd %%xmm0, %%xmm7\n");
            popXMM(2);
            popXMM(1);
            popXMM(0);
        }
    }
    pushXMM(7);
    getNextToken();
}
void e2(){
    e1();
    while(isMul() || isDiv() || isMod()){
        int code = (isMul() ? 3 : 0) + (isDiv() ? 2 : 0) + (isMod() ? 1 : 0);
        getNextToken();
        e1();
        popXMM(1);
        popXMM(0);
        if(code == 3){
            printf("call mul\n");
        } else if(code == 2){
            printf("call div\n");
        } else if(code == 1){
            printf("call mod\n");
        }
        pushXMM(0);
    }
}
void e3(){
    e2();
    while(isPlus() || isSub()){
        int code = (isPlus() ? 2 : 0) + (isSub() ? 1 : 0);
        getNextToken();
        e2();
        popXMM(1);
        popXMM(0);
        if(code == 2){
            printf("call plus\n");
        } else if(code == 1){
            printf("call sub\n");
        }
        pushXMM(0);
    }
}
void e4(){
    e3();
    while(isLT() || isGT()){
        int code = (isLT() ? 2 : 0) + (isGT() ? 1 : 0);
        getNextToken();
        e3();
        popXMM(1);
        popXMM(0);
        if(code == 2){
            printf("call isLT\n");
        } else if(code == 1){
            printf("call isGT\n");
        }
        pushXMM(0);
    }
}
void e5(){
    e4();
    while(isAnd() || isOr()){
        int code = (isOr() ? 2 : 0) + (isAnd() ? 1 : 0);
        getNextToken();
        e4();
        popXMM(1);
        popXMM(0);
        if(code == 1){
            printf("call and\n");
        } else if(code == 2){
            printf("call or\n");
        }
        pushXMM(0);
    }
}
void expression(){
    //printf("mov $%s, %%r9\n", token);
    e5();
    popXMM(0);
    //printf("movsd %%xmm0, %%xmm1\n");
    //printf("call printDouble\n");
    //printf("call printDouble\n");
}
void statement(){
    STNode* currentShape = getVar();
    TypeNode* currentTypeP = getType();
    //fprintf(stderr, "With token %s I found %p\n", token, currentShape);
    if(isSphere()){
        getNextToken();
        char* name = strdup(token);
        getNextToken();
        if(!isSemi()){
            terminate();
        }
        getNextToken();
        addToST(name, SPHERE);
        printf("call createSphere\n");
        printf("mov %%rax, USER_%s\n", name);
    } else if(isLight()){
        getNextToken();
        char* name = strdup(token);
        getNextToken();
        if(!isSemi()){
            terminate();
        }
        getNextToken();
        addToST(name, LIGHT);
        printf("call createLight\n");
        printf("mov %%rax, USER_%s\n", name);
    } else if(isSETTEX()){
        getNextToken();
        if(isCamera()){
            printf("mov $0, %%rdi\n");
        } else {
            printf("mov USER_%s, %%rdi\n", token);
        }
        getNextToken();
        printf("mov $TEXTURE_%s, %%rsi\n", token);
        getNextToken();
        if(!isSemi()){
          terminate();
        }
        getNextToken();
        printf("push %%rbp\n");
        printf("push %%rsp\n");
        printf("call setTexture\n");  
        printf("pop %%rsp\n");
        printf("pop %%rbp\n");
    } else if(isMAP()){
        //MAP <IMAGE NAME> <scaled X coord> <scaled Y coord>
        getNextToken();
        addToTT(token);
        printf("mov $DATA_%s, %%rsi\n", token);
        getNextToken();
        expression();
        printf("movsd %%xmm0, %%xmm3\n");
        expression();
        printf("movsd %%xmm0, %%xmm1\n");
        printf("movsd %%xmm3, %%xmm0\n");
        printf("mov %%r9, %%rdi\n");
        if(!isSemi()){
            terminate();
        }
        printf("call map\n");
        getNextToken();
    } else if(isROTCAM()){
        getNextToken();
        expression();
        pushXMM(0);
        expression();
        pushXMM(0);
        expression();
        printf("movsd %%xmm0, %%xmm2\n");  
        popXMM(1);
        popXMM(0);
        if(!isSemi()){
            terminate();
        }
        printf("call rotateCamera\n");
        getNextToken();
    } else if(currentTypeP != 0) {
        getNextToken();
        char* name = strdup(token);
        getNextToken();
        if(!isSemi()){
            terminate();
        }
        getNextToken();
        int currentType = currentTypeP -> type;
        addToST(name, MATH);
        if(currentType == MATH){
            //fprintf(stderr, "MATH MY FAV\n");
            printf("mov $MATH_%s, %%rdi\n", currentTypeP -> name);
            printf("call createMath\n");
            printf("mov %%rax, USER_%s\n", name);
        }
    } else if(/*currentShape != 0 || strcmp(token, "CAMERA") == 0*/ 1==1){
        char* name = strdup(token);
        int isSpecial = (isR() || isG() || isB() || isA()) && isRGBexpression;
        //fprintf(stderr,"T1: %s\n", token);
        getNextToken();
        //fprintf(stderr,"T2: %s\n", token);
        char* attr = 0;
        if(isDot()){
            getNextToken();
            attr = strdup(token);
            getNextToken();
        }
        //fprintf(stderr, "ABOUT TO CHECK FOR EQ, current token is ~%s~\n", token);
        if(!isEq()){
            terminate();
        }
        getNextToken();
        expression();
        if(isSpecial == 0){
            if(strcmp(name, "CAMERA") == 0){
                printf("mov $0, %%rdi\n");
            } else {
                printf("mov USER_%s, %%rdi\n", name);
            }
        }
        //printf("mov %%r9, %%rsi\n");
        if(strcmp(name, "r") == 0 && isRGBexpression){
            printf("movsd %%xmm0, (%%r9)\n");
        } else if(strcmp(name, "g") == 0 && isRGBexpression){
            printf("movsd %%xmm0, 8(%%r9)\n");
        } else if(strcmp(name, "b") == 0 && isRGBexpression){
            printf("movsd %%xmm0, 16(%%r9)\n");
        } else if(strcmp(name, "a") == 0 && isRGBexpression){
            printf("movsd %%xmm0, 24(%%r9)\n");
        } else if(currentShape == 0 || (currentShape -> type) != LIGHT){
            if(attr != 0){
                printf("call set_%s\n", attr);
            } else {
                printf("movsd %%xmm0, USER_%s\n", name);
            }
        } else {
            fprintf(stderr, "SETTING LIGHT\n");
            if(attr != 0){
                printf("call set_light_%s\n", attr);
            } else {
                printf("movsd %%xmm0, USER_%s\n", name);
            }
        }
        if(!isSemi()){
            terminate();
        }
        getNextToken();
    } else {
        fprintf(stderr, "FAILURE: %s\n", token);
        getNextToken();
    }
}
void sequence(){
    if(!isLeftCurl()){
        terminate();
    }
    getNextToken();
    while(!isRightCurl()){
        statement();
    }
    getNextToken();
}
int main(int argc, char** argv){
    printf(".text\n");
    printf(".global main\n");
    getNextToken();
    while(token[0] != 0){
        if(isInit()){
            printf("main:\n");
            printf("push %%rbp\n");
            printf("mov (%%rsi), %%rsi\n");
            printf("mov %%rsi, FILENAME\n");
            printf("call startup\n");
            getNextToken();
            sequence();
            printf("call initTextures\n");
            printf("mov TIME_START, %%rax\n");
            printf("mov %%rax, TIME_START\n");
            printf("renderloop:\n");
            printf("mov TIME, %%rdi\n");
            printf("call convertLLUtoDouble\n");
            printf("movsd %%xmm0, TIME_DOUBLE\n");
            printf("call frames\n");
            printf("mov TIME, %%rdi\n");
            printf("call render\n");
            printf("mov TIME, %%rax\n");
            printf("mov %%rax, %%rdx\n");
            printf("inc %%rdx\n");
            printf("mov %%rdx, TIME\n");
            printf("mov $0, %%rbx\n");
            printf("mov %%rbx, LINE_NUM\n");
            printf("mov TIME_END, %%rbx\n");
            printf("cmp %%rax, %%rbx\n");
            printf("jnz renderloop\n");
            printf("pop %%rbp\n");
            printf("mov $0, %%rax\n");
            printf("ret\n");
        } else if(isShape()){
            fprintf(stderr, "Shape Block");
            getNextToken();
            char* name = strdup(token);
            getNextToken();
            if(!isLeftCurl()){
                terminate();
            }
            getNextToken();
            if(isMath()){
                addToTypes(name, MATH);
                printf("MATH_%s:\n", name);
                printf("push %%rbp\n");
                printf("mov %%rsp, %%rbp\n");
                pushXMM(0);
                pushXMM(1);
                pushXMM(2);
                fprintf(stderr, "Math Shape\n");
                getNextToken();
                if(!isRet()){
                    terminate();
                }
                getNextToken();
                isXYZexpression = 1;
                expression();
                isXYZexpression = 0;
                if(!isSemi()){
                    terminate();
                }
                getNextToken();
                if(!isRightCurl()){
                    terminate();
                }
                getNextToken();
                popXMM(2);
                popXMM(1);
                popXMM(1);
                printf("pop %%rbp\n");
                printf("ret\n");
            }

        } else if(isTexture()){
            fprintf(stderr, "Texture Block\n");
            getNextToken();
            char* name = strdup(token);
            printf("TEXTURE_%s:\n", name);
            printf("push %%rbp\n");
            printf("mov %%rdi, %%r9\n");
            printf("mov %%rsp, %%rbp\n");
            getNextToken();
            if(!isLeftCurl()){
                terminate();
            }
            pushXMM(0);
            pushXMM(1);
            pushXMM(2);
            isXYZexpression = 1;
            isRGBexpression = 1;
            sequence();
            isRGBexpression = 0;
            isXYZexpression = 0;
            popXMM(2);
            popXMM(1);
            popXMM(1);
            printf("pop %%rbp\n");
            printf("ret\n");
        } else if(isFrames()){
            fprintf(stderr, "FRAMES\n");
            printf("frames:\n");
            printf("push %%rbp\n");
            printf("mov %%rsp, %%rbp\n");
            getNextToken();
            if(!isLeftSquare()){
                terminate();
            }
            getNextToken();
            if(!isInt()){
                terminate();
            }
            T_START = strtoull(token, NULL, 10);
            getNextToken();
            if(!isComma()){
                terminate();
            }
            getNextToken();
            if(!isInt()){
                terminate();
            }
            T_END = strtoull(token, NULL, 10);
            getNextToken();
            if(!isRightSquare()){
                terminate();
            }
            getNextToken();
            isTexpression = 1;
            sequence();
            isTexpression = 0;
            printf("pop %%rbp\n");
            printf("ret\n");
            fprintf(stderr, "END FRAMES\n");
        }
        if(token[0] == 0){
            break;
        }
        printf("\n\n\n");
    }
    printf("initTextures:\n");
    printf("push %%rbp\n");
    TTNode* currentTex = textures;
    while(currentTex != 0){
        printf("mov $DATA_%s, %%rdi\n", (currentTex -> name));
        printf("call readImage\n");
        currentTex = currentTex -> next;
    }
    printf("pop %%rbp\n");
    printf("ret\n");
    printf("\n\n\n.global getRowNum\n");
    printf("getRowNum:\n");
    printf("push %%rbp\n");
    printf("mov LINE_NUM, %%rax\n");
    printf("getRowNumLoop:\n");
    printf("mov %%rax, %%rcx\n");
    printf("inc %%rcx\n");
    printf("cmpxchgq %%rcx, LINE_NUM\n");
    printf("jnz getRowNumLoop\n");
    printf("pop %%rbp\n");
    printf("ret\n");
    printf("\n\n\n.data\n");
    printf(".global FILENAME\n");
    printf("FILENAME: .quad 0\n");
    printf("LINE_NUM: .quad 0\n");
    printf("TIME: .quad 0\n");
    printf("TIME_DOUBLE: .quad 0\n");
    printf("TIME_START: .quad %lu\n", T_START);
    printf("TIME_END: .quad %lu\n", T_END);
    STNode* currentData = ST;
    while(currentData != 0){
        printf("USER_%s: .quad 0\n", currentData -> name);
        currentData = currentData -> next;
    }
    currentTex = textures;
    while(currentTex != 0){
        char* texname = (currentTex -> name);
        printf("DATA_%s:\n", texname);
        int index = 0;
        while(texname[index] != 0){
            printf(".byte %d\n", texname[index]);
            index++;
        }
        printf(".byte 0\n");
        currentTex = currentTex -> next;
    }
    //printf("DOUBLE_STORE: .quad 0\n");
    return 0;
}
