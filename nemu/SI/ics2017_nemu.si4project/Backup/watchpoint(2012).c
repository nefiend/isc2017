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


void WP_WpPoolInsertHead(WP **ppstFreeHead, WP *pstNode){
  WP *pstNodeTemp = NULL;

  Assert(NULL != ppstFreeHead, "NULL pointer!");

  pstNodeTemp = *ppstFreeHead;

  pstNode->next = ppstFreeHead;
}

WP* WP_WpPoolDeleteHead(WP **ppstWpPoolHead){
  WP *pstNodeTemp = NULL;
  
  Assert(NULL != ppstWpPoolHead, "NULL pointer!");
  
  pstNodeTemp = *ppstWpPoolHead;
  
  *ppstWpPoolHead = (*ppstWpPoolHead)->next;

  return pstNodeTemp;
} 

WP* new_wp(){
  WP *pstNewWpNode;
  
  Assert(NULL != free_, "wp_pool is empty!");

  pstNewWpNode = WP_WpPoolDeleteHead(&head);

}

void free_wp(WP *pstWpNode){
  Assert(NULL != pstWpNode, "NULL pointer!");

  WP_WpPoolInsertHead(&free_, pstWpNode);
}


