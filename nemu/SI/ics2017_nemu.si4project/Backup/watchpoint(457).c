#include "monitor/watchpoint.h"
#include "monitor/expr.h"


#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */


/*********************************************************************
 * Function Name  : new_wp
 * Author         : Nefiend
 * Create Date    : 2018-9-3
 * Description    : 从free_链表中返回一个空闲的监视点结构,head链表中新增一各监视点结构
 * Input          : None
 * return         : WP*
 *********************************************************************/
WP* new_wp(){
    WP *pWPNode;
    WP *pCur;

    Assert(NULL != free_, "free_ is empty!");

    pWPNode = free_;
    free_ = free_->next;

    pCur = head;
    if (NULL == pCur)
    {
        pCur = pWPNode;
    }
    else
    {
        while(NULL != pCur->next)
        {
            pCur = pCur->next;
        }
        pCur->next = pWPNode;
    }


    return pWPNode;
}

/*********************************************************************
 * Function Name  : free_wp
 * Author         : Nefiend
 * Create Date    : 2018-9-3
 * Description    : 将wp归还到free_链表当中，并将head链表中相应的监视点结构删除
 * Input          : WP *wp  
 * return         : 
 *********************************************************************/
void free_wp(WP *wp){
    WP *pWPNode;

    Assert(NULL != wp, "Parameter Err!");

    pWPNode = free_;
    free_ = wp;
    free_->next = pWPNode;

    return;
}

void check_all_watchpoints()
{
    head = wp_pool;
}

