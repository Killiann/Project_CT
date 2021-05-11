#pragma once
#include <SFML/Graphics.hpp>
#include <math.h>
#include <iostream>
#include <random>

namespace lin {

	//checking for point in rotated rectangles
	bool isPointToLeft(sf::Vector2f A, sf::Vector2f B, sf::Vector2f P);
	bool isPointToRight(sf::Vector2f A, sf::Vector2f B, sf::Vector2f P);

	bool doesRectContainPoint(sf::Vector2f P, sf::Vector2f A, sf::Vector2f B, sf::Vector2f C, sf::Vector2f D);
	bool doesConvexShapeContainPoint(sf::Vector2f P, sf::ConvexShape shape);
	
	/// <summary>
	/// Line Structure in the form Ax + By = C
	/// </summary>
	struct Line {
		sf::Vector2f p1;
		sf::Vector2f p2;
		Line() { A = B = C = 0.f; }
		Line(sf::Vector2f _p1, sf::Vector2f _p2) : p1(_p1), p2(_p2) {
			A = p2.y - p1.y;
			B = p1.x - p2.x;
			C = (A * p1.x) + (B * p1.y);
		}
		inline float GetLength() {
			return std::sqrt((float)std::pow(p1.x - p2.x, 2) + (float)std::pow(p1.y - p2.y, 2));
		}
		float A, B, C;
	};

	//euler const
	const float EULER = std::exp(1.f);
	
	//activation functions
	float act_sigmoid(float input);
	float act_leakyRelu(float input);
	float act_binary(float input);
	float act_tanh(float input);

	sf::Vector2f Normalise(const sf::Vector2f& source);

	//Matrices
	class Matrix {
		unsigned int rows = 0, cols = 0;
		std::vector<std::vector<float>> data;

	public:
		Matrix(){}
		Matrix(unsigned int r, unsigned int c);
		void Randomise(float min, float max);
		void Scale(float n);
		void Add(float n);
		void Add(Matrix m2);

		inline unsigned int GetRows() { return rows; }
		inline unsigned int GetCols() { return cols; }
		inline std::vector<std::vector<float>> GetData() { return data; }
		inline void SetData(std::vector<std::vector<float>> d) { data = d; }

		std::vector<float>& operator[](unsigned int i);
		Matrix operator*(float n);		//scale
		Matrix operator*(Matrix m2);	//multiplyMatrices
		Matrix operator+(float n);		//add float
		Matrix operator+(Matrix m2);	//add matrix
		void operator=(Matrix m);
		
		void Map(float (f)(float n));
		std::vector<float> ToVector();
	};

	Matrix ToMatrix(std::vector<float> v);
	Matrix MultiplyMatrices(Matrix m1, Matrix m2);
	void printMatrix(lin::Matrix m);
}
