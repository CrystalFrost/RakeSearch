#ifndef DLX_DLS_h
#define DLX_DLS_h

#include <vector>
#include <fstream>
#include <iostream>
#include <chrono>
#include <string>
#include <algorithm>

static inline double cpuTime(void) { return (double)clock() / CLOCKS_PER_SEC; }

using namespace std;

class DLX_column {
public:
	int size;
	int column_number;
	int row_id;
	DLX_column *Left;
	DLX_column *Right;
	DLX_column *Up;
	DLX_column *Down;
	DLX_column *Column;
};

class orth_mate_search
{
public:
	void generate_permutations(int n, vector<vector<int>> &perm, bool diag);
	void construct_square_from_tv(vector<vector<int>> &tv_set, vector<int> &tv_ind, vector<vector<int>> &SQ);
	void construct_squares_from_tv_set(vector<vector<int>>&tv_set, vector<vector<int>> &tv_index_sets, vector<vector<vector<int>>> &SQUARES);
	void TV_check(vector<vector<int>> &TVSET, vector<vector<int>> &LS, vector<int> &indices);
	void cover(DLX_column *&c);
	void uncover(DLX_column *&c);
	void choose_c(DLX_column &h, DLX_column *&c);
	void print_solution(vector<DLX_column*> &ps);
	void search_limited(int k, DLX_column &h, vector<DLX_column*> &ps, vector<vector<int>> &tvr, bool &cont, unsigned long long &limit, bool &count_only, unsigned long long &count);
	void search(int k, DLX_column &h, vector<DLX_column*> &ps, vector<vector<int>> &tvr);
	void TVSET_TO_DLX(DLX_column &root, vector<vector<int>> & tvset, vector<DLX_column*> & elements);
	void SQ_TO_DLX(DLX_column &root, vector<vector<int>> & SQ, vector<DLX_column*> & elements);
	void TVSET_TO_DLX_EXT(DLX_column &root, vector<vector<int>> & tvset, vector<DLX_column*> & columns, vector<vector<DLX_column*>> &rows);
	vector<vector<int>> find_tv_dlx(int n, vector<vector<int>> &SQ);
	void check_dlx_rc1(vector<vector<int>> SQ, vector<vector<vector<int>>> &ort_SQ_vec);
	void generate_permutations_masked_rc1(int n, vector<vector<int>> &perm, vector<vector<int>> mask_LS, bool diag);
	bool isdiagls(int n, vector<vector<int>> &SQ);
	void Generate_DLS_masked_DLXrefresh(int n, bool diag, unsigned long long limit, vector<vector<int>> mask, vector<vector<vector<int>>> &squares_vec);
	void print_sq(vector<vector<int>> &SQ);
	vector<vector<int>> compute_masked_LS(vector<vector<int>> &LS, vector<vector<int>> &MASK);
	vector<vector<int>> compute_masked_LS(vector<vector<int>> &LS, int k);
};

#endif