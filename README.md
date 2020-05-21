# CppTemplater

**Work in Progress**

CppTemplater is a library and a tool to help create templates of files (mainly
for web development) that are compiled to C++ files.

# Who to compile?

```
cmake . && make
```

# How to use?

```
# cpptemplater -i my_project --namespace lzcoders::templater::test --class index --parent-class web_page_renderer <EOF
<html>
    <head><title><%!page_title()%></title></title>
    <body>
        <% for (int i = 0; i < 9; i++) { %>
            <h<%!i%>>This is an H<%!i%> header</h<%!i%>
        <% } %>
    </body>
</html>
<%e
/* This is written at the bottom of the file. No other output can be
   generated after this section.
*/
lzcoders::web::route::register("/", lzcoders::templater::test::index());
EOF
```

This will generate the following output:

```c++
#include <my_project>
namespace lzcoders::templater::test {
class index : public web_page_renderer {
public:
template<class OStream> OStream& render(OStream& result) {
result << "<html>\n    <head><title>" << (page_title()) << "</title></title>\n    <body>\n        ";
for (int i = 0; i < 9; i++) { result << "\n            <h" << (i) << ">This is an H" << (i) << " header</h" << (i) << "\n        ";
} result << "\n    </body>\n</html>\n";
return result;
}
};
}

/* This is written at the bottom of the file. No other output can be
   generated after this section.
*/
lzcoders::web::route::register("/", lzcoders::templater::test::index());
```
