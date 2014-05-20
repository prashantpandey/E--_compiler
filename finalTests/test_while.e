event a(int a2, double a3);

void abc(int a2, double f) {
    int g = 0;
    int h = 5;
    int i = 10;
    int j = 15;
    string s = "\n";
    
    while(g < 20) {
	g = g + 1;
	prt -> g;
	prt -> s;
	while(h < 20) {
	    h = h + 1;
	    prt -> h;
	    prt -> s;
	    while(i < 20) {
		i = i + 1;
	    prt -> i;
	    prt -> s;
		while(j < 20) {
		    j = j + 1;
	    prt -> j;
	    prt -> s;
		    break 3;
		}
	    }
	    while(i < 10) {
		i = i + 1;
	    prt -> i;
	    prt -> s;
	    break 2;
	    }
	}
    }
    prt -> g;
};

a(a2, a3) -> {
   abc(a2, a3);
;};
