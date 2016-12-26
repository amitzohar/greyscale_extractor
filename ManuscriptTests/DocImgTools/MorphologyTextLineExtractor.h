
#ifndef MORPHOLOGYTEXTLINEEXTRACTOR_H_
#define MORPHOLOGYTEXTLINEEXTRACTOR_H_

#include "TextLineExtractor.h"
#include "TextLine.h"
#include "ManuscriptExport.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Morphology Text line extractor is a class that implements a morphology-based text
///             line extraction algorithm  </summary>
///
/// <remarks>	Inna Kr, 16/08/2015. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

class MANUSCRIPT_EXPORT MorphologyTextLineExtractor : public TextLineExtractor{

    private:
	Mat convertMat();

	public:

	MorphologyTextLineExtractor();
	~MorphologyTextLineExtractor();

	void extract(vector<TextLine*>& );

};

#endif /* MORPHOLOGYTEXTLINEEXTRACTOR_H_ */
