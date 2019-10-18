#pragma once

#include "glm/glm.hpp"
#include <memory>
namespace Paper {
	class Curve;
	class Path;
	class Segment
	{
	private:
		friend class Path;
		friend class Curve;

		Path * mPath{ nullptr };
		int mIndex = NAN;
		glm::vec2 mPoint; // TODO: when set update path length
		glm::vec2 mHandleIn;
		glm::vec2 mHandleOut;

	public:
		//constrctors
		Segment();
		Segment(glm::vec2 point);
		Segment(glm::vec2 point, glm::vec2 handle);
		Segment(glm::vec2 point, glm::vec2 handleIn, glm::vec2 handleOut);

		//accessors
		glm::vec2 point() const;
		void point(glm::vec2 val);

		glm::vec2 handleIn() const;
		void handleIn(glm::vec2 val);

		glm::vec2 handleOut() const;
		void handleOut(glm::vec2 val);
	};
}

