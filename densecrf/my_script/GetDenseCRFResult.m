% compute the densecrf result (.bin) to png
%

map_folder = fullfile('C:\Programming\CProjects\nnsu\movie_maker-build\bin\densecrf_val_original'); 
save_root_folder = map_folder; 
map_dir = dir(fullfile(map_folder, '*.bin'));

fprintf(1,' saving to %s\n', save_root_folder);

save_result_folder = fullfile(map_folder, 'png');

if ~exist(save_result_folder, 'dir')
    mkdir(save_result_folder);
end

for i = 1 : numel(map_dir)
    fprintf(1, 'processing %d (%d)...\n', i, numel(map_dir));
    map = LoadBinFile(fullfile(map_folder, map_dir(i).name), 'int16');

    img_fn = map_dir(i).name(1:end-4);
    imwrite(uint8(map), colormap, fullfile(save_result_folder, [img_fn, '.png']));
end
