event xyz(int a2, double a3);

void abc(int a2, double f) {
    int g = 0;
    
    if((2>3) && (6>7) || (g>0)) {
	g = 3;
    }
    else {
	g = 4;
    }
};

xyz(a2, a3) -> {
   abc(a2, a3);
;};
