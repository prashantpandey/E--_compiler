event a(int a2, double a3);
int res;
string s = "\\n";

int abc(int a2, double f) {
    prt -> a2;
    prt -> s;
    prt -> f;
    prt -> s;

    a2 = a2 + 1;
    f = f + 1;
    
    if(a2 == 85) {
	return a2;
    }
    abc(a2, f);

    //string c = "Hello World..!!!";
    //prt -> c;
    //prt -> s;
    return a2;
};

a(a2, a3) -> {
    prt -> a2;
    prt -> s;
    prt -> a3;
    prt -> s;
   res = abc(a2, a3);
   prt -> res;

;};
