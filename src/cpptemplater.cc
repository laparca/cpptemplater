//#include <fstream>
#include <iostream>
#include <cpptemplater.hh>
#include <string_view>
#include <tuple>

template<class... Args>
constexpr bool is_any_of(std::string_view const& arg, Args&&... values) {
    return ((arg == values) || ... || false);
}

struct is_any_of_h {
    std::string_view const& arg_;

    constexpr is_any_of_h(std::string_view const& arg) : arg_(arg) {}

    template<class... Args>
    bool operator()(Args&&... values) {
        return is_any_of(arg_, std::forward<Args>(values)...);
    }
};

template<class Base, class Type, class... ValidValues>
struct option_t {
    std::tuple<ValidValues...> values_;
    Type Base::*placeholder;

    constexpr bool process(std::string_view const& arg) const {
        if (std::apply(is_any_of_h(arg), values_)) {
        }
        return true;
    }
};

int main(int argc, char** argv) {
    lzcoders::templater::templater_parameters params;

    for (int arg = 1; arg < argc; ++arg) {
        if (is_any_of(argv[arg], "--namespace", "-n")) {
            if (arg+1 >= argc) {
                std::cerr << "Parameter " << argv[arg] << " reqires one argument." << std::endl;
                return 1;
            }
            params.result_namespace = argv[++arg];
        }
        else if (is_any_of(argv[arg], "--class", "-c")) {
            if (arg+1 >= argc) {
                std::cerr << "Parameter " << argv[arg] << " reqires one argument." << std::endl;
                return 1;
            }
            params.class_name = argv[++arg];
        }
        else if (is_any_of(argv[arg], "--result_name", "-r")) {
            if (arg+1 >= argc) {
                std::cerr << "Parameter " << argv[arg] << " reqires one argument." << std::endl;
                return 1;
            }
            params.result_name = argv[++arg];
        }
        else if (is_any_of(argv[arg], "--parent_class", "-p")) {
            if (arg+1 >= argc) {
                std::cerr << "Parameter " << argv[arg] << " reqires one argument." << std::endl;
                return 1;
            }
            params.parent_class_name = argv[++arg];
        }
        else if (is_any_of(argv[arg], "--function_name", "-f")) {
            if (arg+1 >= argc) {
                std::cerr << "Parameter " << argv[arg] << " reqires one argument." << std::endl;
                return 1;
            }
            params.function_name = argv[++arg];
        }
        else if (is_any_of(argv[arg], "--include", "-i")) {
            if (arg+1 >= argc) {
                std::cerr << "Parameter " << argv[arg] << " reqires one argument." << std::endl;
                return 1;
            }
            params.includes.push_back(std::string(argv[++arg]));
        }

    }

    lzcoders::templater::parse_file(std::cin, std::cout, params);
    return 0;
}
