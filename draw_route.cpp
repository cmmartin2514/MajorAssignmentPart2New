#include "draw_route.h"
#include "map_drawing.h"

extern shared_vars shared;

void draw_route() {
	shared.waypoints[0].lat = 5364188;
	shared.waypoints[0].lon = -11348914;
	shared.waypoints[1].lat = 5363838;
	shared.waypoints[1].lon = -11349423;
	shared.waypoints[2].lat = 5363757;
	shared.waypoints[2].lon = -11349634;
	shared.waypoints[3].lat = 5363635;
	shared.waypoints[3].lon = -11350026;
	shared.waypoints[4].lat = 5363389;
	shared.waypoints[4].lon = -11350776;
	shared.waypoints[5].lat = 5363374;
	shared.waypoints[5].lon = -11350815;
	shared.waypoints[6].lat = 5363356;
	shared.waypoints[6].lon = -11350849;
	shared.waypoints[7].lat = 5363327;
	shared.waypoints[7].lon = -11350890;
	shared.num_waypoints = 8;

	// implement this!
	if (shared.num_waypoints > 0 && shared.num_waypoints <= max_waypoints) {
		// make sure num waypoints is in range, may not need this depending on other functions
		// if the other functions already check this I don't need to
		for (int i = 0; i < shared.num_waypoints - 1; i++) {
			// for each edge, need to check if it is within the bounds of the screen
			// get a waypoint
			lon_lat_32 currWaypoint = shared.waypoints[i];
			// get the next waypoint to connect to
			// assuming waypoints are in order?
			lon_lat_32 nextWaypoint = shared.waypoints[i+1];

			int32_t xFirst = longitude_to_x(shared.map_number, currWaypoint.lon);
			int32_t yFirst = latitude_to_y(shared.map_number, currWaypoint.lat);
			int32_t xNext = longitude_to_x(shared.map_number, nextWaypoint.lon);
			int32_t yNext = latitude_to_y(shared.map_number, nextWaypoint.lat);
			Serial.println(xFirst);
			Serial.println(yFirst);
			Serial.println(xNext);
			Serial.println(yNext);

			// make sure drawings are in range
			// if (xFirst < 0) {
			// 	xFirst = 0;
			// } else if (xFirst > displayconsts::display_width) {
			// 	xFirst = displayconsts::display_width;
			// }
			// if (yFirst < 0) {
			// 	yFirst = 0;
			// } else if (yFirst > displayconsts::display_height) {
			// 	yFirst = displayconsts::display_width;
			// }
			// if (xNext < 0) {
			// 	xNext = 0;
			// } else if (xNext > displayconsts::display_width) {
			// 	xNext = displayconsts::display_width;
			// }
			// if (yNext < 0) {
			// 	yNext = 0;
			// } else if (yNext > displayconsts::display_height) {
			// 	yNext = displayconsts::display_height;
			// }

			// draw line if you can
			// if (xFirst != xNext || yFirst != yNext) {
				// if line is completely out of range they will all =0, so don't try drawing
				shared.tft->drawLine(xFirst, yFirst, xNext, yNext, TFT_BLACK);
			//}
		}
	}
  // implement this!
}
