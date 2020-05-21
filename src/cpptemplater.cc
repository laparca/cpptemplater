//#include <fstream>
#include <iostream>
#include <cpptemplater.hh>

template<class... Args>
constexpr bool is_any_of(std::string_view const& arg, Args&&... values) {
    return ((arg == values) || ... || false);
}

template<class OStream>
void prepare_top(OStream& out, const templater_parameters& params) {
    for(auto& include : params.includes) {
        out << "#include <" << include << ">" << std::endl;
    }

    if (params.result_namespace) {
        out << "namespace " << params.result_namespace.value() << " {" << std::endl;
    }

    if (params.class_name) {
        out << "class " << params.class_name.value();
        if (params.parent_class_name) {
            out << " : public " << params.parent_class_name.value();
        }

        out << " {" << std::endl;
        out << "public:" << std::endl;
    }

    out << "template<class OStream> OStream& " << params.function_name << "(OStream& " << params.result_name << ") ";
    if (params.function_override)
        out << "override ";
    out << "{" << std::endl;
}

template<class OStream>
void prepare_bottom(OStream& out, const templater_parameters& params) {
    out << "return " << params.result_name << ";" << std::endl;
    out << "}" << std::endl;

    if (params.class_name) {
        out << "};" << std::endl;
    }

    if (params.result_namespace) {
        out << "}" << std::endl;
    }
}

#include <cctype>
bool is_valid_block_indicator(char ch) {
    return ch == 'b' || ch == 'e' || ch == '!' || std::isspace(ch);
}

template<class IStream, class OStream>
bool parse_template(IStream& input, OStream& out, const templater_parameters& params, parser_status& status) {
    using traits_type = typename IStream::traits_type;
    using char_type = typename IStream::char_type;
    block_type_t block_type;
    auto ch = input.get();

    if (ch == traits_type::eof() || ch != '%') {
        if (!input.eof())
            input.unget();
        return false;
    }
   
    ch = input.get();
    if (!is_valid_block_indicator(ch)) {
        /* error */
    }
    if ((ch == 'b' && status != START) || (std::isspace(ch) && status == END) || (ch == '!' && status == END)) {
        /* error */
    }

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
