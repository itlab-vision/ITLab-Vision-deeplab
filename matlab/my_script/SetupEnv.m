%SETUPENV Initializes a set of working variables
%   

%% Datasets
dataset    = 'voc12';          %'voc12', 'coco'
trainset   = 'trainval';
testset    = 'val';            %'val', 'test'

%% Completed steps
feature_name = 'features2';
feature_type = 'crf'; % fc8 / crf
feature_is_png = true;

% The output has been taken argmax already (e.g., coco dataset). 
% assume the argmax takes C-convention (i.e., start from 0)
is_argmax_taken = false;

%% Model
model_name = 'deeplab_largeFOV';

% Models and parameters:
% vgg128_noup (not optimized well), aka DeepLab
% bi_w = 5, bi_x_std = 50, bi_r_std = 10

% vgg128_ms_pool3, aka DeepLab-MSc
% bi_w = 3, bi_x_std = 95, bi_r_std = 3

% vgg128_noup_pool3_cocomix, aka DeepLab-COCO
% bi_w = 5, bi_x_std = 67, bi_r_std = 3

%% these are used for the bounding box weak annotation experiments (i.e., to generate the Bbox-Seg)
% erode_gt (bbox)
% bi_w = 41, bi_x_std = 33, bi_r_std = 4

% erode_gt/bboxErode20
% bi_w = 45, bi_x_std = 37, bi_r_std = 3, pos_w = 15, pos_x_std = 3
 
%
% initial or default values for crf
bi_w           = 5; 
bi_x_std       = 50;
bi_r_std       = 3;

pos_w          = 3;
pos_x_std      = 3;


%% used for cross-validation
%rng(10)

% downsampling files for cross-validation
down_sample_method = 2;      % 1: equally sample with "down_sample_rate", 2: randomly pick "num_sample" samples
down_sample_rate   = 8;
num_sample         = 100;    % number of samples used for cross-validation

% ranges for cross-validation
range_pos_w = [3];
range_pos_x_std = [3];

range_bi_w = [5];
range_bi_x_std = [49];
range_bi_r_std = [4 5];

%% Pathes
colormap_path = fullfile('pascal_seg_colormap.mat');
fprintf('Loading colormap from %s\n', colormap_path);
load(colormap_path);

dataset_dir = fullfile('Z:\Semantic segmentation\Datasets\pascal2012devkit');
dataset_images_dir = fullfile(dataset_dir, 'JPEGImages');
gt_dir = fullfile('C:\Programming\CProjects\nnsu\datasets\voc2012\SegmentationAug');

exper_dir      = fullfile('Z:\Semantic Segmentation');
output_dir     = fullfile(exper_dir, 'Results', model_name, [feature_type, '_', testset]);
output_png_dir = fullfile(output_dir, 'png');

VOC_opts = GetVOCopts(dataset_dir, output_png_dir, trainset, testset, 'VOC2012');