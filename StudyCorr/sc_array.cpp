#include "sc_array.h"

namespace StudyCorr
{
    Array2D new2D(int dimension1, int dimension2)
    {
        return Array2D(dimension1, std::vector<float>(dimension2, 0.0f));
    }

    Array3D new3D(int dimension1, int dimension2, int dimension3)
    {
        return Array3D(dimension1, std::vector<std::vector<float>>(dimension2, std::vector<float>(dimension3, 0.0f)));
    }

    Array4D new4D(int dimension1, int dimension2, int dimension3, int dimension4)
    {
        return Array4D(dimension1, std::vector<std::vector<std::vector<float>>>(dimension2, std::vector<std::vector<float>>(dimension3, std::vector<float>(dimension4, 0.0f))));
    }

}