#include "sc_point.h"

namespace StudyCorr
{
   inline float Point2D::norm() const
   {
       return sqrt(x * x + y * y);
   }

   inline float Point3D::norm() const
   {
       return sqrt(x * x + y * y + z * z);
   }

}//namespace StudyCorr
