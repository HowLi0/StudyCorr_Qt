#pragma once
#include <opencv2/opencv.hpp>

namespace StudyCorr 
{
	// 2D 数组
	using Array2D = std::vector<std::vector<float>>;
	Array2D new2D(int dimension1, int dimension2);

	// 3D 数组
	using Array3D = std::vector<std::vector<std::vector<float>>>;
	Array3D new3D(int dimension1, int dimension2, int dimension3);

	// 4D 数组
	using Array4D = std::vector<std::vector<std::vector<std::vector<float>>>>;
	Array4D new4D(int dimension1, int dimension2, int dimension3, int dimension4);
}

