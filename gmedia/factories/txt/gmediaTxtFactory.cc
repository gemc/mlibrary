// txt gmedia
#include "gmediaTxtFactory.h"

void GMediaTxtFactory::writeObservables(GObservables* observableHitVarAtIndex, int index)
{
	switch(observableHitVarAtIndex->getTypeAtIndex(index)) {
		case gint_t:
			*ofile << observableHitVarAtIndex->getIntVarAtIndex(index) << " ";
			break;
		case gfloat_t:
			*ofile << observableHitVarAtIndex->getFloatVarAtIndex(index) << " ";
			break;
		case gdouble_t:
			*ofile << observableHitVarAtIndex->getDoubleVarAtIndex(index) << " ";
			break;
		case gstring_t:
			*ofile << observableHitVarAtIndex->getIntVarAtIndex(index) << " ";
			break;
	}
}
