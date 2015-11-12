% Script allows easily get results of segmentation
SetupEnv2;

fprintf('Saving to %s\n', results_dir);
if (exist(results_dir, 7) == false)
   mkdir(results_dir); 
end

% Convert results to png
if (is_mat == true)
    blobmat2png(net_output_mat_dir, dataset_images_dir, colormap_path, results_dir);
    is_mat = false;
elseif (is_denseCRF_done == false)
    densecrfbin2png(denseCRF_results_dir, results_dir, colormap_path);
    is_denseCRF_done = true;
end

% Get iou score
if strcmp(testset, 'val')
  [accuracies, avacc, conf, rawcounts] = MyVOCevalseg(VOC_opts, id);
else
  fprintf('This is test set. No evaluation. Just saved as png\n');
end 