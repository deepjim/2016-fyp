/*****************************************************************
*  CSCI 4998 Final Year Project I
*  Title: Querying Large Graph Databases
*  Algorithm : Pruned Landmark labelling
*  Code by Deep
****************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

//graph seting
FILE *gdata;
uint32_t i = 0;
//to read each line of data
char num1[30], num2[30], num_dis[30], buff, txt[20];
int n1 = 0, n2 = 0, n3 = 0, count = 0, flag = 0;

//shortest distance for some nodes: node_sdis
//minimum node: node_min
uint32_t node_sdis = 99999, node_min = 99999, deg_max = 0, deg_n = 0;

int max_nbdis = 0;

//struct of graph
struct graph {  
	uint32_t nb;
	unsigned int dis;
	struct graph *next;
};
struct graph *node_p = NULL, *ndata = NULL;

//struct of index 
struct index { 
	uint32_t vt;
	unsigned int dis;
	struct index *next;
};
struct index *ind = NULL, *indH = NULL, *tn = NULL;
struct index *inds = NULL, *indt = NULL;

//struct of path
struct path {
	uint32_t  vt;
	unsigned int dis2s;
	struct path *next;
	struct path *sort_node;
};
struct path Path[10];
struct path *pat = NULL, *patH = NULL, *pat_ptr = NULL;

//For query
uint32_t D = 99999, s = 0, t = 0, v = 0, max_thr = 0, num_path = 0, path_nodenum = 0;
uint32_t ss = 0, tt = 0; //backup s and t
uint32_t D_font = 0, v_nb = 0;
int chek = 0; //check input


//find out the number of nodes in graph
int get_node() {

	uint32_t num_node = 0;
	count = 0;
	memset(num1, 0, 30);
	memset(num2, 0, 30);
	n1 = 0;
	n2 = 0;
	flag = 0;
	if (txt[0] != '\0') {
		gdata = fopen(txt, "r");
	}
	else {
		gdata = fopen("t3.txt", "r");
	}
	if (!gdata) return 1;
	while ((buff = fgetc(gdata)) != EOF) {
		if (buff == '\n') {
			count = 0;
			memset(num1, 0, 30);
			memset(num2, 0, 30);
			n1 = 0;
			n2 = 0;
			flag = 0;
		}
		if (buff == ' ') {
			count++;
		}
		if (count == 1 && buff != ' ') { //start node
			num1[n1] = buff;
			n1++;
		}
		if (count == 2 && buff != ' ') { //end node
			num2[n2] = buff;
			n2++;
		}
		if (count == 3 && flag == 0) {
			if (atoi(num1)>num_node) {   //num store the number of nodes
				num_node = atoi(num1);   //update
			}
			if (atoi(num2)>num_node) {
				num_node = atoi(num2);   //update
			}
			if (atoi(num1)<node_min) {   //node_s store the value of smallest nodes
				node_min = atoi(num1);   //update
			}
			if (atoi(num2)<node_min) {
				node_min = atoi(num2);   //update
			}
			flag = 1;
		}
	}
	fclose(gdata);
	return num_node;
}

//sort the node of the graph in degree
int get_deg(uint32_t num_node, struct graph node[], struct index label[], uint32_t deg[], uint32_t deg_s[], uint32_t deg_r[]) {
	
	count = 0;
	memset(num1, 0, 30);
	memset(num2, 0, 30);
	memset(num_dis, 0, 30);
	n1 = 0;
	n2 = 0;
	n3 = 0;
	//init some struct array
	for (i = 0; i <= num_node; i++) {
		node_p = &node[i];
		node_p->next = NULL;
		ind = &label[i];
		ind->next = NULL;
		deg[i] = 0;
		deg_r[i] = 0;
		deg_s[i] = 0;
	}


	//find degree and neigbours of each node ////
	if (txt[0] != '\0') {
		gdata = fopen(txt, "r");
	}
	else {
		gdata = fopen("t3.txt", "r");
	}
	if (!gdata) return 1;

	while ((buff = fgetc(gdata)) != EOF) {
		/////////////////////record num1,2 when read '\n'
		if (buff == '\n') {
			node_p = &node[atoi(num1)]; //add data to num1			
			while (node_p->next != NULL) { node_p = node_p->next; }
			ndata = (struct graph*)malloc(sizeof(struct graph));
			ndata->nb = atoi(num2);

			if (atoi(num_dis) == 0) {
				ndata->dis = 1;
			}
			else {
				ndata->dis = atoi(num_dis);
				
			}
			if (ndata->dis<node_sdis) {
				node_sdis = ndata->dis;
			}
		
			ndata->next = NULL;
			node_p->next = ndata;

			node_p = &node[atoi(num2)]; //add data to num2
			while (node_p->next != NULL) { node_p = node_p->next; }
			ndata = (struct graph*)malloc(sizeof(struct graph));
			ndata->nb = atoi(num1);

			if (atoi(num_dis) == 0) {
				ndata->dis = 1;
			}
			else {
				ndata->dis = atoi(num_dis);
				
			}

			if (ndata->dis<node_sdis) {
				node_sdis = ndata->dis;
			}
			ndata->next = NULL;
			node_p->next = ndata;

			deg[atoi(num1)]++;
			deg[atoi(num2)]++;
			count = 0;
			memset(num1, 0, 30);
			memset(num2, 0, 30);
			memset(num_dis, 0, 30);
			n1 = 0;
			n2 = 0;
			n3 = 0;
		}
		if (buff == ' ') {
			count++;
		}
		if (count == 1 && buff != ' ') { //start node
			num1[n1] = buff;
			n1++;
		}
		if (count == 2 && buff != ' ') { //end node
			num2[n2] = buff;
			n2++;
		}
		if (count == 3 && buff != ' ' && buff != '\n') { // L2 dis
			num_dis[n3] = buff;
			n3++;
		}
	}
	fclose(gdata);

	//find the largest degree
	for (i = node_min; i <= num_node; i++) {
		if (deg[i]>deg_max) {
			deg_max = deg[i];  //update
		}
	}
	//printf("max deg: %d\n",deg_max);		
	
	//sort the nodes by decreasing degree and save to deg_s[]//
	deg_n = node_min;
	while (deg_max>0) {
		for (i = node_min; i <= num_node; i++) {
			if (deg[i] == deg_max) {  //degree is match
				deg_s[deg_n] = i; //put it to the sorted array
				deg_r[i] = deg_n;
				//  printf("deg_s[%d]:%d\n",deg_n,deg_s[deg_n]);
				deg_n++;
			}
		}
		deg_max--;
	}
	return 0;
}

// BFS
int BFS(uint32_t vk, struct graph node[], struct index label[], uint32_t P[], uint32_t Q[], uint32_t T[], uint32_t deg_s[], uint32_t num_node, uint32_t deg_r[], uint32_t R[]) {
	printf("Loading  %d/%d ...\n", vk, num_node);
	uint32_t Qn_ptr = 0, Qn = 0, D_bfs = 0;
	//////////deg_s[vk] is the only element in Q/////////
	P[deg_s[vk]] = 0;
	Q[Qn] = deg_s[vk];
	Qn++;

	//set up T[] to do query in BFS in |L(u)| time
	//T[] will store all the index of vk
	tn = &label[deg_s[vk]];
	if (tn->next != NULL) {
		tn = tn->next;
		while (tn != NULL) {
			T[tn->vt] = tn->dis;
			if (tn->next != NULL) {
				tn = tn->next;
			}
			else {
				goto endtn;
			}
		}
	}
endtn:

	//dequeue u from Q
	while (Qn_ptr<Qn) {
		//do query by T[vk]+label[u]
		D_bfs = 99999;
		D = 99999;
		indt = &label[Q[Qn_ptr]]; // search index[u]
		if (indt->next != NULL) {
			indt = indt->next;
			while (indt != NULL) {
				if (T[indt->vt] != -1) { //check whether index[vk] have label with same v
					D_bfs = T[indt->vt] + indt->dis;
					if (D_bfs<D) { D = D_bfs; }
				}
				if (indt->next != NULL) { //move down to next label of index[vk]
					indt = indt->next;
				}
				else {
					goto endmatch;
				}
			}
		}
	endmatch:

		//Do pruning when D = QUERY(vk,u)< P[u]
		if (D <= P[Q[Qn_ptr]]) {
			//if(QUERY(Q[Qn_ptr],deg_s[vk],label,deg_r)<=P[Q[Qn_ptr]]){
			goto pruned;
			//add label 
		}
		else {
			if (R[Q[Qn_ptr]] == 0) {
				ind = (struct index*)malloc(sizeof(struct index));
				ind->vt = deg_s[vk];
				ind->dis = P[Q[Qn_ptr]];
				ind->next = NULL;
				indH = &label[Q[Qn_ptr]];  // add label to u
				while (indH->next != NULL) {
					//find the end part of index linklist to add a new one
					indH = indH->next;
				}
				indH->next = ind;
				R[Q[Qn_ptr]] = 1;
			}
			else {
				indH = &label[Q[Qn_ptr]];  // add label to u
				if (indH->next != NULL) {
					indH = indH->next;
					while (indH != NULL) {
						if (indH->vt == deg_s[vk]) {
							indH->dis = P[Q[Qn_ptr]];
							break;
						}
						while (indH->next != NULL) {
							indH = indH->next;
						}
					}
				}
			}
			//printf("%d\n",Qn);
			//put neigbours w of u to Q
			node_p = &node[Q[Qn_ptr]];
			node_p = node_p->next;
			while (node_p != NULL) {//put w to Q
				if (P[node_p->nb] == -1) {
					Q[Qn] = node_p->nb; // put w to Q
					P[Q[Qn]] = P[Q[Qn_ptr]] + node_p->dis; //update w 's BFS distance
					if (max_nbdis<P[Q[Qn]]) { max_nbdis = P[Q[Qn]]; }
					Qn++;
				}
				if (P[node_p->nb] != -1  ) {
					if (P[Q[Qn_ptr]] + node_p->dis<P[node_p->nb]) {
						P[node_p->nb] = P[Q[Qn_ptr]] + node_p->dis;
						if (R[node_p->nb]!=0) {
							Q[Qn] = node_p->nb;
						    Qn++;
						}
						
					}
				}
				if (node_p->next != NULL) {
					node_p = node_p->next;//move down to find another w
				}
				else {
					goto pruned;
				}
			}
		}
	pruned:
		Qn_ptr++;
	}
	Qn_ptr = 0;
	//reset P[vk],T[vk]that !=INF ,to (-1)INF
	while (Qn_ptr<Qn) {
		P[Q[Qn_ptr]] = -1;
		R[Q[Qn_ptr]] = 0;
		Qn_ptr++;
	}
	tn = &label[deg_s[vk]];
	if (tn->next != NULL) {
		tn = tn->next;
		while (tn != NULL) {
			T[tn->vt] = -1;
			if (tn->next != NULL) {
				tn = tn->next;
			}
			else {
				goto retn;
			}
		}
	}
retn:
	return 0;
}

//Query search of 2labels
int QUERY(uint32_t s1, uint32_t t1, struct index label[], uint32_t deg_r[]) {
	uint32_t D_query = 0;
	D = 99999;
	inds = &label[s1];
	indt = &label[t1];

	//ensure label of s and t is not empty
	if (inds->next != NULL && indt->next != NULL) {
		inds = inds->next;
		indt = indt->next;
	}
	else {
		return D;
	}
	//the point inds and indt move down the index 
	//when the index[s] and index[t] has some v or not
	while (inds != NULL && indt != NULL) {
		//	printf("s: %d;t: %d\n",inds->vt,indt->vt);
		if (deg_r[inds->vt] == deg_r[indt->vt]) { //find a vertex v
			D_query = (inds->dis + indt->dis);

			if (D_query<D) {
				D = D_query;
				v = inds->vt;
			}  //update D when shorter distance  is found
			if (inds->next != NULL && indt->next != NULL) {
				inds = inds->next;
				indt = indt->next;
			}
			else { return D; }
			//merge-join like (just a simple searching)
		}
		else if (deg_r[inds->vt] > deg_r[indt->vt]) {
			if (indt->next != NULL) {
				indt = indt->next;//
			}
			else { return D; }
		}
		else if (deg_r[inds->vt] < deg_r[indt->vt]) {
			if (inds->next != NULL) {
				inds = inds->next;//
			}
			else { return D; }
		}
	}
	return D;
}

//To-find-nodes of shortest-path of 2 nodes
int find_path(uint32_t s1, uint32_t t1, struct index label[], uint32_t deg_r[], struct graph node[]) {
	//   printf("check:%d & %d \n",s1,t1);
	uint32_t D_st = 0, vv = 0;
	D_st = QUERY(s1, t1, label, deg_r);  //backup distance of s1 to t1
										 //case that s=v || t=v
	if (v == s1 || v == t1) {
		if (s1 != t1) { //avoid error in s to s
			node_p = &node[s1];//find neigbours u of s
			node_p = node_p->next;
			while (node_p != NULL) {
				if (node_p->nb == t1) { //case that s is near to t
					goto finbd;
				}
				// find a neigbor u that QUERY(s,u)+QUERY(u,t)=QUERY(s,t) and avoid add t again 
				if (QUERY(node_p->nb, s1, label, deg_r) + QUERY(node_p->nb, t1, label, deg_r) == D_st) {
					vv = node_p->nb; //store the neigbor u
					pat = (struct path*)malloc(sizeof(struct path));
					pat->dis2s = QUERY(ss, s1, label, deg_r) + QUERY(s1, vv, label, deg_r);
					pat->vt = vv;
					pat->next = NULL;
					patH->next = pat;
					patH = patH->next;
					path_nodenum++; // count the number of nodes has added to Path[1]
					find_path(vv, t1, label, deg_r, node);//continue to find the nodes betweem v & t
					goto finbd;
				}
				else {
					if (node_p->next != NULL) {
						node_p = node_p->next;
					}
					else {
						goto finbd;
					}
				}
			}
		}
		//normal case that v != s or t
	}
	else {
		vv = v;
		pat = (struct path*)malloc(sizeof(struct path));
		pat->vt = vv; //store v to Path[1] directly
		pat->dis2s = QUERY(ss, s1, label, deg_r) + QUERY(s1, vv, label, deg_r);
		pat->next = NULL;
		patH->next = pat;
		patH = patH->next;
		path_nodenum++;
		find_path(s1, vv, label, deg_r, node);//continue to find the nodes between s & v 
		find_path(vv, t1, label, deg_r, node);//continue to find the nodes betweem v & t
	}
finbd:
	return 0;
}

//form-a-path-by nodes
int form_path() {
	uint32_t DD = 99999, counter = 0;
	patH = &Path[1];//target on unsorted
	pat = &Path[1];//target on sorted
	pat->sort_node = patH->next; //sort_node store the sorted path
	patH = patH->next;
	pat = pat->sort_node;
	//printf("%d ",pat->vt); //print s first as it must be the first one
	while (patH != NULL) {
		if (patH->dis2s < DD) {
			//ensure this node larger than the node got before in sorted path 
			if (patH->dis2s > pat->dis2s) {
				DD = patH->dis2s;
				pat_ptr = patH;  //use a pointer point to that node
			}
		}
		if (patH->next != NULL) {
			patH = patH->next;
		}
		else {
			patH = &Path[1];
			patH = patH->next;
			if (pat_ptr != NULL) {
				pat->sort_node = pat_ptr;//get that node
				pat = pat->sort_node;
				pat->sort_node = NULL;
			
				counter++;
			
			}
			DD = 99999;
			//stop when node t is find or enough node is sorted
			if (counter == path_nodenum || pat->vt == tt) {
				/// reset parameters
				path_nodenum = 0;
				pat = &Path[1];
				pat->next = NULL;
				pat_ptr = NULL;
				goto print_path;
			}
		}
	}
print_path:
	patH = &Path[1];
	patH = patH->sort_node;
	printf("%d ", patH->vt);
	while (patH->sort_node != NULL) {
		patH = patH->sort_node;
		printf("- %d ", patH->vt);
	}
	printf("\n");
	return 0;
}

//find the shortest path
int shortest_path(struct index label[], uint32_t deg_r[], struct graph node[]) {
	//put s and t to the unsorted path array
	pat = (struct path*)malloc(sizeof(struct path));
	pat->dis2s = 0;
	pat->vt = s;  //put s to Path[1] first
	patH = &Path[1];
	patH->next = pat;
	patH = patH->next;
	path_nodenum = 0; //number of nodes in the unsorted path array
	pat = (struct path*)malloc(sizeof(struct path));
	pat->dis2s = QUERY(s, t, label, deg_r);
	pat->vt = t;  //put t to Path[1] after s
	pat->next = NULL;
	patH->next = pat;
	patH = patH->next;
	path_nodenum++;
	ss = s; //save the value of s
	tt = t; //save the value of t

	find_path(s, t, label, deg_r, node); //find the nodes in path between s to t
	form_path();     //form the path and print it out
	return 0;
}


//To-find-nodes of diff. paths of 2 nodes 
int find_path2(uint32_t s1, uint32_t t1, struct index label[], uint32_t deg_r[], struct graph node[]) {
	//  printf("check:%d & %d \n",s1,t1);
	uint32_t D_st = 0, vv = 0;
	D_st = QUERY(s1, t1, label, deg_r);  //backup distance of s1 to t1
										 //case that s=v || t=v
	if (v == s1 || v == t1) {
		if (s1 != t1) { //avoid error in s to s
			node_p = &node[s1];//find neigbours u of s
			node_p = node_p->next;
			while (node_p != NULL) {
				if (node_p->nb == t1) { //case that s is near to t
					goto finbd;
				}
				// find a neigbor u that QUERY(s,u)+QUERY(u,t)=QUERY(s,t) and avoid add t again 
				if (QUERY(node_p->nb, s1, label, deg_r) + QUERY(node_p->nb, t1, label, deg_r) == D_st) {
					vv = node_p->nb; //store the neigbor u
					pat = (struct path*)malloc(sizeof(struct path));
					pat->dis2s = D_font + QUERY(v_nb, s1, label, deg_r) + QUERY(s1, vv, label, deg_r);
					pat->vt = vv;
					pat->next = NULL;
					patH->next = pat;
					patH = patH->next;
					path_nodenum++; // count the number of nodes has added to Path[1]
					find_path2(vv, t1, label, deg_r, node);//continue to find the nodes betweem v & t
					goto finbd;
				}
				else {
					if (node_p->next != NULL) {
						node_p = node_p->next;
					}
					else {
						goto finbd;
					}
				}
			}
		}
		////////////////////////////////////////////////////////
		//normal case that v != s or t
	}
	else {
		vv = v;
		pat = (struct path*)malloc(sizeof(struct path));
		pat->vt = vv; //store v to Path[1] directly
		pat->dis2s = D_font + QUERY(v_nb, s1, label, deg_r) + QUERY(s1, vv, label, deg_r);
		pat->next = NULL;
		patH->next = pat;
		patH = patH->next;
		path_nodenum++;
		find_path2(s1, vv, label, deg_r, node);//continue to find the nodes between s & v 
		find_path2(vv, t1, label, deg_r, node);//continue to find the nodes betweem v & t
	}
finbd:
	return 0;
}


//To-find-diff.paths
int other_path(struct index label[], uint32_t deg_r[], struct graph node[], uint32_t P[], uint32_t Q[]) {
	uint32_t D_other = 0, Qn = 0, Qn_p = 0, w = 0, num_path_ptr = 0, D_pre = 0;

	D_other = QUERY(s, t, label, deg_r); //shortest distance from s to t
	ss = s; //use ss and tt store the start and end point
	tt = t;
	printf("Shortest Distance: %d\n", D_other);
	shortest_path(label, deg_r, node); //get the nodes in shortest path
	pat = &Path[1];
	if (pat->sort_node != NULL) {
		pat = pat->sort_node;
		while (pat != NULL) {
			P[pat->vt] = 0;
			Q[Qn] = pat->vt;  //put the nodes in shortest path to Q
							  //printf("%d\n",Q[Qn]);
			Qn++;
			if (pat->sort_node != NULL) {
				pat = pat->sort_node;
			}
			else {
				goto down;
			}
		}
	}
down:
	//do BFS to find nodes u that form a path from s to u to t,
	//the distance similar to shortest one
	while (Qn_p<Qn) {
		path_nodenum = 0;
		//put nb  to Q 
		node_p = &node[Q[Qn_p]];
		node_p = node_p->next;
		while (node_p != NULL) {
			if (P[node_p->nb] == -1) {
				//ensure from s pass through u to t is short enough
				if (QUERY(ss, node_p->nb, label, deg_r) + QUERY(node_p->nb, tt, label, deg_r) <= D_other + max_thr) {
					//printf("v:%d\n",Q[Qn_p]);
					P[Q[Qn_p]] = 0;  //avoid repeated cases
					Q[Qn] = node_p->nb;
					P[Q[Qn]] = 1;
					Qn++;
				}

			}
			if (P[node_p->nb] == 1) {   //avoid repeated cases
				P[Q[Qn_p]] = 0;
			}
			if (node_p->next != NULL) {
				node_p = node_p->next;
			}
			else {
				goto endnb;
			}
		}
	endnb:
		//exclude node on shortest path and case that some nodes u in the same path
		if (P[Q[Qn_p]] != 0) { //

			pat = (struct path*)malloc(sizeof(struct path));
			pat->dis2s = 0;
			pat->vt = ss;  //add s to unsorted linklist Path[1] first
			patH = &Path[1];
			patH->next = pat;
			patH = patH->next;
			pat = (struct path*)malloc(sizeof(struct path));
			pat->dis2s = QUERY(ss, Q[Qn_p], label, deg_r) + QUERY(Q[Qn_p], tt, label, deg_r);
			pat->vt = tt;  //then add t to Path[1]
			patH->next = pat;
			patH = patH->next;
			path_nodenum++;
			printf("\nNB: %d Distance: %d\n", Q[Qn_p], pat->dis2s);
			pat = (struct path*)malloc(sizeof(struct path));
			pat->dis2s = QUERY(ss, Q[Qn_p], label, deg_r);
			pat->vt = Q[Qn_p];  //add the node u to Path[1]
			patH->next = pat;
			patH = patH->next;
			path_nodenum++;
			num_path_ptr++;
			D_font = QUERY(ss, Q[Qn_p], label, deg_r);
			v_nb = Q[Qn_p];
			find_path(ss, Q[Qn_p], label, deg_r, node);//find nodes from s to u
			find_path2(Q[Qn_p], tt, label, deg_r, node);//find nodes from u to t
			form_path();//form a path that from s pass through u to t
		}
		//reset parameters
		Qn_p++;
		if (num_path_ptr == num_path - 1) {
			Qn_p = 0;
			//reset P[vk],T[vk]that !=INF ,to INF
			while (Qn_p<Qn) {
				P[Q[Qn_p]] = -1;
				Qn_p++;;
			}
			return 0;
		}
	}
	Qn_p = 0;
	///reset P[vk],T[vk]that !=INF ,to INF
	while (Qn_p<Qn) {
		P[Q[Qn_p]] = -1;
		Qn_p++;;
	}
}

//check input from user
int checkinput(uint32_t in1, uint32_t in2, uint32_t thr, uint32_t path, uint32_t num_node) {

	if (in1<node_min || in1>num_node) {
		chek = 1;
	}
	if (in2<node_min || in2>num_node) {
		chek = 1;
	}
	if (thr<0 || path<1) {
		chek = 1;
	}
	return 0;
}

//show-fn-of-the-program
int selec(uint32_t num_node, struct graph node[], struct index label[], uint32_t deg_r[], uint32_t P[], uint32_t Q[]) {
	uint32_t sel = 0, nod = 0;
	printf("------------------------------\n|  Pruned Landmark Labeling  |\n------------------------------\n");
	while (1) {
		printf("\nPlease input from 1 to 5, or 6 to help, 7 to quit\n");
		scanf("%d", &sel);
		switch (sel) {
			//****************
		case 1: //print index[i]
			printf("Input from %d to %d\n", node_min, num_node);
			printf("node: ");
			scanf("%d", &nod);
			checkinput(nod, 1, 0, 1, num_node);
			if (chek == 1) {
				chek = 0;
				printf("Wrong input\n");
			}
			else {
				indH = &label[nod];
				indH = indH->next;
				printf("index[%d]\n", nod);
				while (indH != NULL) {
					printf("(%d,%d)\n", indH->vt, indH->dis);
					if (indH->next != NULL) {
						indH = indH->next;
					}
					else {
						break;
					}
				}
			}
			break;
			//****************
		case 2://print all index
			for (i = node_min; i <= num_node; i++) {
				indH = &label[i];
				indH = indH->next;
				printf("index[%d]\n", i);
				while (indH != NULL) {
					printf("(%d,%d)\n", indH->vt, indH->dis);
					if (indH->next != NULL) {
						indH = indH->next;
					}
					else {
						break;
					}
				}
			}
			break;
			//****************
		case 3://print neighbours 
			printf("Input from %d to %d\n", node_min, num_node);
			printf("node: ");
			scanf("%d", &nod);
			checkinput(nod, 1, 0, 1, num_node);
			if (chek == 1) {
				chek = 0;
				printf("Wrong input\n");
			}
			else {
				node_p = &node[nod];
				node_p = node_p->next;
				printf("neighbors:\n");
				while (node_p != NULL) {
					printf("(%d,%d)\n", node_p->nb, node_p->dis);
					if (node_p->next != NULL) {
						node_p = node_p->next;
					}
					else {
						break;
					}
				}
			}
			break;
			//****************
		case 4://find shortest distance and path
			printf("Input from %d to %d\n", node_min, num_node);
			s = 0, t = 0;
			printf("s: ");
			scanf("%d", &s);
			printf("t: ");
			scanf("%d", &t);
			checkinput(s, t, 0, 1, num_node);
			if (chek == 1) {
				chek = 0;
				printf("Wrong input\n");
			}
			else {
				printf("Result:\n\n");
				D = QUERY(s, t, label, deg_r);
				printf("Shortest distance: %d\n", D);
				printf("v: %d\n", v);
				printf("shortest path:\n");
				shortest_path(label, deg_r, node);
				printf("\n");
			}
			break;
			//****************
		case 5://find some paths
			printf("Input from %d to %d\n", node_min, num_node);
			printf("s: ");
			scanf("%d", &s);
			printf("t: ");
			scanf("%d", &t);
			printf("max threshold: ");
			scanf("%d", &max_thr);
			printf("max number of path: ");
			scanf("%d", &num_path);
			checkinput(s, t, max_thr, num_path, num_node);
			if (chek == 1) {
				chek = 0;
				printf("Wrong input\n");
			}
			else {
				printf("Result:\n\n");
				printf("The paths: \n");
				other_path(label, deg_r, node, P, Q);
			}
			break;
			//****************
		case 6://help
			printf("---------------------------------------------------------------------\n");
			printf("|[1]- Print the index table of node[?]                              |\n");
			printf("|[2]- Print the index table of all nodes                            |\n");
			printf("|[3]- Print the neighbours of node[?]                               |\n");
			printf("|[4]- Print the shortest distance and path from node[s] to node[t]  |\n");
			printf("|[5]- Print the some short distance and path from node[s] to node[t]|\n");
			printf("|[6]- Help                                                          |\n");
			printf("---------------------------------------------------------------------\n");
			break;
		case 7://quit
			printf("Bye\n");
			return 0;
			break;
		default:
			printf("Wrong input\n");
			printf("Input from %d to %d\n", node_min, num_node);
		}
	}
}



int main(int argc, char *argv[]) {
	uint32_t num_node = 0, node_n;
	uint32_t k = 1;
	char *arg1;
	i = 0;

	//read txt file name 
	if (argv[1] != NULL) {
		arg1 = argv[1];
		while (*arg1 != '\0') {
			txt[i] = *arg1;
			arg1++;
			i++;
		}
	}

	//read data
	num_node = get_node();
	node_n = num_node + 1;
	uint32_t *deg = new uint32_t[node_n], *deg_s = new uint32_t[node_n], *deg_r = new uint32_t[node_n];
	graph *node = new graph[node_n];
	index *label = new index[node_n];

	get_deg(num_node, node, label, deg, deg_s, deg_r);
	//printf("num of node: %d\nmin. dis.: %d\n",num_node,node_sdis);
	//for(i=0;i<=num_node;i++) printf("deg_s[%d]:%d\n",i,deg_s[i]);
	
	//BFS
	uint32_t *Q = new uint32_t[15000000], *T = new uint32_t[node_n], *P = new uint32_t[node_n], *R = new uint32_t[node_n];
	//set P[v],T[v] == -1(act as flag)
	for (i = 0; i <= num_node; i++) {

		P[i] = -1;
		T[i] = -1;
		R[i] = 0;
	}
	for (i = 0; i <= 15000000; i++) {
		Q[i] = 0;
	}

	//do BFS from vk :0 to num of node
	for (k = node_min; k <= num_node; k++) {

		BFS(k, node, label, P, Q, T, deg_s, num_node, deg_r, R);
	}
	printf("\nIndex Table completely formed\n");
	if (node_min == 0) {
		printf("Number of nodes: %d\n", num_node + 1);
	}
	else {
		printf("Number of nodes: %d\n", num_node);
	}

	selec(num_node, node, label, deg_r, P, Q);

	return 0;
}