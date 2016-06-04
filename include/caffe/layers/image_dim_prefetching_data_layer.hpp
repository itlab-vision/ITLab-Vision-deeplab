#ifndef CAFFE_IMAGE_DIM_PREFETCHING_DATA_LAYER_HPP_
#define CAFFE_IMAGE_DIM_PREFETCHING_DATA_LAYER_HPP_

#include <vector>

#include "caffe/blob.hpp"
#include "caffe/data_transformer.hpp"
#include "caffe/internal_thread.hpp"
#include "caffe/layer.hpp"
#include "caffe/layers/base_data_layer.hpp"
#include "caffe/proto/caffe.pb.h"
#include "caffe/util/blocking_queue.hpp"

namespace caffe {

template <typename Dtype>
struct SegmentationBatch {
  Blob<Dtype> data_;
  Blob<Dtype> label_;
  Blob<Dtype> dim_;
};

/** Deeplab
 * @brief prefetching data layer which also prefetches data dimensions
 *
 * TODO(dox): thorough documentation for Forward and proto params.
 */
template <typename Dtype>
class ImageDimPrefetchingDataLayer : 
    public BaseDataLayer<Dtype>, public InternalThread  {
 public:
  explicit ImageDimPrefetchingDataLayer(const LayerParameter& param);
  virtual ~ImageDimPrefetchingDataLayer() {}
  
  void LayerSetUp(const vector<Blob<Dtype>*>& bottom,
      const vector<Blob<Dtype>*>& top);

  virtual void Forward_cpu(const vector<Blob<Dtype>*>& bottom,
      const vector<Blob<Dtype>*>& top);
  virtual void Forward_gpu(const vector<Blob<Dtype>*>& bottom,
      const vector<Blob<Dtype>*>& top);
 protected:
  // Prefetches batches (asynchronously if to GPU memory)
  static const size_t MAX_PREFETCH_COUNT = 3;

  // The thread's function
  virtual void InternalThreadEntry();
  virtual void load_batch(SegmentationBatch<Dtype>* batch) = 0;

  SegmentationBatch<Dtype> prefetch_[MAX_PREFETCH_COUNT];
  size_t prefetch_count_;
  BlockingQueue<SegmentationBatch<Dtype>*> prefetch_free_;
  BlockingQueue<SegmentationBatch<Dtype>*> prefetch_full_;
  Blob<Dtype> transformed_data_;
  bool output_data_dim_;
};

}  // namespace caffe

#endif  // CAFFE_IMAGE_DIM_PREFETCHING_DATA_LAYER_HPP_
