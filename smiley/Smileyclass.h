class smiley
{
	float radius, xc, yc;
	int npoints, happy;
	void set_numpoints();
	public:
	smiley(float r, float x, float y);
	void draw();
	void set_happy(int s){happy = s;}
	void resize(float r);
	float get_radius() {return radius;}
	float get_x() {return xc;}
	float get_y() {return yc;}
};

