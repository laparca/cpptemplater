#ifndef CPP_TEMPLATER_HH__
#define CPP_TEMPLATER_HH__

#include <optional>
#include <vector>
#include <cctype>
namespace lzcoders::templater {

    struct templater_parameters {
        std::optional<std::string> result_namespace;
        std::string result_name = std::string("result");
        bool function_override = false;
        std::optional<std::string> class_name;
        std::optional<std::string> parent_class_name;
        std::string function_name = std::string("render");
        std::vector<std::string> includes;
    };

#include <detail/cpptemplater.hh>
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
                       out << params.result_name << " << \"";
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

#endif
