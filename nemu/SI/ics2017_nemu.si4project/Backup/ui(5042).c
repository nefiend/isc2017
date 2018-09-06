#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_si(char *args){
  if(NULL == args){
    cpu_exec(1);
  }
  else{
    cpu_exec(atoi(args));
  }
  return 0;
}

static int cmd_info(char *args){
  if(NULL == args){
    printf("Please type r or w for params!\n");}
  else if(strcmp(args, "r") == 0){
    char *reg[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", 
    for (uint32_t i = 0; i < 8; i++){
      printf("%s \t %p \t 0x%x\n",  reg[i], &reg_l(i), reg_l(i));
      //printf("%s \t 0x%x\n",  reg[i], reg_w(i));
      //printf("%s \t 0x%x\n",  reg[i], reg_b(i));
    }
    printf("rip \t %p \t 0x%x\n", &cpu.eip, cpu.eip);
  }
  else if(strcmp(args, "w") == 0){

  }
  else{
    printf("Unknown command '%s'\n", args);
  }
  return 0;
}

/*********************************************************************
 * Function Name  : cmd_p
 * Author         : Nefiend
 * Create Date    : 2018-8-8
 * Description    : 计算表达式
 * Input          : char *args  
 * return         : static
 *********************************************************************/
static int cmd_p(char *args){
    bool success;
    uint32_t uiRv;
    uiRv = expr(args, &success);
    if (true == success){
        printf("%s = 0x%x\r\n", args, uiRv);
        return 0;
    }
    else{
        assert(0);
        return -1;
    }
}

/*********************************************************************
 * Function Name  : cmd_x
 * Author         : Nefiend
 * Create Date    : 2018-7-26
 * Description    : 扫描内存接口
 * Input          : char *args  
 * return         : static
 *********************************************************************/
static int cmd_x(char *args){
    bool bSuccess;
    uint32_t uiRv;
    char *cExpression;
    uint32_t uiIdx;

    /* 获取第一个参数，即连续输出N个4字节 */
    char *arg = strtok(args, " ");
    Log("arg = %d\r\n", atoi(arg));

    if (NULL == arg){
        panic("param error!");
        return -1;
    }
    else{
        cExpression = arg + strlen(arg) + 1;
    }

    uiRv = expr(cExpression, &bSuccess);
    Log("uiResult = %u\r\n", uiRv);

    Log("result = %d\r\n", *(int *)guest_to_host(uiRv));

    uint32_t *p;
    //p = (int *)guest_to_host(0x100000);
    p = (uint32_t *)guest_to_host(uiRv);
    for (uiIdx = 0; uiIdx < atoi(arg); uiIdx++){
        printf("%p:0x%x\r\n", (p + uiIdx), *(p + uiIdx));
    }

    return 0;
}

/*********************************************************************
 * Function Name  : cmd_w
 * Author         : Nefiend
 * Create Date    : 2018-8-6
 * Description    : 设置监视点
 * Input          : char *args      输入表达式字符串
 * return         : static
 *********************************************************************/
static int cmd_w(char *args){
    WP *pWPNode;
    bool bSuccess;
    //uint32_t uiRv;

    pWPNode = new_wp();
    pWPNode->cExpr = args;
    pWPNode->uiExprVal = expr(pWPNode->cExpr, &bSuccess);
    pWPNode->bIsChanged = false;
    

    return 0;
}

static int cmd_d(char *args)
{
  return 0;
}

static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  {
   "When N is not Given, the default is 1", cmd_si},
  {"info", "info {r | w}. Print register status or watch point info", cmd_info},
  {"p", "p EXPR. Find the value of EXPR", cmd_p},
  {"x", "x N EXPR. Find the value of EXPR and use the value as start memory address, output N DWORD continuously", cmd_x},
  {"w", 
  {"d", "d N. Delete the #N watchpoint.", cmd_d},
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%-4s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%-4s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    Log("str = %s", str);
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    Log("cmd = %s", cmd);
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }
	Log("args = %s", args);

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}