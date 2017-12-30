#include "DLX_DLS.h"

void orth_mate_search::generate_permutations(int n, vector<vector<int>> &perm, bool diag)
{
	vector<int> seed;
	for (int i = 0; i < n; i++) {
		seed.push_back(i);
	}

	do {
		bool acc = true;
		if (diag == true) {
			int md = 0;
			int ad = 0;
			for (int j = 0; j < n; j++) {
				if (seed[j] == j) { md++; }
				if (seed[j] == n - j - 1) { ad++; }
			}
			if ((md != 1) || (ad != 1)) { acc = false; }
		}
		if (acc == true) { perm.push_back(seed); }
	} while (std::next_permutation(seed.begin(), seed.end()));
	//cout << "Generated " << perm.size() << "permutations" << endl;
}

void orth_mate_search::construct_square_from_tv(vector<vector<int>> &tv_set, vector<int> &tv_ind, vector<vector<int>> &SQ)
{
	sort(tv_ind.begin(), tv_ind.end());
	for (int i = 0; i < tv_ind.size(); i++) {
		for (int j = 0; j < tv_set[tv_ind[i]].size(); j++) {
			SQ[j][tv_set[tv_ind[i]][j]] = i;
		}
	}
}

void orth_mate_search::construct_squares_from_tv_set(vector<vector<int>>&tv_set, vector<vector<int>> &tv_index_sets, vector<vector<vector<int>>> &SQUARES)
{
	for (int i = 0; i < tv_index_sets.size(); i++) {
		construct_square_from_tv(tv_set, tv_index_sets[i], SQUARES[i]);
	}
}

void TV_check(vector<vector<int>> &TVSET, vector<vector<int>> &LS, vector<int> &indices) 
{
	int n = LS.size();

	for (int i = 0; i < TVSET.size(); i++) {
		vector<int> tmp(10);
		bool a = true;
		for (int u = 0; u < n; u++) {
			tmp[LS[u][TVSET[i][u]]]++;
			if (tmp[LS[u][TVSET[i][u]]]>1) {
				a = false;
				break;
			}
		}
		if (a == true) { indices.push_back(i); }
	}
}

void orth_mate_search::cover(DLX_column *&c)
{
	//cout << "Covered " << c->column_number << endl;
	c->Right->Left = c->Left;
	c->Left->Right = c->Right;

	DLX_column *i;
	DLX_column *j;
	i = c->Down;
	while (i != c) {
		j = i->Right;
		while (j != i) {
			j->Down->Up = j->Up;
			j->Up->Down = j->Down;
			//	cout << "covered element " << j->row_id << " in column " << j->column_number << endl;
			j->Column->size--;
			if (j->Column->size < 0) {
				cout << "We are in deep trouble" << endl;
			}
			j = j->Right;
		}
		i = i->Down;
	}
}

void orth_mate_search::uncover(DLX_column *&c)
{
	//cout << "Uncovered " << c->column_number << endl;
	DLX_column *i;
	DLX_column *j;
	i = c->Up;
	while (i != c) {
		j = i->Left;
		while (j != i) {
			j->Column->size++;

			j->Down->Up = j;
			j->Up->Down = j;

			j = j->Left;
		}
		i = i->Up;
	}
	c->Right->Left = c;
	c->Left->Right = c;
}

void orth_mate_search::choose_c(DLX_column &h, DLX_column *&c)
{
	DLX_column * j;

	j = h.Right;
	int min = j->size;
	c = j;
	while (j != &h) {
		if (j->size < min) {
			c = j;
			min = j->size;
		}
		j = j->Right;
	}
}

void orth_mate_search::print_solution(vector<DLX_column*> &ps)
{
	cout << endl;
	for (int i = 0; i < ps.size(); i++) {
		cout << ps[i]->row_id << " ";
	}
	cout << endl;
}

void orth_mate_search::search_limited(int k, DLX_column &h, vector<DLX_column*> &ps, vector<vector<int>> &tvr, bool &cont, unsigned long long &limit, bool &count_only, unsigned long long &count)
{
	//pd = partial solution
	if (k > 10) {
		cout << "we are in trouble" << endl;

	}
	//	cout << "Search " << k << endl;
	if (cont == true) {
		if (h.Right == &h) {
			count++;
			if (count_only == false) {
				vector<int> tmpv;
				for (int i = 0; i < ps.size(); i++) {
					tmpv.push_back(ps[i]->row_id);
				}
				tvr.push_back(tmpv);
			}
			if (count > limit) { cont = false; }
			if (count % 10000000 == 0) { cout << count << endl; }
			//print_solution(ps);
		}
		else {
			DLX_column * c = NULL;
			choose_c(h, c);
			//cout << "picked column " << c->column_number << endl;
			cover(c);
			DLX_column * r = c->Down;
			while ((r != c) && (cont == true)) {
				ps.push_back(r);
				DLX_column * j;
				j = r->Right;
				while (j != r) {
					cover(j->Column);
					j = j->Right;
				}

				search_limited(k + 1, h, ps, tvr, cont, limit, count_only, count);
				r = ps.back();
				//questionable.
				ps.pop_back();
				c = r->Column;

				j = r->Left;
				while (j != r) {
					uncover(j->Column);
					j = j->Left;
				}
				r = r->Down;
			}
			uncover(c);
			//return;
		}
	}
}

void orth_mate_search::search(int k, DLX_column &h, vector<DLX_column*> &ps, vector<vector<int>> &tvr)
{
	//pd = partial solution
	if (k > 10) {
		cout << "we are in trouble" << endl;

	}
	//	cout << "Search " << k << endl;
	if (h.Right == &h) {
		vector<int> tmpv;
		for (int i = 0; i < ps.size(); i++) {
			tmpv.push_back(ps[i]->row_id);
		}
		tvr.push_back(tmpv);
		//cout << tvr.size() << endl;
		//print_solution(ps);
	}
	else {
		DLX_column * c = NULL;
		choose_c(h, c);
		//	cout << "picked column " << c->column_number << endl;
		cover(c);
		DLX_column * r = c->Down;
		while (r != c) {
			ps.push_back(r);
			DLX_column * j;
			j = r->Right;
			while (j != r) {
				cover(j->Column);
				j = j->Right;
			}

			search(k + 1, h, ps, tvr);

			r = ps.back();
			//questionable.
			ps.pop_back();
			c = r->Column;

			j = r->Left;
			while (j != r) {
				uncover(j->Column);
				j = j->Left;
			}
			r = r->Down;
		}
		uncover(c);
		//return;
	}
}

void orth_mate_search::TVSET_TO_DLX(DLX_column &root, vector<vector<int>> & tvset, vector<DLX_column*> & elements)
{
	int dimension = tvset[0].size();
	root.Up = NULL;
	root.Down = NULL;
	root.Column = NULL;
	root.row_id = -1;
	root.size = -1;
	//	root.column_number= -1;
	elements.push_back(&root);
	vector<DLX_column *> columns;
	DLX_column * lastleft = &root;
	for (int i = 0; i < dimension* dimension; i++) {
		DLX_column *ct;
		ct = new (DLX_column);
		//	ct->column_number = i;
		ct->Down = ct;
		ct->Up = ct;
		ct->size = 0;
		ct->row_id = 0;
		ct->Column = ct;
		ct->Left = lastleft;
		lastleft->Right = ct;
		lastleft = ct;
		columns.push_back(ct);
		elements.push_back(ct);
	}
	lastleft->Right = &root;
	root.Left = lastleft;

	for (int i = 0; i < tvset.size(); i++) {
		vector<int> curtv = tvset[i];
		vector<DLX_column *> tvrow;
		for (int j = 0; j < curtv.size(); j++) {
			DLX_column *ctve;
			ctve = new (DLX_column);
			//column corresponds to characteristic vector of LS or smth of that kind
			int k = j*dimension + curtv[j];

			ctve->Column = columns[k];
			ctve->Column->size++;
			ctve->Down = columns[k];
			ctve->Up = columns[k]->Up;
			ctve->Up->Down = ctve;
			ctve->Down->Up = ctve;
			ctve->row_id = i;
			//	ctve->column_number = k;
			ctve->size = -10;
			elements.push_back(ctve);
			tvrow.push_back(ctve);
		}

		for (int j = 0; j < tvrow.size() - 1; j++) {
			tvrow[j]->Right = tvrow[j + 1];
			tvrow[j]->Right->Left = tvrow[j];
		}
		tvrow[tvrow.size() - 1]->Right = tvrow[0];
		tvrow[0]->Left = tvrow[tvrow.size() - 1];
	}
	DLX_column *pr = &root;

}

void orth_mate_search::SQ_TO_DLX(DLX_column &root, vector<vector<int>> & SQ, vector<DLX_column*> & elements)
{
	int dimension = SQ[0].size();
	root.Up = NULL;
	root.Down = NULL;
	root.Column = NULL;
	root.row_id = -1;
	root.size = -1;
	//	root.column_number= -1;
	elements.push_back(&root);
	vector<DLX_column *> columns;
	DLX_column * lastleft = &root;
	// first n - row number
	// n to 2n - column number
	//2n to 3n - value
	//3n+1 - diag
	//3n+2 - antidiag
	for (int i = 0; i < 3 * dimension + 2; i++) {
		DLX_column *ct;
		ct = new (DLX_column);
		//	ct->column_number = i;
		ct->Down = ct;
		ct->Up = ct;
		ct->size = 0;
		ct->row_id = 0;
		ct->Column = ct;
		ct->Left = lastleft;
		lastleft->Right = ct;
		lastleft = ct;
		columns.push_back(ct);
		elements.push_back(ct);
	}
	lastleft->Right = &root;
	root.Left = lastleft;
	for (int i = 0; i < SQ.size(); i++) {
		for (int j = 0; j < SQ[i].size(); j++) {
			vector<DLX_column *> tvrow;
			DLX_column *ctve;
			ctve = new (DLX_column);

			ctve->Column = columns[i];
			ctve->Column->size++;
			ctve->Down = columns[i];
			ctve->Up = columns[i]->Up;
			ctve->Up->Down = ctve;
			ctve->Down->Up = ctve;
			ctve->row_id = i*dimension + j;
			//	ctve->column_number = k;
			ctve->size = -10;
			elements.push_back(ctve);
			tvrow.push_back(ctve);

			ctve = new (DLX_column);
			//column corresponds to characteristic vector of LS or smth of that kind

			ctve->Column = columns[dimension + j];
			ctve->Column->size++;
			ctve->Down = columns[dimension + j];
			ctve->Up = columns[dimension + j]->Up;
			ctve->Up->Down = ctve;
			ctve->Down->Up = ctve;
			ctve->row_id = i*dimension + j;
			//	ctve->column_number = k;
			ctve->size = -10;
			elements.push_back(ctve);
			tvrow.push_back(ctve);

			ctve = new (DLX_column);
			//column corresponds to characteristic vector of LS or smth of that kind

			ctve->Column = columns[2 * dimension + SQ[i][j]];
			ctve->Column->size++;
			ctve->Down = columns[2 * dimension + SQ[i][j]];
			ctve->Up = columns[2 * dimension + SQ[i][j]]->Up;
			ctve->Up->Down = ctve;
			ctve->Down->Up = ctve;
			ctve->row_id = i*dimension + j;
			//	ctve->column_number = k;
			ctve->size = -10;
			elements.push_back(ctve);
			tvrow.push_back(ctve);

			if (i == j) {
				ctve = new (DLX_column);
				ctve->Column = columns[3 * dimension];
				ctve->Column->size++;
				ctve->Down = columns[3 * dimension];
				ctve->Up = columns[3 * dimension]->Up;
				ctve->Up->Down = ctve;
				ctve->Down->Up = ctve;
				ctve->row_id = i*dimension + j;
				//	ctve->column_number = k;
				ctve->size = -10;
				elements.push_back(ctve);
				tvrow.push_back(ctve);
			}

			if (i == (dimension - j - 1)) {
				ctve = new (DLX_column);
				ctve->Column = columns[3 * dimension + 1];
				ctve->Column->size++;
				ctve->Down = columns[3 * dimension + 1];
				ctve->Up = columns[3 * dimension + 1]->Up;
				ctve->Up->Down = ctve;
				ctve->Down->Up = ctve;
				ctve->row_id = i*dimension + j;
				//	ctve->column_number = k;
				ctve->size = -10;
				elements.push_back(ctve);
				tvrow.push_back(ctve);
			}

			for (int j = 0; j < tvrow.size() - 1; j++) {
				tvrow[j]->Right = tvrow[j + 1];
				tvrow[j]->Right->Left = tvrow[j];
			}
			tvrow[tvrow.size() - 1]->Right = tvrow[0];
			tvrow[0]->Left = tvrow[tvrow.size() - 1];

		}
	}
	DLX_column *pr = &root;
}

void orth_mate_search::TVSET_TO_DLX_EXT(DLX_column &root, vector<vector<int>> & tvset, vector<DLX_column*> & columns, vector<vector<DLX_column*>> &rows)
{
	int dimension = tvset[0].size();
	root.Up = NULL;
	root.Down = NULL;
	root.Column = NULL;
	root.row_id = -1;
	root.size = -1;
	//	root.column_number= -1;
	columns.clear();
	rows.clear();
	DLX_column * lastleft = &root;
	for (int i = 0; i < dimension* dimension; i++) {
		DLX_column *ct;
		ct = new (DLX_column);
		//	ct->column_number = i;
		ct->Down = ct;
		ct->Up = ct;
		ct->size = 0;
		ct->row_id = 0;
		ct->Column = ct;
		ct->Left = lastleft;
		lastleft->Right = ct;
		lastleft = ct;
		columns.push_back(ct);
	}
	lastleft->Right = &root;
	root.Left = lastleft;

	for (int i = 0; i < tvset.size(); i++) {
		vector<int> curtv = tvset[i];
		vector<DLX_column *> tvrow;
		for (int j = 0; j < curtv.size(); j++) {
			DLX_column *ctve;
			ctve = new (DLX_column);
			//column corresponds to characteristic vector of LS or smth of that kind
			int k = j*dimension + curtv[j];

			ctve->Column = columns[k];
			ctve->Column->size++;
			ctve->Down = columns[k];
			ctve->Up = columns[k]->Up;
			ctve->Up->Down = ctve;
			ctve->Down->Up = ctve;
			ctve->row_id = i;
			//	ctve->column_number = k;
			ctve->size = -10;
			tvrow.push_back(ctve);
		}

		for (int j = 0; j < tvrow.size() - 1; j++) {
			tvrow[j]->Right = tvrow[j + 1];
			tvrow[j]->Right->Left = tvrow[j];
		}
		tvrow[tvrow.size() - 1]->Right = tvrow[0];
		tvrow[0]->Left = tvrow[tvrow.size() - 1];
		rows.push_back(tvrow);
	}
}

vector<vector<int>> orth_mate_search::find_tv_dlx(int n, vector<vector<int>> &SQ)
{
	DLX_column *root;
	root = new (DLX_column);
	vector<DLX_column*> elements;
	SQ_TO_DLX(*root, SQ, elements);
	vector<DLX_column*> ps;
	ps.clear();
	vector<vector<int>> tvr;
	search(0, *root, ps, tvr);


	//cout << "Found " << tvr.size() << " transversals\n";
	for (int i = 0; i < tvr.size(); i++) {
		sort(tvr[i].begin(), tvr[i].end());
		for (int j = 0; j < tvr[i].size(); j++) {
			tvr[i][j] = tvr[i][j] % n;
		}
		//printvector(tvr[i]);
		//cout << endl;
	}
	for (auto i = 0; i < elements.size(); i++) {
		delete elements[i];
	}
	elements.clear();
	return tvr;
}

// Find all orthogonal mates for a given DLS
void orth_mate_search::check_dlx_rc1(vector<vector<int>> SQ, vector<vector<vector<int>>> &ort_SQ_vec )
{
	vector<vector<int>> trm = find_tv_dlx(SQ.size(), SQ);
	DLX_column *root;
	root = new (DLX_column);
	vector<DLX_column*> elements;
	TVSET_TO_DLX(*root, trm, elements);
	vector<DLX_column*> ps;
	ps.clear();
	vector<vector<int>> tvr;
	
	search(0, *root, ps, tvr);
	for (int i = 0; i < tvr.size(); i++)
		sort(tvr[i].begin(), tvr[i].end());
	for (int i = 0; i < elements.size(); i++)
		delete elements[i];
	
	if (tvr.size() == 0)
		return;

	//cout << "LS with orthogonal mates found \n";
	/*out.open(filename, ios::app);
	for (int i = 0; i < SQ.size(); i++) {
		for (int j = 0; j < SQ[i].size(); j++) {
			out << SQ[i][j] << " ";
		}
		out << endl;
	}
	out << "Found " << tvr.size() << " sets of disjoint transversals" << endl;
	out << "(DLX_refresh)Total: " << trm.size() << " transversals" << endl;*/
	ort_SQ_vec.resize(tvr.size());
	vector<vector<int>> ort_SQ(SQ.size(), vector<int>(SQ.size()));
	for (int i = 0; i < tvr.size(); i++) {
		for (auto u = 0; u < SQ.size(); u++)
			for (auto v = 0; v < SQ.size(); v++)
				ort_SQ[v][trm[tvr[i][u]][v]] = u;
		ort_SQ_vec[i] = ort_SQ;
	}
}

void orth_mate_search::generate_permutations_masked_rc1(int n, vector<vector<int>> &perm, vector<vector<int>> mask_LS, bool diag)
{
	vector<vector<int>> MTV(10, vector<int>(10));
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			MTV[i][j] = -1;
		}
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (mask_LS[i][j] != -1) {
				MTV[mask_LS[i][j]][i] = j;
			}
		}
	}
	vector<vector<int>> pm10;
	generate_permutations(n, pm10, diag);
	vector<vector<int>> res;
	for (int i = 0; i < pm10.size(); i++) {
		for (int k = 0; k < MTV.size(); k++) {
			bool acc = true;
			for (int j = 0; j < pm10[i].size(); j++) {
				if ((pm10[i][j] != MTV[k][j]) && (MTV[k][j] != -1)) {
					acc = false;
					break;
				}
			}
			if (acc == true) {
				res.push_back(pm10[i]);
			}
		}
	}
	perm = res;
}

bool orth_mate_search::isdiagls(int n, vector<vector<int>> &SQ)
{
	bool b = true;
	for (auto i = 0; i < n; i++) {
		vector<int> r(n);
		//checking rows
		for (auto j = 0; j < n; j++) {
			r[SQ[i][j]]++;
			if (r[SQ[i][j]]>1) {
				b = false;
				return false;
			}
		}
		//checking columns
		vector<int> c(n);
		for (auto j = 0; j < n; j++) {
			c[SQ[j][i]]++;
			if (c[SQ[j][i]]>1) {
				b = false;
				return false;
			}
		}
	}
	vector<int> md(n);
	vector<int> ad(n);
	//checking diags;
	for (auto j = 0; j < n; j++) {
		md[SQ[j][j]]++;
		ad[SQ[j][n - j - 1]]++;
		if ((md[SQ[j][j]]>1) || (ad[SQ[j][n - j - 1]]>1)) {
			b = false;
			return false;
		}
	}
	return b;
}

void orth_mate_search::Generate_DLS_masked_DLXrefresh( int n, bool diag, unsigned long long limit, 
													   vector<vector<int>> mask, vector<vector<vector<int>>> &squares_vec)
{
	vector<vector<int>> perm_diag;
	generate_permutations_masked_rc1(n, perm_diag, mask, diag);
	sort(perm_diag.begin(), perm_diag.end());

	DLX_column *root;
	root = new (DLX_column);
	vector<DLX_column*> elements;
	TVSET_TO_DLX(*root, perm_diag, elements);

	vector<DLX_column*> ps;
	ps.clear();
	vector<vector<int>> tvr;
	bool cont = true;
	double t1 = cpuTime();
	unsigned long long count = 0;
	bool count_only = false;
	search_limited(0, *root, ps, tvr, cont, limit, count_only, count);
	double t2 = cpuTime();

	//cout << tvr.size() << " squares generated in " << t2 - t1 << " seconds" << endl;

	double sc_t1 = cpuTime();
	vector<vector<vector<int>>> SQUARES(tvr.size(), vector<vector<int>>(n, vector<int>(n)));
	construct_squares_from_tv_set(perm_diag, tvr, SQUARES);
	squares_vec = SQUARES;
	/*
	vector<vector<vector<int>>> ort_SQ_vec;
	double OLDDLX_check0_rc1 = cpuTime();
	for (int i = 0; i < SQUARES.size(); i++) {
		check_dlx_rc1(SQUARES[i], ort_SQ_vec);
	}
	double OLDDLX_check1_rc1 = cpuTime();
	cout << "checking squares with DLX_refresh algorithm finished\n";
	//checking squares with new DLX implementation

	//check_dlx_rc1

	cout << "RESULTS\n";

	cout << "Checking all SQUARES using DLX refresh algorithm took " << OLDDLX_check1_rc1 - OLDDLX_check0_rc1 << " seconds\n";

	//cout << "Checking all squares took " << sc_check_t2 - sc_check_t1 << " seconds,\n";
	//check_squares_DLX(SQUARES, true, logname);*/
}

void orth_mate_search::print_sq(vector<vector<int>> &SQ)
{
	cout << endl;
	for (int i = 0; i < SQ.size(); i++) {
		for (int j = 0; j < SQ[i].size(); j++) {
			cout << SQ[i][j] << " ";
		}
		cout << endl;
	}
}

vector<vector<int>> orth_mate_search::compute_masked_LS(vector<vector<int>> &LS, vector<vector<int>> &MASK)
{
	vector<vector<int>> res(LS);
	if (MASK.size() != LS.size()) {
		std::cout << "LS and MASK sizes dont match \n";
	}
	for (int i = 0; i < MASK.size(); i++) {
		if (MASK[i].size() != LS[i].size()) {
			std::cout << "LS and MASK sizes dont match @ " << i << "\n";
		}
		for (int j = 0; j < MASK[i].size(); j++) {
			if (MASK[i][j] == -1) {
				res[i][j] = -1;
			}
		}
	}
	return res;
}

vector<vector<int>> orth_mate_search::compute_masked_LS(vector<vector<int>> &LS, int k)
{
	vector<vector<int>> res(LS);
	int n = LS.size();
	for (int i = k; i < n*n; i++) {
		res[i / n][i%n] = -1;
	}
	return res;
}