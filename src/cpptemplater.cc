//#include <fstream>
#include <iostream>

enum parser_status {
    SHOULD_OPEN_OUTPUT = 0,
    OUTPUT_OPENED = 1
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

template<class IStream, class OStream>
bool parse_template(IStream& input, OStream& output, parser_status& status) {
    using traits_type = typename IStream::traits_type;
    using char_type = typename IStream::char_type;
    
    auto ch = input.get();
    if (ch == traits_type::eof() || ch != '%') {
        if (!input.eof())
            input.unget();
        return false;
    }
    
    while ((ch = input.get()) != traits_type::eof()) {
        if (ch == '%') {
            auto ch_next = input.get();
            if (ch_next != traits_type::eof()) {
                if (ch_next == '>')
                    return true;
                else
                    input.unget();
            }
            else {
                return true;
            }
        }

        if (status == OUTPUT_OPENED) {
            output << "\";" << std::endl;
            status = SHOULD_OPEN_OUTPUT;
        }
        output.put(ch);
    }

    return true;
}


template<class IStream, class OStream>
void parse_file(IStream& input, OStream& out) {
    using traits_type = typename IStream::traits_type;
    using char_type = typename IStream::char_type;
    parser_status status = SHOULD_OPEN_OUTPUT;

    out << "template<class OutStream> OutStream& render(OutStream& result) {" << std::endl;
    until_eof_read_one(input,
            [&out,&status](IStream& input, char_type ch){
                if (ch != '<' || !parse_template(input, out, status)) {
                    if (status == SHOULD_OPEN_OUTPUT) {
                        status = OUTPUT_OPENED;
                        out << "result << \"";
                    }

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

    if (status == OUTPUT_OPENED)
        out << "\";" << std::endl << "return result;" << std::endl << "}" << std::endl;
}

int main(int argc, char** argv) {
    parse_file(std::cin, std::cout);
    return 0;
}
