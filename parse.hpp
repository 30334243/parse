#ifndef PARSE_HPP
#define PARSE_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <iterator>
#include <list>
#include <cmath>
#include "../shit_msvc13/shit_msvc13.hpp"

// PARSE
namespace Parse {
	// CONSTANTS
	static std::string const kCalc{"calc("};
	static std::string const kCalcEnd{");"};
	static std::string const kHex{"0x"};
	static std::string const kBin{"0b"};
	static uint8_t const kHexBase{16};
	static uint8_t const kBinBase{2};
	static uint8_t const kDecBase{10};
	static uint8_t const kSzChar{1};
	static uint8_t const kSzShort{2};
	static uint8_t const kSzInt{4};
	static uint8_t const kSzLong{8};
	static uint64_t lid{};
	static uint8_t offset{};
	std::vector<std::string> const kNameFuncs{
		"crop{","shr{","shl{","mask{","filter{"
	};
	// MEMBERS
	static std::vector<std::function<bool(uint8_t**,uint8_t**)>> funcs{};
	// USING
	using ItStr = std::string::iterator;
	using Poly = std::tuple<uint8_t,uint16_t,uint32_t,uint64_t>;
	// CROP ARGUMENTS
	static std::tuple<size_t,size_t> CropArgs(ItStr& beg, ItStr& end) {
		std::tuple<size_t,size_t> ret{};
		if (std::count(beg, end, ',') == 1) {
			std::string const str_beg(beg, std::find(beg, end, ','));
			size_t const arg_beg{std::stoul(str_beg)};
			std::string const str_end(beg + str_beg.size() + 1, end);
			size_t const arg_end{std::stoul(str_end)};
			ret = std::make_tuple(arg_beg, arg_end);
			beg += str_beg.size() + str_end.size() + 1;
		} else {
			beg = end;
		}
		return ret;	
	}
	// FILTER
	static std::list<Shit::Func> Filter(ItStr& beg, ItStr& end) {
		std::list<Shit::Func> ret{};
		return ret;
	}
	// CROP
	static std::list<Shit::Func> Crop(ItStr& beg, ItStr& end) {
		std::list<Shit::Func> ret{};
		auto args = CropArgs(beg, end);
		if (0 < std::tuple_size<decltype(args)>::value) {
			ret.emplace_back(Shit::Check::OutOfRange());
			ret.emplace_back(Shit::Crop(std::get<0>(args), std::get<1>(args)));
		} else {
			std::cout << "invalid arguments: " + std::string(beg,end) << std::endl;;
			beg = end;
		}
		return ret;
	}
	// GET SIZE TYPE
	static uint8_t GetSizeType(ItStr& beg, ItStr& end, uint8_t const base) {
		uint8_t ret{};
		size_t const num_chars{(size_t)std::distance(beg, end)};
		size_t const sz{base == kBinBase ?
			(size_t)std::ceil((double)num_chars / 8) :
				num_chars};
		if (sz == kSzChar) {
			ret = kSzChar;
		} else if (sz <= kSzShort) {
			ret = kSzShort;
		} else if (sz <= kSzInt) {
			ret = kSzInt;
		} else if (sz <= kSzLong) {
			ret = kSzLong;
		} else {
			std::cout << "big size: " + std::to_string(num_chars) << std::endl;
		}
		return ret;
	}
	// CONVERT
	static Poly Convert(ItStr& beg, ItStr& end, uint8_t const base) {
		Poly ret{};
		uint8_t const sz_type{GetSizeType(beg, end, base)};
		if (sz_type == kSzChar) {
			std::get<0>(ret) = std::stoul(std::string(beg, end), nullptr, base);
		} else if (sz_type == kSzShort) {
			std::get<1>(ret) = std::stoul(std::string(beg, end), nullptr, base);
		} else if (sz_type == kSzInt) {
			std::get<2>(ret) = std::stoul(std::string(beg, end), nullptr, base);
		} else if (sz_type == kSzLong) {
			std::get<3>(ret) = std::stoul(std::string(beg, end), nullptr, base);
		} else {
			std::cout << "unknown base: " << std::endl;
		}
		return ret;
	}
	// GET BASE
	static uint8_t GetBase(ItStr& beg, ItStr& end) {
		uint8_t ret{};
		if (std::search(beg, beg+2, kHex.cbegin(), kHex.cend()) != beg+2) {
			ret = kHexBase;
		} else if (std::search(beg, beg+2, kBin.cbegin(), kBin.cend()) != beg+2) {
			ret = kBinBase;
		} else {
			ret = kDecBase;
		}
		return ret;
	}
	// MASK ARGUMENT
	static Poly MaskArg(ItStr& beg, ItStr& end) {
		Poly ret{};
		if (std::count(beg, end, ',') == 0) {
			uint8_t const base{GetBase(beg, end)};
			beg += base == kDecBase ? 0 : 2;
			ret = Convert(beg, end, base);
		} else {
			std::cout << "error mask args: " + std::string(beg, end) << std::endl;
		}
		return ret;
	}
	// MASK
	static std::list<Shit::Func> Mask(ItStr& beg, ItStr& end) {
		std::list<Shit::Func> ret{};
		uint8_t sz8{};
		uint16_t sz16{};
		uint32_t sz32{};
		uint64_t sz64{};
		std::tie(sz8,sz16,sz32,sz64) = MaskArg(beg, end);
		if (sz8) {
			ret.emplace_back(Shit::Check::OutOfRange());
			ret.emplace_back(Shit::Mask(sz8,
												 Shit::SaveToLid<uint8_t>(lid, offset)));
		} else if (sz16) {
			ret.emplace_back(Shit::Check::OutOfRange());
			ret.emplace_back(Shit::Mask(sz16,
												 Shit::SaveToLid<uint16_t>(lid, offset)));
		} else if (sz32) {
			ret.emplace_back(Shit::Check::OutOfRange());
			ret.emplace_back(Shit::Mask(sz32,
												 Shit::SaveToLid<uint32_t>(lid, offset)));
		} else if (sz64) {
			ret.emplace_back(Shit::Check::OutOfRange());
			ret.emplace_back(Shit::Mask(sz64,
												 Shit::SaveToLid<uint64_t>(lid, offset)));
		} else {
			std::cout << "error ret value \"MaskArg\"" << std::endl;
		}
		return ret;
	}
	// GET COMMAND
	std::string GetCommand(std::string const& name, ItStr& beg, ItStr& end) {
		std::string ret{};
		auto it = std::search(beg, end,
								name.begin(), name.end());
		if (it < end) {
			beg = it;
			ret = std::string(beg, beg + name.size());
		}
		return ret;
	}
	// COMMAND
	std::list<Shit::Func> Command(ItStr& beg, ItStr& end) {
		std::list<Shit::Func> ret{};
		for (auto const& name : kNameFuncs) {
			std::string const command{GetCommand(name, beg, end)};
			if (command.empty()) {
				std::cout << "unknown command: " + std::string(beg,end) << std::endl;
				continue;
			}
			auto end_arg = std::find(beg, end, '}');
			beg += name.size();
			if (command == "crop{") {
				ret.splice(ret.cend(), Crop(beg, end_arg));
			} else if (command == "mask{") {
				ret.splice(ret.cend(), Mask(beg, end_arg));
			} else {
				ret.clear();
				ret.emplace_back(Shit::Empty());
				std::cout << "unknown command: " + command << std::endl;
				break;
			}
		}
		beg = end;
		return ret;
	}
	// CALCULATION
	std::list<Shit::Func> Calc(std::string str) {
		std::list<Shit::Func> ret{};
		auto rem = [] (char const c) {
			return c==' '||c=='\n'||c=='\r'||c=='\t'||c=='_'||c=='\'';
		};
		str.erase(std::remove_if(str.begin(), str.end(), rem), str.end());
		auto calc = std::search(str.begin(), str.end(),
										kCalc.begin(), kCalc.end());
		auto calc_end = calc + kCalc.size();
		auto end = std::search(calc, str.end(),
									  kCalcEnd.begin(), kCalcEnd.end());
		while (calc_end != end) {
			ret.splice(ret.end(), Command(calc_end, end));
		}
		return ret;
	}
}

#endif
