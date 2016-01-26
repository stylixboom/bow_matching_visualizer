/*
 * bow_matching_visualizer.cpp
 *
 *  Created on: November 5, 2013
 *      Author: Siriwat Kasamwattanarote
 */
#include <unistd.h>     // sysconf
#include <sys/stat.h>   // file-directory existing
#include <sys/types.h>  // file-directory
#include <dirent.h>     // file-directory
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <map>
#include <bitset>
#include <cmath>

#include "opencv2/core/core.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"

// Siriwat's header
#include "../lib/alphautils/alphautils.h"
#include "../lib/alphautils/imtools.h"
#include "../lib/ins/kp_dumper.h"

#include "bow_matching_visualizer.h"

using namespace std;
using namespace cv;
using namespace alphautils;
using namespace alphautils::imtools;
using namespace ins;

int main(int argc,char *argv[])
{
    string query_dump_path = "";
    int query_idx = -1;
    int query_sequence_id = -1;
    string query_sequence_filename = "";
    string dataset_dump_path = "";
    int dataset_idx = -1;
    int dataset_sequence_id = -1;
    string dataset_sequence_filename = "";
    bool backproject = false;
    bool normsift = true;

    // App Options
    unordered_map<string, string> opts;

    if (argc > 1) // Explicitly run
    {
        // Grab all params
        for (int param_idx = 1; param_idx < argc; param_idx += 2)
            opts[string(argv[param_idx])] = string(argv[param_idx + 1]);

        /// Back projection check
        if (opts.find("backproject") != opts.end() && opts["backproject"] == "y")
        {
            if (opts.find("q") == opts.end() && opts.find("d") == opts.end())
            {
                cout << "Please specified dataset dump and query dump for drawing a back-projection." << endl;
                exit(0);
            }
            backproject = true;
        }

        /// Dump file availability check
        if (opts.find("q") != opts.end() || opts.find("d") != opts.end())
        {
            if (opts.find("q") != opts.end())   // Query dump path
            {
                query_dump_path = opts["q"];
                if (!is_path_exist(query_dump_path))
                {
                    cout << "Query dump path: " << query_dump_path << " does not exit!" << endl;
                    exit(0);
                }

                // Specific image index to be draw
                if (opts.find("qid") != opts.end())
                {
                    vector<string> idx;
                    StringExplode(opts["qid"], ":", idx);
                    // idx1 only number
                    query_idx = atoi(idx[0].c_str());
                    // idx2 can be number and filename
                    if (idx[1][0] == 'i')       // Check 's' sequence_id as an input
                        query_sequence_id = atoi(str_replace_first(idx[1], "i-", "").c_str());
                    else if (idx[1][0] == 'f')  // Check filename as an input
                        query_sequence_filename = str_replace_first(idx[1], "f-", "");
                }
                else
                {
                    query_idx = -1;
                    query_sequence_id = -1;
                }
            }
            if (opts.find("d") != opts.end())   // Dataset dump path
            {
                dataset_dump_path = opts["d"];
                if (!is_path_exist(dataset_dump_path))
                {
                    cout << "Dataset dump path: " << dataset_dump_path << " does not exit!" << endl;
                    exit(0);
                }

                // Specific image index to be draw
                if (opts.find("did") != opts.end())
                {
                    vector<string> idx;
                    StringExplode(opts["did"], ":", idx);
                    // idx1 only number
                    dataset_idx = atoi(idx[0].c_str());
                    // idx2 can be number and filename
                    if (idx[1][0] == 'i')       // Check 's' sequence_id as an input
                        dataset_sequence_id = atoi(str_replace_first(idx[1], "i-", "").c_str());
                    else if (idx[1][0] == 'f')  // Check filename as an input
                        dataset_sequence_filename = str_replace_first(idx[1], "f-", "");
                }
                else
                {
                    dataset_idx = -1;
                    dataset_sequence_id = -1;
                }
            }
        }

        /// output directory
        if (opts.find("o") != opts.end())
            out_dir = opts["o"];
        else
            out_dir = string(getenv("PWD")) + "/visualize";

        /// Prepare out_dir directory
        make_dir_available(out_dir);

        /// normsift option
        if (opts.find("normsift") != opts.end())
        {
            if (opts["normsift"] == "n")
                normsift = false;
        }
    }
    else
    {
        cout << "Params must be \"bow_matching_visualizer q query_dump [qid idx:i-(sequence_id)|f-(frame_filename)] d dataset_dump [did idx:i-(sequence_id)|f-(frame_filename)] o out_directory backproject y|n normsift y|n\"" << endl;
		exit(1);
    }

    /// Start visualize
    visualize(query_dump_path, query_idx, query_sequence_id, query_sequence_filename,
              dataset_dump_path, dataset_idx, dataset_sequence_id, dataset_sequence_filename,
              backproject, normsift);

    /* Old
    if (opts.find("i") != opts.end())   // File matching
    {
        // Load match file
        cout << "Load match file..."; cout.flush();
        load_match_file(bow_matches_path);
        cout << "done!" << endl;

        // Draw matches
        cout << "Drawing..."; cout.flush();
        draw_matches(output_image_path, normsift);
        cout << "done!" << endl;

        // Release memory
        matches.clear();
    }
    else                                // Back projection
    {
        // Load match file
        vector<string> matchlists;
        DIR* dirp = opendir(bow_matches_path.c_str());
        dirent* dp;
        while ((dp = readdir(dirp)) != NULL)
        {
            //cout << static_cast<unsigned>(dp->d_type) << " " << static_cast<unsigned>(DT_REG) << endl;
            if (string::npos != string(dp->d_name).find(".matches") && string::npos == string(dp->d_name).find("_vis.png"))
                matchlists.push_back(string(dp->d_name));
        }
        closedir(dirp);
        cout << matchlists.size() << " .matches files were found." << endl;

        // Draw back projected images
        bool firstrun = false;
        Mat merged_space;

        cout << "Draw back projecting..";
        cout.flush();
        timespec startTime = CurrentPreciseTime();
        for (size_t match_id = 0; match_id < matchlists.size(); match_id++) // matchlists.size()
        {
            // Load match file
            //cout << "Load match file..."; cout.flush();
            load_match_file(bow_matches_path + "/" + matchlists[match_id]);
            //cout << "done!" << endl;

            // Initializing first run
            if (!firstrun)
            {
                query_size = get_image_size(query_path);
                merged_space = Mat(query_size.height, query_size.width, CV_8UC3, Scalar(0, 0, 0));
                //imread(query_path).convertTo(merged_space, CV_64FC3);
                firstrun = false;
            }

            // Draw matches
            //cout << "Drawing..."; cout.flush();
            draw_back_project(merged_space, normsift);
            //cout << "done!" << endl;

            // Release memory
            matches.clear();

            percentout(match_id, matchlists.size(), 1);
        }

        imwrite(bow_matches_path + "/back_projected.png", merged_space);

        cout << "done! (in " << setprecision(2) << fixed << TimeElapse(startTime) << " s)" << endl;
    }*/
}

void visualize(const string& query_dump_path, const int query_idx, const int query_sequence_id, const string& query_sequence_filename,
               const string& dataset_dump_path, const int dataset_idx, const int dataset_sequence_id, const string& dataset_sequence_filename,
               bool back_projection, bool normsift)
{
    /// Only query
    if (dataset_dump_path == "")
    {
        // Load dump file
        kp_dumper query_dump;
        query_dump.load(query_dump_path);

        // Draw range
        int idx = 0, sequence_id = 0;
        int total_idx = 0, total_sequenceid = 0;
        if (query_idx != -1)
        {
            // Check out of image
            if (size_t(query_idx) > query_dump._img_roots.size() - 1)
            {
                cout << "Query index: " << query_idx << " does not exist" << endl;
                exit(1);
            }
            if (query_sequence_id != -1 && size_t(query_sequence_id) > query_dump._img_filenames[query_idx].size() - 1)
            {
                cout << "Query sequence_id: " << query_sequence_id << " does not exist" << endl;
                exit(1);
            }
            idx = query_idx;
            if (query_sequence_id != -1)
                sequence_id = query_sequence_id;

            // Convert filename to sequence_id
            if (query_sequence_filename != "")
                sequence_id = query_dump.convert_imgfilename_to_sequenceid(idx, query_sequence_filename);

            // Init total to next index
            total_idx = idx + 1;
            total_sequenceid = sequence_id + 1;
        }
        else
        {
            total_idx = query_dump._img_roots.size();

            if (query_sequence_id != -1)
                total_sequenceid = query_sequence_id + 1;
        }

        // Draw
        for (; idx < total_idx; idx++)
        {
            // Reset sequence_id
            if (sequence_id == total_sequenceid)
                sequence_id = 0;

            if (query_sequence_id == -1 && query_sequence_filename == "")
                total_sequenceid = query_dump._img_filenames[idx].size();
            for (; sequence_id < total_sequenceid; sequence_id++)
            {
                string org_img_path = query_dump.get_full_imgpath(idx, sequence_id);
                string vis_img_path = out_dir + "/query." + ZeroPadString(toString(idx), 3) + "-" + ZeroPadString(toString(sequence_id), 3) +
                                "." + get_filename(org_img_path) + ".jpg";

                /// For query, this is important!!
                /// Please remove prefix/postfix of pre/post processing filename before use
                org_img_path = str_replace_all(org_img_path, ".qe", "");
                org_img_path = str_replace_all(org_img_path, ".qb_it0", "");

                Mat draw_img = imread(org_img_path);
                //Mat out_img = Mat(original_img.rows, original_img.cols, CV_8UC3, Scalar(0, 0, 0));
                vector<dump_object> single_filtered_dump;
                query_dump.get_singledump_with_filter(idx, sequence_id, single_filtered_dump);
                draw_kp(single_filtered_dump, draw_img, normsift);

                // Save image output
                imwrite(vis_img_path.c_str(), draw_img);

                // Write ok signal
                string vis_img_path_ok = vis_img_path + ".ok";
                text_write(vis_img_path_ok, ":)");

                // Visualize output to filename
                cout << get_filename(vis_img_path) << endl;
            }
        }

        return;
    }

    /// Only dataset
    if (query_dump_path == "")
    {
        // Load dump file
        kp_dumper dataset_dump;
        dataset_dump.load(dataset_dump_path);

        // Draw range
        int idx = 0, sequence_id = 0;
        int total_idx = 0, total_sequenceid = 0;
        if (dataset_idx != -1)
        {
            // Check out of image
            if (size_t(dataset_idx) > dataset_dump._img_roots.size() - 1)
            {
                cout << "Dataset index: " << dataset_idx << " does not exist" << endl;
                exit(1);
            }
            if (dataset_sequence_id != -1 && size_t(dataset_sequence_id) > dataset_dump._img_filenames[dataset_idx].size() - 1)
            {
                cout << "Dataset sequence_id: " << dataset_sequence_id << " does not exist" << endl;
                exit(1);
            }
            idx = dataset_idx;
            if (dataset_sequence_id != -1)
                sequence_id = dataset_sequence_id;

            // Convert filename to sequence_id
            if (dataset_sequence_filename != "")
                sequence_id = dataset_dump.convert_imgfilename_to_sequenceid(idx, dataset_sequence_filename);

            // Init total to next index
            total_idx = idx + 1;
            total_sequenceid = sequence_id + 1;
        }
        else
        {
            total_idx = dataset_dump._img_roots.size();

            if (dataset_sequence_id != -1)
                total_sequenceid = dataset_sequence_id + 1;
        }

        // Draw
        for (; idx < total_idx; idx++)
        {
            // Reset sequence_id
            if (sequence_id == total_sequenceid)
                sequence_id = 0;

            if (dataset_sequence_id == -1 && dataset_sequence_filename == "")
                total_sequenceid = dataset_dump._img_filenames[idx].size();
            for (; sequence_id < total_sequenceid; sequence_id++)
            {
                string org_img_path = dataset_dump.get_full_imgpath(idx, sequence_id);
                string vis_img_path = out_dir + "/dataset." + ZeroPadString(toString(idx), 3) + "-" + ZeroPadString(toString(sequence_id), 3) +
                                "." + get_filename(org_img_path) + ".jpg";
                Mat draw_img = imread(org_img_path);
                //Mat out_img = Mat(original_img.rows, original_img.cols, CV_8UC3, Scalar(0, 0, 0));
                vector<dump_object> single_filtered_dump;
                dataset_dump.get_singledump_with_filter(idx, sequence_id, single_filtered_dump);
                draw_kp(single_filtered_dump, draw_img, normsift);

                // Save image output
                imwrite(vis_img_path.c_str(), draw_img);

                // Write ok signal
                string vis_img_path_ok = vis_img_path + ".ok";
                text_write(vis_img_path_ok, ":)");

                // Visualize output to filename
                cout << get_filename(vis_img_path) << endl;
            }
        }

        return;
    }

    /// Both query and dataset
    // /*Support only one-to-one drawing*/
    if (query_dump_path != "" && dataset_dump_path != "" &&
        query_idx != -1 && dataset_idx != -1)
    {
        // Load dump file
		timespec startTime = CurrentPreciseTime();
		cout << "Load query dump.."; cout.flush();
        kp_dumper query_dump;
        query_dump.load(query_dump_path);
		cout << "done! (in " << setprecision(2) << fixed << TimeElapse(startTime) << " s)" << endl;
		
		startTime = CurrentPreciseTime();
		cout << "Load dataset dump.."; cout.flush();
        kp_dumper dataset_dump;
        dataset_dump.load(dataset_dump_path);
		cout << "done! (in " << setprecision(2) << fixed << TimeElapse(startTime) << " s)" << endl;

        // Convert filename to sequence_id
        int _query_sequence_id = query_sequence_id;
        int _dataset_sequence_id = dataset_sequence_id;
        if (query_sequence_filename != "")
            _query_sequence_id = query_dump.convert_imgfilename_to_sequenceid(query_idx, query_sequence_filename);
        if (dataset_sequence_filename != "")
            _dataset_sequence_id = dataset_dump.convert_imgfilename_to_sequenceid(dataset_idx, dataset_sequence_filename);

        /// Draw back projection from dataset to query
        if (back_projection)
        {

        }
        /// Draw both query and dataset, with link them together
        else
        {
            // Only support one by one drawing
			startTime = CurrentPreciseTime();
			cout << "Get image path.."; cout.flush();
            string query_image_path = query_dump.get_full_imgpath(query_idx, _query_sequence_id);
            string dataset_image_path = dataset_dump.get_full_imgpath(dataset_idx, _dataset_sequence_id);
            string vis_img_path = out_dir + "/match." + ZeroPadString(toString(query_idx), 3) + "-" + ZeroPadString(toString(_query_sequence_id), 3) + "-" +
                                                        ZeroPadString(toString(dataset_idx), 3) + "-" + ZeroPadString(toString(_dataset_sequence_id), 3) + "." +
                                                        get_filename(query_image_path) + "-" + get_filename(dataset_image_path) + "_vis.jpg";
			cout << "done! (in " << setprecision(2) << fixed << TimeElapse(startTime) << " s)" << endl;

            /// For query, this is important!!
            /// Please remove prefix/postfix of pre/post processing filename before use
            query_image_path = str_replace_all(query_image_path, ".qe", "");
            query_image_path = str_replace_all(query_image_path, ".qb_it0", "");

            // Fallback (for handling specific web opendir case)
            if (!is_path_exist(query_image_path))
            {
                string fallback_prefix = get_filename(get_directory(get_directory(get_directory(query_dump_path))));
                cout << "Fallback prefix: " << fallback_prefix << endl;
                query_image_path = str_replace_first(query_image_path, "/dev/shm/query/", "/dev/shm/query/" + fallback_prefix + "/");
                cout << "Trying to fall back query image to: " << greenc << query_image_path << endc << ".."; cout.flush();
                if (is_path_exist(query_image_path))
                    cout << greenc << "success" << endc << endl;
                else
                    cout << redc << "fail" << endc << endl;
            }

            Mat query_image = imread(query_image_path);         // Query
            Mat dataset_image = imread(dataset_image_path);     // Dataset
            Mat draw_image;
            concatimage(query_image, dataset_image, draw_image);// Drawing space
			
			startTime = CurrentPreciseTime();
			cout << "Get dump.."; cout.flush();
            vector<dump_object> query_single_filtered_dump;
            query_dump.get_singledump_with_filter(query_idx, _query_sequence_id, query_single_filtered_dump);
            vector<dump_object> dataset_single_filtered_dump;
            dataset_dump.get_singledump_with_filter(dataset_idx, _dataset_sequence_id, dataset_single_filtered_dump);
			cout << "done! (in " << setprecision(2) << fixed << TimeElapse(startTime) << " s)" << endl;
			// Draw
			cout << "Drawing.." << endl;
            draw_matches(query_single_filtered_dump, query_image,
                         dataset_single_filtered_dump, dataset_image,
                         draw_image, normsift);

            // Save output image (tmp)
            imwrite(vis_img_path.c_str(), draw_image, vector<int>({CV_IMWRITE_JPEG_QUALITY, 90}));

            // Write ok signal
            string vis_img_path_ok = vis_img_path + ".ok";
            text_write(vis_img_path_ok, ":)");

            // Visualize output to filename
            cout << get_filename(vis_img_path) << endl;
        }

        return;
    }

    // Cannot run any case
    cout << "Nothing can be visualized, please check parameters" << endl;
    return;
}

void draw_query_backprojection(kp_dumper& query_dump, kp_dumper& dataset_dump, bool normsift)
{
    /*
    Mat dataset_img = imread(dataset_path);

    // Draw point
    //cout << "query_size: " << query_size << endl;
    for (size_t match_id = 0; match_id < matches.size(); match_id++)
    {
        Mat affine(dataset_img.rows, dataset_img.cols, CV_8UC1, Scalar(0));

        SIFT_Keypoint affine_kp;
        affine_kp.x = matches[match_id].left_keypoint.x;
        affine_kp.y = matches[match_id].left_keypoint.y;
        affine_kp.a = matches[match_id].left_keypoint.a;
        affine_kp.b = matches[match_id].left_keypoint.b;
        affine_kp.c = matches[match_id].left_keypoint.c;

        if (normsift)
        {
            affine_kp.x *= dataset_img.cols;   // Re-scale pstart
            affine_kp.y *= dataset_img.rows;
        }

        // Draw affine mask
        draw_a_sift(affine, affine_kp, DRAW_AFFINE_MASK, RGB_SPACE, false);    // norm point is false, because it already rescaled.

        // Calculate bounding box
        // Store the set of points in the image before assembling the bounding box
        vector<Point> affine_mask_points;
        for (int row_id = 0; row_id < affine.rows; row_id++)
        {
            uchar *affine_ptr = affine.ptr<uchar>(row_id);
            for (int col_id = 0; col_id < affine.cols; col_id++)
            {
                if (affine_ptr[col_id])
                    affine_mask_points.push_back(Point(col_id, row_id));
            }
        }

        // Skip null affine !!! THIS ZERO AFFINE NEED TO BE CHECKED
        if (affine_mask_points.size() == 0)
            continue;

        // Compute minimal bounding box
        Rect affine_box = boundingRect(Mat(affine_mask_points));

        // Projecting back affine on query image
        if (normsift)
        {
            matches[match_id].right_keypoint.x *= query_size.width;   // Re-scale pstart
            matches[match_id].right_keypoint.y *= query_size.height;
        }
        Rect project_box(   matches[match_id].right_keypoint.x - (affine_box.width / 2.0f),
                            matches[match_id].right_keypoint.y - (affine_box.height / 2.0f),
                            affine_box.width,
                            affine_box.height);

        // Preparing dataset affine space
        Mat dataset_affine(draw_space.rows, draw_space.cols, CV_8UC3, Scalar(0, 0, 0));

        // Skip if overbox
        if (project_box.x + project_box.width > query_size.width - 1 ||
            project_box.y + project_box.height > query_size.height - 1 ||
            project_box.x < 0 ||
            project_box.y < 0)
            continue;

        // Copy image only within a croped affine area
        dataset_img(affine_box).copyTo(dataset_affine(project_box), affine(affine_box));

        //cout << project_box << endl;
        Mat dataset_affine_mask;
        cvtColor(dataset_affine, dataset_affine_mask, CV_RGB2GRAY);
        threshold(dataset_affine_mask, dataset_affine_mask, 1, 255, THRESH_BINARY);

        //rectangle(draw_space, project_box, Scalar(0, 255, 255), 2, 8);
        //draw_space(project_box) += (0.06 * dataset_affine);
        //draw_space = (draw_space.mul(0.5 * dataset_affine_mask)) + (0.5 * dataset_affine);

        for (int row_id = 0; row_id < draw_space.rows; row_id++)
        {
            uchar *draw_space_ptr = draw_space.ptr<uchar>(row_id);
            uchar *dataset_affine_ptr = dataset_affine.ptr<uchar>(row_id);
            for (int col_id = 0; col_id < draw_space.cols * 3; col_id++)
            {
                uchar a = draw_space_ptr[col_id];
                uchar b = dataset_affine_ptr[col_id];
                if (b > 0)
                    draw_space_ptr[col_id] = a * 0.5 + b * 0.5;
            }
        }
*/
        /*stringstream outpath;
        outpath << "/run/shm/query/paris6k_sifthesaff-rgb-norm-root_akm_1000000_kd3_16_qscale_r80_mask_roi_qbootstrap2_200_f2_qbmining_20_report/moulinrouge_4/paris_moulinrouge_000794.jpg_scaled_1_80.jpg_matches/affine" << match_id << ".png";
        cout << outpath.str() << endl;
        imwrite(outpath.str(), draw_space);*/
    //}

}

void draw_matches(vector<dump_object>& query_singledump, Mat& query_image, vector<dump_object>& dataset_singledump, Mat& dataset_image, Mat& draw_image, bool normsift)
{
	// Look up table
	bitset<1000000> query_mask;
	bitset<1000000> query_fg;
	Point2f* query_dump_LUT = new Point2f[1000000];
	
	timespec startTime = CurrentPreciseTime();
	cout << "Preparing Query lookup.."; cout.flush();
	// Constructing query lookup table.
	for (size_t query_dump_idx = 0; query_dump_idx < query_singledump.size(); query_dump_idx++)
	{
		size_t cluster_id = query_singledump[query_dump_idx].cluster_id;
		// Set only one feature
		if (!query_mask[cluster_id])
		{
			query_dump_LUT[cluster_id] = Point2f(query_singledump[query_dump_idx].kp.x, query_singledump[query_dump_idx].kp.y);
			query_mask.set(cluster_id);
		}
		// Set fg
		if (query_singledump[query_dump_idx].fg)
			query_fg.set(cluster_id);
	}
	cout << "done! (in " << setprecision(2) << fixed << TimeElapse(startTime) << " s)" << endl;
	
    // Draw line (all points to query)
	startTime = CurrentPreciseTime();
	cout << "Draw lines.."; cout.flush();
    for (size_t dataset_dump_idx = 0; dataset_dump_idx < dataset_singledump.size(); dataset_dump_idx++)
	{
		// Draw only match the same cluster_id
		size_t cluster_id = dataset_singledump[dataset_dump_idx].cluster_id;
		if (query_mask[cluster_id])
		{
			// Make point
			Point2f pstart = query_dump_LUT[cluster_id];
			Point2f pend(dataset_singledump[dataset_dump_idx].kp.x, dataset_singledump[dataset_dump_idx].kp.y);
			if (normsift)
			{
				pstart.x *= query_image.cols;   // Re-scale pstart
				pstart.y *= query_image.rows;
				pend.x *= dataset_image.cols;   // Re-scale pend
				pend.y *= dataset_image.rows;
				pend.x += query_image.cols;     // Shift end Offset
			}
			// Draw line
			line(draw_image, pstart, pend, Scalar(200, 0, 0), 1, CV_AA);    // Line
		}
	}
	cout << "done! (in " << setprecision(2) << fixed << TimeElapse(startTime) << " s)" << endl;

    // Draw point (all points to query)	
	startTime = CurrentPreciseTime();
	cout << "Draw points.."; cout.flush();
    for (size_t dataset_dump_idx = 0; dataset_dump_idx < dataset_singledump.size(); dataset_dump_idx++)
	{
		// Draw only match the same cluster_id
		size_t cluster_id = dataset_singledump[dataset_dump_idx].cluster_id;
		if (query_mask[cluster_id])
		{
			// Make point
			Point2f pstart = query_dump_LUT[cluster_id];
			Point2f pend(dataset_singledump[dataset_dump_idx].kp.x, dataset_singledump[dataset_dump_idx].kp.y);
			if (normsift)
			{
				pstart.x *= query_image.cols;   // Re-scale pstart
				pstart.y *= query_image.rows;
				pend.x *= dataset_image.cols;   // Re-scale pend
				pend.y *= dataset_image.rows;
				pend.x += query_image.cols;     // Shift end Offset
			}

			// Draw affine
			//draw_a_sift(draw_image, matches[match_id].left_keypoint, DRAW_AFFINE, RGB_SPACE, false);    // norm point is false, because it already rescaled.
			//draw_a_sift(draw_image, matches[match_id].right_keypoint, DRAW_AFFINE, RGB_SPACE, false);   // norm point is false, because it already rescaled.

			// Draw point
			Scalar color;
			if (query_fg[cluster_id])
				color = Scalar(0, 255, 0);  // Green
			else
				color = Scalar(0, 0, 255);  // Red
			circle(draw_image, pstart, 0, Scalar(0, 0, 0), 5, CV_AA);    // Border
			circle(draw_image, pend, 0, Scalar(0, 0, 0), 5, CV_AA);      // Border
			circle(draw_image, pstart, 0, color, 4, CV_AA);
			circle(draw_image, pend, 0, color, 4, CV_AA);

		}
	}
	cout << "done! (in " << setprecision(2) << fixed << TimeElapse(startTime) << " s)" << endl;
	
	// Release memory
	delete[] query_dump_LUT;
	
}

void draw_link(vector<dump_object>& query_singledump, Mat& query_image, vector<dump_object>& dataset_singledump, Mat& dataset_image, Mat& draw_image, bool normsift)
{

}

void draw_kp(vector<dump_object>& kp_dump, Mat& draw_image, bool normsift)
{
    // Draw all kp in this dump
    for (size_t kp_idx = 0; kp_idx < kp_dump.size(); kp_idx++)
    {
        Point2f center(kp_dump[kp_idx].kp.x, kp_dump[kp_idx].kp.y);             // x, y
        if (normsift)
        {
            center.x *= draw_image.cols;
            center.y *= draw_image.rows;
        }
        // FG/BG color
        Scalar color;
        if (kp_dump[kp_idx].fg)
            color = Scalar(0, 255, 0);  // Green
        else
            color = Scalar(0, 0, 255);  // Red

        circle(draw_image, center, 0, Scalar(0, 0, 0), 3, CV_AA);   // Border
        circle(draw_image, center, 0, color, 2, CV_AA);
    }
}

//;)
