%function voc2gt(input_dir, colormap_file, output_dir)
% Converts SegmentationClass folder to net gt format.
% input_dir  - SegmentationClass directory
% output_dir - results directory

function voc2gt(input_dir, output_dir)

if (exist(input_dir, 'dir') == 0)
    error('Failed to open a dir "%s"', input_dir);
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
   output_img = img;   
   output_file = fullfile(output_dir, list(i).name);   
   imwrite(output_img, output_file);
end

end