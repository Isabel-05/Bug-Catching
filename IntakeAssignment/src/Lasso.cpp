#include "Lasso.h"
#include "Util.h"
#include <string>
#include <iostream>

constexpr float linewidth = 20.0f;
constexpr float LASSOLENIENCY = 165.0f;

const std::string LINETEXTUREPATH = "./Images/Line.bmp";


void Lasso::init(SDL_Renderer* renderer)
{
	const char* filename = LINETEXTUREPATH.c_str();

	SDL_Surface* imageSf = SDL_LoadBMP(filename);

	if (!imageSf)
	{
		SDL_Log("Failed to load image: %s (%s)", filename, SDL_GetError());
		return;
	}

	lineTexture = SDL_CreateTextureFromSurface(renderer, imageSf);

	if (!lineTexture) {
		SDL_Log("Failed to create texture from: %s (%s)", filename, SDL_GetError());
	}

	SDL_DestroySurface(imageSf);
}

Lasso::~Lasso()
{
	SDL_DestroyTexture(lineTexture);
}

void Lasso::drawLasso(SDL_Renderer* renderer)
{
	if (!lineTexture || vertices.size() < 0) { return; }

	SDL_RenderGeometry(renderer, lineTexture,
		vertices.data(), static_cast<int>(vertices.size()),
		indices.data(), static_cast<int>(indices.size()));
}

void Lasso::addPoint(SDL_FPoint& point)
{
	if (lassoPoints.empty())
	{
		lassoPoints.push_back(point);
		return;
	}

	//denoise the amount of samples
	if(Util::pointsWithinRadius(point, lassoPoints.back(), 10.0f)) { return; }

	//build the quad (vertices and indices)
	int vertexCount = static_cast<int>(vertices.size());

	glm::vec2 p1{ lassoPoints.back().x, lassoPoints.back().y };
	glm::vec2 p2{ point.x, point.y };
	glm::mat2 rotation{ 0, -1, 1, 0 };

	glm::vec2 direction = p1 - p2;
	glm::vec2 perpVec = glm::normalize(direction * rotation) * linewidth;
	
	SDL_FColor white = { 1.0f, 1.0f, 1.0f, 1.0f };

	if (lassoPoints.size() > 2)
	{
		vertices.at(vertices.size() - 1).tex_coord.x = 0.4f;
		vertices.at(vertices.size() - 2).tex_coord.x = 0.4f;
		vertices.at(vertices.size() - 3).tex_coord.x = 1.0f;
		vertices.at(vertices.size() - 4).tex_coord.x = 1.0f;

		SDL_Vertex v1 = vertices.at(vertices.size() - 2);
		SDL_Vertex v2 = vertices.at(vertices.size() - 1);
		v1.tex_coord.x = 0.2f;
		v1.tex_coord.y = 0.0f;
		v2.tex_coord.x = 0.2f;
		v2.tex_coord.y = 1.0f;
		vertices.push_back(v1);
		vertices.push_back(v2);
		vertices.push_back({ {p2.x + perpVec.x, p2.y + perpVec.y}, white, {0.0f, 0.0f} }); //top right
		vertices.push_back({ {p2.x - perpVec.x, p2.y - perpVec.y}, white, {0.0f, 1.0f} }); //bottom right
	}
	else
	{
		//first quad needs four calculations + modified UVs so it samples the rounded part of the texture
		vertices.push_back({ {p1.x + perpVec.x, p1.y + perpVec.y}, white, {0.0f, 0.0f} }); //top left
		vertices.push_back({ {p1.x - perpVec.x, p1.y - perpVec.y}, white, {0.0f, 1.0f} }); //bottom left
		vertices.push_back({ {p2.x + perpVec.x, p2.y + perpVec.y}, white, {0.27f, 0.0f} }); //top right
		vertices.push_back({ {p2.x - perpVec.x, p2.y - perpVec.y}, white, {0.27f, 1.0f} }); //bottom right
	}

	//triangle 1
	indices.push_back(vertexCount);
	indices.push_back(vertexCount + 1);
	indices.push_back(vertexCount + 2);
	//triangle 2
	indices.push_back(vertexCount + 2);
	indices.push_back(vertexCount + 3);
	indices.push_back(vertexCount + 1);


	lassoPoints.push_back(point);
}

void Lasso::clearPoints()
{
	lassoPoints.clear();
	vertices.clear();
	indices.clear();
}


bool Lasso::isPointInLasso(const glm::vec2 inPoint)
{
	//This is my own implementation of the Point-in-Poly algorithm by WR Franklin
	//Source: https://wrfranklin.org/Research/Short_Notes/pnpoly.html

	size_t pointCount = lassoPoints.size();

	if (pointCount < 3) { return false; }

	bool isInside = false;

	for (size_t i = 0; i < pointCount; i++)
	{
		//get next edge
		SDL_FPoint edgeStart = lassoPoints.at(i);
		SDL_FPoint edgeEnd = lassoPoints.at((i + 1) % pointCount); //modulo so its circular

		//check if inPoint.y is in between the two points y (aka ray intersects the edge)
		bool edgeCrossesHeight = (edgeStart.y > inPoint.y) != (edgeEnd.y > inPoint.y);
		if (!edgeCrossesHeight) { continue; }

		//find value between 0 and 1 where the inPoint.y crosses the edge
		//also this never divides by zero since edgeCrossesHeight has to be true to run into this (which cant be true if theyre equal)
		float scalingFactor = (inPoint.y - edgeStart.y) / (edgeEnd.y - edgeStart.y);
		//Find the x position based on that scalar
		float crossingX = edgeStart.x + scalingFactor * (edgeEnd.x - edgeStart.x);

		//check if the point on the edge is to the right of the inPoint, if so flip the bool
		//this ensures that if the polygon is drawn next to the bug it hits this an even number of times and returns false
		//so it essentially only returns true if hits only one edge (aka is inside the polygon)
		if (inPoint.x < crossingX) {
			isInside = !isInside;
		}
	}

	return isInside;
}

bool Lasso::isClosed()
{
	if (lassoPoints.size() < 3) { return false; }

	//if the the first and last point are within a certain radius of each other, the lasso counts as closed
	bool closeEnough = Util::pointsWithinRadius(lassoPoints.front(), lassoPoints.back(), LASSOLENIENCY);
	if(closeEnough) { return true; }

	//if the to points don't, the line might have crossed itself which also counts as closed
	//algorithm source: https://bryceboe.com/2006/10/23/line-segment-intersection-algorithm/ 

	size_t len = lassoPoints.size();

	//pincer style loop since the intersection usually happens near the far ends
	//a small optimization for this brute force, but the amount of sample points is denoised so it shouldnt be too bad
	for (size_t i = 0; i < len; i++)
	{
		SDL_FPoint a1 = lassoPoints.at(i);
		SDL_FPoint a2 = lassoPoints.at((i + 1) % len);

		for (size_t j = len - 1; j > i; j--)
		{
			SDL_FPoint b1 = lassoPoints.at(j);
			SDL_FPoint b2 = lassoPoints.at(j - 1);
			bool intersects = (Util::ccw(a1, b1, b2) != Util::ccw(a2, b1, b2)) &&
				(Util::ccw(a1, a2, b1) != Util::ccw(a1, a2, b2));

			if (intersects) {
				return true; 
			}
		}
	}

	return false;
}