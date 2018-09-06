#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

#define EXPR_LEN 32

typedef struct watchpoint 
{
    int NO;
    struct watchpoint *next;

    /* TODO: Add more members if necessary */
    char acExpr[EXPR_LEN];
    uint32_t  uiExprVal;
    bool bIsChanged;

} WP;


/*********************************************************************
 * Function Name  : new_wp
 * Author         : Nefiend
 * Create Date    : 2018-9-3
 * Description    : 从free_链表中返回一个空闲的监视点结构,head链表中新增一个监视点结构
 * Input          : None
 * return         : WP*
 *********************************************************************/
WP* new_wp();

/*********************************************************************
 * Function Name  : free_wp
 * Author         : Nefiend
 * Create Date    : 2018-9-3
 * Description    : 将wp归还到free_链表当中，并将head链表中相应的监视点结构删除
 * Input          : WP *wp  
 * return         : 
 *********************************************************************/
void free_wp(WP *wp);

/*********************************************************************
 * Function Name  : check_all_watchpoints
 * Author         : Nefiend
 * Create Date    : 2018-9-4
 * Description    : 检查所有的监视点的值
 * Input          : None
 * return         : 
 *********************************************************************/
bool check_all_watchpoints();

void display_active_watchpoints();

void delete_watchpoint(int Num);



#endif
