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
    free_ = free_->next;
    pWPNode->next = NULL;

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
    memset(wp->acExpr, 0 ,EXPR_LEN);
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

    /* 遍历所有的监视点，发生变化的结点，标记为置为true */
    bIsChanged = false;
    pCur = head;
    while (NULL != pCur){
        uiRv = expr(pCur->acExpr, &bSuccess);
        if (uiRv != pCur->uiExprVal){
            pCur->bIsChanged = true;
            bIsChanged = true;
            printf("Watchpoint %d: %s\r\n\r\n", pCur->NO, pCur->acExpr);
            printf("Old value = 0x%x \r\n", pCur->uiExprVal);
            printf("New value = 0x%x \r\n", uiRv);
        }
        pCur = pCur->next;
    }

    return bIsChanged;
}

void display_active_watchpoints(){
    WP *pCur;

    if (NULL == head){return;}
    
    printf("Num     Type           Disp Enb Address    What\r\n");
    
    pCur = head;
    while(NULL != pCur){
        printf(" %-2d  watchpoint    keep  y                %s\r\n", pCur->NO, pCur->acExpr);

        pCur = pCur->next;
    }
    
    return;
}


void delete_watchpoint(int Num){
    WP *pCur;

    if (NULL == head){return;}

    pCur = head;
    while(NULL != pCur){
        Log("Num: %d, pCur->NO: %d\r\n", Num, pCur->NO);
        
        if(Num == pCur->NO){
            free_wp(pCur);
            printf("watchpoint %d has been deleted!\r\n", pCur->NO);
            return;
        }

        pCur = pCur->next;
    }

    return;
}

