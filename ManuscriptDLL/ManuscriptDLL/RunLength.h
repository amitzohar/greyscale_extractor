#pragma once
#include <math.h>

#include "dll_export_import.h"

class MANUSCRIPT_API RunLength{
	int _start;
	int _end;
	int _label;

public:
	RunLength(){
		_start = 0 ;
		_end   = 0 ;
		_label = 0;
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

