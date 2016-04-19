/*****************************************************************
*  CSCI 4998 Final Year Project I
*  Title: Querying Large Graph Databases
*  Algorithm : Yen's
*  Code by Deep
****************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

FILE *gdata;

char num1[15], num2[15], num3[15],buff,txt[20];
int n1=0, n2=0,n3=0, min_node=9999,node_num=0,num_count=0, num_flag=0;	
		  
struct graph{  //struct of graph
       int nb;
       int dis;
	   int spr;
    struct graph *next;
};
struct graph *node_p=NULL, *ndata=NULL;

struct path{  //struct of path
   int  vt;
   struct path *next;
};
struct path Path[3];
struct path *pat=NULL, *patH=NULL;

//src: sourse point in dijkstra
//end: end point in dijkstra
//pathnum: number of paths to find in yen
//path_nodenum: store the nodes in path each time
int i=0,src=0,end=0,pathnum=0;
int path_nodenum=1; 
//path_setA: store the path set A of yen
//path_setB: store the path set B of yen
//set_dis: store the distance of each nodes to src of set B's paths
//path_nodenumB: store the number of nodes in set B's paths
//path_setB_dis: store the distance of each path in set B
int path_setA[1000][2000],path_setB[1000][2000], set_dis[1000][1000];
int path_nodenumB[100], path_setB_dis[100];
//for yen's search
int B_min=99999, B_min_ptr=-1;
int root_dis=-1,root_num=-1, A_num=0,B_num=0;



int main(int argc, char *argv[]) {
    
	//read argv[1]
	i=0;
	if(argv[1]!=NULL){
		char *arg1=argv[1];
		while(*arg1 !='\0'){ 
	    txt[i]=*arg1;
	    arg1++;
	    i++;
      } 
	}
	///////graph data process/////
	get_node();
	struct graph node[node_num+1];
	for(i=0;i<=node_num;i++){
		node_p=&node[i];
		node_p->next=NULL;
	}
	get_deg(node);
	//////////////////////////////
    //start dijkstra and yen
	int dist[node_num+1], sptset[node_num+1];
	printf("---------------------\n|  Yen's Algorithm  |\n--------------------- code by Deep\n");
	printf("\nEnter from %d to %d to find some paths\n",min_node,node_num);
    printf("start point: ");
	scanf("%d",&src);//get startpoint
	printf("end point: ");
	scanf("%d",&end);//get endpoint
	
	if(src==end){//ensure path more than 1 node
		printf("\nstart point equal to end point!! bye bye~\n\n");
		return 0;
	}else if(src<min_node || src>node_num || end<min_node || end>node_num){
		printf("\nPlz enter from %d to %d \n",min_node,node_num);
		return 0;
	}
	
	dijkstra(dist,sptset,node);//first dijkstra
	//printSolution(dist);
	shortest_path(dist,node);//form the shortest path
	
	printf("number of path: ");
	scanf("%d",&pathnum);//get number of paths you need
	if(pathnum>1000){//max. path number is 1000
		printf("memory is limited, maximun is 1000 paths\n");
	}
	printf("As much as paths will be printed out\n");
	
	printf("\nset A[0]:\n");//print the shortest path
	print_path(dist);
	
	yen(node_num,dist,sptset,node);//yen's algorithm
	
}

///////////graph data mining//////////////////////////////////
int get_node(){
	if(txt[0]!='\0'){
		gdata =fopen(txt,"r");
	}else{
		gdata =fopen("t3.txt","r");
	}
    if (!gdata) return 1;
	    while ((buff = fgetc(gdata)) != EOF){
				if(buff=='\n'){
					 num_count=0;
					 memset(num1,0,15);
					 memset(num2,0,15);
					 n1=0;
					 n2=0;
					 num_flag=0;
				 }
				if(buff==' '){
					 num_count++;
				 }
				if(num_count==1 && buff!=' '){
					num1[n1]=buff;
					n1++;
				}
				if(num_count==2 && buff!=' '){
				   num2[n2]=buff;
				   n2++;
				}
				if(num_count==3 && num_flag==0){
					if(atoi(num1)>node_num){
						node_num=atoi(num1);   //update************
					}
					if(atoi(num2)>node_num){
						node_num=atoi(num2);   //update************
					}
					if(atoi(num1)<min_node){
						min_node=atoi(num1);   //update************
					}
					if(atoi(num2)<min_node){
						min_node=atoi(num2);   //update************
					}
					num_flag=1;
				}
			}
	fclose(gdata);
	return 0;
}
//////////////////////////////////////////////////////////
int get_deg(struct graph node[]){
	//////////init the struct array///////////////
	for (i=0;i<=node_num;i++){
		node_p=&node[i];
		node_p->next=NULL;
	}
	num_count=0;
	n1=0;
	n2=0;
	n3=0;
	///////////////////////////////////////////////////////
	////////////find degree and neigbours of each node ////
	if(txt[0]!='\0'){
		gdata =fopen(txt,"r");
	}else{
		gdata =fopen("t3.txt","r");
	}
    if (!gdata) return 1;
	    while ((buff = fgetc(gdata)) != EOF){
			/////////////////////record num1,2 when read '\n'
				if(buff=='\n'){ 
				node_p=&node[atoi(num1)]; //add data to num1
				while(node_p->next!=NULL){node_p=node_p->next;}
                ndata = (struct graph*)malloc(sizeof(struct graph));
                ndata->nb= atoi(num2);
				ndata->spr=0;
				if(atoi(num3)>0){
					ndata->dis= atoi(num3);
					//ndata->dis= 123;
				}else{
					ndata->dis= 1;
				}
				ndata->next=NULL;
				node_p->next=ndata;

				node_p=&node[atoi(num2)]; //add data to num2
				while(node_p->next!=NULL){node_p=node_p->next;}
                ndata = (struct graph*)malloc(sizeof(struct graph));
                ndata->nb= atoi(num1);
				ndata->spr=0;
				if(atoi(num3)>0){
					ndata->dis= atoi(num3);
					//ndata->dis= 123;
				}else{
					ndata->dis= 1;
				}
				ndata->next=NULL;
				node_p->next=ndata;
				
					 num_count=0;
					 memset(num1,0,15);
					 memset(num2,0,15);
					 memset(num3,0,15);
					 n1=0;
					 n2=0;
					 n3=0;
				 }
	     ///////////////////////////////////////////////////
				if(buff==' '){
					 num_count++;
				 }
		//////////////////////////////////////////////////	
				if(num_count==1 && buff!=' '){ //2nd num,start node
					num1[n1]=buff;
					n1++;				
				}
				if(num_count==2 && buff!=' '){//3rd num, end node
				   num2[n2]=buff;
				   n2++;				 
				}
				if(num_count==3 && buff!=' ' && buff!='\n'){//4th num L2 dis
					num3[n3]=buff;
					n3++;
				}
			}
		/////////////////////////////////////////////////////
					fclose(gdata);				
    		return 0;
}
//////////////////////////////////////////////////////////////////
///////////////dijkstra algorithm////////////////
int dijkstra(int dist[], int sptset[],struct graph node[]){
	int count =0, min_vertex=0;
    for (i = 0; i <= node_num; i++){
		  dist[i] = 99999, sptset[i] = 0;
	}
    dist[src] = 0;
	if(root_num>=1){
	for(i=0;i<=root_num-1;i++){
		sptset[path_setA[A_num][i]]=1;
	}
	}
    for (count = min_node; count <= node_num; count++){
        min_vertex = minDistance(dist, sptset);
        sptset[min_vertex] = 1;//true
		if (dist[min_vertex] != 99999){
			node_p=&node[min_vertex];
			node_p=node_p->next;
			while(node_p!=NULL){
				if(sptset[node_p->nb]==0 && node_p->dis+dist[min_vertex] < dist[node_p->nb]){
					if(min_vertex==src && node_p->spr==1){//check any coincide nodes
				    }else{                                //(for yen's)
						dist[node_p->nb] = dist[min_vertex] + node_p->dis; 
					}
				}
				if(node_p->next!=NULL){
					node_p=node_p->next;
				}else{
					break;
				}   
		    }								   
        }	
    }
}
///////////get minimum vertex(for dijkstra)/////////
int minDistance(int dist[], int sptset[]){
   int v =0, min_dis = 99999, min_index=0;
   for (v = min_node; v <= node_num; v++){
	   if (sptset[v] == 0 && dist[v] <= min_dis){
		   min_dis = dist[v];
		   min_index = v;
	   }
   } 
   return min_index;
}
///////////form the shortest path/////////
int shortest_path(int dist[],struct graph node[]){
	int back_node = end;
	if(root_num!=-1){
		path_nodenum=root_num+1;
	}else{
		path_nodenum=1;
	}
	//backtrace to find the path from dist[]
	patH=&Path[1];
	pat =(struct path*)malloc(sizeof(struct path));
	pat->vt=end;
	patH->next=pat;
	patH=patH->next;
	while(back_node != src){
	    node_p=&node[back_node];
	    node_p=node_p->next;
	  while(node_p!=NULL){
		if(dist[back_node] == dist[node_p->nb] + node_p->dis){
			back_node = node_p->nb;
			pat =(struct path*)malloc(sizeof(struct path));
	        pat->vt=back_node;
			pat->next=NULL;
	        patH->next=pat;
			patH=patH->next;
			path_nodenum++;
			break;
		}else{
			if(node_p->next!=NULL){
				node_p=node_p->next;
		    }
	    }
	  }
    }
	int arr_ptr=path_nodenum-1;
	//store the path in array so easy use it
	patH=&Path[1];
	if(patH->next!=NULL){
		patH=patH->next;
		while(patH!=NULL){
			if(root_num==-1){
				path_setA[A_num][arr_ptr]=patH->vt;
			}else{
				path_setB[B_num][arr_ptr]=patH->vt;
			}
			
			arr_ptr--;
			if(patH->next!=NULL){
				patH=patH->next;
			}else{
				goto endpath;
			}
		}
	}
	endpath:
	if(root_num!=-1){
		for(i=arr_ptr;i>=0;i--){
		path_setB[B_num][i]=path_setA[A_num][i];
	}	
	//mark the spur nodes(for yen's)
	for(i=0;i<=path_nodenum-2;i++){
		node_p=&node[path_setB[B_num][i]];
		node_p=node_p->next;
		while(node_p!=NULL){
			if(node_p->nb==path_setB[B_num][i+1]){
				node_p->spr=1;
			}
			if(node_p->next!=NULL){
				node_p=node_p->next;
			}else{
				break;
			}
		}
	}
	path_nodenumB[B_num]=path_nodenum;
	path_setB_dis[B_num]=dist[end]+root_dis;
	}
	
	return 0;
}
////////////print solution of dijkstra/////////////////
int printSolution(int dist[]){
   int i =0;
   printf("Vertex   Distance from Source\n");
   for (i = min_node; i <= node_num; i++){
	  printf("%d \t\t %d\n", i, dist[i]);
   }
}
//////////////////////////////////////////////////////

//print the paths that from shortest_path()
int print_path(int dist[]){
	
	for(i=0;i<=path_nodenum-1;i++){
		    if(root_num==-1){
				printf("%d ",path_setA[A_num][i]);	
			}else{
				printf("%d ",path_setB[B_num][i]);
			}
			
		if(i!=path_nodenum-1){
			if(root_num==-1){
				printf("- ");
			}else{
				//printf("-");
			  // printf("(%d)",set_dis[B_num][i+1]-set_dis[B_num][i]);
			    printf("- ");
			}
			
		}
	}
	    	if(root_num==-1){
				printf("\nDistance: %d\n",dist[end]);
			}else{
				printf("\nDistance: %d\n",path_setB_dis[B_num]);
			}
		
}

/////////////////////////////yen's algorithm///////////////////////////
int yen(int node_num,int dist[],int sptset[],struct graph node[]){
	int j=0, setA_nodenum=path_nodenum, dist2[node_num+1];
	for(i=0;i<=setA_nodenum-1;i++){//use for the first path in set A
		node_p=&node[path_setA[A_num][i]];
		node_p=node_p->next;
		while(node_p!=NULL){
			if(node_p->nb==path_setA[A_num][i+1]){
				node_p->spr=1;
			}
			if(node_p->next!=NULL){
				node_p=node_p->next;
			}else{
				break;
			}
		}
		dist2[path_setA[A_num][i]]=dist[path_setA[A_num][i]];
	}
	//find n paths
	for(j=1;j<=pathnum-1;j++){

	if(A_num<=B_num){
	setA_nodenum=path_nodenum;
	root_num=-1, B_min=99999;
	
	while(root_num<setA_nodenum-2){
		root_num++;
		src=path_setA[A_num][root_num];
		
		if(A_num==0){
			root_dis=dist2[path_setA[A_num][root_num]];
		}else{
			root_dis=set_dis[B_min_ptr][root_num];
		}
		
		dijkstra(dist,sptset,node);
		if(dist[end]!=99999){
		shortest_path(dist,node);
		//find the distance of each nodes to src of set B's paths
		for(i=0;i<=path_nodenum-1;i++){
			node_p=&node[path_setB[B_num][i]];
			node_p=node_p->next;
			if(i==0){
				set_dis[B_num][i]=0;
			}else{
				while(node_p!=NULL){
				if(node_p->nb==path_setB[B_num][i-1]){
					if(i==1){
						set_dis[B_num][i]=node_p->dis;
						break;
					}else{
						set_dis[B_num][i]=node_p->dis+set_dis[B_num][i-1];
						break;
					}
				}
					if(node_p->next!=NULL){
						node_p=node_p->next;
					}
			    }
			}
		}
		//printf("\nset B[%d] spur node: %d\n",B_num,src);
		//print_path(dist);
		B_num++;
		}
	}
	
	//find the shortest path in set B
	B_min_ptr=-1;
	for(i=0;i<B_num;i++){
		if(path_setB_dis[i]<B_min){
			B_min = path_setB_dis[i];
			B_min_ptr = i;
		}
	}
	if(B_min_ptr!=-1){
	A_num++;
	path_nodenum= path_nodenumB[B_min_ptr];
	printf("\nset A[%d] from  set B[%d]\n",A_num,B_min_ptr);
	
    //put the shortest path in set B to set A
	for(i=0;i<=path_nodenumB[B_min_ptr]-1;i++){
		path_setA[A_num][i]=path_setB[B_min_ptr][i];
		printf("%d ",path_setA[A_num][i]);
		if(i!=path_nodenumB[B_min_ptr]-1){
			//printf("-");
			//printf("(%d)",set_dis[B_min_ptr][i+1]-set_dis[B_min_ptr][i]);
			printf("- ");
		}
	}
	printf("\nDistance: %d\n",path_setB_dis[B_min_ptr]);
	path_setB_dis[B_min_ptr]=99999;
	}
	}
	}
}
//////////////////////////////////////////////////////


