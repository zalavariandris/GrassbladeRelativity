#pragma once
#include <vector>

/* Animation */
namespace Animation {
	struct Key {
		bool selected{ false };
		int frame;
		double value;
		Key(): frame(0), value(0){};
		Key(int frame, double value) :frame(frame), value(value) {};
	};

	class AnimCurve {
	private:
		std::vector<Key> _keys;
		double lerp(double a, double b, double t) const {
			return a + t * (b - a);
		};

	public:
		std::string label{ "" };
		AnimCurve();
		AnimCurve(std::vector<Key> keys, std::string label = "");

		void setKeys(std::vector<Key> keys);

		double getValueAtFrame(double frame) const;

		void insertKeyAtFrame(int frame);

		void setValueAtFrame(double value, int frame);

		int getKeyIndexAtFrame(int frame);

		bool hasKeyAtFrame(int frame);

		bool removeKeyAtFrame(int frame);

		std::vector<Key> & getKeys();
	};
}