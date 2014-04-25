/*
 * bow_matching_visualizer.cpp
 *
 *  Created on: November 5, 2013
 *      Author: Siriwat Kasamwattanarote
 */
#include "bow_matching_visualizer.h"

using namespace std;
using namespace tr1;
using namespace cv;
using namespace alphautils;
using namespace alphautils::imtools;

int main(int argc,char *argv[])
{
    string bow_matches_path;
    string output_image_path;
    bool normsift = true;

    // App Options
    unordered_map<string, string> opts;

    if (argc > 1) // Explicitly run
    {
        // Grab all params
        for (int param_idx = 1; param_idx < argc; param_idx += 2)
            opts[string(argv[param_idx])] = string(argv[param_idx + 1]);

        /// bow_matches_file or back projection matches into query
        if (opts.find("i") != opts.end() || opts.find("pj") != opts.end())
        {
            if (opts.find("i") != opts.end())   // File matching
                bow_matches_path = opts["i"];
            else                                // Back projection
                bow_matches_path = opts["pj"];

            if (!is_path_exist(bow_matches_path))
            {
                cout << "Bow matches path: " << bow_matches_path << " does not exit!" << endl;
                exit(0);
            }
        }
        else
        {
            cout << "Please specify bow matches path" << endl;
            exit(0);
        }

        /// output_image
        if (opts.find("o") != opts.end())
        {
            output_image_path = opts["o"];
            make_dir_available(get_directory(output_image_path));
        }
        else
            output_image_path = bow_matches_path + "_vis.png";

        /// normsift option
        if (opts.find("norm") != opts.end())
        {
            if (opts["norm"] == "n")
                normsift = false;
        }
    }
    else
    {
        cout << "Params must be \"bow_matching_visualizer i bow_matches_file o output_image norm normsift:y|n\"" << endl;
		exit(1);
    }

    //opts["pj"] = "/run/shm/query/paris6k_sifthesaff-rgb-norm-root_akm_1000000_kd3_16_qscale_r80_mask_roi_qbootstrap2_200_f2_qbmining_20_report/moulinrouge_1/paris_moulinrouge_000667.jpg_scaled_1_80.jpg_matches";
    //bow_matches_path = opts["pj"];
    //output_image_path = bow_matches_path + "_vis.png";


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
    }
}

void load_match_file(const string& bow_matches_path)
{
    ifstream InFile (bow_matches_path.c_str());
    if (InFile)
    {
        // Initial onetime variable
        has_dataset_path = false;
        has_query_path = false;

        string read_line;
        while (!InFile.eof())
        {
            getline(InFile, read_line);

            if (read_line != "")
            {
                if (read_line[0] == '/')
                {
                    if (!has_dataset_path)
                    {   // Read dataset path
                        dataset_path = read_line;
                        has_dataset_path = true;
                    }
                    else
                    {   // Read query path
                        query_path = read_line;
                        has_query_path = true;
                    }
                }
                else
                {
                    const char* delim = " ";// space
                    vector<string> vals;
                    string_splitter(read_line, delim, vals);

                    size_t cluster_id = atoll(vals[0].c_str());
                    float weight = atof(vals[1].c_str());
                    SIFT_Keypoint left_keypoint = {float(atof(vals[2].c_str())), float(atof(vals[3].c_str())), float(atof(vals[4].c_str())), float(atof(vals[5].c_str())), float(atof(vals[6].c_str()))};
                    SIFT_Keypoint right_keypoint = {float(atof(vals[7].c_str())), float(atof(vals[8].c_str())), float(atof(vals[9].c_str())), float(atof(vals[10].c_str())), float(atof(vals[11].c_str()))};
                    match_object new_obj = {cluster_id, weight, left_keypoint, right_keypoint};
                    matches.push_back(new_obj);
                }
            }
        }
        // Close file
        InFile.close();
    }
}

void draw_matches(const string& output_image_path, bool normsift)
{
    Mat dataset_img;
    Mat query_img;
    Mat draw_space;

    // Load image
    if (has_dataset_path)
        dataset_img = imread(dataset_path);
    if (has_query_path)
        query_img = imread(query_path);

    // Create draw space
    if (has_query_path)
        concatimage(dataset_img, query_img, draw_space);
    else
        draw_space = dataset_img;

    // Draw line
    for (size_t match_id = 0; match_id < matches.size(); match_id++)
    {
        Point2f pstart(matches[match_id].left_keypoint.x, matches[match_id].left_keypoint.y);
        Point2f pend(matches[match_id].right_keypoint.x, matches[match_id].right_keypoint.y);
        if (normsift)
        {
            pstart.x *= dataset_img.cols;   // Re-scale pstart
            pstart.y *= dataset_img.rows;
            if (has_query_path)
            {
                pend.x *= query_img.cols; // Re-scale pend
                pend.y *= query_img.rows;
                pend.x += dataset_img.cols; // Shift end Offset
            }
            else
            {
                pend.x *= dataset_img.cols; // Re-scale pend
                pend.y *= dataset_img.rows;
            }

            // Save back for later use
            matches[match_id].left_keypoint.x = pstart.x;
            matches[match_id].left_keypoint.y = pstart.y;
            matches[match_id].right_keypoint.x = pend.x;
            matches[match_id].right_keypoint.y = pend.y;
        }
        //int rand_b = rand() % 175 + 80;
        //int rand_g = rand() % 175 + 80;
        //int rand_r = rand() % 175 + 80;

        int weight_g = 255 * matches[match_id].weight * 5;
        int weight_r = 255 * (1 - matches[match_id].weight * 5);

        line(draw_space, pstart, pend, Scalar(0, weight_g, weight_r), 3, CV_AA);
        //line(draw_space, pstart, pend, Scalar(rand_b, rand_g, rand_r), 1, CV_AA);
    }

    // Draw point
    for (size_t match_id = 0; match_id < matches.size(); match_id++)
    {
        Point2f pstart(matches[match_id].left_keypoint.x, matches[match_id].left_keypoint.y);
        Point2f pend(matches[match_id].right_keypoint.x, matches[match_id].right_keypoint.y);
        // Resue the rescaled with offset keypoint
        /*if (normsift)
        {
            pstart.x *= dataset_img.cols;   // Re-scale pstart
            pstart.y *= dataset_img.rows;
            if (has_query_path)
            {
                pend.x *= query_img.cols; // Re-scale pend
                pend.y *= query_img.rows;
                pend.x += dataset_img.cols; // Shift end Offset
            }
            else
            {
                pend.x *= dataset_img.cols; // Re-scale pend
                pend.y *= dataset_img.rows;
            }
        }*/

        // Draw affine for both pstart, and pend
        draw_a_sift(draw_space, matches[match_id].left_keypoint, DRAW_AFFINE, RGB_SPACE, false);    // norm point is false, because it already rescaled.
        draw_a_sift(draw_space, matches[match_id].right_keypoint, DRAW_AFFINE, RGB_SPACE, false);   // norm point is false, because it already rescaled.

        circle(draw_space, pstart, 0, Scalar(0, 0, 0), 6, CV_AA);
        circle(draw_space, pend, 0, Scalar(0, 0, 0), 6, CV_AA);
        circle(draw_space, pstart, 0, Scalar(0, 255, 0), 3, CV_AA);
        circle(draw_space, pend, 0, Scalar(0, 255, 255), 3, CV_AA);
    }

    imwrite(output_image_path, draw_space);
}

void draw_back_project(Mat& draw_space, bool normsift)
{
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

        /*stringstream outpath;
        outpath << "/run/shm/query/paris6k_sifthesaff-rgb-norm-root_akm_1000000_kd3_16_qscale_r80_mask_roi_qbootstrap2_200_f2_qbmining_20_report/moulinrouge_4/paris_moulinrouge_000794.jpg_scaled_1_80.jpg_matches/affine" << match_id << ".png";
        cout << outpath.str() << endl;
        imwrite(outpath.str(), draw_space);*/
    }
}

//;)
