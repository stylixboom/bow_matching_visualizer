/*
 * bow_matching_visualizer.h
 *
 *  Created on: November 5, 2013
 *      Author: Siriwat Kasamwattanarote
 */
#include <unistd.h>     // sysconf
#include <sys/stat.h>   // file-directory existing
#include <sys/types.h>  // file-directory
#include <dirent.h>     // file-directory
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <tr1/unordered_map>
#include <bitset>
#include <math.h>

#include "opencv2/core/core.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "../lib/alphautils/alphautils.h"
#include "../lib/alphautils/imtools.h"

using namespace std;
using namespace tr1;
using namespace cv;
using namespace alphautils;
using namespace alphautils::imtools;

typedef struct _match_object{ size_t cluster_id; float weight; SIFT_Keypoint left_keypoint; SIFT_Keypoint right_keypoint; } match_object;
vector<match_object> matches;
bool has_dataset_path;
bool has_query_path;
string dataset_path;
string query_path;
Size query_size;
string bow_matches_path;
size_t feature_counter = 0;

void load_match_file(const string& bow_matches_path);
void draw_matches(const string& output_image_path, bool normsift = true);
void draw_back_project(Mat& draw_space, bool normsift = true);

//;)
