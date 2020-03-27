/************************************
Name: Celine Fong (1580124) 
	  Claire Martin (1571140)
CMPUT 275, Winter Semester
Major Assignment 2, Part 1
************************************/

#include "wdigraph.h"
#include "dijkstra.h"
#include "serialport.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

struct Point {
long long lat; // latitude of the point
long long lon; // longitude of the point
};

long long manhattan(const Point& pt1, const Point& pt2) {
/* 
	Calculate the manhattan distance between two points

	PARAMETERS:
	pt1, pt2: Call-by-reference to two Point objects, each
	with a latitute/longitude that is used to calculate
	distance.
*/

	long long dist = abs(pt1.lat - pt2.lat) + abs(pt1.lon - pt2.lon);
	return dist;
}

void readGraph(string filename, WDigraph& graph, unordered_map<int, Point>& points) {
/*
	Read the Edmonton map data from the provided file
	and load it into the given WDigraph object.
	Store vertex coordinates in Point struct and map
	each vertex to its corresponding Point struct.
	PARAMETERS:
	filename: name of the file describing a road network
	graph: an instance of the weighted directed graph (WDigraph) class
	points: a mapping between vertex identifiers and their coordinates
*/
	// graph is already instantiated
	// initialize ifstream object
	ifstream file;

	// open the file
	file.open(filename);

	// initialize line that getline is reading
	string line;

	// declare variables
	int IDend, latEnd, lonEnd;
	int ID1end, ID2end;
	int ID, ID1, ID2;
	double latIn, lonIn;
	long long newLat, newLon;

	// check if file is opened correctly
	if (file.is_open()) {
		while (getline(file, line)) {
			if (line.find("V") == 0) {
				// the line specifies a vertex
				// find position of all commas
				IDend = line.find(',', 2);
				latEnd = line.find(',', IDend+1);
				lonEnd = line.length();

				// get ID as int
				ID = stoi(line.substr(2, IDend-2));

				// get lat and lon as double
				latIn = stod(line.substr(IDend+1, latEnd-IDend));
				lonIn = stod(line.substr(latEnd+1, lonEnd-latEnd));

				// store the coordinates to 100000th degree
				newLat = static_cast <long long> (latIn*100000);
				newLon = static_cast <long long> (lonIn*100000);

				// add vertex by ID to graph
				graph.addVertex(ID);

				// temporary Point struct to add to map
				Point tempPoint;
				tempPoint.lat = newLat;
				tempPoint.lon = newLon;

				// store Point struct in points map, paired w/ vertex ID
				pair<int, Point> tempPair (ID, tempPoint);
				points.insert(tempPair);
			} else if (line.find("E") == 0) {
				// the line specifies an edge
				// find position of all commas
				ID1end = line.find(',', 2);
				ID2end = line.find(',', ID1end+1);

				// get both vertex IDs as integers
				ID1 = stoi(line.substr(2, ID1end - 2));
				ID2 = stoi(line.substr(ID1end+1, ID2end - ID1end - 1));

				// add edge one direction only to make directed graph
				long long cost = manhattan(points.at(ID1), points.at(ID2));
				graph.addEdge(ID1, ID2, cost);
			}		
		}
	}

	// close file
	file.close();
}

int main() {
	// create SerialPort object to read and write to serial
	SerialPort Serial("/dev/ttyACM0");
	// finite state machine
	enum {WAIT_FOR_POINTS, SEND_POINTS} curr_mode = WAIT_FOR_POINTS;
	// instantiate weighted digraph
	WDigraph graph;
	unordered_map<int, Point> points;
	unordered_map<int, PIL> tree;
	// read and create the graph
	readGraph("edmonton-roads-2.0.1.txt", graph, points);

	// read in input from stdin
	// char startInput;
	// cin >> startInput; // should be R
	// Point start;
	// cin >> start.lat >> start.lon; // read in first point
	// Point end;
	// cin >> end.lat >> end.lon; // read in second point

	// NEW: Read in input from SERIAL
	string input;
	string startInput;
	// no timeout, just wait to read the line
	while(true) {
		if (curr_mode == WAIT_FOR_POINTS) {
			// keep reading a line until you read R
			input = Serial.readline();
			startInput = input.substr(0,1); // should be R
			if (startInput == 'R') {
				curr_mode = SEND_POINTS;
			}
		} else {
			// only gets here if curr_mode = SEND_POINTS
			// now get lat and lon of start and end
			int lat1Start, lat1End, lon1Start, lon1End;
			int lat2Start, lat2End, lon2Start, lon2End;
			long long lat1, lat2, lon1, lon2;
			// find first space, this is where lat1 starts
			lat1End = input.find(' ',2);
			lat1 = stod(input.substr(2, lat1End - 2));
			lon1End = input.find(' ', lat1End + 1);
			lon1 = stod(input.substr(lat1End + 1, lon1End - lat1End - 1));
			lat2End = input.find(' ', lon1End + 1);
			lat2 = stod(input.substr(lon1End + 1, lat2End - lon1End - 1));
			// IGNORE \N AT END?? CONFUSED SHOULD IT BE -1 OR NOT
			lon2End = input.length() - 1;
			lon2 = stod(input.substr(lat2End + 1, lon2End - lat2End - 1));

			// add lat and lon to point structs
			Point start, end;
			start.lat = lat1;
			start.lon = lon1;
			end.lat = lat2;
			end.lon = lon2;

			long long shortestToStart;
			long long shortestToEnd;
			int startID, endID;

			// find closest vertex to start point
			for (auto iter = points.begin(); iter != points.end(); ++iter) {
				long long distStart = manhattan(start, iter->second);
				if (iter == points.begin()) {
					shortestToStart = distStart;
				}
				if (distStart < shortestToStart) {
					startID = iter->first;
					shortestToStart = distStart;
				}
			}
			// find closest vertex to end point
			for (auto iter = points.begin(); iter != points.end(); ++iter) {
				long long distEnd = manhattan(end, iter->second);
				if (iter == points.begin()) {
					shortestToEnd = distEnd;
				}
				if (distEnd < shortestToEnd) {
					endID = iter->first;
					shortestToEnd = distEnd;
				}
			}

			// run dijkstra's from start vertex
			dijkstra(graph, startID, tree);

			// initialize vector to keep track of waypoints
			vector <Point> waypoints;

			// initialize node count
			int nodeCount = 1;

			// start tracking back waypoints from the endpoint
			int ID = endID;

			// keep reading waypoint IDs until we reach the start point
			while (ID != startID) {
				// add new waypoint to the vector
				waypoints.push_back(points[ID]);
				ID = tree[ID].first;
				nodeCount++;
			}

			// add start point to waypoint vector
			// change nodeCount to string
			string nodeCountStr = to_string(nodeCount);
			waypoints.push_back(points[startID]);
			Serial.writeline("N ");
			Serial.writeline(nodeCountStr);
			Serial.writeline("\n"); // need to write newline

			// iterate backwards through vector to get points from start to end
			int i = nodeCount - 1;
			while (curr_mode == SEND_POINTS && i >= 0) {
				// read in acknowledgment from client
				// timeout 1s
				input = Serial.readline(1000); // should be A
				// returns "" if timeout
				if (input.compare("") == 0) {
					// timeout, A not received, go back to waiting
					curr_mode = WAIT_FOR_POINTS;
				} else if (input.find("A") == 0) {
					// A received, send next waypoint
					Point currPoint = waypoints[i];
					string latStr, lonStr;
					latStr = to_string(currPoint.lat);
					lonStr = to_string(currPoint.lon);
					// print waypoint to stdout
					Serial.writeline("W ");
					Serial.writeline(latStr);
					Serial.writeline(" ");
					Serial.writeline(lonStr);
					Serial.writeline("\n");
				} else {
					// something else received, go back to waitin
					curr_mode = WAIT_FOR_POINTS;
				}
				i--;
			}
			if (curr_mode == SEND_POINTS) {
				// program finish
				Serial.writeline("E");
				Serial.writeline("\n");
			}
			// go back to waiting for points when all done
			curr_mode = WAIT_FOR_POINTS;
		}
	}
	return 0;
}