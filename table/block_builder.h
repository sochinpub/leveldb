// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_TABLE_BLOCK_BUILDER_H_
#define STORAGE_LEVELDB_TABLE_BLOCK_BUILDER_H_

#include <vector>

#include <stdint.h>
#include "leveldb/slice.h"

namespace leveldb {

struct Options;

class BlockBuilder {
 public:
  explicit BlockBuilder(const Options* options);

  // Reset the contents as if the BlockBuilder was just constructed.
  /*
   * 重置该block构造器的内容
   */
  void Reset();

  // REQUIRES: Finish() has not been called since the last call to Reset().
  // REQUIRES: key is larger than any previously added key
  /*
   * 上次Reset（）后，不能调用finish
   * key大于上次添加的key
   *
   */
  void Add(const Slice& key, const Slice& value);

  // Finish building the block and return a slice that refers to the
  // block contents.  The returned slice will remain valid for the
  // lifetime of this builder or until Reset() is called.
  /*
   * 结束构造，并返回一个指向block内容的Slice对象
   */
  Slice Finish();

  // Returns an estimate of the current (uncompressed) size of the block
  // we are building.
  /*
   *
   * 估计当前构造的block（未压缩）的size
   */
  size_t CurrentSizeEstimate() const;

  // Return true iff no entries have been added since the last Reset()
  bool empty() const {
    return buffer_.empty();
  }

 private:
  const Options*        options_;
  std::string           buffer_;      // Destination buffer   最终构造的buffer
  std::vector<uint32_t> restarts_;    // Restart points       保存restart的点
  int                   counter_;     // Number of entries emitted since restart 自restart后的entries数目
  bool                  finished_;    // Has Finish() been called?      Finish（）是否被调用过
  std::string           last_key_;    // 最近一次添加的key

  // No copying allowed
  /*
   *
   * 拷贝构造和赋值是private，不允许拷贝构造和赋值
   */
  BlockBuilder(const BlockBuilder&);
  void operator=(const BlockBuilder&);
};

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_TABLE_BLOCK_BUILDER_H_
