/* 
 * File:   BinarizerKabir.h
 * Author: user
 *
 * Created on June 17, 2015, 10:43 AM
 */

#ifndef _BINARIZER_KABIR_H
#define	_BINARIZER_KABIR_H

#include "dll_export_import.h"

#include "ImageBinarizer.h"
#include "DImage.h"

class MANUSCRIPT_API BinarizerKabir : public ImageBinarizer{

    protected:
        int _strokeWidth;
        int _k;
        Mat _structuralContrast, _grayLevel , _meanStrokeWidth;
        Mat _featureSpace;
        Mat _binaryNiblack;
        Mat _labels;
        Mat _partitionBinary;
       
    public:
        BinarizerKabir();
        BinarizerKabir(const BinarizerKabir& orig);
        BinarizerKabir(int new_SW,int _K);
        DImage* binarize();
        virtual ~BinarizerKabir();
        void setStrokeWidth(int StrokeWidth);
        void setK(int K);

    private:
        void featureExtraction();
        void calculateMeanStrokeWidth();
        void calculateFeatureSpace();
        void partitionFeatureSpace();
        void runNiBlackAlgorithm();
        void partitionClassification(long partition_count[]);
        Mat pixelClassification(long* countPartition); 
        
        void printArea(long countPartition[]);
        void printAreaBinary(long countPartition[]);
        void printAreaBinaryNiblack(long countPartition[]);
};

#endif	/* BINARIZERKABIR_H */

