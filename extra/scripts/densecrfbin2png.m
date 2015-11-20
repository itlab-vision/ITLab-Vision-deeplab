% function [] = densecrfbin2png(map_folder, color_map_file, png_dir)
%
% map_folder     - directory containing .bin-files (denseCRF output files)
% color_map_file - PASCAL color map (pascal_segm_colormap.mat)
% png_dir        - output directory for saving .png-images

function [] = densecrfbin2png(map_folder, color_map_file, png_dir)

if (exist(map_folder, 'dir') == 0)
    error('Failed to open a dir: "%s"', map_folder);
end
if (exist(png_dir, 'dir') == 0)
    mkdir(png_dir);
end

map_files = dir(fullfile(map_folder, '*.bin'));

% load PASCAL colormap to fill segments
color_map = load(color_map_file);
color_map = color_map.colormap;

for i = 1 : numel(map_files)
    fprintf(1, 'processing %d (%d)...\n', i, numel(map_files));
    map = loadbin(fullfile(map_folder, map_files(i).name), 'int16');

    img_fn = map_files(i).name(1:end-4);
    imwrite(uint8(map), color_map, fullfile(png_dir, [img_fn, '.png']));
end