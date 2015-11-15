% function CropImages(input_dir, output_dir, output_size)
% input_dir
% output_dir
% output_size

function cropImages(input_dir, output_dir, output_size)

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