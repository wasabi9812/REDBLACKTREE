#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>




rbtree *new_rbtree(void) {

  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));     // rbtree를 위한 메모리 할당
  
  // rbtree를 위한 메모리 할당 실패 시 예외 처리
  if (p == NULL)
  {
        fprintf(stderr, "Memory allocation failed\n"); // stderr: 표준 에러 출력 스트림
        exit(EXIT_FAILURE);                            // EXIT_FAILURE: C 프로그래밍 언어에서 프로그램이 비정상적으로 종료될 때 사용되는 매크로 상수 (1)

        /*-----------------------------
        * 프로그램 종료 방법 2가지
        * -----------------------------
        * return 1: 
        *   - main 함수가 종료되면서 프로그램이 종료됩니다. 
        *   - 사용 상황: main 함수 내에서 프로그램의 종료 상태를 설정하는 데 사용됩니다.
        * exit(1) : 
        *   - 호출 시점에서 즉시 프로그램을 종료합니다. 
        *   - 현재 호출 스택의 모든 함수의 종료를 유도합니다.
        *   - 프로그램 종료 전에 `atexit`로 등록된 종료 처리 함수들이 실행됩니다.
        *   - 사용 상황: 프로그램의 어떤 위치에서도 종료 상태를 설정하고 즉시 종료시킬 때 사용됩니다.
        */

        /*-----------------------------
        * 매크로 상수와 enum 상수
        * -----------------------------
        * 매크로 상수:
        *   - <예시> #define MAX_SIZE 100;
        *   - 장점: 정의된 상수를 한 곳에서 변경하면 코드 전체에 반영된다.
        *   - 단점: 
        *       - 매크로 상수는 단순한 치환이므로, 타입 체크가 없다. 컴파일 타임에 문제가 발생할 수 있다.
        * enum 상수:
        *   - <예시> enum EnumName { CONSTANT1, CONSTANT2, ...};
        *   - 열거형 상수는 자동으로 정수 값이 부여된다. 첫 번째 상수는 `0`, 두 번째는 `1` 이렇게 계속해서 증가한다.
        */
  }

  // TODO: initialize struct if needed

  p->nil = (node_t *)calloc(1, sizeof(node_t));          // nil을 위한 메모리 할당

  // NIL 노드를 위한 메모리 할당 실패 시 예외 처리
  if (p->nil == NULL)
  {
      fprintf(stderr, "Memory allocation failed\n");
      exit(EXIT_FAILURE);
  }

  // NIL 노드 초기화
  p->nil->color = RBTREE_BLACK;
  p->nil->left =  NULL; 
  p->nil->right =  NULL;
  p->nil->parent =  NULL;                            // nil 노드의 부모를 자기 자신으로 설정 (또는 NULL을 가리기케 하는 방법도 있음)


  // 루트 노드 초기화
  p->root = p->nil;
  return p;
}


int rbtree_left_rotate(rbtree *t, node_t *x)
{
  node_t *y = x->right;                   // y에 x의 오른쪽 자식 노드 주소를 저장
  x->right = y->left;                     // x의 오른쪽 자식을 y의 왼쪽 자식으로 연결

  if (y->left != t->nil)                  // y의 왼쪽 자식이 NIL 노드가 아니라면
  {
    y->left->parent = x;                  // y의 왼쪽 자식 노드를 x와 연결
  }

  y->parent = x->parent;                  // y를 x의 부모 노드에 연결
  if (x->parent == t->nil)                // x의 부모가 NIL 노드라면 (루트 노드라면)
  {
    t->root = y;                          // 트리의 루트를 y로 변경
  }
  else if (x == x->parent->left)          // x가 부모의 왼쪽 자식 노드라면
  {
    x->parent->left = y;                  // y를 부모의 왼쪽 자식으로 연결
  }
  else                                    // x가 부모의 오른쪽 자식 노드라면
  {
    x->parent->right = y;                 // y를 부모의 오른쪽 자식으로 연결
  }

  y->left = x;                            // x를 y의 왼쪽 자식으로 연결
  x->parent = y;                          // y를 x의 부모로 연결
  
  return 0;
}

int rbtree_right_rotate(rbtree *t, node_t *x)
{
  node_t *y = x->left;                    // y에 x의 왼쪽 자식 노드 주소를 저장
  x->left = y->right;                     // x의 왼쪽 자식을 y의 오른쪽 자식으로 연결
  if (y->right != t->nil)                 // y의 오른쪽 자식이 NIL 노드가 아니라면
  {
    y->right->parent = x;                 // y의 오른쪽 자식 노드를 x와 연결
  }

  // 여기부터 아래까지는 LEFT_ROTATE와 코드 동일
  y->parent = x->parent;                             
  if (x->parent == t->nil)
  {
    t->root = y;
  }
  else if (x == x->parent->left)
  {
    x->parent->left = y;
  }
  else
  { 
    x->parent->right = y;
  }

  y->right = x;
  x->parent = y;

  return 0;
}



void rbtree_insert_fixup(rbtree *t, node_t *z)
{
  while (z->parent->color == RBTREE_RED)
  {
    // 부모가 할아버지의 왼쪽에 있을 때
    if (z->parent == z->parent->parent->left)
    {
      node_t *y = z->parent->parent->right;
      if (y->color == RBTREE_RED) // Case 1: 삼촌 y가 RED
      {
        // Case 1: Recoloring
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else
      {
        if (z == z->parent->right) // Case 2: z가 오른쪽 자식인 경우
        {
          z = z->parent;
          rbtree_left_rotate(t, z); // 왼쪽 회전으로 Case 3로 변환
        }
        // Case 3: z가 왼쪽 자식인 경우
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        rbtree_right_rotate(t, z->parent->parent);
      }
    }
    else // 부모가 할아버지의 오른쪽에 있을 때 (위와 대칭)
    {
      node_t *y = z->parent->parent->left;
      if (y->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else
      {
        if (z == z->parent->left)
        {
          z = z->parent;
          rbtree_right_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        rbtree_left_rotate(t, z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK; // 루트는 항상 BLACK
}



node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert

  node_t *z = (node_t *)calloc(1, sizeof(node_t));     // node_t 위한 메모리 할당
  printf("%p",z);
  // node_t를 위한 메모리 할당 실패 시 예외 처리
  if (z == NULL)
  {
        fprintf(stderr, "Memory allocation failed\n"); // stderr: 표준 에러 출력 스트림
        exit(EXIT_FAILURE);   
  }
  
  // 새롭게 삽입할 노드의 key 설정
  z->key = key;

  node_t *y = t->nil;
  node_t *x = t->root;

  while (x != t->nil)
  {
    y = x;                      // 반복문 첫 번째 시행 시, z의 부모 노드는 잠정적으로 루트 노드인 x
    if (z->key < x->key)
    {
      x = x->left;              // pointer를 x의 left로 변경
    }
    else                        // z의 키가 x의 키보다 크거나 또는 두 개가 같을 때
    {
      x = x->right;             // pointer를 x의 right로 변경
    }
  }

  z->parent = y;
  if (y == t->nil)
  {
    t->root = z;
  }
  else if (z->key < y->key)
  {
    y->left = z;
  }
  else 
  {
    y->right = z;
  }

  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;

  rbtree_insert_fixup(t, z);
  
  return z;
}



node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *nil = t->nil;
  node_t *cur = t->root;
  while(cur != nil) {
    if (cur->key == key) { // 검색하는 값을 찾으면
      return cur;
    } else if (cur->key > key) { // 현재 노드의 값보다 검색값이 작으면
      cur = cur->left;
    } else { // 현재 노드의 값보다 검색값이 크면
      cur = cur->right;
    }
  }
  return NULL;
}





void freeNode(node_t *node, rbtree *t) {
    if (node == t->nil) {
        return;
    }
    freeNode(node->left, t);
    freeNode(node->right, t);
    free(node);
  return;
}
void delete_rbtree(rbtree *t) {
  freeNode(t->root, t);
  free(t->nil);
  free(t);
}


node_t *rbtree_min(const rbtree *t) {
  node_t *ptr = t->root;
  // 루트에서 왼쪽 자식으로 계속 이동하여 가장 왼쪽 노드를 찾음
  while (ptr->left != t->nil) {
    ptr = ptr->left;
  }
  return ptr;  // 가장 왼쪽 노드가 최소값을 가짐
}

node_t *rbtree_max(const rbtree *t) {
  node_t *ptr = t->root;
  // 루트에서 왼쪽 자식으로 계속 이동하여 가장 왼쪽 노드를 찾음
  while (ptr->right != t->nil) {
    ptr = ptr->right;
  }
  return ptr;  // 가장 왼쪽 노드가 최소값을 가짐
}

// node_t* tree_minimum(rbtree *t, node_t *z){ // successor 찾는중에 오른쪽노드가있을때 들어가서 제일 왼쪽노드 찾으려고 만든함수
//   while(z->left != t->nil){
//     z = z->left;
//   }
//   return z;
// }


node_t *tree_minimum(const rbtree *t, node_t *sub_root){
    // TODO: implement find
    node_t *r = sub_root;
    if (r == t -> nil)
        return r;
    while (r -> left != t -> nil)
    {
        r = r -> left;
    }
    return r;
}


// node_t* tree_successor(rbtree *t ,node_t *z){
//   if(z->right != t->nil){
//     //오른쪽 서브트리가 존재하는경우
//     return tree_minimum(t, z->right);
//   }
//   node_t *y = z->parent; //오른쪽 서브트리가 없는경우
//   while(y!= t->nil && z == y->right){ //부모가 닐이 아니면서
//     z = y; // y로
//     y = y->parent; // y의 부모로    즉 z y 둘다 한단계식 위로
//   }
//   return y; // 모든루프에서 나오면 y반환
// }

void rbtree_transplant(rbtree *t , node_t * u, node_t *v){
  if(u->parent == t->nil){ // 변경하려는 위치의 노드가 루트노드일때
    t->root =v;
  }
  else if(u == u->parent->left){ // 내 부모가 상위노드기준 왼쪽에서 왔는지
    u->parent->left = v;
  }
  else{ // 내 부모가 상위노드기준 오른쪽에서 왔는지
    u->parent->right = v;
  }
  v->parent = u->parent; // 새로 올리려는 노드의 부모주소를 이전에 있던 노드의 부모주소로 부모관계 정리
}


void rb_delete_fixup(rbtree *t, node_t *x){
    node_t *w;
    while ((x != t -> root) && (x -> color == RBTREE_BLACK))
    {
        if (x == x -> parent -> left)
        {
            w = x -> parent -> right;
            if (w -> color == RBTREE_RED)
            {
                w -> color = RBTREE_BLACK;
                x -> parent -> color = RBTREE_RED;
                rbtree_left_rotate(t, x -> parent);
                w = x -> parent -> right;
            }
            if (w -> left -> color == RBTREE_BLACK && w -> right -> color == RBTREE_BLACK)
            {
                w -> color = RBTREE_RED;
                x = x -> parent;
            }
            else
            {
                if (w -> right -> color == RBTREE_BLACK)
                {
                    w -> left -> color = RBTREE_BLACK;
                    w -> color = RBTREE_RED;
                    rbtree_right_rotate(t, w);
                    w = x -> parent -> right;
                }
                w -> color = x -> parent -> color;
                x -> parent -> color = RBTREE_BLACK;
                w -> right -> color = RBTREE_BLACK;
                rbtree_left_rotate(t, x -> parent);
                x = t->root;
            }
        }
        else
        {
            w = x -> parent -> left;
            if (w -> color == RBTREE_RED)
            {
                w -> color = RBTREE_BLACK;
                x -> parent->color = RBTREE_RED;
                rbtree_right_rotate(t, x -> parent);
                w = x -> parent->left;
            }
            if (w -> right -> color == RBTREE_BLACK && w -> left -> color == RBTREE_BLACK)
            {
                w -> color = RBTREE_RED;
                x = x -> parent;
            }
            else
            {
                if (w -> left -> color == RBTREE_BLACK)
                {
                    w -> right -> color = RBTREE_BLACK;
                    w -> color = RBTREE_RED;
                    rbtree_left_rotate(t, w);
                    w = x -> parent -> left;
                }
                w -> color = x -> parent -> color;
                x -> parent -> color = RBTREE_BLACK;
                w -> left -> color = RBTREE_BLACK;
                rbtree_right_rotate(t, x -> parent);
                x = t -> root;
            }
        }
    }
    x -> color = RBTREE_BLACK;
}



int rbtree_erase(rbtree *t, node_t *z){
    node_t *y = z;
    color_t y_orginal_color = y->color;
    node_t *x;
    if (z -> left == t -> nil)
    {
        x = z -> right;
        rbtree_transplant(t, z, z -> right);
    }
    else if (z -> right == t -> nil)
    {
        x = z -> left;
        rbtree_transplant(t, z, z -> left);
    }
    else
    {
        y = tree_minimum(t, z -> right);
        y_orginal_color = y->color;
        x = y -> right;
        if (y -> parent == z)
        {
            x -> parent = y;
        }
        else
        {
            rbtree_transplant(t, y, y -> right);
            y -> right = z -> right;
            y -> right -> parent = y;
        }
        rbtree_transplant(t, z, y);
        y -> left = z -> left;
        y -> left -> parent = y;
        y -> color = z -> color;
    }
    if (y_orginal_color == RBTREE_BLACK)
    {
        rb_delete_fixup(t, x);
    }
    free(z);
    return 0;
}

node_t *get_next_node(const rbtree *t, node_t *p){
  //트리는 변경되지 말라고 const로 받아옴
  node_t *current = p->right;
  if(current == t->nil){ // 현재 오른쪽 자식이 없으면(현재보다 큰값이 없으면)
    current = p;
    while(1){ // 다음 인오더 노드를 찾는 방법
      if(current->parent->right == current){ // 내가 오른쪽에서 온경우
        current = current->parent; //부모노드로 이동후 탐색
      }
      else{
        return current->parent; // current가 왼쪽에서 온경우 부모 리턴
      }
    }
  }
  // 오른쪽 자식이 있는 경우
  while(current->left != t->nil){ // 왼쪽자식이 있는 경우
    current = current->left; // 왼쪽 끝으로 이동
  }
  return current;
}


int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  // 어레이의 값들을 삽입한 트리 자체를 t로 주는것
  // 이진탐색트리의 중위순회 결과값은 오름차순

  if(n==0){
    return 0; // 배열 크기가 0인경우
  }

  node_t *current = rbtree_min(t);
  for(int i =0; i<n; i++){
    
    if(current == t->nil){
      break;
    }
    arr[i] = current->key;
    current = get_next_node(t, current);
  }
  return 0;
}

