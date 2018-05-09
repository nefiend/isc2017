#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

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
  {"[.(.]", '('},
  {"[.).]", ')'}
  
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
        Assert(nr_token < 32);
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

  return true;
}

int eval(int start, int end, int *result){
  if (start > end){
    return -1;
  }
  else if (start == end){
    
  }
  else if (true == check_parentheses(start, end)){

  }
  else{

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
  *success = true;
  TODO();
  Log("expr end!");
  return 0;
}
