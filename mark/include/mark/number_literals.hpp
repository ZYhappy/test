//
// Created by Sasha Wong on 2020/10/7.
//

#ifndef MARK_NUMBER_LITERALS_HPP_
#define MARK_NUMBER_LITERALS_HPP_

#include <cstdint>

namespace mark::number_literals {

constexpr uint8_t operator ""_u8 (unsigned long long int u8) {
  return static_cast<uint8_t>(u8);
}

constexpr int8_t operator ""_i8 (long double i8) {
  return static_cast<int8_t>(i8);
}

constexpr uint16_t operator ""_u16 (unsigned long long int u16) {
  return static_cast<uint16_t>(u16);
}

constexpr int16_t operator ""_i16 (long double i16) {
  return static_cast<int8_t>(i16);
}

constexpr uint32_t operator ""_u32 (unsigned long long int u32) {
  return static_cast<uint8_t>(u32);
}

constexpr int32_t operator ""_i32 (long double i32) {
  return static_cast<int32_t>(i32);
}

constexpr uint64_t operator ""_u64 (unsigned long long int u64) {
  return static_cast<uint64_t>(u64);
}

constexpr int64_t operator ""_i64 (long double i64) {
  return static_cast<int64_t>(i64);
}

}  // namespace mark::number_literals

#endif  // MARK_NUMBER_LITERALS_HPP_
