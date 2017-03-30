#include <algorithm>
#include <fstream>  // NOLINT(readability/streams)
#include <iostream>  // NOLINT(readability/streams)
#include <string>
#include <utility>
#include <vector>

#ifdef USE_OPENCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#endif  // USE_OPENCV

#include "caffe/data_transformer.hpp"
#include "caffe/layers/image_seg_data_layer.hpp"
#include "caffe/util/benchmark.hpp"
#include "caffe/util/io.hpp"
#include "caffe/util/math_functions.hpp"
#include "caffe/util/rng.hpp"

namespace caffe {

template <typename Dtype>
ImageSegDataLayer<Dtype>::~ImageSegDataLayer<Dtype>() {
  this->StopInternalThread();
}

template <typename Dtype>
void ImageSegDataLayer<Dtype>::DataLayerSetUp(const vector<Blob<Dtype>*>& bottom,
      const vector<Blob<Dtype>*>& top) {
  const int new_height = this->layer_param_.image_data_param().new_height();
  const int new_width  = this->layer_param_.image_data_param().new_width();
  const bool is_color  = this->layer_param_.image_data_param().is_color();
  const int label_type = this->layer_param_.image_data_param().label_type();
  string root_folder = this->layer_param_.image_data_param().root_folder();

  TransformationParameter transform_param = this->layer_param_.transform_param();
  CHECK(transform_param.has_mean_file() == false) <<
         "ImageSegDataLayer does not support mean file";
  CHECK((new_height == 0 && new_width == 0) ||
      (new_height > 0 && new_width > 0)) << "Current implementation requires "
      "new_height and new_width to be set at the same time.";

  // Read the file with filenames and labels
  const string& source = this->layer_param_.image_data_param().source();
  LOG(INFO) << "Opening file " << source;
  std::ifstream infile(source.c_str());

  string linestr;
  while (std::getline(infile, linestr)) {
    std::istringstream iss(linestr);
    string imgfn;
    iss >> imgfn;
    string segfn = "";
    if (label_type != ImageDataParameter_LabelType_NONE) {
      iss >> segfn;
    }
    lines_.push_back(std::make_pair(imgfn, segfn));
  }

  CHECK(!lines_.empty()) << "File is empty";

  if (this->layer_param_.image_data_param().shuffle()) {
    // randomly shuffle data
    LOG(INFO) << "Shuffling data";
    const unsigned int prefetch_rng_seed = caffe_rng_rand();
    prefetch_rng_.reset(new Caffe::RNG(prefetch_rng_seed));
    ShuffleImages();
  }
  LOG(INFO) << "A total of " << lines_.size() << " images.";

  lines_id_ = 0;
  // Check if we would need to randomly skip a few data points
  if (this->layer_param_.image_data_param().rand_skip()) {
    unsigned int skip = caffe_rng_rand() %
        this->layer_param_.image_data_param().rand_skip();
    LOG(INFO) << "Skipping first " << skip << " data points.";
    CHECK_GT(lines_.size(), skip) << "Not enough points to skip";
    lines_id_ = skip;
  }

  // Read an image, and use it to initialize the top blob.
  cv::Mat cv_img = ReadImageToCVMat(root_folder + lines_[lines_id_].first,
                                    new_height, new_width, is_color);

  CHECK(cv_img.data) << "Could not load " << lines_[lines_id_].first;

  const int batch_size = this->layer_param_.image_data_param().batch_size();
  CHECK_GT(batch_size, 0) << "Positive batch size required";

  vector<int> top_shape = this->data_transformer_->InferBlobShape(cv_img);
  top_shape[0] = batch_size;

  vector<int> top_data_shape = top_shape;

  vector<int> top_label_shape = top_shape;
  top_label_shape[1] = 1;

  vector<int> top_data_dim_shape(4);
  top_data_dim_shape[0] = batch_size;
  top_data_dim_shape[1] = 1;
  top_data_dim_shape[2] = 1;
  top_data_dim_shape[3] = 2; // dimensions of each image, img_height, img_width

  const size_t prefetch_count = this->prefetch_count_;
  for (int i = 0; i < prefetch_count; ++i) {
    this->prefetch_[i].data_.Reshape(top_data_shape);
    this->prefetch_[i].label_.Reshape(top_label_shape);
    this->prefetch_[i].dim_.Reshape(top_data_dim_shape);
  }

  top[0]->Reshape(top_data_shape);
  this->transformed_data_.Reshape(top_data_shape);

  top[1]->Reshape(top_label_shape);
  this->transformed_label_.Reshape(top_label_shape);


  top[2]->Reshape(top_data_dim_shape);

  LOG(INFO) << "output data size: " << top[0]->num() << ","
      << top[0]->channels() << "," << top[0]->height() << ","
      << top[0]->width();
  // label
  LOG(INFO) << "output label size: " << top[1]->num() << ","
      << top[1]->channels() << "," << top[1]->height() << ","
      << top[1]->width();
  // image_dim
  LOG(INFO) << "output data_dim size: " << top[2]->num() << ","
      << top[2]->channels() << "," << top[2]->height() << ","
      << top[2]->width();
  LOG(INFO) << "prefetching up to " << prefetch_count << " batches";
}

template <typename Dtype>
void ImageSegDataLayer<Dtype>::ShuffleImages() {
  caffe::rng_t* prefetch_rng =
      static_cast<caffe::rng_t*>(prefetch_rng_->generator());
  shuffle(lines_.begin(), lines_.end(), prefetch_rng);
}

template <typename Dtype>
void ImageSegDataLayer<Dtype>::load_batch(SegmentationBatch<Dtype>* batch) {
  CPUTimer batch_timer;
  batch_timer.Start();
  double read_time = 0;
  double trans_time = 0;
  CPUTimer timer;
  CHECK(batch);
  CHECK(batch->data_.count());
  CHECK(batch->label_.count());
  CHECK(batch->dim_.count());
  CHECK(this->transformed_data_.count());
  CHECK(this->transformed_label_.count());

  const int max_height = batch->data_.height();
  const int max_width  = batch->data_.width();

  ImageDataParameter image_data_param = this->layer_param_.image_data_param();
  const int batch_size = image_data_param.batch_size();
  const int new_height = image_data_param.new_height();
  const int new_width  = image_data_param.new_width();
  const int label_type = image_data_param.label_type();
  const int ignore_label = image_data_param.ignore_label();
  const bool is_color  = image_data_param.is_color();
  const string root_folder   = image_data_param.root_folder();

  Dtype* top_data = batch->data_.mutable_cpu_data();
  Dtype* top_label = batch->label_.mutable_cpu_data();
  Dtype* top_data_dim = batch->dim_.mutable_cpu_data();

  vector<cv::Mat> cv_img_seg(2);

  const int lines_size = lines_.size();
  for (int item_id = 0; item_id < batch_size; ++item_id) {
    const int top_data_offset = batch->data_.offset(item_id);
    const int top_label_offset = batch->label_.offset(item_id);
    const int top_data_dim_offset = batch->dim_.offset(item_id);

    // get a blob
    timer.Start();
    CHECK_GT(lines_size, lines_id_);

    ReadImageToCVMat(root_folder + lines_[lines_id_].first,
        new_height, new_width, is_color, &cv_img_seg[0]);
    if (!cv_img_seg[0].data) {
      DLOG(INFO) << "Failed to load image: " << root_folder + lines_[lines_id_].first;
    }

    top_data_dim[top_data_dim_offset + 0] =
        static_cast<Dtype>(std::min(max_height, cv_img_seg.back().rows));
    top_data_dim[top_data_dim_offset + 1] =
        static_cast<Dtype>(std::min(max_width, cv_img_seg.back().cols));

    if (label_type == ImageDataParameter_LabelType_PIXEL) {
      ReadImageToCVMat(root_folder + lines_[lines_id_].second,
          new_height, new_width, false, &cv_img_seg[1]);
      if (!cv_img_seg[1].data) {
        DLOG(INFO) << "Failed to load labels: " << root_folder + lines_[lines_id_].second;
      }
    } else if (label_type == ImageDataParameter_LabelType_IMAGE) {
      const int label = atoi(lines_[lines_id_].second.c_str());
      cv_img_seg[1] = cv::Mat(cv_img_seg[0].rows, cv_img_seg[0].cols,
          CV_8UC1, cv::Scalar(label));
    } else {
      cv_img_seg[1] = cv::Mat(cv_img_seg[0].rows, cv_img_seg[0].cols,
          CV_8UC1, cv::Scalar(ignore_label));
    }

    read_time += timer.MicroSeconds();
    timer.Start();

    // Apply transformations (mirror, crop...) to the image
    this->transformed_data_.set_cpu_data(top_data + top_data_offset);
    this->transformed_label_.set_cpu_data(top_label + top_label_offset);

    this->data_transformer_->TransformImgAndSeg(cv_img_seg,
       &(this->transformed_data_), &(this->transformed_label_),
       ignore_label);
    trans_time += timer.MicroSeconds();

    lines_id_++;
    if (lines_id_ >= lines_size) {
      // We have reached the end. Restart from the first.
      DLOG(INFO) << "Restarting data prefetching from start.";
      lines_id_ = 0;
      if (this->layer_param_.image_data_param().shuffle()) {
        ShuffleImages();
      }
    }
  }
  batch_timer.Stop();
  DLOG(INFO) << "Prefetch batch: " << batch_timer.MilliSeconds() << " ms.";
  DLOG(INFO) << "     Read time: " << read_time / 1000 << " ms.";
  DLOG(INFO) << "Transform time: " << trans_time / 1000 << " ms.";
}

INSTANTIATE_CLASS(ImageSegDataLayer);
REGISTER_LAYER_CLASS(ImageSegData);

}  // namespace caffe
