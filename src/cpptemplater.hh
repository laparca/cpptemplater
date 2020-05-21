#ifndef CPP_TEMPLATER_HH__
#define CPP_TEMPLATER_HH__

#include <optional>
#include <vector>
#include <cctype>
namespace lzcoders {
    namespace templater {
enum parser_status {
    START,
    NOT_PARSING,
    PARSING_FILE,
    PARSING_CODE,
    END
};

enum block_type_t {
    BEGIN_BLOCK,
    END_BLOCK,
    NORMAL_BLOCK,
    EXPRESSION_BLOCK
};

struct templater_parameters {
    std::optional<std::string> result_namespace;
    std::string result_name = std::string("result");
    bool function_override = false;
    std::optional<std::string> class_name;
    std::optional<std::string> parent_class_name;
    std::string function_name = std::string("render");
    std::vector<std::string> includes;
};

template<class IStream, class OnOk, class OnError>
bool read_one(IStream& input, OnOk on_ok, OnError on_error) {
    using traits_type = typename IStream::traits_type;
    using char_type = typename IStream::char_type;

    auto ch = input.get();
    if (ch == traits_type::eof()) return on_error(input);

    return on_ok(input, (char_type)ch);
}

template<class IStream, class OnOk, class OnError>
void until_eof_read_one(IStream& input, OnOk on_ok, OnError on_error) {
    while(read_one(input, on_ok, on_error))
        ;
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

    switch(ch) {
        case 'b': block_type = BEGIN_BLOCK; break;
        case 'e': block_type = END_BLOCK; break;
        case '!': block_type = EXPRESSION_BLOCK; break;
        default: block_type = NORMAL_BLOCK; break;
    }

    while ((ch = input.get()) != traits_type::eof()) {
        if (ch == '%') {
            auto ch_next = input.get();
            if (ch_next != traits_type::eof()) {
                if (ch_next == '>')
                    break;
                else
                    input.unget();
            }
            else {
                break;
            }
        }

        if (status == START && block_type != BEGIN_BLOCK) 
            prepare_top(out, params);

        if (status == PARSING_FILE) {
            if (block_type == EXPRESSION_BLOCK)
                out << "\" << (";
            else {
                out << "\";" << std::endl;
                if (block_type == END_BLOCK)
                    prepare_bottom(out, params);
            }
        }

        status = PARSING_CODE;

        out.put(ch);
    }

    switch(block_type) {
        case BEGIN_BLOCK:
            status = START;
            break;
        case END_BLOCK:
            status = END;
            break;
        case EXPRESSION_BLOCK:
            status = PARSING_FILE;
            out << ") << \"";
            break;
        case NORMAL_BLOCK:
            status = NOT_PARSING;
            break;
    }

    return true;
}

template<class IStream, class OStream>
void parse_file(IStream& input, OStream& out, const templater_parameters& params) {
    using traits_type = typename IStream::traits_type;
    using char_type = typename IStream::char_type;
    parser_status status = START;

    until_eof_read_one(input,
            [&out,&status,&params](IStream& input, char_type ch){
                if (ch != '<' || !parse_template(input, out, params, status)) {
                    if (status == START) {
                        prepare_top(out, params);
                    }
                    if (status == START || status == NOT_PARSING) {
                        out << "result << \"";
                    }
                    status = PARSING_FILE;

                    switch (ch) {
                    case '"':
                        out << "\\" << ch;
                        break;
                    case '\r':
                        out << "\\r";
                        break;
                    case '\n':
                        out << "\\n";
                        break;
                    case '\t':
                        out << "\\t";
                        break;
                    default:
                        out.put(ch);
                    }
                }

                return true;
            },
            [](IStream&){return false;});

    if (status == START)
        prepare_top(out, params);
    if (status != END) {
        if (status == PARSING_FILE) {
            out << "\";" << std::endl;
        }
        prepare_bottom(out, params);
    }
}
    }
}

#endif
