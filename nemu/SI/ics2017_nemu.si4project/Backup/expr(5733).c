#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

enum {
  TK_NOTYPE = 256, 
  TK_EQ,
  TK_NUM,
  TK_VAR,

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},      // spaces
  {"\\+", '+'},           // plus
  {"==", TK_EQ},          // equal
  {"-", '-'},             // subtract
  {"\\*", '*'},           // multiply
  {"/", '/'},             // divide
  {"[0-9]+", TK_NUM},     // number
  {"[a-zA-Z]+", TK_VAR},  // variable
  {",", ','},             // comma
  {"[.(.]", '('},         // open parenthesis
  {"[.).]", ')'}         // close parenthesis
  /*
  {"[.[.]", '['},         // open bracket
  {"[.].]", ']'},         // close bracket
  {"[.{.]", '{'},         // open brace
  {"[.}.]", '}'}          // close brace
  */
  
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  Log("e = %s", e);
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        Assert(substr_len < 32, "strlen is larger than 32.");
        switch (rules[i].token_type) {
          case '+':
          {
            tokens[nr_token].type = '+';
            memcpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          }
          case '-':
          {
            tokens[nr_token].type = '-';
            memcpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          }
          case '*':
          {
            tokens[nr_token].type = '*';
            memcpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          }
          case '/':
          {
            tokens[nr_token].type = '/';
            memcpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          }
          case ',':
          {
            tokens[nr_token].type = ',';
            memcpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          }
          case '(':
          {
            tokens[nr_token].type = '(';
            memcpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          }
          case ')':
          {
            tokens[nr_token].type = ')';
            memcpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          }
          case TK_NOTYPE:
          {
            /* spaces do not record */
            break;
          }
          case TK_EQ:
          {
            tokens[nr_token].type = TK_EQ;
            memcpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          }
          case TK_NUM:
          {
            tokens[nr_token].type = TK_NUM;
            memcpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          }
          case TK_VAR:
          {
            tokens[nr_token].type = TK_VAR;
            memcpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          }
  
          default:
          {
            TODO();
          }
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(int p, int q){
  if (('(' == tokens[p].type) && (')' == tokens[q].type))
    return true;

  return false;
}

int is_parenthesis(char op){
  if ('(' == op)
    return 1;
  else if (')' == op)
    return -1;
  else
    return 0;
}

bool is_mathematic_operator(char op){
  bool ret;
  switch(op){
    case '+':
    case '-':
    case '*':
    case '/':
      ret = true;
    
    default:
      Log("%c is not oprator!\r\n", op);
      ret = false;
  }
  return ret;
}

static inline bool check_opretor_is_add_or_subtract(char op){
  if (('+' == op) || ('-' == op)){
    return true;
  }
  else{
    return false;
  }
}
    
static inline bool check_opretor_is_multiply_or_divide(char op){
  if (('*' == op) || ('/' == op)){
    return true;
  }
  else{
    return false;
  }
}
  
enum{
  op_cmp_small = 0,
  op_cmp_equal,
  op_cmp_big,
};

int operator_compare(char op1, char op2){
  if (true == check_opretor_is_add_or_subtract(op1)){
    if (true == check_opretor_is_add_or_subtract(op2)){
      return op_cmp_equal;
    }
    else if (true == check_opretor_is_multiply_or_divide(op2)){
      return op_cmp_small;
    }
    else{
      return -1;
    }
  }
  else if (true == check_opretor_is_multiply_or_divide(op1)){
    if (true == check_opretor_is_add_or_subtract(op2)){
      return op_cmp_big;
    }
    else if (true == check_opretor_is_multiply_or_divide(op2)){
      return op_cmp_equal;
    }
    else{
      return -1;
    }
  }
  else{
    return -1;
  }
    
}

int eval(int start, int end){
  if (start > end){
    return -1;
  }
  else if (start == end){
    return atoi(tokens[start].str);
  }
  else if (true == check_parentheses(start, end)){
    return eval(start + 1, end - 1);
  }
  else{
    /* find dominant operator */
    int ret, i, val1, val2;
    int op = -1;
    int parenthesis_count = 0;
    for (i = start; i <= end; i++){
      Log("parenthesis_count = %d.", parenthesis_count);
      /* 1、判断当前字符是否是在括号里面 */
      Assert(parenthesis_count >= 0, "parenthesis count is err!");
      if (parenthesis_count > 0){
        parenthesis_count += is_parenthesis(*(tokens[i].str));
        continue;
      }
      else {
        parenthesis_count += is_parenthesis(*(tokens[i].str));
        /* 2、判断字符是否是算术运算符 */
        if (true == is_mathematic_operator(*(tokens[i].str))){
          /* 判断op是否有值 */
          if (-1 == op){
            op = i;
            continue;
          }
          /* 3、判断算数运算符的优先级 */
          else{
            ret = operator_compare(*(tokens[op].str), *(tokens[i].str));
            if (op_cmp_small == ret){
              continue;
            }
            else if ((op_cmp_equal == ret) || (op_cmp_big == ret)){
              op = i;
            }
            else{
              return -1;
            }
          }
        }
        else{
          continue;
        }
      }
    }

    if (-1 == op)
      return -1;
    val1 = eval(start, op-1);
    val2 = eval(op + 1, end);

    switch(*(tokens[op].str)){
      case '+':
        return val1 + val2;
      case '-':
        return val1 - val2;
      case '*':
        return val1 * val2;
      case '/':
        return val1 / val2;
      default:
        assert(0);
    }



    
    return 0;
  }

}


uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  
  /* TODO: Insert codes to evaluate the expression. */
  for (int i = 0; i < nr_token; i ++){
    printf("token[%d].type = %d, tokens[%d].str = %s\n", i, tokens[i].type, i, tokens[i].str);
  }

  Log("xxx = %d", eval(0, nr_token));
  
  *success = true;
  TODO();
  Log("expr end!");
  return 0;
}
