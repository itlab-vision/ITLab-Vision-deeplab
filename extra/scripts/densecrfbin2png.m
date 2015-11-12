function [] = densecrfbin2png(map_folder, img_dir, color_map_file)

map_files = dir(fullfile(map_folder, '*.bin'));

if ~exist(img_dir, 'dir')
    mkdir(img_dir);
end

% load PASCAL colormap to fill segments
color_map = load(color_map_file);
color_map = color_map.colormap;

for i = 1 : numel(map_files)
    fprintf(1, 'processing %d (%d)...\n', i, numel(map_files));
    map = loadbin(fullfile(map_folder, map_files(i).name), 'int16');

    img_fn = map_files(i).name(1:end-4);
    imwrite(uint8(map), color_map, fullfile(img_dir, [img_fn, '.png']));
end
