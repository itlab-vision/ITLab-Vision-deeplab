% Script allows easily get results of segmentation
SetupEnv;

% Convert results to png
if (feature_is_png == false)
    fprintf('Saving png to: "%s"\n', output_png_dir);
    
    if (strcmp(feature_type, 'fc8'))
        blobmat2png(output_dir, colormap_path, dataset_images_dir, output_png_dir);
    elseif (strcmp(feature_type, 'crf'))
        densecrfbin2png(output_dir, colormap_path, output_png_dir);
    end
    
    feature_is_png = true;
end

% Get iou score
if strcmp(testset, 'val')
  [accuracies, avacc, conf, rawcounts] = MyVOCevalseg(VOC_opts);
else
  fprintf('This is test set. No evaluation. Just saved as png\n');
end 