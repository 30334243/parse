#ifndef PARSE_SCRIPT_INTERFACE_HPP
#define PARSE_SCRIPT_INTERFACE_HPP

#include <string>
#include <vector>
#include <functional>

namespace Parse {
	// CONSTANTS
	static std::string const kExec{"exec("};
	static std::string const kExecEnd{");"};
	static std::string const kFilterMask{",["};
	static std::string const kHex{"0x"};
	static std::string const kBin{"0b"};
	static std::string const kInsert{"insert{"};
	static size_t const kMaxChar{(size_t)256};
	static size_t const kMaxShort{(size_t)(int16_t)-1};
	static size_t const kMaxInt{(size_t)(int32_t)-1};
	static size_t const kMaxLong{(size_t)(uint64_t)-1};
	static uint8_t const kHexBase{16};
	static uint8_t const kBinBase{2};
	static uint8_t const kDecBase{10};
	std::vector<std::string> const kNameFuncs{
		"crop{","cropb{",
			"shr{","shrb{",
			"shl{","shlb{",
			"insert{[",
			"replace{[",
			"mask{","!mask{",
			"filter{","!filter{",
			"eq{[","!eq{[",
	};
	std::vector<std::string> const kEditableFuncs{
		"cropb{",
			"shrb{",
			"shlb{",
	};
	// MEMBERS
	static uint64_t lid{};
	static uint8_t offset{};
	// USING
	using ItStr = std::string::iterator;
	using Func = std::function<void(std::string const&,ItStr&,ItStr&,uint8_t&)>;
};
#endif 
