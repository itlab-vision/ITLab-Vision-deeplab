% function [] = jpg2ppm(dataset_dir)
%
% dataset_dir - directory containing folder with .jpg-images
function [] = jpeg2ppm(input_dir, output_dir)

img_dir = dir(fullfile(input_dir, '*.jpg'));

for i = 1 : numel(img_dir)
    fprintf(1, 'processing %d (%d)...\n', i, numel(img_dir));
    img = imread(fullfile(input_dir, img_dir(i).name));

    img_fn = img_dir(i).name(1:end-4);
    save_fn = fullfile(output_dir, [img_fn, '.ppm']);

    imwrite(img, save_fn);
end
