#include<iostream>
#include<unordered_map>

using namespace std;

typedef unsigned long long uint64;

/*----------------------------------------------
Math
-----------------------------------------------*/

uint64 powmod(uint64 g, uint64 a, uint64 p) {
	/* res = g^a (mod p)
	   g = [0, p), a = [0, p) */
	uint64 res = 1;
	while (a) {
		if (a % 2) {	
			//a is odd
			res = (res*g) % p;
		}
		//a is even
		a = a >> 1;					// a = a/2;
		g = (g*g) % p;				// g^(2*a/2) = (g^2)^(a/2)
	}
	return res;
}

uint64 logmod(uint64 g, uint64 a, uint64 p) {
	/* g^res = a (mod p)
	   g = [0, p), a = [0, p)
	   p is prime */
	uint64 res = 0;
	uint64 gpowres = 1;
	while (res < p) {
		if (gpowres == a) return res;
		res++;
		gpowres = (gpowres*g) % p;
	}
	return -1;
}

uint64 logmod_babygiant(uint64 g, uint64 a, uint64 p) {
	/* g^res = a (mod p)
	   g = [0, p), a = [0, p)
	   p is prime  */
	// Algorithm
	/* res = i*n - j
	   i = (0, n)	j = [0, n)
	   n = sqrt(p) + 1
	   g^(i*n) = g^j * a (mod p) */
	
	uint64 n = (uint64)sqrt(p) + 1;
	uint64 gn = powmod(g, n, p); //g^n

	unordered_map<uint64, uint64> values;
	uint64 cur = gn; // cur = g^(n*i)
	for (uint64 i = 1; i<n; i++) {
		if(!values[cur]) 
			values[cur] = i;
		cur = (cur*gn) % p;
	}

	cur = a; // cur = g^j * a
	for (uint64 j = 0; j<n; j++) {
		if (values[cur]) {
			return values[cur]*n - j;
		}
		cur = (cur*g) % p;
	}
	return -1;
}

bool isPrime(uint64 p) {
	if (p <= 3) return p > 0;
	if (p % 2 == 0 || p % 3 == 0) return false;

	//all numbers bigger then sqrt(p) that divide p
	//have factors smaller then sqrt(p) that also divide p
	//all numbers not divisble by 2 or 3 are in the format 6*k+1 or 6*k-1
	for (uint64 i = 6; i - 1 <= sqrt(p); i += 6)
		if (p % (i + 1) == 0 || p % (i - 1) == 0) return false;
	return true;
}

/*----------------------------------------------
Simulation
-----------------------------------------------*/

struct Values {
	uint64 g, p, a, b, ga, gb, key;
	Values(uint64 g_arg, uint64 p_arg, uint64 a_arg, uint64 b_arg, uint64 ga_arg, uint64 gb_arg, uint64 key_arg) {
		g = g_arg; p = p_arg;
		a = a_arg; b = b_arg;
		ga = ga_arg; gb = gb_arg;
		key = key_arg;
	}
};

Values key_exchange() throw (uint64) {
	uint64 g, p, a, b;
	uint64 ga, gb, gab1, gab2;

	cout << "Unesite g, p, a, b: ";
	cin >> g >> p >> a >> b;
	if (!isPrime(p)) throw p;
	if (g < 0 || g >= p) throw g;
	if (a < 0 || a >= p) throw a;
	if (b < 0 || b >= p) throw b;

	gb = powmod(g, b, p); //have to do this earlier so that A has this info

	cout << "\nUcesnik A (poznato: g, p, a, g^b):\n";
	cout << "a = " << a << "\n";
	cout << "Izracunava g^a...\n";
	ga = powmod(g, a, p);
	cout << "g^a = " << ga << "\n";
	cout << "Salje g^a...\n";
	cout << "Dobija g^b...\n";
	cout << "Izracunava (g^b)^a...\n";
	gab1 = powmod(gb, a, p);
	cout << "Kljuc = " << gab1 << "\n";

	cout << "\n";

	cout << "Ucesnik B (poznato: g, p, b, g^a):\n";
	cout << "b = " << b << "\n";
	cout << "Izracunava g^b...\n";
	//gb = powmod(g, b, p); it was done in an earlier line
	cout << "g^b = " << gb << "\n";
	cout << "Salje g^b...\n";
	cout << "Dobija g^a...\n";
	cout << "Izracunava (g^a)^b...\n";
	gab2 = powmod(ga, b, p);
	cout << "Kljuc = " << gab2 << "\n";

	if (gab1 == gab2) {
		cout << "\nImaju isti kljuc, drugima nepoznat!\n\n";
		return Values(g, p, a, b, ga, gb, gab1);
	}
	cout << "Doslo je do greske, nemaju isti kljuc!\n\n";
	return Values(g, p, a, b, ga, gb, -1);
}

uint64 cryptanalysis(uint64 g, uint64 p, uint64 ga, uint64 gb) {
	cout << "\nNapadac (poznato: g, p, g^a, g^b):\n";
	cout << "Izracunava a...\n";
	uint64 a = logmod_babygiant(g, ga, p);
	cout << "a = " << a << "\n";
	cout << "Izracunava (g^b)^a...\n";
	uint64 gab = powmod(gb, a, p);
	cout << "Kljuc = " << gab << "\n";
	return gab;
}

void man_in_the_middle(Values values) {
	uint64 m, gm, gam, gbm;

	cout << "\nMan in the middle (MM): \n";
	cout << "MM bira svoj privatni kljuc: ";
	cin >> m;
	if (m >= values.p || m < 0) {
		m = m%values.p;
		cout << "Izabrani kljuc je neispravan, vas kljuc je: " << m << "\n";
	}
	cout << "\nUcesnici (A, B, MM) racunaju svoje javne kljuceve g^a, g^b, g^m...";

	gm = powmod(values.g, m, values.p);
	cout << "\n\nA salje svoj javni kljuc (" << values.ga << ") B.";
	cout << "\nMM presrece paket i odgovara sa svojim javnim kljucem (" << gm << ").";
	cout << "\nA misli da je dobio javni kljuc od B i racuna zajednicki kljuc.";
	gam = powmod(gm, values.a, values.p);
	cout << "\nMM racuna isti zajednicki kljuc kao A.";

	cout << "\n\nMM salje svoj javni kljuc (" <<  gm << ") B.";
	cout << "\nB misli da je dobio javni kljuc od A i racuna zajednicki kljuc.";
	gbm = powmod(gm, values.b, values.p);
	cout << "\nB salje svoj javni kljuc (" << values.gb << ") ucesniku koji misli da je A, ali je u stvari MM.";
	cout << "\nMM racuna isti zajednicki kljuc kao B.";
	
	cout << "\n\nA i MM imaju zajednicki kljuc " << gam << " dok B i MM imaju zajednicki kljuc " << gbm << ",";
	cout << "\nMM sada moze jednim kljucem da prevodi poruke od jednog, procita, kriptuje drugim kljucem i posalje mu!";
	cout << "\nNapadac je uspeo!!\n\n";
}


void main() {
	try {
		char control;

		//key exchange
		Values values = key_exchange();
		if (values.key < 0) return;

		//cryptanalysis
		cout << "\nUnesite plus kako biste nastavili! ";
		cin >> control;
		if (control != '+') throw(control);

		if (values.key == cryptanalysis(values.g, values.p, values.ga, values.gb))
			cout << "\nNapadac je nasao kljuc!!\n\n";
		else {
			cout << "Doslo je do greske, napadac nije nasao kljuc!\n\n";
			return;
		}

		//man in the middle
		cout << "\nUnesite plus kako biste nastavili! ";
		cin >> control;
		if (control != '+') throw(control);

		man_in_the_middle(values);
	}
	catch (uint64 num) {
		cout << "\n---ERROR--- Unet nepravilan broj: " << num << " ---ERROR---\n\n";
	}
	catch (char) {}
}