#ifndef __FLOORFIELD_H__
#define __FLOORFIELD_H__

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <ctime>
#include "GL/freeglut.h"
#include "boost/assign/list_of.hpp"
#include "boost/optional.hpp"

#include "macro.h"
#include "container.h"
#include "drawingUtility.h"
#include "basicObj.h"

using std::cout;
using std::endl;

class FloorField {
public:
	array2i mDim;       // [0]: width, [1]: height
	array2f mCellSize;  // [0]: width, [1]: height
	arrayNf mCells;     // store the final floor field (use [y-coordinate * mDim[0] + x-coordinate] to access elements)
	arrayNf mCellsStatic, mCellsStatic_e, mCellsDynamic;
	std::vector<Exit> mExits;
	std::vector<Obstacle> mPool_o;
	arrayNi mActiveObstacles;
	float mLambda;
	float mCrowdAvoidance;
	float mKS, mKD, mKE;
	float mDiffuseProb, mDecayProb;
	float mMaxFF, mMaxSFF, mMaxSFF_e; // used for displaying mCells, mCellsStatic and mCellsStatic_e
	///
	int mFlgShowGrid;
	int mFFDisplayType;

	void read( const char *fileName );
	void save() const;
	void update( const std::vector<Agent> &pool, const arrayNi &agents, int type );
	void update_p( int type );
	///
	void print() const;
	void evaluateCells( int root, arrayNf &floorField ) const;

	/*
	 * Editing.
	 */
	boost::optional<array2i> isExisting_exit( const array2i &coord ) const;
	boost::optional<int> isExisting_obstacle( const array2i &coord, bool isMovable ) const;
	void editExit( const array2i &coord );
	void editObstacle( const array2i &coord, bool isMovable );
	int addObstacle( const array2i &coord, bool isMovable ); // push_back the return value to mActiveObstacles to actually add an obstacle
	void deleteObstacle( int i );

	/*
	 * Drawing.
	 */
	void draw() const;
	void drawExit( const array2i &pos ) const;
	void drawGrid() const;

private:
	std::vector<arrayNf> mCellsForExits;        // store the final floor field with respect to each exit
	std::vector<arrayNf> mCellsForExitsStatic;  // store the static floor field with respect to each exit
	std::vector<arrayNf> mCellsForExitsDynamic; // store the dynamic floor field with respect to each exit
	arrayNi mCellStates;                        // use [y-coordinate * mDim[0] + x-coordinate] to access elements

	void removeCells( int i );
	bool validateExitAdjacency( const array2i &coord, int &numNeighbors, bool &isRight, bool &isLeft, bool &isUp, bool &isDown ) const;
	void combineExits( const array2i &coord, int direction );
	void divideExit( const array2i &coord, int direction );
	void updateCellsStatic();
	void updateCellsStatic_p();
	void updateCellsDynamic( const std::vector<Agent> &pool, const arrayNi &agents );
	void updateCellsDynamic_p();
	void setCellStates();
	///
	inline int convertTo1D( int x, int y ) const { return y * mDim[0] + x; }
	inline int convertTo1D( const array2i &coord ) const { return coord[1] * mDim[0] + coord[0]; }

	/*
	 * The definitions are in floorField_tbb.cpp.
	 */
	void updateCellsStatic_tbb();
	void updateCellsDynamic_tbb( const std::vector<Agent> &pool, const arrayNi &agents );
};

#endif