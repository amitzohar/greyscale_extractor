#pragma once
#include <math.h>

class RunLength{
	int _start;
	int _end;
	int _label;

public:
	RunLength(){
	}
	RunLength(int start, int end){
		_start = start;
		_end = end;
	}

	~RunLength(){
	}

	void start(int s) { _start = s; }
	int  start()      { return _start; }

	void end(int e)   { _end = e; }
	int  end()        { return _end ; }

	void label(int l) { _label = l; }
	int  label()      { return _label; }

	int length()      { return abs(_end - _start); }
};

