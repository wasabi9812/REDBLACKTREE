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


void rbtree_left_rotate(rbtree *t, node_t *x)
{
  node_t *y = x->right;     // y는 x의 left

  x->right = y->left;                                  // x 노드의 right: [as-is] y, [to-be] y->left

  if (y->left != t->nil)
  {
    y->left->parent = x;                               // y의 left의 parent: [as-is] y, [to-be] x
    // => y->left를 x의 right로 양방향 연결 성공

  } // [y 노드의 left]가 NIL 노드라면 [y 노드의 left]의 부모 노드의 정보는 그대로 유지 (parent: NIL)

  y->parent = x->parent;                               // y 노드의 parent: [as-is] x, [to-be] x->parent

  if (x->parent == t->nil)
  {
    t->root = y;                                       // 루트 노드: [as-is] x, [to-be] y
  }
  else if (x == x->parent->left)
  {
    x->parent->left = y;                                     // x 노드의 parent의 left: [as-is] x, [to-be] y
  }
  else
  {                              // x 노드의 parent의 right: [as-is] x, [to-be] y
    x->parent->right= y;
  }

  y->left = x;                                         // y 노드의 left: [as-is] y->left, [to-be] x
  x->parent = y;                                       // x 노드의 parent: [as-is] x->parent, [to-be] y
}



void rbtree_right_rotate(rbtree *t, node_t *x)
{
  node_t *y = x->left;     // y는 x의 left

  x->left = y->right;                                  // x 노드의 left: [as-is] y, [to-be] y->left

  if (y->right != t->nil)
  {
    y->right->parent = x;                               // y 노드의 right의 parent: [as-is] y, [to-be] x
    // => y->right x의 left로 양방향 연결 성공

  }

  // 여기부터 아래까지는 LEFT_ROTATE와 코드 동일
  y->parent = x->parent;                               // y 노드의 parent: [as-is] x, [to-be] x->parent

  if (x->parent == t->nil)
  {
    t->root = y;                                       // 루트 노드: [as-is] x, [to-be] y
  }
  else if (x == x->parent->right)
  {
    x->parent->right = y;                               // x 노드의 parent의 left: [as-is] x, [to-be] y
  }
  else
  {                                      // x 노드의 parent의 right: [as-is] x, [to-be] y
    x->parent->left =y;
  }

  y->right = x;                                         // y 노드의 left: [as-is] y->left, [to-be] x
  x->parent = y;                                       // x 노드의 parent: [as-is] x->parent, [to-be] y
}



void rbtree_insert_fixup(rbtree *t, node_t *z)
{
  while (z->parent->color == RBTREE_RED)
  {
    // 부모가 할아버지의 왼쪽에 있을 때
    if (z->parent == z->parent->parent->left)
    {
      node_t *y = z->parent->parent->right;
      if (y->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else
      {
        if (z->parent->right)
        {
          z = z->parent;
          rbtree_left_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        rbtree_right_rotate(t, z->parent->parent);
      }
    }
    else 
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
        if (z->parent->left)
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
  t->root->color = RBTREE_BLACK;
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
    if (x == NULL) {
      break;
    }

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
  // TODO: implement find
  node_t *ptr = t->root;
  
  while (ptr != t->nil)
  {
    if (ptr->key > key) //현재 비교값보다 포인터가 가르키는게 작다면 왼쪽노드로
    {
      ptr = ptr->left;
    }
    else if (ptr-> key < key) //오른쪽 하위노드로
    {
      ptr = ptr->right;
    }
    else
    {
      return ptr; //크기가 같다면 반환
    }
  }

  return NULL;
}





void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
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

node_t* tree_minimum(rbtree *t, node_t *z){ // successor 찾는중에 오른쪽노드가있을때 들어가서 제일 왼쪽노드 찾으려고 만든함수
  while(z->left != t->nil){
    z = z->left;
  }
  return z;
}

node_t* tree_succesor(rbtree *t ,node_t *z){
  if(z->right != t->nil){
    //오른쪽 서브트리가 존재하는경우
    return tree_minimum(t, z->right);
  }
  node_t *y = z->parent; //오른쪽 서브트리가 없는경우
  while(y!= t->nil && z == y->right){ //부모가 닐이 아니면서
    z = y; // y로
    y = y->parent; // y의 부모로    즉 z y 둘다 한단계식 위로
  }
  return y; // 모든루프에서 나오면 y반환
}

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


void rb_delete_fixup(rbtree * t, node_t *x){
//  check1. 내 색이 black 인가?
//  check2. 내가 왼쪽에서 온 자식인가?
//  check3. 형제의 색깔이 무슨색인가?
//  분기는 총 3개
node_t *w; // 삼촌 지칭용 포인터 설정

while(x != t->nil && x->color == RBTREE_BLACK){ // check1
    if(x==x->parent->left){ // check2-1 내가 왼쪽에서 온 자식일때
      w = x->parent->right; // 삼촌 설정
      if(w->color == RBTREE_RED){ // case1 삼촌이 붉은색
          w->color = RBTREE_BLACK;                                                                  // 삼촌을 black으로
          x->parent->color = RBTREE_RED;                                                            // 부모를 redfh
          rbtree_left_rotate(t,x->parent);                                                          // 부모기준으로 왼쪽으로 회전
      }
      if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){ // case2 삼촌이 검은색 그리고 좌우 자녀 모두 검은색
        w->color = RBTREE_RED;                                                                      // 삼촌의 색을 붉은색으로
        x = x->parent;                                                                              //  내 위치를 부모로 탐색지점을 높임
      }
      else{ //case3 삼촌이 검은색 그리고 삼촌의 오른쪽자녀가 검은색, 왼쪽이 붉은색
        if(w->right->color == RBTREE_BLACK){
            w->left->color = RBTREE_BLACK;                                                          // 삼촌 왼쪽의 색을 검은색으로
        w->color = RBTREE_RED;                                                                      // 삼촌색을 붉은색으로
        rbtree_right_rotate(t,w);                                                                   // 부모기준으로 오른쪽 회전
        w = x->parent->right;                                                                       // w가 로테이트 후에 더이상 삼촌위치에 없어 내 부모의 오른쪽에 해당하는것을 새로운 w로 설정해줌
        }
        //case4  삼촌이 검은색 그리고 형제의 오른쪽 자녀가 붉은색일때
        w->color = x->parent->color;                                                                // 삼촌의 색은 내 부모의 색과 같게함
        x->parent->color = RBTREE_BLACK;                                                            // 내 부모의 색을 검정색으로
        w->right->color = RBTREE_BLACK;                                                             // 삼촌의 오른쪽 자식의 색을 검정색으로
        rbtree_left_rotate(t,x->parent);                                                            // 부모를 기준으로 왼쪽으로 회전
        x=t->root;                                                                                  // 다른 case 123과 달리 case4에서는 fixup을 하고나면 더이상의 fixup이 필요없도록 정렬됨 따라서 while문 조건을 기저조건으로 세팅함
      }
    }
    else{ // check2-2 내가 오른쪽에서 온 자식일때
        w  = x->parent->left;                                                                       // 삼촌은 내 부모의 왼쪽의 노드라고 설정
        if(w->color == RBTREE_RED){ // case1 삼촌이 붉은색일때
            w->color =RBTREE_BLACK;
            x->parent->color = RBTREE_RED;
            rbtree_right_rotate(t,x->parent);
            w = x->parent->left;
        }
        if(w->right->color == RBTREE_BLACK && w->left->color ==RBTREE_BLACK){ // case2 삼촌검은색 그리고 두 자녀 검정색
            w->color = RBTREE_RED;
            x = x->parent;
        }
        else{
          if(w->left->color == RBTREE_BLACK){ // 삼촌이 검정색에 오른쪽자녀가 빨간색일때
            w->right->color = RBTREE_BLACK;
            w->color = RBTREE_RED;
            rbtree_left_rotate(t,w);
            w = x->parent->left;
          }
          // case4 삼촌이 검정색에 왼쪽의 자녀가 빨간색일때
          w->color = x->parent->color;
          x->parent->color = RBTREE_BLACK;
          w->left->color = RBTREE_BLACK;
          rbtree_right_rotate(t,x->parent);
          x = t->root; // case4이후 조정완료로 기저조건 설정
        }
    }
  }
x->color = RBTREE_BLACK;
}



int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
    node_t *y = p;
    node_t *x;
    color_t y_origin = y->color;
    if(p->left == t->nil){  // 좌우자식이 없거나 오른쪽 노드만 있거나
        x = p->right; // fixup을 하는 위치 기록
        rbtree_transplant(t, p, p->right); // p를 오른쪽 자식으로 교체
    }
    else if(p->right == t->nil){ // 왼쪽 자식만 있는 경우
        x = p->left; //fixup을 하는위치 기록
        rbtree_transplant(t, p, p->left); //p를 왼쪽 자식으로 교체
    }
    else{  // 자식이 둘다 있는 경우
      y = tree_succesor(t,p->right); // 후속자 찾는 함수로 y에 후속자 저장
      y_origin = y->color; //후속자의 색 기록
      x = y->right; // fixup위치 기록

      if(y != p->right){ // 만약 후속자y가 삭제노드와 바로 붙어있는게 아니라면
        rbtree_transplant(t, y, y->right); // 후속자의 위치에 후속자 오른쪽 자식으로 교체
        y->right = p->right; // 후속자의 오른쪽자식노드에 삭제노드의 오른쪽자식를 달아줌
        y->right->parent = y; // 달아준 삭제노드의 오른쪽 자식의 부모관계를 후속자를 가리키도록 변경
      }
      else{ // 후속자가 삭제노드의 바로 붙어있는 경우 즉 y = p->right
        x->parent = y;
        rbtree_transplant(t, p, y);
        y->left = p->left;
        y->left->parent = y;
        y->color = p->color;
      }
    }
  
    if(y_origin == RBTREE_BLACK){ // 후속자가 검정색인경우 조정이 필요함
      rb_delete_fixup(t,x); //검정색일 경우 조정함수 호출
    }
    if (p == t->root) {
        t->root = (p->left == t->nil && p->right == t->nil) ? t->nil : x;
    }
  free(p); // 해당노드는 삭제되었으니 메모리 해제
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
  node_t *current = rbtree_min(t);
  arr[0] = current->key;
  for(int i =0; i<n; i++){
    if(current == t->nil){
      break;
    }
    current = get_next_node(t, current);
    if(current == t->nil){
      break;
    }
    arr[i] = current->key;
  }


  return 0;
}
