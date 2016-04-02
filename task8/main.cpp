#include <bits/stdc++.h>

using namespace std;

struct Node {
    string type;
    long long val;
    shared_ptr < Node > l, r;

    Node(string type, shared_ptr < Node > l, shared_ptr < Node > r)
        : type(type)
        , l(l)
        , r(r) 
    {}

    Node(string type, long long val)
        : type(type)
        , val(val) 
    {}
};

struct OrdinalParser {
    string data; 
    int cur; 

    OrdinalParser(string data)
        : data(data)
        , cur(0) 
    {}
     
    string nextToken() {
        assert(cur <= (int)data.size());
        if (cur == (int)data.size()) 
            return "";
        return string(1, data[cur]);
    }

    void shiftCur() {
        cur++;
    }

    shared_ptr < Node > parseTerm() {
        if (nextToken() == "w") {
            shiftCur();
            return shared_ptr < Node > (new Node("w", 0));
        }
        if (nextToken() == "(") {
            shiftCur(); 
            auto v = parseExp();
            assert(nextToken() == ")");
            shiftCur();
            return v;
        }
        assert(nextToken().size() == 1 && isdigit(nextToken()[0]));
        long long x = 0;
        while (nextToken().size() == 1 && isdigit(nextToken()[0])) {
            int d = nextToken()[0] - '0';
            x = x * 10 + d;
            shiftCur();
        }
        return shared_ptr < Node > (new Node("0", x));
    }

    shared_ptr < Node > parseMultiplier() {
        auto v = parseTerm();
        while (nextToken() == "^") {
            shiftCur();
            auto u = parseMultiplier();
            v = shared_ptr < Node > (new Node("^", v, u));
        }
        return v;
    }

    shared_ptr < Node > parseSummand() {
        auto v = parseMultiplier();
        while (nextToken() == "*") {
            shiftCur();
            auto u = parseMultiplier();
            v = shared_ptr < Node > (new Node("*", v, u));
        }
        return v;
    }

    shared_ptr < Node > parseExp() {
        auto v = parseSummand();
        while (nextToken() == "+") {
            shiftCur();
            auto u = parseSummand();
            v = shared_ptr < Node > (new Node("+", v, u));
        }
        return v;
    }
};

string removeSpace(string s) {
    string res;
    for (auto ch: s)
        if (!isspace(ch))
            res += ch;
    return res;
}

vector < string > split(string s, string pattern) {
    vector < string > res;
    int cur = 0;
    for (int i = 0; i + pattern.size() <= s.size(); i++) {
        if (s.substr(i, pattern.size()) == pattern) {
            res.push_back(s.substr(cur, i - cur));
            i += pattern.size();
            cur = i;
            i--;
        }
    }
    assert(cur < (int)s.size());
    res.push_back(s.substr(cur, s.size() - cur));
    return res;
}

shared_ptr < Node > parseOrdinal(string s) {
    OrdinalParser p(s);
    return p.parseExp(); 
}

struct Ordinal {
    Ordinal * l, * r;
    string type;
    long long val;
    Ordinal(Ordinal * l, Ordinal * r)
        : l(l)
        , r(r)
        , type("p")
        , val(0) 
    {} 

    Ordinal(long long val)
        : l(NULL)
        , r(NULL)
        , type("val")
        , val(val) 
    {}
};

bool lessW(Ordinal * a, Ordinal * b);

bool lessEW(Ordinal * a, Ordinal * b);

Ordinal * addO(Ordinal * a, Ordinal * b);

Ordinal * mulDot(Ordinal * a, Ordinal * b);

Ordinal * expO(Ordinal * a, Ordinal * b);


Ordinal * no(Ordinal * a, Ordinal * b) {
    return new Ordinal(a, b);
}

Ordinal * no(long long x) {
    return new Ordinal(x);
}

Ordinal * evaluate(shared_ptr < Node > ptr) {
    if (ptr->type == "+")
        return addO(evaluate(ptr->l), evaluate(ptr->r));
    if (ptr->type == "*")
        return mulDot(evaluate(ptr->l), evaluate(ptr->r));
    if (ptr->type == "^")
        return expO(evaluate(ptr->l), evaluate(ptr->r));
    if (ptr->type == "0") 
        return no(ptr->val);
    if (ptr->type == "w")
        return no(no(no(1), no(1)), no(0));

    assert(false); 
}

Ordinal * first(Ordinal * a) {
    assert(a->l != NULL);
    return a->l;
}

Ordinal * rest(Ordinal * a) {
    //assert(a->r != NULL);
    return a->r;
}

Ordinal * firstn(Ordinal * a, int n) {
    if (n == 0) return NULL;
    return no(first(a), firstn(rest(a), n - 1));
}

bool atom(Ordinal * a) {
    if (a == NULL) return 1;
    return a->type != "p"; 
}

Ordinal * restn(Ordinal * a, int n) {
    if (n == 0) return a;
    return restn(rest(a), n - 1);
}

int len(Ordinal * a) {
    if (atom(a)) return 0;
    return 1 + len(rest(a));
}

Ordinal * fe(Ordinal * a) {
    if (atom(a)) return no(0);
    return first(first(a));
}

int size(Ordinal * a) {
    if (atom(a)) return 1;
    return size(fe(a)) + size(rest(a));
}

Ordinal * fc(Ordinal * a) {
    if (atom(a)) return a;
    return rest(first(a));
}

Ordinal * append(Ordinal * a, Ordinal * b) {
    if (atom(a)) return b;
    return no(first(a), append(rest(a), b));
}

int cmpW(Ordinal * p, Ordinal * q) {
    assert(p->type == "val");
    assert(q->type == "val");
    if (p->val < q->val) return -1;
    if (p->val > q->val) return 1;
    return 0;
}

int cmpO(Ordinal * a, Ordinal * b) {
    if (atom(a) && atom(b)) return cmpW(a, b);
    if (atom(a)) return -1;
    if (atom(b)) return 1;
    int res1 = cmpO(fe(a), fe(b));
    if (res1 != 0) return res1;

    int res2 = cmpW(fc(a), fc(b));
    if (res2 != 0) return res2;
    return cmpO(rest(a), rest(b));
}

bool lessO(Ordinal * a, Ordinal * b) {
    if (cmpO(a, b) == -1) return 1;
    return 0;
}

Ordinal * addW(Ordinal * a, Ordinal * b) {
    assert(a->type == "val");
    assert(b->type == "val");
    return no(a->val + b->val);
}

Ordinal * addO(Ordinal * a, Ordinal * b) {
    if (atom(a) && atom(b)) return addW(a, b);
    int res = cmpO(fe(a), fe(b));
    if (res == -1) return b;
    if (res == 0) 
        return no(no(fe(a), addW(fc(a), fc(b))), rest(b));
    return no(no(fe(a), fc(a)), addO(rest(a) , b));
}

Ordinal * subW(Ordinal * a, Ordinal * b) {
    assert(a->type == "val");
    assert(b->type == "val");
    assert(a->val >= b->val);
    return no(a->val - b->val);
}

Ordinal * subO(Ordinal * a, Ordinal * b) {
    if (atom(a) && atom(b) && lessEW(a, b)) return no(0);
    if (atom(a) && atom(b)) return subW(a, b);
    if (cmpO(fe(a), fe(b)) == -1) return no(0);
    if (cmpO(fe(a), fe(b)) == 1) return a;
    if (lessW(fc(a), fc(b))) return 0;
    if (lessW(fc(b), fc(a))) 
        return no(no(fe(a), subW(fc(a), fc(b))), rest(a));
    return subO(rest(a), rest(b));
}

bool lessEW(Ordinal * a, Ordinal * b) {
    assert(a->type == "val");
    assert(b->type == "val");
    return a->val <= b->val;
}

bool lessW(Ordinal * a, Ordinal * b) {
    assert(a->type == "val");
    assert(b->type == "val");
    return a->val < b->val;
}

Ordinal * mulW(Ordinal * a, Ordinal * b) {
    assert(a->type == "val");
    assert(b->type == "val");
    return no(a->val * b->val);
}

int c(Ordinal * a, Ordinal * b) {
    if (lessO(fe(b), fe(a))) return 1 + c(rest(a), b);
    return 0;
}

int count(Ordinal * a, Ordinal * b, int n) {
    return n + c(restn(a, n), b);
}


Ordinal * padd(Ordinal * a, Ordinal * b, int n) {
    return append(firstn(a, n), addO(restn(a, n), b));
}

Ordinal * pmult(Ordinal * a, Ordinal * b, int n) {
    if (atom(a) && a->val == 0) return no(0);
    if (atom(b) && b->val == 0) return no(0);
    if (atom(a) && atom(b)) return mulW(a, b);
    if (atom(b)) return no(no(fe(a), mulW(fc(a), b)), rest(a));
    int m = count(fe(a), fe(b), n);
    auto r1 = pmult(a, rest(b), m);
    auto r2 = padd(fe(a), fe(b), m); 
    auto g = no(no(r2, fc(b)), r1);
    return g;
}

Ordinal * mulDot(Ordinal * a, Ordinal * b) {
    return pmult(a, b, 0);
}

long long binPow(long long a, long long b) {
    long long res = 1;
    for (; b > 0; b /= 2) {
        if (b % 2 == 1)
            res = res * a;
        a = a * a;
    }
    return res;
}
    
Ordinal * expW(Ordinal * a, Ordinal * b) {
    assert(a->type == "val");
    assert(b->type == "val");
    int x = a->val;
    int y = b->val;
    return no(binPow(x, y)); 
}

Ordinal * exp1(int p, Ordinal * b) {
    if (cmpO(fe(b), no(1)) == 0) 
        return no(no(fc(b), expW(no(p), rest(b))), no(0));
    if (atom(rest(b))) {
        Ordinal * t1 = no(subO(fe(b), no(1)), fc(b));
        Ordinal * t2 = no(t1, 0);
        Ordinal * t3 = no(t2, expW(no(p), rest(b)));
        return no(t3, no(0));
    } 
    Ordinal * c = exp1(p, rest(b));
    return no(no(no(no(subO(fe(b), no(1)), no(1)), fe(c)), fc(c)), no(0));
}

Ordinal * exp2(Ordinal * a, int q) {
    if (q == 1) return a;
    return mulDot(no(no(mulDot(fe(a), no(q - 1)), no(1)), no(0)), a);
}

bool limitp(Ordinal * a) {
    if (atom(a)) return a->val == 0;
    return limitp(rest(a));
}

Ordinal * limitpart(Ordinal * a) {
    if (atom(a)) return no(0);
    return no(first(a), limitpart(rest(a)));
}

int natpart(Ordinal * a) {
    if (atom(a)) return a->val;
    return natpart(rest(a));
}

Ordinal * exp3(Ordinal * a, int q) {
    if (q == 0) return no(1);
    if (q == 1) return a; 
    if (limitp(a)) return exp2(a, q);
    return mulDot(exp3(a, q - 1), a);
}

Ordinal * exp4(Ordinal * a, Ordinal * b) {
    return mulDot(no(no(mulDot(fe(a), limitpart(b)), no(1)), no(0)), exp3(a, natpart(b)));
}

Ordinal * expO(Ordinal * a, Ordinal * b) {
    if ((atom(b) && b->val == 0) || (atom(a) && a->val == 1)) return no(1);
    if (atom(a) && a->val == 0) return no(0);
    if (atom(a) && atom(b)) return expW(a, b);
    if (atom(a)) return exp1(a->val, b);
    if (atom(b)) return exp3(a, b->val);
    return exp4(a, b);
}

int main(){
    freopen("resources/in", "r", stdin);

    vector < string > testsName; 
    cin.clear();
    string s;
    getline(cin, s);
    s = removeSpace(s);
    auto tmp = split(s, "=");
    auto left = parseOrdinal(tmp[0]);
    auto right = parseOrdinal(tmp[1]);
   
    auto resLeft = evaluate(left); 
    auto resRight = evaluate(right); 

    if (cmpO(resLeft, resRight) == 0)
        cout << "Equal" << endl;
    else
        cout << "Not equal";

    return 0;
}


