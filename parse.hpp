#ifndef PARSE_HPP
#define PARSE_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <iterator>
#include <list>
#include <cmath>
#include <algorithm>
#include <iostream>
#include "../shit_msvc13/shit_msvc13.hpp"

// PARSE
namespace Parse {
	// CONSTANTS
	static std::string const kExec{"exec("};
	static std::string const kExecEnd{");"};
	static std::string const kFilterMask{",["};
	static std::string const kHex{"0x"};
	static std::string const kBin{"0b"};
	static size_t const kMaxChar{(size_t)(int8_t)-1};
	static size_t const kMaxShort{(size_t)(int16_t)-1};
	static size_t const kMaxInt{(size_t)(int32_t)-1};
	static size_t const kMaxLong{(size_t)(uint64_t)-1};
	static uint8_t const kHexBase{16};
	static uint8_t const kBinBase{2};
	static uint8_t const kDecBase{10};
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
	using PolyVec = std::tuple<std::vector<uint8_t>,std::vector<uint16_t>,std::vector<uint32_t>,std::vector<uint64_t>>;
	using ItPair = std::pair<ItStr,ItStr>;
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
	// CONVERT
	static uint64_t Convert(ItStr& beg, ItStr& end) {
		uint8_t const base{GetBase(beg, end)};
		beg += base == kDecBase ? 0 : 2;
		uint64_t const sz{std::stoul(std::string(beg,end), nullptr, base)};
		return sz;
	}
	// SPLIT
	static std::vector<uint64_t> Split(ItStr& beg, ItStr& end, char const term) {
		std::vector<uint64_t> ret{};
		auto it = std::find(beg, end, term);
		while (it < end) {
			ret.emplace_back(Convert(beg, it));
			beg = ++it;
			it = std::find(it, end, term);
		}
		ret.emplace_back(Convert(beg, it));
		return ret;
	}
	// set
	static void Set(uint64_t const sz, std::list<Shit::Func>& funcs) {
		funcs.emplace_back(Shit::Check::OutOfRange());
		if (sz < kMaxChar) {
			funcs.emplace_back(Shit::Mask((uint8_t)sz,
													Shit::SaveToLid<uint8_t>(lid, offset)));
		} else if (sz < kMaxShort) {
			funcs.emplace_back(Shit::Mask((uint16_t)sz,
													Shit::SaveToLid<uint16_t>(lid, offset)));
		} else if (sz < kMaxInt) {
			funcs.emplace_back(Shit::Mask((uint32_t)sz,
													Shit::SaveToLid<uint32_t>(lid, offset)));
		} else if (sz < kMaxLong) {
			funcs.emplace_back(Shit::Mask((uint64_t)sz,
													Shit::SaveToLid<uint64_t>(lid, offset)));
		} else {
			std::cout << "unknown base: " << std::endl;
		}
	}
	// set
	static void SetFilter(uint64_t const sz, std::vector<uint64_t> const& args, std::list<Shit::Func>& funcs) {
		funcs.emplace_back(Shit::Check::OutOfRange());
		if (sz < kMaxChar) {
			funcs.emplace_back(Shit::Filter<uint8_t>(sz,
																  args,
																  Shit::SaveToLid<uint8_t>(lid, offset)));
		} else if (sz < kMaxShort) {
			funcs.emplace_back(Shit::Filter<uint16_t>(sz,
																	args,
																	Shit::SaveToLid<uint16_t>(lid, offset)));
		} else if (sz < kMaxInt) {
			funcs.emplace_back(Shit::Filter<uint32_t>(sz,
																	args,
																	Shit::SaveToLid<uint32_t>(lid, offset)));
		} else if (sz < kMaxLong) {
			funcs.emplace_back(Shit::Filter<uint64_t>(sz,
																	args,
																	Shit::SaveToLid<uint64_t>(lid, offset)));
		} else {
			std::cout << "unknown base: " << std::endl;
		}
	}
	// MASK ARGUMENT
	static auto Mask(std::list<Shit::Func>& funcs) -> std::function<void(std::string const,ItStr&,ItStr&)> {
		return [&funcs] (std::string const& name, ItStr& beg, ItStr& end) {
			if (name == "mask{") {
				if (beg != end && std::count(beg, end, ',') == 0) {
					uint64_t const sz{Convert(beg, end)};
					Set(sz, funcs);
				} else {
					std::cout << "error mask args: " + std::string(beg, end) << std::endl;
				}
			}
		};
	}
	// GET FILTER MASK
	static uint64_t GetFilterMask(ItStr& beg, ItStr& end) {
		uint64_t ret{};
		auto it = std::search(beg, end, kFilterMask.begin(), kFilterMask.end());
		if (it < end) {
			ret = Convert(beg, it);
			beg = it + kFilterMask.size();
		} else {
			std::cout << "filter arg mask invalid" << std::string{beg,end} << std::endl;
			beg = end;
		}
		return ret;
	}
	// FILTER ARGUMENT
	static std::vector<uint64_t> FilterArgs(ItStr& beg, ItStr& end) {
		std::vector<uint64_t> ret{};
		auto it = std::find(beg, end, ']');
		if (it < end && beg != it) {
			ret = Split(beg, it, ',');
		} else {
			std::cout << "filter args invalid: " << std::string{beg,end} << std::endl;
			beg = end;
		}
		return ret;
	}
	// FILTER
	static void Filter(ItStr& beg, ItStr& end, std::list<Shit::Func>& funcs) {
		uint64_t const mask{GetFilterMask(beg, end)};
		auto vec = FilterArgs(beg,end);
		SetFilter(mask, vec, funcs);
	}
	// FILTER
	static auto Filter(std::list<Shit::Func>& funcs) ->
		std::function<void(std::string const, ItStr&,ItStr&)> {
			return [&funcs] (std::string const& name, ItStr& beg, ItStr& end) {
				if (name == "filter{") {
					Filter(beg, end, funcs);
				}
			};
		}
	// SHIFT RIGHT ARGUMENT
	static size_t const ShrArg(ItStr& beg,ItStr& end) {
		size_t ret{};
		if (std::count(beg, end, ',') == 0) {
			ret = Convert(beg, end);
			beg += std::distance(beg, end);
		} else {
			std::cout << "invalid arguments: " + std::string(beg,end) << std::endl;
			beg = end;
		}
		return ret;
	}
	// SHIFT RIGHT
	static std::list<Shit::Func> Shr(ItStr& beg, ItStr& end) {
		std::list<Shit::Func> ret{};
		size_t const arg = ShrArg(beg, end);
		if (arg != 0) {
			ret.emplace_back(Shit::Check::OutOfRange());
			ret.emplace_back(Shit::Shr(arg));
		}
		return ret;
	}
	// SHIFT RIGHT
	static auto Shr(std::list<Shit::Func>& funcs) ->
		std::function<void(std::string const&,ItStr&,ItStr&)> {
			return [&funcs] (std::string const& name, ItStr& beg, ItStr& end) {
				if (name == "shr{") {
					funcs.splice(funcs.cend(), Shr(beg, end));
				}
			};
		}
	// CROP ARGUMENTS
	static std::tuple<size_t,size_t> CropArgs(ItStr& beg, ItStr& end) {
		std::tuple<size_t,size_t> ret{};
		if (std::count(beg, end, ',') == 1) {
			auto first = std::find(beg, end, ',');
			size_t const arg_beg{Convert(beg, first)};
			++first;
			size_t const arg_end{Convert(first, end)};
			ret = std::make_tuple(arg_beg, arg_end);
			beg += std::distance(beg, end);
		} else {
			beg = end;
		}
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
			std::cout << "invalid arguments: " + std::string(beg,end) << std::endl;
			beg = end;
		} return ret;
	}
	// CROP
	static auto Crop(std::list<Shit::Func>& funcs) ->
		std::function<void(std::string const&, ItStr&,ItStr&)> {
			return [&funcs] (std::string const& name, ItStr& beg, ItStr& end) {
				if (name == "crop{" && std::count(beg, end, ',') == 1) {
					funcs.splice(funcs.cend(), Crop(beg, end));
				}
			};
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
	// FOR TUPLE
	template <size_t I = 0, typename... Ts>
		typename std::enable_if<I == sizeof...(Ts), void>::type
		for_tuple(std::tuple<Ts...> tup, std::string const& name, ItStr& beg, ItStr& end) {
			return;
		}

	template <size_t I = 0, typename... Ts>
		typename std::enable_if<(I < sizeof...(Ts)), void>::type
		for_tuple(std::tuple<Ts...> tup, std::string const& name, ItStr& beg, ItStr& end) {
			std::get<I>(tup)(name, beg, end);
			for_tuple<I + 1>(tup, name, beg, end);
		}
	// COMMANDS
	template<class... ArgsTuple>
		static auto Commands(ArgsTuple... args_tuple) ->
		std::function<void(ItStr&,ItStr&)> {
			return [args_tuple...] (ItStr& beg, ItStr& end) {
				std::tuple<ArgsTuple...> ts{args_tuple...};
				for (auto const& name : kNameFuncs) {
					std::string const command{GetCommand(name, beg, end)};
					if (command.empty()) {
						continue;
					}
					auto end_arg = std::find(beg, end, '}');
					beg += name.size();
					for_tuple(ts, command, beg, end_arg);
				}
				beg = end;
			};
		}
	// REMOVE UNUSED CHARACTERS
	void RemoveUnusedChars(std::string& str) {
		auto rem = [] (char const c) {
			return c==' '||c=='\n'||c=='\r'||c=='\t'||c=='_'||c=='\'';
		};
		str.erase(std::remove_if(str.begin(), str.end(), rem), str.end());
	}
	// EXECUTION
	std::list<Shit::Func> Exec(std::string str) {
		std::list<Shit::Func> ret{};
		RemoveUnusedChars(str);
		auto commands = Commands(Crop(ret),
										 Shr(ret),
										 Filter(ret),
										 Mask(ret));
		auto exec = std::search(str.begin(), str.end(),
										kExec.begin(), kExec.end());
		if (exec != str.end()) {
			auto beg = exec + kExec.size();
			auto end = std::search(beg, str.end(),
										  kExecEnd.begin(), kExecEnd.end());
			while (beg != end) {
				commands(beg, end);
			}
		}
		return ret;
	}
}

#endif
