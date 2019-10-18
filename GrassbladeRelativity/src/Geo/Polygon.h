#pragma once
#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp> // lerp vec2
#include <vector>

namespace Geo {
	class Polygon {
	private:
		std::vector<glm::vec2> mVertices;
		bool mClosed = false;
	public:
		Polygon() {};
		Polygon(std::vector<glm::vec2> vertices) : mVertices(vertices) {};

		std::vector<glm::vec2> & getVertices() {
			return mVertices;
		};

		const std::vector<glm::vec2> & getVertices() const {
			return mVertices;
		};

		const std::vector<glm::vec2> & getPoints() const {
			return mVertices;
		};

		double getLength() const{
			double length = 0;
			auto vertices = getVertices();
			for (auto i=0; i < vertices.size()-1; i++) {
				length+=glm::distance(vertices[i], vertices[i + 1]);
			}
			return length;
		};

		void closed(bool val) {
			mClosed = val;
		};

		bool closed() const{
			return mClosed;
		};

		glm::vec2 getPointAtTime(double t) const{
			if (mVertices.size() < 2) return glm::vec2();
			double findex = t * mVertices.size();
			int idx = floor(findex);
			if (idx == mVertices.size() - 1) {
				return mVertices[mVertices.size() - 1];
			}
			return glm::mix(mVertices[idx], mVertices[idx + 1], findex - idx);
		}

		glm::vec2 getPointAt(double offset) const {
			// handle negative offset like path does
			if (offset < 0) {
				return glm::vec2(NAN);
			}
			double length = 0;
			for (auto i = 0; i < mVertices.size() - 1; i++) {
				double d = glm::distance(mVertices[i], mVertices[i + 1]);
				length += d;
				if (length > offset) {
					double a = (length - offset) / d;
					return glm::mix(mVertices[i+1], mVertices[i], a);
				}
			}
			// offset > length
			return glm::vec2(NAN);
		};

		int getNearestIndex(glm::vec2 point) const {
			auto idx = -1;
			double distance = std::numeric_limits<double>::infinity();
			for (auto i = 0; i < mVertices.size(); i++) {
				auto V = mVertices[i] - point;
				double d = glm::dot(V, V);
				if (d < distance) {
					distance = d;
					idx = i;
				}
			}
			return idx;
		};

		glm::vec2 getNormalAtIndex(int i) const{
			if (mVertices.size() <= 1) return glm::vec2();
			bool isFirst = i == 0;
			bool isLast = i == mVertices.size() - 1;
			if (isFirst) {
				auto tangent = mVertices[i+1] - mVertices[i];
				return glm::normalize(glm::vec2(tangent.y, -tangent.x));
			}
			if (isLast) {
				auto tangent = mVertices[i] - mVertices[i - 1];
				return glm::normalize(glm::vec2(tangent.y, -tangent.x));
			}

			auto tangent = glm::mix(
				glm::normalize(mVertices[i + 1] - mVertices[i]),
				glm::normalize(mVertices[i] - mVertices[i-1]),
				0.5);

			return glm::vec2(tangent.y, -tangent.x);
		}
	};
}