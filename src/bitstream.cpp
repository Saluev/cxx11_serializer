#include <algorithm> // for std::max, std::min
#include <cstring>   // for memcpy
#include "serializer/bitstream.hpp"      // for BitStream
#include "serializer/stream_utility.hpp" // for align, delimiter

namespace serializer {

void BitStream::reallocate(uint64_t how_much) {
    how_much = ((how_much + 63) / 64) * 8; // 8-bytes-aligned
    byte *new_data = new byte[how_much];
    if(bdata != nullptr) {
        memcpy(new_data, bdata, (allocated + 7) / 8);
        delete[] bdata;
    }
    bdata = new_data;
    allocated = how_much * 8;
}

BitStream::BitStream(void):
    bdata(nullptr),
    position (0),
    length   (0),
    state    (0),
    allocated(0) {

}

BitStream::BitStream(void* data, uint64_t length_in_bytes):
    bdata((byte*)data),
    position(0),
    length(length_in_bytes * 8),
    state(0),
    allocated(0) {

}

BitStream::~BitStream(void) {
    if(bdata != nullptr && allocated > 0) {
        delete[] bdata;
    }
}

int BitStream::rdstate(void) const {
    return state;
}

BitStream& BitStream::seekp(uint64_t p) {
    position = std::max(uint64_t(0), std::min(p, length));
    return *this;
}

void BitStream::write_bits(uint8_t how_much, uint64_t bits) {
    if(how_much + position > allocated) {
        if(allocated || bdata == nullptr) {
            reallocate(std::max(how_much + position, uint64_t(1.5 * allocated)));
        } else {
            state = 1; // error
            return;
        }
    }
    bits = bits & ((1 << how_much) - 1);
    // writing
    uint64_t *start = ((uint64_t*)bdata) + (position / 64);
    start[0] |= bits << (position % 64);
    if(how_much + position % 64 > 64) { // check to avoid invalid reads
        start[1] |= bits >> (64 - position % 64);
    }
    length   += how_much;
    position += how_much;
}

uint64_t BitStream::read_bits(uint8_t how_much) {
    if(how_much + position > allocated) {
        state = 1; // error
        return 0;
    }
    // reading
    uint64_t *start = ((uint64_t*)bdata) + position / 64;
    uint64_t result = (start[0] >> (position % 64));
    if(how_much + position % 64 > 64) {
        result |= (start[1] << (64 - position % 64));
    }
    position += how_much;
    return result & ((1 << how_much) - 1);
}

BitStream& delimiter(BitStream& bs) {
    return bs; // no delimiter necessary
}

BitStream& BitStream::operator<<(BitStream&(*func)(BitStream&)) {
    mode = MODE_READ;
    func(*this);
    return *this;
}

BitStream& BitStream::operator>>(BitStream&(*func)(BitStream&)) {
    mode = MODE_WRITE;
    func(*this);
    return *this;
}

BitStream& operator<<(BitStream& bs, const align& aligner) {
    auto mode = bs.get_mode();
    if(mode == bs.MODE_READ) {
        // TODO
    } else if(mode == bs.MODE_WRITE) {
        // TODO
    }
    return bs;
}

} // namespace serializer
