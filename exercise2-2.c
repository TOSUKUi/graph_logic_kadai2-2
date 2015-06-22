/*bp13007
  雨宮俊貴
  2015/06/21
  ver2.2
  ©amemiya-toshiki 2015
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define N_MAX 100         //最大配列長
#define MAX_START_LENGTH 1//1
#define INPUT_FILE "graph2.txt"
#define OUTPUT_FILE "OutPut1-3-1.txt"
#define BORDER_LINE "=====================================\n"
#define RANDOM_SEED 4585798 //乱数の種
#define NUM_SIMULATION 10000 //シミュレーション数
#define RETURN "\n"

struct adjacent_info{
  int numPoint; //グラフの点の数
  int numEdge; //グラフの辺の数
  int **adjacent; //隣接行列
  int *valency; //各々の次数
  double averageValency; //平均次数
  int count;//経路カウンター
};

struct NODE{
  int value; //V_'n'
  struct NODE *next; //次の点
};




FILE *func_InputFile(FILE *,char *); //ファイル読み込み
FILE *func_OutPutFile(FILE *); //ファイル書き出し読み込み
struct adjacent_info func_MakeAdjacencyMatrix(struct adjacent_info,FILE *);  //隣接行列の生成
struct adjacent_info func_getEachValency(struct adjacent_info);                  //それぞれの点の次数を算出する
char func_isEulerGraph(struct adjacent_info);
struct NODE func_makeEulerRoute(struct adjacent_info);//オイラー経路を算出
struct NODE *func_addValueToList(int,struct NODE *);
void func_printList(struct NODE *); //経路を表示する
void func_freeList(struct NODE *); //経路を解放する
int func_getMinimumFromList(struct NODE *);//リストから最小値を取得する
void use_getrusage(void); //メモリの使用量を表示する
void visit(int,int *,int,int**);//わからない
int connect_check(int,int **);//非連結の場合2を出す
int func_getminimum(int *,int); //ある配列の中の最小値を求める
int func_getVertexConnectivity(struct adjacent_info); //店連結度を算出
int **func_Make_MN_Matrix(int,int); //M:N行列を作る
int **func_cpyMatrix(int **,int,int);
void func_freeMatrix(int **,int,int);
int power(int,int); //累乗を計算する


int main (void){
  use_getrusage();
  srand48(RANDOM_SEED);
  FILE *fp;
  FILE *fpout;
  int i,j;
  int sum = 0;
  char input[20];
  int pointConnectivity;

    
  
  /*struct NODE *node; */
  
  printf("Please input textfile name of edge of matrix ->");
  scanf("%s",input);
  
  /*隣接行列生成プロセス*/
  struct adjacent_info ai;
  ai.numPoint = 0;
  ai.numEdge = 0;
  ai.count = 0;
  
  fp = func_InputFile(fp,input);
  fpout = func_OutPutFile(fpout);

  ai = func_MakeAdjacencyMatrix(ai,fp); 
  
  ai = func_getEachValency(ai);

  //店連結度の算出
  pointConnectivity = func_getVertexConnectivity(ai);
  printf("%d\n",pointConnectivity);
  //use_getrusage();
  
 

  /*コンソール出力部*/
  printf(BORDER_LINE);
  printf("隣接行列の情報\n");
  printf("Number of Point = %d\n",ai.numPoint);
  printf("Number of Edge = %d\n",ai.numEdge);
  printf("------------------------------------\n");
  fprintf(fpout,BORDER_LINE"隣接行列の情報\nNumber of Point = %d\nNumber of Edge = %d\n----------------------------\n",ai.numPoint,ai.numEdge);
  /*行列の表示*/
  for(i=0;i<ai.numPoint;i++){
    for(j=0;j<ai.numPoint;j++){	
      printf("%d ",ai.adjacent[i][j]);
      fprintf(fpout,"%d ",ai.adjacent[i][j]);
    }
    printf("  v%dの次数 = %d\n",i,ai.valency[i]);
    fprintf(fpout,"   v%dの次数 = %d\n",i,ai.valency[i]);
    sum += ai.valency[i];
  }
  ai.averageValency = (double)sum/ai.numPoint;//平均次数の算出
  printf("平均次数 = %lf\n",ai.averageValency);
  fprintf(fpout,"平均次数 = %lf\n",ai.averageValency);
  printf("最小次数 = %d\n",func_getminimum(ai.valency,ai.numPoint));
  printf("点連結度 = %d\n",pointConnectivity);
  if(func_isEulerGraph(ai))
    printf("このグラフはオイラーグラフですね\n");
  else
    printf("このグラフはオイラーグラフではないですね\n");
  printf(BORDER_LINE);
  fprintf(fpout,BORDER_LINE);
  
  

  /*メモリ解放*/

  //use_getrusage();
  for(i=0;i<ai.numPoint;i++)
    free(ai.adjacent[i]);
  free(ai.adjacent);
  
  fclose(fp);
  fclose(fpout);

  use_getrusage();
  
  return 0;
  
  
}



FILE *func_InputFile(FILE *fp,char *input){
  if((fp = fopen(input,"r")) == NULL){
    printf("fuck off!!\n");
    exit(EXIT_FAILURE); //エラーなんで落ちます
  }
  return fp;
}

FILE *func_OutPutFile(FILE *fp){
  if((fp = fopen(OUTPUT_FILE,"w")) == NULL){
    printf("fuck off!!\n");
    exit(EXIT_FAILURE); //エラーなんで落ちます
  }
  return fp;
}
  
struct adjacent_info func_MakeAdjacencyMatrix(struct adjacent_info ai,FILE *fp){
  
  int begin,end;

  fscanf(fp,"%d",&ai.numPoint);
  
  ai.adjacent = func_Make_MN_Matrix(ai.numPoint,ai.numPoint);
  
   while(fscanf(fp,"%d %d",&begin,&end) != EOF){
    ai.adjacent[begin][end]++;
    ai.adjacent[end][begin]++;
    ai.numEdge++;
   }  


  return ai;
}    

struct adjacent_info func_getEachValency(struct adjacent_info ai){
  int i,j;
  ai.valency = (int *)malloc(sizeof(int)*ai.numPoint);
  for(i = 0;i < ai.numPoint;i++)    
    for(j = 0;j < ai.numPoint;j++)
      ai.valency[i] += ai.adjacent[i][j];
  
  return ai;
}

char func_isEulerGraph(struct adjacent_info ai){
  int i;
  int flag_Euler=0;
  for(i=0;i<ai.numPoint;i++)
    flag_Euler += ai.valency[i] % 2;
  if(flag_Euler > 0)
    return 0;
  else
    return 1;
}

struct NODE func_makeEulerRoute(struct adjacent_info ai){
  int start;
  start = 0;
  if(ai.adjacent[i][j])
  
  
}

struct NODE *func_addValueToList(int val,struct NODE *root){  
  if(root->value==0)
    root->value = val;
  else{
    while(root->next != NULL)
      root = root->next;
    struct NODE *node = (struct NODE *)malloc(sizeof(struct NODE));
    node->value = val;
    node->next = NULL;
    root -> next = node;
  }
}


int func_getMinimumFromList(struct NODE *root){
  int val = root->value;
  
  while(root->next != NULL){
    root = root -> next;
    if(root->value < val)
      val = root->value; 
  }
  return val;
}


void func_printList(struct NODE *root){
  if(root==NULL)
    return;
  else{
    printf("->");
    printf("%d",root->value);
    return func_printList(root->next);
  }
}

void func_freeList(struct NODE *root){
  struct NODE *next;
  
  while(root != NULL){
    next =root->next;
    free(root);
    root = next;
  }
  return;  

}

int func_getminimum(int *value,int N){
  int i;
  int min=N;
  for(i=0;i < N;i++)
    if(min > value[i])
      min = value[i];
  return min;

  
}

int func_countList(int count,struct NODE *root){
  if(root == NULL)
    return count;
  else{
    count++;
    return func_countList(count,root->next);
  }
}
  
/** getrusage()を使った最大駐在ページサイズの取得と表示 */
void use_getrusage(void)
{
  struct rusage r;
  if (getrusage(RUSAGE_SELF, &r) != 0) {
    /*Failure*/
  }
  printf("maxrss=%ld\n", r.ru_maxrss);
}

int func_getVertexConnectivity(struct adjacent_info ai){
  int i,j,k;
  int l=0;
  int m=0;
  int v[8];
  int total;
  int vertexConnectivity;
  int **matrix;
  int **adjacent_copy = NULL;
  struct NODE *root;
  int count;
  //点集合リスト作成
  root = (struct NODE *)malloc(sizeof(struct NODE));
  root->value = 0;
  root->next = NULL;

  //点集合vの初期化
  for(i=0;i < ai.numPoint;i++)
    v[i] = 0;
  for(i = 1; i<256;i++){
    //隣接行列の初期化2(隣接行列をいじるので，初期化を繰り返す必要がある)
    adjacent_copy = func_cpyMatrix(ai.adjacent,ai.numPoint,ai.numPoint);    
 
    total = 0;
    for(j = 0; j < 8;j++){
      //val % 2^(j+1) / 2^j　の部分
      v[j] = i%power(2,j+1)/power(2,j);
      //printf("%d ％ power(2,%d+1) = %d ,  ,v[j] = %d\n",i,j , i%power(2,j+1),i%power(2,j+1)/power(2,j));
      total += v[j];
    }
    matrix = func_Make_MN_Matrix(ai.numPoint - total,ai.numPoint - total);      
    
    //消される点を全て-1に置き換える
    for(j = 0;j < ai.numPoint;j++)
      if(v[j] == 1)
	for(k=0;k<ai.numPoint;k++){
	  adjacent_copy[j][k] = -1;
	  adjacent_copy[k][j] = -1;
	}
    /*
    for(j = 0;j < ai.numPoint;j++){
      for(k = 0; k <ai.numPoint;k++)
	printf(" %d",adjacent_copy[j][k]);
      printf("\n");
    }
    printf("-----------------------------------------\n");
    */
    //点が-１の場合，その点を消し，その点以外の点で新しい行列を作る
    l=0;
    for(j = 0; j < ai.numPoint;j++){
      count = 0;
      for(k = 0; k <ai.numPoint;k++){
	if(adjacent_copy[j][k] != -1){
	  matrix[l][m] = adjacent_copy[j][k];
	  m++;
	  count++;
	}	
      }
      m=0;
      if(count > 0)
	l++;      
    }
    /*
    for(j = 0;j < ai.numPoint - total;j++){
      for(k = 0; k <ai.numPoint - total;k++)
	printf(" %d",matrix[j][k]);
      printf("\n");
    }
    */
    //連結か非連結かを確かめる
    if(connect_check(ai.numPoint-total,matrix) > 1){      
      func_addValueToList(total,root);
      //func_printList(root);
    }    
    func_freeMatrix(matrix,ai.numPoint - total,ai.numPoint - total);
    func_freeMatrix(adjacent_copy,ai.numPoint,ai.numPoint);
    // use_getrusage();

  }
  //func_printList(root);
  vertexConnectivity = func_getMinimumFromList(root);
  printf("点連結度は%dです",vertexConnectivity);
  func_freeList(root);
  use_getrusage();
  return vertexConnectivity;
  
  
}
  
int connect_check( int N, int **adjacent )
{
  int i,*YetToVisit,count = 0;
  YetToVisit = ( int * )malloc( sizeof( int ) * N );
  for ( i=0; i<N; i++ )
    YetToVisit[i] = 1;
  for ( i=0; i<N; i++ )
    if ( YetToVisit[i] == 1 ) {
      count++;
      visit( i, YetToVisit, N, adjacent );
    }
  free( YetToVisit );
  return ( count );
}

void visit(int v,int *yet,int N,int **adjacent)
{
  int w;

  yet[v] = 0;

  for(w = 0;w < N; w++)
    if( adjacent[v][w] == 1 && yet[w] == 1)
      visit(w,yet,N,adjacent);
}

int **func_Make_MN_Matrix(int M,int N){
  int i,j;
  
  int **matrix = (int **)malloc(sizeof(int *)*M);
  for(i = 0;i < M;i++)
    matrix[i]= (int *)malloc(sizeof(int)*N);
  
  for(i=0;i<M;i++)
    for(j=0;j<N;j++)
      matrix[i][j]= 0;
  
  return matrix;
}

int **func_cpyMatrix(int **origin,int m,int n){
  int i,j;
  int **matrix;
  matrix = func_Make_MN_Matrix(m,n);
  
  for(i=0;i<m;i++)
    for(j=0;j<n;j++)
      matrix[i][j] = origin[i][j];
  
  return matrix;
}
void func_freeMatrix(int **matrix,int m,int n){
  int i;
  for(i=0;i<m;i++)
    free(matrix[i]);
  free(matrix);
}


int power(int val,int num){
   int i;
   if(num == 0)
     return 1;
   else
     return val * power(val,num-1);
 }
  
  



    
  

