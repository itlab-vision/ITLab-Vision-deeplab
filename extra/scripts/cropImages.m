% function CropImages(input_dir, output_dir, output_size)
% input_dir
% output_dir
% output_size - needed [rows cols] of output images

function cropImages(input_dir, output_dir, output_size)

if (exist(input_dir, 'dir') == 0)
   error('Failed to open a dir: "%s"', input_dir);
end
if (exist(output_dir, 'dir') == 0) 
   mkdir(output_dir);
end

list = dir(input_dir);

for i = 1 : numel(list),
   if (list(i).isdir == true) 
       continue;
   end

   path = fullfile(input_dir, list(i).name);
   fprintf('Processing image: "%s"...\n', path);
   img = imread(path);
   channels = size(img, 3);
   
   result_size = [min(output_size(1), size(img, 1)) min(output_size(2), size(img, 2))];
   resized = zeros([result_size channels], 'uint8');
   
   % crop image
   resized(1 : result_size(1), 1 : result_size(2), :) = img(1 : result_size(1), 1 : result_size(2), :);
   
   imwrite(resized, fullfile(output_dir, list(i).name));
end