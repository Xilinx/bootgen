// C++ implementation of a minimal TLC (Transfer List Compiler) "create"
// command compatible with the Python tool's TL format.

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include <set>

namespace tlc {

static inline std::size_t align_up(std::size_t n, std::size_t alignment) {
    return (n + alignment - 1) / alignment * alignment;
}

struct TransferEntry {
    uint32_t id;       // 24-bit significant
    uint8_t hdr_size;  // always 8
    uint32_t data_size;
    std::vector<uint8_t> data;
    uint32_t offset;   // offset of TE header from TL base

    static constexpr uint8_t HDR_SIZE = 8;

    TransferEntry(uint32_t id_, const std::vector<uint8_t>& d, uint32_t off)
        : id(id_), hdr_size(HDR_SIZE), data_size(static_cast<uint32_t>(d.size())), data(d), offset(off) {}

    std::size_t size_with_padding(std::size_t granule) const {
        return align_up(HDR_SIZE + data.size(), granule);
    }

    void write(std::ostream& os) const {
        // Write 3-byte little-endian ID
        uint8_t id_bytes[3] = { static_cast<uint8_t>(id & 0xFF), static_cast<uint8_t>((id >> 8) & 0xFF), static_cast<uint8_t>((id >> 16) & 0xFF) };
        os.write(reinterpret_cast<const char*>(id_bytes), 3);
        // Pack <BI : hdr_size (u8), data_size (u32 LE)
        os.put(static_cast<char>(hdr_size));
        uint32_t ds = data_size; // little-endian
        os.write(reinterpret_cast<const char*>(&ds), sizeof(ds));
        // data
        if (!data.empty()) {
            os.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
        }
    }
};

struct TransferList {
    // Header: <I4B4I
    static constexpr uint32_t SIGNATURE = 0x4A0FB10B;
    static constexpr uint8_t VERSION = 1;
    static constexpr uint8_t HDR_SIZE = 0x18; // 24 bytes
    static constexpr std::size_t GRANULE = 8;

    uint32_t signature = SIGNATURE;
    uint8_t checksum = 0;
    uint8_t version = VERSION;
    uint8_t hdr_size = HDR_SIZE;
    uint8_t alignment = 3; // power-of-two exponent
    uint32_t used_size = HDR_SIZE;
    uint32_t total_size = 0x1000;
    uint32_t flags = 0x1; // checksum enabled
    uint32_t reserved = 0;

    std::vector<TransferEntry> entries;

    explicit TransferList(uint32_t max_size, uint32_t flags_, uint8_t align_pow2)
        : alignment(align_pow2), total_size(max_size), flags(flags_) {
        update_checksum();
    }

    void update_checksum() {
        // checksum byte such that sum(header+entries) % 256 == 0
        checksum = 0;
        uint32_t sum = sum_bytes();
        checksum = static_cast<uint8_t>((256 - (sum % 256)) % 256);
    }

    uint32_t sum_bytes() const {
        // Sum of all bytes of current TL serialization with current checksum
        std::vector<uint8_t> bytes;
        serialize(bytes);
        uint32_t s = 0;
        for (auto b : bytes) s = (s + b) & 0xFF; // keep modulo 256
        return s;
    }

    void serialize(std::vector<uint8_t>& out) const {
        out.clear();
        out.reserve(total_size);
        // Header <I4B4I little-endian
        auto push_u32 = [&out](uint32_t v){
            out.push_back(static_cast<uint8_t>(v & 0xFF));
            out.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
            out.push_back(static_cast<uint8_t>((v >> 16) & 0xFF));
            out.push_back(static_cast<uint8_t>((v >> 24) & 0xFF));
        };
        push_u32(signature);
        out.push_back(checksum);
        out.push_back(version);
        out.push_back(hdr_size);
        out.push_back(alignment);
        push_u32(used_size);
        push_u32(total_size);
        push_u32(flags);
        push_u32(reserved);

        // Entries
        // We'll mirror write_to_file behavior when actually writing to stream.
        // Here for checksum we need the exact same bytes including padding.
        // Recompute serialization like write_to_file would do.
        // Since used_size already reflects sizes, generate from entries vector.
        std::size_t cursor = HDR_SIZE;
        for (const auto& te : entries) {
            // header + data
            // 3-byte ID + 1-byte hdr_size + 4-byte data_size
            out.push_back(static_cast<uint8_t>(te.id & 0xFF));
            out.push_back(static_cast<uint8_t>((te.id >> 8) & 0xFF));
            out.push_back(static_cast<uint8_t>((te.id >> 16) & 0xFF));
            out.push_back(static_cast<uint8_t>(te.hdr_size));
            push_u32(te.data_size);
            out.insert(out.end(), te.data.begin(), te.data.end());
            cursor += 8 + te.data.size();
            std::size_t pad_to = align_up(cursor, GRANULE);
            out.insert(out.end(), pad_to - cursor, 0);
            cursor = pad_to;
        }
    }

    void write(std::ostream& os) {
        update_checksum();
        // Write header
        uint32_t sig = signature;
        os.write(reinterpret_cast<const char*>(&sig), sizeof(sig));
        os.put(static_cast<char>(checksum));
        os.put(static_cast<char>(version));
        os.put(static_cast<char>(hdr_size));
        os.put(static_cast<char>(alignment));
        uint32_t u = used_size, t = total_size, f = flags, r = reserved;
        os.write(reinterpret_cast<const char*>(&u), sizeof(u));
        os.write(reinterpret_cast<const char*>(&t), sizeof(t));
        os.write(reinterpret_cast<const char*>(&f), sizeof(f));
        os.write(reinterpret_cast<const char*>(&r), sizeof(r));

        // Entries with padding to granule
        std::size_t cursor = HDR_SIZE;
        for (const auto& te : entries) {
            te.write(os);
            cursor += 8 + te.data.size();
            std::size_t pad_to = align_up(cursor, GRANULE);
            std::size_t pad = pad_to - cursor;
            if (pad) {
                static const uint8_t zeros[8] = {0};
                while (pad) {
                    std::size_t chunk = std::min<std::size_t>(pad, sizeof(zeros));
                    os.write(reinterpret_cast<const char*>(zeros), static_cast<std::streamsize>(chunk));
                    pad -= chunk;
                }
            }
            cursor = pad_to;
        }
    }

    void add_transfer_entry(uint32_t id, const std::vector<uint8_t>& data, uint8_t data_align_pow2 = 0) {
        // Calculate where this TE's data would start if appended now
        std::size_t data_offset = used_size + TransferEntry::HDR_SIZE;
        uint8_t align_pow2 = data_align_pow2 ? data_align_pow2 : alignment;
        std::size_t aligned_data_offset = align_up(data_offset, static_cast<std::size_t>(1) << align_pow2);

        if (id != 0 && data_offset != aligned_data_offset) {
            // Insert an empty TE (id=0) as in Python to fill the gap
            std::size_t void_len = aligned_data_offset - data_offset - TransferEntry::HDR_SIZE;
            if (used_size + TransferEntry::HDR_SIZE + void_len > total_size) {
                throw std::runtime_error("TL size exceeded while aligning");
            }
            std::vector<uint8_t> zeros(void_len, 0);
            TransferEntry void_te(0, zeros, static_cast<uint32_t>(used_size));
            entries.push_back(void_te);
            used_size += static_cast<uint32_t>(align_up(void_te.data.size() + TransferEntry::HDR_SIZE, GRANULE));
        }

        if (used_size + TransferEntry::HDR_SIZE + data.size() > total_size) {
            throw std::runtime_error("TL size has exceeded the maximum allocation.");
        }

        TransferEntry te(id, data, static_cast<uint32_t>(used_size));
        entries.push_back(te);
        used_size += static_cast<uint32_t>(align_up(te.data.size() + TransferEntry::HDR_SIZE, GRANULE));
        if (align_pow2 > alignment) alignment = align_pow2;
        update_checksum();
    }
};

} // namespace tlc

static void print_usage() {
    std::cerr << "Usage: tlc create [options] <output>\n"
              << "Options:\n"
              << "  --fdt <path>          Add FDT as tag 1\n"
              << "  --entry <id> <path>   Add blob with tag id\n"
              << "  --from-yaml <file>    Build TL from JSON (YAML-compatible) config\n"
              << "  --align,-a <n>        Alignment as power-of-two (default 3)\n"
              << "  --size,-s <bytes>     Max TL size (default 4096)\n"
              << "  --flags <int>         Flags (default 1)\n";
}

static int cmd_create(int argc, char** argv) {
    std::string fdt_path;
    std::string output_path;
    uint32_t max_size = 0x1000;
    uint8_t align_pow2 = 3;
    uint32_t flags = 0x1;
    std::string from_yaml_path;
    struct EntryArg{ uint32_t id; std::string path;};
    std::vector<EntryArg> entries;

    int i = 2;
    while (i < argc) {
        std::string arg = argv[i];
        if (arg == "--fdt") {
            if (i + 1 >= argc) { std::cerr << "Missing value after --fdt\n"; return 1; }
            fdt_path = argv[++i];
        } else if (arg == "--from-yaml") {
            if (i + 1 >= argc) { std::cerr << "Missing value after --from-yaml\n"; return 1; }
            from_yaml_path = argv[++i];
        } else if (arg == "--entry") {
            if (i + 2 >= argc) { std::cerr << "--entry requires <id> <path>\n"; return 1; }
            char* endp=nullptr; long idv = std::strtol(argv[++i], &endp, 0);
            if (endp == argv[i] || idv < 0 || idv > 0xFFFFFF) { std::cerr << "Invalid entry id\n"; return 1; }
            entries.push_back({ static_cast<uint32_t>(idv), std::string(argv[++i]) });
        } else if (arg == "--align" || arg == "-a") {
            if (i + 1 >= argc) { std::cerr << "Missing value after " << arg << "\n"; return 1; }
            char* endp=nullptr; long v = std::strtol(argv[++i], &endp, 0);
            if (endp == argv[i] || v < 0 || v > 31) { std::cerr << "Invalid align pow2\n"; return 1; }
            align_pow2 = static_cast<uint8_t>(v);
        } else if (arg == "--size" || arg == "-s") {
            if (i + 1 >= argc) { std::cerr << "Missing value after " << arg << "\n"; return 1; }
            char* endp=nullptr; long long v = std::strtoll(argv[++i], &endp, 0);
            if (endp == argv[i] || v <= 0 || v > 0x7fffffffLL) { std::cerr << "Invalid size value\n"; return 1; }
            max_size = static_cast<uint32_t>(v);
        } else if (arg == "--flags") {
            if (i + 1 >= argc) { std::cerr << "Missing value after --flags\n"; return 1; }
            char* endp=nullptr; long long v = std::strtoll(argv[++i], &endp, 0);
            if (endp == argv[i] || v < 0 || v > 0xffffffffLL) { std::cerr << "Invalid flags\n"; return 1; }
            flags = static_cast<uint32_t>(v);
        } else if (!arg.empty() && arg[0] == '-') {
            std::cerr << "Unknown option: " << arg << "\n"; return 1;
        } else {
            output_path = arg;
        }
        ++i;
    }

    if (output_path.empty()) { std::cerr << "Output path is required\n"; print_usage(); return 1; }
    if (!fdt_path.empty()) { entries.push_back({1u, fdt_path}); }

    try {
        if (!from_yaml_path.empty()) {
            // Build from YAML config file with proper YAML parser
            struct JVal {
                enum T {NUL, BOOL, NUM, STR, ARR, OBJ} t=NUL; bool b=false; long long i=0; std::string s; std::vector<JVal> a; std::vector<std::pair<std::string,JVal>> o; };

            // Robust YAML parser
            struct YAMLParser {
                std::vector<std::string> lines;
                size_t line_idx = 0;

                YAMLParser(const std::string& text) {
                    std::istringstream iss(text);
                    std::string line;
                    while (std::getline(iss, line)) {
                        lines.push_back(line);
                    }
                }

                int get_indent(const std::string& line) {
                    int indent = 0;
                    for (size_t i = 0; i < line.size(); ++i) {
                        if (line[i] == ' ') indent++;
                        else if (line[i] == '\t') indent += 4;
                        else break;
                    }
                    return indent;
                }

                std::string trim(const std::string& s) {
                    size_t start = 0, end = s.size();
                    while (start < end && (s[start] == ' ' || s[start] == '\t')) ++start;
                    while (end > start && (s[end-1] == ' ' || s[end-1] == '\t' || s[end-1] == '\r')) --end;
                    return s.substr(start, end - start);
                }

                bool is_empty_or_comment(const std::string& trimmed) {
                    return trimmed.empty() || trimmed[0] == '#';
                }

                JVal parse_value(const std::string& val_str) {
                    std::string v = trim(val_str);
                    JVal result;

                    if (v == "true") { result.t = JVal::BOOL; result.b = true; }
                    else if (v == "false") { result.t = JVal::BOOL; result.b = false; }
                    else if (v == "null" || v.empty()) { result.t = JVal::NUL; }
                    else if (v.size() >= 2 && v[0] == '"' && v.back() == '"') {
                        result.t = JVal::STR;
                        result.s = v.substr(1, v.size()-2);
                    }
                    else {
                        // Try to parse as number
                        char* endp = nullptr;
                        if (v.rfind("0x", 0) == 0 || v.rfind("0X", 0) == 0) {
                            long long num = std::strtoll(v.c_str(), &endp, 16);
                            if (endp != v.c_str()) { result.t = JVal::NUM; result.i = num; }
                            else { result.t = JVal::STR; result.s = v; }
                        } else {
                            long long num = std::strtoll(v.c_str(), &endp, 10);
                            if (endp != v.c_str() && (*endp == '\0' || *endp == ' ')) { result.t = JVal::NUM; result.i = num; }
                            else { result.t = JVal::STR; result.s = v; }
                        }
                    }
                    return result;
                }

                JVal parse_object(int base_indent) {
                    JVal obj;
                    obj.t = JVal::OBJ;
                    size_t start_idx = line_idx;

                    while (line_idx < lines.size()) {
                        std::string line = lines[line_idx];
                        std::string trimmed = trim(line);
                        int indent = get_indent(line);

                        if (is_empty_or_comment(trimmed)) {
                            line_idx++;
                            continue;
                        }

                        if (indent < base_indent) break;
                        if (indent > base_indent) {
                            line_idx++;
                            continue;
                        }

                        if (trimmed[0] == '-') break; // Array item

                        size_t colon = trimmed.find(':');
                        if (colon == std::string::npos) {
                            line_idx++;
                            continue;
                        }

                        std::string key = trim(trimmed.substr(0, colon));
                        std::string value_part = trim(trimmed.substr(colon + 1));
                        line_idx++;

                        if (!value_part.empty()) {
                            obj.o.emplace_back(key, parse_value(value_part));
                        } else {
                            // Look ahead for nested content
                            if (line_idx < lines.size()) {
                                std::string next_line = lines[line_idx];
                                int next_indent = get_indent(next_line);
                                std::string next_trimmed = trim(next_line);

                                if (next_indent > base_indent && !is_empty_or_comment(next_trimmed)) {
                                    if (next_trimmed[0] == '-') {
                                        // Parse array
                                        JVal arr = parse_array(next_indent);
                                        obj.o.emplace_back(key, arr);
                                    } else {
                                        // Parse nested object
                                        JVal nested = parse_object(next_indent);
                                        obj.o.emplace_back(key, nested);
                                    }
                                }
                            }
                        }

                        if (line_idx > start_idx + 10000) {
                            throw std::runtime_error("YAML parser: infinite loop detected");
                        }
                    }

                    return obj;
                }

                JVal parse_array(int base_indent) {
                    JVal arr;
                    arr.t = JVal::ARR;
                    size_t start_idx = line_idx;

                    while (line_idx < lines.size()) {
                        std::string line = lines[line_idx];
                        int indent = get_indent(line);
                        std::string trimmed = trim(line);

                        if (is_empty_or_comment(trimmed)) {
                            line_idx++;
                            continue;
                        }

                        if (indent < base_indent) break;
                        if (indent > base_indent) {
                            line_idx++;
                            continue;
                        }

                        if (trimmed[0] != '-') break;

                        std::string item_content = trim(trimmed.substr(1));

                        if (item_content.empty()) {
                            // Array item with nested content on following lines
                            line_idx++;
                            if (line_idx < lines.size()) {
                                std::string next_line = lines[line_idx];
                                int next_indent = get_indent(next_line);

                                if (next_indent > base_indent) {
                                    JVal nested = parse_object(next_indent);
                                    arr.a.push_back(nested);
                                }
                            }
                        } else if (item_content.find(':') != std::string::npos) {
                            // Inline object definition (e.g., "- key: value")
                            line_idx++;
                            // Parse the rest of this item as an object
                            JVal obj;
                            obj.t = JVal::OBJ;

                            // Add the first key-value from the dash line
                            size_t colon = item_content.find(':');
                            std::string first_key = trim(item_content.substr(0, colon));
                            std::string first_value = trim(item_content.substr(colon + 1));

                            if (!first_value.empty()) {
                                obj.o.emplace_back(first_key, parse_value(first_value));
                            } else {
                                // First key has nested content
                                if (line_idx < lines.size()) {
                                    std::string next_line = lines[line_idx];
                                    int next_indent = get_indent(next_line);
                                    std::string next_trimmed = trim(next_line);

                                    if (next_indent > indent && !is_empty_or_comment(next_trimmed)) {
                                        if (next_trimmed[0] == '-') {
                                            JVal arr_val = parse_array(next_indent);
                                            obj.o.emplace_back(first_key, arr_val);
                                        } else if (next_trimmed.find(':') != std::string::npos) {
                                            JVal obj_val = parse_object(next_indent);
                                            obj.o.emplace_back(first_key, obj_val);
                                        }
                                    }
                                }
                            }

                            // Now parse remaining keys at indent level that's > base_indent but <= the object's content level
                            int content_indent = -1;  // Track the indent of the object's keys
                            while (line_idx < lines.size()) {
                                std::string line2 = lines[line_idx];
                                int indent2 = get_indent(line2);
                                std::string trimmed2 = trim(line2);

                                if (is_empty_or_comment(trimmed2)) {
                                    line_idx++;
                                    continue;
                                }

                                // Stop if we dedent back to array level or less
                                if (indent2 <= indent) break;

                                // Skip lines that are more indented (they belong to current value)
                                if (content_indent == -1) {
                                    // First real key - establish the content indent level
                                    content_indent = indent2;
                                } else if (indent2 > content_indent) {
                                    // This line is nested within a value, skip it
                                    line_idx++;
                                    continue;
                                } else if (indent2 < content_indent) {
                                    // Dedented below object level, stop
                                    break;
                                }

                                if (trimmed2[0] == '-') break;  // Another array item

                                size_t colon2 = trimmed2.find(':');
                                if (colon2 == std::string::npos) {
                                    line_idx++;
                                    continue;
                                }

                                std::string key = trim(trimmed2.substr(0, colon2));
                                std::string value_part = trim(trimmed2.substr(colon2 + 1));
                                line_idx++;

                                if (!value_part.empty()) {
                                    obj.o.emplace_back(key, parse_value(value_part));
                                } else {
                                    if (line_idx < lines.size()) {
                                        std::string next_line2 = lines[line_idx];
                                        int next_indent2 = get_indent(next_line2);
                                        std::string next_trimmed2 = trim(next_line2);

                                        if (next_indent2 > content_indent && !is_empty_or_comment(next_trimmed2)) {
                                            if (next_trimmed2[0] == '-') {
                                                JVal arr_val = parse_array(next_indent2);
                                                obj.o.emplace_back(key, arr_val);
                                            } else if (next_trimmed2.find(':') != std::string::npos) {
                                                JVal obj_val = parse_object(next_indent2);
                                                obj.o.emplace_back(key, obj_val);
                                            }
                                        }
                                    }
                                }
                            }

                            arr.a.push_back(obj);
                        } else {
                            // Simple scalar value
                            line_idx++;
                            arr.a.push_back(parse_value(item_content));
                        }

                        if (line_idx > start_idx + 10000) {
                            throw std::runtime_error("YAML parser: infinite loop in array");
                        }
                    }

                    return arr;
                }

                JVal parse() {
                    return parse_object(0);
                }
            };

            auto get_obj = [](const JVal& v)->const std::vector<std::pair<std::string,JVal>>& {
                if (v.t!=JVal::OBJ) {
                    std::string type_name = (v.t==JVal::NUL)?"null":(v.t==JVal::NUM)?"number":(v.t==JVal::STR)?"string":(v.t==JVal::BOOL)?"boolean":(v.t==JVal::ARR)?"array":"unknown";
                    throw std::runtime_error("Expected object, got " + type_name + " with " + std::to_string(v.o.size()) + " pairs");
                }
                return v.o;
            };
            auto get_arr = [](const JVal& v)->const std::vector<JVal>& { if (v.t!=JVal::ARR) throw std::runtime_error("Expected array"); return v.a; };
            auto get_str = [](const JVal& v)->const std::string& { if (v.t==JVal::STR) return v.s; throw std::runtime_error("Expected string"); };
            auto get_bool = [](const JVal& v)->bool { if (v.t==JVal::BOOL) return v.b; throw std::runtime_error("Expected bool"); };
            auto get_int = [](const JVal& v)->long long { if (v.t==JVal::NUM) return v.i; if (v.t==JVal::STR){ const std::string& s=v.s; if (s.rfind("0x",0)==0||s.rfind("0X",0)==0){ return std::stoll(s, nullptr, 16);} return std::stoll(s, nullptr, 10);} throw std::runtime_error("Expected int"); };

            std::ifstream cf(from_yaml_path, std::ios::binary); if (!cf) { std::cerr<<"Failed to open config: "<<from_yaml_path<<"\n"; return 1; }
            std::string text((std::istreambuf_iterator<char>(cf)), std::istreambuf_iterator<char>());
            YAMLParser p(text);
            JVal root = p.parse();

            // defaults
            uint32_t cfg_max_size = 0x1000; bool has_checksum = true; uint8_t cfg_align = align_pow2; std::vector<JVal> cfg_entries;
            // extract
            auto robj = get_obj(root);
            for (auto& kv : robj) {
                if (kv.first == "max_size") cfg_max_size = static_cast<uint32_t>(get_int(kv.second));
                else if (kv.first == "has_checksum") has_checksum = get_bool(kv.second);
                else if (kv.first == "alignment") cfg_align = static_cast<uint8_t>(get_int(kv.second));
                else if (kv.first == "entries") cfg_entries = get_arr(kv.second);
            }
            uint32_t cfg_flags = has_checksum ? 0x1u : 0u;
            tlc::TransferList tl(cfg_max_size, cfg_flags, cfg_align);

            auto tag_name_to_id = [](const std::string& name)->uint32_t{
                if (name=="empty") return 0;
                if (name=="fdt") return 1;
                if (name=="hob_block") return 2;
                if (name=="hob_list") return 3;
                if (name=="acpi_table_aggregate") return 4;
                if (name=="tpm_event_log_table") return 5;
                if (name=="tpm_crb_base_address_table") return 6;
                if (name=="optee_pageable_part") return 0x100;
                if (name=="dt_spmc_manifest") return 0x101;
                if (name=="exec_ep_info") return 0x102;
                if (name=="sram_layout") return 0x104;
                throw std::runtime_error("Unknown tag name: "+name);
            };

            auto pack_le = [](std::vector<uint8_t>& v, uint64_t val, int bytes){ for (int i=0;i<bytes;++i){ v.push_back(static_cast<uint8_t>((val >> (8*i)) & 0xFF)); } };
            auto pack_format = [&](const std::string& fmt, const std::vector<uint64_t>& values){
                std::vector<uint8_t> out;
                size_t vi=0;
                for (size_t i=0;i<fmt.size();){
                    char c=fmt[i];
                    if (c>='0'&&c<='9'){ // repeat count
                        int n=0;
                        while(i<fmt.size() && fmt[i]>='0'&&fmt[i]<='9'){
                            n = n*10 + (fmt[i]-'0');
                            ++i;
                        }
                        if (i>=fmt.size()) break;
                        char t=fmt[i++];
                        if (t=='x'){
                            out.insert(out.end(), n, 0);
                        } else {
                            for(int k=0;k<n;++k){
                                if (vi>=values.size()) throw std::runtime_error("Not enough values for format");
                                if (t=='Q') pack_le(out, values[vi++], 8);
                                else if (t=='I') pack_le(out, values[vi++], 4);
                                else if (t=='H') pack_le(out, values[vi++], 2);
                                else if (t=='B') pack_le(out, values[vi++], 1);
                                else throw std::runtime_error("Unsupported fmt char");
                            }
                        }
                    } else {
                        ++i;
                        if (c=='x'){
                            out.push_back(0);
                        } else {
                            if (vi>=values.size()) throw std::runtime_error("Not enough values for format");
                            if (c=='Q') pack_le(out, values[vi++], 8);
                            else if (c=='I') pack_le(out, values[vi++], 4);
                            else if (c=='H') pack_le(out, values[vi++], 2);
                            else if (c=='B') pack_le(out, values[vi++], 1);
                            else throw std::runtime_error("Unsupported fmt char");
                        }
                    }
                }
                return out;
            };

            auto get_obj_field = [&](const std::vector<std::pair<std::string,JVal>>& o, const char* key)->const JVal&{ for (auto& kv : o) if (kv.first==key) return kv.second; throw std::runtime_error(std::string("Missing field: ")+key); };

            for (const auto& je : cfg_entries) {
                auto eo = get_obj(je);
                // tag id can be name or number
                const JVal& tid = get_obj_field(eo, "tag_id");
                uint32_t tag_id = 0;
                if (tid.t == JVal::STR) tag_id = tag_name_to_id(tid.s); else tag_id = static_cast<uint32_t>(get_int(tid));
                // optional per-entry alignment
                uint8_t ent_align = cfg_align; for (auto& kv : eo) if (kv.first=="alignment") { ent_align = static_cast<uint8_t>(get_int(kv.second)); break; }

                auto add_blob = [&](const std::string& path){ std::ifstream ifs(path, std::ios::binary); if (!ifs) throw std::runtime_error("Failed to open "+path); std::vector<uint8_t> buf((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>()); tl.add_transfer_entry(tag_id, buf, ent_align); };

                bool handled=false;
                // blob_file_path
                for (auto& kv : eo) if (kv.first=="blob_file_path") { add_blob(get_str(kv.second)); handled=true; break; }
                if (handled) continue;

                // specific formats
                if (tag_id == 5) { // tpm_event_log_table
                    // fields: event_log (file), flags (u32)
                    const auto& flags_v = get_obj_field(eo, "flags");
                    const auto& elog_v = get_obj_field(eo, "event_log");
                    uint32_t fl = static_cast<uint32_t>(get_int(flags_v));
                    std::ifstream ifs(get_str(elog_v), std::ios::binary); if (!ifs) throw std::runtime_error("Failed to open event_log");
                    std::vector<uint8_t> data; pack_le(data, fl, 4); data.insert(data.end(), std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
                    tl.add_transfer_entry(tag_id, data, ent_align); handled=true;
                } else if (tag_id == 0x102) { // exec_ep_info
                    const auto& ep_info = get_obj_field(eo, "ep_info"); auto epi = get_obj(ep_info);
                    const auto& h = get_obj_field(epi, "h"); auto ho = get_obj(h);
                    uint64_t type = static_cast<uint64_t>(get_int(get_obj_field(ho, "type")));
                    uint64_t version = static_cast<uint64_t>(get_int(get_obj_field(ho, "version")));
                    uint64_t entry_point_size = 88;
                    // attr may be string of flags "EP_NON_SECURE|..."
                    auto attr_v = get_obj_field(ho, "attr"); uint64_t attr = 0;
                    if (attr_v.t == JVal::STR) {
                        std::string s = attr_v.s; // split by |
                        auto add_flag = [&](const std::string& f){ if (f=="EP_SECURE") attr|=0x0; else if (f=="EP_NON_SECURE") attr|=0x1; else if (f=="EP_REALM") attr|=0x21; else if (f=="EP_EE_LITTLE") attr|=0x0; else if (f=="EP_EE_BIG") attr|=0x2; else if (f=="EP_ST_DISABLE") attr|=0x0; else if (f=="EP_ST_ENABLE") attr|=0x4; else if (f=="EP_NON_EXECUTABLE") attr|=0x0; else if (f=="EP_EXECUTABLE") attr|=0x8; else if (f=="EP_FIRST_EXE") attr|=0x10; else throw std::runtime_error("Unknown EP flag: "+f); };
                        size_t start=0; while (start<=s.size()) { size_t bar = s.find('|', start); std::string token = s.substr(start, bar==std::string::npos? std::string::npos : bar-start); // trim spaces
                            size_t l=0; while(l<token.size() && token[l]==' ') ++l; size_t r=token.size(); while(r>l && token[r-1]==' ') --r; if (r>l) add_flag(token.substr(l, r-l)); if (bar==std::string::npos) break; start = bar+1; }
                    } else { attr = static_cast<uint64_t>(get_int(attr_v)); }
                    uint64_t pc = static_cast<uint64_t>(get_int(get_obj_field(epi, "pc")));
                    uint64_t spsr = static_cast<uint64_t>(get_int(get_obj_field(epi, "spsr")));
                    std::vector<uint64_t> args;
                    const auto& av = get_obj_field(epi, "args"); for (const auto& x : get_arr(av)) args.push_back(static_cast<uint64_t>(get_int(x)));
                    while (args.size() < 8) args.push_back(0);
                    std::vector<uint64_t> vals = {type, version, entry_point_size, attr, pc, spsr}; vals.insert(vals.end(), args.begin(), args.begin()+8);
                    auto data = pack_format("2BHIQI4x8Q", vals);
                    tl.add_transfer_entry(tag_id, data, ent_align); handled=true;
                } else if (tag_id == 6) { // tpm_crb_base_address_table
                    uint64_t crb_base = static_cast<uint64_t>(get_int(get_obj_field(eo, "crb_base_address")));
                    uint64_t crb_size = static_cast<uint64_t>(get_int(get_obj_field(eo, "crb_size")));
                    auto data = pack_format("QI", {crb_base, crb_size}); tl.add_transfer_entry(tag_id, data, ent_align); handled=true;
                } else if (tag_id == 0x104) { // sram_layout
                    uint64_t addr = static_cast<uint64_t>(get_int(get_obj_field(eo, "addr")));
                    uint64_t size = static_cast<uint64_t>(get_int(get_obj_field(eo, "size")));
                    auto data = pack_format("2Q", {addr, size}); tl.add_transfer_entry(tag_id, data, ent_align); handled=true;
                } else if (tag_id == 0x100) { // optee_pageable_part
                    uint64_t pp_addr = static_cast<uint64_t>(get_int(get_obj_field(eo, "pp_addr")));
                    auto data = pack_format("Q", {pp_addr}); tl.add_transfer_entry(tag_id, data, ent_align); handled=true;
                }

                if (!handled) {
                    throw std::runtime_error("Unsupported entry without blob_file_path or known format");
                }
            }

            std::ofstream ofs(output_path, std::ios::binary); if (!ofs) { std::cerr<<"Failed to open output: "<<output_path<<"\n"; return 1; }
            tl.write(ofs);
            std::cout << "tlc(create): wrote TL to "<<output_path<<" (used="<<tl.used_size<<")\n";
        } else {
            tlc::TransferList tl(max_size, flags, align_pow2);
            for (const auto& e : entries) {
                std::ifstream ifs(e.path, std::ios::binary);
                if (!ifs) { std::cerr << "Failed to open " << e.path << "\n"; return 1; }
                std::vector<uint8_t> buf((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                tl.add_transfer_entry(e.id, buf, align_pow2);
            }
            std::ofstream ofs(output_path, std::ios::binary);
            if (!ofs) { std::cerr << "Failed to open output: " << output_path << "\n"; return 1; }
            tl.write(ofs);
            std::cout << "tlc(create): wrote TL to " << output_path << " (used=" << tl.used_size << ")\n";
        }
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n"; return 1;
    }
    return 0;
}

struct ParsedTE { uint32_t id; uint8_t hdr_size; uint32_t data_size; uint32_t offset; std::vector<uint8_t> data; };
struct ParsedTL {
    uint32_t signature; uint8_t checksum; uint8_t version; uint8_t hdr_size; uint8_t alignment;
    uint32_t used_size; uint32_t total_size; uint32_t flags; uint32_t reserved;
    std::vector<ParsedTE> entries;
};

static bool read_exact(std::istream& is, void* buf, std::size_t n) {
    is.read(static_cast<char*>(buf), static_cast<std::streamsize>(n));
    return is.good();
}

static ParsedTL parse_tl_from_file(const std::string& path) {
    std::ifstream is(path, std::ios::binary);
    if (!is) throw std::runtime_error("Failed to open TL: " + path);
    ParsedTL tl{};
    // Header <I4B4I
    if (!read_exact(is, &tl.signature, 4)) throw std::runtime_error("Short read");
    if (!read_exact(is, &tl.checksum, 1)) throw std::runtime_error("Short read");
    if (!read_exact(is, &tl.version, 1)) throw std::runtime_error("Short read");
    if (!read_exact(is, &tl.hdr_size, 1)) throw std::runtime_error("Short read");
    if (!read_exact(is, &tl.alignment, 1)) throw std::runtime_error("Short read");
    if (!read_exact(is, &tl.used_size, 4)) throw std::runtime_error("Short read");
    if (!read_exact(is, &tl.total_size, 4)) throw std::runtime_error("Short read");
    if (!read_exact(is, &tl.flags, 4)) throw std::runtime_error("Short read");
    if (!read_exact(is, &tl.reserved, 4)) throw std::runtime_error("Short read");

    if (tl.signature != tlc::TransferList::SIGNATURE) throw std::runtime_error("Invalid TL signature");
    if (tl.version == 0) throw std::runtime_error("Invalid TL version");

    // Read entries until we reach used_size
    std::size_t cursor = tl.hdr_size;
    while (cursor < tl.used_size) {
        ParsedTE te{};
        te.offset = static_cast<uint32_t>(is.tellg());
        uint8_t id_bytes[3];
        if (!read_exact(is, id_bytes, 3)) throw std::runtime_error("Short read id");
        te.id = static_cast<uint32_t>(id_bytes[0]) | (static_cast<uint32_t>(id_bytes[1]) << 8) | (static_cast<uint32_t>(id_bytes[2]) << 16);
        if (!read_exact(is, &te.hdr_size, 1)) throw std::runtime_error("Short read hdr_size");
        if (!read_exact(is, &te.data_size, 4)) throw std::runtime_error("Short read data_size");
        te.data.resize(te.data_size);
        if (te.data_size && !read_exact(is, te.data.data(), te.data.size())) throw std::runtime_error("Short read data");
        tl.entries.push_back(te);
        cursor += 8 + te.data.size();
        std::size_t pad_to = tlc::align_up(cursor, tlc::TransferList::GRANULE);
        std::size_t pad = pad_to - cursor;
        if (pad) is.seekg(static_cast<std::streamoff>(pad), std::ios::cur);
        cursor = pad_to;
    }
    return tl;
}

static void print_header(const ParsedTL& tl) {
    auto hex32 = [](uint32_t v){ std::ostringstream oss; oss << std::hex << "0x" << v; return oss.str(); };
    std::cout << "signature " << hex32(tl.signature) << "\n";
    std::cout << "checksum  " << hex32(tl.checksum) << "\n";
    std::cout << "version   " << hex32(tl.version) << "\n";
    std::cout << "hdr_size  " << hex32(tl.hdr_size) << "\n";
    std::cout << "alignment " << hex32(tl.alignment) << "\n";
    std::cout << "size      " << hex32(tl.used_size) << "\n";
    std::cout << "total_size " << hex32(tl.total_size) << "\n";
    std::cout << "flags     " << hex32(tl.flags) << "\n";
    std::cout << "reserved  " << hex32(tl.reserved) << "\n";
}

static void print_entries(const ParsedTL& tl) {
    auto hex32 = [](uint32_t v){ std::ostringstream oss; oss.setf(std::ios::hex, std::ios::basefield); oss<<"0x"<<std::nouppercase<<std::hex<<v; return oss.str(); };
    for (std::size_t i=0;i<tl.entries.size();++i) {
        const auto& te = tl.entries[i];
        std::cout << "id        " << hex32(te.id) << "\n";
        std::cout << "data_size " << hex32(te.data_size) << "\n";
        std::cout << "hdr_size  " << hex32(te.hdr_size) << "\n";
        std::cout << "offset    " << hex32(te.offset) << "\n";
        if (i+1 < tl.entries.size()) std::cout << "----\n";
    }
}

static uint32_t calc_sum_mod256_first_n(const std::string& path, uint32_t n) {
    std::ifstream is(path, std::ios::binary);
    if (!is) throw std::runtime_error("Failed to open: "+path);
    uint32_t sum=0; char buf[4096];
    uint32_t remaining = n;
    while (remaining) {
        std::size_t to_read = std::min<uint32_t>(remaining, sizeof(buf));
        is.read(buf, static_cast<std::streamsize>(to_read));
        std::size_t got = static_cast<std::size_t>(is.gcount());
        if (got == 0) break;
        for (std::size_t i=0;i<got;++i) sum = (sum + static_cast<unsigned char>(buf[i])) & 0xFF;
        remaining -= static_cast<uint32_t>(got);
    }
    return sum & 0xFF;
}

static int cmd_info(int argc, char** argv) {
    if (argc < 3) { std::cerr << "Usage: tlc info [--fdt-offset|--header|--entries] <file>\n"; return 1; }
    bool opt_fdt_offset=false, opt_header=false, opt_entries=false; std::string file;
    int i=2; while (i<argc) {
        std::string a=argv[i];
        if (a=="--fdt-offset") opt_fdt_offset=true;
        else if (a=="--header") opt_header=true;
        else if (a=="--entries") opt_entries=true;
        else if (!a.empty() && a[0]=='-') { std::cerr << "Unknown option: "<<a<<"\n"; return 1; }
        else { file=a; }
        ++i;
    }
    if (file.empty()) { std::cerr << "Missing file\n"; return 1; }
    auto tl = parse_tl_from_file(file);
    if (opt_fdt_offset) {
        for (const auto& te : tl.entries) if (te.id==1) { std::cout << (te.offset + te.hdr_size) << "\n"; return 0; }
        std::cerr << "No FDT tag (1) present\n"; return 1;
    }
    if ((opt_header && opt_entries) || (!opt_header && !opt_entries)) {
        print_header(tl); if (!tl.entries.empty()) { std::cout << "----\n"; print_entries(tl);} return 0;
    } else if (opt_header) { print_header(tl); return 0; }
    else { print_entries(tl); return 0; }
}

static int cmd_validate(int argc, char** argv) {
    if (argc < 3) { std::cerr << "Usage: tlc validate <file>\n"; return 1; }
    std::string file = argv[2];
    auto tl = parse_tl_from_file(file);
    uint32_t sum = calc_sum_mod256_first_n(file, tl.used_size);
    if ((sum & 0xFF) == 0) { std::cout << "Valid TL!\n"; return 0; }
    std::cerr << "Invalid checksum\n"; return 1;
}

static tlc::TransferList rebuild_from_parsed(const ParsedTL& pt) {
    tlc::TransferList tl(pt.total_size, pt.flags, pt.alignment);
    for (const auto& te : pt.entries) {
        tl.add_transfer_entry(te.id, te.data, pt.alignment);
    }
    return tl;
}

static int cmd_remove(int argc, char** argv) {
    if (argc < 3) { std::cerr << "Usage: tlc remove --tags <id> [--tags <id>...] <file>\n"; return 1; }
    std::vector<uint32_t> tags; std::string file;
    int i=2; while (i<argc) {
        std::string a=argv[i];
        if (a=="--tags") {
            if (i+1>=argc) { std::cerr << "--tags needs a value\n"; return 1; }
            char* e=nullptr; long v = std::strtol(argv[++i], &e, 0);
            if (e==argv[i] || v<0 || v>0xFFFFFF) { std::cerr << "Invalid tag\n"; return 1; }
            tags.push_back(static_cast<uint32_t>(v));
        } else if (!a.empty() && a[0]=='-') { std::cerr << "Unknown option: "<<a<<"\n"; return 1; }
        else { file=a; }
        ++i;
    }
    if (file.empty()) { std::cerr << "Missing file\n"; return 1; }
    auto pt = parse_tl_from_file(file);
    // filter
    std::vector<ParsedTE> kept;
    for (const auto& te : pt.entries) {
        bool drop=false; for (auto id: tags) if (te.id==id) { drop=true; break; }
        if (!drop) kept.push_back(te);
    }
    pt.entries.swap(kept);
    auto tl = rebuild_from_parsed(pt);
    std::ofstream os(file, std::ios::binary);
    if (!os) { std::cerr << "Failed to open output\n"; return 1; }
    tl.write(os);
    return 0;
}

static int cmd_add(int argc, char** argv) {
    if (argc < 3) { std::cerr << "Usage: tlc add [--align <n>] --entry <id> <path> [...] <file>\n"; return 1; }
    uint8_t align_pow2 = 0; std::string file; struct E{uint32_t id; std::string p;}; std::vector<E> ents;
    int i=2; while (i<argc) {
        std::string a=argv[i];
        if (a=="--align" || a=="-a") { if (i+1>=argc) { std::cerr<<"Missing value after "<<a<<"\n"; return 1;} char* e=nullptr; long v=std::strtol(argv[++i], &e, 0); if (e==argv[i]||v<0||v>31){std::cerr<<"Invalid align\n"; return 1;} align_pow2=static_cast<uint8_t>(v);}
        else if (a=="--entry") { if (i+2>=argc) { std::cerr<<"--entry needs <id> <path>\n"; return 1;} char* e=nullptr; long idv=std::strtol(argv[++i], &e, 0); if (e==argv[i]||idv<0||idv>0xFFFFFF){std::cerr<<"Invalid id\n"; return 1;} ents.push_back({static_cast<uint32_t>(idv), std::string(argv[++i])}); }
        else if (!a.empty() && a[0]=='-') { std::cerr<<"Unknown option: "<<a<<"\n"; return 1; }
        else { file=a; }
        ++i;
    }
    if (file.empty()) { std::cerr<<"Missing file\n"; return 1; }
    auto pt = parse_tl_from_file(file);
    auto tl = rebuild_from_parsed(pt);
    for (const auto& e : ents) {
        std::ifstream ifs(e.p, std::ios::binary); if (!ifs) { std::cerr<<"Failed to open "<<e.p<<"\n"; return 1; }
        std::vector<uint8_t> buf((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        tl.add_transfer_entry(e.id, buf, align_pow2);
    }
    std::ofstream os(file, std::ios::binary); if (!os) { std::cerr<<"Failed to open output\n"; return 1; }
    tl.write(os);
    return 0;
}

static int cmd_unpack(int argc, char** argv) {
    if (argc < 3) { std::cerr << "Usage: tlc unpack [-C <dir>] <file>\n"; return 1; }
    std::string dir = "."; std::string file; int i=2; while (i<argc) { std::string a=argv[i]; if (a=="-C") { if (i+1>=argc){std::cerr<<"-C needs value\n"; return 1;} dir=argv[++i]; } else if (!a.empty() && a[0]=='-') { std::cerr<<"Unknown option: "<<a<<"\n"; return 1;} else { file=a; } ++i; }
    if (file.empty()) { std::cerr<<"Missing file\n"; return 1; }
    auto tl = parse_tl_from_file(file);
    // Write files te_<i>_<id>.bin
    for (std::size_t idx=0; idx<tl.entries.size(); ++idx) {
        const auto& te = tl.entries[idx];
        std::ostringstream name; name << dir << "/te_" << idx << "_" << te.id << ".bin";
        std::ofstream os(name.str(), std::ios::binary);
        if (!os) { std::cerr << "Failed to open "<<name.str()<<"\n"; return 1; }
        if (!te.data.empty()) os.write(reinterpret_cast<const char*>(te.data.data()), static_cast<std::streamsize>(te.data.size()));
    }
    return 0;
}

static int cmd_gen_header(int argc, char** argv) {
    if (argc < 3) { std::cerr << "Usage: tlc gen_header [--output|-O <file>] <tl_file>\n"; return 1; }
    std::string out = "header.h"; std::string file; int i=2; while (i<argc){ std::string a=argv[i]; if (a=="--output"||a=="-O"){ if (i+1>=argc){ std::cerr<<"Missing value after "<<a<<"\n"; return 1;} out=argv[++i]; } else if (!a.empty() && a[0]=='-'){ std::cerr<<"Unknown option: "<<a<<"\n"; return 1;} else { file=a; } ++i; }
    if (file.empty()) { std::cerr<<"Missing tl_file\n"; return 1; }
    auto tl = parse_tl_from_file(file);
    uint32_t dtb_offset = 0; for (const auto& te: tl.entries) if (te.id==1) { dtb_offset = te.offset + te.hdr_size; break; }
    // Generate a simple header with key defs
    auto guard = out; std::replace(guard.begin(), guard.end(), '.', '_'); std::transform(guard.begin(), guard.end(), guard.begin(), [](unsigned char c){ return static_cast<char>(std::toupper(c)); });
    std::ofstream os(out);
    if (!os) { std::cerr<<"Failed to open output: "<<out<<"\n"; return 1; }
    os << "#ifndef "<<guard<<"\n";
    os << "#define "<<guard<<"\n\n";
    os << "#define TL_SIGNATURE 0x"<< std::hex << tl.signature << "\n";
    os << "#define TL_VERSION  0x"<< std::hex << static_cast<unsigned>(tl.version) << "\n";
    os << "#define TL_ALIGNMENT 0x"<< std::hex << static_cast<unsigned>(tl.alignment) << "\n";
    os << "#define TL_USED_SIZE 0x"<< std::hex << tl.used_size << "\n";
    os << "#define TL_TOTAL_SIZE 0x"<< std::hex << tl.total_size << "\n";
    os << "#define TL_FLAGS 0x"<< std::hex << tl.flags << "\n";
    os << "#define TL_DTB_OFFSET 0x"<< std::hex << dtb_offset << "\n\n";
    os << "#endif /* "<<guard<<" */\n";
    return 0;
}

int create_tlbin(int argc, char** argv) {
    if (argc < 2) { print_usage(); return 1; }
    std::string cmd = argv[1];
    if (cmd == "create") return cmd_create(argc, argv);
    if (cmd == "info") return cmd_info(argc, argv);
    if (cmd == "validate") return cmd_validate(argc, argv);
    if (cmd == "remove") return cmd_remove(argc, argv);
    if (cmd == "add") return cmd_add(argc, argv);
    if (cmd == "unpack") return cmd_unpack(argc, argv);
    if (cmd == "gen_header") return cmd_gen_header(argc, argv);
    std::cerr << "Unsupported command: "<<cmd<<"\n"; print_usage(); return 1;
}
