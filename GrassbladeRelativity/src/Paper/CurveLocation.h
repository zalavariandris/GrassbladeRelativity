#pragma once
#include <memory>
#include "glm/glm.hpp"
#include <memory>
#include "Curve.h"
namespace Paper {
	class CurveLocation {
	private:
		bool mValid{ false };
		glm::vec2 mPoint;
		glm::vec2 mTangent;
		glm::vec2 mNormal;
		double mTime;
	public:

		CurveLocation();

		CurveLocation::CurveLocation(Curve const & curve, double time);

		glm::vec2 point() {
			return mPoint;
		}

		glm::vec2 tangent() {
			return mTangent;
		}

		glm::vec2 normal() {
			return mNormal;
		}

		double time() {
			return mTime;
		}

		operator bool() const {
			return mValid;
		}
	};
}
