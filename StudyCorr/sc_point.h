/*
 * This file is part of StudyCorr, based on OpenCorr functionality.
 * Compatible with both OpenCV and Qt point types.
 */

#pragma once

#ifndef _POINT_H_
#define _POINT_H_

#include <iostream>
#include <opencv2/core.hpp>

 // Include Qt headers if Qt is available
#ifdef QT_VERSION
#include <QPoint>
#include <QPointF>
#endif

namespace StudyCorr
{
	// Point in 2D plane
	class Point2D : public cv::Point2f
	{
	public:

		inline Point2D() : cv::Point2f(0.f, 0.f) {}

		inline Point2D(float x, float y) : cv::Point2f(x, y) {}

		inline Point2D(int x, int y) : cv::Point2f((float)x, (float)y) {}

		inline Point2D(const cv::Point2f& point) : cv::Point2f(point) {}

		inline Point2D(const cv::Point& point) : cv::Point2f((float)point.x, (float)point.y) {}

		// Qt compatibility constructors
#ifdef QT_VERSION
		inline Point2D(const QPointF& point) : cv::Point2f(point.x(), point.y()) {}

		inline Point2D(const QPoint& point) : cv::Point2f((float)point.x(), (float)point.y()) {}
#endif

		inline ~Point2D() {}

		// Returns Euclidean norm of the vector
		inline float norm() const;

		// Qt compatibility conversion operators
#ifdef QT_VERSION
		inline QPointF toQPointF() const
		{
			return QPointF(x, y);
		}

		inline QPoint toQPoint() const
		{
			return QPoint((int)x, (int)y);
		}

		inline operator QPointF() const
		{
			return QPointF(x, y);
		}

		inline operator QPoint() const
		{
			return QPoint((int)x, (int)y);
		}
#endif
	};

	// Point in 3D space
	class Point3D : public cv::Point3f
	{
	public:

		inline Point3D() : cv::Point3f(0.f, 0.f, 0.f) {}

		inline Point3D(float x, float y, float z) : cv::Point3f(x, y, z) {}

		inline Point3D(int x, int y, int z) : cv::Point3f((float)x, (float)y, (float)z) {}

		inline Point3D(const cv::Point3f& point) : cv::Point3f(point) {}

		inline Point3D(const cv::Point3i& point) :
			cv::Point3f((float)point.x, (float)point.y, (float)point.z) {}

		inline ~Point3D() {}

		// Returns Euclidean norm of the vector
		inline float norm() const;
	};

	// Overload the << operator for Point2D
	inline std::ostream& operator<<(std::ostream& output, const Point2D& point)
	{
		output << point.x << "," << point.y;
		return output;
	}

	// Overload the << operator for Point3D
	inline std::ostream& operator<<(std::ostream& output, const Point3D& point)
	{
		output << point.x << "," << point.y << "," << point.z;
		return output;
	}

	//reload basic operators for Point2D
	inline Point2D operator+(const Point2D& point, const Point2D& offset)
	{
		return Point2D(point.x + offset.x, point.y + offset.y);
	}

	inline Point2D operator-(const Point2D& point, const Point2D& offset)
	{
		return point + Point2D(-offset.x, -offset.y);
	}

	inline Point2D operator*(float factor, const Point2D& point)
	{
		return Point2D(factor * point.x, factor * point.y);
	}

	inline Point2D operator*(int factor, const Point2D& point)
	{
		return float(factor) * point;
	}

	inline Point2D operator*(const Point2D& point, float factor)
	{
		return factor * point;
	}

	inline Point2D operator*(const Point2D& point, int factor)
	{
		return float(factor) * point;
	}

	//dot product
	inline float operator*(const Point2D& point1, const Point2D& point2)
	{
		return (point1.x * point2.x + point1.y * point2.y);
	}

	inline Point2D operator/(const Point2D& point, float factor)
	{
		return Point2D(point.x / factor, point.y / factor);
	}

	inline Point2D operator/(const Point2D& point, int factor)
	{
		return point / float(factor);
	}

	//cross product (for 2D points, result is a scalar)
	inline float operator/(const Point2D& point1, const Point2D& point2)
	{
		return (point1.x * point2.y - point1.y * point2.x);
	}

	//reload basic operators for Point3D
	inline Point3D operator+(const Point3D& point, const Point3D& offset)
	{
		return Point3D(point.x + offset.x, point.y + offset.y, point.z + offset.z);
	}

	inline Point3D operator-(const Point3D& point, const Point3D& offset)
	{
		return point + Point3D(-offset.x, -offset.y, -offset.z);
	}

	inline Point3D operator*(float factor, const Point3D& point)
	{
		return Point3D(factor * point.x, factor * point.y, factor * point.z);
	}

	inline Point3D operator*(int factor, const Point3D& point)
	{
		return float(factor) * point;
	}

	inline Point3D operator*(const Point3D& point, float factor)
	{
		return factor * point;
	}

	inline Point3D operator*(const Point3D& point, int factor)
	{
		return float(factor) * point;
	}

	//dot product
	inline float operator*(const Point3D& point1, const Point3D& point2)
	{
		return (point1.x * point2.x + point1.y * point2.y + point1.z * point2.z);
	}

	inline Point3D operator/(const Point3D& point, float factor)
	{
		return Point3D(point.x / factor, point.y / factor, point.z / factor);
	}

	inline Point3D operator/(const Point3D& point, int factor)
	{
		return point / float(factor);
	}

	//cross product
	inline Point3D operator/(const Point3D& point1, const Point3D& point2)
	{
		return Point3D((point1.y * point2.z - point1.z * point2.y),
			(point1.z * point2.x - point1.x * point2.z),
			(point1.x * point2.y - point1.y * point2.x));
	}

	// Qt compatibility - additional conversion functions for enhanced interoperability
#ifdef QT_VERSION
// Convert QPointF to Point2D
	inline Point2D QPointFToPoint2D(const QPointF& qpoint)
	{
		return Point2D(qpoint.x(), qpoint.y());
	}

	// Convert QPoint to Point2D
	inline Point2D QPointFToPoint2D(const QPoint& qpoint)
	{
		return Point2D((float)qpoint.x(), (float)qpoint.y());
	}
#endif

}//namespace StudyCorr

#endif //_POINT_H_