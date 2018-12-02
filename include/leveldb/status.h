// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// A Status encapsulates the result of an operation.  It may indicate success,
// or it may indicate an error with an associated error message.
//
// Multiple threads can invoke const methods on a Status without
// external synchronization, but if any of the threads may call a
// non-const method, all threads accessing the same Status must use
// external synchronization.

#ifndef STORAGE_LEVELDB_INCLUDE_STATUS_H_
#define STORAGE_LEVELDB_INCLUDE_STATUS_H_

#include <algorithm>
#include <string>
#include "leveldb/export.h"
#include "leveldb/slice.h"

namespace leveldb {

class LEVELDB_EXPORT Status {
 public:
  // Create a success status.
  /*
   * 无参构造
   */
  Status() noexcept : state_(nullptr) { }
  /*
   * 析构，释放state_内存
   *
   */
  ~Status() { delete[] state_; }

  /*
   *
   * 拷贝构造与赋值
   */
  // 左值构造
  Status(const Status& rhs);
  // 左值赋值
  Status& operator=(const Status& rhs);

  // 右值构造
  Status(Status&& rhs) noexcept : state_(rhs.state_) {
    rhs.state_ = nullptr;   // 右值设置null,防止原始内存被释放
  }
  // 右值赋值
  Status& operator=(Status&& rhs) noexcept;


  /*
   * 类静态方法
   *
   *
   */
  // Return a success status.
  /*
   *
   * 返回成功的Status对象
   */
  static Status OK() { return Status(); }

  // Return error status of an appropriate type.
  static Status NotFound(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(kNotFound, msg, msg2);
  }
  static Status Corruption(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(kCorruption, msg, msg2);
  }
  static Status NotSupported(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(kNotSupported, msg, msg2);
  }
  static Status InvalidArgument(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(kInvalidArgument, msg, msg2);
  }
  static Status IOError(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(kIOError, msg, msg2);
  }

  /*
   * 实例方法
   */
  // Returns true iff the status indicates success.
  /*
   * 是否成功
   */
  bool ok() const { return (state_ == nullptr); }

  // Returns true iff the status indicates a NotFound error.
  /*
   *
   * key没找到
   */
  bool IsNotFound() const { return code() == kNotFound; }

  // Returns true iff the status indicates a Corruption error.
  /*
   * corruption错误
   *
   */
  bool IsCorruption() const { return code() == kCorruption; }

  // Returns true iff the status indicates an IOError.
  /*
   *
   * IO错误
   */
  bool IsIOError() const { return code() == kIOError; }

  // Returns true iff the status indicates a NotSupportedError.
  /*
   * 不支持错误
   */
  bool IsNotSupportedError() const { return code() == kNotSupported; }

  // Returns true iff the status indicates an InvalidArgument.
  /*
   * 参数无效错误
   */
  bool IsInvalidArgument() const { return code() == kInvalidArgument; }

  // Return a string representation of this status suitable for printing.
  // Returns the string "OK" for success.
  /*
   *
   */
  std::string ToString() const;

 private:
  // OK status has a null state_.  Otherwise, state_ is a new[] array
  // of the following form:
  //    state_[0..3] == length of message
  //    state_[4]    == code
  //    state_[5..]  == message
  /*
   * OK：null
   * 其它：char数组, 如上。
   *
   */
  const char* state_;

  /*
   * key错误码定义
   */
  enum Code {
    kOk = 0,
    kNotFound = 1,
    kCorruption = 2,
    kNotSupported = 3,
    kInvalidArgument = 4,
    kIOError = 5
  };

  /*
   *
   * 转换成错误码
   */
  Code code() const {
    return (state_ == nullptr) ? kOk : static_cast<Code>(state_[4]);
  }

  Status(Code code, const Slice& msg, const Slice& msg2);

  /*
   * 类静态方法不属于任何一个实例化对象，仅属于该类。
   * 拷贝state
   */
  static const char* CopyState(const char* s);
};

/*
 * 左值拷贝构造
 */
inline Status::Status(const Status& rhs) {
  state_ = (rhs.state_ == nullptr) ? nullptr : CopyState(rhs.state_);
}
/*
 * 左值赋值
 */
inline Status& Status::operator=(const Status& rhs) {
  // The following condition catches both aliasing (when this == &rhs),
  // and the common case where both rhs and *this are ok.
  /*
   *
   * state相同，则不需要拷贝状态。防止自赋值情况发生
   */
  if (state_ != rhs.state_) {
    // 预先删除state_
    delete[] state_;
    state_ = (rhs.state_ == nullptr) ? nullptr : CopyState(rhs.state_);
  }
  return *this;
}

/*
 *
 * 右值赋值
 */
inline Status& Status::operator=(Status&& rhs) noexcept {
  // 直接交换指针，即使是自赋值，也可以安全释放state_之前的内存
  std::swap(state_, rhs.state_);
  return *this;
}

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_INCLUDE_STATUS_H_
