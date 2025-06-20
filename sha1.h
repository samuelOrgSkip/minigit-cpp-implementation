#pragma once

#include <string>
#include <vector>

namespace hashing::sha1 {

// Rotates the bits of a 32-bit unsigned integer.
uint32_t leftRotate32bits(uint32_t n, std::size_t rotate);

// Transforms the 160-bit SHA-1 signature into a 40 char hex string.
std::string sig2hex(void* sig);

// The SHA-1 algorithm itself, taking in a bytestring.
void* hash_bs(const void* input_bs, uint64_t input_size);

// Converts the string to bytestring and calls the main algorithm.
void* hash(const std::string& message);

}  // namespace hashing::sha1


