#pragma once
#include "../Project.h"
#include <iostream>

#include "imgui.h"
#include "../Im2D/Im2D.h"
#include "../Paper/Path.h"
#include "../Im2DPaper/Im2DPaper.h"
#include "../Im2D/im2d_draw.h"

#include <opencv2/opencv.hpp>
#include <opencv2/shape/shape_transformer.hpp>
#include <vector>
class ThinGrass : Project {
	Paper::Path sourcePath;
	Paper::Path targetPath;
	cv::Mat mat;
	cv::Ptr<cv::ThinPlateSplineShapeTransformer> tps;
public:
	ThinGrass() {
		mat = cv::imread("C:/Users/andris/Pictures/2019-08/IMG_6918.JPG");
		assert(mat.data != NULL);
		cv::resize(mat, mat, cv::Size(500, 500));
		

		sourcePath.add({
			Paper::Segment(glm::vec2(250,0),glm::vec2(0, 0), glm::vec2(20, 175)),
			Paper::Segment(glm::vec2(250, 500), glm::vec2(20, -175), glm::vec2(0, 0))
			});

		targetPath.add({
			Paper::Segment(glm::vec2(250,0),glm::vec2(0, 0), glm::vec2(20, 175)),
			Paper::Segment(glm::vec2(250, 500), glm::vec2(20, -175), glm::vec2(0, 0))
			});


		tps = cv::createThinPlateSplineShapeTransformer(25000);
	};

	~ThinGrass() {

	}
	
	void tick() {
		static int count = { 10 };
		ImGui::Begin("Inspector");
		ImGui::SliderInt("count", &count, 2, 100);
		ImGui::End();

		std::vector<cv::Point> sourcePoints;
		for (auto i = 0; i < count; i++) {
			double t = (double)i / count;
			glm::vec2 P = sourcePath.getPointAtTime(t);
			sourcePoints.push_back(cv::Point(P.x, P.y));
		}

		std::vector<cv::Point> targetPoints;
		for (auto i = 0; i < count; i++) {
			double t = (double)i / count;
			glm::vec2 P = targetPath.getPointAtTime(t);
			targetPoints.push_back(cv::Point(P.x, P.y));
		}

		std::vector<cv::DMatch> matches;
		for (auto i = 0; i < count; i++) {
			matches.push_back(cv::DMatch(i, i, 0.0));
		}
		tps->estimateTransformation(targetPoints, sourcePoints, matches);
		cv::Mat res;
		tps->warpImage(mat, res);


		ImGui::Begin("Viewer");
		Im2D::BeginViewer("viewer");
		static ofTexture tex;
		tex.loadData(res.data, res.cols, res.rows, GL_RGB);
		Im2D::Image(glm::vec2(0, 0), (void*)tex.getTextureData().textureID, ImVec2(tex.getWidth(), tex.getHeight()));
		//Im2D::DragPath("source", &sourcePath);
		for (auto point : sourcePoints) {
			addPoint({ point.x, point.y }, ImColor(0, 255, 0), 2);
		}
		for (auto point : targetPoints) {
			addPoint({ point.x, point.y }, ImColor(0, 0, 255), 2);
		}
		Im2D::DragPath("target", &targetPath);
		Im2D::EndViewer();
		ImGui::End();
	}
};