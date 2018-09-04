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
 * Description    : 从free_链表中返回一个空闲的监视点结构,head链表中新增一个监视点结构
 * Input          : None
 * return         : WP*
 *********************************************************************/
WP* new_wp(){
    WP *pWPNode;
    WP *pCur;

    Assert(NULL != free_, "free_ is empty!");

    /* 将第一个结点返回出去 */
    pWPNode = free_;
    pWPNode->next = NULL;
    free_ = free_->next;

    /* 在head链表中增加监视点 */
    if (NULL == head){
        head = pWPNode;
    }
    else{
        pCur = head;
        while(NULL != pCur->next){
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
    WP *pCur;

    Assert(NULL != wp, "Parameter Err!");
    Assert(NULL != head, "head Err!");

    /* 将head中对应的结点删除 */
    pCur = head;
    if(pCur == wp){
        wp->next = NULL;
        head = pCur->next;
    }else{
        while (pCur->next != wp && NULL != pCur->next){
            pCur = pCur->next;
        }
        
        if(NULL == pCur->next){
            assert(0);
            return;
        }
        pCur->next = wp;
        wp->next = NULL;
    }
    
    /* 将结点归还给free_，插入到最后面 */
    pCur = free_;
    while(NULL != pCur->next){
        pCur->next = wp;
    }

    /* 将wp结点内容清除 */
    wp->uiExprVal = 0;
    wp->cExpr = NULL;
    wp->bIsChanged = false;
    return;
}

/*********************************************************************
 * Function Name  : check_all_watchpoints
 * Author         : Nefiend
 * Create Date    : 2018-9-4
 * Description    : 检查所有的监视点的值
 * Input          : None
 * return         : true    发生了变化
                    false   没有发生变化
 *********************************************************************/
bool check_all_watchpoints(){
    WP *pCur;
    bool bSuccess;
    bool bIsChanged;
    uint32_t uiRv;
    
    Assert(NULL != head, "head Err!");

    bIsChanged = false;
    pCur = head;
    while (NULL != pCur){
        uiRv = expr(pCur->cExpr, &bSuccess);
        if (uiRv != pCur->uiExprVal){
            pCur->bIsChanged = true;
            bIsChanged = true;
        }
    }

    return bIsChanged;
}

