% function [] = jpeg2ppm(input_dir, output_dir)

function [] = jpeg2ppm(input_dir, output_dir)

if (exist(input_dir, 'dir') == 0)
	error('Failed to open a dir: "%s"', input_dir);
end
if (exist(output_dir, 'dir') == 0) 
	mkdir(output_dir);
end

img_dir = dir(fullfile(input_dir, '*.jpg'));

for i = 1 : numel(img_dir)
    fprintf(1, 'processing %d (%d)...\n', i, numel(img_dir));
    img = imread(fullfile(input_dir, img_dir(i).name));

    img_fn = img_dir(i).name(1:end-4);
    save_fn = fullfile(output_dir, [img_fn, '.ppm']);

    imwrite(img, save_fn);
end
