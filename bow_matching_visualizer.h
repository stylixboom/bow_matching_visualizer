/*
 * bow_matching_visualizer.h
 *
 *  Created on: November 5, 2013
 *      Author: Siriwat Kasamwattanarote
 */
#pragma once
#include "../lib/ins/kp_dumper.h"

using namespace ins;

string out_dir; // Output directory for visualized image

void visualize(const string& query_dump_path, const int query_idx, const int query_sequence_id, const string& query_sequence_filename,
               const string& dataset_dump_path, const int dataset_idx, const int dataset_sequence_id, const string& dataset_sequence_filename,
               bool back_projection, bool normsift);
void draw_query_backprojection(kp_dumper& query_dump, kp_dumper& dataset_dump, bool normsift);
void draw_matches(vector<dump_object>& query_singledump, Mat& query_image, vector<dump_object>& dataset_singledump, Mat& dataset_image, Mat& out_image, bool normsift);
void draw_link(vector<dump_object>& query_singledump, Mat& query_image, vector<dump_object>& dataset_singledump, Mat& dataset_image, Mat& out_image, bool normsift);
void draw_kp(vector<dump_object>& kp_singledump,  Mat& draw_image, bool normsift);
//;)
