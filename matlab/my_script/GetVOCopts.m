%VOCopts = GetVOCopts(seg_root, seg_res_dir, trainset, testset, dataset)
% devkit_root_dir - where the dataset is placed
% results_dir - where is results (png)
% trainset, testset, dataset - your experiment configuration

function VOCopts = GetVOCopts(devkit_root_dir, results_dir, trainset, testset, dataset)
VOCopts.dataset = dataset;
VOCopts.trainset = trainset;
VOCopts.testset = testset;

VOCopts.datadir = strrep(fullfile(devkit_root_dir), '\', '/');
VOCopts.resdir = strrep(results_dir, '\', '/');

% initialize main challenge paths
VOCopts.annopath = strrep(fullfile(VOCopts.datadir, '/Annotations/%s.xml'), '\', '/');
VOCopts.imgpath = strrep(fullfile(VOCopts.datadir, '/JPEGImages/%s.jpg'), '\', '/');
VOCopts.imgsetpath = strrep(fullfile(VOCopts.datadir, '/ImageSets/Main/%s.txt'), '\', '/');
VOCopts.clsimgsetpath = strrep(fullfile(VOCopts.datadir, '/ImageSets/Main/%s_%s.txt'), '\', '/');

VOCopts.clsrespath = strrep(fullfile(VOCopts.resdir, ['Main/%s_cls_', testset, '_%s.txt']), '\', '/');
VOCopts.detrespath = strrep(fullfile(VOCopts.resdir, ['Main/%s_det_', testset ,'_%s.txt']), '\', '/');

% initialize segmentation task paths
VOCopts.seg.clsimgpath = [VOCopts.datadir '/SegmentationClass/%s.png'];

VOCopts.seg.instimgpath=[VOCopts.datadir '/SegmentationObject/%s.png'];
VOCopts.seg.imgsetpath=[VOCopts.datadir '/ImageSets/Segmentation/%s.txt'];

VOCopts.seg.clsresdir=[VOCopts.resdir 'Segmentation/%s_%s_cls'];
VOCopts.seg.instresdir=[VOCopts.resdir 'Segmentation/%s_%s_inst'];
VOCopts.seg.clsrespath=[VOCopts.seg.clsresdir '/%s.png'];
VOCopts.seg.instrespath=[VOCopts.seg.instresdir '/%s.png'];

% initialize layout task paths

VOCopts.layout.imgsetpath=[VOCopts.datadir VOCopts.dataset '/ImageSets/Layout/%s.txt'];
VOCopts.layout.respath=[VOCopts.resdir 'Layout/%s_layout_' VOCopts.testset '.xml'];

% initialize action task paths

VOCopts.action.imgsetpath=[VOCopts.datadir VOCopts.dataset '/ImageSets/Action/%s.txt'];
VOCopts.action.clsimgsetpath=[VOCopts.datadir VOCopts.dataset '/ImageSets/Action/%s_%s.txt'];
VOCopts.action.respath=[VOCopts.resdir 'Action/%s_action_' VOCopts.testset '_%s.txt'];

% initialize the VOC challenge options

% classes

if ~isempty(strfind(VOCopts.dataset, 'VOC'))
  VOCopts.classes={...
    'aeroplane'
    'bicycle'
    'bird'
    'boat'
    'bottle'
    'bus'
    'car'
    'cat'
    'chair'
    'cow'
    'diningtable'
    'dog'
    'horse'
    'motorbike'
    'person'
    'pottedplant'
    'sheep'
    'sofa'
    'train'
    'tvmonitor'};
  
elseif ~isempty(strfind(VOCopts.dataset, 'coco')) || ~isempty(strfind(VOCopts.dataset, 'COCO'))
  coco_categories = GetCocoCategories();
  VOCopts.classes = coco_categories.values();
else
  error('Unknown dataset!\n');
end
 
VOCopts.nclasses=length(VOCopts.classes);	


% poses

VOCopts.poses={...
    'Unspecified'
    'Left'
    'Right'
    'Frontal'
    'Rear'};

VOCopts.nposes=length(VOCopts.poses);

% layout parts

VOCopts.parts={...
    'head'
    'hand'
    'foot'};    

VOCopts.nparts=length(VOCopts.parts);

VOCopts.maxparts=[1 2 2];   % max of each of above parts

% actions

VOCopts.actions={...    
    'other'             % skip this when training classifiers
    'jumping'
    'phoning'
    'playinginstrument'
    'reading'
    'ridingbike'
    'ridinghorse'
    'running'
    'takingphoto'
    'usingcomputer'
    'walking'};

VOCopts.nactions=length(VOCopts.actions);

% overlap threshold

VOCopts.minoverlap=0.5;
