#pragma once

#include <stdexcept>
#include <system_error>
#include <iterator>
#include <fstream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/uuid/detail/sha1.hpp>

namespace pre::file {

  namespace {
    constexpr char int_to_hex[16] = {
      '0', '1', '2', '3', '4', '5', '6', '7', '8',
      '9', 'a', 'b', 'c', 'd', 'e', 'f'
    };
  }

  using namespace std::string_literals;

  //! Calculate the SHA1 sum for a file on disk
  inline std::string sha1sum(const std::string &path, std::error_condition &ec) {

    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if (!ifs.is_open()) {
      ec = std::make_error_condition(std::errc::bad_file_descriptor);
      return ""s;
    }

    boost::uuids::detail::sha1 sha1;
    
    const size_t buffer_size = 4096;
    std::vector<char> buffer (buffer_size,0);
    
    while(ifs.read(&buffer[0], buffer_size)) {
      sha1.process_bytes(buffer.data(), buffer_size);
    }

    // consume the remainder
    sha1.process_bytes(buffer.data(), ifs.gcount());

    if (ifs.bad()) {
      ec = std::make_error_condition(std::errc::io_error);
      return "";
    } else {
      ec.clear();
    }

    // compute the hash
    boost::uuids::detail::sha1::digest_type hash = {0};
    char hash_buf[41] = {0};

    sha1.get_digest(hash);    

    if constexpr (std::size(hash) == 20) {
      // Newer Boost has digest_type of type "unsigned char[20]"
      for (size_t i = 0; i < std::size(hash); i++) {
        hash_buf[i * 2 + 0] = int_to_hex[(hash[i] >> 4) & 0xF];
        hash_buf[i * 2 + 1] = int_to_hex[(hash[i]     ) & 0xF];
      }
    } else if constexpr (std::size(hash) == 5) {
      // Older Boost has digest_type of type "unsigned int[5]"
      for (size_t i = 0; i < std::size(hash); i++) {
        hash_buf[i * 2 + 0] = int_to_hex[(hash[i] >> 28) & 0xF];
        hash_buf[i * 2 + 1] = int_to_hex[(hash[i] >> 24) & 0xF];
        hash_buf[i * 2 + 2] = int_to_hex[(hash[i] >> 20) & 0xF];
        hash_buf[i * 2 + 3] = int_to_hex[(hash[i] >> 16) & 0xF];
        hash_buf[i * 2 + 4] = int_to_hex[(hash[i] >> 12) & 0xF];
        hash_buf[i * 2 + 5] = int_to_hex[(hash[i] >>  8) & 0xF];
        hash_buf[i * 2 + 6] = int_to_hex[(hash[i] >>  4) & 0xF];
        hash_buf[i * 2 + 7] = int_to_hex[(hash[i]      ) & 0xF];
      }
    } else {
      throw std::runtime_error("Unsupported boost::uuids::detail::sha1::digest_type size");
    }

    return std::string(hash_buf);
  }

  //! Calculate the SHA1 sum for a file on disk
  //! Throws on error
  inline std::string sha1sum(const std::string &path) {
    std::error_condition ec{};
    auto ret = sha1sum(path, ec);

    if (ec) {
      throw std::runtime_error("Failed to compute SHA1 sum for file "s + path + ": error "s + ec.message());
    }

    return ret;
  }


}
