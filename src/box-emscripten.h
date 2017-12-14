#ifndef LIBHEIF_BOX_EMSCRIPTEN_H
#define LIBHEIF_BOX_EMSCRIPTEN_H

#include <emscripten/bind.h>

#include <memory>
#include <string>
#include <strstream>
#include <vector>

#include "box.h"

namespace heif {

static std::string get_headers_string(Box_hvcC* box) {
  if (!box) {
    return "";
  }

  std::vector<uint8_t> r = box->get_headers();
  return std::string(reinterpret_cast<const char*>(r.data()), r.size());
}

static std::string read_all_data_string(Box_iloc* box,
    const std::string& data) {
  if (!box) {
    return "";
  }

  std::istrstream s(data.data(), data.size());
  std::vector<uint8_t> r = box->read_all_data(s);
  return std::string(reinterpret_cast<const char*>(r.data()), r.size());
}

static std::string dump_box_header(BoxHeader* header) {
  if (!header) {
    return "";
  }

  Indent indent;
  return header->dump(indent);
}

static std::string dump_box(Box* box) {
  if (!box) {
    return "";
  }

  Indent indent;
  return box->dump(indent);
}

class EmscriptenBitstreamRange : public BitstreamRange {
 public:
  explicit EmscriptenBitstreamRange(const std::string& data)
    : BitstreamRange(nullptr, 0),
      data_(data),
      stream_(data_.data(), data_.size()) {
    construct(&stream_, data_.size(), nullptr);
  }
  bool error() const {
    return BitstreamRange::error();
  }

 private:
  std::string data_;
  std::istrstream stream_;
};

EMSCRIPTEN_BINDINGS(libheif) {
  emscripten::class_<Error>("Error")
    .constructor<>()
    ;

  emscripten::class_<BitstreamRange>("BitstreamRangeBase")
    ;

  emscripten::class_<EmscriptenBitstreamRange,
      emscripten::base<BitstreamRange>>("BitstreamRange")
    .constructor<const std::string&>()
    .function("error", &EmscriptenBitstreamRange::error)
    ;

  emscripten::class_<Indent>("Indent")
    .constructor<>()
    .function("get_indent", &Indent::get_indent)
    ;

  emscripten::class_<BoxHeader>("BoxHeader")
    .function("get_box_size", &BoxHeader::get_box_size)
    .function("get_header_size", &BoxHeader::get_header_size)
    .function("get_short_type", &BoxHeader::get_short_type)
    .function("get_type_string", &BoxHeader::get_type_string)
    .function("dump", &dump_box_header, emscripten::allow_raw_pointers())
    ;

  emscripten::class_<Box, emscripten::base<BoxHeader>>("Box")
    .class_function("read", &Box::read)
    .function("get_child_box", &Box::get_child_box)
    .function("dump", &dump_box, emscripten::allow_raw_pointers())
    .smart_ptr<std::shared_ptr<Box>>()
    ;

  emscripten::class_<Box_iloc, emscripten::base<Box>>("Box_iloc")
    .function("read_all_data", &read_all_data_string,
        emscripten::allow_raw_pointers())
    ;

  emscripten::class_<Box_hvcC, emscripten::base<Box>>("Box_hvcC")
    .function("get_headers", &get_headers_string,
        emscripten::allow_raw_pointers())
    ;
}

}  // namespace heif

#endif  // LIBHEIF_BOX_EMSCRIPTEN_H