#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKENS 10000
#define MAX_LINE_LENGTH 1000
int num_tokens = 0,num_vars =0;
//current token number
int cur =0; 
int tree_cur =0;
char* tokens[MAX_TOKENS]; 
char* vars[MAX_TOKENS];  // names of var
char tree[10000];
char* varNames[50];
int vals[50];
char converted_string[20];

// Node struct for syntax tree
typedef struct Node {
    char *name;
    struct Node* children[20];
} Node;

Node* new_node(char* value){
    Node *node = (Node *)malloc(sizeof(Node));
    node->name = value;
    for(int i=0;i<20;i++) node->children[i] = 0;
    return node;
}

//function declarations
Node* program();
Node* statement_list();
Node* declaration();
Node* statement();
Node* read();
Node* write();
Node* assignment();
Node* for_loop();
Node* expression(int limit);
Node* expression_low(int limit);
Node* expression_mid(int limit);
Node* expression_top(int limit);
Node* operator_exp_low();
Node* operator_exp_mid();
Node* operator_exp_top();
Node* variable();
Node* integer_constant();
Node* variable_list();
Node* terminal(char* name);

Node* terminal(char* name){
    Node* n = new_node(name);
    cur++;
    return n;
}

// tokenizer
void tokenize(char* filename) {
    FILE* in_file = fopen(filename, "r");
    
    char line[MAX_TOKENS];
    char token[100];
    int j=0;
    
    while (fgets(line, MAX_LINE_LENGTH, in_file)) {
        for (int i = 0; i < strlen(line); ++i) {
            if(line[i] == '(' || line[i] == ')' || line[i] == ','  || line[i] == ';' || line[i]=='=' || line[i] == '>' || line[i] == '+' || line[i] == '*' || line[i] == '/' || line[i] == '-')
            {
                if(!token[0]){
                    token[0] = line[i];
                    if(line[i]=='=' && i+1<strlen(line) && line[i+1]=='=') {
                        token[1] = line[i+1];i++;
                    }
                    tokens[num_tokens] = malloc(sizeof(token));
                    strcpy(tokens[num_tokens], token);
                    memset(token, 0, strlen(token));
                    num_tokens++;
                    j=0;
                    continue;
                }
                tokens[num_tokens] = malloc(sizeof(token));
                strcpy(tokens[num_tokens], token);
                memset(token, 0, strlen(token));
                token[0] = line[i];
                if(line[i]=='=' && i+1<strlen(line) && line[i+1]=='=') {
                    token[1] = line[i+1];
                    i++;
                }
                tokens[num_tokens+1] = malloc(sizeof(token));
                strcpy(tokens[num_tokens+1], token);
                memset(token, 0, strlen(token));
                num_tokens+=2; 
                j=0;	    
            }	    
            else if (line[i] == ' ' || line[i] == '\t' || line[i] == 10 /* ASCII 10 = Line Feed */ || line[i] == '\n')
            {
                if(!token[0]) continue;
                tokens[num_tokens] = malloc(sizeof(token));
                strcpy(tokens[num_tokens], token);  //push token into tokens array
                memset(token, 0, strlen(token));
                num_tokens++; 
                j=0; 
            }
            else
            {
                token[j]=line[i];
                j++; 
            }
        }
    } 
     
}


void error(char* message){
    printf("%s\n", message);
    exit(1);
}

Node* program(){
    Node* root = new_node("program");
    int cur_child = 0;

    if(!strcmp(tokens[cur],"int")){
        Node* dec = declaration();
        root->children[cur_child++] = dec;
        root->children[cur_child++] = terminal(";");
        if(cur<num_tokens){
            root->children[cur_child++] = statement_list();
        }
    }
    else{
        root->children[cur_child++] = statement_list();
    }

    return root;
}

Node* read(){
    Node* root = new_node("read");
    int cur_child = 0;
    root->children[cur_child++] = terminal("read");
    root->children[cur_child++] = variable();
    return root;
}

Node* write(){
    Node* root = new_node("write");
    int cur_child = 0;
    root->children[cur_child++] = terminal("write");
    char ch = tokens[cur][0];
    if(ch>='0' && ch <= '9')
    {
        root->children[cur_child++] = integer_constant();
    }
    else
    {
        root->children[cur_child++] = variable();
    }
    return root;
}

Node* assignment()
{
    Node* root = new_node("assignment");
    int cur_child = 0;
    root->children[cur_child++] = variable();
    root->children[cur_child++] = terminal("=");
    int temp_cur = cur;
    int numBrack =0;
    while(strcmp(tokens[temp_cur],";"))
    {
        if((!numBrack && !strcmp(tokens[temp_cur],")"))) {
            break;
        }
        numBrack += (!strcmp(tokens[temp_cur],"("));
        numBrack -= (!strcmp(tokens[temp_cur],")"));
        temp_cur++;
    }
    root->children[cur_child++] = expression(temp_cur);
    return root;
}


Node* declaration(){
    Node* root = new_node("declaration");
    int cur_child = 0;
    root->children[cur_child++] = terminal("int");
    root->children[cur_child++] = variable_list();
    return root;
}

Node* for_loop()
{
    Node* root = new_node("for_loop");
    int cur_child = 0;
    root->children[cur_child++] = terminal("for");
    root->children[cur_child++] = terminal("(");
    root->children[cur_child++] = assignment();
    root->children[cur_child++] = terminal(";");
    int temp_cur = cur;
    while(strcmp(tokens[temp_cur],";"))
    {
        temp_cur++;
    }
    root->children[cur_child++] = expression(temp_cur);
    root->children[cur_child++] = terminal(";");
    root->children[cur_child++] = assignment();
    root->children[cur_child++] = terminal(")");
    root->children[cur_child++] = terminal("{");
    root->children[cur_child++] = statement_list();
    root->children[cur_child++] = terminal("}");
    return root;
}

Node* variable_list(){
    Node* root = new_node("variable_list");
    vars[num_vars++] = tokens[cur];
    int cur_child =0;
    root->children[cur_child++] = variable();
    if(!strcmp(tokens[cur],",")){
        root->children[cur_child++] = terminal(",");
        root->children[cur_child++] = variable_list();
    }
    return root;
}

Node* variable(){
    if(!strcmp(tokens[cur], "read") || !strcmp(tokens[cur], "write") || !strcmp(tokens[cur], "for") || !strcmp(tokens[cur], "int")){
        error("Variable names cannot be keywords.");
    }
    
    Node* root = new_node("variable");
    root->children[0] = terminal(tokens[cur]);
    return root;
}

Node* integer_constant(){
    Node* root = new_node("integer_constant");
    root->children[0] = terminal(tokens[cur]);
    return root;
}

Node* statement_list(){
    Node* root = new_node("statement_list");
    int childNumber = 0;
    Node* firstStatement = statement();
    root->children[childNumber] =  firstStatement;
    childNumber++;
    if(!strcmp(tokens[cur],";")){
        root->children[childNumber] = terminal(";");
        childNumber++;
    }
    if(cur == num_tokens || !strcmp(tokens[cur],"}")) return root;
    else{
        Node* second = statement_list();
        root->children[childNumber] = second;
        childNumber++;
        return root;        
    }   
}

Node* statement(){
    Node* root = new_node("statement");
    if(!strcmp(tokens[cur],"read")){
        Node* child = read();
        root->children[0] = child;
        return root;
    }
    else if(!strcmp(tokens[cur],"write")){
        Node* child = write();
        root->children[0] = child;
        return root;
    }
    else if(!strcmp(tokens[cur],"for")){
        Node* child = for_loop();
        root->children[0] = child;
        return root;
    }
    else{
        Node* child = assignment();
        root->children[0] = child;
        return root;
    }
}

Node* operator_exp_low(){
    Node* root = new_node("operator_exp_low");
    root->children[0] = terminal(tokens[cur]);
    return root;
}
Node* operator_exp_mid(){
    Node* root = new_node("operator_exp_mid");
    root->children[0] = terminal(tokens[cur]);
    return root;
}
Node* operator_exp_top(){
    Node* root = new_node("operator_exp_top");
    root->children[0] = terminal(tokens[cur]);
    return root;
}

Node* expression(int limit){
    Node* root = new_node("expression");
    int numBrac = 0;
    int initialCur = cur;
    int last=-1;
    while(strcmp(tokens[cur],";") && cur<limit){
        if(!strcmp(tokens[cur],"(")) numBrac++;
        if(!strcmp(tokens[cur],")")) numBrac--;
        if((!strcmp(tokens[cur],">") || !strcmp(tokens[cur],"==")) && numBrac == 0)last = cur;
        cur++;
    }
    cur = initialCur;
    if(last==-1){
        root->children[0] = expression_low(limit);
        return root;
    }
    root->children[0] = expression(last);
    root->children[1] = terminal(tokens[last]);
    root->children[2] = expression_low(limit);
    return root;
}

Node* expression_low(int limit){
    Node* root = new_node("expression_low");
    int numBrac = 0;
    int initialCur = cur;
    int last=-1;
    while(strcmp(tokens[cur],";") && cur<limit){
        if(!strcmp(tokens[cur],"(")) numBrac++;
        if(!strcmp(tokens[cur],")")) numBrac--;
        if((!strcmp(tokens[cur],"+") || !strcmp(tokens[cur],"-")) && numBrac == 0)last = cur;
        cur++;
    }
    cur = initialCur;
    if(last==-1){
        root->children[0] = expression_mid(limit);
        return root;
    }
    root->children[0] = expression_low(last);
    root->children[1] = terminal(tokens[last]);
    root->children[2] = expression_mid(limit);
    return root;
}

Node* expression_mid(int limit){
    Node* root = new_node("expression_mid");
    int numBrac = 0;
    int initialCur = cur;
    int last=-1;
    while(strcmp(tokens[cur],";") && cur<limit){
        if(!strcmp(tokens[cur],"(")) numBrac++;
        if(!strcmp(tokens[cur],")")) numBrac--;
        if((!strcmp(tokens[cur],"*") || !strcmp(tokens[cur],"/")) && numBrac == 0)last = cur;
        cur++;
    }
    cur = initialCur;
    if(last==-1){
        root->children[0] = expression_top(limit);
        return root;
    }
    root->children[0] = expression_mid(last);
    root->children[1] = terminal(tokens[last]);
    root->children[2] = expression_top(limit);
    return root;
}

Node* expression_top(int limit){
    Node* root = new_node("expression_top");
    if(!strcmp(tokens[cur],"(")){
        root->children[0] = terminal("(");
        root->children[1] = expression(limit);
        root->children[2] = terminal(")");
    }
    else if (tokens[cur][0]>='0' && tokens[cur][0]<='9'){
        root->children[0] = integer_constant();
    }
    else 
        root->children[0] = variable();

    return root;  
}

void traverse_tree(Node* n){
    tree[tree_cur++] = '[';
    int j =0;
    while(n->name[j]){
        tree[tree_cur] = n->name[j];
        tree_cur++;
        j++;
    } 
    for(int child =0;n->children[child];child++){
        tree[tree_cur++] = ' ';
        traverse_tree(n->children[child]);
    }
    tree[tree_cur++] = ']';
}

void convertToString(int num){;
    if(num<0) error("Overflow occurred");
    memset(converted_string,0,sizeof(converted_string));
    if(num==0){
        converted_string[0] = '0';
        return;
    }
    int digits = 0, num2 = num;
    while(num2){
        digits++;
        num2/=10;
    }
    
    num2 = num;
    int i=digits-1;
    while(num2){
        char digit = '0'+(num2%10);
        converted_string[i] = digit;
        num2/=10;
        i--;
    }
}

int convertToInt(char* s){
    int p =1;
    int n = strlen(s);
    int ans =0;
    for(int i=n-1;i>=0;i--){
        ans += (s[i]-'0')*p;
        p*=10;
    }
    return ans;
}

int searchString(char* str){
    if(str[0]>='0' && str[0]<='9') return -1;
    int i = 0;
    while(varNames[i]){
        if(!strcmp(varNames[i],str)) return i;
        i++;
    }
    error("Variable undeclared");
}

char* evaluateExpression(int st, int end){
    char* buff[MAX_TOKENS], *buff1[MAX_TOKENS];
    memset(buff, 0, sizeof(buff));
    memset(buff1, 0, sizeof(buff1));
    int i = st,start=0, j=0;
    int bracketCount = 0;
    while(i<end){
        if(!strcmp(tokens[i], "(") && bracketCount == 0){
            bracketCount++;
            start = i+1;
        }
        else if(!strcmp(tokens[i], "(") && bracketCount != 0){
            bracketCount++;
        }
        else if(!strcmp(tokens[i], ")") && bracketCount == 1){
            //buff[j] = evaluateExpression(start, i);
            strcpy(buff[j],evaluateExpression(start, i));
            bracketCount--;
            j++;
        }
        else if(!strcmp(tokens[i], ")") && bracketCount != 1){
            bracketCount--;
        }
        else if(bracketCount==0){
            buff[j] = malloc(sizeof(tokens[i]));
            strcpy(buff[j], tokens[i]);
            j++;
        }
        i++;
    }

    i=0;j=0;

    while(buff[i]){
        if(!strcmp(buff[i],"*")){
            int val1 = searchString(buff1[j-1]), val2 = searchString(buff[i+1]);
            if(val1==-1) val1 = convertToInt(buff1[j-1]);
            else val1 = vals[val1];
            if(val2==-1) val2 = convertToInt(buff[i+1]);
            else val2 = vals[val2];
            convertToString(val1*val2);
            strcpy(buff1[j-1],converted_string);
            i++;
        }
        else if(!strcmp(buff[i],"/")){
            int val1 = searchString(buff1[j-1]), val2 = searchString(buff[i+1]);
            if(val1==-1) val1 = convertToInt(buff1[j-1]);
            else val1 = vals[val1];
            if(val2==-1) val2 = convertToInt(buff[i+1]);
            else val2 = vals[val2];
            int cur = val1/val2;
            convertToString(cur);
            strcpy(buff1[j-1],converted_string);
            i++;
        }
        else{
            buff1[j] = malloc(sizeof(buff[i]));
            strcpy(buff1[j], buff[i]);
            j++;
        }
        i++;
    }

    i=0;j=0;
    memset(buff, 0, sizeof(buff));

    while(buff1[i]){
        if(!strcmp(buff1[i],"+")){
            int val1 = searchString(buff[j-1]), val2 = searchString(buff1[i+1]);
            if(val1==-1) val1 = convertToInt(buff[j-1]);
            else val1 = vals[val1];
            if(val2==-1) val2 = convertToInt(buff1[i+1]);
            else val2 = vals[val2];
            convertToString(val1+val2);
            strcpy(buff[j-1],converted_string);
            
            i++;
        }
        else if(!strcmp(buff1[i],"-")){
            int val1 = searchString(buff[j-1]), val2 = searchString(buff1[i+1]);
            if(val1==-1) val1 = convertToInt(buff[j-1]);
            else val1 = vals[val1];
            if(val2==-1) val2 = convertToInt(buff1[i+1]);
            else val2 = vals[val2];
            convertToString(val1-val2);
            strcpy(buff[j-1],converted_string);
            i++;
        }
        else{
            buff[j] = malloc(sizeof(buff1[i]));
            strcpy(buff[j], buff1[i]);
            j++;
        }
        i++;
    }

    memset(buff1, 0, sizeof(buff1));

    i=0;j=0;

    while(buff[i]){
        if(!strcmp(buff[i],"==")){
            int val1 = searchString(buff1[j-1]), val2 = searchString(buff[i+1]);
            if(val1==-1) val1 = convertToInt(buff1[j-1]);
            else val1 = vals[val1];
            if(val2==-1) val2 = convertToInt(buff[i+1]);
            else val2 = vals[val2];
            convertToString(val1==val2);
            strcpy(buff1[j-1],converted_string);
            i++;
        }
        else if(!strcmp(buff[i],">")){
            int val1 = searchString(buff1[j-1]), val2 = searchString(buff[i+1]);
            if(val1==-1) val1 = convertToInt(buff1[j-1]);
            else val1 = vals[val1];
            if(val2==-1) val2 = convertToInt(buff[i+1]);
            else val2 = vals[val2];
            convertToString(val1>val2);
            strcpy(buff1[j-1],converted_string);
            i++;
        }
        else{
            buff1[j] = malloc(sizeof(buff[i]));
            strcpy(buff1[j], buff[i]);
            j++;
        }
        i++;
    }
    int fl = searchString(buff1[0]);
    if(fl!=-1){
        convertToString(vals[fl]);
        return converted_string;
    }
    else return buff1[0];
}

int simulate(int i) {
    int j=0;
    int begin = i;
    
    while(tokens[i]){
        if(!strcmp(tokens[i],"}")){
            return i; 
        }
        else if(!strcmp(tokens[i],"int")){
            i++;
            while(strcmp(tokens[i],";")){
                if(!strcmp(tokens[i],",")){
                    i++;
                    continue;
                }
                varNames[j] = malloc(sizeof(tokens[i]));
                strcpy(varNames[j], tokens[i]);
                i++;j++;
            }
            i++;
        }
        else if(!strcmp(tokens[i],"read")){
            i++;
            int temp;
            scanf("%d", &temp);
            vals[searchString(tokens[i])] = temp;
            i+=2;
        }
        else if(!strcmp(tokens[i],"write")){
            i++;
            int val = searchString(tokens[i]);
            if(val==-1) printf("%s",tokens[i]); //check if string is actually a variable
            else printf("%d\n",vals[val]);
            i+=2;
        }
        else if(!strcmp(tokens[i],"for")){
            i+=2;
            int id1 = i;
            i+=2;
            int e1start = i;
            while(strcmp(tokens[i],";")) i++;
            int e1end = i;
            i++;
            int e2start = i;
            while(strcmp(tokens[i],";")) i++;
            int e2end = i;
            i++;
            int id2 = i;
            i+=2;
            int e3start = i;
            while(strcmp(tokens[i],")")) i++;
            int e3end = i;
            i+=2;
            int progStart = i;
            vals[searchString(tokens[id1])] = convertToInt(evaluateExpression(e1start, e1end));
            int fl = 0;
            while(convertToInt(evaluateExpression(e2start, e2end))){
                fl++;
                i = simulate(progStart);
                vals[searchString(tokens[id2])] = convertToInt(evaluateExpression(e3start, e3end));
            }
            if(!fl){
                while(strcmp(tokens[i],"}")) i++;
                i+=2;
            }else i+=2;

        }
        else{ //assignment
            int val = searchString(tokens[i]);
            i+=2;
            int start = i;
            while(strcmp(tokens[i],";")) i++;
            vals[val] = convertToInt(evaluateExpression(start,i));
            i++;
        }
    }

    return i;
     
}

int main(){
    tokenize("input.txt");
    Node* root = program();
    traverse_tree(root);  
    for(int i=0;tree[i];i++){
        printf("%c",tree[i]);
    }
    printf("\n");
    simulate(0);
    return 0;
}