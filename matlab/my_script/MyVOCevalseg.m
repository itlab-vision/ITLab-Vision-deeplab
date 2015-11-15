%VOCEVALSEG Evaluates a set of segmentation results.
% VOCEVALSEG(VOCopts); prints out the per class and overall
% segmentation accuracies. Accuracies are given using the intersection/union 
% metric:
%   true positives / (true positives + false positives + false negatives) 
%
% [ACCURACIES, AVACC, CONF] = VOCEVALSEG(VOCopts) returns the per class
% percentage ACCURACIES,  the average accuracy AVACC and the confusion
% matrix CONF.
%
% [ACCURACIES, AVACC, CONF, RAWCOUNTS] = VOCEVALSEG(VOCopts, ID) also returns
% the unnormalised confusion matrix,  which contains raw pixel counts.
function [accuracies, avacc, conf, rawcounts] = MyVOCevalseg(VOCopts)

% image test set
[gtIDs, t] = textread(sprintf(VOCopts.seg.imgsetpath, VOCopts.testset), '%s %d');

% number of labels = number of classes plus one for the background
classCount = VOCopts.nclasses + 1; 
confcounts = zeros(classCount);
totalImagesCount = 0;

missingImagesCount = 0;

tic;
for i = 1 : length(gtIDs)
    % display progress
    if (1 < toc)
        fprintf('test confusion: %d/%d\n',  i,  length(gtIDs));
        drawnow;
        tic;
    end
        
    imageName = gtIDs{i};
    
    % ground truth label file
    gtFile = sprintf(VOCopts.seg.clsimgpath, imageName);
    [gtImage, map] = imread(gtFile);    
    gtImage = double(gtImage);
    
    % results file
    resultsFile = sprintf(VOCopts.seg.clsrespath, VOCopts.testset, imageName);
    try
      [resultingImage, map] = imread(resultsFile);
    catch err
      missingImagesCount = missingImagesCount + 1;
      fprintf('Failed to read %s\n', resultsFile);
      continue;
    end

    resultingImage = double(resultingImage);
    
    % Check validity of results image
    maxLabel = max(resultingImage(:));
    if (VOCopts.nclasses < maxLabel),  
        error('Results image ''%s'' has out of range value %d (the value should be <= %d)', imageName, maxLabel, VOCopts.nclasses);
    end

    gtImageSize = size(gtImage); resultingImageSize = size(resultingImage);
    if any(gtImageSize ~= resultingImageSize)
        error('Results image ''%s'' is the wrong size, was %d x %d, should be %d x %d.', imageName, resultingImageSize(1), resultingImageSize(2), gtImageSize(1), gtImageSize(2));
    end
    
    %pixel locations to include in computation
    locs = gtImage < 255;
    
    % joint histogram
    sumim = 1 + gtImage + resultingImage * classCount; 
    hs = histc(sumim(locs), 1 : classCount * classCount); 
    totalImagesCount = totalImagesCount + numel(find(locs));
    confcounts(:) = confcounts(:) + hs(:);
end

if (missingImagesCount > 0)
  fprintf('There are %d missing results!\n',  missingImagesCount);
end

% confusion matrix - first index is true label,  second is inferred label
%conf = zeros(classCount);
conf = 100 * confcounts ./ repmat(1E-20 + sum(confcounts, 2), [1 size(confcounts, 2)]);
rawcounts = confcounts;

% Pixel Accuracy
overall_acc = 100 * sum(diag(confcounts)) / sum(confcounts(:));
fprintf('Percentage of pixels correctly labelled overall: %6.3f%%\n', overall_acc);

% Class Accuracy
class_acc = zeros(1,  classCount);
class_count = 0;
fprintf('Accuracy for each class (pixel accuracy)\n');
for i = 1 : classCount
    denom = sum(confcounts(i,  :));
    if (denom == 0)
        denom = 1;
    end
    class_acc(i) = 100 * confcounts(i,  i) / denom; 
    if i == 1
      clname = 'background';
    else
      clname = VOCopts.classes{i-1};
    end
    
    if ~strcmp(clname,  'void')
        class_count = class_count + 1;
        fprintf('  %14s: %6.3f%%\n',  clname,  class_acc(i));
    end
end
fprintf('-------------------------\n');
avg_class_acc = sum(class_acc) / class_count;
fprintf('Mean Class Accuracy: %6.3f%%\n',  avg_class_acc);

% Pixel IOU
accuracies = zeros(VOCopts.nclasses, 1);
fprintf('Accuracy for each class (intersection/union measure)\n');

realClassCount = 0;

for j = 1 : classCount
  gtj = sum(confcounts(j, :));
  resj = sum(confcounts(:, j));
  gtjresj = confcounts(j, j);
  % The accuracy is: true positive / (true positive + false positive + false negative) 
  % which is equivalent to the following percentage:
  denom = (gtj + resj - gtjresj);

  if (denom == 0)
    denom = 1;
  end

  accuracies(j) = 100 * gtjresj/denom;

  clname = 'background';
  if (1 < j),
    clname = VOCopts.classes{j-1};
  end;

  if ~strcmp(clname, 'void')
     realClassCount = realClassCount + 1;
  else
     if denom ~= 1
         fprintf(1, 'WARNING: this void class has denom = %d\n',  denom);
     end
  end

  if ~strcmp(clname, 'void')
     fprintf('  %14s: %6.3f%%\n', clname, accuracies(j));
  end
end

%accuracies = accuracies(1:end);
%avacc = mean(accuracies);
avacc = sum(accuracies) / realClassCount;

fprintf('-------------------------\n');
fprintf('Average accuracy: %6.3f%%\n', avacc);
