#pragma once

#include "AnimCurve.h"
#include <vector>
#include <algorithm>

/* Animation */
namespace Animation {
	AnimCurve::AnimCurve() {};

	AnimCurve::AnimCurve(std::vector<Key> keys, std::string label)
		:label(label)
	{
		setKeys(keys);
	};

	void AnimCurve::setKeys(std::vector<Key> keys) {
		_keys = keys;
		std::sort(_keys.begin(), _keys.end(), [](auto A, auto B) {
			return A.frame < B.frame;
		});
	}

	double AnimCurve::getValueAtFrame(double frame) const {
		if (_keys.empty())
			return 0.0;

		if (frame <= _keys[0].frame)
			return _keys[0].value;

		if (frame >= _keys[_keys.size() - 1].frame)
			return _keys[_keys.size() - 1].value;

		// find embace keys
		double i1 = 0;
		while (_keys[i1].frame <= frame)
			i1++;
		int i0 = i1 - 1;

		// extract values fro lerp
		double v0 = _keys[i0].value;
		double v1 = _keys[i1].value;
		int f0 = _keys[i0].frame;
		int f1 = _keys[i1].frame;
		double t = (frame - f0) / (f1 - f0);
		return lerp(v0, v1, t);
	}

	void AnimCurve::insertKeyAtFrame(int frame) {
		double val = getValueAtFrame(frame);
		setValueAtFrame(val, frame);
	}

	void AnimCurve::setValueAtFrame(double value, int frame) {
		if (_keys.empty())
			_keys.push_back(Key(frame, value));

		if (frame < _keys[0].frame)
			_keys.insert(_keys.begin(), 1, Key(frame, value));

		if (frame > _keys[_keys.size() - 1].frame)
			_keys.push_back(Key(frame, value));

		// find key at frame
		auto i = -1;
		for (int i = 0; i < _keys.size(); i++)
			if (getKeys()[i].frame == frame) {
				_keys[i].value = value;
				return;
			}

		// find embace keys
		int i1 = 0;
		while (_keys[i1].frame <= frame)
			i1++;
		int i0 = i1 - 1;

		_keys.insert(_keys.begin() + i1, 1, Key(frame, value));
	}

	int AnimCurve::getKeyIndexAtFrame(int frame) {
		for (int i = 0; i < _keys.size(); i++) {
			if (getKeys()[i].frame == frame) {
				return i;
			}
		}
		return -1;
	}

	bool AnimCurve::hasKeyAtFrame(int frame) {
		return getKeyIndexAtFrame(frame) >= 0;
	}

	bool AnimCurve::removeKeyAtFrame(int frame) {
		auto idx = getKeyIndexAtFrame(frame);
		if (idx < 0)
			return false;

		_keys.erase(_keys.begin() + idx);
		return true;
	}

	std::vector<Key> & AnimCurve::getKeys() {
		return _keys;
	}
	};