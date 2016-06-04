#include <vector>

#include "caffe/layers/image_dim_prefetching_data_layer.hpp"

namespace caffe {

template <typename Dtype>
void ImageDimPrefetchingDataLayer<Dtype>::Forward_gpu(
    const vector<Blob<Dtype>*>& bottom, const vector<Blob<Dtype>*>& top) {
  SegmentationBatch<Dtype>* batch = prefetch_full_.pop(
      "Data layer prefetch queue empty");  

  caffe_copy(batch->data_.count(), batch->data_.gpu_data(),
	     top[0]->mutable_gpu_data());
  if (this->output_labels_) {
    caffe_copy(batch->label_.count(), batch->label_.gpu_data(),
	       top[1]->mutable_gpu_data());
  }
  if (output_data_dim_) {
    caffe_copy(batch->dim_.count(), batch->dim_.cpu_data(),
	       top[2]->mutable_gpu_data());
  }
  
  // Ensure the copy is synchronous wrt the host, so that the next batch isn't
  // copied in meanwhile.
  CUDA_CHECK(cudaStreamSynchronize(cudaStreamDefault));
  prefetch_free_.push(batch);
}

INSTANTIATE_LAYER_GPU_FORWARD(ImageDimPrefetchingDataLayer);

}  // namespace caffe
