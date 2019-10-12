#pragma once

#include "glm/glm.hpp"
#include <memory>
namespace Paper {
	class Curve;
	class Path;
	class Segment
	{
	public:
		Segment();

		Segment(glm::vec2 point);

		Segment(glm::vec2 point, glm::vec2 handleIn, glm::vec2 handleOut);

		// shoud be private, but friends with curve and path
		glm::vec2 point() const;
		void point(glm::vec2 val);

		glm::vec2 handleIn() const;
		void handleIn(glm::vec2 val);

		glm::vec2 handleOut() const;
		void handleOut(glm::vec2 val);

		int _index = NAN; //TODO shoud be private but friend with curve and path
	private:
		friend class Path;
		friend class Curve;
		const Path * _path{nullptr};
		glm::vec2 _point; // TODO: when set update path length
		glm::vec2 _handleIn;
		glm::vec2 _handleOut;
	};
}

