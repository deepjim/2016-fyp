/*****************************************************************
*  CSCI 4999 Final Year Project II
*  Title: Querying Large Graph Databases
*  Algorithm : Timetable labelling
*  Code by Deep
****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <math.h> 


/*****************************************************************
* Char array to store output file names
****************************************************************/
char *data_file = new char[20], *order_file = new char[25], *lin_file = new char[25], *lout_file = new char[25];
char *lin_uv_file = new char[25], *lout_uv_file = new char[25], *lin_c_file = new char[25], *lout_c_file = new char[25];

/*****************************************************************
* Data set infos
****************************************************************/
long node_num = 0, edge_num = 0, max_out = 0, max_in = 0, bus_num = 0, deg_num = 0;

/*****************************************************************
* Struct to store out/in edges, degree of nodes of dataset
****************************************************************/
struct graph_data_out {
	long to_node;
	long ostart_t;
	long oend_t;
	long ob_id;
	int oran = 0;
	long cost = 0;
	struct graph_data_out *onext = NULL;
};
struct graph_data_out *go_ptr = NULL, *go_ptr2 = NULL, *go_hptr = NULL;

struct graph_data_in {
	long from_node;
	long istart_t;
	long iend_t;
	long ib_id;
	int iran = 0;
	long cost = 0;
	struct graph_data_in *inext = NULL;
};
struct graph_data_in *gi_ptr = NULL, *gi_ptr2 = NULL, *gi_hptr = NULL;


struct graph_data_deg {
	long in_deg = 0;
	long out_deg = 0;
};

/*****************************************************************
* hash table, heap for building Index
****************************************************************/
struct hash_table {
	long eat = 999999;
	long veh = -1;
	long piv = -1;
	long cost = 0;
};

struct mini_heap {
	long x;
	long y;
	long td;
	long ta;
	long b;
	long p;
	long cost = 0;
};

/*****************************************************************
* Struct for heuristic node ordering
****************************************************************/
struct edge {
	long efrom_node;
	long eto_node;
	long estart_t;
	long eend_t;
	long eb_id;
};

struct EAP_tree {
	long *ba = new long[2000];
	long *cno = new long[2000];
	long *coverage = new long[2000];
	long ord = 0;
};

/*****************************************************************
* Struct to store Index
****************************************************************/
struct index_in {
	long iui;
	long itd;
	long ita;
	long ibv;
	long ipv;
	long cost = 0;
	struct index_in *iindnt = NULL;
};
struct index_out {
	long oui;
	long otd;
	long ota;
	long obv;
	long opv;
	long cost = 0;
	struct index_out *oindnt = NULL;
};
struct index { // for pathunfold
	long ui;
	long td;
	long ta;
	long bv;
	long pv;
	long cost = 0;
};
struct index_in *iind_ptr1 = NULL, *iind_ptr2 = NULL;
struct index_out *oind_ptr1 = NULL, *oind_ptr2 = NULL;

/*****************************************************************
* Struct to store Sketch
****************************************************************/
struct sketch {
	long uw = -1;
	long utd;
	long uta;
	long ubv;
	long upv;
	long vw = -1;
	long vtd;
	long vta;
	long vbv;
	long vpv;
	long cost;
	struct sketch *sk_next = NULL;
};
struct sketch sketches, *sk_ptr = NULL, *sk_hptr = NULL, *l_sk = NULL, *r_sk = NULL;
/*****************************************************************
* Struct to store routes
****************************************************************/
struct path {
	long pnode;
	long td;
	long ta;
	long cost;
	struct path *pnode_next = NULL;
};

struct path sans, *path1_ptr = NULL, *path2_ptr = NULL, *path3_ptr = NULL;


/*****************************************************************
*  function prototypes
****************************************************************/

int get_node_num(char data_file[]);
int read_gdata(struct graph_data_out go_dat[], struct graph_data_in gi_dat[], struct graph_data_deg g_dat_deg[]);
int heu_order(struct graph_data_out go_dat[], struct graph_data_in gi_dat[], struct graph_data_deg g_dat_deg[], long node_order[]);
void read_ord_filename();
int check_ord_file();
int read_order_data(long node_order[], long order_node[], struct graph_data_deg g_dat_deg[]);
void read_lab_filename();
int read_sdat(struct graph_data_out go_dat[], struct graph_data_in gi_dat[], struct graph_data_deg g_dat_deg[], long node_num);
void build_index(struct graph_data_out go_dat[], struct graph_data_in gi_dat[], struct graph_data_deg g_dat_deg[], long node_order[], long order_node[], struct index_in label_in[], struct index_out label_out[], long wt);
mini_heap sort_hp(struct mini_heap heap[], long hp, long heap_ptr, long c);
mini_heap sort_hp2(struct mini_heap heap[], long hp, long heap_ptr, long c);
void read_label_data(struct index_in label_in[], struct index_out label_out[]);
int sketchgen(long order_node[], struct index_in label_in[], struct index_out label_out[], long u, long v, long std, long sta);
void pathconst(long order_node[], struct index_in label_in[], struct index_out label_out[], long u, long v, long flag);
void pathunfold(struct index child, long order_node[], struct index_in label_in[], struct index_out label_out[], long u, long v);
sketch findchild(struct index child, long order_node[], struct index_in label_in[], struct index_out label_out[], long u, long v);
sketch child_sketchgen(struct index child, long order_node[], struct index_in label_in[], struct index_out label_out[], long u, long v);
void print_path();
void build_index_uv(struct graph_data_out go_dat[], struct graph_data_in gi_dat[], struct graph_data_deg g_dat_deg[], long node_order[], long order_node[], struct index_in label_in[], struct index_out label_out[]);
void read_label_uv_data(struct index_in label_in[], struct index_out label_out[]);
int sketchgen_uv(long order_node[], struct index_in label_in[], struct index_out label_out[], long u, long v, long std, long sta);
void build_index_c(struct graph_data_out go_dat[], struct graph_data_in gi_dat[], struct graph_data_deg g_dat_deg[], long node_order[], long order_node[], struct index_in label_in[], struct index_out label_out[]);
void read_label_c_data(struct index_in label_in[], struct index_out label_out[]);
int sketchgen_c(long order_node[], struct index_in label_in[], struct index_out label_out[], long u, long v, long std, long sta);
int usage(struct graph_data_out go_dat[], struct graph_data_in gi_dat[], struct graph_data_deg g_dat_deg[], struct index_in label_in[], struct index_out label_out[], long node_order[], long order_node[], struct index_in label_uv_in[], struct index_out label_uv_out[], struct index_in label_c_in[], struct index_out label_c_out[]);

/*****************************************************************
* Main Function  code by Deep
****************************************************************/
int main(int argc, char *argv[]) {
	int i = 0;

	if (argv[1] != NULL) { // get data file name
		strcpy(data_file, argv[1]);
		printf("data set: %s\n", data_file);
	}

	get_node_num(data_file);// get # of nodes
	printf("# of nodes: %d\n", node_num);

	graph_data_out *go_dat = new graph_data_out[node_num];// allocate memory for all needs
	graph_data_in *gi_dat = new graph_data_in[node_num];
	graph_data_deg *g_dat_deg = new graph_data_deg[node_num];
	index_in *label_in = new index_in[node_num];
	index_out *label_out = new index_out[node_num];
	index_in *label_uv_in = new index_in[node_num];
	index_out *label_uv_out = new index_out[node_num];
	index_in *label_c_in = new index_in[node_num];
	index_out *label_c_out = new index_out[node_num];
	long *node_order = new long[node_num], *order_node = new long[node_num];


	read_gdata(go_dat, gi_dat, g_dat_deg); // read data
	read_sdat(go_dat, gi_dat, g_dat_deg, node_num);
	bus_num++;
	printf("# of edges: %d\n", edge_num);
	printf("# of Bus: %d\n", bus_num);

	// select the job
	usage(go_dat, gi_dat, g_dat_deg, label_in, label_out, node_order, order_node, label_uv_in, label_uv_out, label_c_in, label_c_out);

	return 0;
}

/*****************************************************************
* get_node_num() get the node number of dataset
****************************************************************/
int get_node_num(char data_file[]) {
	FILE *data1;
	data1 = fopen(data_file, "r");
	if (!data1) {
		printf("Data File not exist\n");
		return 0;
	}
	fscanf(data1, "%d", &node_num);
	fclose(data1);
	return 0;
}

/*****************************************************************
* read_gdata() read the road network dataset
****************************************************************/
int read_gdata(struct graph_data_out go_dat[], struct graph_data_in gi_dat[], struct graph_data_deg g_dat_deg[]) {
	long nod = 0, to_nod = 0, s_t = 0, bid = 0, du2;// node, to node, start time, duration, bus id
	int oB = 0, iB = 0;
	long du = 0, du3 = 60;
	FILE *data1;

	data1 = fopen(data_file, "r");
	if (!data1) {
		printf("File not exist\n");
		return 0;
	}
	fscanf(data1, "%d", &node_num);

	while (fscanf(data1, "%d:", &nod) == 1) {

		go_hptr = &go_dat[nod];

		while (fscanf(data1, "%d", &to_nod) == 1) {
			if ((int)to_nod == -1) break;
			gi_hptr = &gi_dat[to_nod];

			while (gi_hptr->inext != NULL) {
				gi_hptr = gi_hptr->inext;
			}

			fscanf(data1, "%d %d %d", &s_t, &du, &bid);

			if (bid > 5000) bid /=10 ;
			if (bid > bus_num) bus_num = bid;
			du2 = du / du3;
			go_ptr = (struct graph_data_out*)malloc(sizeof(struct graph_data_out));// out edge of 'nod'
			go_ptr->to_node = to_nod;
			go_ptr->ostart_t = s_t;
			go_ptr->oend_t = s_t + du;
			go_ptr->ob_id = bid;
			go_ptr->oran = 0;
			go_ptr->cost = du2;
			go_ptr->onext = NULL;
			go_hptr->onext = go_ptr;
			go_hptr = go_hptr->onext;

			gi_ptr = (struct graph_data_in*)malloc(sizeof(struct graph_data_in));// in edge of 'nod'
			gi_ptr->from_node = nod;
			gi_ptr->istart_t = s_t;
			gi_ptr->iend_t = s_t + du;
			gi_ptr->ib_id = bid;
			gi_ptr->cost = du2;
			gi_ptr->inext = NULL;
			gi_hptr->inext = gi_ptr;
			gi_hptr = gi_hptr->inext;

			g_dat_deg[nod].out_deg++;
			g_dat_deg[to_nod].in_deg++;
			edge_num++;
		}
	}
	fclose(data1);
	return 0;
}

/*********************************************************************
* read_sdat() sort the data of road network dataset for build index
*********************************************************************/
int read_sdat(struct graph_data_out go_dat[], struct graph_data_in gi_dat[], struct graph_data_deg g_dat_deg[], long node_num) {
	long i = 0, j = 0, k = 0, deg = 0;
	graph_data_out go_ins;
	graph_data_in gi_ins;
	for (i = 0; i < node_num; i++) {

		deg = g_dat_deg[i].out_deg;//out
		for (j = 1; j < deg; j++) {

			go_ptr = &go_dat[i];
			if (go_ptr->onext != NULL) {
				go_ptr = go_ptr->onext;
			}
			else {
				goto next;
			}
			while (go_ptr->onext != NULL) {
				go_ptr2 = go_ptr->onext;

				if (go_ptr->ostart_t > go_ptr2->ostart_t) {
					//	printf("%d-%d\n", gi_ptr->istart_t, gi_ptr2->istart_t);

					go_ins.ostart_t = go_ptr->ostart_t;
					go_ins.oend_t = go_ptr->oend_t;
					go_ins.to_node = go_ptr->to_node;
					go_ins.ob_id = go_ptr->ob_id;
					go_ins.cost = go_ptr->cost;

					go_ptr->ostart_t = go_ptr2->ostart_t;
					go_ptr->oend_t = go_ptr2->oend_t;
					go_ptr->to_node = go_ptr2->to_node;
					go_ptr->ob_id = go_ptr2->ob_id;
					go_ptr->cost = go_ptr2->cost;

					go_ptr2->ostart_t = go_ins.ostart_t;
					go_ptr2->oend_t = go_ins.oend_t;
					go_ptr2->to_node = go_ins.to_node;
					go_ptr2->ob_id = go_ins.ob_id;
					go_ptr2->cost = go_ins.cost;


				}
				else {
					go_ptr = go_ptr->onext;
				}
			}
		next:;

		}







		deg = g_dat_deg[i].in_deg;//in
		for (j = 1; j < deg; j++) {

			gi_ptr = &gi_dat[i];
			if (gi_ptr->inext != NULL) {
				gi_ptr = gi_ptr->inext;
			}
			else {
				goto next2;
			}
			while (gi_ptr->inext != NULL) {
				gi_ptr2 = gi_ptr->inext;

				if (gi_ptr->istart_t < gi_ptr2->istart_t) {
					//	printf("%d-%d\n", gi_ptr->istart_t, gi_ptr2->istart_t);

					gi_ins.istart_t = gi_ptr->istart_t;
					gi_ins.iend_t = gi_ptr->iend_t;
					gi_ins.from_node = gi_ptr->from_node;
					gi_ins.ib_id = gi_ptr->ib_id;
					gi_ins.cost = gi_ptr->cost;


					gi_ptr->istart_t = gi_ptr2->istart_t;
					gi_ptr->iend_t = gi_ptr2->iend_t;
					gi_ptr->from_node = gi_ptr2->from_node;
					gi_ptr->ib_id = gi_ptr2->ib_id;
					gi_ptr->cost = gi_ptr2->cost;

					gi_ptr2->istart_t = gi_ins.istart_t;
					gi_ptr2->iend_t = gi_ins.iend_t;
					gi_ptr2->from_node = gi_ins.from_node;
					gi_ptr2->ib_id = gi_ins.ib_id;
					gi_ptr2->cost = gi_ins.cost;


				}
				else {
					gi_ptr = gi_ptr->inext;
				}
			}
		next2:;

		}



	}






	return 0;
}
/*****************************************************************
* heu_order() order the nodes using heuristic method of the paper
******************************************************************/
int heu_order(struct graph_data_out go_dat[], struct graph_data_in gi_dat[], struct graph_data_deg g_dat_deg[], long node_order[]) {
	long i = 0, j = 0, heap_ptr = 0, hp = 0, heu_edge_num = 0, son_ptr = -1, up_ptr = 0, son_cov = 0;
	long max_c = -1, max_u = -1, order_ptr = 0, ran_num = 0, ran_deg = 0, ran_flag = 0, tree_ptr = 0;
	FILE *ord_out;

	hash_table *htb = new hash_table[node_num];
	mini_heap *heap = new mini_heap[edge_num + 1];
	EAP_tree *tree = new EAP_tree[node_num];
	long *updat = new long[node_num], *tot_cov = new long[node_num], *root = new long[2000];// bool update, total coverage, tree root 

	for (i = 0; i<node_num; i++) { // init
		tot_cov[i] = 0;
		tree[i].ord = 0;
	}
	srand(time(NULL)); // rand seed

	while (heu_edge_num < edge_num * 8 && tree_ptr + 1 <= edge_num) { // form EAP trees until heu_edge_num >= edge_num * 8

		hp = 0, heap_ptr = 0, ran_flag = 0;// init before each loop
		for (i = 0; i < node_num; i++) {
			htb[i].eat = 999999;
		}

		while (ran_flag == 0) { // loop until new rand edge is found
			ran_num = rand() % node_num; // rand node
			if (g_dat_deg[ran_num].out_deg>0) {
				ran_deg = rand() % (g_dat_deg[ran_num].out_deg) + 1; // rand edge from 1 to deg.value

				go_ptr = &go_dat[ran_num];
				go_ptr = go_ptr->onext;
				while (ran_deg > 1) {
					go_ptr = go_ptr->onext;
					ran_deg--;
				}
				if (go_ptr->oran == 0) { // check have rand?
					htb[ran_num].eat = go_ptr->ostart_t;
					heap[heap_ptr].y = ran_num;
					heap_ptr++;
					go_ptr->oran = 1;
					ran_flag = 1;
					root[tree_ptr] = ran_num;
					for (i = 0; i < node_num; i++) { // init 
						tree[i].ba[tree_ptr] = -1;
						tree[i].cno[tree_ptr] = 0;
						tree[i].coverage[tree_ptr] = 0;
					}
				}

			}
		}

		while (hp < heap_ptr) {
			go_ptr = &go_dat[heap[hp].y];// dijkstra
			if (go_ptr->onext != NULL) {
				go_ptr = go_ptr->onext;
				while (go_ptr != NULL) {
					if (go_ptr->ostart_t >= htb[heap[hp].y].eat && go_ptr->oend_t < htb[go_ptr->to_node].eat) {
						htb[go_ptr->to_node].eat = go_ptr->oend_t; //update htb.eat
						heap[heap_ptr].y = go_ptr->to_node; //add y to heap
						if (tree[go_ptr->to_node].ba[tree_ptr] == -1) { // ba = -1
							tree[go_ptr->to_node].ba[tree_ptr] = heap[hp].y;
							tree[heap[hp].y].cno[tree_ptr]++;
						}
						else {
							tree[tree[go_ptr->to_node].ba[tree_ptr]].cno[tree_ptr]--; // ba != -1
							tree[go_ptr->to_node].ba[tree_ptr] = heap[hp].y;
							tree[heap[hp].y].cno[tree_ptr]++;
						}
						heap_ptr++;
					}
					if (go_ptr->onext != NULL) {
						go_ptr = go_ptr->onext;
					}
					else {
						break;
					}
				}
			}
			hp++;
		}

		for (i = 0; i < node_num; i++) { // count edge in EAP trees
			if (htb[i].eat < 999999) heu_edge_num++;
		}
		tree_ptr++;
		//	printf("heu_edge_num: %d\n",heu_edge_num); // for debug

	}

	printf("\n# of EAP trees: %d\n", tree_ptr);

	//ordering
	//find 0th total coverage
	for (j = 0; j < tree_ptr; j++) {
		for (i = 0; i < node_num; i++) {
			if (tree[i].cno[j] == 0 && tree[i].ba[j] != -1) { // is-leave?
				tree[i].coverage[j]++;
				son_ptr = i;
				son_cov = tree[son_ptr].coverage[j];
				while (son_ptr != root[j]) {// != root
					if (tree[tree[son_ptr].ba[j]].coverage[j] == 0) {// 1st branch under a node
						tree[tree[son_ptr].ba[j]].coverage[j] = tree[son_ptr].coverage[j] + 1;
						son_cov = tree[son_ptr].coverage[j] + 1; // coverage of the sub tree
					}
					else {// 2nd - nth branch
						tree[tree[son_ptr].ba[j]].coverage[j] += son_cov;
					}
					son_ptr = tree[son_ptr].ba[j];
				}
			}
		}
	}

	for (j = 0; j < tree_ptr; j++) { // find max_u, u = node
		for (i = 0; i < node_num; i++) {
			tot_cov[i] += tree[i].coverage[j];
		}
	}
	for (i = 0; i < node_num; i++) { // find max_c
		if (tot_cov[i] > max_c) {
			max_c = tot_cov[i];
			max_u = i;
		}
	}
	node_order[order_ptr] = max_u;

	printf("order[%d]:%5d  cov:%d\n", order_ptr, node_order[order_ptr], max_c);

	ord_out = fopen(order_file, "w");// save order
	if (ord_out == NULL) {
		printf("Fail to save order\n");
		return 0;
	}
	fprintf(ord_out, "%d\n", max_u);
	order_ptr++;


	//find orders by rm sub-trees of max_u
	while (order_ptr < node_num) {
		tree[max_u].ord = 1; //mark it as ordered

		for (j = 0; j < tree_ptr; j++) {
			for (i = 0; i < node_num; i++) {
				if (tree[i].cno[j] == 0 && tree[i].ba[j] != -1) { // is-leave?
					tree[i].cno[j] = -1;
					updat[up_ptr] = i;
					up_ptr++;
					son_ptr = i;
					while (son_ptr != max_u && son_ptr != -1) { // add branches to updat[]
						updat[up_ptr] = tree[son_ptr].ba[j];
						up_ptr++;
						son_ptr = tree[son_ptr].ba[j];
					}
					up_ptr--;
					if (son_ptr == max_u) {//**
						while (up_ptr > -1) {// update tot cov of that branch under max_u
							tot_cov[updat[up_ptr]] -= tree[updat[up_ptr]].coverage[j];
							tree[updat[up_ptr]].coverage[j] = 0;
							up_ptr--;
						}
						if (tree[son_ptr].ba[j] != -1) {// update tot cov of that branch above max_u
							if (tree[tree[son_ptr].ba[j]].cno[j] == 1) {
								tree[tree[son_ptr].ba[j]].cno[j] = -2; // will change to 0 later
							}
							else {
								tree[tree[son_ptr].ba[j]].cno[j]--;
							}
							if (tree[son_ptr].coverage[j] != 0) {// only rm once
								tree[son_ptr].coverage[j] = 0; // max_u
								while (tree[son_ptr].ba[j] != -1) {
									tree[tree[son_ptr].ba[j]].coverage[j] -= max_c;
									tot_cov[tree[son_ptr].ba[j]] -= max_c;
									son_ptr = tree[son_ptr].ba[j];
								}
							}
						}
						up_ptr = 0;
					}
					else {
						up_ptr = 0; // that branch don't have max_u
					}
				}
			}
		}

		max_c = -1;

		for (j = 0; j < tree_ptr; j++) {
			for (i = 0; i < node_num; i++) {
				if (tree[i].cno[j] == -2) tree[i].cno[j] = 0; // update child's pos
			}
		}
		for (i = 0; i < node_num; i++) { // find next max_u
			if (tot_cov[i] > max_c && tree[i].ord == 0) {
				max_c = tot_cov[i];
				max_u = i;
			}
		}
		node_order[order_ptr] = max_u;
		tree[max_u].ord = 1;
		fprintf(ord_out, "%d\n", max_u);
		printf("order[%d]:%5d  cov:%d\n", order_ptr, node_order[order_ptr], max_c);

		order_ptr++;
	}
	printf("\nordered is saved as %s\n", order_file);

	fclose(ord_out);
	return 0;
}

/*****************************************************************
* sort_hp() sort the heap by asc order of arrival time
******************************************************************/
mini_heap sort_hp(struct mini_heap heap[], long hp, long heap_ptr, long c) {
	long i = 0, j = 0, k = 0;
	mini_heap ins_hp;
	k = heap_ptr - hp;
	if (k == 0) {
		return *heap;
	}
	for (j = 0; j < k; j++) {

		for (i = hp; i < heap_ptr; i++) {

			if (heap[i].ta>heap[i + 1].ta) {
				ins_hp.x = heap[i + 1].x;
				ins_hp.y = heap[i + 1].y;
				ins_hp.td = heap[i + 1].td;
				ins_hp.ta = heap[i + 1].ta;
				ins_hp.b = heap[i + 1].b;
				ins_hp.p = heap[i + 1].p;
				if (c == 1)ins_hp.cost = heap[i + 1].cost;
				heap[i + 1].x = heap[i].x;
				heap[i + 1].y = heap[i].y;
				heap[i + 1].td = heap[i].td;
				heap[i + 1].ta = heap[i].ta;
				heap[i + 1].b = heap[i].b;
				heap[i + 1].p = heap[i].p;
				if (c == 1)heap[i + 1].cost = heap[i].cost;
				heap[i].x = ins_hp.x;
				heap[i].y = ins_hp.y;
				heap[i].td = ins_hp.td;
				heap[i].ta = ins_hp.ta;
				heap[i].b = ins_hp.b;
				heap[i].p = ins_hp.p;
				if (c == 1)heap[i].cost = ins_hp.cost;
			}
		}
		heap_ptr--;
	}
	return *heap;
}

/*****************************************************************
* sort_hp2() sort the heap by dec order of departure time
******************************************************************/
mini_heap sort_hp2(struct mini_heap heap[], long hp, long heap_ptr, long c) {
	long i = 0, j = 0, k = 0;
	mini_heap ins_hp;
	k = heap_ptr - hp;
	if (k == 0) {

		return *heap;
	}
	for (j = 0; j < k; j++) {

		for (i = hp; i < heap_ptr; i++) {

			if (heap[i].td<heap[i + 1].td) {
				ins_hp.x = heap[i + 1].x;
				ins_hp.y = heap[i + 1].y;
				ins_hp.td = heap[i + 1].td;
				ins_hp.ta = heap[i + 1].ta;
				ins_hp.b = heap[i + 1].b;
				ins_hp.p = heap[i + 1].p;
				if (c == 1) ins_hp.cost = heap[i + 1].cost;
				heap[i + 1].x = heap[i].x;
				heap[i + 1].y = heap[i].y;
				heap[i + 1].td = heap[i].td;
				heap[i + 1].ta = heap[i].ta;
				heap[i + 1].b = heap[i].b;
				heap[i + 1].p = heap[i].p;
				if (c == 1) heap[i + 1].cost = heap[i].cost;
				heap[i].x = ins_hp.x;
				heap[i].y = ins_hp.y;
				heap[i].td = ins_hp.td;
				heap[i].ta = ins_hp.ta;
				heap[i].b = ins_hp.b;
				heap[i].p = ins_hp.p;
				if (c == 1) heap[i].cost = ins_hp.cost;
			}
		}
		heap_ptr--;
	}
	return *heap;
}

/**********************************************************************
* build_index() perform dijkstra search to build Index(original method)
***********************************************************************/
void build_index(struct graph_data_out go_dat[], struct graph_data_in gi_dat[], struct graph_data_deg g_dat_deg[], long node_order[], long order_node[], struct index_in label_in[], struct index_out label_out[], long wt) {
	long order_ptr = 0, t = 0, pt = 999999, deg = 0, t_deg = 0, flag = 0, lab = 0;
	long hp = 0, heap_ptr = 0, i = 0, j = 0, k = 0;
	long *timekeep = new long[node_num];
	hash_table *htb = new hash_table[node_num];
	mini_heap *heap = new mini_heap[edge_num + 1];
	FILE *labin = fopen(lin_file, "w");
	FILE *labout = fopen(lout_file, "w");
	struct edge_flag { // mark for searched edge
		long *eout = new long[max_out + 1];
		long *ein = new long[max_in + 1];
	};
	edge_flag *eflag = new edge_flag[node_num];
	for (i = 0; i < node_num; i++) {
		for (j = 0; j < max_out + 2; j++) eflag[i].eout[j] = 0;
		for (j = 0; j < max_in + 2; j++) eflag[i].ein[j] = 0;
		htb[i].eat = 999999;
		htb[i].piv = -1;
		htb[i].veh = -1;
	}
	wt *= 60;

	while (order_ptr < node_num) { // for 1:N 
		printf("node: %d order: %d  \n", node_order[order_ptr], order_ptr);
		deg_num = g_dat_deg[node_order[order_ptr]].out_deg;
		while (lab == 0) {
			t = -1, deg = 0, hp = 1, heap_ptr = 1;// init
			for (i = 0; i < node_num; i++) {
				htb[i].eat = 999999;
				htb[i].piv = -1;
				htb[i].veh = -1;
			}
			for (i = 0; i < node_num; i++) {
				timekeep[i] = 999999;
			}
			if (g_dat_deg[node_order[order_ptr]].out_deg > 0) {// find max td
				go_ptr = &go_dat[node_order[order_ptr]];
				go_ptr = go_ptr->onext;
				while (go_ptr != NULL) {
					deg++;
					if (eflag[node_order[order_ptr]].eout[deg] == 0 && g_dat_deg[go_ptr->to_node].in_deg != 0) {
						if (go_ptr->ostart_t > t && go_ptr->ostart_t < pt) { // find max td that < previous max td
							t = go_ptr->ostart_t;
							go_hptr = go_ptr;
							t_deg = deg;
						}
					}
					if (go_ptr->onext != NULL) {
						go_ptr = go_ptr->onext;
					}
					else {
						break;
					}
				}
			}

			if (t != -1) {// have found a td
			

				deg_num--;
				pt = t, deg = 0;
				eflag[node_order[order_ptr]].eout[t_deg] = 1;
				heap[heap_ptr].x = node_order[order_ptr];// add max td edge to heap
				heap[heap_ptr].y = go_hptr->to_node;
				heap[heap_ptr].td = go_hptr->ostart_t;
				heap[heap_ptr].ta = go_hptr->oend_t;
				heap[heap_ptr].b = go_hptr->ob_id;
				heap[heap_ptr].p = -1;
				heap_ptr++;

				go_ptr = &go_dat[node_order[order_ptr]];// add same td edge to heap
				go_ptr = go_ptr->onext;
				while (go_ptr != NULL) {
					deg++;
					if (eflag[node_order[order_ptr]].eout[deg] == 0 && g_dat_deg[go_ptr->to_node].in_deg != 0) {
						if (go_ptr->ostart_t == t) {
							heap[heap_ptr].x = node_order[order_ptr];
							heap[heap_ptr].y = go_ptr->to_node;
							heap[heap_ptr].td = go_ptr->ostart_t;
							heap[heap_ptr].ta = go_ptr->oend_t;
							heap[heap_ptr].b = go_ptr->ob_id;
							heap[heap_ptr].p = -1;
							heap_ptr++;
							eflag[node_order[order_ptr]].eout[deg] = 1;
							deg_num--;
						}
					}
					if (go_ptr->onext != NULL) {
						go_ptr = go_ptr->onext;
					}
					else {
						break;
					}
				}


				while (hp < heap_ptr) { //dijk search
					
					flag = 0;


					if (htb[heap[hp].x].eat == 999999) flag = 1;
					if (heap[hp].td >= (htb[heap[hp].x].eat) && heap[hp].ta < htb[heap[hp].y].eat) flag = 1;
					if (flag == 1) { // update htb
						htb[heap[hp].y].eat = heap[hp].ta;
						htb[heap[hp].y].piv = heap[hp].p;
						if (htb[heap[hp].y].veh == -1) {
							htb[heap[hp].y].veh = heap[hp].b;
						}
						else if (htb[heap[hp].y].veh != heap[hp].b) {
							htb[heap[hp].y].veh = -1;
						}

						if (g_dat_deg[heap[hp].y].out_deg > 0) {// add edge of to heap
							go_ptr = &go_dat[heap[hp].y];
							go_ptr = go_ptr->onext;
							while (go_ptr != NULL) {

								if (go_ptr->oend_t < timekeep[go_ptr->to_node]) {
									if (g_dat_deg[go_ptr->to_node].in_deg != 0 && go_ptr->to_node != node_order[order_ptr]) {
										if (go_ptr->ob_id != heap[hp].b) {
											if (go_ptr->ostart_t < (htb[heap[hp].y].eat + wt)) goto no;
											heap[heap_ptr].b = -1;
										}
										else {
											heap[heap_ptr].b = heap[hp].b;
										}
										timekeep[go_ptr->to_node] = go_ptr->oend_t;
										heap[heap_ptr].x = heap[hp].y;
										heap[heap_ptr].y = go_ptr->to_node;
										heap[heap_ptr].td = go_ptr->ostart_t;
										heap[heap_ptr].ta = go_ptr->oend_t;

										if (heap[hp].p == -1) {
											heap[heap_ptr].p = heap[hp].y;
										}
										else {
											if (order_node[heap[hp].p] < order_node[heap[hp].y]) {
												heap[heap_ptr].p = heap[hp].p;
											}
											else {
												heap[heap_ptr].p = heap[hp].y;
											}
										}

										heap_ptr++;
									}
								}
							no:
								if (go_ptr->onext != NULL) {
									go_ptr = go_ptr->onext;
								}
								else {
									*heap = sort_hp(heap, hp + 1, heap_ptr - 1, 0);
									break;
								}

							}
						}
					}
					hp++;
				}

				for (i = 0; i < node_num; i++) { // insert labels

					if (i != node_order[order_ptr] && htb[i].eat != 999999) {
						
						flag = 0;

						iind_ptr1 = &label_in[i];
						if (iind_ptr1->iindnt != NULL) {
							iind_ptr1 = iind_ptr1->iindnt;
							while (iind_ptr1 != NULL) {
								if (iind_ptr1->iui == node_order[order_ptr] && iind_ptr1->ita <= htb[i].eat) {
									flag = 1;
									break;
								}

								if (iind_ptr1->iindnt != NULL) {
									iind_ptr1 = iind_ptr1->iindnt;
								}
								else {
									break;
								}
							}
						}

						if (flag == 0) {
							iind_ptr1 = &label_in[i];
							oind_ptr1 = &label_out[node_order[order_ptr]];

							if (iind_ptr1->iindnt != NULL && oind_ptr1->oindnt != NULL) {
								iind_ptr1 = iind_ptr1->iindnt;
								oind_ptr1 = oind_ptr1->oindnt;
							}
							else {
								goto nomid;
							}

							while (iind_ptr1 != NULL && oind_ptr1 != NULL) {


								if (iind_ptr1->iui == oind_ptr1->oui) {

									if (oind_ptr1->ota < iind_ptr1->itd) {
										if (oind_ptr1->otd >= pt && iind_ptr1->ita <= htb[i].eat) {
											flag = 1;
											break;
										}
									}
									iind_ptr1 = iind_ptr1->iindnt;
									oind_ptr1 = oind_ptr1->oindnt;
								}
								else if (order_node[iind_ptr1->iui] < order_node[oind_ptr1->oui]) {

									if (iind_ptr1->iindnt != NULL) {
										iind_ptr1 = iind_ptr1->iindnt;
									}
									else {
										break;
									}
								}
								else if (order_node[iind_ptr1->iui] > order_node[oind_ptr1->oui]) {

									if (oind_ptr1->oindnt != NULL) {
										oind_ptr1 = oind_ptr1->oindnt;
									}
									else {
										break;
									}
								}
							}

						}
					nomid:

						if (flag == 0) {

							iind_ptr1 = &label_in[i];
							while (iind_ptr1->iindnt != NULL) {
								iind_ptr1 = iind_ptr1->iindnt;
							}
							iind_ptr2 = (struct index_in*)malloc(sizeof(struct index_in));
							iind_ptr2->iui = node_order[order_ptr];
							iind_ptr2->itd = pt;
							iind_ptr2->ita = htb[i].eat;
							iind_ptr2->ibv = htb[i].veh;
							iind_ptr2->ipv = htb[i].piv;
							iind_ptr2->iindnt = NULL;
							iind_ptr1->iindnt = iind_ptr2;
							fprintf(labin, "%ld %ld %ld %ld %ld %ld\n", i, iind_ptr2->iui, iind_ptr2->itd, iind_ptr2->ita, iind_ptr2->ibv, iind_ptr2->ipv);
							//		printf("in: i: %d: ui: %ld td: %ld ta: %ld bv: %ld pv: %ld\n", i, iind_ptr2->iui, iind_ptr2->itd/3600, iind_ptr2->ita/3600, iind_ptr2->ibv, iind_ptr2->ipv);
							
						}
						else {
							flag = 0;
						}
					}
				}
			}
			else {
				lab = 1;
				pt = -1;
				deg_num = g_dat_deg[node_order[order_ptr]].in_deg;
			}
		}
		while (lab == 1) {// reverse dijk 

			t = 999999, deg = 0, hp = 1, heap_ptr = 1; // init
			for (i = 0; i < node_num; i++) {
				htb[i].eat = -1; //LDT , save space
				htb[i].piv = -1;
				htb[i].veh = -1;
			}
			for (i = 0; i < node_num; i++) {
				timekeep[i] = -1;
			}
			if (g_dat_deg[node_order[order_ptr]].in_deg > 0) {// find min ta
				gi_ptr = &gi_dat[node_order[order_ptr]];
				gi_ptr = gi_ptr->inext;
				while (gi_ptr != NULL) {
					deg++;
					if (eflag[node_order[order_ptr]].ein[deg] == 0 && g_dat_deg[gi_ptr->from_node].out_deg != 0) {
						if (gi_ptr->iend_t < t && gi_ptr->iend_t > pt) { // find min ta that > previous min ta
							t = gi_ptr->iend_t;
							gi_hptr = gi_ptr;
							t_deg = deg;
						}
					}
					if (gi_ptr->inext != NULL) {
						gi_ptr = gi_ptr->inext;
					}
					else {
						break;
					}
				}
			}

			if (t != 999999) {// have found a ta
				
				pt = t, deg = 0;
				eflag[node_order[order_ptr]].ein[t_deg] = 1;
				heap[heap_ptr].x = gi_hptr->from_node;// add min ta edge to heap
				heap[heap_ptr].y = node_order[order_ptr];
				heap[heap_ptr].td = gi_hptr->istart_t;
				heap[heap_ptr].ta = gi_hptr->iend_t;
				heap[heap_ptr].b = gi_hptr->ib_id;
				heap[heap_ptr].p = -1;
				heap_ptr++;
				deg_num--;
				gi_ptr = &gi_dat[node_order[order_ptr]];// find same ta edge to heap
				gi_ptr = gi_ptr->inext;
				while (gi_ptr != NULL) {
					deg++;
					if (eflag[node_order[order_ptr]].ein[deg] == 0 && g_dat_deg[gi_ptr->from_node].out_deg != 0) {
						if (gi_ptr->iend_t == t) {
							heap[heap_ptr].x = gi_ptr->from_node;
							heap[heap_ptr].y = node_order[order_ptr];
							heap[heap_ptr].td = gi_ptr->istart_t;
							heap[heap_ptr].ta = gi_ptr->iend_t;
							heap[heap_ptr].b = gi_ptr->ib_id;
							heap[heap_ptr].p = -1;
							heap_ptr++;
							deg_num--;
							eflag[node_order[order_ptr]].ein[deg] = 1;
						}
					}
					if (gi_ptr->inext != NULL) {
						gi_ptr = gi_ptr->inext;
					}
					else {
						break;
					}
				}

				while (hp < heap_ptr) { //dijk search
					
					flag = 0;

					if (htb[heap[hp].y].eat == -1) flag = 1;
					if (heap[hp].td > htb[heap[hp].x].eat && (heap[hp].ta) <= htb[heap[hp].y].eat) flag = 1;
					if (flag == 1) { // update htb
						htb[heap[hp].x].eat = heap[hp].td;
						htb[heap[hp].x].piv = heap[hp].p;
						if (htb[heap[hp].x].veh == -1) {
							htb[heap[hp].x].veh = heap[hp].b;
						}
						else if (htb[heap[hp].x].veh != heap[hp].b) {
							htb[heap[hp].x].veh = -1;
						}

						if (g_dat_deg[heap[hp].x].in_deg > 0) {
							gi_ptr = &gi_dat[heap[hp].x];
							gi_ptr = gi_ptr->inext;
							while (gi_ptr != NULL) {

								if (gi_ptr->istart_t > timekeep[gi_ptr->from_node]) {
									if (g_dat_deg[gi_ptr->from_node].out_deg != 0 && gi_ptr->from_node != node_order[order_ptr]) {
										if (gi_ptr->ib_id != heap[hp].b) {
											if ((gi_ptr->iend_t + wt) > htb[heap[hp].x].eat) goto no2;
											heap[heap_ptr].b = -1;
										}
										else {
											heap[heap_ptr].b = heap[hp].b;
										}
										timekeep[gi_ptr->from_node] = gi_ptr->istart_t;
										heap[heap_ptr].x = gi_ptr->from_node;
										heap[heap_ptr].y = heap[hp].x;
										heap[heap_ptr].td = gi_ptr->istart_t;
										heap[heap_ptr].ta = gi_ptr->iend_t;

										if (heap[hp].p == -1) {
											heap[heap_ptr].p = heap[hp].x;
										}
										else {
											if (order_node[heap[hp].p] < order_node[heap[hp].x]) {
												heap[heap_ptr].p = heap[hp].p;
											}
											else {
												heap[heap_ptr].p = heap[hp].x;
											}
										}

										heap_ptr++;
									}
								}
							no2:
								if (gi_ptr->inext != NULL) {
									gi_ptr = gi_ptr->inext;
								}
								else {
									*heap = sort_hp2(heap, hp + 1, heap_ptr - 1, 0);
									break;
								}
							}
						}
					}
					hp++;
				}

				for (i = 0; i < node_num; i++) { // insert labels

					if (i != node_order[order_ptr] && htb[i].eat != -1) {
				

						flag = 0;
						oind_ptr1 = &label_out[i];
						if (oind_ptr1->oindnt != NULL) {
							oind_ptr1 = oind_ptr1->oindnt;
							while (oind_ptr1 != NULL) {
								if (oind_ptr1->oui == node_order[order_ptr] && oind_ptr1->otd >= htb[i].eat) {
									flag = 1;
									break;
								}

								if (oind_ptr1->oindnt != NULL) {
									oind_ptr1 = oind_ptr1->oindnt;
								}
								else {
									break;
								}
							}
						}

						if (flag == 0) {
							oind_ptr1 = &label_out[i];
							iind_ptr1 = &label_in[node_order[order_ptr]];
							if (oind_ptr1->oindnt != NULL && iind_ptr1->iindnt != NULL) {
								iind_ptr1 = iind_ptr1->iindnt;
								oind_ptr1 = oind_ptr1->oindnt;

							}
							else {
								goto nomid2;

							}
							while (iind_ptr1 != NULL && oind_ptr1 != NULL) {

								if (iind_ptr1->iui == oind_ptr1->oui) {

									if (oind_ptr1->ota < iind_ptr1->itd) {
										if (oind_ptr1->otd >= htb[i].eat && iind_ptr1->ita <= pt) {
											flag = 1;
											break;
										}
									}
									iind_ptr1 = iind_ptr1->iindnt;
									oind_ptr1 = oind_ptr1->oindnt;
								}
								else if (order_node[iind_ptr1->iui] < order_node[oind_ptr1->oui]) {

									if (iind_ptr1->iindnt != NULL) {
										iind_ptr1 = iind_ptr1->iindnt;
									}
									else {
										break;
									}
								}
								else if (order_node[iind_ptr1->iui] > order_node[oind_ptr1->oui]) {

									if (oind_ptr1->oindnt != NULL) {
										oind_ptr1 = oind_ptr1->oindnt;
									}
									else {
										break;
									}
								}
							}

						}
					nomid2:

						if (flag == 0) {


							oind_ptr1 = &label_out[i];
							while (oind_ptr1->oindnt != NULL) {
								oind_ptr1 = oind_ptr1->oindnt;
							}
							oind_ptr2 = (struct index_out*)malloc(sizeof(struct index_out));
							oind_ptr2->oui = node_order[order_ptr];
							oind_ptr2->otd = htb[i].eat;
							oind_ptr2->ota = pt;
							oind_ptr2->obv = htb[i].veh;
							oind_ptr2->opv = htb[i].piv;
							oind_ptr2->oindnt = NULL;
							oind_ptr1->oindnt = oind_ptr2;
							fprintf(labout, "%ld %ld %ld %ld %ld %ld\n", i, oind_ptr2->oui, oind_ptr2->otd, oind_ptr2->ota, oind_ptr2->obv, oind_ptr2->opv);
							//	printf("out: i: %ld: ui: %ld td: %ld ta: %ld bv: %ld pv: %ld\n", i, oind_ptr2->oui, oind_ptr2->otd/3600, oind_ptr2->ota/3600, oind_ptr2->obv, oind_ptr2->opv);
							
						}
						else {
							flag = 0;
						}
					}
				}
			}
			else {
				lab = 2;
			}
		}

		if (lab == 2) {
			g_dat_deg[node_order[order_ptr]].out_deg = 0;
			g_dat_deg[node_order[order_ptr]].in_deg = 0;
			order_ptr++;
			lab = 0;
			pt = 999999;

		}


	}
	fprintf(labin, "-1\n");
	fprintf(labout, "-1\n");
	fclose(labin);
	fclose(labout);

}

/***********************************************************************************
* build_index_uv() perform dijkstra search to build Index of unchanged vehicle route
************************************************************************************/
void build_index_uv(struct graph_data_out go_dat[], struct graph_data_in gi_dat[], struct graph_data_deg g_dat_deg[], long node_order[], long order_node[], struct index_in label_in[], struct index_out label_out[]) {
	long order_ptr = 0, t = 0, pt = 999999, deg = 0, t_deg = 0, flag = 0, lab = 0;
	long hp = 0, heap_ptr = 0, i = 0, j = 0, k = 0, deg_num = 0;
	long *timekeep = new long[node_num];
	struct hash_table_B { // each hash table keeptrack a bus, eg. n bus have n hash table
		long *eat = new long[bus_num+2];
		long *veh = new long[bus_num+2];
		long *piv = new long[bus_num+2];
	};

	hash_table_B *htb = new hash_table_B[node_num ];

	mini_heap *heap = new mini_heap[edge_num + 1];
	FILE *labin = fopen(lin_uv_file, "w");
	FILE *labout = fopen(lout_uv_file, "w");
	struct edge_flag {
		long *eout = new long[max_out + 1];
		long *ein = new long[max_in + 1];
	};
	edge_flag *eflag = new edge_flag[node_num];
	for (i = 0; i < node_num; i++) {
		for (j = 0; j < max_out + 2; j++) eflag[i].eout[j] = 0;
		for (j = 0; j < max_in + 2; j++) eflag[i].ein[j] = 0;

	}

	while (order_ptr < node_num) { // for 1:N 
		printf("node: %d order: %d  \n", node_order[order_ptr], order_ptr);
		deg_num = g_dat_deg[node_order[order_ptr]].out_deg;
		while (lab == 0) {

			t = -1, deg = 0, hp = 1, heap_ptr = 1;// init
			for (j = 0; j <= bus_num; j++) {
				for (i = 0; i < node_num; i++) {
					htb[i].eat[j] = 999999;
					htb[i].piv[j] = -1;
					htb[i].veh[j] = -1;
				}
			}
			for (i = 0; i < node_num; i++) {
				timekeep[i] = 999999;
			}
			if (g_dat_deg[node_order[order_ptr]].out_deg > 0) {// find max td
				go_ptr = &go_dat[node_order[order_ptr]];
				go_ptr = go_ptr->onext;
				while (go_ptr != NULL) {
					deg++;
					if (eflag[node_order[order_ptr]].eout[deg] == 0 && g_dat_deg[go_ptr->to_node].in_deg != 0) {
						if (go_ptr->ostart_t > t && go_ptr->ostart_t < pt) { // find max td that < previous max td
							t = go_ptr->ostart_t;
							go_hptr = go_ptr;
							t_deg = deg;
						}
					}
					if (go_ptr->onext != NULL) {
						go_ptr = go_ptr->onext;
					}
					else {
						break;
					}
				}
			}

			if (t != -1) {// have found a td
				
				deg_num--;
				pt = t, deg = 0;
				eflag[node_order[order_ptr]].eout[t_deg] = 1;
				heap[heap_ptr].x = node_order[order_ptr];// add max td edge to heap
				heap[heap_ptr].y = go_hptr->to_node;
				heap[heap_ptr].td = go_hptr->ostart_t;
				heap[heap_ptr].ta = go_hptr->oend_t;
				heap[heap_ptr].b = go_hptr->ob_id;
				heap[heap_ptr].p = -1;
				heap_ptr++;

				go_ptr = &go_dat[node_order[order_ptr]];// find same td edge to heap
				go_ptr = go_ptr->onext;
				while (go_ptr != NULL) {
					deg++;
					if (eflag[node_order[order_ptr]].eout[deg] == 0 && g_dat_deg[go_ptr->to_node].in_deg != 0) {
						if (go_ptr->ostart_t == t) {
							heap[heap_ptr].x = node_order[order_ptr];
							heap[heap_ptr].y = go_ptr->to_node;
							heap[heap_ptr].td = go_ptr->ostart_t;
							heap[heap_ptr].ta = go_ptr->oend_t;
							heap[heap_ptr].b = go_ptr->ob_id;
							heap[heap_ptr].p = -1;
							heap_ptr++;
							eflag[node_order[order_ptr]].eout[deg] = 1;
							deg_num--;
						}
					}
					if (go_ptr->onext != NULL) {
						go_ptr = go_ptr->onext;
					}
					else {
						break;
					}
				}

				while (hp < heap_ptr) { //dijk search
					
					flag = 0;
				

					if (htb[heap[hp].x].eat[heap[hp].b] == 999999) flag = 1;
					//update resp. htb of each edge 
					if (heap[hp].td >= htb[heap[hp].x].eat[heap[hp].b] && heap[hp].ta < htb[heap[hp].y].eat[heap[hp].b]) flag = 1;
					if (flag == 1) { // update htb
						htb[heap[hp].y].eat[heap[hp].b] = heap[hp].ta;
						htb[heap[hp].y].piv[heap[hp].b] = heap[hp].p;

						htb[heap[hp].y].veh[heap[hp].b] = heap[hp].b;

						if (g_dat_deg[heap[hp].y].out_deg > 0) {// add edge to heap
							go_ptr = &go_dat[heap[hp].y];
							go_ptr = go_ptr->onext;
							while (go_ptr != NULL) {
								if (go_ptr->ostart_t >= htb[heap[hp].y].eat[heap[hp].b] && go_ptr->ob_id == heap[hp].b) {
									if (go_ptr->oend_t < timekeep[go_ptr->to_node]) {
										if (g_dat_deg[go_ptr->to_node].in_deg != 0 && go_ptr->to_node != node_order[order_ptr]) {
											timekeep[go_ptr->to_node] = go_ptr->oend_t;
											heap[heap_ptr].x = heap[hp].y;
											heap[heap_ptr].y = go_ptr->to_node;
											heap[heap_ptr].td = go_ptr->ostart_t;
											heap[heap_ptr].ta = go_ptr->oend_t;

											heap[heap_ptr].b = heap[hp].b;

											if (heap[hp].p == -1) {
												heap[heap_ptr].p = heap[hp].y;
											}
											else {
												if (order_node[heap[hp].p] < order_node[heap[hp].y]) {
													heap[heap_ptr].p = heap[hp].p;
												}
												else {
													heap[heap_ptr].p = heap[hp].y;
												}
											}
											heap_ptr++;
										}
									}
								}
								if (go_ptr->onext != NULL) {
									go_ptr = go_ptr->onext;
								}
								else {
									*heap = sort_hp(heap, hp + 1, heap_ptr - 1, 0);
									break;
								}
							}
						}
					}
					hp++;
				}
				for (j = 0; j <= bus_num; j++) {
					for (i = 0; i < node_num; i++) { // insert labels

						if (i != node_order[order_ptr] && htb[i].eat[j] != 999999) {

							flag = 0;

							iind_ptr1 = &label_in[i];
							if (iind_ptr1->iindnt != NULL) {
								iind_ptr1 = iind_ptr1->iindnt;
								while (iind_ptr1 != NULL) {
									if (iind_ptr1->iui == node_order[order_ptr] && iind_ptr1->ita <= htb[i].eat[j]) {
										if (iind_ptr1->ibv == htb[i].veh[j]) {
											flag = 1;
											break;
										}
									}

									if (iind_ptr1->iindnt != NULL) {
										iind_ptr1 = iind_ptr1->iindnt;
									}
									else {
										break;
									}
								}
							}

							if (flag == 0) {
								iind_ptr1 = &label_in[i];
								oind_ptr1 = &label_out[node_order[order_ptr]];

								if (iind_ptr1->iindnt != NULL && oind_ptr1->oindnt != NULL) {
									iind_ptr1 = iind_ptr1->iindnt;
									oind_ptr1 = oind_ptr1->oindnt;
								}
								else {
									goto nomid;
								}

								while (iind_ptr1 != NULL && oind_ptr1 != NULL) {

									if (iind_ptr1->iui == oind_ptr1->oui) {

										if (oind_ptr1->ota < iind_ptr1->itd && oind_ptr1->obv == iind_ptr1->ibv) {
											if (oind_ptr1->otd >= pt && iind_ptr1->ita <= htb[i].eat[j]) {
												if (oind_ptr1->obv == htb[i].veh[j]) {
													flag = 1;
													break;
												}
											}
										}
										iind_ptr1 = iind_ptr1->iindnt;
										oind_ptr1 = oind_ptr1->oindnt;
									}
									else if (order_node[iind_ptr1->iui] < order_node[oind_ptr1->oui]) {

										if (iind_ptr1->iindnt != NULL) {
											iind_ptr1 = iind_ptr1->iindnt;
										}
										else {
											break;
										}
									}
									else if (order_node[iind_ptr1->iui] > order_node[oind_ptr1->oui]) {

										if (oind_ptr1->oindnt != NULL) {
											oind_ptr1 = oind_ptr1->oindnt;
										}
										else {
											break;
										}
									}
								}

							}
						nomid:

							if (flag == 0) {

								iind_ptr1 = &label_in[i];
								while (iind_ptr1->iindnt != NULL) {
									iind_ptr1 = iind_ptr1->iindnt;
								}
								iind_ptr2 = (struct index_in*)malloc(sizeof(struct index_in));
								iind_ptr2->iui = node_order[order_ptr];
								iind_ptr2->itd = pt;
								iind_ptr2->ita = htb[i].eat[j];
								iind_ptr2->ibv = htb[i].veh[j];
								iind_ptr2->ipv = htb[i].piv[j];
								iind_ptr2->iindnt = NULL;
								iind_ptr1->iindnt = iind_ptr2;
								fprintf(labin, "%ld %ld %ld %ld %ld %ld\n", i, iind_ptr2->iui, iind_ptr2->itd, iind_ptr2->ita, iind_ptr2->ibv, iind_ptr2->ipv);
								//	printf("in: i: %d: ui: %ld td: %ld ta: %ld bv: %ld pv: %ld\n", i, iind_ptr2->iui, iind_ptr2->itd, iind_ptr2->ita, iind_ptr2->ibv, iind_ptr2->ipv);
							}
							else {
								flag = 0;
							}
						}
					}
				}
			}
			else {
				lab = 1;
				pt = -1;
				deg_num = g_dat_deg[node_order[order_ptr]].in_deg;
			}
		}
		while (lab == 1) {// reverse dijk

			t = 999999, deg = 0, hp = 1, heap_ptr = 1; // init
			for (j = 0; j <= bus_num; j++) {
				for (i = 0; i < node_num; i++) {
					htb[i].eat[j] = -1;//LDT
					htb[i].piv[j] = -1;
					htb[i].veh[j] = -1;
				}
			}
			for (i = 0; i < node_num; i++) {
				timekeep[i] = -1;
			}

			if (g_dat_deg[node_order[order_ptr]].in_deg > 0) {// find min ta
				gi_ptr = &gi_dat[node_order[order_ptr]];
				gi_ptr = gi_ptr->inext;
				while (gi_ptr != NULL) {
					deg++;
					if (eflag[node_order[order_ptr]].ein[deg] == 0 && g_dat_deg[gi_ptr->from_node].out_deg != 0) {
						if (gi_ptr->iend_t < t && gi_ptr->iend_t > pt) { // find min ta that > previous min ta
							t = gi_ptr->iend_t;
							gi_hptr = gi_ptr;
							t_deg = deg;
						}
					}
					if (gi_ptr->inext != NULL) {
						gi_ptr = gi_ptr->inext;
					}
					else {
						break;
					}
				}
			}

			if (t != 999999) {// have found a ta
			
				pt = t, deg = 0;
				eflag[node_order[order_ptr]].ein[t_deg] = 1;
				heap[heap_ptr].x = gi_hptr->from_node;// add min ta edge to heap
				heap[heap_ptr].y = node_order[order_ptr];
				heap[heap_ptr].td = gi_hptr->istart_t;
				heap[heap_ptr].ta = gi_hptr->iend_t;
				heap[heap_ptr].b = gi_hptr->ib_id;
				heap[heap_ptr].p = -1;
				heap_ptr++;
				deg_num--;
				gi_ptr = &gi_dat[node_order[order_ptr]];// find same ta edge to heap
				gi_ptr = gi_ptr->inext;
				while (gi_ptr != NULL) {
					deg++;
					if (eflag[node_order[order_ptr]].ein[deg] == 0 && g_dat_deg[gi_ptr->from_node].out_deg != 0) {
						if (gi_ptr->iend_t == t) {
							heap[heap_ptr].x = gi_ptr->from_node;
							heap[heap_ptr].y = node_order[order_ptr];
							heap[heap_ptr].td = gi_ptr->istart_t;
							heap[heap_ptr].ta = gi_ptr->iend_t;
							heap[heap_ptr].b = gi_ptr->ib_id;
							heap[heap_ptr].p = -1;
							heap_ptr++;
							deg_num--;
							eflag[node_order[order_ptr]].ein[deg] = 1;
						}
					}
					if (gi_ptr->inext != NULL) {
						gi_ptr = gi_ptr->inext;
					}
					else {
						break;
					}
				}

				while (hp < heap_ptr) { //dijk search
				
					flag = 0;


					if (htb[heap[hp].y].eat[heap[hp].b] == -1) flag = 1;
					// update resp. htb 
					if (heap[hp].td > htb[heap[hp].x].eat[heap[hp].b] && heap[hp].ta <= htb[heap[hp].y].eat[heap[hp].b]) flag = 1;
					if (flag == 1) { // update htb
						htb[heap[hp].x].eat[heap[hp].b] = heap[hp].td;
						htb[heap[hp].x].piv[heap[hp].b] = heap[hp].p;

						htb[heap[hp].x].veh[heap[hp].b] = heap[hp].b;

						if (g_dat_deg[heap[hp].x].in_deg > 0) { // add edge to heap
							gi_ptr = &gi_dat[heap[hp].x];
							gi_ptr = gi_ptr->inext;
							while (gi_ptr != NULL) {
								if (gi_ptr->iend_t <= htb[heap[hp].x].eat[heap[hp].b] && gi_ptr->ib_id == heap[hp].b) {
									if (gi_ptr->istart_t > timekeep[gi_ptr->from_node]) {
										if (g_dat_deg[gi_ptr->from_node].out_deg != 0 && gi_ptr->from_node != node_order[order_ptr]) {
											timekeep[gi_ptr->from_node] = gi_ptr->istart_t;
											heap[heap_ptr].x = gi_ptr->from_node;
											heap[heap_ptr].y = heap[hp].x;
											heap[heap_ptr].td = gi_ptr->istart_t;
											heap[heap_ptr].ta = gi_ptr->iend_t;

											heap[heap_ptr].b = heap[hp].b;

											if (heap[hp].p == -1) {
												heap[heap_ptr].p = heap[hp].x;
											}
											else {
												if (order_node[heap[hp].p] < order_node[heap[hp].x]) {
													heap[heap_ptr].p = heap[hp].p;
												}
												else {
													heap[heap_ptr].p = heap[hp].x;
												}
											}
											heap_ptr++;
										}
									}
								}
								if (gi_ptr->inext != NULL) {
									gi_ptr = gi_ptr->inext;
								}
								else {
									*heap = sort_hp2(heap, hp + 1, heap_ptr - 1, 0);
									break;
								}
							}
						}
					}
					hp++;
				}
				for (j = 0; j <= bus_num; j++) {
					for (i = 0; i < node_num; i++) { // insert labels
						if (i != node_order[order_ptr] && htb[i].eat[j] != -1) {

							flag = 0;
							oind_ptr1 = &label_out[i];
							if (oind_ptr1->oindnt != NULL) {
								oind_ptr1 = oind_ptr1->oindnt;
								while (oind_ptr1 != NULL) {
									if (oind_ptr1->oui == node_order[order_ptr] && oind_ptr1->otd >= htb[i].eat[j]) {
										if (oind_ptr1->obv == htb[i].veh[j]) {
											flag = 1;
											break;
										}
									}

									if (oind_ptr1->oindnt != NULL) {
										oind_ptr1 = oind_ptr1->oindnt;
									}
									else {
										break;
									}
								}
							}

							if (flag == 0) {
								oind_ptr1 = &label_out[i];
								iind_ptr1 = &label_in[node_order[order_ptr]];
								if (oind_ptr1->oindnt != NULL && iind_ptr1->iindnt != NULL) {
									iind_ptr1 = iind_ptr1->iindnt;
									oind_ptr1 = oind_ptr1->oindnt;

								}
								else {
									goto nomid2;

								}
								while (iind_ptr1 != NULL && oind_ptr1 != NULL) {
									if (iind_ptr1->iui == oind_ptr1->oui) {

										if (oind_ptr1->ota < iind_ptr1->itd && oind_ptr1->obv == iind_ptr1->ibv) {
											if (oind_ptr1->otd >= htb[i].eat[j] && iind_ptr1->ita <= pt) {
												if (oind_ptr1->obv == htb[i].veh[j]) {
													flag = 1;
													break;
												}
											}
										}
										iind_ptr1 = iind_ptr1->iindnt;
										oind_ptr1 = oind_ptr1->oindnt;
									}
									else if (order_node[iind_ptr1->iui] < order_node[oind_ptr1->oui]) {

										if (iind_ptr1->iindnt != NULL) {
											iind_ptr1 = iind_ptr1->iindnt;
										}
										else {
											break;
										}
									}
									else if (order_node[iind_ptr1->iui] > order_node[oind_ptr1->oui]) {

										if (oind_ptr1->oindnt != NULL) {
											oind_ptr1 = oind_ptr1->oindnt;
										}
										else {
											break;
										}
									}
								}

							}
						nomid2:

							if (flag == 0) {
								oind_ptr1 = &label_out[i];
								while (oind_ptr1->oindnt != NULL) {
									oind_ptr1 = oind_ptr1->oindnt;
								}
								oind_ptr2 = (struct index_out*)malloc(sizeof(struct index_out));
								oind_ptr2->oui = node_order[order_ptr];
								oind_ptr2->otd = htb[i].eat[j];
								oind_ptr2->ota = pt;
								oind_ptr2->obv = htb[i].veh[j];
								oind_ptr2->opv = htb[i].piv[j];
								oind_ptr2->oindnt = NULL;
								oind_ptr1->oindnt = oind_ptr2;
								fprintf(labout, "%ld %ld %ld %ld %ld %ld\n", i, oind_ptr2->oui, oind_ptr2->otd, oind_ptr2->ota, oind_ptr2->obv, oind_ptr2->opv);
								//	printf("out: i: %ld: ui: %ld td: %ld ta: %ld bv: %ld pv: %ld\n", i, oind_ptr2->oui, oind_ptr2->otd, oind_ptr2->ota, oind_ptr2->obv, oind_ptr2->opv);
							}
							else {
								flag = 0;
							}
						}
					}
				}
			}
			else {
				lab = 2;
			}
		}

		if (lab == 2) {
			g_dat_deg[node_order[order_ptr]].out_deg = 0;
			g_dat_deg[node_order[order_ptr]].in_deg = 0;
			order_ptr++;
			lab = 0;
			pt = 999999;

		}

	}
	fprintf(labin, "-1\n");
	fprintf(labout, "-1\n");
	fclose(labin);
	fclose(labout);

}

/**************************************************************************
* build_index_c() perform dijkstra search to build Index for money-oriented
***************************************************************************/
void build_index_c(struct graph_data_out go_dat[], struct graph_data_in gi_dat[], struct graph_data_deg g_dat_deg[], long node_order[], long order_node[], struct index_in label_in[], struct index_out label_out[]) {
	long order_ptr = 0, t = 0, pt = 999999, deg = 0, t_deg = 0, flag = 0, lab = 0;
	long hp = 0, heap_ptr = 0, i = 0, j = 0, k = 0, deg_num = 0;
	long indnum = 0;
	long *timekeep = new long[node_num];
	hash_table *htb = new hash_table[node_num];
	mini_heap *heap = new mini_heap[edge_num + 1];
	FILE *labin = fopen(lin_c_file, "w");
	FILE *labout = fopen(lout_c_file, "w");
	struct edge_flag {
		long *eout = new long[max_out + 1];
		long *ein = new long[max_in + 1];
	};
	edge_flag *eflag = new edge_flag[node_num];
	for (i = 0; i < node_num; i++) {
		for (j = 0; j < max_out + 2; j++) eflag[i].eout[j] = 0;
		for (j = 0; j < max_in + 2; j++) eflag[i].ein[j] = 0;
	}

	while (order_ptr < node_num) { // for 1:N 
		printf("node: %d order: %d  \n", node_order[order_ptr], order_ptr);
		deg_num = g_dat_deg[node_order[order_ptr]].out_deg;
		while (lab == 0) {

			t = -1, deg = 0, hp = 1, heap_ptr = 1;// init
			for (i = 0; i < node_num; i++) {
				htb[i].eat = 999999;
				htb[i].piv = -1;
				htb[i].veh = -1;
				htb[i].cost = 0;
			}
			for (i = 0; i < node_num; i++) {
				timekeep[i] = 999999;
			}
			if (g_dat_deg[node_order[order_ptr]].out_deg > 0) {// find max td
				go_ptr = &go_dat[node_order[order_ptr]];
				go_ptr = go_ptr->onext;
				while (go_ptr != NULL) {
					deg++;
					if (eflag[node_order[order_ptr]].eout[deg] == 0 && g_dat_deg[go_ptr->to_node].in_deg != 0) {
						if (go_ptr->ostart_t > t && go_ptr->ostart_t < pt) { // find max td that < previous max td
							t = go_ptr->ostart_t;
							go_hptr = go_ptr;
							t_deg = deg;
						}
					}
					if (go_ptr->onext != NULL) {
						go_ptr = go_ptr->onext;
					}
					else {
						break;
					}
				}
			}

			if (t != -1) {// have found a td
				
				deg_num--;
				pt = t, deg = 0;
				eflag[node_order[order_ptr]].eout[t_deg] = 1;
				heap[heap_ptr].x = node_order[order_ptr];// add max td edge to heap
				heap[heap_ptr].y = go_hptr->to_node;
				heap[heap_ptr].td = go_hptr->ostart_t;
				heap[heap_ptr].ta = go_hptr->oend_t;
				heap[heap_ptr].b = go_hptr->ob_id;
				heap[heap_ptr].p = -1;
				heap[heap_ptr].cost = go_hptr->cost;
				heap_ptr++;

				go_ptr = &go_dat[node_order[order_ptr]];// find same td edge to heap
				go_ptr = go_ptr->onext;
				while (go_ptr != NULL) {
					deg++;
					if (eflag[node_order[order_ptr]].eout[deg] == 0 && g_dat_deg[go_ptr->to_node].in_deg != 0) {
						if (go_ptr->ostart_t == t) {
							heap[heap_ptr].x = node_order[order_ptr];
							heap[heap_ptr].y = go_ptr->to_node;
							heap[heap_ptr].td = go_ptr->ostart_t;
							heap[heap_ptr].ta = go_ptr->oend_t;
							heap[heap_ptr].b = go_ptr->ob_id;
							heap[heap_ptr].p = -1;
							heap[heap_ptr].cost = go_ptr->cost;
							heap_ptr++;
							eflag[node_order[order_ptr]].eout[deg] = 1;
							deg_num--;
						}
					}
					if (go_ptr->onext != NULL) {
						go_ptr = go_ptr->onext;
					}
					else {
						break;
					}
				}

				while (hp < heap_ptr) { //dijk search
					
					flag = 0;


					if (htb[heap[hp].y].cost == 0) flag = 1;
					// update htb only if a lower cost edge is found
					if ((heap[hp].cost + htb[heap[hp].x].cost)  < htb[heap[hp].y].cost) flag = 1;
					if (flag == 1) { // update htb
						htb[heap[hp].y].eat = heap[hp].ta;
						htb[heap[hp].y].piv = heap[hp].p;
						if (htb[heap[hp].y].veh == -1) {
							htb[heap[hp].y].veh = heap[hp].b;
						}
						else if (htb[heap[hp].y].veh != heap[hp].b) {
							htb[heap[hp].y].veh = -1;
						}
						htb[heap[hp].y].cost = heap[hp].cost + htb[heap[hp].x].cost;
					
						if (g_dat_deg[heap[hp].y].out_deg > 0) {
							go_ptr = &go_dat[heap[hp].y];
							go_ptr = go_ptr->onext;
							while (go_ptr != NULL) {
								if (go_ptr->ostart_t >= htb[heap[hp].y].eat) {
									if (go_ptr->oend_t < timekeep[go_ptr->to_node]) {
										if (g_dat_deg[go_ptr->to_node].in_deg != 0 && go_ptr->to_node != node_order[order_ptr]) {
											timekeep[go_ptr->to_node] = go_ptr->oend_t;
											heap[heap_ptr].x = heap[hp].y;
											heap[heap_ptr].y = go_ptr->to_node;
											heap[heap_ptr].td = go_ptr->ostart_t;
											heap[heap_ptr].ta = go_ptr->oend_t;
											heap[heap_ptr].cost = go_ptr->cost;
											if (go_ptr->ob_id != heap[hp].b) {
												heap[heap_ptr].b = -1;
											}
											else {
												heap[heap_ptr].b = heap[hp].b;
											}
											if (heap[hp].p == -1) {
												heap[heap_ptr].p = heap[hp].y;
											}
											else {
												if (order_node[heap[hp].p] < order_node[heap[hp].y]) {
													heap[heap_ptr].p = heap[hp].p;
												}
												else {
													heap[heap_ptr].p = heap[hp].y;
												}
											}
											heap_ptr++;
										}
									}
								}
								if (go_ptr->onext != NULL) {
									go_ptr = go_ptr->onext;
								}
								else {
									*heap = sort_hp(heap, hp + 1, heap_ptr - 1, 1);
									break;
								}

							}
						}
					}
					hp++;
				}

				for (i = 0; i < node_num; i++) { // insert labels

					if (i != node_order[order_ptr] && htb[i].cost != 0) {

						flag = 0;

						iind_ptr1 = &label_in[i];
						if (iind_ptr1->iindnt != NULL) {
							iind_ptr1 = iind_ptr1->iindnt;
							while (iind_ptr1 != NULL) {
								if (iind_ptr1->iui == node_order[order_ptr] && iind_ptr1->ita <= htb[i].eat) {
									if (iind_ptr1->cost <= htb[i].cost) {
										flag = 1;
										break;
									}
								}

								if (iind_ptr1->iindnt != NULL) {
									iind_ptr1 = iind_ptr1->iindnt;
								}
								else {
									break;
								}
							}
						}

						if (flag == 0) {
							iind_ptr1 = &label_in[i];
							oind_ptr1 = &label_out[node_order[order_ptr]];

							if (iind_ptr1->iindnt != NULL && oind_ptr1->oindnt != NULL) {
								iind_ptr1 = iind_ptr1->iindnt;
								oind_ptr1 = oind_ptr1->oindnt;
							}
							else {
								goto nomid;
							}

							while (iind_ptr1 != NULL && oind_ptr1 != NULL) {

								if (iind_ptr1->iui == oind_ptr1->oui) {

									if (oind_ptr1->ota < iind_ptr1->itd) {
										if (oind_ptr1->otd >= pt && iind_ptr1->ita <= htb[i].eat) {
											if ((oind_ptr1->cost + iind_ptr1->cost) <= htb[i].cost) {
												flag = 1;
												break;
											}
										}
									}
									iind_ptr1 = iind_ptr1->iindnt;
									oind_ptr1 = oind_ptr1->oindnt;
								}
								else if (order_node[iind_ptr1->iui] < order_node[oind_ptr1->oui]) {

									if (iind_ptr1->iindnt != NULL) {
										iind_ptr1 = iind_ptr1->iindnt;
									}
									else {
										break;
									}
								}
								else if (order_node[iind_ptr1->iui] > order_node[oind_ptr1->oui]) {

									if (oind_ptr1->oindnt != NULL) {
										oind_ptr1 = oind_ptr1->oindnt;
									}
									else {
										break;
									}
								}
							}

						}
					nomid:

						if (flag == 0) {

							iind_ptr1 = &label_in[i];
							while (iind_ptr1->iindnt != NULL) {
								iind_ptr1 = iind_ptr1->iindnt;
							}
							iind_ptr2 = (struct index_in*)malloc(sizeof(struct index_in));
							iind_ptr2->iui = node_order[order_ptr];
							iind_ptr2->itd = pt;
							iind_ptr2->ita = htb[i].eat;
							iind_ptr2->ibv = htb[i].veh;
							iind_ptr2->ipv = htb[i].piv;
							iind_ptr2->cost = htb[i].cost;
							iind_ptr2->iindnt = NULL;
							iind_ptr1->iindnt = iind_ptr2;
							fprintf(labin, "%ld %ld %ld %ld %ld %ld %ld\n", i, iind_ptr2->iui, iind_ptr2->itd, iind_ptr2->ita, iind_ptr2->ibv, iind_ptr2->ipv, iind_ptr2->cost);
							//	printf("in: i: %d: ui: %ld td: %ld ta: %ld bv: %ld pv: %ld c: %ld\n", i, iind_ptr2->iui, iind_ptr2->itd/3600, iind_ptr2->ita/3600, iind_ptr2->ibv, iind_ptr2->ipv,iind_ptr2->cost);
						}
						else {
							flag = 0;
						}
					}
				}
			}
			else {
				lab = 1;
				pt = -1;
				deg_num = g_dat_deg[node_order[order_ptr]].in_deg;
			}
		}
		while (lab == 1) {// reverse

			t = 999999, deg = 0, hp = 1, heap_ptr = 1; // init
			for (i = 0; i < node_num; i++) {
				htb[i].eat = -1; //ldt
				htb[i].piv = -1;
				htb[i].veh = -1;
				htb[i].cost = 0;
			}
			for (i = 0; i < node_num; i++) {
				timekeep[i] = -1;
			}
			if (g_dat_deg[node_order[order_ptr]].in_deg > 0) {// find min ta
				gi_ptr = &gi_dat[node_order[order_ptr]];
				gi_ptr = gi_ptr->inext;
				while (gi_ptr != NULL) {
					deg++;
					if (eflag[node_order[order_ptr]].ein[deg] == 0 && g_dat_deg[gi_ptr->from_node].out_deg != 0) {
						if (gi_ptr->iend_t < t && gi_ptr->iend_t > pt) { // find min ta that > previous min ta
							t = gi_ptr->iend_t;
							gi_hptr = gi_ptr;
							t_deg = deg;
						}
					}
					if (gi_ptr->inext != NULL) {
						gi_ptr = gi_ptr->inext;
					}
					else {
						break;
					}
				}
			}

			if (t != 999999) {// have found a ta
		
				pt = t, deg = 0;
				eflag[node_order[order_ptr]].ein[t_deg] = 1;
				heap[heap_ptr].x = gi_hptr->from_node;// add min ta edge to heap
				heap[heap_ptr].y = node_order[order_ptr];
				heap[heap_ptr].td = gi_hptr->istart_t;
				heap[heap_ptr].ta = gi_hptr->iend_t;
				heap[heap_ptr].b = gi_hptr->ib_id;
				heap[heap_ptr].p = -1;
				heap[heap_ptr].cost = gi_hptr->cost;
				heap_ptr++;
				deg_num--;
				gi_ptr = &gi_dat[node_order[order_ptr]];// find same ta edge to heap
				gi_ptr = gi_ptr->inext;
				while (gi_ptr != NULL) {
					deg++;
					if (eflag[node_order[order_ptr]].ein[deg] == 0 && g_dat_deg[gi_ptr->from_node].out_deg != 0) {
						if (gi_ptr->iend_t == t) {
							heap[heap_ptr].x = gi_ptr->from_node;
							heap[heap_ptr].y = node_order[order_ptr];
							heap[heap_ptr].td = gi_ptr->istart_t;
							heap[heap_ptr].ta = gi_ptr->iend_t;
							heap[heap_ptr].b = gi_ptr->ib_id;
							heap[heap_ptr].p = -1;
							heap[heap_ptr].cost = gi_ptr->cost;
							heap_ptr++;
							deg_num--;
							eflag[node_order[order_ptr]].ein[deg] = 1;
						}
					}
					if (gi_ptr->inext != NULL) {
						gi_ptr = gi_ptr->inext;
					}
					else {
						break;
					}
				}

				while (hp < heap_ptr) { //dijk search
				
					flag = 0;


					if (htb[heap[hp].x].cost == 0) flag = 1;
					// update htb if lower cost is found
					if (heap[hp].cost + htb[heap[hp].y].cost < htb[heap[hp].x].cost) flag = 1;
					if (flag == 1) { // update htb
						htb[heap[hp].x].eat = heap[hp].td;
						htb[heap[hp].x].piv = heap[hp].p;
						if (htb[heap[hp].x].veh == -1) {
							htb[heap[hp].x].veh = heap[hp].b;
						}
						else if (htb[heap[hp].x].veh != heap[hp].b) {
							htb[heap[hp].x].veh = -1;
						}
						htb[heap[hp].x].cost = heap[hp].cost + htb[heap[hp].y].cost;
				
						if (g_dat_deg[heap[hp].x].in_deg > 0) {
							gi_ptr = &gi_dat[heap[hp].x];
							gi_ptr = gi_ptr->inext;
							while (gi_ptr != NULL) {
								if (gi_ptr->iend_t <= htb[heap[hp].x].eat) {
									if (gi_ptr->istart_t > timekeep[gi_ptr->from_node]) {
										if (g_dat_deg[gi_ptr->from_node].out_deg != 0 && gi_ptr->from_node != node_order[order_ptr]) {
											timekeep[gi_ptr->from_node] = gi_ptr->istart_t;
											heap[heap_ptr].x = gi_ptr->from_node;
											heap[heap_ptr].y = heap[hp].x;
											heap[heap_ptr].td = gi_ptr->istart_t;
											heap[heap_ptr].ta = gi_ptr->iend_t;
											heap[heap_ptr].cost = gi_ptr->cost;
											if (gi_ptr->ib_id != heap[hp].b) {
												heap[heap_ptr].b = -1;
											}
											else {
												heap[heap_ptr].b = heap[hp].b;
											}
											if (heap[hp].p == -1) {
												heap[heap_ptr].p = heap[hp].x;
											}
											else {
												if (order_node[heap[hp].p] < order_node[heap[hp].x]) {
													heap[heap_ptr].p = heap[hp].p;
												}
												else {
													heap[heap_ptr].p = heap[hp].x;
												}
											}

											heap_ptr++;
										}
									}
								}
								if (gi_ptr->inext != NULL) {
									gi_ptr = gi_ptr->inext;
								}
								else {
									*heap = sort_hp2(heap, hp + 1, heap_ptr - 1, 1);
									break;
								}
							}
						}
					}
					hp++;
				}

				for (i = 0; i < node_num; i++) { // insert labels

					if (i != node_order[order_ptr] && htb[i].cost != 0) {
					//	printf("%d ldp: %d c: %d\n", i, htb[i].eat / 3600, htb[i].cost);
						flag = 0;
						oind_ptr1 = &label_out[i];
						if (oind_ptr1->oindnt != NULL) {
							oind_ptr1 = oind_ptr1->oindnt;
							while (oind_ptr1 != NULL) {
								if (oind_ptr1->oui == node_order[order_ptr] && oind_ptr1->otd >= htb[i].eat) {
									if (oind_ptr1->cost <= htb[i].cost) {
										flag = 1;
										break;
									}
								}

								if (oind_ptr1->oindnt != NULL) {
									oind_ptr1 = oind_ptr1->oindnt;
								}
								else {
									break;
								}
							}
						}

						if (flag == 0) {
							oind_ptr1 = &label_out[i];
							iind_ptr1 = &label_in[node_order[order_ptr]];
							if (oind_ptr1->oindnt != NULL && iind_ptr1->iindnt != NULL) {
								iind_ptr1 = iind_ptr1->iindnt;
								oind_ptr1 = oind_ptr1->oindnt;

							}
							else {
								goto nomid2;

							}
							while (iind_ptr1 != NULL && oind_ptr1 != NULL) {
								if (iind_ptr1->iui == oind_ptr1->oui) {

									if (oind_ptr1->ota < iind_ptr1->itd) {
										if (oind_ptr1->otd >= htb[i].eat && iind_ptr1->ita <= pt) {
											if (oind_ptr1->cost + iind_ptr1->cost <= htb[i].cost) {
												flag = 1;
												break;
											}
										}
									}
									iind_ptr1 = iind_ptr1->iindnt;
									oind_ptr1 = oind_ptr1->oindnt;
								}
								else if (order_node[iind_ptr1->iui] < order_node[oind_ptr1->oui]) {

									if (iind_ptr1->iindnt != NULL) {
										iind_ptr1 = iind_ptr1->iindnt;
									}
									else {
										break;
									}
								}
								else if (order_node[iind_ptr1->iui] > order_node[oind_ptr1->oui]) {

									if (oind_ptr1->oindnt != NULL) {
										oind_ptr1 = oind_ptr1->oindnt;
									}
									else {
										break;
									}
								}
							}

						}
					nomid2:

						if (flag == 0) {

							oind_ptr1 = &label_out[i];
							while (oind_ptr1->oindnt != NULL) {
								oind_ptr1 = oind_ptr1->oindnt;
							}
							oind_ptr2 = (struct index_out*)malloc(sizeof(struct index_out));
							oind_ptr2->oui = node_order[order_ptr];
							oind_ptr2->otd = htb[i].eat;
							oind_ptr2->ota = pt;
							oind_ptr2->obv = htb[i].veh;
							oind_ptr2->opv = htb[i].piv;
							oind_ptr2->cost = htb[i].cost;
							oind_ptr2->oindnt = NULL;
							oind_ptr1->oindnt = oind_ptr2;
							fprintf(labout, "%ld %ld %ld %ld %ld %ld %ld\n", i, oind_ptr2->oui, oind_ptr2->otd, oind_ptr2->ota, oind_ptr2->obv, oind_ptr2->opv, oind_ptr2->cost);
							//		printf("out: i: %ld: ui: %ld td: %ld ta: %ld bv: %ld pv: %ld c: %ld\n", i, oind_ptr2->oui, oind_ptr2->otd/3600, oind_ptr2->ota/3600, oind_ptr2->obv, oind_ptr2->opv,oind_ptr2->cost);
						}
						else {
							flag = 0;
						}
					}
				}
			}
			else {
				lab = 2;
			}
		}

		if (lab == 2) {
			g_dat_deg[node_order[order_ptr]].out_deg = 0;
			g_dat_deg[node_order[order_ptr]].in_deg = 0;
			order_ptr++;
			lab = 0;
			pt = 999999;

		}
	}
	fprintf(labin, "-1\n");
	fprintf(labout, "-1\n");
	fclose(labin);
	fclose(labout);

}

/*****************************************************************
* usage() show the fn of the prog.
****************************************************************/
int usage(struct graph_data_out go_dat[], struct graph_data_in gi_dat[], struct graph_data_deg g_dat_deg[], struct index_in label_in[], struct index_out label_out[], long node_order[], long order_node[], struct index_in label_uv_in[], struct index_out label_uv_out[], struct index_in label_c_in[], struct index_out label_c_out[]) {
	long sel = 0, nod = 0, h = 0, m = 0, s = 0, he = 0, me = 0, se = 0, raw = 0;
	long u, v, std, sta, wt, i;
	long test = 0;
	while (1) {

		printf("\n-------------------------------------\n");
		printf("1 -- Print the edges info. of node n\n");
		printf("2 -- Perform heristic ordering\n");
		printf("3 -- Check the order file\n");
		printf("4 -- Build index (original | waiting time involved)\n");
		printf("5 -- Query(original | waiting time involved)\n");
		printf("6 -- Build index (unchanged vehicle)\n");
		printf("7 -- Query (unchanged vehicle)\n");
		printf("8 -- Build index ($-oriented)\n");
		printf("9 -- Query ($-oriented)\n");
		printf("0 -- Quit\n");
		printf("Remarks:\n");
		printf("1. inputs,td, ta are in unit of Hours\n");
		printf("3. Set u = -1 to return\n");

		printf("-------------------------------------\n\n");
		printf("input: ");
		scanf("%d", &sel);

		switch (sel) {
		case 1:
			printf("n: ");
			scanf("%d", &nod);
			printf("raw->1 ?");
			scanf("%d", &raw);
			printf("\nThe out edges of node %d :\nTo node\tStart time\tEnd time\tBus ID\t Cost\n", nod);
			go_ptr = &go_dat[nod];
			if (go_ptr->onext != NULL) {
				go_ptr = go_ptr->onext;
				while (go_ptr != NULL) {
					h = go_ptr->ostart_t / 3600;
					m = (go_ptr->ostart_t - h * 3600) / 60;
					s = (go_ptr->ostart_t - h * 3600) - m * 60;
					he = go_ptr->oend_t / 3600;
					me = (go_ptr->oend_t - he * 3600) / 60;
					se = (go_ptr->oend_t - he * 3600) - me * 60;

					if (raw == 1) {
						//		test++;
						printf("%d\t %d\t    %d\t%d\t%d\n", go_ptr->to_node, go_ptr->ostart_t, go_ptr->oend_t, go_ptr->ob_id, go_ptr->cost);
					}
					else {
						printf("%4d\t ", go_ptr->to_node);
						if (h < 10) { printf("0%d:", h); }
						else { printf("%d:", h); }
						if (m < 10) { printf("0%d:", m); }
						else { printf("%d:", m); }
						if (s < 10) { printf("0%d\t ", s); }
						else { printf("%d\t ", s); }
						if (he < 10) { printf("0%d:", he); }
						else { printf("%d:", he); }
						if (me < 10) { printf("0%d:", me); }
						else { printf("%d:", me); }
						if (se < 10) { printf("0%d\t ", se); }
						else { printf("%d\t ", se); }
						printf("%3d\t", go_ptr->ob_id);
						printf("%d\n", go_ptr->cost);
					}
					if (go_ptr->onext != NULL) {
						go_ptr = go_ptr->onext;
					}
					else {

						break;
					}
				}
			}
			printf("\nThe in edges of node %d :\nFrom node\tStart time\tEnd time\tBus ID\tCost\n", nod);
			gi_ptr = &gi_dat[nod];
			if (gi_ptr->inext != NULL) {
				gi_ptr = gi_ptr->inext;
				while (gi_ptr != NULL) {
					h = gi_ptr->istart_t / 3600;
					m = (gi_ptr->istart_t - h * 3600) / 60;
					s = (gi_ptr->istart_t - h * 3600) - m * 60;
					he = gi_ptr->iend_t / 3600;
					me = (gi_ptr->iend_t - he * 3600) / 60;
					se = (gi_ptr->iend_t - he * 3600) - me * 60;

					if (raw == 1) {
						//	test++;
						printf("%d\t %d\t    %d\t%d\t%d\n", gi_ptr->from_node, gi_ptr->istart_t, gi_ptr->iend_t, gi_ptr->ib_id, gi_ptr->cost);
					}
					else {
						printf("%4d\t ", gi_ptr->from_node);
						if (h < 10) { printf("0%d:", h); }
						else { printf("%d:", h); }
						if (m < 10) { printf("0%d:", m); }
						else { printf("%d:", m); }
						if (s < 10) { printf("0%d\t ", s); }
						else { printf("%d\t ", s); }
						if (he < 10) { printf("0%d:", he); }
						else { printf("%d:", he); }
						if (me < 10) { printf("0%d:", me); }
						else { printf("%d:", me); }
						if (se < 10) { printf("0%d\t ", se); }
						else { printf("%d\t ", se); }
						printf("%3d\t", gi_ptr->ib_id);
						printf("%d\n", gi_ptr->cost);
					}
					if (gi_ptr->inext != NULL) {
						gi_ptr = gi_ptr->inext;
					}
					else {
						//printf("test: %d\n", test);
						break;
					}
				}
			}
			printf("In-degree  Out-degree\n%d\t   %d\n", g_dat_deg[nod].in_deg, g_dat_deg[nod].out_deg);
			break;

		case 2:
			read_ord_filename();
			heu_order(go_dat, gi_dat, g_dat_deg, node_order); // ordering
			break;

		case 3:
			read_ord_filename(); // read order file
			check_ord_file();
			read_order_data(node_order, order_node, g_dat_deg);
			break;
		case 4:
			printf("Input the waiting time(minutes):");
			scanf("%d", &wt);
			//read_gdeg(go_dat, gi_dat, g_dat_deg);
			read_ord_filename(); // read order file name
			read_lab_filename(); // read label file name
			read_order_data(node_order, order_node, g_dat_deg);
			for (i = 0; i < node_num; i++) {
				label_in[i].iindnt = NULL;
				label_out[i].oindnt = NULL;
			}

			build_index(go_dat, gi_dat, g_dat_deg, node_order, order_node, label_in, label_out, wt);  //build index
			break;
		case 5:
			read_ord_filename();
			read_lab_filename();
			read_order_data(node_order, order_node, g_dat_deg);
			read_label_data(label_in, label_out);

			while (1) {
				printf("Input u, v, td, ta\n");
				scanf("%d %d %d %d", &u, &v, &std, &sta);
				if (u == -1) break;
				if (u == v) {
					puts("u can't equal v\n");
					continue;
				}
				if (u<-1 || u>node_num - 1) {
					puts("Wrong Input");
					continue;
				}
				if (v<-1 || v>node_num - 1) {
					puts("Wrong Input");
					continue;
				}
				puts("----------------------------------------------------------------");
				printf("Original:\n");
				sketchgen(order_node, label_in, label_out, u, v, std, sta);
				puts("----------------------------------------------------------------");
			}
			break;
		case 0:
			puts("Bye");
			return 0;
			break;
		case 6:
			//read_gdeg(go_dat, gi_dat, g_dat_deg);
			read_ord_filename(); // read order file name
			read_lab_filename(); // read label file name
			read_order_data(node_order, order_node, g_dat_deg);
			for (i = 0; i < node_num; i++) {
				label_uv_in[i].iindnt = NULL;
				label_uv_out[i].oindnt = NULL;
			}
			build_index_uv(go_dat, gi_dat, g_dat_deg, node_order, order_node, label_uv_in, label_uv_out);  //build index
			break;
		case 7:
			read_ord_filename();
			read_lab_filename();
			read_order_data(node_order, order_node, g_dat_deg);
			read_label_uv_data(label_uv_in, label_uv_out);

			while (1) {
				printf("Input u, v, td, ta\n");
				scanf("%d %d %d %d", &u, &v, &std, &sta);
				if (u == -1) break;
				if (u == v) {
					puts("u can't equal v\n");
					continue;
				}
				if (u<-1 || u>node_num - 1) {
					puts("Wrong Input");
					continue;
				}
				if (v<-1 || v>node_num - 1) {
					puts("Wrong Input");
					continue;
				}
				puts("----------------------------------------------------------------");
				printf("For unchanged vehicle:\n");
				sketchgen_uv(order_node, label_uv_in, label_uv_out, u, v, std, sta);
				puts("----------------------------------------------------------------");
				printf("For at most two vehicles :\n");
				sketchgen(order_node, label_uv_in, label_uv_out, u, v, std, sta);
				puts("----------------------------------------------------------------");
			}
			break;
		case 8:
			//	read_gdeg(go_dat, gi_dat, g_dat_deg);
			read_ord_filename(); // read order file name
			read_lab_filename(); // read label file name
			read_order_data(node_order, order_node, g_dat_deg);
			for (i = 0; i < node_num; i++) {
				label_c_in[i].iindnt = NULL;
				label_c_out[i].oindnt = NULL;
			}
			build_index_c(go_dat, gi_dat, g_dat_deg, node_order, order_node, label_c_in, label_c_out);  //build index
			break;
		case 9:
			read_ord_filename();
			read_lab_filename();
			read_order_data(node_order, order_node, g_dat_deg);
			read_label_c_data(label_c_in, label_c_out);

			while (1) {
				printf("Input u, v, td, ta\n");
				scanf("%d %d %d %d", &u, &v, &std, &sta);
				if (u == -1) break;
				if (u == v) {
					puts("u can't equal v\n");
					continue;
				}
				if (u<-1 || u>node_num - 1) {
					puts("Wrong Input");
					continue;
				}
				if (v<-1 || v>node_num - 1) {
					puts("Wrong Input");
					continue;
				}
				puts("----------------------------------------------------------------");
				printf("Money-oriented:\n");
				sketchgen_c(order_node, label_c_in, label_c_out, u, v, std, sta);
				puts("----------------------------------------------------------------");
			}
			break;
		default:
			return 0;
		}
	}
	return 0;
}

/*****************************************************************
* read_ord_filename() get the name of order file
****************************************************************/
void read_ord_filename() {
	int i = 0;
	char ord_end[10] = "_ord.txt";
	for (i = 0; i < 19; i++) {
		if (data_file[i] != '.') {
			order_file[i] = data_file[i];
		}
		else {
			order_file[i] = '\0';
			break;
		}
	}
	strcat(order_file, ord_end);
	printf("order file: %s\n", order_file);

}

/*****************************************************************
* check_ord_file() check order filename's correctness
****************************************************************/
int check_ord_file() {
	long *chord = new long[node_num];
	long i = 0, j = 0, k = 0;

	for (i = 0; i < node_num; i++) chord[i] = 0;

	FILE *check = fopen(order_file, "r");
	if (!check) {
		printf("Order File not exist\n");
		return 0;
	}

	for (j = 0; j < node_num; j++) {
		fscanf(check, "%d", &i);
		chord[i]++;
	}

	fclose(check);

	for (j = 0; j < node_num; j++) {
		if (chord[j] > 1 || chord[j] == 0) {
			k = 1;
		}
	}
	if (k == 1) {
		printf("\nRepeated nodes exist in order file\n");
	}
	else {
		printf("\nNo repeated nodes exist in order file\n");
	}
	return 0;
}

/*****************************************************************
* read_lab_filename() get the name of label file
****************************************************************/
void read_lab_filename() {
	int i = 0;
	char lin_end[20] = "_lin.txt";
	char lout_end[20] = "_lout.txt";
	char lin_end2[20] = "_uv_lin.txt";
	char lout_end2[20] = "_uv_lout.txt";
	char lin_end3[20] = "_c_lin.txt";
	char lout_end3[20] = "_c_lout.txt";

	for (i = 0; i < 19; i++) {
		if (data_file[i] != '.') {
			lin_file[i] = data_file[i];
			lout_file[i] = data_file[i];
			lin_uv_file[i] = data_file[i];
			lout_uv_file[i] = data_file[i];
			lin_c_file[i] = data_file[i];
			lout_c_file[i] = data_file[i];
		}
		else {
			lin_file[i] = '\0';
			lout_file[i] = '\0';
			lin_uv_file[i] = '\0';
			lout_uv_file[i] = '\0';
			lin_c_file[i] = '\0';
			lout_c_file[i] = '\0';
			break;
		}
	}
	strcat(lin_file, lin_end);
	strcat(lout_file, lout_end);
	strcat(lin_uv_file, lin_end2);
	strcat(lout_uv_file, lout_end2);
	strcat(lin_c_file, lin_end3);
	strcat(lout_c_file, lout_end3);
	printf("label-in file: %s\n", lin_file);
	printf("label-out file: %s\n", lout_file);
	printf("label-in(unchaged vehicle) file: %s\n", lin_uv_file);
	printf("label-out(unchaged vehicle) file: %s\n", lout_uv_file);
	printf("label-in($-oriented) file: %s\n", lin_c_file);
	printf("label-out($-oriented) file: %s\n", lout_c_file);
}

/*****************************************************************
* read_order_data() get the data of order file
****************************************************************/
int read_order_data(long node_order[], long order_node[], struct graph_data_deg g_dat_deg[]) {
	long i = 0, j = 0, k = 0;
	FILE *ord = fopen(order_file, "r");

	if (!ord) {
		printf("Order File not exist\n");
		return 0;
	}
	for (j = 0; j < node_num; j++) {
		fscanf(ord, "%d", &i);
		node_order[j] = i;
		order_node[i] = j;
	}
	fclose(ord);

	for (k = 0; k < node_num; k++) {
		if (g_dat_deg[k].out_deg>max_out) max_out = g_dat_deg[k].out_deg;
		if (g_dat_deg[k].in_deg>max_in) max_in = g_dat_deg[k].in_deg;
	}

	return 0;
}

/*****************************************************************
* read_label_data() get the data of label in/out file
****************************************************************/
void read_label_data(struct index_in label_in[], struct index_out label_out[]) {
	long lvi, lui, ltd, ltv, lbv, lpv;
	FILE *in = fopen(lin_file, "r");
	FILE *out = fopen(lout_file, "r");
	if (!in || !out) {
		printf("\nFile not exist\n");
		exit(EXIT_FAILURE);;
	}

	while (fscanf(in, "%ld", &lvi) == 1) { // read label-in
		if (lvi == -1) break;

		iind_ptr1 = &label_in[lvi];
		while (iind_ptr1->iindnt != NULL) {
			iind_ptr1 = iind_ptr1->iindnt;
		}
		fscanf(in, "%d %d %d %d %d", &lui, &ltd, &ltv, &lbv, &lpv);
		iind_ptr2 = (struct index_in*)malloc(sizeof(struct index_in));
		iind_ptr2->iui = lui;
		iind_ptr2->itd = ltd;
		iind_ptr2->ita = ltv;
		iind_ptr2->ibv = lbv;
		iind_ptr2->ipv = lpv;
		iind_ptr2->iindnt = NULL;
		iind_ptr1->iindnt = iind_ptr2;

	}
	puts("");
	while (fscanf(out, "%ld", &lvi) == 1) {// read label-out
		if (lvi == -1) break;

		oind_ptr1 = &label_out[lvi];
		while (oind_ptr1->oindnt != NULL) {
			oind_ptr1 = oind_ptr1->oindnt;
		}
		fscanf(out, "%d %d %d %d %d", &lui, &ltd, &ltv, &lbv, &lpv);
		oind_ptr2 = (struct index_out*)malloc(sizeof(struct index_out));
		oind_ptr2->oui = lui;
		oind_ptr2->otd = ltd;
		oind_ptr2->ota = ltv;
		oind_ptr2->obv = lbv;
		oind_ptr2->opv = lpv;
		oind_ptr2->oindnt = NULL;
		oind_ptr1->oindnt = oind_ptr2;

	}

	fclose(in);
	fclose(out);

}

/**********************************************************************************
* read_label_uv_data() get the data of label in/out of unchanged vehicle path file
**********************************************************************************/
void read_label_uv_data(struct index_in label_in[], struct index_out label_out[]) {
	long lvi, lui, ltd, ltv, lbv, lpv;
	FILE *in = fopen(lin_uv_file, "r");
	FILE *out = fopen(lout_uv_file, "r");
	if (!in || !out) {
		printf("\nFile not exist\n");
		exit(EXIT_FAILURE);;
	}

	while (fscanf(in, "%ld", &lvi) == 1) { // read label-in
		if (lvi == -1) break;

		iind_ptr1 = &label_in[lvi];
		while (iind_ptr1->iindnt != NULL) {
			iind_ptr1 = iind_ptr1->iindnt;
		}
		fscanf(in, "%d %d %d %d %d", &lui, &ltd, &ltv, &lbv, &lpv);
		iind_ptr2 = (struct index_in*)malloc(sizeof(struct index_in));
		iind_ptr2->iui = lui;
		iind_ptr2->itd = ltd;
		iind_ptr2->ita = ltv;
		iind_ptr2->ibv = lbv;
		iind_ptr2->ipv = lpv;
		iind_ptr2->iindnt = NULL;
		iind_ptr1->iindnt = iind_ptr2;

	}
	puts("");
	while (fscanf(out, "%ld", &lvi) == 1) {// read label-out
		if (lvi == -1) break;

		oind_ptr1 = &label_out[lvi];
		while (oind_ptr1->oindnt != NULL) {
			oind_ptr1 = oind_ptr1->oindnt;
		}
		fscanf(out, "%d %d %d %d %d", &lui, &ltd, &ltv, &lbv, &lpv);
		oind_ptr2 = (struct index_out*)malloc(sizeof(struct index_out));
		oind_ptr2->oui = lui;
		oind_ptr2->otd = ltd;
		oind_ptr2->ota = ltv;
		oind_ptr2->obv = lbv;
		oind_ptr2->opv = lpv;
		oind_ptr2->oindnt = NULL;
		oind_ptr1->oindnt = oind_ptr2;

	}

	fclose(in);
	fclose(out);
}

/*****************************************************************
* read_label_c_data() get the data of label in/out file
****************************************************************/
void read_label_c_data(struct index_in label_in[], struct index_out label_out[]) {
	long lvi, lui, ltd, ltv, lbv, lpv, lc;
	FILE *in = fopen(lin_c_file, "r");
	FILE *out = fopen(lout_c_file, "r");
	if (!in || !out) {
		printf("\nFile not exist\n");
		exit(EXIT_FAILURE);;
	}

	while (fscanf(in, "%ld", &lvi) == 1) { // read label-in
		if (lvi == -1) break;

		iind_ptr1 = &label_in[lvi];
		while (iind_ptr1->iindnt != NULL) {
			iind_ptr1 = iind_ptr1->iindnt;
		}
		fscanf(in, "%d %d %d %d %d %d", &lui, &ltd, &ltv, &lbv, &lpv, &lc);
		iind_ptr2 = (struct index_in*)malloc(sizeof(struct index_in));
		iind_ptr2->iui = lui;
		iind_ptr2->itd = ltd;
		iind_ptr2->ita = ltv;
		iind_ptr2->ibv = lbv;
		iind_ptr2->ipv = lpv;
		iind_ptr2->cost = lc;
		iind_ptr2->iindnt = NULL;
		iind_ptr1->iindnt = iind_ptr2;

	}
	puts("");
	while (fscanf(out, "%ld", &lvi) == 1) {// read label-out
		if (lvi == -1) break;

		oind_ptr1 = &label_out[lvi];
		while (oind_ptr1->oindnt != NULL) {
			oind_ptr1 = oind_ptr1->oindnt;
		}
		fscanf(out, "%d %d %d %d %d %d", &lui, &ltd, &ltv, &lbv, &lpv, &lc);
		oind_ptr2 = (struct index_out*)malloc(sizeof(struct index_out));
		oind_ptr2->oui = lui;
		oind_ptr2->otd = ltd;
		oind_ptr2->ota = ltv;
		oind_ptr2->obv = lbv;
		oind_ptr2->opv = lpv;
		oind_ptr2->cost = lc;
		oind_ptr2->oindnt = NULL;
		oind_ptr1->oindnt = oind_ptr2;

	}

	fclose(in);
	fclose(out);

}
/*****************************************************************
* sketchgen() gen. sketches
****************************************************************/
int sketchgen(long order_node[], struct index_in label_in[], struct index_out label_out[], long u, long v, long std, long sta) {
	long u_flag = 0, v_flag = 0, max_td = std * 3600, min_ta = sta * 3600, flag = -1;
	std *= 3600, sta *= 3600;
	sk_hptr = &sketches;
	sk_hptr->sk_next = NULL;

	oind_ptr1 = &label_out[u];
	iind_ptr1 = &label_in[v];
	if (oind_ptr1->oindnt != NULL) {
		oind_ptr1 = oind_ptr1->oindnt;
	}
	else {
		u_flag = 1;

	}
	if (iind_ptr1->iindnt != NULL) {
		iind_ptr1 = iind_ptr1->iindnt;
	}
	else {
		v_flag = 1;

	}


	while (u_flag == 0 || v_flag == 0) {

		if (u_flag == 0) {
			if (oind_ptr1->otd<std || oind_ptr1->ota>sta) { // [td,ta] !subset [t,t']
				if (oind_ptr1->oindnt != NULL) {
					oind_ptr1 = oind_ptr1->oindnt;
					continue;
				}
				else u_flag = 1;
			}
			else if (oind_ptr1->oui == v) { // w = v

				sk_ptr = (struct sketch*)malloc(sizeof(struct sketch));
				sk_ptr->uw = oind_ptr1->oui;
				sk_ptr->utd = oind_ptr1->otd;
				sk_ptr->uta = oind_ptr1->ota;
				sk_ptr->ubv = oind_ptr1->obv;
				sk_ptr->upv = oind_ptr1->opv;
				sk_ptr->vw = -1;

				sk_ptr->sk_next = NULL;
				sk_hptr->sk_next = sk_ptr;
				sk_hptr = sk_hptr->sk_next;
				if (oind_ptr1->oindnt != NULL) {
					oind_ptr1 = oind_ptr1->oindnt;
					continue;
				}
				else u_flag = 1;
			}
		}
		if (v_flag == 0) {
			if (iind_ptr1->itd<std || iind_ptr1->ita>sta) {// [td',ta'] !subset [t,t']
				if (iind_ptr1->iindnt != NULL) {
					iind_ptr1 = iind_ptr1->iindnt;
					continue;
				}
				else v_flag = 1;
			}
			else if (iind_ptr1->iui == u) {  //w' = u

				sk_ptr = (struct sketch*)malloc(sizeof(struct sketch));
				sk_ptr->vw = iind_ptr1->iui;
				sk_ptr->vtd = iind_ptr1->itd;
				sk_ptr->vta = iind_ptr1->ita;
				sk_ptr->vbv = iind_ptr1->ibv;
				sk_ptr->vpv = iind_ptr1->ipv;
				sk_ptr->uw = -1;

				sk_ptr->sk_next = NULL;
				sk_hptr->sk_next = sk_ptr;
				sk_hptr = sk_hptr->sk_next;
				if (iind_ptr1->iindnt != NULL) {
					iind_ptr1 = iind_ptr1->iindnt;
					continue;
				}
				else v_flag = 1;
			}
		}
		if (u_flag == 0 && v_flag == 0) {
			if (oind_ptr1->oui == iind_ptr1->iui) { // w = w'
				if (oind_ptr1->ota <= iind_ptr1->itd) {
					sk_ptr = (struct sketch*)malloc(sizeof(struct sketch));
					sk_ptr->uw = oind_ptr1->oui;
					sk_ptr->utd = oind_ptr1->otd;
					sk_ptr->uta = oind_ptr1->ota;
					sk_ptr->ubv = oind_ptr1->obv;
					sk_ptr->upv = oind_ptr1->opv;
					sk_ptr->vw = iind_ptr1->iui;
					sk_ptr->vtd = iind_ptr1->itd;
					sk_ptr->vta = iind_ptr1->ita;
					sk_ptr->vbv = iind_ptr1->ibv;
					sk_ptr->vpv = iind_ptr1->ipv;

					sk_ptr->sk_next = NULL;
					sk_hptr->sk_next = sk_ptr;
					sk_hptr = sk_hptr->sk_next;
					if (oind_ptr1->oindnt != NULL) {
						oind_ptr1 = oind_ptr1->oindnt;
						continue;
					}
					else u_flag = 1;
				}
				else {
					if (iind_ptr1->iindnt != NULL) {
						iind_ptr1 = iind_ptr1->iindnt;
						continue;
					}
					else v_flag = 1;
				}
			}
			else {
				if (order_node[oind_ptr1->oui] < order_node[iind_ptr1->iui]) {
					if (oind_ptr1->oindnt != NULL) {
						oind_ptr1 = oind_ptr1->oindnt;
						continue;
					}
					else u_flag = 1;
				}
				else {
					if (iind_ptr1->iindnt != NULL) {
						iind_ptr1 = iind_ptr1->iindnt;
						continue;
					}
					else v_flag = 1;
				}
			}
		}
		if (u_flag == 0) {
			if (oind_ptr1->oindnt != NULL) {
				oind_ptr1 = oind_ptr1->oindnt;
				continue;
			}
			else u_flag = 1;
		}
		if (v_flag == 0) {
			if (iind_ptr1->iindnt != NULL) {
				iind_ptr1 = iind_ptr1->iindnt;
				continue;
			}
			else v_flag = 1;
		}

	}

	sk_hptr = NULL;
	sk_ptr = &sketches;
	if (sk_ptr->sk_next != NULL) {
		sk_ptr = sk_ptr->sk_next;
	}
	else {
		puts("\n1: No path is found\n");
		return 0;
	}

	while (sk_ptr != NULL) { // find SDP
		if (sk_ptr->uw != -1 && sk_ptr->vw != -1) {
			if (sk_ptr->utd >= max_td && sk_ptr->vta <= min_ta) {
				max_td = sk_ptr->utd;
				min_ta = sk_ptr->vta;
				sk_hptr = sk_ptr;
				flag = 2;
			}
		}
		else if (sk_ptr->uw != -1) {
			if (sk_ptr->utd >= max_td && sk_ptr->uta <= min_ta) {
				max_td = sk_ptr->utd;
				min_ta = sk_ptr->uta;
				sk_hptr = sk_ptr;
				flag = 0;
			}
		}
		else {
			if (sk_ptr->vtd >= max_td && sk_ptr->vta <= min_ta) {
				max_td = sk_ptr->vtd;
				min_ta = sk_ptr->vta;
				sk_hptr = sk_ptr;
				flag = 1;
			}
		}
		if (sk_ptr->sk_next != NULL) {
			sk_ptr = sk_ptr->sk_next;
		}
		else {
			break;
		}

	}

	if (sk_hptr != NULL) {
		printf("\nLabel:\n");
		if (flag == 0) printf("w: %d td: %d ta: %d  bv: %d pv: %d\n\n", sk_hptr->uw, sk_hptr->utd, sk_hptr->uta, sk_hptr->ubv, sk_hptr->upv);
		if (flag == 1) printf("w': %d td': %d ta': %d  bv': %d pv': %d\n\n", sk_hptr->vw, sk_hptr->vtd, sk_hptr->vta, sk_hptr->vbv, sk_hptr->vpv);
		if (flag == 2) {
			printf("w: %d td: %d ta: %d  bv: %d pv: %d\n", sk_hptr->uw, sk_hptr->utd, sk_hptr->uta, sk_hptr->ubv, sk_hptr->upv);
			printf("w': %d td': %d ta': %d  bv': %d pv': %d\n\n", sk_hptr->vw, sk_hptr->vtd, sk_hptr->vta, sk_hptr->vbv, sk_hptr->vpv);
		}
		pathconst(order_node, label_in, label_out, u, v, flag);
		print_path();
	}
	else {
		puts("\n2: No path is found\n");
		return 0;
	}
	return 0;
}

/*****************************************************************
* sketchgen_uv() gen. sketches for unchanged vehicle sketch
****************************************************************/
int sketchgen_uv(long order_node[], struct index_in label_in[], struct index_out label_out[], long u, long v, long std, long sta) {
	long u_flag = 0, v_flag = 0, max_td = std * 3600, min_ta = sta * 3600, flag = -1;
	sk_hptr = &sketches;
	sk_hptr->sk_next = NULL;
	std *= 3600, sta *= 3600;
	oind_ptr1 = &label_out[u];
	iind_ptr1 = &label_in[v];
	if (oind_ptr1->oindnt != NULL) {
		oind_ptr1 = oind_ptr1->oindnt;
	}
	else {
		u_flag = 1;
	}
	if (iind_ptr1->iindnt != NULL) {
		iind_ptr1 = iind_ptr1->iindnt;
	}
	else {
		v_flag = 1;
	}


	while (u_flag == 0 || v_flag == 0) {

		if (u_flag == 0) {
			if (oind_ptr1->otd<std || oind_ptr1->ota>sta) { // [td,ta] !subset [t,t']
				if (oind_ptr1->oindnt != NULL) {
					oind_ptr1 = oind_ptr1->oindnt;
					continue;
				}
				else u_flag = 1;
			}
			else if (oind_ptr1->oui == v) { // w = v

				sk_ptr = (struct sketch*)malloc(sizeof(struct sketch));
				sk_ptr->uw = oind_ptr1->oui;
				sk_ptr->utd = oind_ptr1->otd;
				sk_ptr->uta = oind_ptr1->ota;
				sk_ptr->ubv = oind_ptr1->obv;
				sk_ptr->upv = oind_ptr1->opv;
				sk_ptr->vw = -1;

				sk_ptr->sk_next = NULL;
				sk_hptr->sk_next = sk_ptr;
				sk_hptr = sk_hptr->sk_next;
				if (oind_ptr1->oindnt != NULL) {
					oind_ptr1 = oind_ptr1->oindnt;
					continue;
				}
				else u_flag = 1;
			}
		}
		if (v_flag == 0) {
			if (iind_ptr1->itd<std || iind_ptr1->ita>sta) {// [td',ta'] !subset [t,t']
				if (iind_ptr1->iindnt != NULL) {
					iind_ptr1 = iind_ptr1->iindnt;
					continue;
				}
				else v_flag = 1;
			}
			else if (iind_ptr1->iui == u) {  //w' = u

				sk_ptr = (struct sketch*)malloc(sizeof(struct sketch));
				sk_ptr->vw = iind_ptr1->iui;
				sk_ptr->vtd = iind_ptr1->itd;
				sk_ptr->vta = iind_ptr1->ita;
				sk_ptr->vbv = iind_ptr1->ibv;
				sk_ptr->vpv = iind_ptr1->ipv;
				sk_ptr->uw = -1;

				sk_ptr->sk_next = NULL;
				sk_hptr->sk_next = sk_ptr;
				sk_hptr = sk_hptr->sk_next;
				if (iind_ptr1->iindnt != NULL) {
					iind_ptr1 = iind_ptr1->iindnt;
					continue;
				}
				else v_flag = 1;
			}
		}
		if (u_flag == 0 && v_flag == 0) {
			if (oind_ptr1->oui == iind_ptr1->iui) { // w = w'
				if (oind_ptr1->ota <= iind_ptr1->itd) {

				}
				else {
					if (iind_ptr1->iindnt != NULL) {
						iind_ptr1 = iind_ptr1->iindnt;
						continue;
					}
					else v_flag = 1;
				}
			}
			else {
				if (order_node[oind_ptr1->oui] < order_node[iind_ptr1->iui]) {
					if (oind_ptr1->oindnt != NULL) {
						oind_ptr1 = oind_ptr1->oindnt;
						continue;
					}
					else u_flag = 1;
				}
				else {
					if (iind_ptr1->iindnt != NULL) {
						iind_ptr1 = iind_ptr1->iindnt;
						continue;
					}
					else v_flag = 1;
				}
			}
		}
		if (u_flag == 0) {
			if (oind_ptr1->oindnt != NULL) {
				oind_ptr1 = oind_ptr1->oindnt;
				continue;
			}
			else u_flag = 1;
		}
		if (v_flag == 0) {
			if (iind_ptr1->iindnt != NULL) {
				iind_ptr1 = iind_ptr1->iindnt;
				continue;
			}
			else v_flag = 1;
		}

	}

	sk_hptr = NULL;
	sk_ptr = &sketches;
	if (sk_ptr->sk_next != NULL) {
		sk_ptr = sk_ptr->sk_next;
	}
	else {
		puts("\nNo path is found\n");
		return 0;
	}

	while (sk_ptr != NULL) {
		if (sk_ptr->uw != -1 && sk_ptr->vw != -1) {
			if (sk_ptr->utd >= max_td && sk_ptr->vta <= min_ta) {
				max_td = sk_ptr->utd;
				min_ta = sk_ptr->vta;
				sk_hptr = sk_ptr;
				flag = 2;
			}
		}
		else if (sk_ptr->uw != -1) {
			if (sk_ptr->utd >= max_td && sk_ptr->uta <= min_ta) {
				max_td = sk_ptr->utd;
				min_ta = sk_ptr->uta;
				sk_hptr = sk_ptr;
				flag = 0;
			}
		}
		else {
			if (sk_ptr->vtd >= max_td && sk_ptr->vta <= min_ta) {
				max_td = sk_ptr->vtd;
				min_ta = sk_ptr->vta;
				sk_hptr = sk_ptr;
				flag = 1;
			}
		}
		if (sk_ptr->sk_next != NULL) {
			sk_ptr = sk_ptr->sk_next;
		}
		else {
			break;
		}

	}

	if (sk_hptr != NULL) {
		printf("\nLabel:\n");
		if (flag == 0) printf("w: %d td: %d ta: %d  bv: %d pv: %d\n\n", sk_hptr->uw, sk_hptr->utd, sk_hptr->uta, sk_hptr->ubv, sk_hptr->upv);
		if (flag == 1) printf("w': %d td': %d ta': %d  bv': %d pv': %d\n\n", sk_hptr->vw, sk_hptr->vtd, sk_hptr->vta, sk_hptr->vbv, sk_hptr->vpv);
		if (flag == 2) {
			printf("w: %d td: %d ta: %d  bv: %d pv: %d\n", sk_hptr->uw, sk_hptr->utd, sk_hptr->uta, sk_hptr->ubv, sk_hptr->upv);
			printf("w': %d td': %d ta': %d  bv': %d pv': %d\n\n", sk_hptr->vw, sk_hptr->vtd, sk_hptr->vta, sk_hptr->vbv, sk_hptr->vpv);
		}
		pathconst(order_node, label_in, label_out, u, v, flag);
		print_path();
	}
	else {
		puts("\nNo path is found\n");
		return 0;
	}
	return 0;
}
/*****************************************************************
* sketchgen_c() gen. sketches with money-oriented
****************************************************************/
int sketchgen_c(long order_node[], struct index_in label_in[], struct index_out label_out[], long u, long v, long std, long sta) {
	long u_flag = 0, v_flag = 0, min_cost = 999999, flag = -1;
	std *= 3600, sta *= 3600;
	sk_hptr = &sketches;
	sk_hptr->sk_next = NULL;

	oind_ptr1 = &label_out[u];
	iind_ptr1 = &label_in[v];
	if (oind_ptr1->oindnt != NULL) {
		oind_ptr1 = oind_ptr1->oindnt;
	}
	else {
		u_flag = 1;

	}
	if (iind_ptr1->iindnt != NULL) {
		iind_ptr1 = iind_ptr1->iindnt;
	}
	else {
		v_flag = 1;

	}


	while (u_flag == 0 || v_flag == 0) {

		if (u_flag == 0) {
			if (oind_ptr1->otd<std || oind_ptr1->ota>sta) { // [td,ta] !subset [t,t']
				if (oind_ptr1->oindnt != NULL) {
					oind_ptr1 = oind_ptr1->oindnt;
					continue;
				}
				else u_flag = 1;
			}
			else if (oind_ptr1->oui == v) { // w = v

				sk_ptr = (struct sketch*)malloc(sizeof(struct sketch));
				sk_ptr->uw = oind_ptr1->oui;
				sk_ptr->utd = oind_ptr1->otd;
				sk_ptr->uta = oind_ptr1->ota;
				sk_ptr->ubv = oind_ptr1->obv;
				sk_ptr->upv = oind_ptr1->opv;
				sk_ptr->vw = -1;
				sk_ptr->cost = oind_ptr1->cost;

				sk_ptr->sk_next = NULL;
				sk_hptr->sk_next = sk_ptr;
				sk_hptr = sk_hptr->sk_next;
				if (oind_ptr1->oindnt != NULL) {
					oind_ptr1 = oind_ptr1->oindnt;
					continue;
				}
				else u_flag = 1;
			}
		}
		if (v_flag == 0) {
			if (iind_ptr1->itd<std || iind_ptr1->ita>sta) {// [td',ta'] !subset [t,t']
				if (iind_ptr1->iindnt != NULL) {
					iind_ptr1 = iind_ptr1->iindnt;
					continue;
				}
				else v_flag = 1;
			}
			else if (iind_ptr1->iui == u) {  //w' = u

				sk_ptr = (struct sketch*)malloc(sizeof(struct sketch));
				sk_ptr->vw = iind_ptr1->iui;
				sk_ptr->vtd = iind_ptr1->itd;
				sk_ptr->vta = iind_ptr1->ita;
				sk_ptr->vbv = iind_ptr1->ibv;
				sk_ptr->vpv = iind_ptr1->ipv;
				sk_ptr->uw = -1;
				sk_ptr->cost = iind_ptr1->cost;

				sk_ptr->sk_next = NULL;
				sk_hptr->sk_next = sk_ptr;
				sk_hptr = sk_hptr->sk_next;
				if (iind_ptr1->iindnt != NULL) {
					iind_ptr1 = iind_ptr1->iindnt;
					continue;
				}
				else v_flag = 1;
			}
		}
		if (u_flag == 0 && v_flag == 0) {
			if (oind_ptr1->oui == iind_ptr1->iui) { // w = w'
				if (oind_ptr1->ota <= iind_ptr1->itd) {
					sk_ptr = (struct sketch*)malloc(sizeof(struct sketch));
					sk_ptr->uw = oind_ptr1->oui;
					sk_ptr->utd = oind_ptr1->otd;
					sk_ptr->uta = oind_ptr1->ota;
					sk_ptr->ubv = oind_ptr1->obv;
					sk_ptr->upv = oind_ptr1->opv;
					sk_ptr->vw = iind_ptr1->iui;
					sk_ptr->vtd = iind_ptr1->itd;
					sk_ptr->vta = iind_ptr1->ita;
					sk_ptr->vbv = iind_ptr1->ibv;
					sk_ptr->vpv = iind_ptr1->ipv;
					sk_ptr->cost = (oind_ptr1->cost + iind_ptr1->cost);

					sk_ptr->sk_next = NULL;
					sk_hptr->sk_next = sk_ptr;
					sk_hptr = sk_hptr->sk_next;
					if (oind_ptr1->oindnt != NULL) {
						oind_ptr1 = oind_ptr1->oindnt;
						continue;
					}
					else u_flag = 1;
				}
				else {
					if (iind_ptr1->iindnt != NULL) {
						iind_ptr1 = iind_ptr1->iindnt;
						continue;
					}
					else v_flag = 1;
				}
			}
			else {
				if (order_node[oind_ptr1->oui] < order_node[iind_ptr1->iui]) {
					if (oind_ptr1->oindnt != NULL) {
						oind_ptr1 = oind_ptr1->oindnt;
						continue;
					}
					else u_flag = 1;
				}
				else {
					if (iind_ptr1->iindnt != NULL) {
						iind_ptr1 = iind_ptr1->iindnt;
						continue;
					}
					else v_flag = 1;
				}
			}
		}
		if (u_flag == 0) {
			if (oind_ptr1->oindnt != NULL) {
				oind_ptr1 = oind_ptr1->oindnt;
				continue;
			}
			else u_flag = 1;
		}
		if (v_flag == 0) {
			if (iind_ptr1->iindnt != NULL) {
				iind_ptr1 = iind_ptr1->iindnt;
				continue;
			}
			else v_flag = 1;
		}

	}

	sk_hptr = NULL;
	sk_ptr = &sketches;
	if (sk_ptr->sk_next != NULL) {
		sk_ptr = sk_ptr->sk_next;
	}
	else {
		puts("\n1: No path is found\n");
		return 0;
	}

	while (sk_ptr != NULL) {
		if (sk_ptr->uw != -1 && sk_ptr->vw != -1) {
			if (sk_ptr->cost < min_cost) {

				min_cost = sk_ptr->cost;
				sk_hptr = sk_ptr;
				flag = 2;
			}
		}
		else if (sk_ptr->uw != -1) {
			if (sk_ptr->cost < min_cost) {

				min_cost = sk_ptr->cost;
				sk_hptr = sk_ptr;
				flag = 0;
			}
		}
		else {
			if (sk_ptr->cost < min_cost) {

				min_cost = sk_ptr->cost;
				sk_hptr = sk_ptr;
				flag = 1;
			}
		}
		if (sk_ptr->sk_next != NULL) {
			sk_ptr = sk_ptr->sk_next;
		}
		else {
			break;
		}

	}

	if (sk_hptr != NULL) {
		printf("\nLabel:\n");
		if (flag == 0) printf("w: %d td: %d ta: %d  bv: %d pv: %d \n$: %d\n\n", sk_hptr->uw, sk_hptr->utd, sk_hptr->uta, sk_hptr->ubv, sk_hptr->upv, sk_hptr->cost);
		if (flag == 1) printf("w': %d td': %d ta': %d  bv': %d pv': %d \n$: %d\n\n", sk_hptr->vw, sk_hptr->vtd, sk_hptr->vta, sk_hptr->vbv, sk_hptr->vpv, sk_hptr->cost);
		if (flag == 2) {
			printf("w: %d td: %d ta: %d  bv: %d pv: %d\n", sk_hptr->uw, sk_hptr->utd, sk_hptr->uta, sk_hptr->ubv, sk_hptr->upv);
			printf("w': %d td': %d ta': %d  bv': %d pv': %d\n", sk_hptr->vw, sk_hptr->vtd, sk_hptr->vta, sk_hptr->vbv, sk_hptr->vpv);
			printf("$: %d\n\n", sk_hptr->cost);
		}
		pathconst(order_node, label_in, label_out, u, v, flag);
		print_path();
	}
	else {
		puts("\n2: No path is found\n");
		return 0;
	}
	return 0;
}

/*****************************************************************
* pathconst() for parh construction
****************************************************************/
void pathconst(long order_node[], struct index_in label_in[], struct index_out label_out[], long u, long v, long flag) {
	struct index vin, uout;
	long ins_u;
	path1_ptr = &sans;
	path1_ptr->pnode = -1;
	path2_ptr = (struct path*)malloc(sizeof(struct path));
	path2_ptr->pnode = u; // add start point to path
	path1_ptr->pnode_next = path2_ptr;
	path1_ptr = path1_ptr->pnode_next;

	if (flag == 2) {
		path2_ptr = (struct path*)malloc(sizeof(struct path));
		path2_ptr->pnode = sk_hptr->uw; //add mid. point to path
		path1_ptr->pnode_next = path2_ptr;
		path1_ptr = path1_ptr->pnode_next;
	}

	path2_ptr = (struct path*)malloc(sizeof(struct path));
	path2_ptr->pnode = v; //add end point to path
	path2_ptr->pnode_next = NULL;
	path1_ptr->pnode_next = path2_ptr;
	path1_ptr = path1_ptr->pnode_next;

	if (flag == 0) { //w = v
		uout.ui = sk_hptr->uw; // save the out label
		uout.td = sk_hptr->utd;
		uout.ta = sk_hptr->uta;
		uout.bv = sk_hptr->ubv;
		uout.pv = sk_hptr->upv;
		pathunfold(uout, order_node, label_in, label_out, u, v);
	}
	else if (flag == 1) { //w' = u
		vin.ui = sk_hptr->vw; // save in label 
		vin.td = sk_hptr->vtd;
		vin.ta = sk_hptr->vta;
		vin.bv = sk_hptr->vbv;
		vin.pv = sk_hptr->vpv;
		pathunfold(vin, order_node, label_in, label_out, u, v);
	}
	else { // w = w'
		uout.ui = sk_hptr->uw; // save the out label
		uout.td = sk_hptr->utd;
		uout.ta = sk_hptr->uta;
		uout.bv = sk_hptr->ubv;
		uout.pv = sk_hptr->upv;

		vin.ui = sk_hptr->vw; // save in label 
		vin.td = sk_hptr->vtd;
		vin.ta = sk_hptr->vta;
		vin.bv = sk_hptr->vbv;
		vin.pv = sk_hptr->vpv;
		ins_u = sk_hptr->uw;

		pathunfold(uout, order_node, label_in, label_out, u, sk_hptr->uw);
		pathunfold(vin, order_node, label_in, label_out, ins_u, v);

	}

}

/*****************************************************************
* pathunfold() unfold the labels in pathconstruct()
****************************************************************/
void pathunfold(struct index child, long order_node[], struct index_in label_in[], struct index_out label_out[], long u, long v) {
	struct sketch child2;
	struct index l_child, r_child;

	if (child.pv != -1) {
		path1_ptr = &sans;
		while (path1_ptr->pnode != u) {
			path1_ptr = path1_ptr->pnode_next;
		}
		path3_ptr = path1_ptr->pnode_next;

		path2_ptr = (struct path*)malloc(sizeof(struct path));
		path2_ptr->pnode = child.pv; // add p to path
		path1_ptr->pnode_next = path2_ptr;
		path2_ptr->pnode_next = path3_ptr;

		child2 = findchild(child, order_node, label_in, label_out, u, v);
		l_child.td = child2.utd;
		l_child.ta = child2.uta;
		l_child.pv = child2.upv;
		l_child.bv = child2.ubv;
		l_child.ui = child2.uw;
		r_child.td = child2.vtd;
		r_child.ta = child2.vta;
		r_child.pv = child2.vpv;
		r_child.bv = child2.vbv;
		r_child.ui = child2.vw;
		pathunfold(l_child, order_node, label_in, label_out, u, child.pv);
		pathunfold(r_child, order_node, label_in, label_out, child.pv, v);
	}
	else {
		path1_ptr = &sans;
		while (path1_ptr->pnode != u) {
			path1_ptr = path1_ptr->pnode_next;
		}
		path1_ptr->td = child.td;
		path1_ptr->ta = child.ta;
	}

}

/*****************************************************************
* findchild() find child node for pathunfold()
****************************************************************/
sketch findchild(struct index child, long order_node[], struct index_in label_in[], struct index_out label_out[], long u, long v) {
	struct sketch l_child, r_child, child2;
	long flag = 0, lta = 0, rta = 0, l_flag = -1, r_flag = -1;

	l_child = child_sketchgen(child, order_node, label_in, label_out, u, child.pv);
	r_child = child_sketchgen(child, order_node, label_in, label_out, child.pv, v);

	l_sk = &l_child;
	l_sk = l_sk->sk_next;

	while (l_sk != NULL) {
		if (l_sk->vw != -1 && l_sk->uw != -1) {
			lta = l_sk->vta;
			l_flag = 0;
		}
		else if (l_sk->uw != -1) {
			lta = l_sk->uta;
			l_flag = 1;
		}
		else {
			lta = l_sk->vta;
			l_flag = 2;
		}
		r_sk = &r_child;
		r_sk = r_sk->sk_next;
		while (r_sk != NULL) {
			if (r_sk->vw != -1 && r_sk->uw != -1) {
				rta = r_sk->vta;
				r_flag = 0;
			}
			else if (r_sk->uw != -1) {
				rta = r_sk->uta;
				r_flag = 1;
			}
			else {
				rta = r_sk->vta;
				r_flag = 2;
			}
			if (lta <= rta) {
				flag = 1;
				break;
			}
			else {
				if (r_sk->sk_next != NULL) {
					r_sk = r_sk->sk_next;
				}
				else {
					break;
				}
			}
		}
		if (flag == 1) break;
		if (l_sk->sk_next != NULL) {
			l_sk = l_sk->sk_next;
		}
		else {
			puts("ERR");
			exit(0);
		}
	}
	if (l_flag == 0) {// L1
		child2.uw = -1;
		child2.utd = l_sk->utd;
		child2.uta = l_sk->vta;
		child2.upv = l_sk->uw;
		child2.ubv = -1;
	}
	else if (l_flag == 1) {
		child2.uw = l_sk->uw;
		child2.utd = l_sk->utd;
		child2.uta = l_sk->uta;
		child2.upv = l_sk->upv;
		child2.ubv = l_sk->ubv;
	}
	else {
		child2.uw = l_sk->vw;
		child2.utd = l_sk->vtd;
		child2.uta = l_sk->vta;
		child2.upv = l_sk->vpv;
		child2.ubv = l_sk->vbv;
	}
	if (r_flag == 0) {// L2
		child2.vw = -1;
		child2.vtd = r_sk->utd;
		child2.vta = r_sk->vta;
		child2.vpv = r_sk->uw;
		child2.vbv = -1;
	}
	else if (r_flag == 1) {
		child2.vw = r_sk->uw;
		child2.vtd = r_sk->utd;
		child2.vta = r_sk->uta;
		child2.vpv = r_sk->upv;
		child2.vbv = r_sk->ubv;
	}
	else {
		child2.vw = r_sk->vw;
		child2.vtd = r_sk->vtd;
		child2.vta = r_sk->vta;
		child2.vpv = r_sk->vpv;
		child2.vbv = r_sk->vbv;
	}

	return child2;
}

/*****************************************************************
* child_sketchgen() gen. child sketches for findchild()
****************************************************************/
sketch child_sketchgen(struct index child, long order_node[], struct index_in label_in[], struct index_out label_out[], long u, long v) {
	long u_flag = 0, v_flag = 0;
	struct sketch sketches2;
	sk_hptr = &sketches2;

	oind_ptr1 = &label_out[u];
	iind_ptr1 = &label_in[v];
	if (oind_ptr1->oindnt != NULL) {
		oind_ptr1 = oind_ptr1->oindnt;
	}
	else {

		u_flag = 1;
	}
	if (iind_ptr1->iindnt != NULL) {
		iind_ptr1 = iind_ptr1->iindnt;
	}
	else {

		v_flag = 1;
	}

	while (u_flag == 0 || v_flag == 0) {

		if (u_flag == 0) {
			if (oind_ptr1->otd<child.td || oind_ptr1->ota>child.ta) { // [td,ta] !subset [t,t']

				if (oind_ptr1->oindnt != NULL) {
					oind_ptr1 = oind_ptr1->oindnt;
					continue;
				}
				else u_flag = 1;
			}
			else if (oind_ptr1->oui == v) { // w = v

				sk_ptr = (struct sketch*)malloc(sizeof(struct sketch));
				sk_ptr->uw = oind_ptr1->oui;
				sk_ptr->utd = oind_ptr1->otd;
				sk_ptr->uta = oind_ptr1->ota;
				sk_ptr->ubv = oind_ptr1->obv;
				sk_ptr->upv = oind_ptr1->opv;
				sk_ptr->vw = -1;
				sk_ptr->sk_next = NULL;
				sk_hptr->sk_next = sk_ptr;
				sk_hptr = sk_hptr->sk_next;
				if (oind_ptr1->oindnt != NULL) {
					oind_ptr1 = oind_ptr1->oindnt;
					continue;
				}
				else u_flag = 1;
			}
		}
		if (v_flag == 0) {

			if (iind_ptr1->itd<child.td || iind_ptr1->ita>child.ta) {// [td',ta'] !subset [t,t']

				if (iind_ptr1->iindnt != NULL) {
					iind_ptr1 = iind_ptr1->iindnt;
					continue;
				}
				else v_flag = 1;
			}
			else if (iind_ptr1->iui == u) {  //w' = u

				sk_ptr = (struct sketch*)malloc(sizeof(struct sketch));
				sk_ptr->vw = iind_ptr1->iui;
				sk_ptr->vtd = iind_ptr1->itd;
				sk_ptr->vta = iind_ptr1->ita;
				sk_ptr->vbv = iind_ptr1->ibv;
				sk_ptr->vpv = iind_ptr1->ipv;
				sk_ptr->uw = -1;
				sk_ptr->sk_next = NULL;
				sk_hptr->sk_next = sk_ptr;
				sk_hptr = sk_hptr->sk_next;
				if (iind_ptr1->iindnt != NULL) {
					iind_ptr1 = iind_ptr1->iindnt;
					continue;
				}
				else v_flag = 1;
			}
		}

		if (u_flag == 0 && v_flag == 0) {
			if (oind_ptr1->oui == iind_ptr1->iui) { // w = w'
				if (oind_ptr1->ota <= iind_ptr1->itd) {

					sk_ptr = (struct sketch*)malloc(sizeof(struct sketch));
					sk_ptr->uw = oind_ptr1->oui;
					sk_ptr->utd = oind_ptr1->otd;
					sk_ptr->uta = oind_ptr1->ota;
					sk_ptr->ubv = oind_ptr1->obv;
					sk_ptr->upv = oind_ptr1->opv;
					sk_ptr->vw = iind_ptr1->iui;
					sk_ptr->vtd = iind_ptr1->itd;
					sk_ptr->vta = iind_ptr1->ita;
					sk_ptr->vbv = iind_ptr1->ibv;
					sk_ptr->vpv = iind_ptr1->ipv;
					sk_ptr->sk_next = NULL;
					sk_hptr->sk_next = sk_ptr;
					sk_hptr = sk_hptr->sk_next;
					if (oind_ptr1->oindnt != NULL) {
						oind_ptr1 = oind_ptr1->oindnt;
						continue;
					}
					else u_flag = 1;
				}
				else {
					if (iind_ptr1->iindnt != NULL) {
						iind_ptr1 = iind_ptr1->iindnt;
						continue;
					}
					else v_flag = 1;
				}
			}
			else {
				if (order_node[oind_ptr1->oui] < order_node[iind_ptr1->iui]) {
					if (oind_ptr1->oindnt != NULL) {
						oind_ptr1 = oind_ptr1->oindnt;
						continue;
					}
					else u_flag = 1;
				}
				else {
					if (iind_ptr1->iindnt != NULL) {
						iind_ptr1 = iind_ptr1->iindnt;
						continue;
					}
					else v_flag = 1;
				}
			}
		}
		if (u_flag == 0) {
			if (oind_ptr1->oindnt != NULL) {
				oind_ptr1 = oind_ptr1->oindnt;
				continue;
			}
			else u_flag = 1;
		}
		if (v_flag == 0) {
			if (iind_ptr1->iindnt != NULL) {
				iind_ptr1 = iind_ptr1->iindnt;
				continue;
			}
			else v_flag = 1;
		}

	}
	return sketches2;
}

/*****************************************************************
* print_path() output the route found by Query
****************************************************************/
void print_path() {
	long h, m, s, he, me, se;
	printf("SDP:\n");
	path1_ptr = &sans;
	path1_ptr = path1_ptr->pnode_next;
	while (path1_ptr != NULL) {
		printf(" %d ", path1_ptr->pnode);
		if (path1_ptr->pnode_next != NULL) {
			h = path1_ptr->td / 3600;
			m = (path1_ptr->td - h * 3600) / 60;
			s = (path1_ptr->td - h * 3600) - m * 60;
			he = path1_ptr->ta / 3600;
			me = (path1_ptr->ta - he * 3600) / 60;
			se = (path1_ptr->ta - he * 3600) - me * 60;
			printf("-<");
			if (h < 10) { printf("0%d:", h); }
			else { printf("%d:", h); }
			if (m < 10) { printf("0%d:", m); }
			else { printf("%d:", m); }
			if (s < 10) { printf("0%d", s); }
			else { printf("%d", s); }
			printf(",");
			if (he < 10) { printf("0%d:", he); }
			else { printf("%d:", he); }
			if (me < 10) { printf("0%d:", me); }
			else { printf("%d:", me); }
			if (se < 10) { printf("0%d", se); }
			else { printf("%d", se); }
			printf(">-");
			path1_ptr = path1_ptr->pnode_next;
		}
		else {
			printf("\n\n");
			break;
		}
	}
}

