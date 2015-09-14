// rapidjson/example/simpledom/simpledom.cpp`
#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>

using namespace rapidjson;

TEST_CASE("JSON Parsed", "[rapidjson]"){
    // 1. Parse a JSON string into DOM.
    const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
    Document d;
    d.Parse(json);

    // 2. Modify it by DOM.
    Value& s = d["stars"];
    s.SetInt(s.GetInt() + 1);
    REQUIRE(s.GetInt() == 11);

    // 3. Stringify the DOM
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);

    // Output {"project":"rapidjson","stars":11}
    const std::string expectedString = "{\"project\":\"rapidjson\",\"stars\":11}";
    const std::string testString = buffer.GetString();
    REQUIRE( testString == expectedString );
    // std::cout << buffer.GetString() << std::endl;
    // return 0;
}
