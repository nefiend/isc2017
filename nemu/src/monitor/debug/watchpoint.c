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

/* 从free_链表中返回一个空闲的监视点结构 */
WP* new_wp(){
    WP *pWPNode;

    Assert(NULL != free_, "free_ is empty!");

    pWPNode = free_;
    free_ = free_->next;

    return pWPNode;
}
/* 将wp归还到free_链表当中 */
void free_wp(WP *wp){
    WP *pWPNode;

    Assert(NULL != wp, "Parameter Err!");

    pWPNode = free_;
    free_ = wp;
    free_->next = pWPNode;

    return;
}
