#include <boost/thread.hpp>
#include <vector>

#include "caffe/blob.hpp"
#include "caffe/data_transformer.hpp"
#include "caffe/internal_thread.hpp"
#include "caffe/layer.hpp"
#include "caffe/layers/image_dim_prefetching_data_layer.hpp"
#include "caffe/proto/caffe.pb.h"
#include "caffe/util/blocking_queue.hpp"

namespace caffe {

template <typename Dtype>
ImageDimPrefetchingDataLayer<Dtype>::ImageDimPrefetchingDataLayer(
    const LayerParameter& param) 
    : BaseDataLayer<Dtype>(param),
      prefetch_free_(), prefetch_full_() {
}

template <typename Dtype>
void ImageDimPrefetchingDataLayer<Dtype>::LayerSetUp(
    const vector<Blob<Dtype>*>& bottom, const vector<Blob<Dtype>*>& top) {
  if (top.size() == 3) {
    output_data_dim_ = true;
  } else {
    output_data_dim_ = false;
  }

  prefetch_count_ = std::min<size_t>(MAX_PREFETCH_COUNT,
      this->layer_param_.image_data_param().prefetch());
  if (prefetch_count_ == 0) {
    prefetch_count_ = 1; // do minimal prefetching
  }
  for (size_t i = 0; i < prefetch_count_; ++i) {
    prefetch_free_.push(&prefetch_[i]);
  }
  BaseDataLayer<Dtype>::LayerSetUp(bottom, top);
  // Now, start the prefetch thread. Before calling prefetch, we make two
  // cpu_data calls so that the prefetch thread does not accidentally make
  // simultaneous cudaMalloc calls when the main thread is running. In some
  // GPUs this seems to cause failures if we do not so.
  for (size_t i = 0; i < prefetch_count_; ++i) {
    prefetch_[i].data_.mutable_cpu_data();
    if (this->output_labels_) {
      prefetch_[i].label_.mutable_cpu_data();
    }
    if (output_data_dim_) {
      prefetch_[i].dim_.mutable_cpu_data();
    }
  }
  #ifndef CPU_ONLY
  if (Caffe::mode() == Caffe::GPU) {
    for (size_t i = 0; i < prefetch_count_; ++i) {
      prefetch_[i].data_.mutable_gpu_data();
      if (this->output_labels_) {
        prefetch_[i].label_.mutable_gpu_data();
      }
      if (output_data_dim_) {
        prefetch_[i].dim_.mutable_gpu_data();
      }
    }
  }
  #endif

  DLOG(INFO) << "Initializing prefetch";
  StartInternalThread();
  DLOG(INFO) << "Prefetch initialized.";
}

template <typename Dtype>
void ImageDimPrefetchingDataLayer<Dtype>::InternalThreadEntry() {
#ifndef CPU_ONLY
  cudaStream_t stream;
  if (Caffe::mode() == Caffe::GPU) {
    CUDA_CHECK(cudaStreamCreateWithFlags(&stream, cudaStreamNonBlocking));
  }
#endif

  try {
    while (!must_stop()) {
      SegmentationBatch<Dtype>* batch = prefetch_free_.pop();
      load_batch(batch);
#ifndef CPU_ONLY
      if (Caffe::mode() == Caffe::GPU) {
        batch->data_.data().get()->async_gpu_push(stream);
        CUDA_CHECK(cudaStreamSynchronize(stream));
        batch->label_.data().get()->async_gpu_push(stream);
        CUDA_CHECK(cudaStreamSynchronize(stream));
      }
#endif
      prefetch_full_.push(batch);
    }
  } catch (boost::thread_interrupted&) {
    // Interrupted exception is expected on shutdown
  }
#ifndef CPU_ONLY
  if (Caffe::mode() == Caffe::GPU) {
    CUDA_CHECK(cudaStreamDestroy(stream));
  }
#endif
}

template <typename Dtype>
void ImageDimPrefetchingDataLayer<Dtype>::Forward_cpu(
    const vector<Blob<Dtype>*>& bottom, const vector<Blob<Dtype>*>& top) {
  SegmentationBatch<Dtype>* batch = prefetch_full_.pop(
      "Data layer prefetch queue empty");  

  caffe_copy(batch->data_.count(), batch->data_.cpu_data(),
      top[0]->mutable_cpu_data());
  if (this->output_labels_) {
    caffe_copy(batch->label_.count(), batch->label_.cpu_data(),
        top[1]->mutable_cpu_data());
  }
  if (output_data_dim_) {
    caffe_copy(batch->dim_.count(), batch->dim_.cpu_data(),
        top[2]->mutable_cpu_data());
  }
  
  prefetch_free_.push(batch);
}


#ifdef CPU_ONLY
STUB_GPU_FORWARD(ImageDimPrefetchingDataLayer, Forward);
#endif

INSTANTIATE_CLASS(ImageDimPrefetchingDataLayer);

}  // namespace caffe
