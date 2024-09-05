
#include <cassert>
#include <iostream>

#include <pre/file/hash.hpp>
#include "util.hpp"



int main() {

    const std::string testdata_path{"testdata.bin"};

    // create a known file 
    {
        std::cout << "Generating data for '" << testdata_path << "'\n";
        xorshift64_state prng_state{};
        prng_state.a = 2475521982;  // I rolled a dice I swear :P

        std::fstream ofs(testdata_path, std::ios::out | std::ios::trunc | std::ios::in | std::ios::binary);


        for(int i = 0; i < 1024; i++) {
            ofs << xorshift64(&prng_state);
        }
    }

    // test hashing 
    std::string expected_hash{"e3cf40344d0f553266b66b6a68c413472e1c1f68"};
    std::string hashed_initial = pre::file::sha1sum(testdata_path);

    std::cout << "SHA1 hashed '" << testdata_path << "'\n"
        << " - expected: " << expected_hash << "\n"
        << " - computed: " << hashed_initial << std::endl;

    assert(expected_hash == hashed_initial);

    std::cout << " [PASS]" << std::endl;

    // check that the hash actually changes even for a single modification    
    // add one byte to the end 
    {
        std::fstream ofs(testdata_path, std::ios::out | std::ios::app | std::ios::binary);
        ofs << "X"; 
    }

    std::string hashed_modified = pre::file::sha1sum(testdata_path);

    std::cout << "Modifying '" << testdata_path << "' & hashing again \n"
        << " - computed: " << hashed_modified << std::endl;

    assert(hashed_modified != hashed_initial);

    std::cout << " [PASS]" << std::endl;

    return 0;
}