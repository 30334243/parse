#ifndef PARSE_HPP
#define PARSE_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <iterator>
#include <list>
#include "include/shit_msvc13/shit_msvc13.hpp"

namespace Parse {
	static std::string const kCalc{"calc("};
	static std::string const kCalcEnd{");"};
	std::vector<std::string> kNameFuncs{"crop{","shr{","shl{","mask{","filter{"};
	using ItStr = std::string::iterator;
	static std::vector<std::function<bool(uint8_t**,uint8_t**)>> funcs{};

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
	static std::list<Shit::Func> Crop(ItStr& beg, ItStr& end) {
		std::list<Shit::Func> ret{};
		auto args = CropArgs(beg, end);
		if (0 < std::tuple_size<decltype(args)>::value) {
			ret.emplace_back(Shit::Check::OutOfRange());
			ret.emplace_back(Shit::Crop(std::get<0>(args), std::get<1>(args)));
		} else {
			beg = end;
		}
		return ret;
	}
	static uint64_t Mask(ItStr& beg, ItStr& end) {
		uint64_t ret{};
		if (std::count(beg, end, ',') == 0) {

		}
		return ret;
	}
	std::string GetCommand(std::string const& name, ItStr& beg, ItStr& end) {
		std::string ret{};
		auto command_beg = std::search(beg, end,
												 name.begin(), name.end());
		if (command_beg != end) {
			auto command_end = command_beg + name.size();
			ret = std::string(command_beg, command_end);
		}
		return ret;
	}
	std::list<Shit::Func> Command(ItStr& beg, ItStr& end) {
			for (auto const& name : kNameFuncs) {
				std::string const command{GetCommand(name, beg, end)};
				if (command.empty()) {
					beg = end;
					break;
				}
				auto command_end = beg + command.size();
				auto end_arg = std::find(command_end, end, '}');
				if (command == "crop{") {
					 return Crop(command_end, end_arg);
				} else if (command == "mask{") {
					auto args = Mask(command_end, end_arg);
				}
			}
			return std::list<Shit::Func>{Shit::Empty()};
		}
	std::list<Shit::Func>
		Calc(std::string str) {
			std::list<Shit::Func> ret{};
			str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
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
