#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include <algorithm>
#include <exception>

class Range {
private:
	int a;
	int b;
	bool null;
	void validate_range()
	{
		if (a > b)
			throw std::runtime_error("Invalid range");		
	}

public:
	Range (int a, int b) : a(a), b(b), null(false)
	{
		validate_range();	
	}

	Range() : a(-1), b(-1), null(true) {}

	bool is_null() {return null;}

	const Range intersect(const Range &rhs) const
	{
		int _a = std::max (a, rhs.a);
		int _b = std::min (b, rhs.b);

		if (_a > _b) {
			return Range();
		}

		return Range(_a, _b);
	}

	const Range intersect(int x, int y) const
	{
		if (x > y)
			throw std::runtime_error ("Invalid range given to intersect");

		int _a = std::max (a, x);
		int _b = std::min (b, y);

		if (_a > _b) {
			return Range();
		}

		return Range(_a, _b);
	}

	int get_a() const
	{
		return a;
	}

	int get_b() const
	{
		return b;
	}

	bool operator==(const Range& other) const
	{
		return a == other.a && b == other.b;
	}

	bool is_contained_in (const Range& r_in) const
	{
		return (a >= r_in.a && b <= r_in.b);
	}

	void print() const
	{
		std::cout << "[" << a << "," << b << "]" << std::endl;
	}
};


class Node {
	const Range r;
	int min;
	std::list<Node> children;

public:
	Node (Range r) : r(r), min(4), children() {}

	void insert(const Range rr_in)
	{
		auto _r = rr_in.intersect(r);
		auto it = children.begin();


		if (_r.is_null() || _r == r)
			return;

		if (children.empty()) {
			children.push_back(_r);
			return;
		}

		/* If a_0 > a, we need to insert a node right in the beginning */
		if (children.front().r.get_a() > r.get_a()) {
			auto temp = _r.intersect(r.get_a(), children.front().r.get_a() - 1);
			if (!temp.is_null()) {
				children.insert(it, temp);
			}
		}

		/* Insert _r into the first (old) child node */
		(*it).insert(_r);
		

		/* Capture the b value of the old head before advancing the
		 * iterator */
		int b_prev = (*it).r.get_b();
		++it;

		/* Insert _r into all children nodes and also
		 * handle the gaps in the children
		*/

		for (; it != children.end(); ++it) {
			/* Insert r_in into the child */
			(*it).insert(_r);

			/* if there's a gap in the beginning add to the children */
			if ((*it).r.get_a() - b_prev > 1) {
				auto temp = _r.intersect(b_prev + 1, (*it).r.get_a() - 1);
				if (!temp.is_null())
					children.insert(it, temp);
			}

			b_prev = (*it).r.get_b();
		}

		/* Fill the gap in the end as well */
		if (children.back().r.get_b() < r.get_b()) {
			auto __r = _r.intersect(children.back().r.get_b() + 1, r.get_b());
			if (!__r.is_null())
				children.push_back(__r);
		}
	}

	int find_min_across_contained_nodes(const Range& range) const
	{
		if (r.is_contained_in(range)) {
			return min;
		} else {
			int min = 4;

			for (auto it = children.cbegin(); it != children.cend(); ++it)
				if ((*it).r.is_contained_in(range)) {
					if ((*it).min < min)
						min = (*it).min;
				}
				else {
					int _min = (*it).find_min_across_contained_nodes(range);
					if (_min < min)
						min = _min;
				}

			return min;
		}
	}

	int compute_min(const std::vector<int> & v)
	{	
		auto it = children.begin();

		for (int i = r.get_a(); i <= r.get_b(); i++) {
			if ((it == children.end()) || i < (*it).r.get_a()) {
				if(v[i] < min)
					min = v[i];
			} else {
				if ((*it).compute_min(v) < min)
					min = (*it).min;

				i = (*it).r.get_b();
				++it;
			}
		}

		return min;
	}

	void print(const int level) const
	{
		int l = level;

		while (--l)
			std::cout << '\t';

		r.print();

		for (auto it = children.cbegin(); it!= children.cend(); ++it ) {
			(*it).print(level + 1);
		}
	}
};


int main()
{
	unsigned long int N, T;

	std::cin >> N;
	std::cin >> T;

	Node root(Range(0,N - 1));
	std::vector<int> widths;
	widths.reserve(N);

	while (N--) {
		int w;
		std::cin >> w;
		widths.push_back(w);
	}

	std::vector< std::pair <int, int> > test_cases;
	test_cases.reserve(T);

	while (T--) {
		int a, b;
		std::cin >> a >> b;
		root.insert(Range(a, b));
		test_cases.push_back(std::make_pair(a,b));
	}

	root.print(1);

	return 0;
	root.compute_min(widths);

	for (auto it = test_cases.cbegin(); it != test_cases.cend(); ++it) {
		int min = root.find_min_across_contained_nodes(Range((*it).first, (*it).second));
		//if (min == 4)
		//		throw std::runtime_error("min == 4. Impossible");
		std::cout << min << std::endl;
	}

}