% function [] = blobmat2png(mat_dir, init_jpg_dir, color_map_file, png_dir)
%
% mat_dir        - directory containing .mat-files
% init_jpg_dir   - directory containing initial images
% color_map_file - PASCAL color map (pascal_segm_colormap.mat)
% png_dir        - output directory to save segmented images in .png format

function [] = blobmat2png(mat_dir, init_jpg_dir, color_map_file, png_dir)

% list all .mat-files
mat_files = dir(mat_dir);
% list all .jpg-files
jpg_files = dir(init_jpg_dir);
% load PASCAL colormap to fill segments
color_map = load(color_map_file);
color_map = color_map.colormap;

for i = 1 : length(mat_files)
    if (mat_files(i).isdir)
        continue;
    end

    % load .mat file data
    mat_file_name = fullfile(mat_dir, mat_files(i).name);
    score_map = load(mat_file_name);

    % load initial image size
    jpg_file_name = fullfile(init_jpg_dir, jpg_files(i).name);
    jpg_img = imread(jpg_file_name);
    img_row = size(jpg_img, 1);
    img_col = size(jpg_img, 2);

    % prepare data to write image in .png
    raw_result = permute(score_map.data, [2 1 3]);
    result = raw_result(1 : img_row, 1 : img_col, :);    
    [~, max_nchannel] = max(result, [], 3);
    max_nchannel = uint8(max_nchannel) - 1;

    % write data to .png-file
    index = strfind(mat_files(i).name, '_blob_');
    file_name = mat_files(i).name(1 : index(1) - 1);
    png_file_name = fullfile(png_dir, [file_name, '.png']);
    imwrite(uint8(max_nchannel), color_map, png_file_name);
end
